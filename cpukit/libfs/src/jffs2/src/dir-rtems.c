/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2003 Free Software Foundation, Inc.
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 * Copyright © 2013 embedded brains GmbH <rtems@embedded-brains.de>
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * Port to the RTEMS by embedded brains GmbH.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: dir-ecos.c,v 1.11 2005/02/08 19:36:27 lunn Exp $
 *
 */

#include <linux/kernel.h>
#include <linux/crc32.h>
#include "nodelist.h"

/***********************************************************************/

/* Takes length argument because it can be either NUL-terminated or '/'-terminated */
struct _inode *jffs2_lookup(struct _inode *dir_i, const unsigned char *name, size_t namelen)
{
	struct jffs2_inode_info *dir_f;
	struct jffs2_full_dirent *fd = NULL, *fd_list;
	uint32_t ino = 0;
	uint32_t hash = full_name_hash(name, namelen);
	struct _inode *inode = NULL;

	D1(printk("jffs2_lookup()\n"));

	dir_f = JFFS2_INODE_INFO(dir_i);

	mutex_lock(&dir_f->sem);

	/* NB: The 2.2 backport will need to explicitly check for '.' and '..' here */
	for (fd_list = dir_f->dents; fd_list && fd_list->nhash <= hash; fd_list = fd_list->next) {
		if (fd_list->nhash == hash && 
		    (!fd || fd_list->version > fd->version) &&
		    strlen((char *)fd_list->name) == namelen &&
		    !strncmp((char *)fd_list->name, (char *)name, namelen)) {
			fd = fd_list;
		}
	}
	if (fd)
		ino = fd->ino;
	mutex_unlock(&dir_f->sem);
	if (ino) {
		inode = jffs2_iget(dir_i->i_sb, ino);
		if (IS_ERR(inode)) {
			printk("jffs2_iget() failed for ino #%u\n", ino);
			return inode;
		} else {
			inode->i_fd = fd;
		}
	}

	return inode;
}

/***********************************************************************/



int jffs2_create(struct _inode *dir_i, const char *d_name, size_t d_namelen, int mode)
{
	struct jffs2_raw_inode *ri;
	struct jffs2_inode_info *f, *dir_f;
	struct jffs2_sb_info *c;
	struct _inode *inode;
	int ret;
	struct qstr qstr;

	qstr.name = d_name;
	qstr.len = d_namelen;

	ri = jffs2_alloc_raw_inode();
	if (!ri)
		return -ENOMEM;
	
	c = JFFS2_SB_INFO(dir_i->i_sb);

	D1(printk(KERN_DEBUG "jffs2_create()\n"));

	inode = jffs2_new_inode(dir_i, mode, ri);

	if (IS_ERR(inode)) {
		D1(printk(KERN_DEBUG "jffs2_new_inode() failed\n"));
		jffs2_free_raw_inode(ri);
		return PTR_ERR(inode);
	}

	f = JFFS2_INODE_INFO(inode);
	dir_f = JFFS2_INODE_INFO(dir_i);

	ret = jffs2_do_create(c, dir_f, f, ri, &qstr);

	if (ret) {
		inode->i_nlink = 0;
		jffs2_iput(inode);
		jffs2_free_raw_inode(ri);
		return ret;
	}

	dir_i->i_mtime = dir_i->i_ctime = ITIME(je32_to_cpu(ri->ctime));

	jffs2_free_raw_inode(ri);

	D1(printk(KERN_DEBUG "jffs2_create: Created ino #%lu with mode %o, nlink %d(%d)\n",
		  inode->i_ino, inode->i_mode, inode->i_nlink, f->inocache->pino_nlink));
	jffs2_iput(inode);
	return 0;
}

/***********************************************************************/


int jffs2_unlink(struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name, size_t d_namelen)
{
	struct jffs2_sb_info *c = JFFS2_SB_INFO(dir_i->i_sb);
	struct jffs2_inode_info *dir_f = JFFS2_INODE_INFO(dir_i);
	struct jffs2_inode_info *dead_f = JFFS2_INODE_INFO(d_inode);
	int ret;

	ret = jffs2_do_unlink(c, dir_f, (const char *)d_name,
			       d_namelen, dead_f, get_seconds());
	if (dead_f->inocache)
		d_inode->i_nlink = dead_f->inocache->pino_nlink;
	return ret;
}
/***********************************************************************/


