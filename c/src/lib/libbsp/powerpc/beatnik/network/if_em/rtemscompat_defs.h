#ifndef RTEMS_COMPAT_DEFS_H
#define RTEMS_COMPAT_DEFS_H

/* Number of device instances the driver should support
 * - may be limited to 1 depending on IRQ API
 * (braindamaged PC586 and powerpc)
 */
#define NETDRIVER_SLOTS 1

/* String name to print with error messages */
#define NETDRIVER       "em"
/* Name snippet used to make global symbols unique to this driver */
#define NETDRIVER_PREFIX em

#define adapter			em_softc
#define interface_data	arpcom

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
#define NET_SOFTC_BHANDLE_FIELD osdep.mem_bus_space_handle

/* define the names of the 'if_XXXreg.h' and 'if_XXXvar.h' headers
 * (only if present, i.e., if the BSDNET driver has no respective
 * header, leave this undefined).
 *
 */
#define  IF_REG_HEADER <if_em.h>
#undef  IF_VAR_HEADER

/* define if a pci device */
#define NETDRIVER_PCI <bsp/pci.h>

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

#define NET_ENABLE_IRQS(sc)	do { \
		E1000_WRITE_REG(&sc->hw, IMS, (IMS_ENABLE_MASK)); \
		} while (0)

#define NET_DISABLE_IRQS(sc)	do { \
		E1000_WRITE_REG(&sc->hw, IMC, 0xffffffff);	\
		} while (0)
		
#define KASSERT(a...) do {} while (0)

/* dmamap stuff; these are defined just to work with the current version
 * of this driver and the implementation must be carefully checked if
 * a newer version is merged.!
 *
 * The more cumbersome routines have been commented in the source, the
 * simpler ones are defined to be NOOPs here so the source gets less
 * cluttered...
 *
 * ASSUMPTIONS:
 *
 *	  -> dmamap_sync cannot sync caches; assume we have HW snooping
 *
 */

typedef unsigned bus_size_t;
typedef unsigned bus_addr_t;

typedef struct {
	unsigned	ds_addr;
	unsigned	ds_len;
} bus_dma_segment_t;

#define bus_dma_tag_destroy(args...) do {} while(0)

#define bus_dmamap_destroy(args...) do {} while(0)

#define bus_dmamap_unload(args...) do {} while (0)

#ifdef __PPC__
#define bus_dmamap_sync(args...) do { __asm__ volatile("sync":::"memory"); } while (0)
#else
#define bus_dmamap_sync(args...) do {} while (0)
#endif

#define BUS_DMA_NOWAIT		0xdeadbeef	/* unused */

#endif
