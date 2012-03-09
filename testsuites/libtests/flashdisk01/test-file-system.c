/*
 * Copyright (c) 2010-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include "test-file-system.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/endian.h>

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

#define BUFFER_SIZE (32 * 1024)

#define HEADER_SIZE 8

/**
 * @brief Test states.
 *
 * digraph {
 *   INIT -> MOUNT;
 *   MOUNT -> INIT_ROOT;
 *   MOUNT -> FORMAT;
 *   INIT_ROOT -> CHOOSE_DIR_ACTION
 *   INIT_ROOT -> ERROR;
 *   CHOOSE_DIR_ACTION -> DIR_OPEN;
 *   CHOOSE_DIR_ACTION -> DIR_CLOSE;
 *   CHOOSE_DIR_ACTION -> DIR_CREATE;
 *   CHOOSE_DIR_ACTION -> DIR_DELETE;
 *   CHOOSE_DIR_ACTION -> DIR_SLEEP;
 *   DIR_OPEN -> CHOOSE_DIR_ACTION;
 *   DIR_OPEN -> CHOOSE_FILE_ACTION;
 *   DIR_OPEN -> ERROR;
 *   DIR_CLOSE -> CHOOSE_DIR_ACTION;
 *   DIR_CLOSE -> ERROR;
 *   DIR_CREATE -> CHOOSE_DIR_ACTION;
 *   DIR_CREATE -> DIR_DELETE;
 *   DIR_CREATE -> ERROR;
 *   DIR_DELETE -> CHOOSE_DIR_ACTION;
 *   DIR_DELETE -> ERROR;
 *   DIR_SLEEP -> CHOOSE_DIR_ACTION;
 *   CHOOSE_FILE_ACTION -> FILE_CLOSE;
 *   CHOOSE_FILE_ACTION -> FILE_APPEND;
 *   CHOOSE_FILE_ACTION -> FILE_READ;
 *   CHOOSE_FILE_ACTION -> FILE_SLEEP;
 *   FILE_CLOSE -> CHOOSE_DIR_ACTION;
 *   FILE_CLOSE -> ERROR;
 *   FILE_APPEND -> CHOOSE_FILE_ACTION;
 *   FILE_APPEND -> FILE_CLOSE_AND_UNLINK;
 *   FILE_APPEND -> ERROR;
 *   FILE_READ -> CHOOSE_FILE_ACTION;
 *   FILE_READ -> ERROR;
 *   FILE_SLEEP -> CHOOSE_FILE_ACTION;
 *   FILE_CLOSE_AND_UNLINK -> CHOOSE_DIR_ACTION;
 *   FILE_CLOSE_AND_UNLINK -> ERROR;
 *   ERROR -> FORMAT;
 *   FORMAT -> MOUNT;
 *   FORMAT -> FINISH;
 * }
 */
typedef enum {
  CHOOSE_DIR_ACTION,
  CHOOSE_FILE_ACTION,
  DIR_CLOSE,
  DIR_CREATE,
  DIR_DELETE,
  DIR_OPEN,
  DIR_SLEEP,
  ERROR,
  FILE_APPEND,
  FILE_READ,
  FILE_CLOSE,
  FILE_SLEEP,
  FILE_CLOSE_AND_UNLINK,
  FINISH,
  FORMAT,
  INIT,
  INIT_ROOT,
  MOUNT
} test_state;

typedef struct {
  DIR *dir;
  unsigned level;
  unsigned content_count;
  int fd;
  int eno;
  uint8_t buf [BUFFER_SIZE];
  char file_path [MAXPATHLEN];
} fs_state;

static test_state do_format(
  unsigned index,
  fs_state *fs,
  const char *disk_path,
  const test_file_system_handler *handler,
  void *handler_arg
)
{
  int rv = 0;

  printf("[%02u]: format: %s\n", index, disk_path);

  rv = (*handler->format)(disk_path, handler_arg);
  if (rv == 0) {
    return MOUNT;
  } else {
    fs->eno = errno;

    return FINISH;
  }
}

static uint32_t simple_random(uint32_t v)
{
	v *= 1664525;
	v += 1013904223;

	return v;
}

