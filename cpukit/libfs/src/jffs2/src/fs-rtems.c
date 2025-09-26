#include "rtems-jffs2-config.h"

/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2003 Free Software Foundation, Inc.
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
 *
 * Created by Dominic Ostrowski <dominic.ostrowski@3glab.com>
 * Contributors: David Woodhouse, Nick Garnett, Richard Panton.
 *
 * Port to the RTEMS by embedded brains GmbH & Co. KG
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: fs-ecos.c,v 1.44 2005/07/24 15:29:57 dedekind Exp $
 *
 */

#include <linux/kernel.h>
#include "nodelist.h"
#include <linux/pagemap.h>
#include <linux/crc32.h>
#include <linux/mtd/mtd.h>
#include "compr.h"
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/sysinit.h>

/* Ensure that the JFFS2 values are identical to the POSIX defines */

RTEMS_STATIC_ASSERT(DT_DIR == (S_IFDIR >> 12), DT_DIR);
RTEMS_STATIC_ASSERT(DT_LNK == (S_IFLNK >> 12), DT_LNK);
RTEMS_STATIC_ASSERT(DT_REG == (S_IFREG >> 12), DT_REG);

RTEMS_STATIC_ASSERT(00400 == S_IRUSR, S_IRUSR);
RTEMS_STATIC_ASSERT(00200 == S_IWUSR, S_IWUSR);
RTEMS_STATIC_ASSERT(00100 == S_IXUSR, S_IXUSR);
RTEMS_STATIC_ASSERT(00040 == S_IRGRP, S_IRGRP);
RTEMS_STATIC_ASSERT(00020 == S_IWGRP, S_IWGRP);
RTEMS_STATIC_ASSERT(00010 == S_IXGRP, S_IXGRP);
RTEMS_STATIC_ASSERT(00004 == S_IROTH, S_IROTH);
RTEMS_STATIC_ASSERT(00002 == S_IWOTH, S_IWOTH);
RTEMS_STATIC_ASSERT(00001 == S_IXOTH, S_IXOTH);

RTEMS_STATIC_ASSERT(0140000 == S_IFSOCK, S_IFSOCK);
RTEMS_STATIC_ASSERT(0120000 == S_IFLNK, S_IFLNK);
RTEMS_STATIC_ASSERT(0100000 == S_IFREG, S_IFREG);
RTEMS_STATIC_ASSERT(0060000 == S_IFBLK, S_IFBLK);
RTEMS_STATIC_ASSERT(0040000 == S_IFDIR, S_IFDIR);
RTEMS_STATIC_ASSERT(0020000 == S_IFCHR, S_IFCHR);
RTEMS_STATIC_ASSERT(0010000 == S_IFIFO, S_IFIFO);
RTEMS_STATIC_ASSERT(0004000 == S_ISUID, S_ISUID);
RTEMS_STATIC_ASSERT(0002000 == S_ISGID, S_ISGID);
RTEMS_STATIC_ASSERT(0001000 == S_ISVTX, S_ISVTX);

static int jffs2_read_inode (struct _inode *inode);
static void jffs2_clear_inode (struct _inode *inode);

//==========================================================================
// Ref count and nlink management


// FIXME: This seems like real cruft. Wouldn't it be better just to do the
// right thing?
static void icache_evict(struct _inode *root_i, struct _inode *i)
{
	struct _inode *this = root_i, *next;

 restart:
	D2(printk("icache_evict\n"));
	// If this is an absolute search path from the root,
	// remove all cached inodes with i_count of zero (these are only 
	// held where needed for dotdot filepaths)
	while (this) {
		next = this->i_cache_next;
		if (this != i && this->i_count == 0) {
			struct _inode *parent = this->i_parent;
			if (this->i_cache_next)
				this->i_cache_next->i_cache_prev = this->i_cache_prev;
			if (this->i_cache_prev)
				this->i_cache_prev->i_cache_next = this->i_cache_next;
			jffs2_clear_inode(this);
			memset(this, 0x5a, sizeof(*this));
			free(this);
			if (parent && parent != this) {
				parent->i_count--;
				this = root_i;
				goto restart;
			}
		}
		this = next;
	}
}

// -------------------------------------------------------------------------
// jffs2_fo_write()
// Write data to file.
static int jffs2_extend_file (struct _inode *inode, struct jffs2_raw_inode *ri,
		       unsigned long offset)
{
	struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_full_dnode *fn;
	uint32_t alloc_len;
	int ret = 0;

	/* Make new hole frag from old EOF to new page */
	D1(printk(KERN_DEBUG "Writing new hole frag 0x%x-0x%x between current EOF and new page\n",
		  (unsigned int)inode->i_size, offset));

	ret = jffs2_reserve_space(c, sizeof(*ri), &alloc_len, ALLOC_NORMAL, JFFS2_SUMMARY_INODE_SIZE);
	if (ret)
		return ret;

	mutex_lock(&f->sem);

	ri->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri->nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri->totlen = cpu_to_je32(sizeof(*ri));
	ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

	ri->version = cpu_to_je32(++f->highest_version);
	ri->isize = cpu_to_je32(max((uint32_t)inode->i_size, offset));

	ri->offset = cpu_to_je32(inode->i_size);
	ri->dsize = cpu_to_je32(offset - inode->i_size);
	ri->csize = cpu_to_je32(0);
	ri->compr = JFFS2_COMPR_ZERO;
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));
	ri->data_crc = cpu_to_je32(0);
		
	fn = jffs2_write_dnode(c, f, ri, NULL, 0, ALLOC_NORMAL);
	jffs2_complete_reservation(c);
	if (IS_ERR(fn)) {
		ret = PTR_ERR(fn);
		mutex_unlock(&f->sem);
		return ret;
	}
	ret = jffs2_add_full_dnode_to_inode(c, f, fn);
	if (f->metadata) {
		jffs2_mark_node_obsolete(c, f->metadata->raw);
		jffs2_free_full_dnode(f->metadata);
		f->metadata = NULL;
	}
	if (ret) {
		D1(printk(KERN_DEBUG "Eep. add_full_dnode_to_inode() failed in prepare_write, returned %d\n", ret));
		jffs2_mark_node_obsolete(c, fn->raw);
		jffs2_free_full_dnode(fn);
		mutex_unlock(&f->sem);
		return ret;
	}
	inode->i_size = offset;
	mutex_unlock(&f->sem);
	return 0;
}

