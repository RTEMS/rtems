/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2002-2003 Free Software Foundation, Inc.
 * Copyright © 2013 embedded brains GmbH <rtems@embedded-brains.de>
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * Port to the RTEMS by embedded brains GmbH.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: os-ecos.h,v 1.24 2005/02/09 09:23:55 pavlov Exp $
 *
 */

#ifndef __JFFS2_OS_RTEMS_H__
#define __JFFS2_OS_RTEMS_H__

#include <asm/atomic.h>
#include <asm/bug.h>
#include <linux/compiler.h>
#include <linux/list.h>
#include <linux/pagemap.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <rtems/jffs2.h>

#define CONFIG_JFFS2_RTIME

#define CONFIG_JFFS2_ZLIB

struct _inode;
struct super_block;

struct iovec {
        void *iov_base;
        ssize_t iov_len; 
};

static inline unsigned int full_name_hash(const unsigned char * name, size_t len) {

	unsigned hash = 0;
 	while (len--) {
		hash = (hash << 4) | (hash >> 28);
		hash ^= *(name++);
	}
	return hash;
}

/* NAND flash not currently supported on RTEMS */
#define jffs2_can_mark_obsolete(c) (1)

#define JFFS2_INODE_INFO(i) (&(i)->jffs2_i)
#define OFNI_EDONI_2SFFJ(f)  ((struct _inode *) ( ((char *)f) - ((char *)(&((struct _inode *)NULL)->jffs2_i)) ) )

#define ITIME(sec) (sec)
#define I_SEC(tv) (tv)
 
#define JFFS2_F_I_SIZE(f) (OFNI_EDONI_2SFFJ(f)->i_size)
#define JFFS2_F_I_MODE(f) (OFNI_EDONI_2SFFJ(f)->i_mode)
#define JFFS2_F_I_UID(f) (OFNI_EDONI_2SFFJ(f)->i_uid)
#define JFFS2_F_I_GID(f) (OFNI_EDONI_2SFFJ(f)->i_gid)
#define JFFS2_F_I_CTIME(f) (OFNI_EDONI_2SFFJ(f)->i_ctime)
#define JFFS2_F_I_MTIME(f) (OFNI_EDONI_2SFFJ(f)->i_mtime)
#define JFFS2_F_I_ATIME(f) (OFNI_EDONI_2SFFJ(f)->i_atime)

#define get_seconds() time(NULL)

struct _inode {
	cyg_uint32		i_ino;

	int			i_count;
	mode_t			i_mode;
	nlink_t			i_nlink; // Could we dispense with this?
	uid_t			i_uid;
	gid_t			i_gid;
	time_t			i_atime;
	time_t			i_mtime;
	time_t			i_ctime;
//	union {
		unsigned short	i_rdev; // For devices only
		struct _inode *	i_parent; // For directories only
		off_t		i_size; // For files only
//	};
	struct super_block *	i_sb;
	struct jffs2_full_dirent * i_fd;

	struct jffs2_inode_info	jffs2_i;

        struct _inode *		i_cache_prev; // We need doubly-linked?
        struct _inode *		i_cache_next;
};

#define JFFS2_SB_INFO(sb) (&(sb)->jffs2_sb)
#define OFNI_BS_2SFFJ(c)  ((struct super_block *) ( ((char *)c) - ((char *)(&((struct super_block *)NULL)->jffs2_sb)) ) )

struct super_block {
	struct jffs2_sb_info	jffs2_sb;
	struct _inode *		s_root;
	rtems_jffs2_flash_control	*s_flash_control;
	rtems_jffs2_compressor_control	*s_compressor_control;
	bool			s_is_readonly;
	unsigned char		s_gc_buffer[PAGE_CACHE_SIZE]; // Avoids malloc when user may be under memory pressure
	rtems_id		s_mutex;
	char			s_name_buf[JFFS2_MAX_NAME_LEN];
};

#define sleep_on_spinunlock(wq, sl) spin_unlock(sl)
#define EBADFD 32767

static inline bool jffs2_is_readonly(struct jffs2_sb_info *c)
{
	struct super_block *sb = OFNI_BS_2SFFJ(c);

	return sb->s_is_readonly;
}

static inline void jffs2_garbage_collect_trigger(struct jffs2_sb_info *c)
{
       /* We don't have a GC thread in RTEMS (yet) */
}

