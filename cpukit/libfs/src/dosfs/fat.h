/*
 *  fat.h
 *
 *  Constants/data structures/prototypes for low-level operations on a volume
 *  with FAT filesystem
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

#ifndef __DOSFS_FAT_H__
#define __DOSFS_FAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include <rtems/seterr.h>

/* XXX: temporary hack :(( */
#ifndef set_errno_and_return_minus_one
#define set_errno_and_return_minus_one rtems_set_errno_and_return_minus_one
#endif /* set_errno_and_return_minus_one */

#include <rtems/score/cpu.h>
#include <errno.h>
#include <rtems/bdbuf.h>

#ifndef RC_OK
#define RC_OK 0x00000000
#endif

/* 
 * Remember that all FAT file system on disk data structure is 
 * "little endian"! 
 * (derived from linux)
 */
/*
 * Conversion from and to little-endian byte order. (no-op on i386/i486)
 *
 * Naming: Ca_b_c, where a: F = from, T = to, b: LE = little-endian,
 * BE = big-endian, c: W = word (16 bits), L = longword (32 bits)
 */

#if (CPU_BIG_ENDIAN == TRUE)
#    define CF_LE_W(v) CPU_swap_u16((unsigned16)v)
#    define CF_LE_L(v) CPU_swap_u32((unsigned32)v)
#    define CT_LE_W(v) CPU_swap_u16((unsigned16)v)
#    define CT_LE_L(v) CPU_swap_u32((unsigned32)v)
#else  
#    define CF_LE_W(v) (v)
#    define CF_LE_L(v) (v)
#    define CT_LE_W(v) (v)
#    define CT_LE_L(v) (v)
#endif  

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define FAT_HASH_SIZE   2
#define FAT_HASH_MODULE FAT_HASH_SIZE


#define FAT_SECTOR512_SIZE     512 /* sector size (bytes) */
#define FAT_SECTOR512_BITS       9 /* log2(SECTOR_SIZE) */

/* maximum + 1 number of clusters for FAT12 */
#define FAT_FAT12_MAX_CLN      4085 

/* maximum + 1 number of clusters for FAT16 */
#define FAT_FAT16_MAX_CLN      65525 

#define FAT_FAT12              0x01
#define FAT_FAT16              0x02
#define FAT_FAT32              0x04
 
#define FAT_UNDEFINED_VALUE     (unsigned32)0xFFFFFFFF 

#define FAT_FAT12_EOC          0x0FF8
#define FAT_FAT16_EOC          0xFFF8
#define FAT_FAT32_EOC          (unsigned32)0x0FFFFFF8

#define FAT_FAT12_FREE         0x0000
#define FAT_FAT16_FREE         0x0000
#define FAT_FAT32_FREE         0x00000000

#define FAT_GENFAT_EOC         (unsigned32)0xFFFFFFFF
#define FAT_GENFAT_FREE        (unsigned32)0x00000000

#define FAT_FAT12_SHIFT        0x04

#define FAT_FAT12_MASK         0x00000FFF
#define FAT_FAT16_MASK         0x0000FFFF
#define FAT_FAT32_MASK         (unsigned32)0x0FFFFFFF

#define FAT_MAX_BPB_SIZE       90

/* size of useful information in FSInfo sector */
#define FAT_USEFUL_INFO_SIZE   12

#define FAT_VAL8(x, ofs)       (unsigned8)(*((unsigned8 *)(x) + (ofs)))
 
#define FAT_VAL16(x, ofs)                                   \
    (unsigned16)( (*((unsigned8 *)(x) + (ofs))) |           \
                  ((*((unsigned8 *)(x) + (ofs) + 1)) << 8) )

#define FAT_VAL32(x, ofs)                                                 \
    (unsigned32)( (unsigned32)(*((unsigned8 *)(x) + (ofs))) |             \
                  ((unsigned32)(*((unsigned8 *)(x) + (ofs) + 1)) << 8)  | \
                  ((unsigned32)(*((unsigned8 *)(x) + (ofs) + 2)) << 16) | \
                  ((unsigned32)(*((unsigned8 *)(x) + (ofs) + 3)) << 24) )
                    
