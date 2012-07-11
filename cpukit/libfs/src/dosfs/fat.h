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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __DOSFS_FAT_H__
#define __DOSFS_FAT_H__

#include <string.h>

#include <rtems/seterr.h>

#include <rtems/score/cpu.h>
#include <errno.h>
#include <rtems/bdbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RC_OK
#define RC_OK 0
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
#    define CF_LE_W(v) CPU_swap_u16((uint16_t)(v))
#    define CF_LE_L(v) CPU_swap_u32((uint32_t)(v))
#    define CT_LE_W(v) CPU_swap_u16((uint16_t)(v))
#    define CT_LE_L(v) CPU_swap_u32((uint32_t)(v))
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

#define FAT_UNDEFINED_VALUE     (uint32_t)0xFFFFFFFF

#define FAT_FAT12_EOC          0x0FF8
#define FAT_FAT16_EOC          0xFFF8
#define FAT_FAT32_EOC          (uint32_t)0x0FFFFFF8

#define FAT_FAT12_FREE         0x0000
#define FAT_FAT16_FREE         0x0000
#define FAT_FAT32_FREE         0x00000000

#define FAT_GENFAT_EOC         (uint32_t)0xFFFFFFFF
#define FAT_GENFAT_FREE        (uint32_t)0x00000000

#define FAT_FAT12_SHIFT        0x04

#define FAT_FAT12_MASK         0x00000FFF
#define FAT_FAT16_MASK         0x0000FFFF
#define FAT_FAT32_MASK         (uint32_t)0x0FFFFFFF

#define FAT_MAX_BPB_SIZE       90
#define FAT_TOTAL_MBR_SIZE    512

/* size of useful information in FSInfo sector */
#define FAT_USEFUL_INFO_SIZE   12

#define FAT_GET_ADDR(x, ofs)       ((uint8_t *)(x) + (ofs))

#define FAT_GET_VAL8(x, ofs)       (uint8_t)(*((uint8_t *)(x) + (ofs)))

#define FAT_GET_VAL16(x, ofs)                               \
    (uint16_t)( (*((uint8_t *)(x) + (ofs))) |           \
                  ((*((uint8_t *)(x) + (ofs) + 1)) << 8) )

#define FAT_GET_VAL32(x, ofs)                                             \
    (uint32_t)( (uint32_t)(*((uint8_t *)(x) + (ofs))) |             \
                  ((uint32_t)(*((uint8_t *)(x) + (ofs) + 1)) << 8)  | \
                  ((uint32_t)(*((uint8_t *)(x) + (ofs) + 2)) << 16) | \
                  ((uint32_t)(*((uint8_t *)(x) + (ofs) + 3)) << 24) )

#define FAT_SET_VAL8(x, ofs,val)                    \
                 (*((uint8_t *)(x)+(ofs))=(uint8_t)(val))

#define FAT_SET_VAL16(x, ofs,val) do {              \
                 FAT_SET_VAL8((x),(ofs),(val));     \
                 FAT_SET_VAL8((x),(ofs)+1,(val)>>8);\
                 } while (0)

#define FAT_SET_VAL32(x, ofs,val) do {               \
                 uint32_t val1 = val;                \
                 FAT_SET_VAL16((x),(ofs),(val1)&0xffff);\
                 FAT_SET_VAL16((x),(ofs)+2,(val1)>>16);\
                 } while (0)

/* macros to access boot sector fields */
#define FAT_GET_BR_JMPBOOT(x)                FAT_GET_VAL8( x,  0)
#define FAT_SET_BR_JMPBOOT(x,val)            FAT_SET_VAL8( x,  0,val)

#define FAT_GET_ADDR_BR_OEMNAME(x)           FAT_GET_ADDR( x,  3)
#define FAT_BR_OEMNAME_SIZE              (8)

#define FAT_GET_BR_BYTES_PER_SECTOR(x)       FAT_GET_VAL16(x, 11)
#define FAT_SET_BR_BYTES_PER_SECTOR(x,val)   FAT_SET_VAL16(x, 11,val)

#define FAT_GET_BR_SECTORS_PER_CLUSTER(x)    FAT_GET_VAL8( x, 13)
#define FAT_SET_BR_SECTORS_PER_CLUSTER(x,val)FAT_SET_VAL8( x, 13,val)

