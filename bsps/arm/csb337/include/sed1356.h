/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCSB337
 *
 * @brief Public Interface for SED Video Controller Operations.
 */

/**
 * @defgroup csb337_sed1356 SED Video Controller
 *
 * @ingroup RTEMSBSPsARMCSB337
 *
 * @brief Public Interface for SED Video Controller Operations.
 */

/*
 * Public Interface for SED Video Controller Operations
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 *  from NCB - Sistemas Embarcados Ltda. (Brazil)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __SED1356_h
#define __SED1356_h

void sed_init(void);

int sed_frame_buffer_size(void);

void sed_clr_row(int char_row);

void sed_putchar(char c);

void sed_putstring(char *s);

void sed_clearscreen(void);

void sed_write_frame_buffer(
  uint32_t i,
  uint16_t wr16
);

#endif
/* end of file */
