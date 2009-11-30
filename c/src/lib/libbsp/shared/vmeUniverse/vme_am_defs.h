#ifndef VME_AM_DEFINITIONS_H
#define VME_AM_DEFINITIONS_H

/* vxworks compatible addressing modes */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */
/* NOTE: 64-bit *addresses* are not supported [data are]. */

#ifndef VME_AM_STD_SUP_BLT
#define	VME_AM_STD_SUP_BLT			0x3f
#endif
#ifndef VME_AM_STD_SUP_ASCENDING
#define	VME_AM_STD_SUP_ASCENDING	0x3f
#endif
#ifndef VME_AM_STD_SUP_PGM
#define	VME_AM_STD_SUP_PGM			0x3e
#endif
#ifndef VME_AM_STD_SUP_MBLT
#define	VME_AM_STD_SUP_MBLT			0x3c
#endif
#ifndef VME_AM_STD_USR_BLT
#define	VME_AM_STD_USR_BLT			0x3b
#endif
#ifndef VME_AM_STD_USR_ASCENDING
#define	VME_AM_STD_USR_ASCENDING	0x3b
#endif
#ifndef VME_AM_STD_USR_PGM
#define	VME_AM_STD_USR_PGM			0x3a
#endif
#ifndef VME_AM_STD_SUP_DATA
#define	VME_AM_STD_SUP_DATA			0x3d
#endif
#ifndef VME_AM_STD_USR_DATA
#define	VME_AM_STD_USR_DATA			0x39
#endif
#ifndef VME_AM_STD_USR_MBLT
#define	VME_AM_STD_USR_MBLT			0x38
#endif
#ifndef VME_AM_EXT_SUP_BLT
#define	VME_AM_EXT_SUP_BLT			0x0f
#endif
#ifndef VME_AM_EXT_SUP_ASCENDING
#define	VME_AM_EXT_SUP_ASCENDING	0x0f
#endif
#ifndef VME_AM_EXT_SUP_PGM
#define	VME_AM_EXT_SUP_PGM			0x0e
#endif
#ifndef VME_AM_EXT_SUP_DATA
#define	VME_AM_EXT_SUP_DATA			0x0d
#endif
#ifndef VME_AM_EXT_SUP_MBLT
#define	VME_AM_EXT_SUP_MBLT			0x0c
#endif
#ifndef VME_AM_EXT_USR_BLT
#define	VME_AM_EXT_USR_BLT			0x0b
#endif
#ifndef VME_AM_EXT_USR_ASCENDING
#define	VME_AM_EXT_USR_ASCENDING	0x0b
#endif
#ifndef VME_AM_EXT_USR_PGM
#define	VME_AM_EXT_USR_PGM			0x0a
#endif
#ifndef VME_AM_EXT_USR_DATA
#define	VME_AM_EXT_USR_DATA			0x09
#endif
#ifndef VME_AM_EXT_USR_MBLT
#define	VME_AM_EXT_USR_MBLT			0x08
#endif
#ifndef VME_AM_2eVME_6U
#define VME_AM_2eVME_6U				0x20
#endif
#ifndef VME_AM_2eVME_3U
#define VME_AM_2eVME_3U				0x21
#endif
#ifndef VME_AM_CSR
#define VME_AM_CSR					0x2f
#endif
#ifndef VME_AM_SUP_SHORT_IO
#define	VME_AM_SUP_SHORT_IO			0x2d
#endif
#ifndef VME_AM_USR_SHORT_IO
#define	VME_AM_USR_SHORT_IO			0x29
#endif
#ifndef VME_AM_IS_SHORT
#define VME_AM_IS_SHORT(a)			(((a) & 0x30) == 0x20)
#endif
#ifndef VME_AM_IS_STD
#define VME_AM_IS_STD(a)			(((a) & 0x30) == 0x30)
#endif
#ifndef VME_AM_IS_EXT
#define VME_AM_IS_EXT(a)			(((a) & 0x30) == 0x00)
#endif
#ifndef VME_AM_IS_SUP
#define VME_AM_IS_SUP(a)			((a)  & 4)
#endif

/* Mask for standard address modifiers */

#ifndef VME_AM_MASK
#define VME_AM_MASK					0x3f
#endif

/* Hint that a window is mapping memory; the
 * driver may assume it to be safe to enable decoupled
 * cycles, caching and the like...
 */
#ifndef VME_AM_IS_MEMORY
#define VME_AM_IS_MEMORY			(1<<8)
#endif

/* I don't know AMs for 2eSST so we use some extra bits;
 * HOWEVER: these are just qualifiers to the VME_AM_2eVME_xx modes
 *          i.e., if you want 2eSST you must also select 2eVME...
 */

/* 2eSST broadcast; you still need to set one of the speed bits */
#define VME_AM_2eSST_BCST			(1<<9)
/* Low speed (driver specific) */
#define VME_AM_2eSST_LO				(1<<10)
/* Mid speed (driver specific) */
#define VME_AM_2eSST_MID			(2<<10)
/* High speed (driver specific) */
#define VME_AM_2eSST_HI				(3<<10)

#define VME_AM_IS_2eSST(am)			((am) & (3<<10))

/* Use 8/16/32-bit transfers for coupled- or BLT cycles
 * (MBLT, 2exxx are probably always 64-bit)
 */
#define VME_MODE_DBW_MSK			(3<<12)
#define VME_MODE_DBW8				(1<<12)
#define VME_MODE_DBW16				(2<<12)
#define VME_MODE_DBW32				(3<<12)

/* Unused Flags 1<<14 .. 1<<23  are reserved
 *
 * (20-23 used by DMA API).
 * Flags 1<<24 .. 1<<31 are for driver specific options
 */


#endif
