/*
 *  $Id$
 */

#ifndef __LIBCHIP_SERIAL_SUPPORT_h
#define __LIBCHIP_SERIAL_SUPPORT_h

int termios_baud_to_index(
  int termios_baud
);

int termios_baud_to_number(
  int termios_baud
);

bool libchip_serial_default_probe(
  int minor
);


#endif
/* end of include file */