#define FAT_GET_BR_RESERVED_SECTORS_NUM(x)   FAT_GET_VAL16(x, 14)
#define FAT_SET_BR_RESERVED_SECTORS_NUM(x,val) FAT_SET_VAL16(x, 14,val)

#define FAT_GET_BR_FAT_NUM(x)                FAT_GET_VAL8( x, 16)
#define FAT_SET_BR_FAT_NUM(x,val)            FAT_SET_VAL8( x, 16,val)

#define FAT_GET_BR_FILES_PER_ROOT_DIR(x)     FAT_GET_VAL16(x, 17)
#define FAT_SET_BR_FILES_PER_ROOT_DIR(x,val) FAT_SET_VAL16(x, 17,val)

#define FAT_GET_BR_TOTAL_SECTORS_NUM16(x)    FAT_GET_VAL16(x, 19)
#define FAT_SET_BR_TOTAL_SECTORS_NUM16(x,val)FAT_SET_VAL16(x, 19,val)

#define FAT_GET_BR_MEDIA(x)                  FAT_GET_VAL8( x, 21)
#define FAT_SET_BR_MEDIA(x,val)              FAT_SET_VAL8( x, 21,val)

#define FAT_GET_BR_SECTORS_PER_FAT(x)        FAT_GET_VAL16(x, 22)
#define FAT_SET_BR_SECTORS_PER_FAT(x,val)    FAT_SET_VAL16(x, 22,val)

#define FAT_GET_BR_SECTORS_PER_TRACK(x)      FAT_GET_VAL16(x, 24)
#define FAT_SET_BR_SECTORS_PER_TRACK(x,val)  FAT_SET_VAL16(x, 24,val)

#define FAT_GET_BR_NUMBER_OF_HEADS(x)        FAT_GET_VAL16(x, 26)
#define FAT_SET_BR_NUMBER_OF_HEADS(x,val)    FAT_SET_VAL16(x, 26,val)

#define FAT_GET_BR_HIDDEN_SECTORS(x)         FAT_GET_VAL32(x, 28)
#define FAT_SET_BR_HIDDEN_SECTORS(x,val)     FAT_SET_VAL32(x, 28,val)

#define FAT_GET_BR_TOTAL_SECTORS_NUM32(x)    FAT_GET_VAL32(x, 32)
#define FAT_SET_BR_TOTAL_SECTORS_NUM32(x,val) FAT_SET_VAL32(x, 32,val)
  /* --- start of FAT12/16 specific fields */
#define FAT_GET_BR_DRVNUM(x)                 FAT_GET_VAL8( x, 36)
#define FAT_SET_BR_DRVNUM(x,val)             FAT_SET_VAL8( x, 36,val)

#define FAT_GET_BR_RSVD1(x)                  FAT_GET_VAL8( x, 37)
#define FAT_SET_BR_RSVD1(x,val)              FAT_SET_VAL8( x, 37,val)

#define FAT_GET_BR_BOOTSIG(x)                FAT_GET_VAL8( x, 38)
#define FAT_SET_BR_BOOTSIG(x,val)            FAT_SET_VAL8( x, 38,val)
#define FAT_BR_BOOTSIG_VAL               (0x29)

#define FAT_GET_BR_VOLID(x)                  FAT_GET_VAL32(x, 39)
#define FAT_SET_BR_VOLID(x,val)              FAT_SET_VAL32(x, 39,val)

#define FAT_GET_ADDR_BR_VOLLAB(x)            FAT_GET_ADDR (x, 43)
#define FAT_BR_VOLLAB_SIZE               (11)

#define FAT_GET_ADDR_BR_FILSYSTYPE(x)        FAT_GET_ADDR (x, 54)
#define FAT_BR_FILSYSTYPE_SIZE           (8)
  /* --- end of FAT12/16 specific fields */
  /* --- start of FAT32 specific fields */
#define FAT_GET_BR_SECTORS_PER_FAT32(x)      FAT_GET_VAL32(x, 36)
#define FAT_SET_BR_SECTORS_PER_FAT32(x,val)  FAT_SET_VAL32(x, 36,val)

