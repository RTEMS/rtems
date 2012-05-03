/*  milkymist_tmu.h
 *
 *  Milkymist TMU driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_TMU_H_
#define __MILKYMIST_TMU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Ioctls */
#define TMU_EXECUTE             0x5400
#define TMU_EXECUTE_NONBLOCK    0x5401
#define TMU_EXECUTE_WAIT        0x5402

struct tmu_vertex {
  int x;
  int y;
} __attribute__((packed));

struct tmu_td {
  unsigned int flags;
  unsigned int hmeshlast;
  unsigned int vmeshlast;
  unsigned int brightness;
  unsigned short chromakey;
  struct tmu_vertex *vertices;
  unsigned short *texfbuf;
  unsigned int texhres;
  unsigned int texvres;
  unsigned int texhmask;
  unsigned int texvmask;
  unsigned short *dstfbuf;
  unsigned int dsthres;
  unsigned int dstvres;
  int dsthoffset;
  int dstvoffset;
  unsigned int dstsquarew;
  unsigned int dstsquareh;
  unsigned int alpha;

  bool invalidate_before;
  bool invalidate_after;
};

#define TMU_BRIGHTNESS_MAX     (63)
#define TMU_MASK_NOFILTER      (0x3ffc0)
#define TMU_MASK_FULL          (0x3ffff)
#define TMU_FIXEDPOINT_SHIFT   (6)
#define TMU_ALPHA_MAX          (63)
#define TMU_MESH_MAXSIZE       (128)

#define TMU_FLAG_CHROMAKEY     (2)
#define TMU_FLAG_ADDITIVE      (4)

rtems_device_driver tmu_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver tmu_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define TMU_DRIVER_TABLE_ENTRY {tmu_initialize, \
NULL, NULL, NULL, NULL, tmu_control}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_TMU_H_ */
