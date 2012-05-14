/* motorola.h
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to identify motorola boards.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/motorola.h>
#include <rtems/bspIo.h>
#include <libcpu/io.h>
#include <string.h>
#include <libcpu/cpuIdent.h>

/*
** Board-specific table that maps interrupt names to onboard PCI
** peripherals as well as local PCI busses.  This table is used at
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

#ifdef qemu
static struct _int_map qemu_prep_intmap[] = {
    { 0, -1, 0, { { 1, { 9, -1, -1, -1}},
                  { 2, {11, -1  -1, -1}},
                  NULL_PINMAP }},
	NULL_INTMAP
};
#endif

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

static struct _int_map mvme23xx_intmap[] = {
/* Raven Hostbridge; has int_pin == 0
   {0,  0, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},
*/

/* Winbond PCI/ISA 83c553; has int_pin == 0
   {0, 11, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},
*/

#if 0 /* Leave as ISA interrupts for now */
   {0, 13, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {11,21,-1,-1,-1}},  /* Universe  ISA/PCI */
			/* strictly speaking, a non-multi function device
			 * must only use pin A
			 */
			 {2, {22,-1,-1,-1,-1}},  /* Universe          */
			 {3, {23,-1,-1,-1,-1}},  /* Universe          */
			 {4, {24,-1,-1,-1,-1}},  /* Universe          */
             NULL_PINMAP}},

   {0, 14, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {10,18,-1,-1}},  /* DEC Tulip enet (ISA/PCI)  */
             NULL_PINMAP}},
#endif

   {0, 16, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {25,-1,-1,-1}},  /* pci/pmc slot 1   */
             {2, {26,-1,-1,-1}},
             {3, {27,-1,-1,-1}},
             {4, {28,-1,-1,-1}},
             NULL_PINMAP}},

   {0, 17, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {28,-1,-1,-1}},  /* pci/pmc slot 2   */ /* orig: 0xf */
             {2, {25,-1,-1,-1}},
             {3, {26,-1,-1,-1}},
             {4, {27,-1,-1,-1}},
             NULL_PINMAP}},

   NULL_INTMAP };

static struct _int_map mvme24xx_intmap[] = {
/* Raven Hostbridge; has int_pin == 0
   {0,  0, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},
*/

/* Winbond PCI/ISA 83c553; has int_pin == 0
   {0, 11, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},
*/

   {0, 13, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {11,21,-1,-1}},  /* Universe  ISA/PCI */
			/* strictly speaking, a non-multi function device
			 * must only use pin A
			 */
			 {2, {22,-1,-1,-1}},  /* Universe          */
			 {3, {23,-1,-1,-1}},  /* Universe          */
			 {4, {24,-1,-1,-1}},  /* Universe          */
             NULL_PINMAP}},

   {0, 14, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {10,18,-1,-1}},  /* DEC Tulip enet (ISA/PCI)  */
             NULL_PINMAP}},
   {0, 16, PCI_FIXUP_OPT_OVERRIDE_NAME,
	    {{1, {25,-1,-1,-1}},  /* pci/pmc slot 1   */
             {2, {26,-1,-1,-1}},
             {3, {27,-1,-1,-1}},
             {4, {28,-1,-1,-1}},
             NULL_PINMAP}},

   {0, 17, PCI_FIXUP_OPT_OVERRIDE_NAME,
	    {{1, {28,-1,-1,-1}},  /* pci/pmc slot 2   */ /* orig: 0xf */
             {2, {25,-1,-1,-1}},
             {3, {26,-1,-1,-1}},
             {4, {27,-1,-1,-1}},
             NULL_PINMAP}},
   NULL_INTMAP };

static struct _int_map mvme27xx_intmap[] = {
/* Raven Hostbridge; has int_pin == 0 */
   {0,  0, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},


/* Winbond PCI/ISA 83c553; has int_pin == 0 */
   {0, 11, 0, {{0, {-1,-1,-1,-1}},
               NULL_PINMAP}},