#define FAT_GET_BR_EXT_FLAGS(x)              FAT_GET_VAL16(x, 40)
#define FAT_SET_BR_EXT_FLAGS(x,val)          FAT_SET_VAL16(x, 40,val)

#define FAT_GET_BR_FSVER(x)                  FAT_GET_VAL16(x, 42)
#define FAT_SET_BR_FSVER(x,val)              FAT_SET_VAL16(x, 42,val)

#define FAT_GET_BR_FAT32_ROOT_CLUSTER(x)     FAT_GET_VAL32(x, 44)
#define FAT_SET_BR_FAT32_ROOT_CLUSTER(x,val) FAT_SET_VAL32(x, 44,val)

#define FAT_GET_BR_FAT32_FS_INFO_SECTOR(x)   FAT_GET_VAL16(x, 48)
#define FAT_SET_BR_FAT32_FS_INFO_SECTOR(x,val) FAT_SET_VAL16(x, 48,val)

#define FAT_GET_BR_FAT32_BK_BOOT_SECTOR(x)   FAT_GET_VAL16(x, 50)
#define FAT_SET_BR_FAT32_BK_BOOT_SECTOR(x,val)  FAT_SET_VAL16(x, 50,val)

#define FAT_GET_ADDR_BR_FAT32_RESERVED(x)    FAT_GET_ADDR (x, 52)
#define FAT_BR_FAT32_RESERVED_SIZE       (12)

#define FAT_GET_BR_FAT32_DRVNUM(x)           FAT_GET_VAL8( x, 64)
#define FAT_SET_BR_FAT32_DRVNUM(x,val)       FAT_SET_VAL8( x, 64,val)

#define FAT_GET_BR_FAT32_RSVD1(x)            FAT_GET_VAL8( x, 65)
#define FAT_SET_BR_FAT32_RSVD1(x,val)        FAT_SET_VAL8( x, 65,val)

#define FAT_GET_BR_FAT32_BOOTSIG(x)          FAT_GET_VAL8( x, 66)
#define FAT_SET_BR_FAT32_BOOTSIG(x,val)      FAT_SET_VAL8( x, 66,val)
#define FAT_BR_FAT32_BOOTSIG_VAL         (0x29)

#define FAT_GET_BR_FAT32_VOLID(x)            FAT_GET_VAL32(x, 67)
#define FAT_SET_BR_FAT32_VOLID(x,val)        FAT_SET_VAL32(x, 67,val)

#define FAT_GET_ADDR_BR_FAT32_VOLLAB(x)      FAT_GET_ADDR (x, 71)
#define FAT_BR_FAT32_VOLLAB_SIZE         (11)

#define FAT_GET_ADDR_BR_FAT32_FILSYSTYPE(x)  FAT_GET_ADDR (x, 82)
#define FAT_BR_FAT32_FILSYSTYPE_SIZE     (8)
  /* --- end of FAT32 specific fields */

#define FAT_GET_BR_SIGNATURE(x)              FAT_GET_VAL16(x,510)
#define FAT_SET_BR_SIGNATURE(x,val)          FAT_SET_VAL16(x,510,val)
#define FAT_BR_SIGNATURE_VAL                (0xAA55)

  /*
   * FAT32 FSINFO description
   */
#define FAT_GET_FSINFO_LEAD_SIGNATURE(x)      FAT_GET_VAL32(x,  0)
#define FAT_SET_FSINFO_LEAD_SIGNATURE(x,val)  FAT_SET_VAL32(x,  0,val)
#define FAT_FSINFO_LEAD_SIGNATURE_VALUE   (0x41615252)

#define FAT_GET_FSINFO_STRUC_SIGNATURE(x)     FAT_GET_VAL32(x,484)
#define FAT_SET_FSINFO_STRUC_SIGNATURE(x,val) FAT_SET_VAL32(x,484,val)
#define FAT_FSINFO_STRUC_SIGNATURE_VALUE  (0x61417272)

#define FAT_GET_FSINFO_TRAIL_SIGNATURE(x)     FAT_GET_VAL32(x,508)
#define FAT_SET_FSINFO_TRAIL_SIGNATURE(x,val) FAT_SET_VAL32(x,508,val)
#define FAT_FSINFO_TRAIL_SIGNATURE_VALUE  (0xAA550000)
/*
 * I read FSInfo sector from offset 484 to access the information, so offsets
 * of these fields a relative
 */
