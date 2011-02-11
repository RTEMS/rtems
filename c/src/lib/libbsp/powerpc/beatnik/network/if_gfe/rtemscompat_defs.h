#ifndef RTEMS_COMPAT_DEFS_H
#define RTEMS_COMPAT_DEFS_H

/* Number of device instances the driver should support
 * - may be limited to 1 depending on IRQ API
 * (braindamaged PC586 and powerpc)
 */
#define NETDRIVER_SLOTS 1
/* String name to print with error messages */
#define NETDRIVER       "gfe"
/* Name snippet used to make global symbols unique to this driver */
#define NETDRIVER_PREFIX gfe

/* Define according to endianness of the *ethernet*chip*
 * (not the CPU - most probably are LE)
 * This must be either NET_CHIP_LE or NET_CHIP_BE
 */

#define NET_CHIP_LE
#undef  NET_CHIP_BE

/* Define either NET_CHIP_MEM_IO or NET_CHIP_PORT_IO,
 * depending whether the CPU sees it in memory address space
 * or (e.g. x86) uses special I/O instructions.
 */
#define NET_CHIP_MEM_IO
#undef  NET_CHIP_PORT_IO

/* The name of the hijacked 'bus handle' field in the softc
 * structure. We use this field to store the chip's base address.
 */
#define NET_SOFTC_BHANDLE_FIELD sc_memh

/* define the names of the 'if_XXXreg.h' and 'if_XXXvar.h' headers
 * (only if present, i.e., if the BSDNET driver has no respective
 * header, leave this undefined).
 *
 */
#undef  IF_REG_HEADER
#define  IF_VAR_HEADER <if_gfevar.h>

/* define if a pci device */
/*
#define NETDRIVER_PCI <bsp/pci.h>
*/
#undef NETDRIVER_PCI

/* Macros to disable and enable interrupts, respectively.
 * The 'disable' macro is expanded in the ISR, the 'enable'
 * macro is expanded in the driver task.
 * The global network semaphore usually provides mutex
 * protection of the device registers.
 * Special care must be taken when coding the 'disable' macro,
 * however to MAKE SURE THERE ARE NO OTHER SIDE EFFECTS such
 * as:
 *    - macro must not clear any status flags
 *    - macro must save/restore any context information
 *      (e.g., a address register pointer or a bank switch register)
 *
 * ARGUMENT: the macro arg is a pointer to the driver's 'softc' structure
 */

#define NET_DISABLE_IRQS(sc)	GE_WRITE(sc, EIMR, 0)
#define NET_ENABLE_IRQS(sc)		GE_WRITE(sc, EIMR, sc->sc_intrmask)

/* Driver may provide a macro/function to copy the hardware address
 * from the device into 'softc.arpcom'.
 * If this is undefined, the driver must to the copy itself.
 * Preferrably, it should check soft.arpcom.ac_enaddr for all
 * zeros and leave it alone if it is nonzero, i.e., write it
 * to the hardware.
#define NET_READ_MAC_ADDR(sc)
 */

typedef struct {
	uint32_t	ds_addr;
	uint32_t	ds_len;
} bus_dma_segment_t;

#define dm_segs		gdm_segs
#define dm_nsegs	gdm_nsegs
typedef struct gfe_dmamem *bus_dmamap_t;

typedef uint32_t bus_addr_t;
typedef uint32_t bus_size_t;

typedef struct device blah;

#define BUS_DMA_NOCACHE	0xdeadbeef

#ifdef __PPC__
#define bus_dmamap_sync(args...) do { __asm__ volatile("sync":::"memory"); } while(0)
#else
#error "Dont' know how to sync memory on your CPU"
#endif

int	ether_sprintf_r(const unsigned char *enaddr, char *buf, int len);

/* we have it although we're not ansi */
int snprintf(char *, size_t, const char *,...);

#include <string.h>

/* declare in every routine using ether_sprintf */
#define SPRINTFVARDECL	char rtems_sprintf_local_buf[3*6]	/* ethernet string */

#define ether_sprintf_macro(a)					\
	(snprintf(rtems_sprintf_local_buf,			\
			sizeof(rtems_sprintf_local_buf),	\
			"%02X:%02X:%02X:%02X:%02X:%02X",	\
			a[0],a[1],a[2],a[3],a[4],a[5]) ? 	\
			rtems_sprintf_local_buf : 0		\
	)


#define aprint_normal(args...)	printf(args)
#define aprint_error(args...)	fprintf(stderr,args)

#define delay(arg)	DELAY(arg)

#define KASSERT(a...) do {} while (0)
#endif
