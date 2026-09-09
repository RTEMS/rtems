/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME2100
 *
 * @brief MVME2100 Board Identification
 *
 * This include file describe the data structure and the functions implemented
 * by rtems to identify motorola boards.
 */

/*
 * Copyright (C) 1999 Eric Valette <eric.valette@free.fr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/motorola.h>
#include <libcpu/io.h>
#include <rtems/bspIo.h>

/*
 * Board-specific table that maps interrupt names to onboard PCI
 * peripherals as well as local PCI buses.  This table is used at
 * bspstart() to configure the interrupt name & pin for all devices that
 * do not have it already specified.  If the device is already
 * configured, we leave it alone but sanity check & print a warning if
 * we don't know about the pin/line the card gives us.
 *
 * bus = the bus number of the slot/device in question
 *
 * slot :
 *
 *   If slot != -1, it indicates a device on the given bus in that slot
 *   is to use one of the listed interrupt names given an interrupt pin.
 *
 *   If slot == -1, it means devices on this bus can occupy any slot-
 *   and for pci, this means the particular interrupt pin that the
 *   device signals is therefore dependent on the particular slot.  To
 *   work from the slot to the interrupt pin, the swizzle table is used.
 *   Once the bus and interrupt pin is known, the correct interrupt name
 *   can be pulled from the table.  The swizzle table relates the
 *   interrupt pin from the device to the particular interrupt
 *   controller interrupt pin- so it is quite reasonable for a device on
 *   bus 1 signalling interrupt pin 1 to show up at the interrupt
 *   controller as pin 4- this is why the int pin field varies for
 *   bridged pci buses.
 *
 *
 * opts = bitmap of options that control the configuration of this
 * slot/bus.
 *
 * pin_routes[] = array of pin & vectors that may serve this slot;
 *
 *      pin = the pin # which delivers an interrupt on this route, A=1,
 *      B=2, C=3, D=4
 *
 *      int_name[4] = an array of up to 4 bsp-specific interrupt name
 *      that can be used by this route.  Unused entries should be -1.
 *      The array is of primary use for slots that can be vectored thru
 *      multiple interrupt lines over the interrupt pin supplied by the
 *      record.  If more than one entry is present, the most preferable
 *      should supplied first.
 *
 */

#define NULL_PINMAP {-1, {-1, -1, -1, -1}}
#define NULL_INTMAP                                                            \
  {                                                                            \
    -1, -1, -1, {                                                              \
    }                                                                          \
  }

static struct _int_map mvme2100_intmap[] = {
    {0,
     0,
     0,
     {{1, {16, -1, -1, -1}}, /* something shows up in slot 0 and OpenPIC  */
                             /* 0 is unused.  This hushes the init code.  */
      NULL_PINMAP}},

    {0,
     13,
     0,
     {{1, {23, -1, -1, -1}}, /* Universe Lint[0-3]; not quite legal     */
      {2, {24, -1, -1, -1}}, /* since the universe is a single-function */
      {3, {25, -1, -1, -1}}, /* device. We leave it for info purposes   */
      {4, {26, -1, -1, -1}},
      NULL_PINMAP}},

    {0,
     14,
     0,
     {{1, {17, -1, -1, -1}}, /* onboard ethernet */
      NULL_PINMAP}},

    {0,
     16,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {18, -1, -1, -1}}, /* PMC slot; all pins are routed to 18     */
      {2, {18, -1, -1, -1}}, /* I give the OVERRIDE option since I had  */
      {3, {18, -1, -1, -1}}, /* problems with devices behind a bridge   */
      {4, {18, -1, -1, -1}}, /* on a PMC card reading irq line 0...     */
      NULL_PINMAP}},

    /* FIXME: I don't know how MIP works or what it is; these probably won't
       work */

    {0,
     -1,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {23, -1, -1, -1}}, /* PCI INT[A-D] expansion */
      {2, {24, -1, -1, -1}},
      {3, {25, -1, -1, -1}},
      {4, {26, -1, -1, -1}},
      NULL_PINMAP}},

    NULL_INTMAP};

/*
 * This table represents the standard PCI swizzle defined in the
 * PCI bus specification.  Table taken from Linux 2.4.18, prep_pci.c,
 * the values in this table are interrupt_pin values (1 based).
 */
static unsigned char prep_pci_intpins[4][4] = {
    {1, 2, 3, 4}, /* Buses 0, 4, 8, ... */
    {2, 3, 4, 1}, /* Buses 1, 5, 9, ... */
    {3, 4, 1, 2}, /* Buses 2, 6, 10 ... */
    {4, 1, 2, 3}, /* Buses 3, 7, 11 ... */
};

static int prep_pci_swizzle(int slot, int pin) {
  return prep_pci_intpins[slot % 4][pin - 1];
}

motorolaBoard currentBoard;

motorolaBoard getMotorolaBoard(void) {
  currentBoard = MVME_2100;
  return currentBoard;
}

const char *motorolaBoardToString(motorolaBoard board) {
  if (board != MVME_2100) {
    return "Unknown motorola board";
  }
  return "MVME 2100";
}

const struct _int_map *motorolaIntMap(motorolaBoard board) {
  if (board != MVME_2100) {
    return NULL;
  }
  return mvme2100_intmap;
}

const void *motorolaIntSwizzle(motorolaBoard board) {
  if (board != MVME_2100) {
    return NULL;
  }
  return (void *)prep_pci_swizzle;
}