/* macros to access boot sector fields */
#define FAT_BR_BYTES_PER_SECTOR(x)       FAT_VAL16(x, 11)
#define FAT_BR_SECTORS_PER_CLUSTER(x)    FAT_VAL8(x, 13) 
#define FAT_BR_RESERVED_SECTORS_NUM(x)   FAT_VAL16(x, 14)
#define FAT_BR_FAT_NUM(x)                FAT_VAL8(x, 16)
#define FAT_BR_FILES_PER_ROOT_DIR(x)     FAT_VAL16(x, 17)
#define FAT_BR_TOTAL_SECTORS_NUM16(x)    FAT_VAL16(x, 19)
#define FAT_BR_MEDIA(x)                  FAT_VAL8(x, 21) 
#define FAT_BR_SECTORS_PER_FAT(x)        FAT_VAL16(x, 22)
#define FAT_BR_TOTAL_SECTORS_NUM32(x)    FAT_VAL32(x, 32)
#define FAT_BR_SECTORS_PER_FAT32(x)      FAT_VAL32(x, 36)
#define FAT_BR_EXT_FLAGS(x)              FAT_VAL16(x, 40)
#define FAT_BR_FAT32_ROOT_CLUSTER(x)     FAT_VAL32(x, 44)
#define FAT_BR_FAT32_FS_INFO_SECTOR(x)   FAT_VAL16(x, 48)
#define FAT_FSINFO_LEAD_SIGNATURE(x)     FAT_VAL32(x, 0)
/* 
 * I read FSInfo sector from offset 484 to access the information, so offsets 
 * of these fields a relative
 */
#define FAT_FSINFO_FREE_CLUSTER_COUNT(x) FAT_VAL32(x, 4)
#define FAT_FSINFO_NEXT_FREE_CLUSTER(x)  FAT_VAL32(x, 8)

#define FAT_FSINFO_FREE_CLUSTER_COUNT_OFFSET 488

#define FAT_FSINFO_NEXT_FREE_CLUSTER_OFFSET  492

#define FAT_RSRVD_CLN                        0x02  

#define FAT_FSINFO_LEAD_SIGNATURE_VALUE      0x41615252

#define FAT_FSI_LEADSIG_SIZE                 0x04

#define FAT_FSI_INFO                         484

#define MS_BYTES_PER_CLUSTER_LIMIT           0x8000     /* 32K */

#define FAT_BR_EXT_FLAGS_MIRROR              0x0080

#define FAT_BR_EXT_FLAGS_FAT_NUM             0x000F


#define FAT_DIRENTRY_SIZE          32 
 
#define FAT_DIRENTRIES_PER_SEC512  16

/* 
 * Volume descriptor
 * Description of the volume the FAT filesystem is located on - generally 
 * the fields of the structure corresponde to Boot Sector and BPB Srtucture
 * (see M$ White Paper) fields
 */
typedef struct fat_vol_s 
{
    unsigned16   bps;            /* bytes per sector */
    unsigned8    sec_log2;       /* log2 of bps */
    unsigned8    sec_mul;        /* log2 of 512bts sectors number per sector */
    unsigned8    spc;            /* sectors per cluster */
    unsigned8    spc_log2;       /* log2 of spc */
    unsigned16   bpc;            /* bytes per cluster */
    unsigned8    bpc_log2;       /* log2 of bytes per cluster */
    unsigned8    fats;           /* number of FATs */
    unsigned8    type;           /* FAT type */
    unsigned32   mask;
    unsigned32   eoc_val;
    unsigned16   fat_loc;        /* FAT start */
    unsigned32   fat_length;     /* sectors per FAT */
    unsigned32   rdir_loc;       /* root directory start */
    unsigned16   rdir_entrs;     /* files per root directory */
    unsigned32   rdir_secs;      /* sectors per root directory */
    unsigned32   rdir_size;      /* root directory size in bytes */
    unsigned32   tot_secs;       /* total count of sectors */
    unsigned32   data_fsec;      /* first data sector */
    unsigned32   data_cls;       /* count of data clusters */
    unsigned32   rdir_cl;        /* first cluster of the root directory */
    unsigned16   info_sec;       /* FSInfo Sector Structure location */
    unsigned32   free_cls;       /* last known free clusters count */
    unsigned32   next_cl;        /* next free cluster number */
    unsigned8    mirror;         /* mirroring enabla/disable */
    unsigned32   afat_loc;       /* active FAT location */
    unsigned8    afat;           /* the number of active FAT */
    dev_t        dev;            /* device ID */
    disk_device *dd;             /* disk device (see libblock) */
    void        *private_data;   /* reserved */
} fat_vol_t;


typedef struct fat_cache_s
{
    unsigned32     blk_num;
    rtems_boolean  modified;
    unsigned8      state;
    bdbuf_buffer   *buf;
} fat_cache_t;
    
/*  
 * This structure identifies the instance of the filesystem on the FAT 
 * ("fat-file") level.
 */
