/**
 * @file rtems/imfs.h
 *
 * Header file for the In-Memory File System
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_IMFS_H
#define _RTEMS_IMFS_H

#include <limits.h>

#include <rtems/libio_.h>
#include <rtems/pipe.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Data types
 */

struct IMFS_jnode_tt;
typedef struct IMFS_jnode_tt IMFS_jnode_t;

typedef struct {
  rtems_chain_control                    Entries;
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
  char *name;
} IMFS_sym_link_t;

typedef struct {
  pipe_control_t  *pipe;
} IMFS_fifo_t;

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

#define IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK     128
  extern int imfs_rq_memfile_bytes_per_block;
  extern int imfs_memfile_bytes_per_block;

#define IMFS_MEMFILE_BYTES_PER_BLOCK imfs_memfile_bytes_per_block
#define IMFS_MEMFILE_BLOCK_SLOTS \
  (IMFS_MEMFILE_BYTES_PER_BLOCK / sizeof(void *))

typedef uint8_t *block_p;
typedef block_p *block_ptr;

typedef struct {
  off_t         size;             /* size of file in bytes */
  block_ptr     indirect;         /* array of 128 data blocks pointers */
  block_ptr     doubly_indirect;  /* 128 indirect blocks */
  block_ptr     triply_indirect;  /* 128 doubly indirect blocks */
} IMFS_memfile_t;

typedef struct {
  off_t         size;             /* size of file in bytes */
  block_p       direct;           /* pointer to file image */
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

typedef enum {
  IMFS_DIRECTORY = RTEMS_FILESYSTEM_DIRECTORY,
  IMFS_DEVICE = RTEMS_FILESYSTEM_DEVICE,
  IMFS_HARD_LINK = RTEMS_FILESYSTEM_HARD_LINK,
  IMFS_SYM_LINK =  RTEMS_FILESYSTEM_SYM_LINK,
  IMFS_MEMORY_FILE = RTEMS_FILESYSTEM_MEMORY_FILE,
  IMFS_LINEAR_FILE,
  IMFS_FIFO
} IMFS_jnode_types_t;

typedef union {
  IMFS_directory_t   directory;
  IMFS_device_t      device;
  IMFS_link_t        hard_link;
  IMFS_sym_link_t    sym_link;
  IMFS_memfile_t     file;
  IMFS_linearfile_t  linearfile;
  IMFS_fifo_t        fifo;
} IMFS_types_union;

/*
 * Major device number for the IMFS. This is not a real device number because
 * the IMFS is just a file system and does not have a driver.
 */
#define IMFS_DEVICE_MAJOR_NUMBER (0xfffe)

/*
 *  Maximum length of a "basename" of an IMFS file/node.
 */

#define IMFS_NAME_MAX  32

/*
 *  The control structure for an IMFS jnode.
 */

struct IMFS_jnode_tt {
  rtems_chain_node    Node;                  /* for chaining them together */
  IMFS_jnode_t       *Parent;                /* Parent node */
  char                name[IMFS_NAME_MAX+1]; /* "basename" */
  mode_t              st_mode;               /* File mode */
  unsigned short      reference_count;
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

#define IMFS_mtime_ctime_update( _jnode )   \
  do {                                      \
    struct timeval tv;                      \
    gettimeofday( &tv, 0 );                 \
    _jnode->stat_mtime  = (time_t) tv.tv_sec; \
    _jnode->stat_ctime  = (time_t) tv.tv_sec; \
  } while (0)

typedef struct {
  int                                     instance;
  ino_t                                   ino_count;
  const rtems_filesystem_file_handlers_r *memfile_handlers;
  const rtems_filesystem_file_handlers_r *directory_handlers;
  const rtems_filesystem_file_handlers_r *link_handlers;
  const rtems_filesystem_file_handlers_r *fifo_handlers;
} IMFS_fs_info_t;

/*
 *  Shared Data
 */

extern const rtems_filesystem_file_handlers_r       IMFS_directory_handlers;
extern const rtems_filesystem_file_handlers_r       IMFS_device_handlers;
extern const rtems_filesystem_file_handlers_r       IMFS_link_handlers;
extern const rtems_filesystem_file_handlers_r       IMFS_memfile_handlers;
extern const rtems_filesystem_file_handlers_r       IMFS_fifo_handlers;
extern const rtems_filesystem_operations_table      IMFS_ops;
extern const rtems_filesystem_operations_table      fifoIMFS_ops;
extern const rtems_filesystem_limits_and_options_t  IMFS_LIMITS_AND_OPTIONS;

/*
 *  Routines
 */

extern int IMFS_initialize(
   rtems_filesystem_mount_table_entry_t *mt_entry,
   const void                           *data
);

extern int fifoIMFS_initialize(
  rtems_filesystem_mount_table_entry_t  *mt_entry,
  const void                            *data
);

extern int miniIMFS_initialize(
   rtems_filesystem_mount_table_entry_t *mt_entry,
   const void                           *data
);

extern int IMFS_initialize_support(
   rtems_filesystem_mount_table_entry_t       *mt_entry,
   const rtems_filesystem_operations_table    *op_table,
   const rtems_filesystem_file_handlers_r     *link_handlers,
   const rtems_filesystem_file_handlers_r     *fifo_handlers
);

extern void IMFS_fsunmount(
   rtems_filesystem_mount_table_entry_t *mt_entry
);

extern int rtems_tarfs_load(
   const char *mountpoint,
   uint8_t *tar_image,
   size_t tar_size
);

extern void IMFS_dump( void );

/*
 * Return the size of the largest file which can be created
 * using the IMFS memory file type.
 */
extern int IMFS_memfile_maximum_size( void );

extern void IMFS_node_destroy( IMFS_jnode_t *node );

extern int IMFS_node_clone( rtems_filesystem_location_info_t *loc );

extern void IMFS_node_free( const rtems_filesystem_location_info_t *loc );

extern rtems_filesystem_node_types_t IMFS_node_type(
  const rtems_filesystem_location_info_t *loc
);

extern int IMFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);

extern void IMFS_Set_handlers( rtems_filesystem_location_info_t *loc );

extern void IMFS_eval_path(
  rtems_filesystem_eval_path_context_t *ctx
);

extern int IMFS_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
);

