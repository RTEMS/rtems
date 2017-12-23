/**
 * @file
 *
 * @ingroup Console
 *
 * This file defines the interfaces between the single port
 * polled console framework and the BSP.
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __BSP_CONSOLE_POLLED_h
#define __BSP_CONSOLE_POLLED_h

/**
 * This method polls the specified character @a ch to the specified
 * console @a port.
 *
 * @param[in] port is the output port
 * @param[in] ch is the character to print
 */
void console_outbyte_polled(int port, char ch);

/**
 * This method polls the specified @a port for an input character.
 * console @a port.
 *
 * @param[in] port is the input port
 *
 * @return This method returns the character read of -1 if there is no data.
 */
int console_inbyte_nonblocking(int port);

/**
 * This method is invoked to initialize the console hardware device(s).
 */
void console_initialize_hardware(void);

#endif
