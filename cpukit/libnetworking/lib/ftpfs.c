/**
 * @file
 *
 * File Transfer Protocol file system (FTP client).
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * (c) Copyright 2002
 * Thomas Doerfler
 * IMD Ingenieurbuero fuer Microcomputertechnik
 * Herbststr. 8
 * 82178 Puchheim, Germany
 * <Thomas.Doerfler@imd-systems.de>
 *
 * This code has been created after closly inspecting "tftpdriver.c" from Eric
 * Norum.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <rtems.h>
#include <rtems/ftpfs.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#ifdef DEBUG
  #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
  #define DEBUG_PRINTF(...)
#endif

/**
 * Connection entry for each open file stream.
 */
typedef struct {
  off_t file_size;

  /**
   * Control connection socket.
   */
  int ctrl_socket;

  uint32_t client_address;

  /**
   * Data transfer socket.
   */
  int data_socket;

  /**
   * Current index into the reply buffer.
   */
  size_t reply_current;

  /**
   * End index of the reply buffer.
   */
  size_t reply_end;

  /**
   * Buffer for relpy data.
   */
  char reply_buffer [128];

  /**
   * End of file flag.
   */
  bool eof;

  bool write;

  /**
   * Indicates if we should do a SIZE command.
   *
   * The first call to the rtems_ftpfs_fstat() handler is issued by the path
   * evaluation to check for access permission.  For this case we avoid the
   * SIZE command.
   */
  bool do_size_command;

  ino_t ino;

  const char *user;

  const char *password;

  const char *hostname;

  const char *filename;

  char buffer [];
} rtems_ftpfs_entry;

/**
 * Mount entry for each file system instance.
 */
typedef struct {
  /**
   * Verbose mode enabled or disabled.
   */
  bool verbose;

  /**
   * Timeout value
   */
  struct timeval timeout;

  /**
   * Inode counter.
   */
  ino_t ino;
} rtems_ftpfs_mount_entry;

static const rtems_filesystem_operations_table rtems_ftpfs_ops;

static const rtems_filesystem_file_handlers_r rtems_ftpfs_handlers;

static const rtems_filesystem_file_handlers_r rtems_ftpfs_root_handlers;

static bool rtems_ftpfs_use_timeout(const struct timeval *to)
{
  return to->tv_sec != 0 || to->tv_usec != 0;
}

static int rtems_ftpfs_set_connection_timeout(
  int socket,
  const struct timeval *to
)
{
  if (rtems_ftpfs_use_timeout(to)) {
    int rv = 0;

    rv = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, to, sizeof(*to));
    if (rv != 0) {
      return EIO;
    }

    rv = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, to, sizeof(*to));
    if (rv != 0) {
      return EIO;
    }
  }

  return 0;
}

static rtems_status_code rtems_ftpfs_do_ioctl(
  const char *mount_point,
  ioctl_command_t req,
  ...
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int rv = 0;
  int fd = 0;
  va_list ap;

  if (mount_point == NULL) {
    mount_point = RTEMS_FTPFS_MOUNT_POINT_DEFAULT;
  }

  fd = open(mount_point, O_RDWR);
  if (fd < 0) {
    return RTEMS_INVALID_NAME;
  }

  va_start(ap, req);
  rv = ioctl(fd, req, va_arg(ap, void *));
  va_end(ap);
  if (rv != 0) {
    sc = RTEMS_INVALID_NUMBER;
  }

  rv = close(fd);
  if (rv != 0 && sc == RTEMS_SUCCESSFUL) {
    sc = RTEMS_IO_ERROR;
  }

  return sc;
}

rtems_status_code rtems_ftpfs_get_verbose(const char *mount_point, bool *verbose)
{
  return rtems_ftpfs_do_ioctl(
    mount_point,
    RTEMS_FTPFS_IOCTL_GET_VERBOSE,
    verbose
  );
}

rtems_status_code rtems_ftpfs_set_verbose(const char *mount_point, bool verbose)
{
  return rtems_ftpfs_do_ioctl(
    mount_point,
    RTEMS_FTPFS_IOCTL_SET_VERBOSE,
    &verbose
  );
}

rtems_status_code rtems_ftpfs_get_timeout(
  const char *mount_point,
  struct timeval *timeout
)
{
  return rtems_ftpfs_do_ioctl(
    mount_point,
    RTEMS_FTPFS_IOCTL_GET_TIMEOUT,
    timeout
  );
}

