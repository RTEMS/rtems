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


/*
** Board-specific table that maps interrupt names to onboard pci
** peripherals as well as local pci busses.  This table is used at
** bspstart() to configure the interrupt name & pin for all devices that
** do not have it already specified.  If the device is already
** configured, we leave it alone but sanity check & print a warning if
** we don't know about the pin/line the card gives us.
**
** bus = the bus number of the slot/device in question
**
** slot :
**
**   If slot != -1, it indicates a device on the given bus in that slot
**   is to use one of the listed interrupt names given an interrupt pin.
**
**   If slot == -1, it means devices on this bus can occupy any slot-
**   and for pci, this means the particular interrupt pin that the
**   device signals is therefore dependent on the particular slot.  To
**   work from the slot to the interrupt pin, the swizzle table is used.
**   Once the bus and interrupt pin is known, the correct interrupt name
**   can be pulled from the table.  The swizzle table relates the
**   interrupt pin from the device to the particular interrupt
**   controller interrupt pin- so it is quite reasonable for a device on
**   bus 1 signalling interrupt pin 1 to show up at the interrupt
**   controller as pin 4- this is why the int pin field varies for
**   bridged pci busses.
**
**
** opts = bitmap of options that control the configuration of this
** slot/bus.
**
** pin_routes[] = array of pin & vectors that may serve this slot;
**
**      pin = the pin # which delivers an interrupt on this route, A=1,
**      B=2, C=3, D=4
**
**      int_name[4] = an array of up to 4 bsp-specific interrupt name
**      that can be used by this route.  Unused entries should be -1.
**      The array is of primary use for slots that can be vectored thru
**      multiple interrupt lines over the interrupt pin supplied by the
**      record.  If more than one entry is present, the most preferable
**      should supplied first.
** 
*/

#define NULL_PINMAP     {-1,{-1,-1,-1,-1}}
#define NULL_INTMAP     {-1,-1,-1,{}}



static struct _int_map mcp750_intmap[] = { 

   { 0, 16, 0, {{1,  {5, 19,-1,-1}}, /* pmc slot */
                NULL_PINMAP}},

   { 0, 14, 0, {{1,  {10,18,-1,-1}}, /* onboard ethernet */
                NULL_PINMAP}},

   { 1, -1, 0, {{1,  {24,-1,-1,-1}},
                {2,  {25,-1,-1,-1}},
                {3,  {26,-1,-1,-1}},
                {4,  {27,-1,-1,-1}},
                NULL_PINMAP}},

   NULL_INTMAP };




static struct _int_map mtx603_intmap[] = {

   {0, 14, 0, {{1, {10,16,-1,-1}},  /* onboard ethernet */
               NULL_PINMAP}},

   {0, 12, 0, {{1, {14,18,-1,-1}},  /* onboard scsi */
               NULL_PINMAP}},

   {0, 16, 0, {{1, {25,-1,-1,-1}},  /* pci/pmc slot 1 */
               {2, {26,-1,-1,-1}},
               {3, {27,-1,-1,-1}},
               {4, {28,-1,-1,-1}},
               NULL_PINMAP}},

   {0, 17, 0, {{1, {26,-1,-1,-1}},  /* pci/pmc slot 2 */
               {2, {27,-1,-1,-1}},
               {3, {28,-1,-1,-1}},
               {4, {25,-1,-1,-1}},
               NULL_PINMAP}},

   {0, 18, 0, {{1, {27,-1,-1,-1}},  /* pci slot 3 */
               {2, {28,-1,-1,-1}},
               {3, {25,-1,-1,-1}},
               {4, {26,-1,-1,-1}},
               NULL_PINMAP}},

   NULL_INTMAP };








/*
 * This table represents the standard PCI swizzle defined in the
 * PCI bus specification.  Table taken from Linux 2.4.18, prep_pci.c,
 * the values in this table are interrupt_pin values (1 based).
 */
static unsigned char prep_pci_intpins[4][4] =
{
	{ 1, 2, 3, 4 },  /* Buses 0, 4, 8, ... */
	{ 2, 3, 4, 1 },  /* Buses 1, 5, 9, ... */
	{ 3, 4, 1, 2 },  /* Buses 2, 6, 10 ... */
	{ 4, 1, 2, 3 },  /* Buses 3, 7, 11 ... */
};

static int prep_pci_swizzle(int slot, int pin)
{
   return prep_pci_intpins[ slot % 4 ][ pin-1 ];
}









typedef struct {
  /*
   * 0x100 mask assumes for Raven and Hawk boards
   * that the level/edge are set.
   * 0x200 if this board has a Hawk chip.
   */
      int		cpu_type;
      int		base_type;
      const char	*name;

      struct _int_map   *intmap;
      int               (*swizzler)(int, int);
} mot_info_t;


static const mot_info_t mot_boards[] = {
  {0x300, 0x00, "MVME 2400", NULL, NULL},
  {0x010, 0x00, "Genesis", NULL, NULL},
  {0x020, 0x00, "Powerstack (Series E)", NULL, NULL},
  {0x040, 0x00, "Blackhawk (Powerstack)", NULL, NULL},
  {0x050, 0x00, "Omaha (PowerStack II Pro3000)", NULL, NULL},
  {0x060, 0x00, "Utah (Powerstack II Pro4000)", NULL, NULL},
  {0x0A0, 0x00, "Powerstack (Series EX)", NULL, NULL},
  {0x1E0, 0xE0, "Mesquite cPCI (MCP750)", mcp750_intmap, prep_pci_swizzle},
  {0x1E0, 0xE1, "Sitka cPCI (MCPN750)", mcp750_intmap, prep_pci_swizzle},
  {0x1E0, 0xE2, "Mesquite cPCI (MCP750) w/ HAC", mcp750_intmap, prep_pci_swizzle},
  {0x1E0, 0xF6, "MTX Plus", NULL, NULL},
  {0x1E0, 0xF7, "MTX w/o Parallel Port", mtx603_intmap, prep_pci_swizzle},
  {0x1E0, 0xF8, "MTX w/ Parallel Port", mtx603_intmap, prep_pci_swizzle},
  {0x1E0, 0xF9, "MVME 2300", NULL, NULL},
  {0x1E0, 0xFA, "MVME 2300SC/2600", NULL, NULL},
  {0x1E0, 0xFB, "MVME 2600 with MVME712M", NULL, NULL},
  {0x1E0, 0xFC, "MVME 2600/2700 with MVME761", NULL, NULL},
  {0x1E0, 0xFD, "MVME 3600 with MVME712M", NULL, NULL},
  {0x1E0, 0xFE, "MVME 3600 with MVME761", NULL, NULL},
  {0x1E0, 0xFF, "MVME 1600-001 or 1600-011", NULL, NULL},
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


const struct _int_map *motorolaIntMap(motorolaBoard board)
{
  if (board == MOTOROLA_UNKNOWN) return NULL;
  return mot_boards[board].intmap;
}


const void *motorolaIntSwizzle(motorolaBoard board)
{
  if (board == MOTOROLA_UNKNOWN) return NULL;
  return (void *)mot_boards[board].swizzler;
}

