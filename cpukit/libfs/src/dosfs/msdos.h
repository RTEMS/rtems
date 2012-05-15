/*
 *  msdos.h
 *
 *  The MSDOS filesystem constants/data structures/prototypes
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __DOSFS_MSDOS_H__
#define __DOSFS_MSDOS_H__

#include <rtems.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_file.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSDOS_NAME_NOT_FOUND_ERR  0x7D01

/*
 * This structure identifies the instance of the filesystem on the MSDOS
 * level.
 */
typedef struct msdos_fs_info_s
{
    fat_fs_info_t                     fat;                /*
                                                           * volume
                                                           * description
                                                           */
    const rtems_filesystem_file_handlers_r *directory_handlers; /*
                                                                 * a set of routines
                                                                 * that handles the
                                                                 * nodes of directory
                                                                 * type
                                                                 */
    const rtems_filesystem_file_handlers_r *file_handlers; /*
                                                            * a set of routines
                                                            * that handles the
                                                            * nodes of file
                                                            * type
                                                            */
    rtems_id                          vol_sema;           /*
                                                           * semaphore
                                                           * associated with
                                                           * the volume
                                                           */
    uint8_t                          *cl_buf;              /*
                                                            * just placeholder
                                                            * for anything
                                                            */
} msdos_fs_info_t;

/* a set of routines that handle the nodes which are directories */
extern const rtems_filesystem_file_handlers_r  msdos_dir_handlers;

/* a set of routines that handle the nodes which are files */
extern const rtems_filesystem_file_handlers_r  msdos_file_handlers;

/* Volume semaphore timeout value. This value can be changed to a number
 * of ticks to help debugging or if you need such a  */
#define MSDOS_VOLUME_SEMAPHORE_TIMEOUT    RTEMS_NO_TIMEOUT

/* Node types */
#define MSDOS_DIRECTORY     RTEMS_FILESYSTEM_DIRECTORY
#define MSDOS_REGULAR_FILE  RTEMS_FILESYSTEM_MEMORY_FILE
#define MSDOS_HARD_LINK     RTEMS_FILESYSTEM_HARD_LINK /* pseudo type */

typedef rtems_filesystem_node_types_t msdos_node_type_t;

/*
 * Macros for fetching fields from 32 bytes long FAT Directory Entry
 * Structure
 */
#define MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE    32 /* 32 bytes */

#define MSDOS_DIR_NAME(x)                 (char     *)((x) + 0)
#define MSDOS_DIR_ENTRY_TYPE(x)           (uint8_t  *)((x) + 0)
#define MSDOS_DIR_ATTR(x)                 (uint8_t  *)((x) + 11)
#define MSDOS_DIR_NT_RES(x)               (uint8_t  *)((x) + 12)
#define MSDOS_DIR_LFN_CHECKSUM(x)         (uint8_t  *)((x) + 13)
#define MSDOS_DIR_CRT_TIME_TENTH(x)       (uint8_t  *)((x) + 13)
#define MSDOS_DIR_CRT_TIME(x)             (uint16_t *)((x) + 14)
#define MSDOS_DIR_CRT_DATE(x)             (uint16_t *)((x) + 16)
#define MSDOS_DIR_LAST_ACCESS_DATE(x)     (uint16_t *)((x) + 18)
#define MSDOS_DIR_FIRST_CLUSTER_HI(x)     (uint16_t *)((x) + 20)
#define MSDOS_DIR_WRITE_TIME(x)           (uint16_t *)((x) + 22)
#define MSDOS_DIR_WRITE_DATE(x)           (uint16_t *)((x) + 24)
#define MSDOS_DIR_FIRST_CLUSTER_LOW(x)    (uint16_t *)((x) + 26)
#define MSDOS_DIR_FILE_SIZE(x)            (uint32_t *)((x) + 28)

#define MSDOS_EXTRACT_CLUSTER_NUM(p)                                         \
            (uint32_t)( (CF_LE_W(*MSDOS_DIR_FIRST_CLUSTER_LOW(p))) |       \
                          ((uint32_t)(CF_LE_W((*MSDOS_DIR_FIRST_CLUSTER_HI(p))))<<16) )

/*
 * Fields offset in 32 bytes long FAT Directory Entry
 * Structure
 */
#define MSDOS_FILE_SIZE_OFFSET            28
#define MSDOS_FILE_NAME_OFFSET             0
#define MSDOS_FIRST_CLUSTER_HI_OFFSET     20
#define MSDOS_FIRST_CLUSTER_LOW_OFFSET    26
#define MSDOS_FILE_WDATE_OFFSET           24
#define MSDOS_FILE_WTIME_OFFSET           22
#define MSDOS_FILE_ADATE_OFFSET           18

