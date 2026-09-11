/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME2700
 *
 * @brief MVME2700 Board Identification
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
#include <string.h>

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
 */

#define NULL_PINMAP {-1, {-1, -1, -1, -1}}
#define NULL_INTMAP                                                            \
  {                                                                            \
    -1, -1, -1, {                                                              \
    }                                                                          \
  }

static struct _int_map mvme27xx_intmap[] = {
    /* Raven Hostbridge; has int_pin == 0 */
    {0, 0, 0, {{0, {-1, -1, -1, -1}}, NULL_PINMAP}},

    /* Winbond PCI/ISA 83c553; has int_pin == 0 */
    {0, 11, 0, {{0, {-1, -1, -1, -1}}, NULL_PINMAP}},

    {0,
     13,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {11, 21, -1, -1}}, /* Universe  ISA/PCI */
      /* strictly speaking, a non-multi function device
       * must only use pin A
       */
      {2, {22, -1, -1, -1}}, /* Universe          */
      {3, {23, -1, -1, -1}}, /* Universe          */
      {4, {24, -1, -1, -1}}, /* Universe          */
      NULL_PINMAP}},

    {0,
     14,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {10, 18, -1, -1}}, /* DEC Tulip enet (ISA/PCI)  */
      NULL_PINMAP}},
    {0,
     16,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {25, -1, -1, -1}}, /* pci/pmc slot 1   */
      {2, {26, -1, -1, -1}},
      {3, {27, -1, -1, -1}},
      {4, {28, -1, -1, -1}},
      NULL_PINMAP}},

    {0,
     17,
     PCI_FIXUP_OPT_OVERRIDE_NAME,
     {{1, {28, -1, -1, -1}},
      /* pci/pmc slot 2   */ /* orig: 0xf */
      {2, {25, -1, -1, -1}},
      {3, {26, -1, -1, -1}},
      {4, {27, -1, -1, -1}},
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

prep_t currentPrepType;
motorolaBoard currentBoard;

prep_t checkPrepBoardType(RESIDUAL *res) {
  prep_t PREP_type;
  /* figure out what kind of prep workstation we are */
  if (res->ResidualLength != 0) {
    if (!strncmp((char *)res->VitalProductData.PrintableModel, "IBM", 3)) {
      PREP_type = PREP_IBM;
    } else if (!strncmp((char *)res->VitalProductData.PrintableModel,
                        "Radstone", 8)) {
      PREP_type = PREP_Radstone;
    } else {
      PREP_type = PREP_Motorola;
    }
  } else /* assume motorola if no residual (netboot?) */ {
    PREP_type = PREP_Motorola;
  }
  currentPrepType = PREP_type;
  return PREP_type;
}

motorolaBoard getMotorolaBoard(void) {
  currentBoard = MVME_2600_2700_W_MVME761;
  return currentBoard;
}

const char *motorolaBoardToString(motorolaBoard board) {
  if (board != MVME_2600_2700_W_MVME761) {
    return "Unknown motorola board";
  }
  return "MVME 2600/2700 with MVME761";
}

const struct _int_map *motorolaIntMap(motorolaBoard board) {
  if (board != MVME_2600_2700_W_MVME761) {
    return NULL;
  }
  return mvme27xx_intmap;
}

const void *motorolaIntSwizzle(motorolaBoard board) {
  if (board != MVME_2600_2700_W_MVME761) {
    return NULL;
  }
  return (void *)prep_pci_swizzle;
}
