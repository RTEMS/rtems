/*
 *  Header file for the In-Memory File System
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __IMFS_h
#define __IMFS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <chain.h>

#include <sys/types.h>
#include <limits.h>
#include <rtems/libio.h>

/*
 *  File name macros
 */

#define IMFS_is_valid_name_char( _ch ) ( 1 )

#define IMFS_is_separator( _ch ) \
   rtems_filesystem_is_separator( _ch )

/*
 *  Data types
 */

struct IMFS_jnode_tt;
typedef struct IMFS_jnode_tt IMFS_jnode_t;

typedef struct {
  Chain_Control                          Entries;
  rtems_filesystem_mount_table_entry_t  *mt_fs;
}  IMFS_directory_t;

typedef struct {
  rtems_device_major_number  major;
  rtems_device_minor_number  minor;
}  IMFS_device_t;

typedef struct {
  IMFS_jnode_t  *link_node;
} IMFS_link_t;

typedef struct {
  const char *name;
} IMFS_sym_link_t;

/*
 *  IMFS "memfile" information
 *
 *  The data structure for the in-memory "memfiles" is based on classic UNIX.
 *
 *  block_ptr is a pointer to a block of IMFS_MEMFILE_BYTES_PER_BLOCK in
 *  length which could be data or a table of pointers to blocks.
 *
 *  Setting IMFS_MEMFILE_BYTES_PER_BLOCK to different values has a significant
 *  impact on the maximum file size supported as well as the amount of
 *  memory wasted due to internal file fragmentation.  The following
 *  is a list of maximum file sizes based on various settings
 *
 *    max_filesize with blocks of   16 is         1,328
 *    max_filesize with blocks of   32 is        18,656
 *    max_filesize with blocks of   64 is       279,488
 *    max_filesize with blocks of  128 is     4,329,344
 *    max_filesize with blocks of  256 is    68,173,568
 *    max_filesize with blocks of  512 is 1,082,195,456
 */

#define IMFS_MEMFILE_BYTES_PER_BLOCK     128
#define IMFS_MEMFILE_BLOCK_SLOTS \
  (IMFS_MEMFILE_BYTES_PER_BLOCK / sizeof(void *))

typedef unsigned char * block_p;
typedef block_p *block_ptr;

typedef struct {
  off_t      size;             /* size of file in bytes */
  block_ptr  indirect;         /* array of 128 data blocks pointers */
  block_ptr  doubly_indirect;  /* 128 indirect blocks */
  block_ptr  triply_indirect;  /* 128 doubly indirect blocks */
} IMFS_memfile_t;

typedef struct {
  off_t      size;             /* size of file in bytes */
  block_p    direct;           /* pointer to file image */
} IMFS_linearfile_t;

/*
 *  Important block numbers for "memfiles"
 */

#define FIRST_INDIRECT           (0)
#define LAST_INDIRECT            (IMFS_MEMFILE_BLOCK_SLOTS - 1)

#define FIRST_DOUBLY_INDIRECT    (LAST_INDIRECT + 1)
#define LAST_DOUBLY_INDIRECT     \
   (LAST_INDIRECT + \
     (IMFS_MEMFILE_BLOCK_SLOTS * IMFS_MEMFILE_BLOCK_SLOTS))

#define FIRST_TRIPLY_INDIRECT    (LAST_DOUBLY_INDIRECT + 1)
#define LAST_TRIPLY_INDIRECT \
   (LAST_DOUBLY_INDIRECT +\
     (IMFS_MEMFILE_BLOCK_SLOTS * \
        IMFS_MEMFILE_BLOCK_SLOTS * IMFS_MEMFILE_BLOCK_SLOTS))

#define IMFS_MEMFILE_MAXIMUM_SIZE \
  (LAST_TRIPLY_INDIRECT * IMFS_MEMFILE_BYTES_PER_BLOCK)

/*
 *  What types of IMFS file systems entities there can be.
 */