#define FAT_GET_FSINFO_FREE_CLUSTER_COUNT(x)      FAT_GET_VAL32(x, 4)
#define FAT_SET_FSINFO_FREE_CLUSTER_COUNT(x,val)  FAT_SET_VAL32(x, 4,val)
#define FAT_GET_FSINFO_NEXT_FREE_CLUSTER(x)       FAT_GET_VAL32(x, 8)
#define FAT_SET_FSINFO_NEXT_FREE_CLUSTER(x,val)   FAT_SET_VAL32(x, 8,val)

#define FAT_FSI_INFO                         484
#define FAT_FSINFO_STRUCT_OFFSET             488
#define FAT_FSINFO_FREE_CLUSTER_COUNT_OFFSET (FAT_FSINFO_STRUCT_OFFSET+0)

#define FAT_FSINFO_NEXT_FREE_CLUSTER_OFFSET  (FAT_FSINFO_STRUCT_OFFSET+4)

#define FAT_RSRVD_CLN                        0x02

#define FAT_FSI_LEADSIG_SIZE                 0x04

#define FAT_TOTAL_FSINFO_SIZE               512

#define MS_BYTES_PER_CLUSTER_LIMIT           0x8000     /* 32K */

#define FAT_BR_EXT_FLAGS_MIRROR              0x0080

#define FAT_BR_EXT_FLAGS_FAT_NUM             0x000F

#define FAT_BR_MEDIA_FIXED                  0xf8

#define FAT_DIRENTRY_SIZE          32

#define FAT_DIRENTRIES_PER_SEC512  16

/*
 * Volume descriptor
 * Description of the volume the FAT filesystem is located on - generally
 * the fields of the structure correspond to Boot Sector and BPB Structure
 * fields
 */
typedef struct fat_vol_s
{
    uint16_t           bps;            /* bytes per sector */
    uint8_t            sec_log2;       /* log2 of bps */
    uint8_t            sec_mul;        /* log2 of 512bts sectors number per sector */
    uint8_t            spc;            /* sectors per cluster */
    uint8_t            spc_log2;       /* log2 of spc */
    uint16_t           bpc;            /* bytes per cluster */
    uint8_t            bpc_log2;       /* log2 of bytes per cluster */
    uint8_t            fats;           /* number of FATs */
    uint8_t            type;           /* FAT type */
    uint32_t           mask;
    uint32_t           eoc_val;
    uint16_t           fat_loc;        /* FAT start */
    uint32_t           fat_length;     /* sectors per FAT */
    uint32_t           rdir_loc;       /* root directory start */
    uint16_t           rdir_entrs;     /* files per root directory */
    uint32_t           rdir_secs;      /* sectors per root directory */
    uint32_t           rdir_size;      /* root directory size in bytes */
    uint32_t           tot_secs;       /* total count of sectors */
    uint32_t           data_fsec;      /* first data sector */
    uint32_t           data_cls;       /* count of data clusters */
    uint32_t           rdir_cl;        /* first cluster of the root directory */
    uint16_t           info_sec;       /* FSInfo Sector Structure location */
    uint32_t           free_cls;       /* last known free clusters count */
    uint32_t           next_cl;        /* next free cluster number */
    uint8_t            mirror;         /* mirroring enabla/disable */
    uint32_t           afat_loc;       /* active FAT location */
    uint8_t            afat;           /* the number of active FAT */
    int                fd;             /* the disk device file descriptor */
    rtems_disk_device *dd;             /* disk device (see libblock) */
    void              *private_data;   /* reserved */
} fat_vol_t;


typedef struct fat_cache_s
{
    uint32_t            blk_num;
    bool                modified;
    uint8_t             state;
    rtems_bdbuf_buffer *buf;
} fat_cache_t;

/*
 * This structure identifies the instance of the filesystem on the FAT
 * ("fat-file") level.
 */