static unsigned get_bucket_with_random(unsigned count, long random)
{
  long unsigned unit = (1U << 31) / count;
  long unsigned bucket = (long unsigned) random / unit;

  if (bucket != count) {
    return bucket;
  } else {
    return bucket - 1;
  }
}

static unsigned get_bucket(unsigned count)
{
  return get_bucket_with_random(count, lrand48());
}

static test_state do_choose_dir_action(void)
{
  switch (get_bucket(8)) {
    case 0:
    case 1:
      return DIR_CLOSE;
    case 2:
    case 3:
      return DIR_CREATE;
    case 4:
    case 5:
      return DIR_OPEN;
    case 6:
      return DIR_DELETE;
    case 7:
      return DIR_SLEEP;
    default:
      assert(false);
      break;
  }
}

static test_state do_choose_file_action(void)
{
  switch (get_bucket(4)) {
    case 0:
      return FILE_CLOSE;
    case 1:
      return FILE_SLEEP;
    case 2:
      return FILE_APPEND;
    case 3:
      return FILE_READ;
    default:
      assert(false);
      break;
  }
}

static bool is_normal_entry(const char *entry_name)
{
  static const char *black_list [] = {
    ".",
    "..",
    "lost+found"
  };
  size_t n = sizeof(black_list) / sizeof(black_list [0]);
  size_t i = 0;
  bool ok = true;

  while (ok && i < n) {
    ok = ok && strcmp(entry_name, black_list [i]) != 0;
    ++i;
  }

  return ok;
}

static bool open_dir(fs_state *fs, const char *dir_path)
{
  int rv = 0;
  bool change_dir = true;

  if (dir_path == NULL) {
    if (fs->level > 1) {
      rv = chdir("..");
      if (rv != 0) {
        fs->eno = errno;

        return false;
      }

      --fs->level;
    } else {
      return true;
    }
    dir_path = ".";
    change_dir = false;
  }

  if (fs->dir != NULL) {
    rv = closedir(fs->dir);
    if (rv != 0) {
      fs->eno = errno;

      return false;
    }
  }

  fs->content_count = 0;
  fs->dir = opendir(dir_path);

  if (fs->dir != NULL) {
    struct dirent *de = NULL;

    rewinddir(fs->dir);
    while ((de = readdir(fs->dir)) != NULL) {
      if (is_normal_entry(de->d_name)) {
        ++fs->content_count;
      }
    }
  } else {
    fs->eno = errno;

    return false;
  }

  if (change_dir) {
    rv = chdir(dir_path);
    if (rv != 0) {
      fs->eno = errno;

      return false;
    }

    ++fs->level;
  }

  return true;
}

static char *get_dir_entry(fs_state *fs, bool *is_dir)
{
  int rv = 0;
  char *entry_name = NULL;

  if (fs->content_count > 0) {
    struct dirent *de = NULL;
    unsigned bucket = get_bucket(fs->content_count);
    unsigned i = 0;

    rewinddir(fs->dir);
    while ((de = readdir(fs->dir)) != NULL) {
      if (is_normal_entry(de->d_name)) {
        if (i != bucket) {
          ++i;
        } else {
          break;
        }
      }
    }

    if (de != NULL) {
      struct stat st;

      rv = stat(de->d_name, &st);
      if (rv == 0) {
        *is_dir = S_ISDIR(st.st_mode);

        entry_name = strdup(de->d_name);
      }
    }
  }

  return entry_name;
}


static test_state do_init_root(unsigned index, fs_state *fs, const char *mount_path)
{
  printf("[%02u]: init root: %s\n", index, mount_path);

  if (open_dir(fs, mount_path)) {
    return CHOOSE_DIR_ACTION;
  } else {
    return ERROR;
  }
}

static test_state do_dir_close(unsigned index, fs_state *fs)
{
  if (fs->level > 1) {
    printf("[%02u]: close dir\n", index);

    if (open_dir(fs, NULL)) {
      return CHOOSE_DIR_ACTION;
    } else {
      return ERROR;
    }
  } else {
    return CHOOSE_DIR_ACTION;
  }
}

