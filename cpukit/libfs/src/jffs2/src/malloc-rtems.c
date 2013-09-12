/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Free Software Foundation, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: malloc-ecos.c,v 1.4 2003/11/26 15:55:35 dwmw2 Exp $
 *
 */

#include <linux/kernel.h>
#include "nodelist.h"

struct jffs2_full_dirent *jffs2_alloc_full_dirent(int namesize)
{
	return malloc(sizeof(struct jffs2_full_dirent) + namesize);
}

void jffs2_free_full_dirent(struct jffs2_full_dirent *x)
{
	free(x);
}

struct jffs2_full_dnode *jffs2_alloc_full_dnode(void)
{
	return malloc(sizeof(struct jffs2_full_dnode));
}

void jffs2_free_full_dnode(struct jffs2_full_dnode *x)
{
	free(x);
}

struct jffs2_raw_dirent *jffs2_alloc_raw_dirent(void)
{
	return malloc(sizeof(struct jffs2_raw_dirent));
}

void jffs2_free_raw_dirent(struct jffs2_raw_dirent *x)
{
	free(x);
}

struct jffs2_raw_inode *jffs2_alloc_raw_inode(void)
{
	return malloc(sizeof(struct jffs2_raw_inode));
}

void jffs2_free_raw_inode(struct jffs2_raw_inode *x)
{
	free(x);
}

struct jffs2_tmp_dnode_info *jffs2_alloc_tmp_dnode_info(void)
{
	return malloc(sizeof(struct jffs2_tmp_dnode_info));
}

void jffs2_free_tmp_dnode_info(struct jffs2_tmp_dnode_info *x)
{
	free(x);
}

static struct jffs2_raw_node_ref *jffs2_alloc_refblock(void)
{
	struct jffs2_raw_node_ref *ret;

	ret = malloc((REFS_PER_BLOCK + 1) * sizeof(*ret));
	if (ret) {
		int i = 0;
		for (i=0; i < REFS_PER_BLOCK; i++) {
			ret[i].flash_offset = REF_EMPTY_NODE;
			ret[i].next_in_ino = NULL;
		}
		ret[i].flash_offset = REF_LINK_NODE;
		ret[i].next_in_ino = NULL;
	}
	return ret;
}

int jffs2_prealloc_raw_node_refs(struct jffs2_sb_info *c,
				 struct jffs2_eraseblock *jeb, int nr)
{
	struct jffs2_raw_node_ref **p, *ref;
	int i = nr;

	p = &jeb->last_node;
	ref = *p;

	/* If jeb->last_node is really a valid node then skip over it */
	if (ref && ref->flash_offset != REF_EMPTY_NODE)
		ref++;

	while (i) {
		if (!ref) {
			ref = *p = jffs2_alloc_refblock();
			if (!ref)
				return -ENOMEM;
		}
		if (ref->flash_offset == REF_LINK_NODE) {
			p = &ref->next_in_ino;
			ref = *p;
			continue;
		}
		i--;
		ref++;
	}
	jeb->allocated_refs = nr;

	return 0;
}

void jffs2_free_refblock(struct jffs2_raw_node_ref *x)
{
	free(x);
}

struct jffs2_node_frag *jffs2_alloc_node_frag(void)
{
	return malloc(sizeof(struct jffs2_node_frag));
}

void jffs2_free_node_frag(struct jffs2_node_frag *x)
{
	free(x);
}

struct jffs2_inode_cache *jffs2_alloc_inode_cache(void)
{
	struct jffs2_inode_cache *ret = malloc(sizeof(struct jffs2_inode_cache));
	D1(printk(KERN_DEBUG "Allocated inocache at %p\n", ret));
	return ret;
}

void jffs2_free_inode_cache(struct jffs2_inode_cache *x)
{
	D1(printk(KERN_DEBUG "Freeing inocache at %p\n", x));
	free(x);
}