int jffs2_link (struct _inode *old_d_inode, struct _inode *dir_i, const unsigned char *d_name, size_t d_namelen)
{
	struct jffs2_sb_info *c = JFFS2_SB_INFO(old_d_inode->i_sb);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(old_d_inode);
	struct jffs2_inode_info *dir_f = JFFS2_INODE_INFO(dir_i);
	int ret;

	/* XXX: This is ugly */
	uint8_t type = (old_d_inode->i_mode & S_IFMT) >> 12;
	if (!type) type = DT_REG;

	ret = jffs2_do_link(c, dir_f, f->inocache->ino, type, 
                            (const char * )d_name, 
                            d_namelen, get_seconds());

	if (!ret) {
		mutex_lock(&f->sem);
		old_d_inode->i_nlink = ++f->inocache->pino_nlink;
		mutex_unlock(&f->sem);
	}
	return ret;
}

/***********************************************************************/

int jffs2_mknod(
	struct _inode *dir_i,
	const unsigned char *d_name,
	size_t d_namelen,
	int mode,
	const unsigned char *data,
	size_t datalen
)
{
	struct jffs2_inode_info *f, *dir_f;
	struct jffs2_sb_info *c;
	struct _inode *inode;
	struct jffs2_raw_inode *ri;
	struct jffs2_raw_dirent *rd;
	struct jffs2_full_dnode *fn;
	struct jffs2_full_dirent *fd;
	uint32_t alloclen;
	int ret;

	/* FIXME: If you care. We'd need to use frags for the data
	   if it grows much more than this */
	if (datalen > 254)
		return -ENAMETOOLONG;

	ri = jffs2_alloc_raw_inode();

	if (!ri)
		return -ENOMEM;

	c = JFFS2_SB_INFO(dir_i->i_sb);

	/* Try to reserve enough space for both node and dirent.
	 * Just the node will do for now, though
	 */
	ret = jffs2_reserve_space(c, sizeof(*ri) + datalen, &alloclen,
				  ALLOC_NORMAL, JFFS2_SUMMARY_INODE_SIZE);

	if (ret) {
		jffs2_free_raw_inode(ri);
		return ret;
	}

	inode = jffs2_new_inode(dir_i, mode, ri);

	if (IS_ERR(inode)) {
		jffs2_free_raw_inode(ri);
		jffs2_complete_reservation(c);
		return PTR_ERR(inode);
	}

	f = JFFS2_INODE_INFO(inode);

	inode->i_size = datalen;
	ri->isize = ri->dsize = ri->csize = cpu_to_je32(inode->i_size);
	ri->totlen = cpu_to_je32(sizeof(*ri) + inode->i_size);
	ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

	ri->compr = JFFS2_COMPR_NONE;
	ri->data_crc = cpu_to_je32(crc32(0, data, datalen));
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));

	fn = jffs2_write_dnode(c, f, ri, data, datalen, ALLOC_NORMAL);

	jffs2_free_raw_inode(ri);

	if (IS_ERR(fn)) {
		/* Eeek. Wave bye bye */
		mutex_unlock(&f->sem);
		jffs2_complete_reservation(c);
		ret = PTR_ERR(fn);
		goto fail;
	}

	if (S_ISLNK(mode)) {
		/* We use f->target field to store the target path. */
		f->target = kmemdup(data, datalen + 1, GFP_KERNEL);
		if (!f->target) {
			pr_warn("Can't allocate %d bytes of memory\n", datalen + 1);
			mutex_unlock(&f->sem);
			jffs2_complete_reservation(c);
			ret = -ENOMEM;
			goto fail;
		}

		jffs2_dbg(1, "%s(): symlink's target '%s' cached\n",
			  __func__, (char *)f->target);
	}

	/* No data here. Only a metadata node, which will be
	   obsoleted by the first data write
	*/
	f->metadata = fn;
	mutex_unlock(&f->sem);

	jffs2_complete_reservation(c);

	ret = jffs2_reserve_space(c, sizeof(*rd)+d_namelen, &alloclen,
				  ALLOC_NORMAL, JFFS2_SUMMARY_DIRENT_SIZE(d_namelen));
	if (ret)
		goto fail;

	rd = jffs2_alloc_raw_dirent();
	if (!rd) {
		/* Argh. Now we treat it like a normal delete */
		jffs2_complete_reservation(c);
		ret = -ENOMEM;
		goto fail;
	}

	dir_f = JFFS2_INODE_INFO(dir_i);
	mutex_lock(&dir_f->sem);

	rd->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	rd->nodetype = cpu_to_je16(JFFS2_NODETYPE_DIRENT);
	rd->totlen = cpu_to_je32(sizeof(*rd) + d_namelen);
	rd->hdr_crc = cpu_to_je32(crc32(0, rd, sizeof(struct jffs2_unknown_node)-4));

	rd->pino = cpu_to_je32(dir_i->i_ino);
	rd->version = cpu_to_je32(++dir_f->highest_version);
	rd->ino = cpu_to_je32(inode->i_ino);
	rd->mctime = cpu_to_je32(get_seconds());
	rd->nsize = d_namelen;

	/* XXX: This is ugly. */
	rd->type = (mode & S_IFMT) >> 12;

	rd->node_crc = cpu_to_je32(crc32(0, rd, sizeof(*rd)-8));
	rd->name_crc = cpu_to_je32(crc32(0, d_name, d_namelen));

	fd = jffs2_write_dirent(c, dir_f, rd, d_name, d_namelen, ALLOC_NORMAL);

	if (IS_ERR(fd)) {
		/* dirent failed to write. Delete the inode normally
		   as if it were the final unlink() */
		jffs2_complete_reservation(c);
		jffs2_free_raw_dirent(rd);
		mutex_unlock(&dir_f->sem);
		ret = PTR_ERR(fd);
		goto fail;
	}

	dir_i->i_mtime = dir_i->i_ctime = ITIME(je32_to_cpu(rd->mctime));

	jffs2_free_raw_dirent(rd);

	/* Link the fd into the inode's list, obsoleting an old
	   one if necessary. */
	jffs2_add_fd_to_list(c, fd, &dir_f->dents);

	mutex_unlock(&dir_f->sem);
	jffs2_complete_reservation(c);

 fail:
	jffs2_iput(inode);

	return ret;
}

