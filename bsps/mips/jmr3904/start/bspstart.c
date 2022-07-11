/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
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
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

/* Structure filled in by get_mem_info.  Only the size field is
 * actually used (to clear bss), so the others aren't even filled in.
 */
struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};

void bsp_start( void );
void clear_cache( void *address, size_t n );
void get_mem_info( struct s_mem *mem );

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  mips_set_sr( 0xff00 );  /* all interrupts unmasked but globally off */
                          /* depend on the IRC to take care of things */
  bsp_interrupt_initialize();
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
  mem->size = 0x1000000;        /* XXX figure out something here */
}
