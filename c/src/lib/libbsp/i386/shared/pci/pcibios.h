/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#ifndef _PCIB_H
#define _PCIB_H

#include <rtems/pci.h>

/*
 * Make device signature from bus number, device numebr and function
 * number
 */
#define PCIB_DEVSIG_MAKE(b,d,f) ((b<<8)|(d<<3)|(f))

/*
 * Extract valrous part from device signature
 */
#define PCIB_DEVSIG_BUS(x) (((x)>>8) &0xff)
#define PCIB_DEVSIG_DEV(x) (((x)>>3) & 0x1f)
#define PCIB_DEVSIG_FUNC(x) ((x) & 0x7)

#ifdef __cplusplus
extern "C" {
#endif

int pcib_find_by_class(int classCode, int idx, int *sig);
int pcib_special_cycle(int busNo, int data);
int pcib_conf_read8(int sig, int off, uint8_t *data);
int pcib_conf_read16(int sig, int off, uint16_t *data);
int pcib_conf_read32(int sig, int off, uint32_t *data);
int pcib_conf_write8(int sig, int off, uint8_t data);
int pcib_conf_write16(int sig, int off, uint16_t data);
int pcib_conf_write32(int sig, int off, uint32_t data);

int
pci_find_device( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun );

#ifdef __cplusplus
}
#endif

#endif /* _PCIB_H */
