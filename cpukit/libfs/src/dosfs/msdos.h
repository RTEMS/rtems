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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  @(#) $Id$
 */
#ifndef __DOSFS_MSDOS_H__
#define __DOSFS_MSDOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_file.h"
 
#ifndef RC_OK
#define RC_OK 0x00000000
#endif

#define MSDOS_NAME_NOT_FOUND_ERR  0xDD000001 

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
    rtems_filesystem_file_handlers_r *directory_handlers; /* 
                                                           * a set of routines
                                                           * that handles the 
                                                           * nodes of directory 
                                                           * type
                                                           */
    rtems_filesystem_file_handlers_r *file_handlers;      /* 
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
    unsigned8                        *cl_buf;              /* 
                                                            * just placeholder
                                                            * for anything 
                                                            */ 
} msdos_fs_info_t;

/* a set of routines that handle the nodes which are directories */
extern rtems_filesystem_file_handlers_r  msdos_dir_handlers;

/* a set of routines that handle the nodes which are files */
extern rtems_filesystem_file_handlers_r  msdos_file_handlers;

/* Volume semaphore timeout value */
#define MSDOS_VOLUME_SEMAPHORE_TIMEOUT    100

/* Node types */
#define MSDOS_DIRECTORY     RTEMS_FILESYSTEM_DIRECTORY
#define MSDOS_REGULAR_FILE  RTEMS_FILESYSTEM_MEMORY_FILE
   
typedef rtems_filesystem_node_types_t msdos_node_type_t;

/* 
 * Macros for fetching fields from 32 bytes long FAT Directory Entry 
 * Structure (see M$ White Paper)
 */
#define MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE    32 /* 32 bytes */

#define MSDOS_DIR_NAME(x)                 (unsigned8 *)((x) + 0)
#define MSDOS_DIR_ATTR(x)                 (unsigned8 *)((x) + 11)
#define MSDOS_DIR_NT_RES(x)               (unsigned8 *)((x) + 12)
#define MSDOS_DIR_CRT_TIME_TENTH(x)       (unsigned8 *)((x) + 13)
#define MSDOS_DIR_CRT_TIME(x)             (unsigned16 *)((x) + 14)
#define MSDOS_DIR_CRT_DATE(x)             (unsigned16 *)((x) + 16)
#define MSDOS_DIR_LAST_ACCESS_DATE(x)     (unsigned16 *)((x) + 18)
#define MSDOS_DIR_FIRST_CLUSTER_HI(x)     (unsigned16 *)((x) + 20)
#define MSDOS_DIR_WRITE_TIME(x)           (unsigned16 *)((x) + 22)
#define MSDOS_DIR_WRITE_DATE(x)           (unsigned16 *)((x) + 24)
#define MSDOS_DIR_FIRST_CLUSTER_LOW(x)    (unsigned16 *)((x) + 26)
#define MSDOS_DIR_FILE_SIZE(x)            (unsigned32 *)((x) + 28)

#define MSDOS_EXTRACT_CLUSTER_NUM(p)                                         \
            (unsigned32)( (CF_LE_W(*MSDOS_DIR_FIRST_CLUSTER_LOW(p))) |       \
                          ((unsigned32)(CF_LE_W((*MSDOS_DIR_FIRST_CLUSTER_HI(p))))<<16) )

/*
 * Fields offset in 32 bytes long FAT Directory Entry
 * Structure (see M$ White Paper)
 */
#define MSDOS_FILE_SIZE_OFFSET            28
#define MSDOS_FILE_NAME_OFFSET             0
#define MSDOS_FIRST_CLUSTER_HI_OFFSET     20
#define MSDOS_FIRST_CLUSTER_LOW_OFFSET    26
#define MSDOS_FILE_WDATE_OFFSET           24
#define MSDOS_FILE_WTIME_OFFSET           22

/*
 * Possible values of DIR_Attr field of 32 bytes long FAT Directory Entry
 * Structure (see M$ White Paper)
 */
#define MSDOS_ATTR_READ_ONLY    0x01
#define MSDOS_ATTR_HIDDEN       0x02
#define MSDOS_ATTR_SYSTEM       0x04
#define MSDOS_ATTR_VOLUME_ID    0x08
#define MSDOS_ATTR_DIRECTORY    0x10
#define MSDOS_ATTR_ARCHIVE      0x20

/*
 * Possible values of DIR_Name[0] field of 32 bytes long FAT Directory Entry
 * Structure (see M$ White Paper)
 */
#define MSDOS_THIS_DIR_ENTRY_EMPTY             0xE5
#define MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY    0x00


/*
 *  Macros for names parsing and formatting
 */
#define msdos_is_valid_name_char(_ch)    (1)
#define msdos_is_separator(_ch)          rtems_filesystem_is_separator(_ch)

#define MSDOS_SHORT_NAME_LEN             11 /* 11 characters */
#define MSDOS_NAME_MAX                   MSDOS_SHORT_NAME_LEN
#define MSDOS_NAME_MAX_WITH_DOT          (MSDOS_NAME_MAX + 1)

#define MSDOS_DOT_NAME     ".          " /* ".", padded to MSDOS_NAME chars */
#define MSDOS_DOTDOT_NAME  "..         " /* "..", padded to MSDOS_NAME chars */

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

/* Size limits for files and directories (see M$ White Paper) */
#define MSDOS_MAX_DIR_LENGHT               0x200000   /* 2,097,152 bytes */
#define MSDOS_MAX_FILE_SIZE                0xFFFFFFFF /* 4 Gb */

/* 
 * The number of 32 bytes long FAT Directory Entry
 * Structures per 512 bytes sector 
 */ 
#define MSDOS_DPS512_NUM    16

/* Prototypes */
int 
msdos_initialize(rtems_filesystem_mount_table_entry_t *temp_mt_entry);

