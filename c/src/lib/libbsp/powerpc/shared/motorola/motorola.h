/* motorola.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to identify motorola boards.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H
#define LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H

#include <bsp/residual.h>
#include <bsp/pci.h>

typedef enum {
  PREP_IBM 	= 0,
  PREP_Radstone = 1,
  PREP_Motorola = 2
} prep_t;

typedef enum {
  MVME_2400 			= 0,
  MVME_2400_750			= 1,
  GENESIS   			= 2,
  POWERSTACK_E			= 3,
  BLACKAWK			= 4,
  OMAHA				= 5,
  UTAH				= 6,
  POWERSTACK_EX			= 7,
  MESQUITE			= 8,
  SITKA				= 9,
  MESQUITE_W_HAC		= 10,
  MTX_PLUS			= 11,
  MTX_WO_PP			= 12,
  MTX_W_PP			= 13,
  MVME_2300			= 14,
  MVME_2300SC_2600		= 15,
  MVME_2600_W_MVME712M		= 16,
  MVME_2600_2700_W_MVME761	= 17,
  MVME_3600_W_MVME712M		= 18,
  MVME_3600_W_MVME761		= 19,
  MVME_1600			= 20,
  /* In the table, slot 21 is the marker for end of automatic probe and scan */
  MVME_2100			= 22,
  MOTOROLA_UNKNOWN		= 255
} motorolaBoard;

typedef enum {
  HOST_BRIDGE_RAVEN	= 0,
  HOST_BRIDGE_HAWK	= 1,
  HOST_BRIDGE_UNKNOWN	= 255
} motorolaHostBridge;

#define MOTOROLA_CPUTYPE_REG	0x800
#define MOTOROLA_BASETYPE_REG	0x803

extern prep_t 			checkPrepBoardType(RESIDUAL *res);
extern prep_t 			currentPrepType;
extern motorolaBoard		getMotorolaBoard();
extern motorolaBoard		currentBoard;
extern const char*		motorolaBoardToString(motorolaBoard);
extern const struct _int_map    *motorolaIntMap(motorolaBoard board);
extern const void               *motorolaIntSwizzle(motorolaBoard board);

#endif /* LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H */
