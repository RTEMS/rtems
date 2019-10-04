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
#define mtd_write(mtd_sp, ofs, len, retlen, buf) \
  jffs2_flash_direct_write(RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd), ofs, len, retlen, buf)
#define mtd_read(mtd_sp, ofs, len, retlen, buf) \
  jffs2_flash_direct_read(RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd), ofs, len, retlen, buf)

#endif /* __LINUX_MTD_MTD_H__ */
