#ifndef VME_AM_DEFINITIONS_H
#define VME_AM_DEFINITIONS_H

/* vxworks compatible addressing modes */

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

/* Flags 1<<11 .. 1<<8  are reserved
 * Flags 1<<12 .. 1<<31 are for driver specific options
 */

#endif
