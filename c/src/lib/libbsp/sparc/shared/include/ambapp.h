/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2009.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __AMBAPP_H__
#define __AMBAPP_H__

/* Include VENDOR and DEVICE definitions */
#include <ambapp_ids.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Max supported AHB buses */
#define AHB_BUS_MAX 6

struct ambapp_dev;
struct ambapp_core;
struct ambapp_apb_info;
struct ambapp_ahb_info;

struct ambapp_dev {
	struct ambapp_dev *next;	/* Next */
	struct ambapp_dev *prev;	/* Previous Device. If (this == 
					 * rev->child) prev is bus bridge */
	struct ambapp_dev *children;	/* Points to first device on sub-bus */
	void *owner;			/* Owner of this AMBA device */
	unsigned char dev_type;		/* AHB MST, AHB SLV or APB SLV*/
	unsigned char vendor;		/* Vendor ID */
	unsigned short device;		/* Device ID */
	int devinfo[0];			/* Device info (APB/AHB dep. on type) */
};

#define AMBAPP_FLAG_FFACT_DIR	0x100	/* Frequency factor direction, 0=down, 1=up */
#define AMBAPP_FLAG_FFACT	0x0f0	/* Frequency factor against top bus */
#define AMBAPP_FLAG_MBUS	0x00c
#define AMBAPP_FLAG_SBUS	0x003

/* Get APB or AHB information from a AMBA device */
#define DEV_TO_APB(adev) ((struct ambapp_apb_info *)((adev)->devinfo))
#define DEV_TO_AHB(adev) ((struct ambapp_ahb_info *)((adev)->devinfo))
#define DEV_TO_COMMON(adev) ((struct ambapp_common_info *)((adev)->devinfo))
/* Convert address of ambapp_apb_info/ambapp_ahb_info into ambapp_dev */
#define APB_TO_DEV(apb_info) ((struct ambapp_dev *)(unsigned int(apb_info) - \
				offsetof(struct ambapp_dev, devinfo)))
#define AHB_TO_DEV(ahb_info) ((struct ambapp_dev *)(unsigned int(ahb_info) - \
				offsetof(struct ambapp_dev, devinfo)))

struct ambapp_common_info {
	unsigned char irq;
	unsigned char ver;
	unsigned char ahbidx;	/* AHB Bus Index */
};

struct ambapp_apb_info {
	/* COMMON */
	unsigned char irq;
	unsigned char ver;
	unsigned char ahbidx;	/* AHB Bus Index */

	/* APB SPECIFIC */
	unsigned int start;
	unsigned int mask;
};

struct ambapp_ahb_info {
	/* COMMON */
	unsigned char irq;
	unsigned char ver;
	unsigned char ahbidx;	/* AHB Bus Index */

	/* AHB SPECIFIC */
	unsigned int start[4];
	unsigned int mask[4];
	char type[4];		/* type[N] Determine type of start[N]-mask[N],
				 * 2=AHB Memory Space, 3=AHB I/O Space */
	unsigned int custom[3];
};

/* Describes a complete AMBA Core. Each device may consist of 3 interfaces */
struct ambapp_core {
	char			irq;		/* irq=-1 indicate no IRQ */
	unsigned char		vendor;
	unsigned short		device;
	int			index;		/* Core index */
	struct ambapp_ahb_info	*ahb_mst;
	struct ambapp_ahb_info	*ahb_slv;
	struct ambapp_apb_info	*apb_slv;
};

struct ambapp_ahb_bus {
	unsigned int ioarea;	/* AHB Bus IOAREA */
	unsigned int freq_hz;	/* Frequency of AHB Bus */
	struct ambapp_dev *bridge;/* Bridge Device on Parent AHB Bus */
	struct ambapp_dev *dev;	/* First Device on AHB Bus */
};

struct ambapp_mmap {
	unsigned int		size;
	unsigned int		local_adr;
	unsigned int		remote_adr;
};

/* Complete AMBA PnP information */
struct ambapp_bus {
	struct ambapp_dev	*root;			/* AHB/APB Device Tree*/
	struct ambapp_mmap	*mmaps;			/* Memory MAP Array */
	struct ambapp_ahb_bus	ahbs[AHB_BUS_MAX];	/* AHB Buses */
};

/* 
 * Return values
 *  0 - continue
 *  1 - stop scanning
 */