#define IMFS_jnode_types_t rtems_filesystem_node_types_t
#define IMFS_DIRECTORY     RTEMS_FILESYSTEM_DIRECTORY
#define IMFS_DEVICE        RTEMS_FILESYSTEM_DEVICE
#define IMFS_HARD_LINK     RTEMS_FILESYSTEM_HARD_LINK
#define IMFS_SYM_LINK      RTEMS_FILESYSTEM_SYM_LINK
#define IMFS_MEMORY_FILE   RTEMS_FILESYSTEM_MEMORY_FILE
#define IMFS_LINEAR_FILE   (IMFS_MEMORY_FILE + 1)

#define IMFS_NUMBER_OF_TYPES  (IMFS_LINEAR_FILE + 1)

typedef union {
  IMFS_directory_t   directory;
  IMFS_device_t      device;
  IMFS_link_t        hard_link;
  IMFS_sym_link_t    sym_link;   
  IMFS_memfile_t     file;   
  IMFS_linearfile_t  linearfile;   
} IMFS_types_union;

/*
 *  Maximum length of a "basename" of an IMFS file/node.
 */

#define IMFS_NAME_MAX  32

/*
 *  The control structure for an IMFS jnode.
 */

struct IMFS_jnode_tt {
  Chain_Node          Node;                  /* for chaining them together */
  IMFS_jnode_t       *Parent;                /* Parent node */
  char                name[IMFS_NAME_MAX+1]; /* "basename" */ 
  mode_t              st_mode;               /* File mode */
  nlink_t             st_nlink;              /* Link count */
  ino_t               st_ino;                /* inode */

  uid_t               st_uid;                /* User ID of owner */
  gid_t               st_gid;                /* Group ID of owner */

  time_t              stat_atime;            /* Time of last access */
  time_t              stat_mtime;            /* Time of last modification */
  time_t              stat_ctime;            /* Time of last status change */
  IMFS_jnode_types_t  type;                  /* Type of this entry */
  IMFS_types_union    info;
};

#define IMFS_update_atime( _jnode )         \
  do {                                      \
    struct timeval tv;                      \
    gettimeofday( &tv, 0 );                 \
    _jnode->stat_atime  = (time_t) tv.tv_sec; \
  } while (0)
                
#define IMFS_update_mtime( _jnode )         \
  do {                                      \
    struct timeval tv;                      \
    gettimeofday( &tv, 0 );                 \
    _jnode->stat_mtime  = (time_t) tv.tv_sec; \
  } while (0)
                
#define IMFS_update_ctime( _jnode )         \
  do {                                      \
    struct timeval tv;                      \
    gettimeofday( &tv, 0 );                 \
    _jnode->stat_ctime  = (time_t) tv.tv_sec; \
  } while (0)

#define IMFS_atime_mtime_update( _jnode )   \
  do {                                      \
    struct timeval tv;                      \
    gettimeofday( &tv, 0 );                 \
    _jnode->stat_mtime  = (time_t) tv.tv_sec; \
    _jnode->stat_atime  = (time_t) tv.tv_sec; \
  } while (0)

typedef struct {
  ino_t                             ino_count;
  rtems_filesystem_file_handlers_r *linearfile_handlers;
  rtems_filesystem_file_handlers_r *memfile_handlers;
  rtems_filesystem_file_handlers_r *directory_handlers;
} IMFS_fs_info_t;

#if UNUSED
/* FIXME: Unused, we might want to remove it */
#define increment_and_check_linkcounts( _fs_info )                  \
  ((IMFS_fs_info_t * )_fs_info)->link_counts++;                     \
  if ( ((IMFS_fs_info_t * )_fs_info)->link_counts  > MAXSYMLINKS )  \
    rtems_set_errno_and_return_minus_one( ELOOP )
#endif

#define decrement_linkcounts(  _fs_info )             \
  ((IMFS_fs_info_t * )_fs_info)->link_counts--;        

/*
 *  Type defination for tokens returned from IMFS_get_token
 */

typedef enum {
  IMFS_NO_MORE_PATH,
  IMFS_CURRENT_DIR,
  IMFS_UP_DIR,
  IMFS_NAME,
  IMFS_INVALID_TOKEN
} IMFS_token_types;