typedef struct fat_fs_info_s
{
    fat_vol_t      vol;           /* volume descriptor */
    Chain_Control *vhash;         /* "vhash" of fat-file descriptors */
    Chain_Control *rhash;         /* "rhash" of fat-file descriptors */
    char          *uino;          /* array of unique ino numbers */
    unsigned32     index;
    unsigned32     uino_pool_size; /* size */
    unsigned32     uino_base;
    fat_cache_t    c;             /* cache */
    unsigned8     *sec_buf; /* just placeholder for anything */
} fat_fs_info_t;

/* 
 * if the name we looking for is file we store not only first data cluster
 * number, but and cluster number and offset for directory entry for this 
 * name
 */
typedef struct fat_auxiliary_s
{
    unsigned32 cln;
    unsigned32 ofs;
} fat_auxiliary_t;

#define FAT_FAT_OFFSET(fat_type, cln)                  \
    ((fat_type) & FAT_FAT12 ? ((cln) + ((cln) >> 1)) : \
     (fat_type) & FAT_FAT16 ? ((cln) << 1)           : \
     ((cln) << 2))

#define FAT_CLUSTER_IS_ODD(n)  ((n) & 0x0001)

#define FAT12_SHIFT      0x4    /* half of a byte */

/* initial size of array of unique ino */
#define FAT_UINO_POOL_INIT_SIZE  0x100

/* cache support */
#define FAT_CACHE_EMPTY   0x0
#define FAT_CACHE_ACTUAL  0x1

#define FAT_OP_TYPE_READ  0x1
#define FAT_OP_TYPE_GET   0x2

static inline unsigned32
fat_cluster_num_to_sector_num(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                            cln
    )
{
    register fat_fs_info_t *fs_info = mt_entry->fs_info;
  
    if ( (cln == 0) && (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)) )
        return fs_info->vol.rdir_loc;  

    return (((cln - FAT_RSRVD_CLN) << fs_info->vol.spc_log2) + 
            fs_info->vol.data_fsec);
} 

static inline unsigned32
fat_cluster_num_to_sector512_num(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                            cln
    )
{
    fat_fs_info_t *fs_info = mt_entry->fs_info;

    if (cln == 1)
        return 1;

    return (fat_cluster_num_to_sector_num(mt_entry, cln) <<
            fs_info->vol.sec_mul);
} 

static inline int
fat_buf_access(fat_fs_info_t *fs_info, unsigned32 blk, int op_type, 
               bdbuf_buffer **buf)
{
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    unsigned8         i;
    rtems_boolean     sec_of_fat;
    

    if (fs_info->c.state == FAT_CACHE_EMPTY)
    {
        if (op_type == FAT_OP_TYPE_READ)
            sc = rtems_bdbuf_read(fs_info->vol.dev, blk, &fs_info->c.buf);
        else
            sc = rtems_bdbuf_get(fs_info->vol.dev, blk, &fs_info->c.buf);    
        if (sc != RTEMS_SUCCESSFUL)
            set_errno_and_return_minus_one(EIO);
        fs_info->c.blk_num = blk;    
	fs_info->c.modified = 0;    
        fs_info->c.state = FAT_CACHE_ACTUAL;    
    }
    
    sec_of_fat = ((fs_info->c.blk_num >= fs_info->vol.fat_loc) && 
                  (fs_info->c.blk_num < fs_info->vol.rdir_loc));

    if (fs_info->c.blk_num != blk)
    {
        if (fs_info->c.modified)
        {
            if (sec_of_fat && !fs_info->vol.mirror)
                memcpy(fs_info->sec_buf, fs_info->c.buf->buffer, 
                       fs_info->vol.bps);
            
            sc = rtems_bdbuf_release_modified(fs_info->c.buf);
	    fs_info->c.state = FAT_CACHE_EMPTY;
            fs_info->c.modified = 0;    
            if (sc != RTEMS_SUCCESSFUL)
                set_errno_and_return_minus_one(EIO);
            
            if (sec_of_fat && !fs_info->vol.mirror)
            {
                bdbuf_buffer *b;
            
                for (i = 1; i < fs_info->vol.fats; i++)
                {
                    sc = rtems_bdbuf_get(fs_info->vol.dev,
                                         fs_info->c.blk_num + 
                                         fs_info->vol.fat_length * i, 
                                         &b);
                    if ( sc != RTEMS_SUCCESSFUL)
                        set_errno_and_return_minus_one(ENOMEM);
                    memcpy(b->buffer, fs_info->sec_buf, fs_info->vol.bps);
                    sc = rtems_bdbuf_release_modified(b);
                    if ( sc != RTEMS_SUCCESSFUL)
                        set_errno_and_return_minus_one(ENOMEM);
                }
            }    
        }
        else
        {
            sc = rtems_bdbuf_release(fs_info->c.buf);
	    fs_info->c.state = FAT_CACHE_EMPTY;
            if (sc != RTEMS_SUCCESSFUL)
                set_errno_and_return_minus_one(EIO);
        
        }         
        if (op_type == FAT_OP_TYPE_READ)
            sc = rtems_bdbuf_read(fs_info->vol.dev, blk, &fs_info->c.buf);
        else
            sc = rtems_bdbuf_get(fs_info->vol.dev, blk, &fs_info->c.buf);    
        if (sc != RTEMS_SUCCESSFUL)
            set_errno_and_return_minus_one(EIO);
        fs_info->c.blk_num = blk;
	fs_info->c.state = FAT_CACHE_ACTUAL;
    }
    *buf = fs_info->c.buf;
    return RC_OK;
}