typedef int (*ambapp_func_t)(struct ambapp_dev *dev, int index, void *arg);

#define DEV_IS_FREE(dev) (dev->owner == NULL)
#define DEV_IS_ALLOCATED(dev) (dev->owner != NULL)

/* Options to ambapp_for_each */
#define OPTIONS_AHB_MSTS	0x00000001
#define OPTIONS_AHB_SLVS	0x00000002
#define OPTIONS_APB_SLVS	0x00000004
#define OPTIONS_ALL_DEVS	(OPTIONS_AHB_MSTS|OPTIONS_AHB_SLVS|OPTIONS_APB_SLVS)

#define OPTIONS_FREE		0x00000010
#define OPTIONS_ALLOCATED	0x00000020
#define OPTIONS_ALL		(OPTIONS_FREE|OPTIONS_ALLOCATED)

/* Depth first search, Defualt is breath first search. */
#define OPTIONS_DEPTH_FIRST	0x00000100

#define DEV_AHB_NONE 0
#define DEV_AHB_MST  1
#define DEV_AHB_SLV  2
#define DEV_APB_SLV 3

/* Structures used to access Plug&Play information directly */
struct ambapp_pnp_ahb {
	const unsigned int	id;		/* VENDOR, DEVICE, VER, IRQ, */
	const unsigned int	custom[3];
	const unsigned int	mbar[4];	/* MASK, ADDRESS, TYPE, CACHABLE/PREFETCHABLE */
};

struct ambapp_pnp_apb {
	const unsigned int	id;		/* VENDOR, DEVICE, VER, IRQ, */
	const unsigned int	iobar;		/* MASK, ADDRESS, TYPE, CACHABLE/PREFETCHABLE */
};

#define ambapp_pnp_vendor(id) (((id) >> 24) & 0xff)
#define ambapp_pnp_device(id) (((id) >> 12) & 0xfff)
#define ambapp_pnp_ver(id) (((id)>>5) & 0x1f)
#define ambapp_pnp_irq(id) ((id) & 0x1f)

#define ambapp_pnp_start(mbar)  (((mbar) & 0xfff00000) & (((mbar) & 0xfff0) << 16)) 
#define ambapp_pnp_mbar_mask(mbar) (((mbar)>>4) & 0xfff)
#define ambapp_pnp_mbar_type(mbar) ((mbar) & 0xf)

#define ambapp_pnp_apb_start(iobar, base) ((base) | ((((iobar) & 0xfff00000)>>12) & (((iobar) & 0xfff0)<<4)) )
#define ambapp_pnp_apb_mask(iobar) ((~(ambapp_pnp_mbar_mask(iobar)<<8) & 0x000fffff) + 1)

#define AMBA_TYPE_AHBIO_ADDR(addr,base_ioarea) ((unsigned int)(base_ioarea) | ((addr) >> 12))

#define AMBA_TYPE_APBIO 0x1
#define AMBA_TYPE_MEM   0x2
#define AMBA_TYPE_AHBIO 0x3

/* Copy Data from AMBA PnP I/O Area */
typedef void *(*ambapp_memcpy_t)(
	void *dest,		/* Destination RAM copy */
	const void *src,	/* Source AMBA PnP Address to copy from */
	int n,			/* Number of bytes to be copied */
	struct ambapp_bus *abus	/* Optional AMBA Bus pointer */
	);

/* Scan a AMBA Plug & Play bus and create all device structures describing the 
 * the devices. The devices will form a tree, where every node describes one
 * interface. The resulting tree is placed in the location pointed to by root.
 *
 * Since it the tree is located in RAM it is easier to work with AMBA buses
 * that is located over PCI and SpaceWire etc.
 *
 * \param ioarea   The IO-AREA where Plug & Play information can be found.
 * \param parent   Used internally when recursing down a bridge. Set to NULL.
 * \param mmaps    Is used to perform address translation if needed.
 * \param root     Resulting device node tree root is stored here.
 *
 */
extern int ambapp_scan(
	struct ambapp_bus *abus,
	unsigned int ioarea,
	ambapp_memcpy_t memfunc,
	struct ambapp_mmap *mmaps
	);

/* Initialize the frequency [Hz] of all AHB Buses from knowing the frequency
 * of one particular APB/AHB Device.
 */
extern void ambapp_freq_init(
	struct ambapp_bus *abus,
	struct ambapp_dev *dev,
	unsigned int freq);