static int jffs2_do_setattr (struct _inode *inode, struct iattr *iattr)
{
	struct jffs2_full_dnode *old_metadata, *new_metadata;
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
	struct jffs2_raw_inode *ri;
	unsigned char *mdata = NULL;
	int mdatalen = 0;
	unsigned int ivalid;
	uint32_t alloclen;
	int ret;
	int alloc_type = ALLOC_NORMAL;

	jffs2_dbg(1, "%s(): ino #%lu\n", __func__, inode->i_ino);

	/* Special cases - we don't want more than one data node
	   for these types on the medium at any time. So setattr
	   must read the original data associated with the node
	   (i.e. the device numbers or the target name) and write
	   it out again with the appropriate data attached */
	if (S_ISBLK(inode->i_mode) || S_ISCHR(inode->i_mode)) {
		return -EIO;
	} else if (S_ISLNK(inode->i_mode)) {
		mutex_lock(&f->sem);
		mdatalen = f->metadata->size;
		mdata = kmalloc(f->metadata->size, GFP_USER);
		if (!mdata) {
			mutex_unlock(&f->sem);
			return -ENOMEM;
		}
		ret = jffs2_read_dnode(c, f, f->metadata, mdata, 0, mdatalen);
		if (ret) {
			mutex_unlock(&f->sem);
			kfree(mdata);
			return ret;
		}
		mutex_unlock(&f->sem);
		jffs2_dbg(1, "%s(): Writing %d bytes of symlink target\n",
			  __func__, mdatalen);
	}

	ri = jffs2_alloc_raw_inode();
	if (!ri) {
		if (S_ISLNK(inode->i_mode))
			kfree(mdata);
		return -ENOMEM;
	}

	ret = jffs2_reserve_space(c, sizeof(*ri) + mdatalen, &alloclen,
				  ALLOC_NORMAL, JFFS2_SUMMARY_INODE_SIZE);
	if (ret) {
		jffs2_free_raw_inode(ri);
		if (S_ISLNK(inode->i_mode))
			 kfree(mdata);
		return ret;
	}
	mutex_lock(&f->sem);
	ivalid = iattr->ia_valid;

	ri->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri->nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri->totlen = cpu_to_je32(sizeof(*ri) + mdatalen);
	ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

	ri->ino = cpu_to_je32(inode->i_ino);
	ri->version = cpu_to_je32(++f->highest_version);

	ri->uid = cpu_to_je16((ivalid & ATTR_UID)?
		from_kuid(&init_user_ns, iattr->ia_uid):i_uid_read(inode));
	ri->gid = cpu_to_je16((ivalid & ATTR_GID)?
		from_kgid(&init_user_ns, iattr->ia_gid):i_gid_read(inode));

	if (ivalid & ATTR_MODE)
		ri->mode = cpu_to_jemode(iattr->ia_mode);
	else
		ri->mode = cpu_to_jemode(inode->i_mode);


	ri->isize = cpu_to_je32((ivalid & ATTR_SIZE)?iattr->ia_size:inode->i_size);
	ri->atime = cpu_to_je32(I_SEC((ivalid & ATTR_ATIME)?iattr->ia_atime:inode->i_atime));
	ri->mtime = cpu_to_je32(I_SEC((ivalid & ATTR_MTIME)?iattr->ia_mtime:inode->i_mtime));
	ri->ctime = cpu_to_je32(I_SEC((ivalid & ATTR_CTIME)?iattr->ia_ctime:inode->i_ctime));

	ri->offset = cpu_to_je32(0);
	ri->csize = ri->dsize = cpu_to_je32(mdatalen);
	ri->compr = JFFS2_COMPR_NONE;
	if (ivalid & ATTR_SIZE && inode->i_size < iattr->ia_size) {
		/* It's an extension. Make it a hole node */
		ri->compr = JFFS2_COMPR_ZERO;
		ri->dsize = cpu_to_je32(iattr->ia_size - inode->i_size);
		ri->offset = cpu_to_je32(inode->i_size);
	} else if (ivalid & ATTR_SIZE && !iattr->ia_size) {
		/* For truncate-to-zero, treat it as deletion because
		   it'll always be obsoleting all previous nodes */
		alloc_type = ALLOC_DELETION;
	}
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));
	if (mdatalen)
		ri->data_crc = cpu_to_je32(crc32(0, mdata, mdatalen));
	else
		ri->data_crc = cpu_to_je32(0);

	new_metadata = jffs2_write_dnode(c, f, ri, mdata, mdatalen, alloc_type);
	if (S_ISLNK(inode->i_mode))
		kfree(mdata);

	if (IS_ERR(new_metadata)) {
		jffs2_complete_reservation(c);
		jffs2_free_raw_inode(ri);
		mutex_unlock(&f->sem);
		return PTR_ERR(new_metadata);
	}
	/* It worked. Update the inode */
	inode->i_atime = ITIME(je32_to_cpu(ri->atime));
	inode->i_ctime = ITIME(je32_to_cpu(ri->ctime));
	inode->i_mtime = ITIME(je32_to_cpu(ri->mtime));
	inode->i_mode = jemode_to_cpu(ri->mode);
	i_uid_write(inode, je16_to_cpu(ri->uid));
	i_gid_write(inode, je16_to_cpu(ri->gid));


	old_metadata = f->metadata;

	if (ivalid & ATTR_SIZE && inode->i_size > iattr->ia_size)
		jffs2_truncate_fragtree (c, &f->fragtree, iattr->ia_size);

	if (ivalid & ATTR_SIZE && inode->i_size < iattr->ia_size) {
		jffs2_add_full_dnode_to_inode(c, f, new_metadata);
		inode->i_size = iattr->ia_size;
		f->metadata = NULL;
	} else {
		f->metadata = new_metadata;
	}
	if (old_metadata) {
		jffs2_mark_node_obsolete(c, old_metadata->raw);
		jffs2_free_full_dnode(old_metadata);
	}
	jffs2_free_raw_inode(ri);

	mutex_unlock(&f->sem);
	jffs2_complete_reservation(c);

	/* We have to do the truncate_setsize() without f->sem held, since
	   some pages may be locked and waiting for it in readpage().
	   We are protected from a simultaneous write() extending i_size
	   back past iattr->ia_size, because do_truncate() holds the
	   generic inode semaphore. */
	if (ivalid & ATTR_SIZE && inode->i_size > iattr->ia_size) {
		truncate_setsize(inode, iattr->ia_size);
	}	

	return 0;
}

typedef struct {
	struct super_block sb;
	struct jffs2_inode_cache *inode_cache[];
} rtems_jffs2_fs_info;

static void rtems_jffs2_do_lock(struct super_block *sb)
{
	rtems_recursive_mutex_lock(&sb->s_mutex);
}

static void rtems_jffs2_do_unlock(struct super_block *sb)
{
	rtems_recursive_mutex_unlock(&sb->s_mutex);
}

static void rtems_jffs2_free_directory_entries(struct _inode *inode)
{
        struct jffs2_full_dirent *current = inode->jffs2_i.dents;

	while (current != NULL) {
		struct jffs2_full_dirent *victim = current;

		current = victim->next;
		jffs2_free_full_dirent(victim);
	}
}

static void rtems_jffs2_flash_control_destroy(rtems_jffs2_flash_control *fc)
{
	if (fc->destroy != NULL) {
		(*fc->destroy)(fc);
	}
}
static void rtems_jffs2_compressor_control_destroy(rtems_jffs2_compressor_control *cc)
{
	if (cc != NULL && cc->destroy != NULL) {
		(*cc->destroy)(cc);
	}
}


static void rtems_jffs2_free_fs_info(rtems_jffs2_fs_info *fs_info, bool do_mount_fs_was_successful)
{
	struct super_block *sb = &fs_info->sb;
	struct jffs2_sb_info *c = JFFS2_SB_INFO(sb);

	if (do_mount_fs_was_successful) {
		jffs2_free_ino_caches(c);
		jffs2_free_raw_node_refs(c);
		free(c->blocks);
	}

	rtems_jffs2_flash_control_destroy(fs_info->sb.s_flash_control);
	rtems_jffs2_compressor_control_destroy(fs_info->sb.s_compressor_control);
	rtems_recursive_mutex_destroy(&sb->s_mutex);
	free(fs_info);
}

static int rtems_jffs2_eno_to_rv_and_errno(int eno)
{
	if (eno == 0) {
		return 0;
	} else {
		errno = eno;

		return -1;
	}
}