int jffs2_rmdir (struct _inode *dir_i, struct _inode *d_inode, const unsigned char *d_name, size_t d_namelen)
{
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(d_inode);
	struct jffs2_full_dirent *fd;

	for (fd = f->dents ; fd; fd = fd->next) {
		if (fd->ino)
			return -ENOTEMPTY;
	}
	return jffs2_unlink(dir_i, d_inode, d_name, d_namelen);
}

int jffs2_rename (struct _inode *old_dir_i, struct _inode *d_inode, const unsigned char *old_d_name, size_t old_d_namelen,
		  struct _inode *new_dir_i, const unsigned char *new_d_name, size_t new_d_namelen)
{
	int ret;
	struct jffs2_sb_info *c = JFFS2_SB_INFO(old_dir_i->i_sb);
	struct jffs2_inode_info *victim_f = NULL;
	uint8_t type;
	uint32_t now;

#if 0 /* FIXME -- this really doesn't belong in individual file systems. 
	 The fileio code ought to do this for us, or at least part of it */
	if (new_dentry->d_inode) {
		if (S_ISDIR(d_inode->i_mode) && 
		    !S_ISDIR(new_dentry->d_inode->i_mode)) {
			/* Cannot rename directory over non-directory */
			return -EINVAL;
		}

		victim_f = JFFS2_INODE_INFO(new_dentry->d_inode);

		if (S_ISDIR(new_dentry->d_inode->i_mode)) {
			struct jffs2_full_dirent *fd;

			if (!S_ISDIR(d_inode->i_mode)) {
				/* Cannot rename non-directory over directory */
				return -EINVAL;
			}
			mutex_lock(&victim_f->sem);
			for (fd = victim_f->dents; fd; fd = fd->next) {
				if (fd->ino) {
					mutex_unlock(&victim_f->sem);
					return -ENOTEMPTY;
				}
			}
			mutex_unlock(&victim_f->sem);
		}
	}
#endif

	/* XXX: We probably ought to alloc enough space for
	   both nodes at the same time. Writing the new link, 
	   then getting -ENOSPC, is quite bad :)
	*/

	/* Make a hard link */
	
	/* XXX: This is ugly */
	type = (d_inode->i_mode & S_IFMT) >> 12;
	if (!type) type = DT_REG;

	now = get_seconds();
	ret = jffs2_do_link(c, JFFS2_INODE_INFO(new_dir_i),
			    d_inode->i_ino, type,
			    (const char *)new_d_name,
                            new_d_namelen, now);

	if (ret)
		return ret;

	if (victim_f) {
		/* There was a victim. Kill it off nicely */
		/* Don't oops if the victim was a dirent pointing to an
		   inode which didn't exist. */
		if (victim_f->inocache) {
			mutex_lock(&victim_f->sem);
			victim_f->inocache->pino_nlink--;
			mutex_unlock(&victim_f->sem);
		}
	}

	/* Unlink the original */
	ret = jffs2_do_unlink(c, JFFS2_INODE_INFO(old_dir_i),
                              (const char *)old_d_name,
                              old_d_namelen, NULL, now);

	if (ret) {
		/* Oh shit. We really ought to make a single node which can do both atomically */
		struct jffs2_inode_info *f = JFFS2_INODE_INFO(d_inode);
		mutex_lock(&f->sem);
		if (f->inocache)
			d_inode->i_nlink = f->inocache->pino_nlink++;
		mutex_unlock(&f->sem);

		printk(KERN_NOTICE "jffs2_rename(): Link succeeded, unlink failed (err %d). You now have a hard link\n", ret);
	}
	return ret;
}