rtems_status_code rtems_ftpfs_set_timeout(
  const char *mount_point,
  const struct timeval *timeout
)
{
  return rtems_ftpfs_do_ioctl(
    mount_point,
    RTEMS_FTPFS_IOCTL_SET_TIMEOUT,
    timeout
  );
}

typedef void (*rtems_ftpfs_reply_parser)(
  const char * /* reply fragment */,
  size_t /* reply fragment length */,
  void * /* parser argument */
);

typedef enum {
  RTEMS_FTPFS_REPLY_START,
  RTEMS_FTPFS_REPLY_SINGLE_LINE,
  RTEMS_FTPFS_REPLY_DONE,
  RTEMS_FTPFS_REPLY_MULTI_LINE,
  RTEMS_FTPFS_REPLY_MULTI_LINE_START
} rtems_ftpfs_reply_state;

typedef enum {
  RTEMS_FTPFS_REPLY_ERROR = 0,
  RTEMS_FTPFS_REPLY_1 = '1',
  RTEMS_FTPFS_REPLY_2 = '2',
  RTEMS_FTPFS_REPLY_3 = '3',
  RTEMS_FTPFS_REPLY_4 = '4',
  RTEMS_FTPFS_REPLY_5 = '5'
} rtems_ftpfs_reply;

#define RTEMS_FTPFS_REPLY_SIZE 3

static bool rtems_ftpfs_is_reply_code_valid(unsigned char *reply)
{
  return isdigit(reply [0]) && isdigit(reply [1]) && isdigit(reply [2]);
}

static rtems_ftpfs_reply rtems_ftpfs_get_reply(
  rtems_ftpfs_entry *e,
  rtems_ftpfs_reply_parser parser,
  void *parser_arg,
  bool verbose
)
{
  rtems_ftpfs_reply_state state = RTEMS_FTPFS_REPLY_START;
  unsigned char reply_code [RTEMS_FTPFS_REPLY_SIZE] = { 'a', 'a', 'a' };
  size_t reply_code_index = 0;

  while (state != RTEMS_FTPFS_REPLY_DONE) {
    char *buf = NULL;
    size_t i = 0;
    size_t n = 0;

    /* Receive reply fragment from socket */
    if (e->reply_current == e->reply_end) {
      ssize_t rv = 0;

      e->reply_current = 0;
      e->reply_end = 0;

      rv = recv(
        e->ctrl_socket,
        &e->reply_buffer [0],
        sizeof(e->reply_buffer),
        0
      );

      if (rv > 0) {
        e->reply_end = (size_t) rv;
      } else {
        return RTEMS_FTPFS_REPLY_ERROR;
      }
    }

    buf = &e->reply_buffer [e->reply_current];
    n = e->reply_end - e->reply_current;

    /* Invoke parser if necessary */
    if (parser != NULL) {
      parser(buf, n, parser_arg);
    }

    /* Parse reply fragment */
    for (i = 0; i < n && state != RTEMS_FTPFS_REPLY_DONE; ++i) {
      char c = buf [i];

      switch (state) {
        case RTEMS_FTPFS_REPLY_START:
          if (reply_code_index < RTEMS_FTPFS_REPLY_SIZE) {
            reply_code [reply_code_index] = c;
            ++reply_code_index;
          } else if (rtems_ftpfs_is_reply_code_valid(reply_code)) {
            if (c == '-') {
              state = RTEMS_FTPFS_REPLY_MULTI_LINE;
            } else {
              state = RTEMS_FTPFS_REPLY_SINGLE_LINE;
            }
          } else {
            return RTEMS_FTPFS_REPLY_ERROR;
          }
          break;
        case RTEMS_FTPFS_REPLY_SINGLE_LINE:
          if (c == '\n') {
            state = RTEMS_FTPFS_REPLY_DONE;
          }
          break;
        case RTEMS_FTPFS_REPLY_MULTI_LINE:
          if (c == '\n') {
            state = RTEMS_FTPFS_REPLY_MULTI_LINE_START;
            reply_code_index = 0;
          }
          break;
        case RTEMS_FTPFS_REPLY_MULTI_LINE_START:
          if (reply_code_index < RTEMS_FTPFS_REPLY_SIZE) {
            if (reply_code [reply_code_index] == c) {
              ++reply_code_index;
            } else {
              state = RTEMS_FTPFS_REPLY_MULTI_LINE;
            }
          } else {
            if (c == ' ') {
              state = RTEMS_FTPFS_REPLY_SINGLE_LINE;
            } else {
              state = RTEMS_FTPFS_REPLY_MULTI_LINE;
            }
          }
          break;
        default:
          return RTEMS_FTPFS_REPLY_ERROR;
      }
    }

    /* Be verbose if necessary */
    if (verbose) {
      write(STDERR_FILENO, buf, i);
    }

    /* Update reply index */
    e->reply_current += i;
  }

  return reply_code [0];
}

