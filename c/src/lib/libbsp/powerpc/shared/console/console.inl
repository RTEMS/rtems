/* inline routines for console i/o
 *
 * The purpose of this file is to provide generic inline functions,
 * i.e. not using hardcoded base addresses. These are provided by
 * the BSP header.
 */

#include <bsp.h>

#define INL_IN_DECL(name,base) \
static inline unsigned char name(int off) \
{ \
	return in_8((unsigned char*)(((unsigned long)base) + off)); \
}

#define INL_OUT_DECL(name,base) \
static inline void name(int off, unsigned int val) \
{ \
	out_8((unsigned char*)(((unsigned long)base) + off), val); \
}

#ifdef BSP_UART_IOBASE_COM1
INL_IN_DECL(com1_inb,	BSP_UART_IOBASE_COM1)
INL_OUT_DECL(com1_outb,	BSP_UART_IOBASE_COM1)
#endif
#ifdef BSP_UART_IOBASE_COM2
INL_IN_DECL(com2_inb,	BSP_UART_IOBASE_COM2)
INL_OUT_DECL(com2_outb,	BSP_UART_IOBASE_COM2)
#endif

#if defined(BSP_CONSOLE_PORT)
#if   (BSP_CONSOLE_PORT == BSP_UART_COM1) && defined(BSP_UART_IOBASE_COM1)
#define INL_CONSOLE_INB		com1_inb
#define INL_CONSOLE_OUTB	com1_outb
#elif (BSP_CONSOLE_PORT == BSP_UART_COM2) && defined(BSP_UART_IOBASE_COM2)
#define INL_CONSOLE_INB		com2_inb
#define INL_CONSOLE_OUTB	com2_outb
#endif
#endif

#ifdef BSP_KBD_IOBASE
INL_IN_DECL(kbd_inb,	BSP_KBD_IOBASE)
INL_OUT_DECL(kbd_outb,	BSP_KBD_IOBASE)
#endif

#ifdef BSP_VGA_IOBASE
INL_OUT_DECL(vga_outb,  BSP_VGA_IOBASE)
#endif
