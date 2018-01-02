/* Routines to access PCI memory/configuration space and other PCI related
 * functions the PCI Library provides.
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#ifndef __PCI_ACCESS_H__
#define __PCI_ACCESS_H__

#include <stdint.h>
#include <libcpu/byteorder.h>
#include <rtems/score/basedefs.h>
#include <pci.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Identification of a PCI configuration space device (16-bit) */
typedef uint16_t pci_dev_t;
/* Create a PCI Configuration Space ID */
#define PCI_DEV(bus, slot, func) (((bus)<<8) | ((slot)<<3) | (func))
/* Get Bus of a PCI Configuration Space ID */
#define PCI_DEV_BUS(dev) (((dev) >> 8) & 0xff)
/* Get Slot/Device of a PCI Configuration Space ID */
#define PCI_DEV_SLOT(dev) (((dev) >> 3) & 0x1f)
/* Get Function of a PCI Configuration Space ID */
#define PCI_DEV_FUNC(dev) ((dev) & 0x7)
/* Get Device and Function of a PCI Configuration Space ID */
#define PCI_DEV_DEVFUNC(dev) ((dev) & 0xff)
/* Expand Device into argument lists */
#define PCI_DEV_EXPAND(dev) PCI_DEV_BUS((dev)), PCI_DEV_SLOT((dev)), PCI_DEV_FUNC((dev))

/* Configuration Space Read/Write Operations */
struct pci_cfg_ops {
	/* Configuration Space Access and Setup Routines */
	int (*read8)(pci_dev_t dev, int ofs, uint8_t *data);
	int (*read16)(pci_dev_t dev, int ofs, uint16_t *data);
	int (*read32)(pci_dev_t dev, int ofs, uint32_t *data);
	int (*write8)(pci_dev_t dev, int ofs, uint8_t data);
	int (*write16)(pci_dev_t dev, int ofs, uint16_t data);
	int (*write32)(pci_dev_t dev, int ofs, uint32_t data);
};

/* Read a register over PCI I/O Space, and swap it if necessary (due to 
 * PCI endianness)
 */
struct pci_io_ops {
	uint8_t (*read8)(uint8_t *adr);
	uint16_t(*read16)(uint16_t *adr);
	uint32_t (*read32)(uint32_t *adr);
	void (*write8)(uint8_t *adr, uint8_t data);
	void (*write16)(uint16_t *adr, uint16_t data);
	void (*write32)(uint32_t *adr, uint32_t data);
};

/* Read a register over PCI Memory Space (non-prefetchable memory), and 
 * swap it if necessary (due to PCI endianness)
 */
struct pci_memreg_ops {
	uint8_t (*ld8)(uint8_t *adr);
	void (*st8)(uint8_t *adr, uint8_t data);

	uint16_t(*ld_le16)(uint16_t *adr);
	void (*st_le16)(uint16_t *adr, uint16_t data);
	uint16_t(*ld_be16)(uint16_t *adr);
	void (*st_be16)(uint16_t *adr, uint16_t data);

	uint32_t (*ld_le32)(uint32_t *adr);
	void (*st_le32)(uint32_t *adr, uint32_t data);
	uint32_t (*ld_be32)(uint32_t *adr);
	void (*st_be32)(uint32_t *adr, uint32_t data);
};

typedef uint8_t (*pci_ld8_t)(uint8_t *adr);
typedef void (*pci_st8_t)(uint8_t *adr, uint8_t data);
typedef uint16_t(pci_ld16_t)(uint16_t *adr);
typedef void (*pci_st16_t)(uint16_t *adr, uint16_t data);
typedef uint32_t (*pci_ld32_t)(uint32_t *adr);
typedef void (*pci_st32_t)(uint32_t *adr, uint32_t data);

struct pci_access_drv {
	/* Configuration */
	struct pci_cfg_ops cfg;

	/* I/O Access operations */
	struct pci_io_ops io;

	/* Registers over Memory Access operations. Note that these funcs
	 * are only for code that need to be compatible with both Big-Endian
	 * and Little-Endian PCI bus or for some other reason need function
	 * pointers to access functions. Normally drivers use the inline
	 * functions for Registers-over-Memory access to avoid extra function
	 * call.
	 */
	struct pci_memreg_ops *memreg;

	/* Translate from PCI address to CPU address (dir=0). Translate
	 * CPU address to PCI address (dir!=0). The address will can be
	 * used to perform I/O access or memory access by CPU or PCI DMA
	 * peripheral.
	 *
	 * address    In/Out. CPU address or PCI address.
	 * type       Access type. 1=I/O, 2=MEMIO, 3=MEM
	 * dir        Translate direction. 0=PCI-to-CPU, 0!=CPU-to-PCI,
	 *
	 * Return Value
	 *  0   = Success
	 *  -1  = Requested Address not mapped into other address space
	 *        i.e. not accessible
	 */
	int (*translate)(uint32_t *address, int type, int dir);
};

/* Access Routines valid after a PCI-Access-Driver has registered */
extern struct pci_access_drv pci_access_ops;

/* Register PCI Access Driver */
extern int pci_access_drv_register(struct pci_access_drv *drv);

/* Set/unset bits in command and status register of a PCI device */
extern void pci_modify_cmdsts(pci_dev_t dev, uint32_t mask, uint32_t val);

/* Enable Memory in command register */
RTEMS_INLINE_ROUTINE void pci_mem_enable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_MEMEN, PCIM_CMD_MEMEN);
}

