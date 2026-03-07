/**
 * @file
 * @ingroup i386_apic
 * @brief Local and I/O APIC definitions
 */

/*
 * Author: Erich Boleyn  <erich@uruk.org>
 *         http://www.uruk.org/~erich/
 *
 * Copyright (c) 1997-2011 Erich Boleyn.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup i386_apci
 * @ingroup i386_pci
 * @brief Intel Architecture local and I/O APIC definitions
 * @{
 */

/*
 *  Header file for Intel Architecture local and I/O APIC definitions.
 *
 *  This file was created from information in the Intel Pentium Pro
 *  Family Developer's Manual, Volume 3: Operating System Writer's
 *  Manual, order number 242692-001, which can be ordered from the
 *  Intel literature center.
 */

#ifndef _APIC_H
#define _APIC_H

#include <bsp/apic.h>
#include <rtems/score/basedefs.h>

/*
 *  APIC Defines.
 */

/*
 * Recommendation:  Don't use this except for MSI interrupt delivery.
 * In general, the "Destination Mode" can be used to control this, since
 * it is DIFFERENT (0xF) for Pentium and P6, but not on the same APIC
 * version for AMD Opteron.
 */
#define APIC_BCAST_ID                           0xFF

/*
 *  APIC register definitions
 */

/*
 *  Shared defines for I/O and local APIC definitions
 */
/** @brief APIC version register */
#define APIC_VERSION(x)                         ((x) & 0xFF)
#define APIC_MAXREDIR(x)                        (((x) >> 16) & 0xFF)
/** @brief APIC id register */
#define APIC_ID(x)                              ((x) >> 24)
#define APIC_VER_NEW                            0x10

#define IOAPIC_REGSEL                           0
#define IOAPIC_RW                               0x10
#define IOAPIC_ID                               0
#define IOAPIC_VER                              1
#define IOAPIC_REDIR                            0x10

/**
 * @brief Initializes the Local APIC by hardware and software enabling it.
 *
 * Sets the i386 local apic base pointer and enabled the local apic.
 *
 * @param lapic_addr The physical address of the Local APIC.
 *
 * @return true if successful. 
 */
bool lapic_initialize(uint32_t lapic_addr);

/**
 * @brief Enables the Local APIC via the spurious interrupt vector register.
 */
void lapic_enable(void);

/**
 * @brief Clears the error status register of the Local APIC.
 *
 * This is done by writing to the ESR and then doing a dummy read to make sure
 * the write has taken effect.
 */
void lapic_clear_errors(void);

#ifdef RTEMS_SMP
/**
 * @brief Sends an interprocessor interrupt to a specified processor.
 *
 * @param target_cpu_index The processor index of the target processor.
 * @param isr_vector The vector of the interrupt being sent.
 */
void lapic_send_ipi(uint32_t target_cpu_index, uint8_t isr_vector);

/**
 * @brief Resets and starts the Application Processor that corresponds to cpu_index.
 * 
 * This function is only for integrated APICs (version >= APIC_VER_NEW).
 * The AP is reset by sending an INIT IPI and then started by sending a SIPI 
 * with the page vector of the trampoline code. 
 * 
 * @param cpu_index The processor to be started.
 * @param page_vector The under 1MB 4KB page where the trampoline code is located.
 */
void lapic_reset_and_start_ap(uint32_t cpu_index, uint8_t page_vector);

/**
 * @brief Resets the Application Processor that corresponds to cpu_index.
 * 
 * For older APIC versions, the AP does not need SIPIs to start, so only 
 * an INIT IPI is sent to reset the AP.
 *
 * @param cpu_index The processor to be reset.
 */
void lapic_reset_ap(uint32_t cpu_index);

/** 
 * @brief Get the CPU index for a given Local APIC ID. 
 * 
 * @param lapic_id The Local APIC ID to look up.
 * @return The CPU index corresponding to the given Local APIC ID.
 */
uint8_t lapic_get_cpu_index(uint8_t lapic_id);

/**
 * @brief Get the Local APIC ID for a given CPU index.
 * 
 * @param cpu_index The CPU index to look up.
 * @return The Local APIC ID corresponding to the given CPU index.
 */
uint8_t lapic_get_lapic_id(uint32_t cpu_index);

/**
 * @brief Stores a mapping from APIC id to CPU index.
 *
 * @param lapic_id The Local APIC ID to store.
 * @param cpu_index The CPU index to store.
 */
void lapic_set_cpu_index(uint8_t lapic_id, uint32_t cpu_index);

/**
 * @brief Stores a mapping from CPU index to APIC id.
 *
 * @param cpu_index The CPU index to store.
 * @param lapic_id The Local APIC ID to store.
 */
void lapic_set_lapic_id(uint32_t cpu_index, uint8_t lapic_id);

#endif

/**
 * @brief Retrieves the Local APIC ID of the current processor.
 * @return The Local APIC ID of the current processor.
 */
uint8_t lapic_get_id(void);

/**
 * @brief Sends an End of Interrupt (EOI) signal to the Local APIC.
 */
void lapic_send_eoi(void);

#endif  /* _APIC_H */

/** @} */