/*
 * Possible values of DIR_Attr field of 32 bytes long FAT Directory Entry
 * Structure
 */
#define MSDOS_ATTR_READ_ONLY    0x01
#define MSDOS_ATTR_HIDDEN       0x02
#define MSDOS_ATTR_SYSTEM       0x04
#define MSDOS_ATTR_VOLUME_ID    0x08
#define MSDOS_ATTR_DIRECTORY    0x10
#define MSDOS_ATTR_ARCHIVE      0x20
#define MSDOS_ATTR_LFN          (MSDOS_ATTR_READ_ONLY | \
                                 MSDOS_ATTR_HIDDEN | \
                                 MSDOS_ATTR_SYSTEM | \
                                 MSDOS_ATTR_VOLUME_ID)
#define MSDOS_ATTR_LFN_MASK     (MSDOS_ATTR_READ_ONLY | \
                                 MSDOS_ATTR_HIDDEN | \
                                 MSDOS_ATTR_SYSTEM | \
                                 MSDOS_ATTR_VOLUME_ID | \
                                 MSDOS_ATTR_DIRECTORY | \
                                 MSDOS_ATTR_ARCHIVE)

#define MSDOS_LAST_LONG_ENTRY         0x40
#define MSDOS_LAST_LONG_ENTRY_MASK    0x3F

#define MSDOS_DT_2SECONDS_MASK        0x1F    /* seconds divided by 2 */
#define MSDOS_DT_2SECONDS_SHIFT       0
#define MSDOS_DT_MINUTES_MASK         0x7E0   /* minutes */
#define MSDOS_DT_MINUTES_SHIFT        5
#define MSDOS_DT_HOURS_MASK           0xF800  /* hours */
#define MSDOS_DT_HOURS_SHIFT          11

#define MSDOS_DD_DAY_MASK             0x1F    /* day of month */
#define MSDOS_DD_DAY_SHIFT            0
#define MSDOS_DD_MONTH_MASK           0x1E0   /* month */
#define MSDOS_DD_MONTH_SHIFT          5
#define MSDOS_DD_YEAR_MASK            0xFE00  /* year - 1980 */
#define MSDOS_DD_YEAR_SHIFT           9


/*
 * Possible values of DIR_Name[0] field of 32 bytes long FAT Directory Entry
 * Structure
 */
#define MSDOS_THIS_DIR_ENTRY_EMPTY             0xE5
#define MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY    0x00

/*
 * Number of characters per directory entry for a long filename.
 */
#define MSDOS_LFN_LEN_PER_ENTRY (13)

/*
 *  Macros for names parsing and formatting
 */

#define MSDOS_SHORT_BASE_LEN             8  /* 8 characters */
#define MSDOS_SHORT_EXT_LEN              3  /* 3 characters */
#define MSDOS_SHORT_NAME_LEN             (MSDOS_SHORT_BASE_LEN+\
                                          MSDOS_SHORT_EXT_LEN) /* 11 chars */
#define MSDOS_NAME_MAX_LNF_LEN           (255)
#define MSDOS_NAME_MAX                   MSDOS_SHORT_NAME_LEN
#define MSDOS_NAME_MAX_WITH_DOT          (MSDOS_NAME_MAX + 1)
#define MSDOS_NAME_MAX_LFN_WITH_DOT      (260)


extern const char *const MSDOS_DOT_NAME;    /* ".", padded to MSDOS_NAME chars */
extern const char *const MSDOS_DOTDOT_NAME; /* ".", padded to MSDOS_NAME chars */

typedef enum msdos_name_types_e
{
    MSDOS_NAME_INVALID = 0, /* Unknown name type. Has invalid characters. */
    MSDOS_NAME_SHORT,       /* Name can be short. */
    MSDOS_NAME_LONG         /* Name is long; cannot be short. */
} msdos_name_type_t;

typedef enum msdos_token_types_e
{
    MSDOS_NO_MORE_PATH,
    MSDOS_CURRENT_DIR,
    MSDOS_UP_DIR,
    MSDOS_NAME,
    MSDOS_INVALID_TOKEN
} msdos_token_types_t;

/* Others macros */
#define MSDOS_RES_NT_VALUE     0x00
#define MSDOS_INIT_DIR_SIZE    0x00

/* "dot" entry offset in a directory */
#define MSDOS_DOT_DIR_ENTRY_OFFSET       0x00 /* first entry in directory */

/* "dotdot" entry offset in a directory */
#define MSDOS_DOTDOT_DIR_ENTRY_OFFSET    0x20 /* second entry in directory */

/* 'p' should be char* */
#define DOT_NODE_P(p)     ((char *)(p))
#define DOTDOT_NODE_P(p)  ((char *)((p) + MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE))

