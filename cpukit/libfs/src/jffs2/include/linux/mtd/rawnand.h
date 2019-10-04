#ifndef __LINUX_MTD_RAWNAND_H__
#define __LINUX_MTD_RAWNAND_H__

#define mtd_read_oob(mtd_sp, offset, ops) ({ \
  struct jffs2_sb_info *sb_info = RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd); \
  int sc = jffs2_flash_oob_read(sb_info, offset, (ops)->oobbuf, (ops)->ooblen); \
  if (sc) { \
    sc = -EIO; \
  } else { \
    (ops)->oobretlen = (ops)->ooblen; \
  } \
  sc; \
})
#define mtd_write_oob(mtd_sp, offset, ops) ({ \
  struct jffs2_sb_info *sb_info = RTEMS_CONTAINER_OF(&(mtd_sp), struct jffs2_sb_info, mtd); \
  int sc = jffs2_flash_oob_write(sb_info, offset, (ops)->oobbuf, (ops)->ooblen); \
  if (sc != RTEMS_SUCCESSFUL) { \
    sc = -EIO; \
  } else { \
    (ops)->oobretlen = (ops)->ooblen; \
  } \
  sc; \
})
#define MTD_OPS_AUTO_OOB 1

#endif /* __LINUX_MTD_RAWNAND_H__ */