static rtems_ftpfs_reply rtems_ftpfs_send_command_with_parser(
  rtems_ftpfs_entry *e,
  const char *cmd,
  const char *arg,
  rtems_ftpfs_reply_parser parser,
  void *parser_arg,
  bool verbose
)
{
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  size_t cmd_len = strlen(cmd);
  size_t arg_len = arg != NULL ? strlen(arg) : 0;
  size_t len = cmd_len + arg_len + 2;
  char *buf = malloc(len);

  if (buf != NULL) {
    ssize_t n = 0;
    char *buf_arg = buf + cmd_len;
    char *buf_eol = buf_arg + arg_len;

    memcpy(buf, cmd, cmd_len);
    memcpy(buf_arg, arg, arg_len);
    buf_eol [0] = '\r';
    buf_eol [1] = '\n';

    /* Send */
    n = send(e->ctrl_socket, buf, len, 0);
    if (n == (ssize_t) len) {
      if (verbose) {
        write(STDERR_FILENO, buf, len);
      }

      /* Reply */
      reply = rtems_ftpfs_get_reply(e, parser, parser_arg, verbose);
    }

    free(buf);
  }

  return reply;
}

static rtems_ftpfs_reply rtems_ftpfs_send_command(
  rtems_ftpfs_entry *e,
  const char *cmd,
  const char *arg,
  bool verbose
)
{
  return rtems_ftpfs_send_command_with_parser(
    e,
    cmd,
    arg,
    NULL,
    NULL,
    verbose
  );
}

typedef enum {
  STATE_USER_NAME,
  STATE_START_PASSWORD,
  STATE_START_HOST_NAME,
  STATE_START_HOST_NAME_OR_PATH,
  STATE_START_PATH,
  STATE_PASSWORD,
  STATE_HOST_NAME,
  STATE_DONE,
  STATE_INVALID
} split_state;

static int rtems_ftpfs_split_names (
  char *s,
  const char **user,
  const char **password,
  const char **hostname,
  const char **path
)
{
  split_state state = STATE_USER_NAME;
  size_t len = strlen(s);
  size_t i = 0;

  *user = s;

  for (i = 0; i < len; ++i) {
    char c = s [i];

    switch (state) {
      case STATE_USER_NAME:
        if (c == ':') {
          state = STATE_START_PASSWORD;
          s [i] = '\0';
        } else if (c == '@') {
          state = STATE_START_HOST_NAME;
          s [i] = '\0';
        } else if (c == '/') {
          state = STATE_START_HOST_NAME_OR_PATH;
          s [i] = '\0';
        }
        break;
      case STATE_START_PASSWORD:
        state = STATE_PASSWORD;
        *password = &s [i];
        --i;
        break;
      case STATE_START_HOST_NAME:
        state = STATE_HOST_NAME;
        *hostname = &s [i];
        --i;
        break;
      case STATE_START_HOST_NAME_OR_PATH:
        if (c == '@') {
          state = STATE_START_HOST_NAME;
        } else {
          state = STATE_DONE;
          *path = &s [i];
          goto done;
        }
        break;
      case STATE_START_PATH:
        state = STATE_DONE;
        *path = &s [i];
        goto done;
      case STATE_PASSWORD:
        if (c == '@') {
          state = STATE_START_HOST_NAME;
          s [i] = '\0';
        } else if (c == '/') {
          state = STATE_START_HOST_NAME_OR_PATH;
          s [i] = '\0';
        }
        break;
      case STATE_HOST_NAME:
        if (c == '/') {
          state = STATE_START_PATH;
          s [i] = '\0';
        }
        break;
      default:
        state = STATE_INVALID;
        goto done;
    }
  }

done:

  /* This is a special case with no username and password */
  if (*hostname == NULL) {
    *hostname = &s [0];
    *user = "anonymous";
    *password = *user;
  }

  /* If we have no password use the user name */
  if (*password == NULL) {
    *password = *user;
  }

  return state == STATE_DONE ? 0 : ENOENT;
}

static socklen_t rtems_ftpfs_create_address(
  struct sockaddr_in *sa,
  unsigned long address,
  unsigned short port
)
{
  memset(sa, 0, sizeof(*sa));

  sa->sin_family = AF_INET;
  sa->sin_addr.s_addr = address;
  sa->sin_port = port;
  sa->sin_len = sizeof(*sa);

  return sizeof(*sa);
}