/*
 *  Shared Data
 */

extern rtems_filesystem_file_handlers_r       IMFS_directory_handlers;
extern rtems_filesystem_file_handlers_r       IMFS_device_handlers;
extern rtems_filesystem_file_handlers_r       IMFS_link_handlers;
extern rtems_filesystem_file_handlers_r       IMFS_linearfile_handlers;
extern rtems_filesystem_file_handlers_r       IMFS_memfile_handlers;
extern rtems_filesystem_operations_table      IMFS_ops;
extern rtems_filesystem_operations_table      miniIMFS_ops;
extern rtems_filesystem_limits_and_options_t  IMFS_LIMITS_AND_OPTIONS; 

/*
 *  Routines
 */

int IMFS_initialize( 
   rtems_filesystem_mount_table_entry_t *mt_entry
);

int miniIMFS_initialize( 
   rtems_filesystem_mount_table_entry_t *mt_entry
);

int IMFS_initialize_support( 
   rtems_filesystem_mount_table_entry_t *mt_entry,
   rtems_filesystem_operations_table    *op_table,
   rtems_filesystem_file_handlers_r     *linearfile_handlers,
   rtems_filesystem_file_handlers_r     *memfile_handlers,
   rtems_filesystem_file_handlers_r     *directory_handlers
);

int IMFS_fsunmount(
   rtems_filesystem_mount_table_entry_t *mt_entry
);

int rtems_tarfs_load(
   char          *mountpoint,
   unsigned char *addr,
   unsigned long length
);

/*
 * Returns the number of characters copied from path to token.
 */
IMFS_token_types IMFS_get_token(
  const char       *path,
  char             *token,
  int              *token_len
);

void IMFS_dump( void );

void IMFS_initialize_jnode(
  IMFS_jnode_t        *the_jnode,
  IMFS_jnode_types_t   type,
  IMFS_jnode_t        *the_parent,
  char                *name,
  mode_t               mode
);

IMFS_jnode_t *IMFS_find_match_in_dir(
  IMFS_jnode_t *directory,                         /* IN */
  char         *name                               /* IN */
);

rtems_filesystem_node_types_t IMFS_node_type(
  rtems_filesystem_location_info_t    *pathloc     /* IN */
);

int IMFS_stat(
  rtems_filesystem_location_info_t *loc,           /* IN  */
  struct stat                      *buf            /* OUT */
);

int IMFS_Set_handlers(  
  rtems_filesystem_location_info_t   *loc 
);

int IMFS_evaluate_link(
  rtems_filesystem_location_info_t *node,        /* IN/OUT */
  int                               flags        /* IN     */
);

int IMFS_eval_path(  
  const char                        *pathname,     /* IN     */
  int                               flags,         /* IN     */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
);


int IMFS_link(
  rtems_filesystem_location_info_t  *to_loc,      /* IN */
  rtems_filesystem_location_info_t  *parent_loc,  /* IN */
  const char                        *token        /* IN */
);

int IMFS_unlink(
  rtems_filesystem_location_info_t  *pathloc       /* IN */
);

int IMFS_chown(
  rtems_filesystem_location_info_t  *pathloc,      /* IN */
  uid_t                              owner,        /* IN */
  gid_t                              group         /* IN */
);

int IMFS_freenodinfo(
  rtems_filesystem_location_info_t  *pathloc       /* IN */
);

int IMFS_mknod(
  const char                        *path,         /* IN */
  mode_t                             mode,         /* IN */
  dev_t                              dev,          /* IN */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
);

IMFS_jnode_t *IMFS_create_node(
  rtems_filesystem_location_info_t  *parent_loc,   /* IN  */
  IMFS_jnode_types_t                 type,         /* IN  */
  char                              *name,         /* IN  */
  mode_t                             mode,         /* IN  */
  IMFS_types_union                  *info          /* IN  */
);