/* fs-rtems.c */
struct _inode *jffs2_new_inode (struct _inode *dir_i, int mode, struct jffs2_raw_inode *ri);
struct _inode *jffs2_iget(struct super_block *sb, cyg_uint32 ino);
void jffs2_iput(struct _inode * i);
void jffs2_gc_release_inode(struct jffs2_sb_info *c, struct jffs2_inode_info *f);
struct jffs2_inode_info *jffs2_gc_fetch_inode(struct jffs2_sb_info *c, int inum, int nlink);
unsigned char *jffs2_gc_fetch_page(struct jffs2_sb_info *c, struct jffs2_inode_info *f, 
				   unsigned long offset, unsigned long *priv);
void jffs2_gc_release_page(struct jffs2_sb_info *c, unsigned char *pg, unsigned long *priv);

/* Avoid polluting RTEMS namespace with names not starting in jffs2_ */
#define os_to_jffs2_mode(x) jffs2_from_os_mode(x)
static inline uint32_t jffs2_from_os_mode(uint32_t osmode)
{
  return osmode & (S_IFMT | S_IRWXU | S_IRWXG | S_IRWXO);
}

static inline uint32_t jffs2_to_os_mode (uint32_t jmode)
{
  return jmode & (S_IFMT | S_IRWXU | S_IRWXG | S_IRWXO);
}


/* flashio.c */
int jffs2_flash_read(struct jffs2_sb_info *c, cyg_uint32 read_buffer_offset,
			  const size_t size, size_t * return_size, unsigned char * write_buffer);
int jffs2_flash_write(struct jffs2_sb_info *c, cyg_uint32 write_buffer_offset,
			   const size_t size, size_t * return_size, unsigned char * read_buffer);
int jffs2_flash_direct_writev(struct jffs2_sb_info *c, const struct iovec *vecs,
			      unsigned long count, loff_t to, size_t *retlen);
int jffs2_flash_erase(struct jffs2_sb_info *c, struct jffs2_eraseblock *jeb);

// dir-rtems.c
struct _inode *jffs2_lookup(struct _inode *dir_i, const unsigned char *name, size_t namelen);
int jffs2_create(struct _inode *dir_i, const char *d_name, size_t d_namelen, int mode);
int jffs2_mknod(struct _inode *dir_i, const unsigned char *d_name, size_t d_namelen, int mode, const unsigned char *data, size_t datalen);
int jffs2_link (struct _inode *old_d_inode, struct _inode *dir_i, const unsigned char *d_name, size_t d_namelen);
int jffs2_unlink(struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name, size_t d_namelen);
int jffs2_rmdir (struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name, size_t d_namelen);
int jffs2_rename (struct _inode *old_dir_i, struct _inode *d_inode, const unsigned char *old_d_name, size_t old_d_namelen,
		  struct _inode *new_dir_i, const unsigned char *new_d_name, size_t new_d_namelen);

/* erase.c */
static inline void jffs2_erase_pending_trigger(struct jffs2_sb_info *c)
{ }

#ifndef CONFIG_JFFS2_FS_WRITEBUFFER
#define SECTOR_ADDR(x) ( ((unsigned long)(x) & ~(c->sector_size-1)) )
#define jffs2_can_mark_obsolete(c) (1)
#define jffs2_is_writebuffered(c) (0)
#define jffs2_cleanmarker_oob(c) (0)
#define jffs2_write_nand_cleanmarker(c,jeb) (-EIO)

#define jffs2_flush_wbuf_pad(c) (c=c)
#define jffs2_flush_wbuf_gc(c, i) ({ (void)(c), (void) i, 0; })
#define jffs2_nand_read_failcnt(c,jeb) do { ; } while(0)
#define jffs2_write_nand_badblock(c,jeb,p) (0)
#define jffs2_flash_setup(c) (0)
#define jffs2_nand_flash_cleanup(c) do {} while(0)
#define jffs2_wbuf_dirty(c) (0)
#define jffs2_flash_writev(a,b,c,d,e,f) jffs2_flash_direct_writev(a,b,c,d,e)
#define jffs2_wbuf_timeout NULL
#define jffs2_wbuf_process NULL
#define jffs2_nor_ecc(c) (0)
#else
#error no nand yet
#endif

#ifndef BUG_ON
#define BUG_ON(x) do { if (unlikely(x)) BUG(); } while(0)
#endif

#define __init

#endif /* __JFFS2_OS_RTEMS_H__ */