/* Returns the frequency [Hz] of a AHB/APB device */
extern unsigned int ambapp_freq_get(
	struct ambapp_bus *abus,
	struct ambapp_dev *dev);

/* Iterates through all AMBA devices previously found, it calls func 
 * once for every device that match the search arguments.
 *
 * SEARCH OPTIONS
 * All search options must be fulfilled, type of devices searched (options)
 * and AMBA Plug&Play ID [VENDOR,DEVICE], before func() is called. The options
 * can be use to search only for AMBA APB or AHB Slaves or AHB Masters for
 * example. Note that when VENDOR=-1 or DEVICE=-1 it will match any vendor or
 * device ID, this means setting both VENDOR and DEVICE to -1 will result in
 * calling all devices matches the options argument.
 *
 * \param abus     AMBAPP Bus to search
 * \param options  Search options, see OPTIONS_* above
 * \param vendor   AMBAPP VENDOR ID to search for
 * \param device   AMBAPP DEVICE ID to search for
 * \param func     Function called for every device matching search options
 * \param arg      Optional argument passed on to func
 *
 * func return value affects the search, returning a non-zero value will
 * stop the search and ambapp_for_each will return immediately returning the
 * same non-zero value.
 *
 * Return Values
 *  0 - all devices was scanned
 *  non-zero - stopped by user function returning the non-zero value
 */
extern int ambapp_for_each(
	struct ambapp_bus *abus,
	unsigned int options,
	int vendor,
	int device,
	ambapp_func_t func,
	void *arg);

/* Helper function for ambapp_for_each(), find a device by index. If pcount
 * is NULL the first device is returned, else pcount is interpreted as index
 * by decrementing the value until zero is reaced: *count=0 first device,
 * *count=1 second device etc.
 *
 * The matching device is returned, which will stop the ambapp_for_each search.
 * If zero is returned from ambapp_for_each no device matching the index was
 * found
 */
extern int ambapp_find_by_idx(struct ambapp_dev *dev, int index, void *pcount);

/* Get number of devices matching the options/vendor/device arguments, the
 * arguments are passed onto ambapp_for_each().
 */
extern int ambapp_dev_count(struct ambapp_bus *abus, unsigned int options,
				int vendor, int device);

/* Print short information about devices on the AMBA bus onto the console */
extern void ambapp_print(struct ambapp_bus *abus, int show_depth);

/* Mark a device taken (allocate), Owner field is set with owner Data. Returns
 * -1 if device has already been allocated.
 */
extern int ambapp_alloc_dev(struct ambapp_dev *dev, void *owner);

/* Owner field is cleared, which indicates that device is not allocated */
extern void ambapp_free_dev(struct ambapp_dev *dev);

/* Find AHB/APB Bridge or AHB/AHB Bridge Parent */
extern struct ambapp_dev *ambapp_find_parent(struct ambapp_dev *dev);

/* Returns bus depth (number of sub AHB buses) of device from root bus */
extern int ambapp_depth(struct ambapp_dev *dev);

/* Get Device Name from AMBA PnP name database */
extern char *ambapp_device_id2str(int vendor, int id);

/* Get Vendor Name from AMBA PnP name database */
extern char *ambapp_vendor_id2str(int vendor);

/* Set together VENDOR_DEVICE Name from AMBA PnP name database. Return length
 * of C-string stored in buf not including string termination '\0'.
 */
extern int ambapp_vendev_id2str(int vendor, int id, char *buf);

/* Help functions for backwards compability */

extern int ambapp_find_apbslv(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_apb_info *dev);

extern int ambapp_find_apbslv_next(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_apb_info *dev,
	int index);

extern int ambapp_find_apbslvs_next(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_apb_info *dev,
	int index,
	int maxno);

extern int ambapp_find_apbslvs(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_apb_info *dev,
	int maxno);

extern int ambapp_find_ahbslv(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_ahb_info *dev);

extern int ambapp_find_ahbslv_next(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_ahb_info *dev,
	int index);

extern int ambapp_find_ahbslvs_next(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_ahb_info *dev,
	int index,
	int maxno);

extern int ambapp_find_ahbslvs(
	struct ambapp_bus *abus,
	int vendor,
	int device,
	struct ambapp_ahb_info *dev,
	int maxno);


#ifdef __cplusplus
}
#endif

#endif
