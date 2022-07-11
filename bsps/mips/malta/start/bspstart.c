/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This file contains the bsp_start() method and support.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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
#include <libcpu/isr_entries.h>
#include <bsp/bootcard.h>
#include <rtems/pci.h>
#include <bsp/irq-generic.h>
#include <bsp/i8259.h>

/*
 * STRUCTURES
 */

/* Structure filled in by get_mem_info.  Only the size field is
 * actually used (to clear bss), so the others aren't even filled in.
 */
struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};


/*
 * GLOBALS
 */
uint32_t bsp_clicks_per_microsecond;


/*
 * PROTOTYPES
 */
void clear_cache( void *address, size_t n );
void get_mem_info( struct s_mem *mem );

/*
 * EXTERNs
 */
extern int RamSize;

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  /* uint32_t  board_ID = 0x420; */
  static    int j = 1;
  int       pci_init_retval;

  /*
   * Note: This is the value that works for qemu, and it was
   * unable to be validated on the actual hardware.
   */
  mips_set_sr( 0x04100000 );

  bsp_interrupt_initialize();

  /*
   *  XXX need to figure out a real value. :)
   *  This works for the qemu simulation, but timeing may
   *  be off for the actual hardware.
   */
  bsp_clicks_per_microsecond = 100;

  #if 1
  while ( j != 1 ) {
    int i;
    printk (".");
    for (i=0; i<1000; i++);
  }
  #endif

  /*
   * init PCI Bios interface...
   */
  pci_init_retval = pci_initialize();
  if (pci_init_retval != PCIB_ERR_SUCCESS) {
      printk("PCI bus: could not initialize PCI BIOS interface\n");
  }

  BSP_i8259s_init();

}

/*
 *  Required routine by some gcc run-times.
 */
void clear_cache( void *address, size_t n )
{
}

void get_mem_info(
  struct s_mem *mem
)
{
  mem->size = (int) (&RamSize); /* Normally 128 or 256 MB */
}
