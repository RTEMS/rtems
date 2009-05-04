/*
 *  $Id$
 * 
 *  Copyright 2003  S. Kate Feng <feng1@bnl.gov>,
 *            NSLS, Brookhaven National Laboratory. All rights reserved.
 *            under the Deaprtment of Energy contract DE-AC02-98CH10886
 */

#ifndef __GT64260TWSI_h
#define __GT64260TWSI_h

/* GT64260TWSI.h -  header for the GT64260 Two-Wire Serial Interface */

/* TWSI Control Register Bits */
#define TWSI_ACK	   4
#define TWSI_INTFLG	   8
#define TWSI_STOP       0x10
#define TWSI_START      0x20
#define TWSI_TWSIEN     0x40
#define TWSI_INTEN      0x80

void GT64260TWSIinit();
int GT64260TWSIstart();
int GT64260TWSIwrite(unsigned char Data);
int GT64260TWSIread(unsigned char *, int lastByte);
int GT64260TWSIstop();

#endif