static int rtems_ftpfs_close_data_connection(
  rtems_ftpfs_entry *e,
  bool verbose,
  bool error
)
{
  int eno = 0;

  /* Close data connection if necessary */
  if (e->data_socket >= 0) {
    int rv = close(e->data_socket);

    e->data_socket = -1;
    if (rv != 0) {
      eno = EIO;
    }

    /* For write connections we have to obtain the transfer reply  */
    if (e->write && !error) {
      rtems_ftpfs_reply reply =
        rtems_ftpfs_get_reply(e, NULL, NULL, verbose);

      if (reply != RTEMS_FTPFS_REPLY_2) {
        eno = EIO;
      }
    }
  }

  return eno;
}

static int rtems_ftpfs_open_ctrl_connection(
  rtems_ftpfs_entry *e,
  bool verbose,
  const struct timeval *timeout
)
{
  int rv = 0;
  int eno = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct in_addr address = { .s_addr = 0 };
  struct sockaddr_in sa;
  socklen_t size = 0;

  /* Create the socket for the control connection */
  e->ctrl_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (e->ctrl_socket < 0) {
    return ENOMEM;
  }

  /* Set up the server address from the hostname */
  if (inet_aton(e->hostname, &address) == 0) {
    /* Try to get the address by name */
    struct hostent *he = gethostbyname(e->hostname);

    if (he != NULL) {
      memcpy(&address, he->h_addr, sizeof(address));
    } else {
      return ENOENT;
    }
  }
  rtems_ftpfs_create_address(&sa, address.s_addr, htons(RTEMS_FTPFS_CTRL_PORT));
  DEBUG_PRINTF("server = %s\n", inet_ntoa(sa.sin_addr));

  /* Open control connection */
  rv = connect(
    e->ctrl_socket,
    (struct sockaddr *) &sa,
    sizeof(sa)
  );
  if (rv != 0) {
    return ENOENT;
  }

  /* Set control connection timeout */
  eno = rtems_ftpfs_set_connection_timeout(e->ctrl_socket, timeout);
  if (eno != 0) {
    return eno;
  }

  /* Get client address */
  size = rtems_ftpfs_create_address(&sa, INADDR_ANY, 0);
  rv = getsockname(
    e->ctrl_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (rv != 0) {
    return ENOMEM;
  }
  e->client_address = ntohl(sa.sin_addr.s_addr);
  DEBUG_PRINTF("client = %s\n", inet_ntoa(sa.sin_addr));

  /* Now we should get a welcome message from the server */
  reply = rtems_ftpfs_get_reply(e, NULL, NULL, verbose);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return ENOENT;
  }

  /* Send USER command */
  reply = rtems_ftpfs_send_command(e, "USER ", e->user, verbose);
  if (reply == RTEMS_FTPFS_REPLY_3) {
    /* Send PASS command */
    reply = rtems_ftpfs_send_command(e, "PASS ", e->password, verbose);
    if (reply != RTEMS_FTPFS_REPLY_2) {
      return EACCES;
    }

    /* TODO: Some server may require an account */
  } else if (reply != RTEMS_FTPFS_REPLY_2) {
    return EACCES;
  }

  /* Send TYPE command to set binary mode for all data transfers */
  reply = rtems_ftpfs_send_command(e, "TYPE I", NULL, verbose);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return EIO;
  }

  return 0;
}