static inline int 
fat_buf_release(fat_fs_info_t *fs_info)
{
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    unsigned8         i;
    rtems_boolean     sec_of_fat;
        
    if (fs_info->c.state == FAT_CACHE_EMPTY)
        return RC_OK;
            
    sec_of_fat = ((fs_info->c.blk_num >= fs_info->vol.fat_loc) && 
                  (fs_info->c.blk_num < fs_info->vol.rdir_loc));

    if (fs_info->c.modified)
    {
        if (sec_of_fat && !fs_info->vol.mirror)
            memcpy(fs_info->sec_buf, fs_info->c.buf->buffer, fs_info->vol.bps);
            
        sc = rtems_bdbuf_release_modified(fs_info->c.buf);
        if (sc != RTEMS_SUCCESSFUL)
            set_errno_and_return_minus_one(EIO);
        fs_info->c.modified = 0;    
          
        if (sec_of_fat && !fs_info->vol.mirror)
        {
            bdbuf_buffer *b;
            
            for (i = 1; i < fs_info->vol.fats; i++)
            {
                sc = rtems_bdbuf_get(fs_info->vol.dev,
                                     fs_info->c.blk_num + 
                                     fs_info->vol.fat_length * i, 
                                     &b);
                if ( sc != RTEMS_SUCCESSFUL)
                    set_errno_and_return_minus_one(ENOMEM);
                memcpy(b->buffer, fs_info->sec_buf, fs_info->vol.bps);
                sc = rtems_bdbuf_release_modified(b);
                if ( sc != RTEMS_SUCCESSFUL)
                    set_errno_and_return_minus_one(ENOMEM);
            }
        }    
    }
    else
    {
        sc = rtems_bdbuf_release(fs_info->c.buf);
        if (sc != RTEMS_SUCCESSFUL)
            set_errno_and_return_minus_one(EIO);
    }         
    fs_info->c.state = FAT_CACHE_EMPTY;
    return RC_OK;
}

static inline void
fat_buf_mark_modified(fat_fs_info_t *fs_info)
{
    fs_info->c.modified = TRUE;
}



ssize_t
_fat_block_read(rtems_filesystem_mount_table_entry_t *mt_entry,
                unsigned32                            start,
                unsigned32                            offset,
                unsigned32                            count,
                void                                 *buff);

ssize_t
_fat_block_write(rtems_filesystem_mount_table_entry_t *mt_entry,
                 unsigned32                            start,
                 unsigned32                            offset,
                 unsigned32                            count,
                 const void                           *buff);

ssize_t
fat_cluster_read(rtems_filesystem_mount_table_entry_t *mt_entry,
                  unsigned32                           cln,
                  void                                *buff);

ssize_t
fat_cluster_write(rtems_filesystem_mount_table_entry_t *mt_entry,
                   unsigned32                           cln,
                   const void                          *buff);

int
fat_init_volume_info(rtems_filesystem_mount_table_entry_t *mt_entry);

int
fat_init_clusters_chain(rtems_filesystem_mount_table_entry_t *mt_entry,
                        unsigned32                            start_cln);

unsigned32
fat_cluster_num_to_sector_num(rtems_filesystem_mount_table_entry_t *mt_entry,
                              unsigned32                            cln);

int
fat_shutdown_drive(rtems_filesystem_mount_table_entry_t *mt_entry);


unsigned32
fat_get_unique_ino(rtems_filesystem_mount_table_entry_t *mt_entry);
                                                           
rtems_boolean
fat_ino_is_unique(rtems_filesystem_mount_table_entry_t *mt_entry,
                  unsigned32                            ino);

void
fat_free_unique_ino(rtems_filesystem_mount_table_entry_t *mt_entry,
                    unsigned32                            ino);

int
fat_fat32_update_fsinfo_sector(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  unsigned32                            free_count,
  unsigned32                            next_free
  );
                       
#ifdef __cplusplus
}
#endif
                                            
#endif /* __DOSFS_FAT_H__ */
