/*
 io_efa2.h by CyteX

 Based on io_mpfc.h by chishm (Michael Chisholm)

 Hardware Routines for reading the NAND flash located on
 EFA2 flash carts

 Used with permission from Cytex.
*/

#ifndef IO_EFA2_H
#define IO_EFA2_H

// 'EFA2'
#define DEVICE_TYPE_EFA2 0x32414645

#include "disc_io.h"

// export interface
extern const IO_INTERFACE _io_efa2;

#endif	// define IO_EFA2_H