   {0, 13, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {11,21,-1,-1}},  /* Universe  ISA/PCI */
			/* strictly speaking, a non-multi function device
			 * must only use pin A
			 */
			 {2, {22,-1,-1,-1}},  /* Universe          */
			 {3, {23,-1,-1,-1}},  /* Universe          */
			 {4, {24,-1,-1,-1}},  /* Universe          */
             NULL_PINMAP}},

   {0, 14, PCI_FIXUP_OPT_OVERRIDE_NAME,
			{{1, {10,18,-1,-1}},  /* DEC Tulip enet (ISA/PCI)  */
             NULL_PINMAP}},
   {0, 16, PCI_FIXUP_OPT_OVERRIDE_NAME,
	    {{1, {25,-1,-1,-1}},  /* pci/pmc slot 1   */
             {2, {26,-1,-1,-1}},
             {3, {27,-1,-1,-1}},
             {4, {28,-1,-1,-1}},
             NULL_PINMAP}},

   {0, 17, PCI_FIXUP_OPT_OVERRIDE_NAME,
	    {{1, {28,-1,-1,-1}},  /* pci/pmc slot 2   */ /* orig: 0xf */
             {2, {25,-1,-1,-1}},
             {3, {26,-1,-1,-1}},
             {4, {27,-1,-1,-1}},
             NULL_PINMAP}},
   NULL_INTMAP };

static struct _int_map mvme2100_intmap[] = {
   {0, 0, 0, {{1, {16,-1,-1,-1}}, /* something shows up in slot 0 and OpenPIC  */
                                  /* 0 is unused.  This hushes the init code.  */
               NULL_PINMAP}},

   {0, 13, 0, {{1, {23,-1,-1,-1}},  /* Universe Lint[0-3]; not quite legal     */
               {2, {24,-1,-1,-1}},  /* since the universe is a single-function */
               {3, {25,-1,-1,-1}},  /* device. We leave it for info purposes   */
               {4, {26,-1,-1,-1}},
               NULL_PINMAP}},

   {0, 14, 0, {{1, {17,-1,-1,-1}},  /* onboard ethernet */
               NULL_PINMAP}},

   {0, 16, PCI_FIXUP_OPT_OVERRIDE_NAME,
              {{1, {18,-1,-1,-1}},  /* PMC slot; all pins are routed to 18     */
               {2, {18,-1,-1,-1}},  /* I give the OVERRIDE option since I had  */
               {3, {18,-1,-1,-1}},  /* problems with devices behind a bridge   */
               {4, {18,-1,-1,-1}},  /* on a PMC card reading irq line 0...     */
               NULL_PINMAP}},

   /* FIXME: I don't know how MIP works or what it is; these probably won't work */