RTEMS_INLINE_ROUTINE void pci_mem_disable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_MEMEN, 0);
}

RTEMS_INLINE_ROUTINE void pci_io_enable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_PORTEN, PCIM_CMD_PORTEN);
}

RTEMS_INLINE_ROUTINE void pci_io_disable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_PORTEN, 0);
}

RTEMS_INLINE_ROUTINE void pci_master_enable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_BUSMASTEREN, PCIM_CMD_BUSMASTEREN);
}

RTEMS_INLINE_ROUTINE void pci_master_disable(pci_dev_t dev)
{
	pci_modify_cmdsts(dev, PCIM_CMD_BUSMASTEREN, 0);
}

/* Configuration Space Access Read Routines */
extern int pci_cfg_r8(pci_dev_t dev, int ofs, uint8_t *data);
extern int pci_cfg_r16(pci_dev_t dev, int ofs, uint16_t *data);
extern int pci_cfg_r32(pci_dev_t dev, int ofs, uint32_t *data);

/* Configuration Space Access Write Routines */
extern int pci_cfg_w8(pci_dev_t dev, int ofs, uint8_t data);
extern int pci_cfg_w16(pci_dev_t dev, int ofs, uint16_t data);
extern int pci_cfg_w32(pci_dev_t dev, int ofs, uint32_t data);

/* Read a register over PCI I/O Space */
extern uint8_t pci_io_r8(uint32_t adr);
extern uint16_t pci_io_r16(uint32_t adr);
extern uint32_t pci_io_r32(uint32_t adr);

/* Write a register over PCI I/O Space */
extern void pci_io_w8(uint32_t adr, uint8_t data);
extern void pci_io_w16(uint32_t adr, uint16_t data);
extern void pci_io_w32(uint32_t adr, uint32_t data);

/* Translate PCI address into CPU accessible address */
RTEMS_INLINE_ROUTINE int pci_pci2cpu(uint32_t *address, int type)
{
	return pci_access_ops.translate(address, type, 0);
}

/* Translate CPU accessible address into PCI address (for DMA) */
RTEMS_INLINE_ROUTINE int pci_cpu2pci(uint32_t *address, int type)
{
	return pci_access_ops.translate(address, type, 1);
}

/*** Read/Write a register over PCI Memory Space ***/

RTEMS_INLINE_ROUTINE uint8_t pci_ld8(volatile uint8_t *addr)
{
	return *addr;
}

RTEMS_INLINE_ROUTINE void pci_st8(volatile uint8_t *addr, uint8_t val)
{
	*addr = val;
}

/* Registers-over-Memory Space access routines. The routines are not inlined
 * so it is possible during run-time to select which function implemention
 * to use. The use of these functions are not recommended since it will have a
 * performance penalty.
 *
 * 8-bit accesses are the same for Little and Big endian PCI buses.
 */
uint8_t pci_mem_ld8(uint8_t *adr);
void pci_mem_st8(uint8_t *adr, uint8_t data);
/* Registers-over-Memory Space - Generic Big endian PCI bus definitions */
uint16_t pci_mem_be_ld_le16(uint16_t *adr);
uint16_t pci_mem_be_ld_be16(uint16_t *adr);
uint32_t pci_mem_be_ld_le32(uint32_t *adr);
uint32_t pci_mem_be_ld_be32(uint32_t *adr);
void pci_mem_be_st_le16(uint16_t *adr, uint16_t data);
void pci_mem_be_st_be16(uint16_t *adr, uint16_t data);
void pci_mem_be_st_le32(uint32_t *adr, uint32_t data);
void pci_mem_be_st_be32(uint32_t *adr, uint32_t data);
/* Registers-over-Memory Space - Generic Little endian PCI bus definitions */
uint16_t pci_mem_le_ld_le16(uint16_t *adr);
uint16_t pci_mem_le_ld_be16(uint16_t *adr);
uint32_t pci_mem_le_ld_le32(uint32_t *adr);
uint32_t pci_mem_le_ld_be32(uint32_t *adr);
void pci_mem_le_st_le16(uint16_t *adr, uint16_t data);
void pci_mem_le_st_be16(uint16_t *adr, uint16_t data);
void pci_mem_le_st_le32(uint32_t *adr, uint32_t data);
void pci_mem_le_st_be32(uint32_t *adr, uint32_t data);

/* Get Read/Write function for accessing a register over PCI Memory Space
 * (non-inline functions).
 *
 * Arguments
 *  wr             0(Read), 1(Write)
 *  size           1(Byte), 2(Word), 4(Double Word)
 *  func           Where function pointer will be stored
 *  endian         PCI_LITTLE_ENDIAN or PCI_BIG_ENDIAN
 *  type           1(I/O), 3(REG over MEM), 4(CFG)
 *
 * Return
 *  0              Found function
 *  others         No such function defined by host driver or BSP
 */
extern int pci_access_func(int wr, int size, void **func, int endian, int type);

/* Predefined functions for Host drivers or BSPs that define the 
 * register-over-memory space functions operations.
 */
extern struct pci_memreg_ops pci_mem_le_ops; /* For Little-Endian PCI bus */
extern struct pci_memreg_ops pci_mem_be_ops; /* For Big-Endian PCI bus */

#ifdef __cplusplus
}
#endif

#endif /* !__PCI_ACCESS_H__ */