static struct _inode *rtems_jffs2_get_inode_by_location(
	const rtems_filesystem_location_info_t *loc
)
{
	return loc->node_access;
}

static struct _inode *rtems_jffs2_get_inode_by_iop(
	const rtems_libio_t *iop
)
{
	return iop->pathinfo.node_access;
}

static int rtems_jffs2_fstat(
	const rtems_filesystem_location_info_t *loc,
	struct stat *buf
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct super_block *sb = inode->i_sb;
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	rtems_jffs2_do_lock(sb);

	buf->st_dev = fc->device_identifier;
	buf->st_blksize = PAGE_SIZE;
	buf->st_mode = inode->i_mode;
	buf->st_ino = inode->i_ino;
	buf->st_nlink = inode->i_nlink;
	buf->st_uid = inode->i_uid;
	buf->st_gid = inode->i_gid;
	buf->st_size = inode->i_size;
	buf->st_atime = inode->i_atime;
	buf->st_mtime = inode->i_mtime;
	buf->st_ctime = inode->i_ctime;

	rtems_jffs2_do_unlock(sb);

	return 0;
}

static int rtems_jffs2_fill_dirent(struct dirent *de, off_t off, uint32_t ino, const char *name, unsigned char type)
{
	int eno = 0;
	size_t len;

	memset(de, 0, sizeof(*de));

	de->d_off = off * sizeof(*de);
	de->d_reclen = sizeof(*de);
	de->d_ino = ino;
#ifdef RTEMS_JFFS2_HAVE_D_TYPE
	de->d_type = type;
#endif

	len = strlen(name);
	de->d_namlen = len;

	if (len < sizeof(de->d_name) - 1) {
		memcpy(&de->d_name[0], name, len);
	} else {
		eno = EOVERFLOW;
	}

	return eno;
}

static ssize_t rtems_jffs2_dir_read(rtems_libio_t *iop, void *buf, size_t len)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_iop(iop);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct dirent *de = buf;
	off_t fd_off = 2;
	int eno = 0;
	struct jffs2_full_dirent *fd;
	off_t begin;
	off_t end;
	off_t off;

	rtems_jffs2_do_lock(inode->i_sb);

	fd = f->dents;
	begin = iop->offset;
	end = begin + len / sizeof(*de);
	off = begin;

	if (off == 0 && off < end) {
		eno = rtems_jffs2_fill_dirent(de, off, inode->i_ino, ".", DT_DIR);
		assert(eno == 0);
		++off;
		++de;
	}

	if (off == 1 && off < end) {
		eno = rtems_jffs2_fill_dirent(de, off, inode->i_parent->i_ino, "..", DT_DIR);
		assert(eno == 0);
		++off;
		++de;
	}

	while (eno == 0 && off < end && fd != NULL) {
		if (fd->ino != 0) {
			if (off == fd_off) {
				eno = rtems_jffs2_fill_dirent(de, off, fd->ino, fd->name, fd->type);
				++off;
				++de;
			}

			++fd_off;
		}

		fd = fd->next;
	}

	rtems_jffs2_do_unlock(inode->i_sb);

	if (eno == 0) {
		iop->offset = off;

		return (off - begin) * sizeof(*de);
	} else {
		return rtems_jffs2_eno_to_rv_and_errno(eno);
	}
}

static uint32_t rtems_jffs2_count_blocks(const struct list_head *list)
{
	uint32_t count = 0;
	struct jffs2_eraseblock *jeb;

	list_for_each_entry(jeb, list, list) {
		++count;
	}

	return count;
}

static void rtems_jffs2_get_info(
	const struct jffs2_sb_info *c,
	rtems_jffs2_info           *info
)
{
	info->flash_size = c->flash_size;
	info->flash_blocks = c->nr_blocks;
	info->flash_block_size = c->sector_size;
	info->used_size = c->used_size;
	info->dirty_size = c->dirty_size;
	info->wasted_size = c->wasted_size;
	info->free_size = c->free_size;
	info->bad_size = c->bad_size;
	info->clean_blocks = rtems_jffs2_count_blocks(&c->clean_list);
	info->dirty_blocks = rtems_jffs2_count_blocks(&c->very_dirty_list)
		+ rtems_jffs2_count_blocks(&c->dirty_list);
	info->dirty_blocks += c->gcblock != NULL;
	info->erasable_blocks = rtems_jffs2_count_blocks(&c->erasable_list)
		+ rtems_jffs2_count_blocks(&c->erasable_pending_wbuf_list)
		+ rtems_jffs2_count_blocks(&c->erasing_list)
		+ rtems_jffs2_count_blocks(&c->erase_checking_list)
		+ rtems_jffs2_count_blocks(&c->erase_pending_list)
		+ rtems_jffs2_count_blocks(&c->erase_complete_list);
	info->free_blocks = rtems_jffs2_count_blocks(&c->free_list);
	info->free_blocks += c->nextblock != NULL;
	info->bad_blocks = rtems_jffs2_count_blocks(&c->bad_list);
}

static int rtems_jffs2_on_demand_garbage_collection(struct jffs2_sb_info *c)
{
	if (jffs2_thread_should_wake(c)) {
		return -jffs2_garbage_collect_pass(c);
	} else {
		return 0;
	}
}

static int rtems_jffs2_ioctl(
	rtems_libio_t   *iop,
	ioctl_command_t  request,
	void            *buffer
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_iop(iop);
	int eno;

	rtems_jffs2_do_lock(inode->i_sb);

	switch (request) {
		case RTEMS_JFFS2_GET_INFO:
			rtems_jffs2_get_info(&inode->i_sb->jffs2_sb, buffer);
			eno = 0;
			break;
		case RTEMS_JFFS2_ON_DEMAND_GARBAGE_COLLECTION:
			eno = rtems_jffs2_on_demand_garbage_collection(&inode->i_sb->jffs2_sb);
			break;
		case RTEMS_JFFS2_FORCE_GARBAGE_COLLECTION:
			eno = -jffs2_garbage_collect_pass(&inode->i_sb->jffs2_sb);
			if (!eno) {
			  eno = -jffs2_flush_wbuf_pad(&inode->i_sb->jffs2_sb);
			}
			break;
		default:
			eno = EINVAL;
			break;
	}

	rtems_jffs2_do_unlock(inode->i_sb);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static const rtems_filesystem_file_handlers_r rtems_jffs2_directory_handlers = {
	.open_h = rtems_filesystem_default_open,
	.close_h = rtems_filesystem_default_close,
	.read_h = rtems_jffs2_dir_read,
	.write_h = rtems_filesystem_default_write,
	.ioctl_h = rtems_jffs2_ioctl,
	.lseek_h = rtems_filesystem_default_lseek_directory,
	.fstat_h = rtems_jffs2_fstat,
	.ftruncate_h = rtems_filesystem_default_ftruncate_directory,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_filesystem_default_fcntl,
	.kqfilter_h = rtems_filesystem_default_kqfilter,
	.mmap_h = rtems_filesystem_default_mmap,
	.poll_h = rtems_filesystem_default_poll,
	.readv_h = rtems_filesystem_default_readv,
	.writev_h = rtems_filesystem_default_writev
};

static ssize_t rtems_jffs2_file_read(rtems_libio_t *iop, void *buf, size_t len)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_iop(iop);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
	int err = 0;
	off_t pos;

	rtems_jffs2_do_lock(inode->i_sb);

	pos = iop->offset;

	if (pos >= inode->i_size) {
		len = 0;
	} else {
		uint32_t pos_32 = (uint32_t) pos;
		uint32_t max_available = inode->i_size - pos_32;

		if (len > max_available) {
			len = max_available;
		}

		err = jffs2_read_inode_range(c, f, buf, pos_32, len);
	}

	if (err == 0) {
		iop->offset += len;
	}

	rtems_jffs2_do_unlock(inode->i_sb);

	if (err == 0) {
		return (ssize_t) len;
	} else {
		errno = -err;

		return -1;
	}
}