   {0, -1, PCI_FIXUP_OPT_OVERRIDE_NAME,
              {{1, {23,-1,-1,-1}},  /* PCI INT[A-D] expansion */
               {2, {24,-1,-1,-1}},
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
      ppc_cpu_id_t      proc_type;
      const char	*name;

      struct _int_map   *intmap;
      int               (*swizzler)(int, int);
} mot_info_t;

/* NOTE: When adding boards here the 'motorolaBoard' enums MUST be
 *       updated accordingly!
 */
static const mot_info_t mot_boards[] = {
  {0x0E0, 0xF9, PPC_604,     "MVME 2400", mvme24xx_intmap,prep_pci_swizzle},
  {0x3E0, 0x00, PPC_750,     "MVME 2400 (PPC 750)", mvme24xx_intmap,prep_pci_swizzle},
  {0x010, 0x00, PPC_UNKNOWN, "Genesis", NULL, NULL},
  {0x020, 0x00, PPC_UNKNOWN, "Powerstack (Series E)", NULL, NULL},
  {0x040, 0x00, PPC_UNKNOWN, "Blackhawk (Powerstack)", NULL, NULL},
  {0x050, 0x00, PPC_UNKNOWN, "Omaha (PowerStack II Pro3000)", NULL, NULL},
  {0x060, 0x00, PPC_UNKNOWN, "Utah (Powerstack II Pro4000)", NULL, NULL},
  {0x0A0, 0x00, PPC_UNKNOWN, "Powerstack (Series EX)", NULL, NULL},
#ifdef qemu
  {0x1E0, 0xE0, PPC_UNKNOWN, "QEMU", qemu_prep_intmap, prep_pci_swizzle},
#else
  {0x1E0, 0xE0, PPC_UNKNOWN, "Mesquite cPCI (MCP750)", mcp750_intmap, prep_pci_swizzle},
#endif
  {0x1E0, 0xE1, PPC_UNKNOWN, "Sitka cPCI (MCPN750)", mcp750_intmap, prep_pci_swizzle},
  {0x1E0, 0xE2, PPC_UNKNOWN, "Mesquite cPCI (MCP750) w/ HAC", mcp750_intmap, prep_pci_swizzle},
  {0x1E0, 0xF6, PPC_UNKNOWN, "MTX Plus", NULL, NULL},
  {0x1E0, 0xF7, PPC_UNKNOWN, "MTX w/o Parallel Port", mtx603_intmap, prep_pci_swizzle},
  {0x1E0, 0xF8, PPC_UNKNOWN, "MTX w/ Parallel Port", mtx603_intmap, prep_pci_swizzle},
  {0x1E0, 0xF9, PPC_604,     "MVME 2300", mvme23xx_intmap, prep_pci_swizzle},
  {0x1E0, 0xFA, PPC_UNKNOWN, "MVME 2300SC/2600", mvme23xx_intmap, prep_pci_swizzle},
  {0x1E0, 0xFB, PPC_UNKNOWN, "MVME 2600 with MVME712M", NULL, NULL},
  {0x1E0, 0xFC, PPC_750,     "MVME 2600/2700 with MVME761", mvme27xx_intmap, prep_pci_swizzle},
  {0x1E0, 0xFD, PPC_UNKNOWN, "MVME 3600 with MVME712M", NULL, NULL},
  {0x1E0, 0xFE, PPC_UNKNOWN, "MVME 3600 with MVME761", NULL, NULL},
  {0x1E0, 0xFF, PPC_UNKNOWN, "MVME 1600-001 or 1600-011", NULL, NULL},
  {0x000, 0x00, PPC_UNKNOWN, ""},   /* end of probeable values for automatic scan */
  {0x000, 0x00, PPC_UNKNOWN, "MVME 2100", mvme2100_intmap, prep_pci_swizzle},
};

prep_t currentPrepType;
motorolaBoard currentBoard;

prep_t checkPrepBoardType(RESIDUAL *res)
{
  prep_t PREP_type;
  /* figure out what kind of prep workstation we are */
  if ( res->ResidualLength != 0 ) {
    if ( !strncmp((char*)res->VitalProductData.PrintableModel,"IBM",3) )
      PREP_type = PREP_IBM;
    else if (!strncmp((char*)res->VitalProductData.PrintableModel,
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

motorolaBoard getMotorolaBoard(void)
{
/*
 *  At least the MVME2100 does not have the CPU Type and Base Type Registers,
 *  so it cannot be probed.
 *
 *  NOTE: Every path must set currentBoard.
 */
#if defined(mvme2100)
  currentBoard = (motorolaBoard) MVME_2100;
#else
  unsigned char  cpu_type;
  unsigned char  base_mod;
  ppc_cpu_id_t   proc_type;
  int	       	 entry;
  int	       	 mot_entry = -1;

  cpu_type  = inb(MOTOROLA_CPUTYPE_REG) & 0xF0;
  base_mod  = inb(MOTOROLA_BASETYPE_REG);
  proc_type = get_ppc_cpu_type ();

  for (entry = 0; mot_boards[entry].cpu_type != 0; entry++) {
    if ((mot_boards[entry].cpu_type & 0xff) != cpu_type)
      continue;

    if ((mot_boards[entry].proc_type != PPC_UNKNOWN) &&
	(mot_boards[entry].proc_type != proc_type))
      /*
       * IMD: processor type does not match
       * (here we distinguish a MVME2300 and a MVME2400)
       */
      continue;

    if (mot_boards[entry].base_type != base_mod)
      continue;
    else {
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
#endif
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
  /* printk( "IntMap board %d 0x%08x\n", board, mot_boards[board].intmap ); */
  return mot_boards[board].intmap;
}

const void *motorolaIntSwizzle(motorolaBoard board)
{
  if (board == MOTOROLA_UNKNOWN) return NULL;
  return (void *)mot_boards[board].swizzler;
}