typedef struct fat_fs_info_s
{
    fat_vol_t            vol;           /* volume descriptor */
    rtems_chain_control *vhash;         /* "vhash" of fat-file descriptors */
    rtems_chain_control *rhash;         /* "rhash" of fat-file descriptors */
    char                *uino;          /* array of unique ino numbers */
    uint32_t             index;
    uint32_t             uino_pool_size; /* size */
    uint32_t             uino_base;
    fat_cache_t          c;             /* cache */
    uint8_t             *sec_buf; /* just placeholder for anything */
} fat_fs_info_t;

/*
 * FAT position is a the cluster and the offset into the
 * cluster.
 */
typedef struct fat_pos_s
{
    uint32_t   cln;
    uint32_t   ofs;
} fat_pos_t;

/*
 * If the name we looking for is file we store not only first data cluster
 * number, but and cluster number and offset for directory entry for this
 * name. We also add the LFN start offset so we can delete it the whole
 * file name. We can then use this to delete the file.
 */
typedef struct fat_dir_pos_s
{
    fat_pos_t  sname;
    fat_pos_t  lname;
} fat_dir_pos_t;

/*
 * Set the long name entries to this value for a short file name.
 */
#define FAT_FILE_SHORT_NAME (0xffffffff)

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

static inline void
fat_dir_pos_init(
    fat_dir_pos_t *dir_pos
    )
{
  dir_pos->sname.cln = 0;
  dir_pos->sname.ofs = 0;
  dir_pos->lname.cln = FAT_FILE_SHORT_NAME;
  dir_pos->lname.ofs = FAT_FILE_SHORT_NAME;
}

static inline uint32_t
fat_cluster_num_to_sector_num(
    const fat_fs_info_t *fs_info,
    uint32_t             cln
    )
{
    if ( (cln == 0) && (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)) )
        return fs_info->vol.rdir_loc;

    return (((cln - FAT_RSRVD_CLN) << fs_info->vol.spc_log2) +
            fs_info->vol.data_fsec);
}

static inline uint32_t
fat_cluster_num_to_sector512_num(
    const fat_fs_info_t *fs_info,
    uint32_t             cln
    )
{
    if (cln == 1)
        return 1;

    return (fat_cluster_num_to_sector_num(fs_info, cln) <<
            fs_info->vol.sec_mul);
}

static inline void
fat_buf_mark_modified(fat_fs_info_t *fs_info)
{
    fs_info->c.modified = true;
}

int
fat_buf_access(fat_fs_info_t *fs_info, uint32_t   blk, int op_type,
               rtems_bdbuf_buffer **buf);

int
fat_buf_release(fat_fs_info_t *fs_info);

ssize_t
_fat_block_read(fat_fs_info_t                        *fs_info,
                uint32_t                              start,
                uint32_t                              offset,
                uint32_t                              count,
                void                                 *buff);

ssize_t
_fat_block_write(fat_fs_info_t                        *fs_info,
                 uint32_t                              start,
                 uint32_t                              offset,
                 uint32_t                              count,
                 const void                           *buff);

int
_fat_block_zero(fat_fs_info_t                         *fs_info,
                 uint32_t                              start,
                 uint32_t                              offset,
                 uint32_t                              count);

int
_fat_block_release(fat_fs_info_t *fs_info);

ssize_t
fat_cluster_read(fat_fs_info_t                        *fs_info,
                  uint32_t                             cln,
                  void                                *buff);

ssize_t
fat_cluster_write(fat_fs_info_t                        *fs_info,
                   uint32_t                             cln,
                   const void                          *buff);

int
fat_init_volume_info(fat_fs_info_t *fs_info, const char *device);

int
fat_init_clusters_chain(fat_fs_info_t                        *fs_info,
                        uint32_t                              start_cln);

int
fat_shutdown_drive(fat_fs_info_t *fs_info);


uint32_t
fat_get_unique_ino(fat_fs_info_t *fs_info);

bool
fat_ino_is_unique(fat_fs_info_t                        *fs_info,
                  uint32_t                              ino);

void
fat_free_unique_ino(fat_fs_info_t                        *fs_info,
                    uint32_t                              ino);

int
fat_fat32_update_fsinfo_sector(
  fat_fs_info_t                        *fs_info,
  uint32_t                              free_count,
  uint32_t                              next_free
  );

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_FAT_H__ */
