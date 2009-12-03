#ifndef RTEMS_COMPAT_BSD_NET_H
#define RTEMS_COMPAT_BSD_NET_H

/* BSD -> rtems wrappers; stuff that must be defined
 *        prior to including most BSD headers
 */

/* Copyright: Till Straumann <strauman@slac.stanford.edu>, 2005;
 * License:   see LICENSE file.
 */

#include <rtems.h>
#include <sys/errno.h>
#include <sys/types.h>

#include <stdlib.h>

/* Check for RTEMS version; true if >= ma.mi.re */
#define ISMINVERSION(ma,mi,re) \
	(    __RTEMS_MAJOR__  > (ma)	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__  > (mi))	\
	 || (__RTEMS_MAJOR__ == (ma) && __RTEMS_MINOR__ == (mi) && __RTEMS_REVISION__ >= (re)) \
    )

/* 'align' ARG is evaluated more than once */
#define _DO_ALIGN(addr, align) (((uint32_t)(addr) + (align) - 1) & ~((align)-1))


/* malloc/free are redefined :-( */
static inline void *the_real_malloc(size_t n)
{
	return malloc(n);
}

static inline void  the_real_free(void *p)
{
	return free(p);
}

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>
#ifdef __i386__
#include <libcpu/cpu.h>
#elif defined(__PPC__)
#include <libcpu/io.h>
#else
#error "dunno what IO ops to use on this architecture"
#endif
#include <rtems/bspIo.h>

#include "rtemscompat_defs.h"

#define NET_EMB(x,y,z) x ## y ## z
#define NET_EMBEMB(x,y,z) NET_EMB(x,y,z)

#define NET_STR(arg)	#arg
#define NET_STRSTR(arg)	NET_STR(arg)

#define NET_SOFTC 	NET_EMBEMB(,NETDRIVER_PREFIX,_softc)

#define METHODS     NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_methods)
extern struct _net_drv_tbl METHODS;

#ifdef DEBUG_MODULAR
#define METHODSPTR  NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_methods_p)
extern struct _net_drv_tbl *volatile METHODSPTR;
#else
#define METHODSPTR  (&METHODS)
#endif

#if defined(__LITTLE_ENDIAN__) || (__i386__)
static inline uint16_t  htole16(uint16_t  v) { return v; }
static inline uint32_t  htole32(uint32_t  v) { return v; }
static inline uint64_t  htole64(uint64_t  v) { return v; }
static inline uint16_t  le16toh(uint16_t  v) { return v; }
static inline uint32_t  le32toh(uint32_t  v) { return v; }
static inline uint64_t  le64toh(uint64_t  v) { return v; }
#elif defined(__BIG_ENDIAN__)
#ifdef __PPC__
#include <libcpu/byteorder.h>

/* Different RTEMS versions use different types
 * for 32-bit (unsigned vs. unsigned long which
 * always cause gcc warnings and possible alias
 * violations, sigh).
 */

#if ISMINVERSION(4,8,0)
typedef uint32_t rtemscompat_32_t;
#else
typedef unsigned rtemscompat_32_t;
#endif

static inline uint16_t
htole16(uint16_t v)
{
uint16_t rval;
	st_le16(&rval,v);
	return rval;
}

static inline uint16_t
le16toh(uint16_t v)
{
	return ld_le16((unsigned short*)&v);
}

static inline uint32_t
htole32(uint32_t v)
{
rtemscompat_32_t rval;
	st_le32(&rval,v);
	return rval;
}

static inline uint32_t
le32toh(uint32_t v)
{
rtemscompat_32_t vv = v;
	return ld_le32(&vv);
}

/* Compiler generated floating point instructions for this
 * and rtems_bsdnet_newproc()-generated tasks are non-FP
 * :-(
 */
static inline uint64_t
htole64(uint64_t  v) 
{
union {
	rtemscompat_32_t tmp[2];
	uint64_t          rval;
} u;

	st_le32( &u.tmp[0], (unsigned)(v&0xffffffff) );
	st_le32( &u.tmp[1], (unsigned)((v>>32)&0xffffffff) );

	return u.rval;
}

#else
#error "need htoleXX() implementation for this CPU arch"
#endif

#else
#error "Unknown CPU endianness"
#endif