static int rtems_ftpfs_open_data_connection_active(
  rtems_ftpfs_entry *e,
  const char *file_command,
  bool verbose,
  const struct timeval *timeout
)
{
  int rv = 0;
  int eno = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct sockaddr_in sa;
  socklen_t size = 0;
  int port_socket = -1;
  char port_command [] = "PORT 000,000,000,000,000,000";
  uint16_t data_port = 0;

  /* Create port socket to establish a data data connection */
  port_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (port_socket < 0) {
    eno = ENOMEM;
    goto cleanup;
  }

  /* Bind port socket */
  rtems_ftpfs_create_address(&sa, INADDR_ANY, 0);
  rv = bind(
    port_socket,
    (struct sockaddr *) &sa,
    sizeof(sa)
  );
  if (rv != 0) {
    eno = EBUSY;
    goto cleanup;
  }

  /* Get port number for data socket */
  size = rtems_ftpfs_create_address(&sa, INADDR_ANY, 0);
  rv = getsockname(
    port_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (rv != 0) {
    eno = ENOMEM;
    goto cleanup;
  }
  data_port = ntohs(sa.sin_port);

  /* Send PORT command to set data connection port for server */
  snprintf(
    port_command,
    sizeof(port_command),
    "PORT %lu,%lu,%lu,%lu,%lu,%lu",
    (e->client_address >> 24) & 0xffUL,
    (e->client_address >> 16) & 0xffUL,
    (e->client_address >> 8) & 0xffUL,
    (e->client_address >> 0) & 0xffUL,
    (data_port >> 8) & 0xffUL,
    (data_port >> 0) & 0xffUL
  );
  reply = rtems_ftpfs_send_command(e, port_command, NULL, verbose);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    eno = ENOTSUP;
    goto cleanup;
  }

  /* Listen on port socket for incoming data connections */
  rv = listen(port_socket, 1);
  if (rv != 0) {
    eno = EBUSY;
    goto cleanup;
  }

  /* Send RETR or STOR command with filename */
  reply = rtems_ftpfs_send_command(e, file_command, e->filename, verbose);
  if (reply != RTEMS_FTPFS_REPLY_1) {
    eno = EIO;
    goto cleanup;
  }

  /* Wait for connect on data connection if necessary */
  if (rtems_ftpfs_use_timeout(timeout)) {
    struct timeval to = *timeout;
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(port_socket, &fds);

    rv = select(port_socket + 1, &fds, NULL, NULL, &to);
    if (rv <= 0) {
      eno = EIO;
      goto cleanup;
    }
  }

  /* Accept data connection  */
  size = sizeof(sa);
  e->data_socket = accept(
    port_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (e->data_socket < 0) {
    eno = EIO;
    goto cleanup;
  }

cleanup:

  /* Close port socket if necessary */
  if (port_socket >= 0) {
    rv = close(port_socket);
    if (rv != 0) {
      eno = EIO;
    }
  }

  return eno;
}

typedef enum {
  RTEMS_FTPFS_PASV_START = 0,
  RTEMS_FTPFS_PASV_JUNK,
  RTEMS_FTPFS_PASV_DATA,
  RTEMS_FTPFS_PASV_DONE
} rtems_ftpfs_pasv_state;

typedef struct {
  rtems_ftpfs_pasv_state state;
  size_t index;
  uint8_t data [6];
} rtems_ftpfs_pasv_entry;

static void rtems_ftpfs_pasv_parser(
  const char* buf,
  size_t len,
  void *arg
)
{
  rtems_ftpfs_pasv_entry *pe = arg;
  size_t i = 0;

  for (i = 0; i < len; ++i) {
    int c = buf [i];

    switch (pe->state) {
      case RTEMS_FTPFS_PASV_START:
        if (!isdigit(c)) {
          pe->state = RTEMS_FTPFS_PASV_JUNK;
          pe->index = 0;
        }
        break;
      case RTEMS_FTPFS_PASV_JUNK:
        if (isdigit(c)) {
          pe->state = RTEMS_FTPFS_PASV_DATA;
          pe->data [pe->index] = (uint8_t) (c - '0');
        }
        break;
      case RTEMS_FTPFS_PASV_DATA:
        if (isdigit(c)) {
          pe->data [pe->index] =
            (uint8_t) (pe->data [pe->index] * 10 + c - '0');
        } else if (c == ',') {
          ++pe->index;
          if (pe->index < sizeof(pe->data)) {
            pe->data [pe->index] = 0;
          } else {
            pe->state = RTEMS_FTPFS_PASV_DONE;
          }
        } else {
          pe->state = RTEMS_FTPFS_PASV_DONE;
        }
        break;
      default:
        return;
    }
  }
}

static int rtems_ftpfs_open_data_connection_passive(
  rtems_ftpfs_entry *e,
  const char *file_command,
  bool verbose,
  const struct timeval *timeout
)
{
  int rv = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct sockaddr_in sa;
  uint32_t data_address = 0;
  uint16_t data_port = 0;
  rtems_ftpfs_pasv_entry pe;

  memset(&pe, 0, sizeof(pe));

  /* Send PASV command */
  reply = rtems_ftpfs_send_command_with_parser(
    e,
    "PASV",
    NULL,
    rtems_ftpfs_pasv_parser,
    &pe,
    verbose
  );
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return ENOTSUP;
  }
  data_address = ((uint32_t)(pe.data [0]) << 24)
    + ((uint32_t)(pe.data [1]) << 16)
    + ((uint32_t)(pe.data [2]) << 8)
    + ((uint32_t)(pe.data [3]));
  data_port = (uint16_t) ((pe.data [4] << 8) + pe.data [5]);
  rtems_ftpfs_create_address(&sa, htonl(data_address), htons(data_port));
  DEBUG_PRINTF(
    "server data = %s:%u\n",
    inet_ntoa(sa.sin_addr),
    (unsigned) ntohs(sa.sin_port)
  );

  /* Create data socket */
  e->data_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (e->data_socket < 0) {
    return ENOMEM;
  }

  /* Open data connection */
  rv = connect(
    e->data_socket,
    (struct sockaddr *) &sa,
    sizeof(sa)
  );
  if (rv != 0) {
    return EIO;
  }

  /* Send RETR or STOR command with filename */
  reply = rtems_ftpfs_send_command(e, file_command, e->filename, verbose);
  if (reply != RTEMS_FTPFS_REPLY_1) {
    return EIO;
  }

  return 0;
}

