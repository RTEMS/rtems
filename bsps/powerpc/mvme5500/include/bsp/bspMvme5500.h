/* GT64260 register base mapping on the MVME5500
 *
 * (C) Shuchen K. Feng <feng1@bnl.gov>,NSLS,
 * Brookhaven National Laboratory, 2003
 *
 */
#define _256M           0x10000000
#define _512M           0x20000000

#define GT64260_REG_BASE	0xf1000000  /* Base of GT64260 Reg Space */
#define GT64260_REG_SPACE_SIZE	0x10000     /* 64Kb Internal Reg Space */

#define GT64260_DEV1_BASE       0xf1100000  /* Device bank1(chip select 1) base
                                             */
#define GT64260_DEV1_SIZE 	0x00100000 /* Device bank size */