#ifdef __PPC__
#define _out_byte(a,v) out_8((volatile unsigned char*)(a),(v))
#define _inp_byte(a)   in_8((volatile unsigned char*)(a))
#ifdef NET_CHIP_LE
#define _out_word(a,v) out_le16((volatile unsigned short*)(a),(v))
#define _out_long(a,v) out_le32((volatile unsigned *)(a),(v))
#define _inp_word(a)   in_le16((volatile unsigned short*)(a))
#define _inp_long(a)   in_le32((volatile unsigned *)(a))
#elif defined(NET_CHIP_BE)
#define _out_word(a,v) out_be16((volatile unsigned short*)(a),(v))
#define _out_long(a,v) out_be32((volatile unsigned *)(a),(v))
#define _inp_word(a)   in_be16((volatile unsigned short*)(a))
#define _inp_long(a)   in_be32((volatile unsigned *)(a))
#else
#error rtemscompat_defs.h must define either NET_CHIP_LE or NET_CHIP_BE
#endif
static inline void wrle32(unsigned *a, unsigned v)
{
	asm volatile("stwbrx %1,0,%2":"=m"(*a):"r"(v),"r"(a));
}
static inline unsigned rdle32(unsigned *a)
{
	asm volatile("lwbrx %0,0,%0":"=r"(a):"0"(a),"m"(*a));
	return (unsigned)a;
}
static inline void orle32(unsigned *a,unsigned v) { wrle32(a, rdle32(a) | v); }
static inline void anle32(unsigned *a,unsigned v) { wrle32(a, rdle32(a) & v); }

static inline void wrle16(unsigned short *a, unsigned short v)
{
	asm volatile("sthbrx %1,0,%2":"=m"(*a):"r"(v),"r"(a));
}
static inline unsigned short rdle16(unsigned short *a)
{
	asm volatile("lhbrx %0,0,%0":"=r"(a):"0"(a),"m"(*a));
	return (unsigned short)(unsigned)a;
}
static inline void orle16(unsigned short *a,unsigned short v) { wrle16(a, rdle16(a) | v); }
static inline void anle16(unsigned short *a,unsigned short v) { wrle16(a, rdle16(a) & v); }
#endif

#ifdef __i386__
#ifdef NET_CHIP_BE
#error dunno how to output BE data
#endif

static inline void wrle32(volatile unsigned *p, unsigned v) { *p  = v; }
static inline void orle32(volatile unsigned *p, unsigned v) { *p |= v; }
static inline void anle32(volatile unsigned *p, unsigned v) { *p &= v; }
static inline unsigned rdle32(volatile unsigned *p) { return *p; }

static inline void wrle16(volatile unsigned short *p, unsigned short v) { *p  = v; }
static inline void orle16(volatile unsigned short *p, unsigned short v) { *p |= v; }
static inline void anle16(volatile unsigned short *p, unsigned short v) { *p &= v; }
static inline unsigned short rdle16(volatile unsigned short *p) { return *p; }

#ifdef NET_CHIP_MEM_IO

#ifdef __i386__
static inline void           _out_byte(unsigned a, unsigned char v)  {        *(volatile unsigned char*)a = v; }
static inline unsigned char  _inp_byte(unsigned a)                   { return *(volatile unsigned char*)a;     }
#ifdef NET_CHIP_LE
static inline void           _out_word(unsigned a, unsigned short v) {        *(volatile unsigned short*)a = v; }
static inline unsigned short _inp_word(unsigned a)                   { return *(volatile unsigned short*)a;     }
static inline void           _out_long(unsigned a, unsigned v)       {        *(volatile unsigned      *)a = v; }
static inline unsigned       _inp_long(unsigned a)                   { return *(volatile unsigned      *)a; }
#elif defined(NET_CHIP_BE)
#error "BE memory IO not implemented for i386 yet"
#else
#error rtemscompat_defs.h must define either NET_CHIP_LE or NET_CHIP_BE
#endif
#else

#error "Memory IO not implemented for this CPU architecture yet"

#endif
#elif defined(NET_CHIP_PORT_IO)
#define _out_byte(addr,val) outport_byte((addr),(val))
#define _out_word(addr,val) outport_word((addr),(val))
#define _out_long(addr,val) outport_long((addr),(val))