static ssize_t rtems_jffs2_file_write(rtems_libio_t *iop, const void *buf, size_t len)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_iop(iop);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
	struct jffs2_raw_inode ri;
	uint32_t writtenlen;
	off_t pos;
	int eno = 0;

	memset(&ri, 0, sizeof(ri));

	ri.ino = cpu_to_je32(f->inocache->ino);
	ri.mode = cpu_to_jemode(inode->i_mode);
	ri.uid = cpu_to_je16(inode->i_uid);
	ri.gid = cpu_to_je16(inode->i_gid);
	ri.atime = ri.ctime = ri.mtime = cpu_to_je32(get_seconds());

	rtems_jffs2_do_lock(inode->i_sb);

	if (rtems_libio_iop_is_append(iop)) {
		pos = inode->i_size;
	} else {
		pos = iop->offset;
	}

	if (pos > inode->i_size) {
		ri.version = cpu_to_je32(++f->highest_version);
		eno = -jffs2_extend_file(inode, &ri, pos);
	}

	if (eno == 0) {
		ri.isize = cpu_to_je32(inode->i_size);

		eno = -jffs2_write_inode_range(c, f, &ri, (void *) buf, pos, len, &writtenlen);
	}

	if (eno == 0) {
		pos += writtenlen;

		inode->i_mtime = inode->i_ctime = je32_to_cpu(ri.mtime);

		if (pos > inode->i_size) {
			inode->i_size = pos;
		}

		iop->offset = pos;

		if (writtenlen != len) {
			eno = ENOSPC;
		}
	}

	rtems_jffs2_do_unlock(inode->i_sb);

	if (eno == 0) {
		return writtenlen;
	} else {
		errno = eno;

		return -1;
	}
}

static int rtems_jffs2_file_ftruncate(rtems_libio_t *iop, off_t length)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_iop(iop);
	struct iattr iattr;
	int eno;

	iattr.ia_valid = ATTR_SIZE | ATTR_MTIME | ATTR_CTIME;
	iattr.ia_size = length;
	iattr.ia_mtime = get_seconds();
	iattr.ia_ctime = iattr.ia_mtime;

	rtems_jffs2_do_lock(inode->i_sb);

	eno = -jffs2_do_setattr(inode, &iattr);

	rtems_jffs2_do_unlock(inode->i_sb);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static const rtems_filesystem_file_handlers_r rtems_jffs2_file_handlers = {
	.open_h = rtems_filesystem_default_open,
	.close_h = rtems_filesystem_default_close,
	.read_h = rtems_jffs2_file_read,
	.write_h = rtems_jffs2_file_write,
	.ioctl_h = rtems_jffs2_ioctl,
	.lseek_h = rtems_filesystem_default_lseek_file,
	.fstat_h = rtems_jffs2_fstat,
	.ftruncate_h = rtems_jffs2_file_ftruncate,
	.fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
	.fcntl_h = rtems_filesystem_default_fcntl,
	.kqfilter_h = rtems_filesystem_default_kqfilter,
	.mmap_h = rtems_filesystem_default_mmap,
	.poll_h = rtems_filesystem_default_poll,
	.readv_h = rtems_filesystem_default_readv,
	.writev_h = rtems_filesystem_default_writev
};

static const rtems_filesystem_file_handlers_r rtems_jffs2_link_handlers = {
	.open_h = rtems_filesystem_default_open,
	.close_h = rtems_filesystem_default_close,
	.read_h = rtems_filesystem_default_read,
	.write_h = rtems_filesystem_default_write,
	.ioctl_h = rtems_jffs2_ioctl,
	.lseek_h = rtems_filesystem_default_lseek,
	.fstat_h = rtems_jffs2_fstat,
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

static void rtems_jffs2_set_location(rtems_filesystem_location_info_t *loc, struct _inode *inode)
{
	loc->node_access = inode;

	switch (inode->i_mode & S_IFMT) {
		case S_IFREG:
			loc->handlers = &rtems_jffs2_file_handlers;
			break;
		case S_IFDIR:
			loc->handlers = &rtems_jffs2_directory_handlers;
			break;
		case S_IFLNK:
			loc->handlers = &rtems_jffs2_link_handlers;
			break;
		default:
			loc->handlers = &rtems_filesystem_null_handlers;
			break;
	};
}

static bool rtems_jffs2_eval_is_directory(
	rtems_filesystem_eval_path_context_t *ctx,
	void *arg
)
{
	(void) arg;

	rtems_filesystem_location_info_t *currentloc =
		rtems_filesystem_eval_path_get_currentloc(ctx);
	struct _inode *inode = rtems_jffs2_get_inode_by_location(currentloc);

	return S_ISDIR(inode->i_mode);
}

static rtems_filesystem_eval_path_generic_status rtems_jffs2_eval_token(
	rtems_filesystem_eval_path_context_t *ctx,
	void *arg,
	const char *token,
	size_t tokenlen
)
{
	(void) arg;

	rtems_filesystem_eval_path_generic_status status =
		RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
	rtems_filesystem_location_info_t *currentloc =
		rtems_filesystem_eval_path_get_currentloc(ctx);
	struct _inode *dir_i = rtems_jffs2_get_inode_by_location(currentloc);
	bool access_ok = rtems_filesystem_eval_path_check_access(
		ctx,
		RTEMS_FS_PERMS_EXEC,
		dir_i->i_mode,
		dir_i->i_uid,
		dir_i->i_gid
	);

	if (access_ok) {
		struct _inode *entry_i;

		if (rtems_filesystem_is_current_directory(token, tokenlen)) {
			entry_i = dir_i;
			++entry_i->i_count;
		} else if (rtems_filesystem_is_parent_directory(token, tokenlen)) {
			entry_i = dir_i->i_parent;
			++entry_i->i_count;
		} else {
			entry_i = jffs2_lookup(dir_i, token, (int) tokenlen);
		}

		if (IS_ERR(entry_i)) {
			rtems_filesystem_eval_path_error(ctx, PTR_ERR(entry_i));
		} else if (entry_i != NULL) {
			bool terminal = !rtems_filesystem_eval_path_has_path(ctx);
			int eval_flags = rtems_filesystem_eval_path_get_flags(ctx);
			bool follow_sym_link = (eval_flags & RTEMS_FS_FOLLOW_SYM_LINK) != 0;

			rtems_filesystem_eval_path_clear_token(ctx);

			if (S_ISLNK(entry_i->i_mode) && (follow_sym_link || !terminal)) {
				struct jffs2_inode_info *f = JFFS2_INODE_INFO(entry_i);
				const char *target = f->target;

				rtems_filesystem_eval_path_recursive(ctx, target, strlen(target));

				jffs2_iput(entry_i);
			} else {
				if (S_ISDIR(entry_i->i_mode) && entry_i->i_parent == NULL) {
					entry_i->i_parent = dir_i;
					++dir_i->i_count;
				}

				jffs2_iput(dir_i);
				rtems_jffs2_set_location(currentloc, entry_i);

				if (rtems_filesystem_eval_path_has_path(ctx)) {
					status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
				}
			}
		} else {
			status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
		}
	}

	return status;
}

static const rtems_filesystem_eval_path_generic_config rtems_jffs2_eval_config = {
	.is_directory = rtems_jffs2_eval_is_directory,
	.eval_token = rtems_jffs2_eval_token
};

static void rtems_jffs2_lock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
	rtems_jffs2_fs_info *fs_info = mt_entry->fs_info;
	struct super_block *sb = &fs_info->sb;

	rtems_jffs2_do_lock(sb);
}