static int open_file(fs_state *fs, const char *path, int flags, mode_t mode)
{
  size_t n = strlcpy(fs->file_path, path, sizeof(fs->file_path));
  assert(n < sizeof(fs->file_path));

  return open(fs->file_path, flags, mode);
}

static test_state do_dir_create(unsigned index, fs_state *fs)
{
  int rv = 0;
  test_state state = ERROR;
  long number = lrand48();
  char name [64];

  snprintf(name, sizeof(name), "%li", number);

  if ((number % 2) == 0) {
    printf("[%02u]: create file: %s\n", index, name);

    rv = open_file(fs, name, O_RDONLY | O_CREAT, 0777);

    if (rv >= 0) {
      rv = close(rv);

      if (rv == 0) {
        state = CHOOSE_DIR_ACTION;
      }
    } else if (errno == ENOSPC) {
      state = DIR_DELETE;
    } else {
      fs->eno = errno;
    }
  } else {
    printf("[%02u]: create dir: %s\n", index, name);

    rv = mkdir(name, 0777);

    if (rv == 0) {
      ++fs->content_count;

      state = CHOOSE_DIR_ACTION;
    } else if (errno == EEXIST) {
      state = CHOOSE_DIR_ACTION;
    } else if (errno == ENOSPC) {
      state = DIR_DELETE;
    } else {
      fs->eno = errno;
    }
  }

  return state;
}

static test_state remove_node(unsigned index, fs_state *fs, const char *path, bool is_dir)
{
  test_state state = ERROR;
  int rv = 0;

  if (is_dir) {
    printf("[%02u]: remove dir: %s\n", index, path);

    rv = rmdir(path);
  } else {
    printf("[%02u]: remove file: %s\n", index, path);

    rv = unlink(path);
  }

  if (rv == 0) {
    --fs->content_count;

    state = CHOOSE_DIR_ACTION;
  } else if (errno == ENOTEMPTY) {
    state = CHOOSE_DIR_ACTION;
  } else {
    fs->eno = errno;
  }

  return state;
}

static test_state do_dir_delete(unsigned index, fs_state *fs)
{
  test_state state = ERROR;

  if (fs->content_count > 0) {
    bool is_dir = false;
    char *dir_entry_path = get_dir_entry(fs, &is_dir);

    if (dir_entry_path != NULL) {
      state = remove_node(index, fs, dir_entry_path, is_dir);
      free(dir_entry_path);
    }
  } else {
    state = CHOOSE_DIR_ACTION;
  }

  return state;
}

static test_state do_dir_open(unsigned index, fs_state *fs)
{
  test_state state = ERROR;

  if (fs->content_count > 0) {
    bool is_dir = false;
    char *dir_entry_path = get_dir_entry(fs, &is_dir);

    if (dir_entry_path != NULL) {
      if (is_dir) {
        printf("[%02u]: open dir: %s\n", index, dir_entry_path);

        if (open_dir(fs, dir_entry_path)) {
          state = CHOOSE_DIR_ACTION;
        }
      } else {
        printf("[%02u]: open file: %s\n", index, dir_entry_path);

        fs->fd = open_file(fs, dir_entry_path, O_RDWR, 0);

        if (fs->fd >= 0) {
          state = CHOOSE_FILE_ACTION;
        } else {
          fs->eno = errno;
        }
      }

      free(dir_entry_path);
    }
  } else {
    state = CHOOSE_DIR_ACTION;
  }

  return state;
}

#if BYTE_ORDER == LITTLE_ENDIAN
  #define VALUE_SHIFT_INIT 0
  #define VALUE_SHIFT_INC 8
#else
  #define VALUE_SHIFT_INIT 24
  #define VALUE_SHIFT_INC -8
#endif

