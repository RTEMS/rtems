/* motorola.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to identify motorola boards.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <bsp/motorola.h>
#include <rtems/bspIo.h>
#include <libcpu/io.h>
#include <string.h>

typedef struct {
  /*
   * 0x100 mask assumes for Raven and Hawk boards
   * that the level/edge are set.
   * 0x200 if this board has a Hawk chip.
   */
  int		cpu_type;
  int		base_type;
  const char	*name;
} mot_info_t;


static const mot_info_t mot_boards[] = {
  {0x300, 0x00, "MVME 2400"},
  {0x010, 0x00, "Genesis"},
  {0x020, 0x00, "Powerstack (Series E)"},
  {0x040, 0x00, "Blackhawk (Powerstack)"},
  {0x050, 0x00, "Omaha (PowerStack II Pro3000)"},
  {0x060, 0x00, "Utah (Powerstack II Pro4000)"},
  {0x0A0, 0x00, "Powerstack (Series EX)"},
  {0x1E0, 0xE0, "Mesquite cPCI (MCP750)"},
  {0x1E0, 0xE1, "Sitka cPCI (MCPN750)"},
  {0x1E0, 0xE2, "Mesquite cPCI (MCP750) w/ HAC"},
  {0x1E0, 0xF6, "MTX Plus"},
  {0x1E0, 0xF7, "MTX wo/ Parallel Port"},
  {0x1E0, 0xF8, "MTX w/ Parallel Port"},
  {0x1E0, 0xF9, "MVME 2300"},
  {0x1E0, 0xFA, "MVME 2300SC/2600"},
  {0x1E0, 0xFB, "MVME 2600 with MVME712M"},
  {0x1E0, 0xFC, "MVME 2600/2700 with MVME761"},
  {0x1E0, 0xFD, "MVME 3600 with MVME712M"},
  {0x1E0, 0xFE, "MVME 3600 with MVME761"},
  {0x1E0, 0xFF, "MVME 1600-001 or 1600-011"},
  {0x000, 0x00, ""}
};

prep_t currentPrepType;
motorolaBoard		currentBoard;
prep_t checkPrepBoardType(RESIDUAL *res)
{
  prep_t PREP_type;
  /* figure out what kind of prep workstation we are */
  if ( res->ResidualLength != 0 ) {
    if ( !strncmp(res->VitalProductData.PrintableModel,"IBM",3) )
      PREP_type = PREP_IBM;
    else if (!strncmp(res->VitalProductData.PrintableModel,
		      "Radstone",8)){
      PREP_type = PREP_Radstone;
    }
    else
      PREP_type = PREP_Motorola;
  }
  else /* assume motorola if no residual (netboot?) */ {
    PREP_type = PREP_Motorola;
  }
  currentPrepType = PREP_type;
  return PREP_type;
}

motorolaBoard	getMotorolaBoard()
{
  unsigned char  cpu_type;
  unsigned char  base_mod;
  int	       	 entry;
  int	       	 mot_entry = -1;

  cpu_type = inb(MOTOROLA_CPUTYPE_REG) & 0xF0;
  base_mod = inb(MOTOROLA_BASETYPE_REG);

  for (entry = 0; mot_boards[entry].cpu_type != 0; entry++) {
    if ((mot_boards[entry].cpu_type & 0xff) != cpu_type)
      continue;
      
    if (mot_boards[entry].base_type == 0) {
      mot_entry = entry;
      break;
    }
      
    if (mot_boards[entry].base_type != base_mod)
      continue;
    else{
      mot_entry = entry;
      break;
    }
  }
  if (mot_entry == -1) {
    printk("Unknown motorola board Please update libbsp/powerpc/shared/motorola/motorola.c\n");
    printk("cpu_type = %x\n", (unsigned) cpu_type);
    printk("base_mod = %x\n", (unsigned) base_mod);
    currentBoard = MOTOROLA_UNKNOWN;
    return currentBoard;
  }
  currentBoard = (motorolaBoard) mot_entry;
  return currentBoard;
}

const char* motorolaBoardToString(motorolaBoard board)
{
  if (board == MOTOROLA_UNKNOWN) return "Unknown motorola board";
  return (mot_boards[board].name);
}