static void rtems_jffs2_unlock(const rtems_filesystem_mount_table_entry_t *mt_entry)
{
	rtems_jffs2_fs_info *fs_info = mt_entry->fs_info;
	struct super_block *sb = &fs_info->sb;

	rtems_jffs2_do_unlock(sb);
}

static void rtems_jffs2_eval_path(rtems_filesystem_eval_path_context_t *ctx)
{
	rtems_filesystem_eval_path_generic(ctx, NULL, &rtems_jffs2_eval_config);
}

static int rtems_jffs2_link(
	const rtems_filesystem_location_info_t *parentloc,
	const rtems_filesystem_location_info_t *targetloc,
	const char *name,
	size_t namelen
)
{
	struct _inode *old_d_inode = rtems_jffs2_get_inode_by_location(targetloc);
	struct _inode *dir_i = rtems_jffs2_get_inode_by_location(parentloc);
	int eno;

	eno = -jffs2_link(old_d_inode, dir_i, name, namelen);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static bool rtems_jffs2_are_nodes_equal(
	const rtems_filesystem_location_info_t *a,
	const rtems_filesystem_location_info_t *b
)
{
	struct _inode *inode_a = rtems_jffs2_get_inode_by_location(a);
	struct _inode *inode_b = rtems_jffs2_get_inode_by_location(b);

	return inode_a->i_ino == inode_b->i_ino;
}

static int rtems_jffs2_mknod(
	const rtems_filesystem_location_info_t *parentloc,
	const char *name,
	size_t namelen,
	mode_t mode,
	dev_t dev
)
{
	(void) dev;

	struct _inode *dir_i = rtems_jffs2_get_inode_by_location(parentloc);
	int eno;

	switch (mode & S_IFMT) {
		case S_IFDIR:
			eno = -jffs2_mknod(dir_i, name, namelen, mode, NULL, 0);
			break;
		case S_IFREG:
			eno = -jffs2_create(dir_i, name, namelen, mode);
			break;
		default:
			eno = EINVAL;
			break;
	}

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_cache_fd_name(struct _inode *inode, char **name, size_t *namelen)
{
	struct super_block *sb = inode->i_sb;
	char *fd_name = inode->i_fd->name;
	size_t fd_namelen = strlen(fd_name);
	int eno = 0;

	if (fd_namelen <= sizeof(sb->s_name_buf)) {
		*namelen = fd_namelen;
		*name = memcpy(&sb->s_name_buf[0], fd_name, fd_namelen);
	} else {
		eno = ENOMEM;
	}

	return eno;
}

static int rtems_jffs2_rmnod(
	const rtems_filesystem_location_info_t *parentloc,
	const rtems_filesystem_location_info_t *loc
)
{
	struct _inode *dir_i = rtems_jffs2_get_inode_by_location(parentloc);
	struct _inode *entry_i = rtems_jffs2_get_inode_by_location(loc);
	char *name;
	size_t namelen;
	int eno = rtems_jffs2_cache_fd_name(entry_i, &name, &namelen);

	if (eno == 0) {
		switch (dir_i->i_mode & S_IFMT) {
			case S_IFDIR:
				eno = -jffs2_rmdir(dir_i, entry_i, name, namelen);
				break;
			case S_IFREG:
				eno = -jffs2_unlink(dir_i, entry_i, name, namelen);
				break;
			default:
				eno = EINVAL;
				break;
		}
	}

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_fchmod(
	const rtems_filesystem_location_info_t *loc,
	mode_t mode
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct iattr iattr;
	int eno;

	iattr.ia_valid = ATTR_MODE | ATTR_CTIME;
	iattr.ia_mode = mode;
	iattr.ia_ctime = get_seconds();

	eno = -jffs2_do_setattr(inode, &iattr);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_chown(
	const rtems_filesystem_location_info_t *loc,
	uid_t owner,
	gid_t group
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct iattr iattr;
	int eno;

	iattr.ia_valid = ATTR_UID | ATTR_GID | ATTR_CTIME;
	iattr.ia_uid = owner;
	iattr.ia_gid = group;
	iattr.ia_ctime = get_seconds();

	eno = -jffs2_do_setattr(inode, &iattr);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_clonenode(rtems_filesystem_location_info_t *loc)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);

	++inode->i_count;

	return 0;
}

static void rtems_jffs2_freenode(const rtems_filesystem_location_info_t *loc)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);

	jffs2_iput(inode);
}

static void jffs2_remove_delayed_work(struct delayed_work *dwork);

static void rtems_jffs2_fsunmount(rtems_filesystem_mount_table_entry_t *mt_entry)
{
	rtems_jffs2_fs_info *fs_info = mt_entry->fs_info;
	struct _inode *root_i = mt_entry->mt_fs_root->location.node_access;
#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
	struct jffs2_sb_info *c = JFFS2_SB_INFO(&fs_info->sb);

	/* Remove wbuf delayed work */
	jffs2_remove_delayed_work(&c->wbuf_dwork);

	/* Flush any pending writes */
	if (!sb_rdonly(&fs_info->sb)) {
		jffs2_flush_wbuf_gc(c, 0);
		jffs2_flush_wbuf_pad(c);
	}
#endif

	jffs2_sum_exit(c);

	icache_evict(root_i, NULL);
	assert(root_i->i_cache_next == NULL);
	assert(root_i->i_count == 1);
	jffs2_iput(root_i);

	rtems_jffs2_free_directory_entries(root_i);
	free(root_i);

#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
	jffs2_nand_flash_cleanup(c);
	free(c->mtd);
	c->mtd = NULL;
#endif

	rtems_jffs2_free_fs_info(fs_info, true);
}

static int rtems_jffs2_rename(
	const rtems_filesystem_location_info_t *oldparentloc,
	const rtems_filesystem_location_info_t *oldloc,
	const rtems_filesystem_location_info_t *newparentloc,
	const char *name,
	size_t namelen
)
{
	struct _inode *old_dir_i = rtems_jffs2_get_inode_by_location(oldparentloc);
	struct _inode *new_dir_i = rtems_jffs2_get_inode_by_location(newparentloc);
	struct _inode *d_inode = rtems_jffs2_get_inode_by_location(oldloc);
	char *oldname;
	size_t oldnamelen;
	int eno = rtems_jffs2_cache_fd_name(d_inode, &oldname, &oldnamelen);

	if (eno == 0) {
		eno = -jffs2_rename(old_dir_i, d_inode, oldname, oldnamelen, new_dir_i, name, namelen);
	}

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_statvfs(
	const rtems_filesystem_location_info_t *__restrict loc,
	struct statvfs *__restrict buf
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct super_block *sb = inode->i_sb;
	struct jffs2_sb_info *c = JFFS2_SB_INFO(sb);
	unsigned long avail;

	spin_lock(&c->erase_completion_lock);
	avail = c->dirty_size + c->free_size;
	if (avail > c->sector_size * c->resv_blocks_write) {
		avail -= c->sector_size * c->resv_blocks_write;
	} else {
		avail = 0;
	}
	spin_unlock(&c->erase_completion_lock);

	buf->f_bavail = avail >> PAGE_SHIFT;
	buf->f_blocks = c->flash_size >> PAGE_SHIFT;
	buf->f_bsize = 1UL << PAGE_SHIFT;
	buf->f_fsid = JFFS2_SUPER_MAGIC;
	buf->f_namemax = JFFS2_MAX_NAME_LEN;

	buf->f_bfree = buf->f_bavail;
	buf->f_frsize = buf->f_bsize;

	return 0;
}

static int rtems_jffs2_utimens(
	const rtems_filesystem_location_info_t *loc,
	struct timespec times[2]
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct iattr iattr;
	int eno;

	iattr.ia_valid = ATTR_ATIME | ATTR_MTIME | ATTR_CTIME;
	iattr.ia_atime = times[0].tv_sec;
	iattr.ia_mtime = times[1].tv_sec;
	iattr.ia_ctime = get_seconds();

	eno = -jffs2_do_setattr(inode, &iattr);

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static int rtems_jffs2_symlink(
	const rtems_filesystem_location_info_t *parentloc,
	const char *name,
	size_t namelen,
	const char *target
)
{
	struct _inode *dir_i = rtems_jffs2_get_inode_by_location(parentloc);
	int eno;

	eno = -jffs2_mknod(dir_i, name, namelen, S_IFLNK | S_IRWXUGO, target, strlen(target));

	return rtems_jffs2_eno_to_rv_and_errno(eno);
}

static ssize_t rtems_jffs2_readlink(
	const rtems_filesystem_location_info_t *loc,
	char *buf,
	size_t bufsize
)
{
	struct _inode *inode = rtems_jffs2_get_inode_by_location(loc);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	const char *target = f->target;
	ssize_t i;

	for (i = 0; i < (ssize_t) bufsize && target[i] != '\0'; ++i) {
		buf[i] = target[i];
	}

	return i;
}

static const rtems_filesystem_operations_table rtems_jffs2_ops = {
	.lock_h = rtems_jffs2_lock,
	.unlock_h = rtems_jffs2_unlock,
	.eval_path_h = rtems_jffs2_eval_path,
	.link_h = rtems_jffs2_link,
	.are_nodes_equal_h = rtems_jffs2_are_nodes_equal,
	.mknod_h = rtems_jffs2_mknod,
	.rmnod_h = rtems_jffs2_rmnod,
	.fchmod_h = rtems_jffs2_fchmod,
	.chown_h = rtems_jffs2_chown,
	.clonenod_h = rtems_jffs2_clonenode,
	.freenod_h = rtems_jffs2_freenode,
	.mount_h = rtems_filesystem_default_mount,
	.unmount_h = rtems_filesystem_default_unmount,
	.fsunmount_me_h = rtems_jffs2_fsunmount,
	.utimens_h = rtems_jffs2_utimens,
	.symlink_h = rtems_jffs2_symlink,
	.readlink_h = rtems_jffs2_readlink,
	.rename_h = rtems_jffs2_rename,
	.statvfs_h = rtems_jffs2_statvfs
};

static int calculate_inocache_hashsize(uint32_t flash_size)
{
	/*
	 * Pick a inocache hash size based on the size of the medium.
	 * Count how many megabytes we're dealing with, apply a hashsize twice
	 * that size, but rounding down to the usual big powers of 2. And keep
	 * to sensible bounds.
	 */

	int size_mb = flash_size / 1024 / 1024;
	int hashsize = (size_mb * 2) & ~0x3f;

	if (hashsize < INOCACHE_HASHSIZE_MIN)
		return INOCACHE_HASHSIZE_MIN;
	if (hashsize > INOCACHE_HASHSIZE_MAX)
		return INOCACHE_HASHSIZE_MAX;

	return hashsize;
}

/* Chain for holding delayed work */
rtems_chain_control delayed_work_chain;

/* Lock for protecting the delayed work chain */
struct mutex delayed_work_mutex;

/*
 * All delayed work structs are initialized and added to the chain during FS
 * init. Must be called with no locks held
 */
static void add_delayed_work_to_chain(struct delayed_work *work)
{
	/* Initialize delayed work */
	mutex_init(&work->dw_mutex);
	work->pending = false;
	_Chain_Initialize_node(&work->work.node); \
	work->callback = NULL;

	mutex_lock(&delayed_work_mutex);
	rtems_chain_append_unprotected(&delayed_work_chain, &work->work.node);
	mutex_unlock(&delayed_work_mutex);
}

void jffs2_queue_delayed_work(struct delayed_work *work, int delay_ms)
{
	mutex_lock(&work->dw_mutex);
	if (!work->pending) {
		work->execution_time = rtems_clock_get_uptime_nanoseconds();
		work->execution_time += delay_ms*1000000;
		work->pending = true;
	}
	mutex_unlock(&work->dw_mutex);
}

/* Clean up during FS unmount */
static void jffs2_remove_delayed_work(struct delayed_work *dwork)
{
	mutex_lock(&delayed_work_mutex);
	rtems_chain_extract_unprotected(&dwork->work.node);
	mutex_unlock(&delayed_work_mutex);
	/* Don't run pending delayed work, this will happen during unmount */
}

static void process_delayed_work(void)
{
	struct delayed_work* work;
	rtems_chain_node*    node;

	mutex_lock(&delayed_work_mutex);

	if (rtems_chain_is_empty(&delayed_work_chain)) {
		mutex_unlock(&delayed_work_mutex);
		return;
	}

	node = rtems_chain_first(&delayed_work_chain);
	while (!rtems_chain_is_tail(&delayed_work_chain, node)) {
		work = (struct delayed_work*) node;
		node = rtems_chain_next(node);

		if (!work->pending) {
			continue;
		}

		if (rtems_clock_get_uptime_nanoseconds() < work->execution_time) {
			continue;
		}

		mutex_lock(&work->dw_mutex);
		work->pending = false;
		mutex_unlock(&work->dw_mutex);

		rtems_jffs2_do_lock(work->sb);
		work->callback(&work->work);
		rtems_jffs2_do_unlock(work->sb);
	}
	mutex_unlock(&delayed_work_mutex);
}

/* Task for processing delayed work */
static rtems_task delayed_work_task(
  rtems_task_argument unused
)
{
	(void)unused;
	while (1) {
		process_delayed_work();
		usleep(1);
	}
}

rtems_id delayed_work_task_id;

static void jffs2_init_delayed_work_task(void)
{
	/* Initialize chain for delayed work */
	rtems_chain_initialize_empty(&delayed_work_chain);

	/* Initialize lock for delayed work */
	mutex_init(&delayed_work_mutex);

	/* Create task for delayed work */
	rtems_status_code err = rtems_task_create(
		rtems_build_name( 'J', 'F', 'F', 'S' ),
		jffs2_config.delayed_write_priority,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&delayed_work_task_id
	);
	if (err != RTEMS_SUCCESSFUL) {
		printk("JFFS2 delayed work task processor creation failed\n");
	}
}

RTEMS_SYSINIT_ITEM(
  jffs2_init_delayed_work_task,
  RTEMS_SYSINIT_LIBIO,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

int rtems_jffs2_initialize(
	rtems_filesystem_mount_table_entry_t *mt_entry,
	const void *data
)
{
	const rtems_jffs2_mount_data *jffs2_mount_data = data;
	rtems_jffs2_flash_control *fc = jffs2_mount_data->flash_control;
	int inocache_hashsize = calculate_inocache_hashsize(fc->flash_size);
	rtems_jffs2_fs_info *fs_info = calloc(
		1,
		sizeof(*fs_info) + (size_t) inocache_hashsize * sizeof(fs_info->inode_cache[0])
	);
	bool do_mount_fs_was_successful = false;
	struct super_block *sb;
	struct jffs2_sb_info *c = NULL;
	int err;

	if (fs_info != NULL) {
		err = 0;
	} else {
		err = -ENOMEM;
	}


	if (err == 0) {
		sb = &fs_info->sb;
		c = JFFS2_SB_INFO(sb);
#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
		c->wbuf_dwork.sb = sb;
		add_delayed_work_to_chain(&c->wbuf_dwork);
#endif
		spin_lock_init(&c->erase_completion_lock);
		spin_lock_init(&c->inocache_lock);
		c->mtd = NULL;
		rtems_recursive_mutex_init(&sb->s_mutex, RTEMS_FILESYSTEM_TYPE_JFFS2);
	}

	/* Start task for delayed work if it hasn't already been started */
	if (err == 0) {
		err = rtems_task_start( delayed_work_task_id, delayed_work_task, 0 );
		/* Task already running from previous mount */
		if (err == RTEMS_INCORRECT_STATE) {
			err = 0;
		}
	}

	if (err == 0) {
		uint32_t blocks = fc->flash_size / fc->block_size;

		if ((fc->block_size * blocks) != fc->flash_size) {
			fc->flash_size = fc->block_size * blocks;
			pr_info("Flash size not aligned to erasesize, reducing to %dKiB\n",
				fc->flash_size / 1024);
		}

		if (fc->flash_size < 5*fc->block_size) {
			pr_err("Too few erase blocks (%d)\n",
			       fc->flash_size / fc->block_size);
			err = -EINVAL;
		}
	}

	if (err == 0) {
		sb->s_is_readonly = !mt_entry->writeable;
		sb->s_flash_control = fc;
		sb->s_compressor_control = jffs2_mount_data->compressor_control;

#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
		c->mtd = malloc(sizeof(struct mtd_info));
		if (!c->mtd) {
			err = -ENOMEM;
		}
	}

	if (err == 0) {
		c->mtd->oobavail = jffs2_flash_get_oob_size(c);
		c->mtd->oobsize = c->mtd->oobavail;
		c->mtd->size = fc->flash_size;
		c->mtd->erasesize = fc->block_size;
		c->mtd->writesize = fc->write_size;
#endif
		c->inocache_hashsize = inocache_hashsize;
		c->inocache_list = &fs_info->inode_cache[0];
		c->sector_size = fc->block_size;
		c->flash_size = fc->flash_size;
		c->cleanmarker_size = sizeof(struct jffs2_unknown_node);
#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
		err = jffs2_nand_flash_setup(c);
	}

	if (err == 0) {
#endif
		err = jffs2_do_mount_fs(c);
	}

	if (err == 0) {
		do_mount_fs_was_successful = true;

		sb->s_root = jffs2_iget(sb, 1);
		if (IS_ERR(sb->s_root)) {
			err = PTR_ERR(sb->s_root);
		}
	}

	if (err == 0) {
		sb->s_root->i_parent = sb->s_root;

		if (!jffs2_is_readonly(c)) {
			jffs2_erase_pending_blocks(c, 0);
		}

		mt_entry->fs_info = fs_info;
		mt_entry->ops = &rtems_jffs2_ops;
		mt_entry->mt_fs_root->location.node_access = sb->s_root;
		mt_entry->mt_fs_root->location.handlers = &rtems_jffs2_directory_handlers;

		return 0;
	} else {
		if (fs_info != NULL) {
#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
			jffs2_remove_delayed_work(&c->wbuf_dwork);
#endif
			free(c->mtd);
			c->mtd = NULL;
			rtems_jffs2_free_fs_info(fs_info, do_mount_fs_was_successful);
		} else {
			rtems_jffs2_flash_control_destroy(fc);
			rtems_jffs2_compressor_control_destroy(jffs2_mount_data->compressor_control);
		}

		errno = -err;

		return -1;
	}
}

//==========================================================================
// 
// Called by JFFS2
// ===============
// 
//
//==========================================================================

static struct _inode *new_inode(struct super_block *sb)
{

	// Only called in write.c jffs2_new_inode
	// Always adds itself to inode cache

	struct _inode *inode;
	struct _inode *cached_inode;

	inode = malloc(sizeof (struct _inode));
	if (inode == NULL)
		return 0;

	D2(printk
	   ("malloc new_inode %x ####################################\n",
	    inode));

	memset(inode, 0, sizeof (struct _inode));
	inode->i_sb = sb;
	inode->i_ino = 1;
	inode->i_count = 1;
	inode->i_nlink = 1;	// Let JFFS2 manage the link count
	inode->i_size = 0;

	inode->i_cache_next = NULL;	// Newest inode, about to be cached

	mutex_init(&JFFS2_INODE_INFO(inode)->sem);

	// Add to the icache
	for (cached_inode = sb->s_root; cached_inode != NULL;
	     cached_inode = cached_inode->i_cache_next) {
		if (cached_inode->i_cache_next == NULL) {
			cached_inode->i_cache_next = inode;	// Current last in cache points to newcomer
			inode->i_cache_prev = cached_inode;	// Newcomer points back to last
			break;
		}
	}
	return inode;
}

static struct _inode *ilookup(struct super_block *sb, cyg_uint32 ino)
{
	struct _inode *inode = NULL;

	D2(printk("ilookup\n"));
	// Check for this inode in the cache
	for (inode = sb->s_root; inode != NULL; inode = inode->i_cache_next) {
		if (inode->i_ino == ino) {
			inode->i_count++;
			break;
		}
	}
	return inode;
}

struct _inode *jffs2_iget(struct super_block *sb, cyg_uint32 ino)
{
	// Called in super.c jffs2_read_super, dir.c jffs2_lookup,
	// and gc.c jffs2_garbage_collect_pass

	// Must first check for cached inode 
	// If this fails let new_inode create one

	struct _inode *inode;
	int err;

	D2(printk("jffs2_iget\n"));

	inode = ilookup(sb, ino);
	if (inode)
		return inode;

	// Not cached, so malloc it
	inode = new_inode(sb);
	if (inode == NULL)
		return ERR_PTR(-ENOMEM);

	inode->i_ino = ino;

	err = jffs2_read_inode(inode);
	if (err) {
		printk("jffs2_read_inode() failed\n");
                inode->i_nlink = 0; // free _this_ bad inode right now
		jffs2_iput(inode);
		inode = NULL;
		return ERR_PTR(err);
	}
	return inode;
}

// -------------------------------------------------------------------------
// Decrement the reference count on an inode. If this makes the ref count
// zero, then this inode can be freed.

void jffs2_iput(struct _inode *i)
{
	// Called in jffs2_find 
	// (and jffs2_open and jffs2_ops_mkdir?)
	// super.c jffs2_read_super,
	// and gc.c jffs2_garbage_collect_pass
 recurse:
	assert(i != NULL);

	i->i_count--;

	if (i->i_count < 0)
		BUG();

	if (i->i_count)
                return;
        
	if (!i->i_nlink) {
		struct _inode *parent;

		// Remove from the icache linked list and free immediately
		if (i->i_cache_prev)
			i->i_cache_prev->i_cache_next = i->i_cache_next;
		if (i->i_cache_next)
			i->i_cache_next->i_cache_prev = i->i_cache_prev;

		parent = i->i_parent;
		jffs2_clear_inode(i);
		memset(i, 0x5a, sizeof(*i));
		free(i);

		if (parent && parent != i) {
			i = parent;
			goto recurse;
		}

	} else {
		// Evict some _other_ inode with i_count zero, leaving
		// this latest one in the cache for a while 
		icache_evict(i->i_sb->s_root, i);
	}
}


// -------------------------------------------------------------------------
// EOF jffs2.c


static inline void jffs2_init_inode_info(struct jffs2_inode_info *f)
{
	/* These must be set manually to preserve other members */
	f->highest_version = 0;
	f->fragtree = RTEMS_RB_ROOT;
	f->metadata = NULL;
	f->dents = NULL;
	f->target = NULL;
	f->flags = 0;
	f->usercompr = 0;
}

static void jffs2_clear_inode (struct _inode *inode)
{
        /* We can forget about this inode for now - drop all
         *  the nodelists associated with it, etc.
         */
        struct jffs2_sb_info *c = JFFS2_SB_INFO(inode->i_sb);
        struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);

        D1(printk(KERN_DEBUG "jffs2_clear_inode(): ino #%lu mode %o\n", inode->i_ino, inode->i_mode));

        jffs2_do_clear_inode(c, f);
}


/* jffs2_new_inode: allocate a new inode and inocache, add it to the hash,
   fill in the raw_inode while you're at it. */
struct _inode *jffs2_new_inode (struct _inode *dir_i, int mode, struct jffs2_raw_inode *ri)
{
	struct _inode *inode;
	struct super_block *sb = dir_i->i_sb;
	struct jffs2_sb_info *c;
	struct jffs2_inode_info *f;
	int ret;

	D1(printk(KERN_DEBUG "jffs2_new_inode(): dir_i %ld, mode 0x%x\n", dir_i->i_ino, mode));

	c = JFFS2_SB_INFO(sb);
	
	inode = new_inode(sb);
	
	if (!inode)
		return ERR_PTR(-ENOMEM);

	f = JFFS2_INODE_INFO(inode);
	jffs2_init_inode_info(f);

	memset(ri, 0, sizeof(*ri));
	/* Set OS-specific defaults for new inodes */
	ri->uid = cpu_to_je16(geteuid());
	ri->gid = cpu_to_je16(getegid());
	ri->mode =  cpu_to_jemode(mode);
	ret = jffs2_do_new_inode (c, f, mode, ri);
	if (ret) {
                // forceful evict: f->sem is locked already, and the
                // inode is bad.
                if (inode->i_cache_prev)
                       inode->i_cache_prev->i_cache_next = inode->i_cache_next;
                if (inode->i_cache_next)
                       inode->i_cache_next->i_cache_prev = inode->i_cache_prev; 
                mutex_unlock(&(f->sem));
                jffs2_clear_inode(inode);
                memset(inode, 0x6a, sizeof(*inode));
                free(inode);
                return ERR_PTR(ret);
	}
	inode->i_nlink = 1;
	inode->i_ino = je32_to_cpu(ri->ino);
	inode->i_mode = jemode_to_cpu(ri->mode);
	inode->i_gid = je16_to_cpu(ri->gid);
	inode->i_uid = je16_to_cpu(ri->uid);
	inode->i_atime = inode->i_ctime = inode->i_mtime = get_seconds();
	ri->atime = ri->mtime = ri->ctime = cpu_to_je32(inode->i_mtime);

	inode->i_size = 0;

	return inode;
}


static int jffs2_read_inode (struct _inode *inode)
{
	struct jffs2_inode_info *f;
	struct jffs2_sb_info *c;
	struct jffs2_raw_inode latest_node;
	int ret;

	D1(printk(KERN_DEBUG "jffs2_read_inode(): inode->i_ino == %lu\n", inode->i_ino));

	f = JFFS2_INODE_INFO(inode);
	c = JFFS2_SB_INFO(inode->i_sb);

	jffs2_init_inode_info(f);
	mutex_lock(&f->sem);
	
	ret = jffs2_do_read_inode(c, f, inode->i_ino, &latest_node);

	if (ret) {
		mutex_unlock(&f->sem);
		return ret;
	}
	inode->i_mode = jemode_to_cpu(latest_node.mode);
	inode->i_uid = je16_to_cpu(latest_node.uid);
	inode->i_gid = je16_to_cpu(latest_node.gid);
	inode->i_size = je32_to_cpu(latest_node.isize);
	inode->i_atime = je32_to_cpu(latest_node.atime);
	inode->i_mtime = je32_to_cpu(latest_node.mtime);
	inode->i_ctime = je32_to_cpu(latest_node.ctime);

	inode->i_nlink = f->inocache->pino_nlink;
	mutex_unlock(&f->sem);

	D1(printk(KERN_DEBUG "jffs2_read_inode() returning\n"));
	return 0;
}


void jffs2_gc_release_inode(struct jffs2_sb_info *c,
				   struct jffs2_inode_info *f)
{
	(void) c;

	jffs2_iput(OFNI_EDONI_2SFFJ(f));
}

struct jffs2_inode_info *jffs2_gc_fetch_inode(struct jffs2_sb_info *c,
					      int inum, int unlinked)
{
	struct _inode *inode;
	struct jffs2_inode_cache *ic;

	if (unlinked) {
		/* The inode has zero nlink but its nodes weren't yet marked
		   obsolete. This has to be because we're still waiting for
		   the final (close() and) iput() to happen.

		   There's a possibility that the final iput() could have
		   happened while we were contemplating. In order to ensure
		   that we don't cause a new read_inode() (which would fail)
		   for the inode in question, we use ilookup() in this case
		   instead of iget().

		   The nlink can't _become_ zero at this point because we're
		   holding the alloc_sem, and jffs2_do_unlink() would also
		   need that while decrementing nlink on any inode.
		*/
		inode = ilookup(OFNI_BS_2SFFJ(c), inum);
		if (!inode) {
			jffs2_dbg(1, "ilookup() failed for ino #%u; inode is probably deleted.\n",
				  inum);

			spin_lock(&c->inocache_lock);
			ic = jffs2_get_ino_cache(c, inum);
			if (!ic) {
				jffs2_dbg(1, "Inode cache for ino #%u is gone\n",
					  inum);
				spin_unlock(&c->inocache_lock);
				return NULL;
			}
			if (ic->state != INO_STATE_CHECKEDABSENT) {
				/* Wait for progress. Don't just loop */
				jffs2_dbg(1, "Waiting for ino #%u in state %d\n",
					  ic->ino, ic->state);
				sleep_on_spinunlock(&c->inocache_wq, &c->inocache_lock);
			} else {
				spin_unlock(&c->inocache_lock);
			}

			return NULL;
		}
	} else {
		/* Inode has links to it still; they're not going away because
		   jffs2_do_unlink() would need the alloc_sem and we have it.
		   Just iget() it, and if read_inode() is necessary that's OK.
		*/
		inode = jffs2_iget(OFNI_BS_2SFFJ(c), inum);
		if (IS_ERR(inode))
			return ERR_CAST(inode);
	}

	return JFFS2_INODE_INFO(inode);
}