static test_state do_file_read(unsigned index, fs_state *fs)
{
  test_state state = ERROR;
  int fd = fs->fd;
  off_t pos = lseek(fd, 0, SEEK_SET);
  struct stat st;
  int rv = fstat(fd, &st);
  off_t file_size = st.st_size;

  printf("[%02u]: read from file\n", index);

  if (pos == 0 && rv == 0) {
    if (file_size >= HEADER_SIZE) {
      size_t remaining = (size_t) file_size;
      size_t header = HEADER_SIZE;
      size_t data = 0;
      uint32_t header_buf [2];
      uint8_t *header_pos = (uint8_t *) header_buf;
      uint32_t value = 0;
      unsigned value_shift = VALUE_SHIFT_INIT;

      while (remaining > 0) {
        size_t buf_size = sizeof(fs->buf);
        size_t in = remaining < buf_size ? remaining : buf_size;
        ssize_t in_actual = 0;
        uint8_t *buf = fs->buf;

        in_actual = read(fd, buf, in);
        if (in_actual > 0) {
          while (in_actual > 0) {
            if (header > 0) {
              size_t copy = header <= (size_t) in_actual ?
                header : (size_t) in_actual;

              memcpy(header_pos, buf, copy);

              in_actual -= (ssize_t) copy;
              remaining -= copy;
              buf += copy;
              header -= copy;
              header_pos += copy;

              if (header == 0) {
                data = header_buf [0];
                value = header_buf [1];
                value_shift = VALUE_SHIFT_INIT;
                value = simple_random(value);
              }
            }

            if (data > 0) {
              size_t compare = data <= (size_t) in_actual ?
                data : (size_t) in_actual;
              size_t c = 0;

              for (c = 0; c < compare; ++c) {
                assert(buf [c] == (uint8_t) (value >> value_shift));
                value_shift = (value_shift + VALUE_SHIFT_INC) % 32;
                if (value_shift == VALUE_SHIFT_INIT) {
                  value = simple_random(value);
                }
              }

              in_actual -= (ssize_t) compare;
              remaining -= compare;
              buf += compare;
              data -= compare;

              if (data == 0) {
                header = HEADER_SIZE;
                header_pos = (uint8_t *) header_buf;
              }
            }
          }
        } else if (in_actual == 0) {
          /* This is either a severe bug or a damaged file system */
          printf("[%02u]: error: invalid file size\n", index);
          remaining = 0;
        } else {
          fs->eno = errno;
          remaining = 0;
        }
      }

      if (remaining == 0) {
        state = CHOOSE_FILE_ACTION;
      }
    } else if (file_size == 0) {
      state = CHOOSE_FILE_ACTION;
    } else {
      printf("[%02u]: error: unexpected file size\n", index);
    }
  } else {
    fs->eno = errno;
  }

  return state;
}

static test_state do_file_append(unsigned index, fs_state *fs)
{
  test_state state = ERROR;
  int fd = fs->fd;
  off_t pos = lseek(fd, 0, SEEK_END);

  printf("[%02u]: append to file\n", index);

  if (pos != (off_t) -1) {
    size_t buf_size = sizeof(fs->buf);
    long random = lrand48();
    size_t out = get_bucket_with_random(buf_size, random) + 1;
    ssize_t out_actual = 0;
    uint8_t *buf = fs->buf;
    uint32_t value = (uint32_t) random;
    uint32_t *words = (uint32_t *) &buf [0];
    size_t word_count = 0;
    size_t w = 0;

    /* Must be big enough for the header */
    out = out >= HEADER_SIZE ? out : HEADER_SIZE;

    /*
     * In case out is not an integral multiple of four we will write a bit to
     * much.  This does not hurt since the buffer is big enough.
     */
    word_count = (out + 3) / 4;

    /* The first word will contain the byte count with random data */
    words [0] = out - HEADER_SIZE;

    for (w = 1; w < word_count; ++w) {
      words [w] = value;
      value = simple_random(value);
    }

    out_actual = write(fd, buf, out);

    if (out_actual == (ssize_t) out) {
      state = CHOOSE_FILE_ACTION;
    } else if (out_actual >= 0 || errno == ENOSPC) {
      state = FILE_CLOSE_AND_UNLINK;
    } else {
      fs->eno = errno;
    }
  } else {
    fs->eno = errno;
  }

  return state;
}

static test_state do_file_close(unsigned index, fs_state *fs)
{
  int rv = 0;
  test_state state = ERROR;

  printf("[%02u]: close file\n", index);

  rv = close(fs->fd);
  fs->fd = -1;

  if (rv == 0) {
    state = CHOOSE_DIR_ACTION;
  } else {
    fs->eno = errno;
  }

  return state;
}