extern int IMFS_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
);

extern int IMFS_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
);

extern IMFS_jnode_t *IMFS_allocate_node(
  IMFS_jnode_types_t                type,         /* IN  */
  const char                       *name,         /* IN  */
  size_t                            namelen,      /* IN  */
  mode_t                            mode          /* IN  */
);

extern IMFS_jnode_t *IMFS_create_root_node(void);

extern IMFS_jnode_t *IMFS_create_node(
  const rtems_filesystem_location_info_t *pathloc, /* IN  */
  IMFS_jnode_types_t                      type,    /* IN  */
  const char                             *name,    /* IN  */
  size_t                                  namelen, /* IN  */
  mode_t                                  mode,    /* IN  */
  const IMFS_types_union                 *info     /* IN  */
);

extern int IMFS_mount(
  rtems_filesystem_mount_table_entry_t *mt_entry  /* IN */
);

extern int IMFS_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry  /* IN */
);

extern int IMFS_memfile_remove(
 IMFS_jnode_t  *the_jnode         /* IN/OUT */
);

extern int memfile_ftruncate(
  rtems_libio_t *iop,               /* IN  */
  off_t          length             /* IN  */
);

extern int imfs_dir_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  int            oflag,           /* IN  */
  mode_t         mode             /* IN  */
);

extern ssize_t imfs_dir_read(
  rtems_libio_t *iop,              /* IN  */
  void          *buffer,           /* IN  */
  size_t         count             /* IN  */
);

extern off_t imfs_dir_lseek(
  rtems_libio_t        *iop,              /* IN  */
  off_t                 offset,           /* IN  */
  int                   whence            /* IN  */
);

extern int memfile_open(
  rtems_libio_t *iop,             /* IN  */
  const char    *pathname,        /* IN  */
  int            oflag,           /* IN  */
  mode_t         mode             /* IN  */
);

extern ssize_t memfile_read(
  rtems_libio_t *iop,             /* IN  */
  void          *buffer,          /* IN  */
  size_t         count            /* IN  */
);

extern ssize_t memfile_write(
  rtems_libio_t *iop,             /* IN  */
  const void    *buffer,          /* IN  */
  size_t         count            /* IN  */
);

extern int memfile_ioctl(
  rtems_libio_t *iop,             /* IN  */
  uint32_t       command,         /* IN  */
  void          *buffer           /* IN  */
);

extern off_t memfile_lseek(
  rtems_libio_t        *iop,        /* IN  */
  off_t                 offset,     /* IN  */
  int                   whence      /* IN  */
);

extern int device_open(
  rtems_libio_t *iop,            /* IN  */
  const char    *pathname,       /* IN  */
  int            oflag,          /* IN  */
  mode_t         mode            /* IN  */
);

extern int device_close(
  rtems_libio_t *iop             /* IN  */
);

extern ssize_t device_read(
  rtems_libio_t *iop,            /* IN  */
  void          *buffer,         /* IN  */
  size_t         count           /* IN  */
);

extern ssize_t device_write(
  rtems_libio_t *iop,               /* IN  */
  const void    *buffer,            /* IN  */
  size_t         count              /* IN  */
);

extern int device_ioctl(
  rtems_libio_t *iop,               /* IN  */
  uint32_t       command,           /* IN  */
  void          *buffer             /* IN  */
);

extern off_t device_lseek(
  rtems_libio_t *iop,               /* IN  */
  off_t          offset,            /* IN  */
  int            whence             /* IN  */
);

extern int device_ftruncate(
  rtems_libio_t *iop,               /* IN  */
  off_t          length             /* IN  */
);

extern int IMFS_utime(
  const rtems_filesystem_location_info_t *loc,
  time_t actime,
  time_t modtime
);

extern int IMFS_fchmod(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
);

extern int IMFS_symlink(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  const char *target
);

extern ssize_t IMFS_readlink(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
);

extern int IMFS_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
);

extern int IMFS_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
);

/*
 *  Turn on IMFS assertions when RTEMS_DEBUG is defined.
 */
#ifdef RTEMS_DEBUG
  #include <assert.h>

  #define IMFS_assert(_x) assert(_x)
#else
  #define IMFS_assert(_x)
#endif

static inline void IMFS_add_to_directory(
  IMFS_jnode_t *dir,
  IMFS_jnode_t *node
)
{
  node->Parent = dir;
  rtems_chain_append_unprotected( &dir->info.directory.Entries, &node->Node );
}

static inline void IMFS_remove_from_directory( IMFS_jnode_t *node )
{
  IMFS_assert( node->Parent != NULL );
  node->Parent = NULL;
  rtems_chain_extract_unprotected( &node->Node );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
