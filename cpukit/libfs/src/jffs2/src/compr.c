/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 * Copyright © 2004 Ferenc Havasi <havasi@inf.u-szeged.hu>,
 *		    University of Szeged, Hungary
 * Copyright © 2013 embedded brains GmbH <rtems@embedded-brains.de>
 *
 * Created by Arjan van de Ven <arjan@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "compr.h"

/* jffs2_compress:
 * @data_in: Pointer to uncompressed data
 * @cpage_out: Pointer to returned pointer to buffer for compressed data
 * @datalen: On entry, holds the amount of data available for compression.
 *	On exit, expected to hold the amount of data actually compressed.
 * @cdatalen: On entry, holds the amount of space available for compressed
 *	data. On exit, expected to hold the actual size of the compressed
 *	data.
 *
 * Returns: Lower byte to be stored with data indicating compression type used.
 * Zero is used to show that the data could not be compressed - the
 * compressed version was actually larger than the original.
 * Upper byte will be used later. (soon)
 *
 * If the cdata buffer isn't large enough to hold all the uncompressed data,
 * jffs2_compress should compress as much as will fit, and should set
 * *datalen accordingly to show the amount of data which were compressed.
 */
uint16_t jffs2_compress(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
			unsigned char *data_in, unsigned char **cpage_out,
			uint32_t *datalen, uint32_t *cdatalen)
{
	struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_compressor_control *cc = sb->s_compressor_control;
	int ret;

	if (cc != NULL) {
		*cpage_out = &cc->buffer[0];
		ret = (*cc->compress)(cc, data_in, *cpage_out, datalen, cdatalen);
	} else {
		ret = JFFS2_COMPR_NONE;
	}

	if (ret == JFFS2_COMPR_NONE) {
		*cpage_out = data_in;
		*datalen = *cdatalen;
	}
	return ret;
}

int jffs2_decompress(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
		     uint16_t comprtype, unsigned char *cdata_in,
		     unsigned char *data_out, uint32_t cdatalen, uint32_t datalen)
{
	struct super_block *sb = OFNI_BS_2SFFJ(c);
	rtems_jffs2_compressor_control *cc = sb->s_compressor_control;

	/* Older code had a bug where it would write non-zero 'usercompr'
	   fields. Deal with it. */
	if ((comprtype & 0xff) <= JFFS2_COMPR_ZLIB)
		comprtype &= 0xff;

	switch (comprtype & 0xff) {
	case JFFS2_COMPR_NONE:
		/* This should be special-cased elsewhere, but we might as well deal with it */
		memcpy(data_out, cdata_in, datalen);
		break;
	case JFFS2_COMPR_ZERO:
		memset(data_out, 0, datalen);
		break;
	default:
		if (cc != NULL) {
			return (*cc->decompress)(cc, comprtype, cdata_in, data_out, cdatalen, datalen);
		} else {
			return -EIO;
		}
	}
	return 0;
}