static test_state do_file_close_and_unlink(unsigned index, fs_state *fs)
{
  test_state state = do_file_close(index, fs);

  if (state != ERROR) {
    state = remove_node(index, fs, fs->file_path, false);
  }

  return state;
}

static test_state do_sleep(unsigned index, test_state state)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interval ms = ((rtems_interval) get_bucket(10) + 1) * 100;
  rtems_interval interval = ms / rtems_configuration_get_milliseconds_per_tick();

  printf("[%02u]: sleep: %" PRIu32 " ms\n", index, ms);

  sc = rtems_task_wake_after(interval);
  ASSERT_SC(sc);

  return state;
}

static test_state do_mount(
  unsigned index,
  fs_state *fs,
  const char *disk_path,
  const char *mount_path,
  const test_file_system_handler *handler,
  void *handler_arg
)
{
  int rv = 0;

  printf("[%02u]: mount: %s -> %s\n", index, disk_path, mount_path);

  rv = (*handler->mount)(disk_path, mount_path, handler_arg);
  if (rv == 0) {
    return INIT_ROOT;
  } else {
    fs->eno = errno;

    return FORMAT;
  }
}

static test_state do_error(unsigned index, fs_state *fs, const char *mount_path)
{
  int rv = 0;

  if (fs->eno > 0) {
    printf("[%02u]: error: %s\n", index, strerror(fs->eno));
  } else {
    printf("[%02u]: error\n", index);
  }
  fs->eno = 0;

  if (fs->fd >= 0) {
    close(fs->fd);
    fs->fd = -1;
  }

  if (fs->dir != NULL) {
    closedir(fs->dir);
    fs->dir = NULL;
  }

  chdir("/");
  fs->level = 0;

  rv = unmount(mount_path);
  if (rv == 0) {
    return FORMAT;
  } else {
    return FINISH;
  }
}

void test_file_system_with_handler(
  unsigned index,
  const char *disk_path,
  const char *mount_path,
  const test_file_system_handler *handler,
  void *handler_arg
)
{
  int counter = 0;
  test_state state = INIT;
  fs_state *fs = calloc(1, sizeof(*fs));

  if (fs == NULL) {
    printf("[%02u]: not enough memory\n", index);
    return;
  }

  fs->fd = -1;

  printf("[%02u]: start\n", index);

  while (state != FINISH && counter < 600) {
    switch (state) {
      case CHOOSE_DIR_ACTION:
        state = do_choose_dir_action();
        break;
      case CHOOSE_FILE_ACTION:
        state = do_choose_file_action();
        break;
      case DIR_CLOSE:
        state = do_dir_close(index, fs);
        break;
      case DIR_CREATE:
        state = do_dir_create(index, fs);
        break;
      case DIR_DELETE:
        state = do_dir_delete(index, fs);
        break;
      case DIR_OPEN:
        state = do_dir_open(index, fs);
        break;
      case DIR_SLEEP:
        state = do_sleep(index, CHOOSE_DIR_ACTION);
        break;
      case ERROR:
        state = do_error(index, fs, mount_path);
        break;
      case FILE_APPEND:
        state = do_file_append(index, fs);
        break;
      case FILE_READ:
        state = do_file_read(index, fs);
        break;
      case FILE_CLOSE:
        state = do_file_close(index, fs);
        break;
      case FILE_CLOSE_AND_UNLINK:
        state = do_file_close_and_unlink(index, fs);
        break;
      case FILE_SLEEP:
        state = do_sleep(index, CHOOSE_FILE_ACTION);
        break;
      case FORMAT:
        state = do_format(index, fs, disk_path, handler, handler_arg);
        break;
      case INIT:
        state = MOUNT;
        break;
      case INIT_ROOT:
        state = do_init_root(index, fs, mount_path);
        break;
      case MOUNT:
        state = do_mount(
          index,
         fs,
          disk_path,
          mount_path,
          handler,
          handler_arg
        );
        break;
      default:
        assert(false);
        break;
    }

    ++counter;
  }

  printf("[%02u]: finish\n", index);

  free(fs);
}