typedef enum {
  RTEMS_FTPFS_SIZE_START = 0,
  RTEMS_FTPFS_SIZE_SPACE,
  RTEMS_FTPFS_SIZE_NUMBER,
  RTEMS_FTPFS_SIZE_NL
} rtems_ftpfs_size_state;

typedef struct {
  rtems_ftpfs_size_state state;
  size_t index;
  off_t size;
} rtems_ftpfs_size_entry;

static void rtems_ftpfs_size_parser(
  const char* buf,
  size_t len,
  void *arg
)
{
  rtems_ftpfs_size_entry *se = arg;
  size_t i = 0;

  for (i = 0; se->size >= 0 && i < len; ++i, ++se->index) {
    int c = buf [i];

    switch (se->state) {
      case RTEMS_FTPFS_SIZE_START:
        if (se->index == 2) {
          se->state = RTEMS_FTPFS_SIZE_SPACE;
        }
        break;
      case RTEMS_FTPFS_SIZE_SPACE:
        if (c == ' ') {
          se->state = RTEMS_FTPFS_SIZE_NUMBER;
        } else {
          se->size = -1;
        }
        break;
      case RTEMS_FTPFS_SIZE_NUMBER:
        if (isdigit(c)) {
          se->size = 10 * se->size + c - '0';
        } else if (c == '\r') {
          se->state = RTEMS_FTPFS_SIZE_NL;
        } else {
          se->size = -1;
        }
        break;
      case RTEMS_FTPFS_SIZE_NL:
        if (c != '\n') {
          se->size = -1;
        }
        break;
      default:
        se->size = -1;
        break;
    }
  }
}

static void rtems_ftpfs_get_file_size(rtems_ftpfs_entry *e, bool verbose)
{
  if (e->file_size < 0) {
    if (e->write) {
      e->file_size = 0;
    } else {
      rtems_ftpfs_size_entry se;
      rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;

      memset(&se, 0, sizeof(se));

      reply = rtems_ftpfs_send_command_with_parser(
        e,
        "SIZE ",
        e->filename,
        rtems_ftpfs_size_parser,
        &se,
        verbose
      );
      if (reply == RTEMS_FTPFS_REPLY_2 && se.size >= 0) {
        e->file_size = se.size;
      } else {
        e->file_size = 0;
      }
    }
  }
}

static int rtems_ftpfs_open(
  rtems_libio_t *iop,
  const char *path,
  int oflag,
  mode_t mode
)
{
  int eno = 0;
  rtems_ftpfs_entry *e = iop->pathinfo.node_access;
  rtems_ftpfs_mount_entry *me = iop->pathinfo.mt_entry->fs_info;
  bool verbose = me->verbose;
  const struct timeval *timeout = &me->timeout;

  e->write = rtems_libio_iop_is_writeable(iop);

  /* Check for either read-only or write-only flags */
  if (
    rtems_libio_iop_is_writeable(iop)
      && rtems_libio_iop_is_readable(iop)
  ) {
    eno = ENOTSUP;
  }

  if (eno == 0) {
    rtems_ftpfs_get_file_size(e, verbose);
  }

  if (eno == 0) {
    const char *file_command = e->write ? "STOR " : "RETR ";

    /* Open passive data connection */
    eno = rtems_ftpfs_open_data_connection_passive(
      e,
      file_command,
      verbose,
      timeout
    );
    if (eno == ENOTSUP) {
      /* Open active data connection */
      eno = rtems_ftpfs_open_data_connection_active(
        e,
        file_command,
        verbose,
        timeout
      );
    }
  }

  /* Set data connection timeout */
  if (eno == 0) {
    eno = rtems_ftpfs_set_connection_timeout(e->data_socket, timeout);
  }

  if (eno == 0) {
    return 0;
  } else {
    rtems_ftpfs_close_data_connection(e, verbose, true);

    rtems_set_errno_and_return_minus_one(eno);
  }
}