static inline u_int8_t _inp_byte(volatile unsigned char *a)
{
register u_int8_t rval;
	inport_byte((unsigned short)(unsigned)a,rval);
	return rval;
}
static inline u_int16_t _inp_word(volatile unsigned short *a)
{
register u_int16_t rval;
	inport_word((unsigned short)(unsigned)a,rval);
	return rval;
}
static inline u_int32_t _inp_long(volatile unsigned *a)
{
register u_int32_t rval;
	inport_long((unsigned short)(unsigned)a,rval);
	return rval;
}
#else
#error either NET_CHIP_MEM_IO or NET_CHIP_PORT_IO must be defined
#endif
#endif

#ifndef __FBSDID
#define __FBSDID(arg)
#endif

#define _KERNEL

#define device_printf(device,format,args...) printk(format,## args)

static inline u_int8_t bus_space_do_read_1(u_long handle, unsigned reg)
{
	return _inp_byte((volatile unsigned char*)((handle)+(reg)));
}

static inline u_int16_t bus_space_do_read_2(u_long handle, unsigned reg)
{
	return _inp_word((volatile unsigned short*)((handle)+(reg)));
}

static inline u_int32_t bus_space_do_read_4(u_long handle, unsigned reg)
{
	return _inp_long((volatile unsigned *)((handle)+(reg)));
}

#define bus_space_read_1(tag,handle,reg) bus_space_do_read_1((handle),(reg))
#define bus_space_read_2(tag,handle,reg) bus_space_do_read_2((handle),(reg))
#define bus_space_read_4(tag,handle,reg) bus_space_do_read_4((handle),(reg))

static inline void bus_space_do_write_multi_1(u_long handle, unsigned reg, unsigned char *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++) _out_byte( (handle) + (reg), (addr)[i]);
}

static inline void bus_space_do_write_multi_2(u_long handle, unsigned reg, unsigned short *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++) _out_word( (handle) + (reg), (addr)[i]);
}

static inline void bus_space_do_write_multi_4(u_long handle, unsigned reg, unsigned long *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++) _out_long( (handle) + (reg), (addr)[i]);
}


#define bus_space_write_multi_1(tag, handle, reg, addr, cnt) \
		bus_space_do_write_multi_1(handle, reg, addr, cnt)
#define bus_space_write_multi_2(tag, handle, reg, addr, cnt) \
		bus_space_do_write_multi_2(handle, reg, addr, cnt)
#define bus_space_write_multi_4(tag, handle, reg, addr, cnt) \
		bus_space_do_write_multi_4(handle, reg, addr, cnt)

static inline void bus_space_do_read_multi_1(u_long handle, unsigned reg, unsigned char *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++)
				(addr)[i] = _inp_byte((volatile unsigned char*)((handle)+(reg)));
}

static inline void bus_space_do_read_multi_2(u_long handle, unsigned reg, unsigned short *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++)
				(addr)[i] = _inp_word((volatile unsigned short*)((handle)+(reg)));
}

static inline void bus_space_do_read_multi_4(u_long handle, unsigned reg, unsigned long *addr, int cnt)
{
	int i; for (i=0; i<cnt; i++)
				(addr)[i] = _inp_long((volatile unsigned *)((handle)+(reg)));
}

#define bus_space_read_multi_1(tag, handle, reg, addr, cnt) \
		bus_space_do_read_multi_1(handle, reg, addr, cnt)
#define bus_space_read_multi_2(tag, handle, reg, addr, cnt) \
		bus_space_do_read_multi_2(handle, reg, addr, cnt)
#define bus_space_read_multi_4(tag, handle, reg, addr, cnt) \
		bus_space_do_read_multi_4(handle, reg, addr, cnt)



#define bus_space_write_1(tag, handle, reg, val) \
	do { _out_byte( (handle) + (reg), (val)); } while (0)

#define bus_space_write_2(tag, handle, reg, val) \
	do { _out_word( (handle) + (reg), (val)); } while (0)

#define bus_space_write_4(tag, handle, reg, val) \
	do { _out_long( (handle) + (reg), (val)); } while (0)

#define BPF_MTAP(a,b)	do { } while (0)

extern unsigned net_driver_ticks_per_sec;

