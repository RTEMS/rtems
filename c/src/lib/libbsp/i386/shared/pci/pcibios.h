/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#ifndef _PCIB_H
#define _PCIB_H

/* Error codes */
#define PCIB_ERR_SUCCESS       (0)
#define PCIB_ERR_UNINITIALIZED (-1)  /* PCI BIOS is not initilized */
#define PCIB_ERR_NOTPRESENT    (-2)  /* PCI BIOS not present */
#define PCIB_ERR_NOFUNC        (-3)  /* Function not supported */
#define PCIB_ERR_BADVENDOR     (-4)  /* Bad Vendor ID */
#define PCIB_ERR_DEVNOTFOUND   (-5)  /* Device not found */
#define PCIB_ERR_BADREG        (-6)  /* Bad register number */   

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

int pcib_init(void);
int pcib_find_by_devid(int vendorId, int devId, int idx, int *sig);
int pcib_find_by_class(int classCode, int idx, int *sig);
int pcib_special_cycle(int busNo, int data);
int pcib_conf_read8(int sig, int off, unsigned char *data);
int pcib_conf_read16(int sig, int off, unsigned short *data);
int pcib_conf_read32(int sig, int off, unsigned int *data);
int pcib_conf_write8(int sig, int off, unsigned int data);
int pcib_conf_write16(int sig, int off, unsigned int data);
int pcib_conf_write32(int sig, int off, unsigned int data);

int
BSP_pciFindDevice( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun );

#ifdef __cplusplus
}
#endif

#endif /* _PCIB_H */

