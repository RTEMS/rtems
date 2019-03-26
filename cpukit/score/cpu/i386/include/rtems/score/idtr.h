/**
 * @file
 * 
 * @brief Intel I386 Data Structures
 * 
 * @addtogroup RTEMSScoreCPUi386
 * 
 * This file contains definitions for data structure related
 *          to Intel system programming. More information can be found
 *          on Intel site and more precisely in the following book :
 *
 *              Pentium Processor familly
 *              Developper's Manual
 *
 *              Volume 3 : Architecture and Programming Manual
 *
 * Formerly contained in and extracted from libcpu/i386/cpu.h.
 */

/*
 * COPYRIGHT (C) 1998  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Applications must not include this file directly.
 */

#ifndef _RTEMS_SCORE_IDTR_H
#define _RTEMS_SCORE_IDTR_H

/*
 * See page 14.9 Figure 14-2.
 *
 */
typedef struct
{
  unsigned int low_offsets_bits:16;
  unsigned int segment_selector:16;
  unsigned int fixed_value_bits:8;
  unsigned int gate_type:5;
  unsigned int privilege:2;
  unsigned int present:1;
  unsigned int high_offsets_bits:16;
} interrupt_gate_descriptor;

/*
 * C callable function enabling to create a interrupt_gate_descriptor
 */
extern void create_interrupt_gate_descriptor (interrupt_gate_descriptor*, rtems_raw_irq_hdl);

/*
 * C callable function enabling to get easily usable info from
 * the actual value of IDT register.
 */
extern void i386_get_info_from_IDTR (interrupt_gate_descriptor** table,
				unsigned* limit);

/*
 * C callable function enabling to change the value of IDT register. Must be called
 * with interrupts masked at processor level!!!.
 */
extern void i386_set_IDTR (interrupt_gate_descriptor* table,
		      unsigned limit);

#endif