#ifdef __PPC__
/* PPC has a timebase - based delay */
#define DELAY(n) do {		\
	if ( (n) > 10000 )		\
		rtems_task_wake_after((((n)*net_driver_ticks_per_sec)/1000000) + 1);	\
	else					\
		rtems_bsp_delay(n);	\
	} while (0)
#else
#warning "Have no good usec delay implementation"
#define DELAY(n) do {		\
		rtems_task_wake_after((((n)*net_driver_ticks_per_sec)/1000000) + 1);	\
	} while (0)
#endif


#define IRQ_LOCKED(code) \
	do { unsigned long _xtre_irq_flags; \
		rtems_interrupt_disable(_xtre_irq_flags); \
			do { code } while(0); 	\
		rtems_interrupt_enable(_xtre_irq_flags); \
	} while (0)

typedef struct _netdev_t *device_t;
typedef void (driver_intr_t)(void *);

#define if_xname if_name

/* need to replace those with LOCAL2PCI() and make sure the bus handle is initialized
 * (on most BSPs we get away with PCI_DRAM_OFFSET [no bus handle needed at all]
 */
#ifndef PCI_DRAM_OFFSET
#define PCI_DRAM_OFFSET 0
#endif

#ifndef PCI_MEM_BASE
#define PCI_MEM_BASE    0
#endif

#define kvtop(a)			((unsigned long)(a) + PCI_DRAM_OFFSET)
#define vtophys(a)			((unsigned long)(a) + PCI_DRAM_OFFSET)

#define PCI2LOCAL(a,bus)	((unsigned long)(a) + PCI_MEM_BASE)

#ifdef PCI0_IO_BASE /* special mvme5500 hack :-( */
#define PCI_IO_2LOCAL(a,bus) ((unsigned long)(a) + PCI0_IO_BASE)
#elif defined(PCI_IO_BASE)
#define PCI_IO_2LOCAL(a,bus) ((unsigned long)(a) + PCI_IO_BASE)
#elif defined(_IO_BASE)
#define PCI_IO_2LOCAL(a,bus) ((unsigned long)(a) + _IO_BASE)
#else
#warning "Unable to determine base address of PCI IO space; using ZERO"
#define PCI_IO_2LOCAL(a,bus) ((unsigned long)(a))
#endif

#define if_printf(if,fmt,args...)  printf("%s:"fmt,(if)->if_name,args)

#ifndef BUS_PROBE_DEFAULT
#define BUS_PROBE_DEFAULT 0
#endif

static inline void *
contigmalloc(
	unsigned long size,
	int type,
	int flags,
	unsigned long lo,
	unsigned long hi,
	unsigned long align,
	unsigned long bound)
{
void *ptr  = rtems_bsdnet_malloc(size + sizeof(ptr) + align-1, type, flags);
char *rval = 0;
	if ( ptr ) {
		unsigned tmp = (unsigned)ptr + align - 1;
		tmp -= tmp % align;
		rval = (char*)tmp;
		/* save backlink */
		*(void**)(rval+size) =  ptr;
	}
	return rval;
}

static inline void
contigfree(void *ptr, size_t size, int type)
{
	rtems_bsdnet_free( *(void**)((unsigned)ptr + size), type);
}

/* callout stuff */
#define callout_init(args...) do {} while (0);
#define callout_reset(args...) do {} while (0);
#define callout_stop(args...) do {} while (0);

#define IFQ_DRV_IS_EMPTY(q) (0 == (q)->ifq_head)
#define IFQ_DRV_DEQUEUE(q,m) IF_DEQUEUE((q),(m))
#define IFQ_DRV_PREPEND(q,m) IF_PREPEND((q),(m))

#define DO_ETHER_INPUT_SKIPPING_ETHER_HEADER(ifp,m)				\
		{	struct ether_header *eh;							\
			eh				  = mtod(m, struct ether_header*);	\
       		m->m_data        += sizeof(struct ether_header);	\
        	m->m_len         -= sizeof(struct ether_header);	\
        	m->m_pkthdr.len  -= sizeof(struct ether_header);	\
			m->m_pkthdr.rcvif = ifp;							\
    		ether_input(ifp, eh, m);							\
		} while (0)


#ifndef __KERNEL_RCSID
#define __KERNEL_RCSID(arg...) 
#endif

#endif