int IMFS_evaluate_for_make(
  const char                         *path,        /* IN     */
  rtems_filesystem_location_info_t   *pathloc,     /* IN/OUT */
  const char                        **name         /* OUT    */
);

int IMFS_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry  /* IN */
);

int IMFS_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry  /* IN */
);

int IMFS_freenod(
  rtems_filesystem_location_info_t  *node         /* IN/OUT */
);

int IMFS_memfile_remove(
 IMFS_jnode_t  *the_jnode         /* IN/OUT */
);

int memfile_ftruncate(
  rtems_libio_t *iop,               /* IN  */
  off_t          length             /* IN  */
);

int imfs_dir_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  unsigned32     flag,            /* IN  */
  unsigned32     mode             /* IN  */
);

int imfs_dir_close(
  rtems_libio_t *iop             /* IN  */
);

ssize_t imfs_dir_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  unsigned32     count             /* IN  */
);

int imfs_dir_lseek(
  rtems_libio_t        *iop,              /* IN  */
  off_t                 offset,           /* IN  */
  int                   whence            /* IN  */
);

int imfs_dir_fstat(
  rtems_filesystem_location_info_t *loc,         /* IN  */
  struct stat                      *buf          /* OUT */
);

int imfs_dir_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
);

int linearfile_read(
  rtems_libio_t *iop,             /* IN  */
  void          *buffer,          /* IN  */
  unsigned32     count            /* IN  */
);

int linearfile_lseek(
  rtems_libio_t        *iop,        /* IN  */
  off_t                 offset,     /* IN  */
  int                   whence      /* IN  */
);

int memfile_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  unsigned32     flag,            /* IN  */
  unsigned32     mode             /* IN  */
);

int memfile_close(
  rtems_libio_t *iop             /* IN  */
);

ssize_t memfile_read(
  rtems_libio_t *iop,             /* IN  */
  void          *buffer,          /* IN  */
  unsigned32     count            /* IN  */
);

ssize_t memfile_write(
  rtems_libio_t *iop,             /* IN  */
  const void    *buffer,          /* IN  */
  unsigned32     count            /* IN  */
);

int memfile_ioctl(
  rtems_libio_t *iop,             /* IN  */
  unsigned32     command,         /* IN  */
  void          *buffer           /* IN  */
);

int memfile_lseek(
  rtems_libio_t        *iop,        /* IN  */
  off_t                 offset,     /* IN  */
  int                   whence      /* IN  */
);

int memfile_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
);

int device_open(
  rtems_libio_t *iop,            /* IN  */
  const char    *pathname,       /* IN  */
  unsigned32     flag,           /* IN  */
  unsigned32     mode            /* IN  */
);

int device_close(
  rtems_libio_t *iop             /* IN  */
);

ssize_t device_read(
  rtems_libio_t *iop,            /* IN  */
  void          *buffer,         /* IN  */
  unsigned32     count           /* IN  */
);

ssize_t device_write(
  rtems_libio_t *iop,               /* IN  */
  const void    *buffer,            /* IN  */
  unsigned32     count              /* IN  */
);

int device_ioctl(
  rtems_libio_t *iop,               /* IN  */
  unsigned32     command,           /* IN  */
  void          *buffer             /* IN  */
);

int device_lseek(
  rtems_libio_t *iop,               /* IN  */
  off_t          offset,            /* IN  */
  int            whence             /* IN  */
);

int IMFS_utime(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  time_t                             actime,        /* IN */
  time_t                             modtime        /* IN */
);

int IMFS_fchmod(
  rtems_filesystem_location_info_t *loc,
  mode_t                            mode
);

int IMFS_symlink(
  rtems_filesystem_location_info_t  *parent_loc,  /* IN */
  const char                        *link_name,
  const char                        *node_name
);

int IMFS_readlink(
 rtems_filesystem_location_info_t   *loc,         /* IN */
 char                               *buf,         /* OUT */
 size_t                             bufsize    
);

int IMFS_fdatasync(
  rtems_libio_t *iop
);

int IMFS_fcntl(
  int            cmd,
  rtems_libio_t *iop
);

int IMFS_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
