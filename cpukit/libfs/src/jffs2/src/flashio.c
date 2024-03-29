#include "rtems-jffs2-config.h"

/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * Created by Dominic Ostrowski <dominic.ostrowski@3glab.com>
 * Contributors: David Woodhouse, Nick Garnett, Richard Panton.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: flashio.c,v 1.1 2003/11/26 14:09:29 dwmw2 Exp $
 *
 */

#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include "nodelist.h"

#ifndef CONFIG_JFFS2_FS_WRITEBUFFER
int jffs2_flash_read(struct jffs2_sb_info * c,
			  loff_t read_buffer_offset, const size_t size,
			  size_t * return_size, unsigned char *write_buffer)
{
	return jffs2_flash_direct_read(c, read_buffer_offset, size, return_size, write_buffer);
}

int jffs2_flash_write(struct jffs2_sb_info * c,
			   loff_t write_buffer_offset, size_t size,
			   size_t * return_size, const unsigned char *read_buffer)
{
	return jffs2_flash_direct_write(c, write_buffer_offset, size, return_size, read_buffer);
}
#endif
int jffs2_flash_direct_read(struct jffs2_sb_info * c,
			  loff_t read_buffer_offset, const size_t size,
			  size_t * return_size, unsigned char *write_buffer)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	*return_size = size;

	return (*fc->read)(fc, read_buffer_offset, write_buffer, size);
}

int jffs2_flash_erase(struct jffs2_sb_info * c,
			   struct jffs2_eraseblock * jeb)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	return (*fc->erase)(fc, jeb->offset);
}

#ifdef CONFIG_JFFS2_FS_WRITEBUFFER
int jffs2_flash_block_is_bad(struct jffs2_sb_info * c,
			cyg_uint32 block_offset, bool *bad)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	return (*fc->block_is_bad)(fc, block_offset, bad);
}

int jffs2_flash_block_mark_bad(struct jffs2_sb_info * c,
			cyg_uint32 block_offset)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	return (*fc->block_mark_bad)(fc, block_offset);
}

int jffs2_flash_oob_write(struct jffs2_sb_info * c,
			cyg_uint32 block_offset,
			uint8_t *oobbuf,
			uint32_t ooblen)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	return (*fc->oob_write)(fc, block_offset, oobbuf, ooblen);
}

int jffs2_flash_oob_read(struct jffs2_sb_info * c,
			cyg_uint32 block_offset,
			uint8_t *oobbuf,
			uint32_t ooblen)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	return (*fc->oob_read)(fc, block_offset, oobbuf, ooblen);
}

int jffs2_flash_get_oob_size(struct jffs2_sb_info * c)
{
	const struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_flash_control *fc = sb->s_flash_control;

	if (fc->get_oob_size == NULL) {
		return 0;
	}
	return (*fc->get_oob_size)(fc);
}
#endif