static ssize_t rtems_ftpfs_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  rtems_ftpfs_entry *e = iop->pathinfo.node_access;
  const rtems_ftpfs_mount_entry *me = iop->pathinfo.mt_entry->fs_info;
  bool verbose = me->verbose;
  char *in = buffer;
  size_t todo = count;

  if (e->eof) {
    return 0;
  }

  while (todo > 0) {
    ssize_t rv = recv(e->data_socket, in, todo, 0);

    if (rv <= 0) {
      if (rv == 0) {
        rtems_ftpfs_reply reply =
          rtems_ftpfs_get_reply(e, NULL, NULL, verbose);

        if (reply == RTEMS_FTPFS_REPLY_2) {
          e->eof = true;
          break;
        }
      }

      rtems_set_errno_and_return_minus_one(EIO);
    }

    in += rv;
    todo -= (size_t) rv;
  }

  return (ssize_t) (count - todo);
}

static ssize_t rtems_ftpfs_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  rtems_ftpfs_entry *e = iop->pathinfo.node_access;
  const char *out = buffer;
  size_t todo = count;

  while (todo > 0) {
    ssize_t rv = send(e->data_socket, out, todo, 0);

    if (rv <= 0) {
      if (rv == 0) {
        break;
      } else {
        rtems_set_errno_and_return_minus_one(EIO);
      }
    }

    out += rv;
    todo -= (size_t) rv;

    e->file_size += rv;
  }

  return (ssize_t) (count - todo);
}

static int rtems_ftpfs_close(rtems_libio_t *iop)
{
  rtems_ftpfs_entry *e = iop->pathinfo.node_access;
  const rtems_ftpfs_mount_entry *me = iop->pathinfo.mt_entry->fs_info;
  int eno = rtems_ftpfs_close_data_connection(e, me->verbose, false);

  if (eno == 0) {
    return 0;
  } else {
    rtems_set_errno_and_return_minus_one(eno);
  }
}

/* Dummy version to let fopen(*,"w") work properly */
static int rtems_ftpfs_ftruncate(rtems_libio_t *iop, off_t count)
{
  return 0;
}

static void rtems_ftpfs_eval_path(
  rtems_filesystem_eval_path_context_t *self
)
{
  int eno = 0;

  rtems_filesystem_eval_path_eat_delimiter(self);

  if (rtems_filesystem_eval_path_has_path(self)) {
    const char *path = rtems_filesystem_eval_path_get_path(self);
    size_t pathlen = rtems_filesystem_eval_path_get_pathlen(self);
    rtems_ftpfs_entry *e = calloc(1, sizeof(*e) + pathlen + 1);

    rtems_filesystem_eval_path_clear_path(self);

    if (e != NULL) {
      memcpy(e->buffer, path, pathlen);

      eno = rtems_ftpfs_split_names(
          e->buffer,
          &e->user,
          &e->password,
          &e->hostname,
          &e->filename
      );

      DEBUG_PRINTF(
        "user = '%s', password = '%s', filename = '%s'\n",
        e->user,
        e->password,
        e->filename
      );

      if (eno == 0) {
        rtems_filesystem_location_info_t *currentloc =
          rtems_filesystem_eval_path_get_currentloc(self);
        rtems_ftpfs_mount_entry *me = currentloc->mt_entry->fs_info;

        rtems_libio_lock();
        ++me->ino;
        e->ino = me->ino;
        rtems_libio_unlock();

        e->file_size = -1;
        e->ctrl_socket = -1;

        eno = rtems_ftpfs_open_ctrl_connection(
          e,
          me->verbose,
          &me->timeout
        );
        if (eno == 0) {
          currentloc->node_access = e;
          currentloc->handlers = &rtems_ftpfs_handlers;
        }
      }

      if (eno != 0) {
        free(e);
      }
    } else {
      eno = ENOMEM;
    }
  }

  if (eno != 0) {
    rtems_filesystem_eval_path_error(self, eno);
  }
}

static void rtems_ftpfs_free_node(const rtems_filesystem_location_info_t *loc)
{
  rtems_ftpfs_entry *e = loc->node_access;

  /* The root node handler has no entry */
  if (e != NULL) {
    const rtems_ftpfs_mount_entry *me = loc->mt_entry->fs_info;

    /* Close control connection if necessary */
    if (e->ctrl_socket >= 0) {
      rtems_ftpfs_send_command(e, "QUIT", NULL, me->verbose);

      close(e->ctrl_socket);
    }

    free(e);
  }
}

