/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */

#ifndef _VGA_P_H_
#define _VGA_P_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VGA_FB		((unsigned32)PCI_MEM_BASE+0xb8000)
#define VGA_NUM_ROWS	25
#define VGA_NUM_COLS	80

#define VGA_WRITE_SEQ(reg, val) \
	outport_byte(0x3c4, reg); \
	outport_byte(0x3c5, val)
#define VGA_READ_SEQ(reg, val) \
	outport_byte(0x3c4, reg); \
	inport_byte(0x3c5, val)
#define VGA_WRITE_CRTC(reg, val) \
	outport_byte(0x3d4, reg); \
	outport_byte(0x3d5, val)
#define VGA_WRITE_GRA(reg, val) \
	outport_byte(0x3ce, reg); \
	outport_byte(0x3cf, val)
#define VGA_WRITE_ATT(reg, val) \
	{ \
		volatile unsigned8 ucDummy; \
		inport_byte(0x3da, ucDummy); \
		outport_byte(0x3c0, reg); \
		outport_byte(0x3c0, val); \
	}

/*
 * Exported functions
 */
extern boolean vga_probe(int minor);

extern void vga_init(int minor);

extern void vga_write(
	int   minor, 
	char  cChar
);

extern int vga_write_support(
	int   minor, 
	const char *buf, 
	int   len
);

#ifdef __cplusplus
}
#endif

#endif /* _VGA_P_H_ */
