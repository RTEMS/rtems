#ifndef __LINUX_MTD_MTD_H__
#define __LINUX_MTD_MTD_H__

#include <linux/slab.h>
#include <errno.h>

#define MTD_FAIL_ADDR_UNKNOWN -1LL

struct mtd_info {
  uint32_t oobavail;
  uint32_t oobsize;
  uint32_t size;
  uint32_t erasesize;
  uint32_t writesize;
};

static inline int do_mtd_point(size_t *retlen, void **ebuf)
{
	*retlen = 0;
	*ebuf = NULL;

	return -EOPNOTSUPP;
}

#define mtd_point(a, b, c, d, e, f) do_mtd_point(d, e)

#define mtd_unpoint(a, b, c) do { } while (0)

#define mtd_kmalloc_up_to(a, b) kmalloc(*(b), GFP_KERNEL)

struct mtd_oob_ops {
  uint32_t mode;
  size_t len;
  size_t retlen;
  size_t ooblen;
  size_t oobretlen;
  uint32_t ooboffs;
  uint8_t *datbuf;
  uint8_t *oobbuf;
};

#define EUCLEAN EAGAIN
static inline int mtd_is_bitflip(int err) { return (err == -EUCLEAN); }

#define mtd_block_isbad(mtd_sp, offset) ({ \
  bool bad; \
  int sc = jffs2_flash_block_is_bad(RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd), offset, &bad); \
  if (sc) { \
    return sc; \
  } \
  bad; \
})
#define mtd_block_markbad(mtd_sp, offset) \
  jffs2_flash_block_mark_bad(RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd), offset)
#define mtd_write(mtd_sp, write_buffer_offset, size, return_size, write_buffer) \
({ \
  struct jffs2_sb_info *_c = RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd); \
  const struct super_block *_sb = OFNI_BS_2SFFJ(_c); \
  rtems_jffs2_flash_control *_fc = _sb->s_flash_control; \
  *return_size = size; \
  (*_fc->write)(_fc, write_buffer_offset, write_buffer, size); \
})
#define mtd_writev(mtd_sp, vecs, count, to, retlen) \
({ \
  size_t _totlen = 0; \
  size_t _thislen; \
  int _ret = 0; \
  for (int _i = 0; _i < count; _i++) { \
    _ret = mtd_write(c->mtd, to, vecs[_i].iov_len, &_thislen, \
          vecs[_i].iov_base); \
    _totlen += _thislen; \
    if (_ret || _thislen != vecs[_i].iov_len) { \
      break; \
    } \
    to += vecs[_i].iov_len; \
  } \
  if (retlen) { \
    *retlen = _totlen; \
  } \
  _ret; \
})
#define mtd_read(mtd_sp, ofs, len, retlen, buf) \
  jffs2_flash_direct_read(RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd), ofs, len, retlen, buf)

#endif /* __LINUX_MTD_MTD_H__ */