int 
msdos_shut_down(rtems_filesystem_mount_table_entry_t *temp_mt_entry);

int 
msdos_eval_path(const char                       *pathname, /* IN */
                int                               flags,    /* IN */
                rtems_filesystem_location_info_t *pathloc   /* IN/OUT */);

int 
msdos_eval4make(const char                       *path,     /* IN */
                rtems_filesystem_location_info_t *pathloc,  /* IN/OUT */
                const char                       **name     /* OUT    */); 
         
int 
msdos_unlink(rtems_filesystem_location_info_t *pathloc /* IN */);

int 
msdos_free_node_info(rtems_filesystem_location_info_t *pathloc /* IN */);

rtems_filesystem_node_types_t 
msdos_node_type(rtems_filesystem_location_info_t    *pathloc);

int 
msdos_mknod(const char                       *path,   /* IN */
            mode_t                            mode,   /* IN */
            dev_t                             dev,    /* IN */
            rtems_filesystem_location_info_t *pathloc /* IN/OUT */);

int 
msdos_utime(rtems_filesystem_location_info_t *pathloc, /* IN */
            time_t                            actime,  /* IN */
            time_t                            modtime  /* IN */);

int
msdos_initialize_support(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry,
  rtems_filesystem_operations_table    *op_table,
  rtems_filesystem_file_handlers_r     *file_handlers,
  rtems_filesystem_file_handlers_r     *directory_handlers
);

int 
msdos_file_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  unsigned32     flag,            /* IN  */
  unsigned32     mode             /* IN  */
);

int 
msdos_file_close(rtems_libio_t *iop /* IN  */);

ssize_t 
msdos_file_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  unsigned32     count             /* IN  */
);

ssize_t 
msdos_file_write(
  rtems_libio_t *iop,             /* IN  */
  const void    *buffer,          /* IN  */
  unsigned32     count            /* IN  */
);

int 
msdos_file_lseek(
  rtems_libio_t        *iop,              /* IN  */
  off_t                 offset,           /* IN  */
  int                   whence            /* IN  */
);

int 
msdos_file_stat(rtems_filesystem_location_info_t *loc, /* IN  */
                struct stat                      *buf  /* OUT */);

int 
msdos_file_ftruncate(
  rtems_libio_t *iop,               /* IN  */
  off_t          length             /* IN  */
);

int 
msdos_file_sync(rtems_libio_t *iop);

int 
msdos_file_datasync(rtems_libio_t *iop);
 
int 
msdos_file_ioctl(
  rtems_libio_t *iop,             /* IN  */
  unsigned32     command,         /* IN  */
  void          *buffer           /* IN  */
);

int 
msdos_file_rmnod(rtems_filesystem_location_info_t *pathloc /* IN */);
 
int 
msdos_dir_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  unsigned32     flag,            /* IN  */
  unsigned32     mode             /* IN  */
);

int 
msdos_dir_close(rtems_libio_t *iop /* IN  */);

ssize_t 
msdos_dir_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  unsigned32     count             /* IN  */
);

int 
msdos_dir_lseek(
  rtems_libio_t        *iop,              /* IN  */
  off_t                 offset,           /* IN  */
  int                   whence            /* IN  */
);

int 
msdos_dir_rmnod(rtems_filesystem_location_info_t *pathloc /* IN */);

int 
msdos_dir_sync(rtems_libio_t *iop);

int 
msdos_dir_stat(
  rtems_filesystem_location_info_t *loc,         /* IN  */
  struct stat                      *buf          /* OUT */
);

int
msdos_creat_node(rtems_filesystem_location_info_t  *parent_loc,
                 msdos_node_type_t                  type,
                 char                              *name,
                 mode_t                             mode);

/* Misc prototypes */
msdos_token_types_t msdos_get_token(const char *path,
                                    char       *token,
                                    int        *token_len);

int
msdos_find_name(rtems_filesystem_location_info_t *parent_loc,
                char                             *name);

int
msdos_get_name_node(rtems_filesystem_location_info_t *parent_loc,
                    char                             *name,
                    fat_auxiliary_t                  *paux,
                    char                             *name_dir_entry);

int
msdos_dir_info_remove(rtems_filesystem_location_info_t *pathloc);

void 
msdos_date_unix2dos(int             unix_date,
                    unsigned short *time_val, 
                    unsigned short *date);

unsigned int  
msdos_date_dos2unix(unsigned short time_val, unsigned short date);

int
msdos_set_first_cluster_num(rtems_filesystem_mount_table_entry_t *mt_entry,
                            fat_file_fd_t                        *fat_fd);

int
msdos_set_file_size(rtems_filesystem_mount_table_entry_t *mt_entry,
                    fat_file_fd_t                        *fat_fd);

int 
msdos_set_first_char4file_name(rtems_filesystem_mount_table_entry_t *mt_entry,
                               unsigned32  cl,
                               unsigned32  ofs,
                               unsigned char first_char);

int
msdos_set_dir_wrt_time_and_date(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
);
                               

int
msdos_dir_is_empty(rtems_filesystem_mount_table_entry_t *mt_entry,
                   fat_file_fd_t                        *fat_fd, 
                   rtems_boolean                        *ret_val);

int
msdos_find_name_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd, 
    char                                 *name, 
    fat_auxiliary_t                      *paux,
    char                                 *name_dir_entry);
                            
int
msdos_find_node_by_cluster_num_in_fat_file(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    unsigned32                            cl4find, 
    fat_auxiliary_t                      *paux,
    char                                 *dir_entry
);

int
msdos_get_dotdot_dir_info_cluster_num_and_offset(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                            cln,
    fat_auxiliary_t                      *paux,
    char                                 *dir_entry
);

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_MSDOS_H__ */
