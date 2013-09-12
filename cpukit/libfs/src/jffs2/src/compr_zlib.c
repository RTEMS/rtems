/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright © 2001-2007 Red Hat, Inc.
 * Copyright © 2004-2010 David Woodhouse <dwmw2@infradead.org>
 *
 * Created by David Woodhouse <dwmw2@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 */

#if !defined(__KERNEL__) && !defined(__ECOS)
#error "The userspace support got too messy and was removed. Update your mkfs.jffs2"
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/zlib.h>
#include <linux/zutil.h>
#include "nodelist.h"
#include "compr.h"

	/* Plan: call deflate() with avail_in == *sourcelen,
		avail_out = *dstlen - 12 and flush == Z_FINISH.
		If it doesn't manage to finish,	call it again with
		avail_in == 0 and avail_out set to the remaining 12
		bytes for it to clean up.
	   Q: Is 12 bytes sufficient?
	*/
#define STREAM_END_SPACE 12

static DEFINE_MUTEX(deflate_mutex);
static DEFINE_MUTEX(inflate_mutex);

static rtems_jffs2_compressor_zlib_control *get_zlib_control(
	rtems_jffs2_compressor_control *super
)
{
	return (rtems_jffs2_compressor_zlib_control *) super;
}

uint16_t rtems_jffs2_compressor_zlib_compress(
	rtems_jffs2_compressor_control *super,
	unsigned char *data_in,
	unsigned char *cpage_out,
	uint32_t *sourcelen,
	uint32_t *dstlen
)
{
	rtems_jffs2_compressor_zlib_control *self = get_zlib_control(super);
	z_stream *def_strm = &self->stream;
	int ret;

	if (*dstlen <= STREAM_END_SPACE)
		return JFFS2_COMPR_NONE;

	mutex_lock(&deflate_mutex);

	if (Z_OK != zlib_deflateInit(def_strm, 3)) {
		pr_warn("deflateInit failed\n");
		mutex_unlock(&deflate_mutex);
		return JFFS2_COMPR_NONE;
	}

	def_strm->next_in = data_in;
	def_strm->total_in = 0;

	def_strm->next_out = cpage_out;
	def_strm->total_out = 0;

	while (def_strm->total_out < *dstlen - STREAM_END_SPACE && def_strm->total_in < *sourcelen) {
		def_strm->avail_out = *dstlen - (def_strm->total_out + STREAM_END_SPACE);
		def_strm->avail_in = min((unsigned)(*sourcelen-def_strm->total_in), def_strm->avail_out);
		jffs2_dbg(1, "calling deflate with avail_in %d, avail_out %d\n",
			  def_strm->avail_in, def_strm->avail_out);
		ret = zlib_deflate(def_strm, Z_PARTIAL_FLUSH);
		jffs2_dbg(1, "deflate returned with avail_in %d, avail_out %d, total_in %ld, total_out %ld\n",
			  def_strm->avail_in, def_strm->avail_out,
			  def_strm->total_in, def_strm->total_out);
		if (ret != Z_OK) {
			jffs2_dbg(1, "deflate in loop returned %d\n", ret);
			zlib_deflateEnd(def_strm);
			mutex_unlock(&deflate_mutex);
			return JFFS2_COMPR_NONE;
		}
	}
	def_strm->avail_out += STREAM_END_SPACE;
	def_strm->avail_in = 0;
	ret = zlib_deflate(def_strm, Z_FINISH);
	zlib_deflateEnd(def_strm);

	if (ret != Z_STREAM_END) {
		jffs2_dbg(1, "final deflate returned %d\n", ret);
		ret = JFFS2_COMPR_NONE;
		goto out;
	}

	if (def_strm->total_out >= def_strm->total_in) {
		jffs2_dbg(1, "zlib compressed %ld bytes into %ld; failing\n",
			  def_strm->total_in, def_strm->total_out);
		ret = JFFS2_COMPR_NONE;
		goto out;
	}

	jffs2_dbg(1, "zlib compressed %ld bytes into %ld\n",
		  def_strm->total_in, def_strm->total_out);

	*dstlen = def_strm->total_out;
	*sourcelen = def_strm->total_in;
	ret = JFFS2_COMPR_ZLIB;
 out:
	mutex_unlock(&deflate_mutex);
	return ret;
}

int rtems_jffs2_compressor_zlib_decompress(
	rtems_jffs2_compressor_control *super,
	uint16_t comprtype,
	unsigned char *data_in,
	unsigned char *cpage_out,
	uint32_t srclen,
	uint32_t destlen
)
{
	rtems_jffs2_compressor_zlib_control *self = get_zlib_control(super);
	z_stream *inf_strm = &self->stream;
	int ret;
	int wbits = MAX_WBITS;

	if (comprtype != JFFS2_COMPR_ZLIB) {
		return -EIO;
	}

	mutex_lock(&inflate_mutex);

	inf_strm->next_in = data_in;
	inf_strm->avail_in = srclen;
	inf_strm->total_in = 0;

	inf_strm->next_out = cpage_out;
	inf_strm->avail_out = destlen;
	inf_strm->total_out = 0;

	/* If it's deflate, and it's got no preset dictionary, then
	   we can tell zlib to skip the adler32 check. */
	if (srclen > 2 && !(data_in[1] & PRESET_DICT) &&
	    ((data_in[0] & 0x0f) == Z_DEFLATED) &&
	    !(((data_in[0]<<8) + data_in[1]) % 31)) {

		jffs2_dbg(2, "inflate skipping adler32\n");
		wbits = -((data_in[0] >> 4) + 8);
		inf_strm->next_in += 2;
		inf_strm->avail_in -= 2;
	} else {
		/* Let this remain D1 for now -- it should never happen */
		jffs2_dbg(1, "inflate not skipping adler32\n");
	}


	if (Z_OK != zlib_inflateInit2(inf_strm, wbits)) {
		pr_warn("inflateInit failed\n");
		mutex_unlock(&inflate_mutex);
		return -EIO;
	}

	while((ret = zlib_inflate(inf_strm, Z_FINISH)) == Z_OK)
		;
	if (ret != Z_STREAM_END) {
		pr_notice("inflate returned %d\n", ret);
	}
	zlib_inflateEnd(inf_strm);
	mutex_unlock(&inflate_mutex);
	return 0;
}