/* Size limits for files and directories */
#define MSDOS_MAX_DIR_LENGHT               0x200000   /* 2,097,152 bytes */
#define MSDOS_MAX_FILE_SIZE                0xFFFFFFFF /* 4 Gb */

/*
 * The number of 32 bytes long FAT Directory Entry
 * Structures per 512 bytes sector
 */
#define MSDOS_DPS512_NUM    16

/* Prototypes */
void msdos_shut_down(rtems_filesystem_mount_table_entry_t *temp_mt_entry);

void msdos_eval_path(rtems_filesystem_eval_path_context_t *ctx);

void msdos_free_node_info(const rtems_filesystem_location_info_t *pathloc);

rtems_filesystem_node_types_t msdos_node_type(
  const rtems_filesystem_location_info_t *loc
);

int msdos_mknod(
  const rtems_filesystem_location_info_t *loc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
);

int msdos_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
);

int msdos_rename(
  const rtems_filesystem_location_info_t *old_parent_loc,
  const rtems_filesystem_location_info_t *old_loc,
  const rtems_filesystem_location_info_t *new_parent_loc,
  const char *new_name,
  size_t new_namelen
);

void msdos_lock(const rtems_filesystem_mount_table_entry_t *mt_entry);

void msdos_unlock(const rtems_filesystem_mount_table_entry_t *mt_entry);

int msdos_initialize_support(
  rtems_filesystem_mount_table_entry_t    *temp_mt_entry,
  const rtems_filesystem_operations_table *op_table,
  const rtems_filesystem_file_handlers_r  *file_handlers,
  const rtems_filesystem_file_handlers_r  *directory_handlers
);

int msdos_file_close(rtems_libio_t *iop /* IN  */);

ssize_t msdos_file_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  size_t         count             /* IN  */
);

ssize_t msdos_file_write(
  rtems_libio_t *iop,             /* IN  */
  const void    *buffer,          /* IN  */
  size_t         count            /* IN  */
);

int msdos_file_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);

int
msdos_file_ftruncate(
  rtems_libio_t *iop,               /* IN  */
  off_t          length            /* IN  */
);

int msdos_file_sync(rtems_libio_t *iop);

int msdos_file_datasync(rtems_libio_t *iop);

ssize_t msdos_dir_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  size_t         count             /* IN  */
);

int msdos_dir_sync(rtems_libio_t *iop);

int msdos_dir_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);

int msdos_creat_node(const rtems_filesystem_location_info_t *parent_loc,
                     msdos_node_type_t                       type,
                     const char                             *name,
                     int                                     name_len,
                     mode_t                                  mode,
                     const fat_file_fd_t                    *link_fd);

/* Misc prototypes */

int msdos_find_name(
  rtems_filesystem_location_info_t *parent_loc,
  const char                       *name,
  int                               name_len
);

int msdos_get_name_node(
  const rtems_filesystem_location_info_t *parent_loc,
  bool                                    create_node,
  const char                             *name,
  int                                     name_len,
  msdos_name_type_t                       name_type,
  fat_dir_pos_t                          *dir_pos,
  char                                   *name_dir_entry
);

int msdos_dir_info_remove(rtems_filesystem_location_info_t *pathloc);

msdos_name_type_t msdos_long_to_short(const char *lfn, int lfn_len,
                                      char* sfn, int sfn_len);

int msdos_filename_unix2dos(const char *un, int unlen, char *dn);

void msdos_date_unix2dos(
  unsigned int tsp, uint16_t *ddp,
  uint16_t *dtp);

unsigned int msdos_date_dos2unix(unsigned int dd, unsigned int dt);

int msdos_set_first_cluster_num(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  fat_file_fd_t                        *fat_fd
);

int msdos_set_file_size(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  fat_file_fd_t                        *fat_fd
);

int msdos_set_first_char4file_name(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  fat_dir_pos_t                        *dir_pos,
  unsigned char                         first_char
);

int msdos_set_dir_wrt_time_and_date(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
);


int msdos_dir_is_empty(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  fat_file_fd_t                        *fat_fd,
  bool                                 *ret_val
);

int msdos_find_name_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    bool                                  create_node,
    const char                           *name,
    int                                   name_len,
    msdos_name_type_t                     name_type,
    fat_dir_pos_t                        *dir_pos,
    char                                 *name_dir_entry
);

int msdos_find_node_by_cluster_num_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    uint32_t                              cl4find,
    fat_dir_pos_t                        *dir_pos,
    char                                 *dir_entry
);

int msdos_get_dotdot_dir_info_cluster_num_and_offset(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    uint32_t                              cln,
    fat_dir_pos_t                        *dir_pos,
    char                                 *dir_entry
);

int msdos_sync_unprotected(msdos_fs_info_t *fs_info);

int msdos_sync(rtems_libio_t *iop);

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_MSDOS_H__ */