int rtems_ftpfs_initialize(
  rtems_filesystem_mount_table_entry_t *e,
  const void                           *d
)
{
  rtems_ftpfs_mount_entry *me = calloc(1, sizeof(*me));

  /* Mount entry for FTP file system instance */
  e->fs_info = me;
  if (e->fs_info == NULL) {
    rtems_set_errno_and_return_minus_one(ENOMEM);
  }
  me->verbose = false;
  me->timeout.tv_sec = 0;
  me->timeout.tv_usec = 0;

  /* Set handler and oparations table */
  e->mt_fs_root->location.handlers = &rtems_ftpfs_root_handlers;
  e->ops = &rtems_ftpfs_ops;

  /* We maintain no real file system nodes, so there is no real root */
  e->mt_fs_root->location.node_access = NULL;

  return 0;
}

static void rtems_ftpfs_unmount_me(
  rtems_filesystem_mount_table_entry_t *e
)
{
  free(e->fs_info);
}

static int rtems_ftpfs_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *arg
)
{
  rtems_ftpfs_mount_entry *me = iop->pathinfo.mt_entry->fs_info;
  bool *verbose = arg;
  struct timeval *timeout = arg;

  if (arg == NULL) {
    rtems_set_errno_and_return_minus_one(EINVAL);
  }

  switch (command) {
    case RTEMS_FTPFS_IOCTL_GET_VERBOSE:
      *verbose = me->verbose;
      break;
    case RTEMS_FTPFS_IOCTL_SET_VERBOSE:
      me->verbose = *verbose;
      break;
    case RTEMS_FTPFS_IOCTL_GET_TIMEOUT:
      *timeout = me->timeout;
      break;
    case RTEMS_FTPFS_IOCTL_SET_TIMEOUT:
      me->timeout = *timeout;
      break;
    default:
      rtems_set_errno_and_return_minus_one(EINVAL);
  }

  return 0;
}

/*
 * The stat() support is intended only for the cp shell command.  Each request
 * will return that we have a regular file with read, write and execute
 * permissions for every one.  The node index uses a global counter to support
 * a remote to remote copy.  This is not a very sophisticated method.
 */
static int rtems_ftpfs_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *st
)
{
  int eno = 0;
  rtems_ftpfs_entry *e = loc->node_access;

  /* FIXME */
  st->st_ino = e->ino;
  st->st_dev = rtems_filesystem_make_dev_t(0xcc494cd6U, 0x1d970b4dU);

  st->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;

  if (e->do_size_command) {
    const rtems_ftpfs_mount_entry *me = loc->mt_entry->fs_info;

    rtems_ftpfs_get_file_size(e, me->verbose);
    st->st_size = e->file_size;
  } else {
    e->do_size_command = true;
  }

  if (eno == 0) {
    return 0;
  } else {
    rtems_set_errno_and_return_minus_one(eno);
  }
}

static void rtems_ftpfs_lock_or_unlock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  /* Do nothing */
}

static const rtems_filesystem_operations_table rtems_ftpfs_ops = {
  .lock_h = rtems_ftpfs_lock_or_unlock,
  .unlock_h = rtems_ftpfs_lock_or_unlock,
  .eval_path_h = rtems_ftpfs_eval_path,
  .link_h = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .mknod_h = rtems_filesystem_default_mknod,
  .rmnod_h = rtems_filesystem_default_rmnod,
  .fchmod_h = rtems_filesystem_default_fchmod,
  .chown_h = rtems_filesystem_default_chown,
  .clonenod_h = rtems_filesystem_default_clonenode,
  .freenod_h = rtems_ftpfs_free_node,
  .mount_h = rtems_filesystem_default_mount,
  .unmount_h = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_ftpfs_unmount_me,
  .utime_h = rtems_filesystem_default_utime,
  .symlink_h = rtems_filesystem_default_symlink,
  .readlink_h = rtems_filesystem_default_readlink,
  .rename_h = rtems_filesystem_default_rename,
  .statvfs_h = rtems_filesystem_default_statvfs
};

static const rtems_filesystem_file_handlers_r rtems_ftpfs_handlers = {
  .open_h = rtems_ftpfs_open,
  .close_h = rtems_ftpfs_close,
  .read_h = rtems_ftpfs_read,
  .write_h = rtems_ftpfs_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = rtems_ftpfs_fstat,
  .ftruncate_h = rtems_ftpfs_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static const rtems_filesystem_file_handlers_r rtems_ftpfs_root_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = rtems_filesystem_default_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_ftpfs_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = rtems_filesystem_default_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};
