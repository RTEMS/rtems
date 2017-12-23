/*
 *  GRPCI2 DMA Driver
 *
 *  COPYRIGHT (c) 2017
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  OVERVIEW
 *  ========
 *  This driver controls the DMA on the GRPCI2 device, located 
 *  at an on-chip AMBA.
 */

#ifndef __GRPCI2DMA_H__
#define __GRPCI2DMA_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error return codes */
#define GRPCI2DMA_ERR_OK 0
#define GRPCI2DMA_ERR_WRONGPTR -1
#define GRPCI2DMA_ERR_NOINIT -2
#define GRPCI2DMA_ERR_TOOMANY -3
#define GRPCI2DMA_ERR_ERROR -4
#define GRPCI2DMA_ERR_STOPDMA -5
#define GRPCI2DMA_ERR_NOTFOUND -6

/* Size of a dma descriptors */
#define GRPCI2DMA_BD_CHAN_SIZE 0x10
#define GRPCI2DMA_BD_DATA_SIZE 0x10

/* Alignment of dma descriptors */
#define GRPCI2DMA_BD_CHAN_ALIGN 0x10
#define GRPCI2DMA_BD_DATA_ALIGN 0x10

/* User-helper functions to allocate/deallocate
 * channel and data descriptors
 */
extern void * grpci2dma_channel_new(int number);
extern void grpci2dma_channel_delete(void * chanbd);
extern void * grpci2dma_data_new(int number);
extern void grpci2dma_data_delete(void * databd);

/* Function:
 *  -grpci2dma_prepare
 * Description:
 *  -Prepare a transfer, initializing the required data descriptors
 * Parameters:
 *  -pci_start: Where in PCI/remote starts the transfer
 *  -ahb_start: Where in AHB/local starts the transfer
 *  -dir: Direction of the transfer (AHBTOPCI or PCITOAHB)
 *  -endianness: Endianness of the transfer (LITTLEENDIAN or BIGENDIAN)
 *  -size: Size in bytes of the transfer (the function will calculate if there
 *  are enough descriptors)
 *  -databd: Pointer to the data descriptor buffer
 *  -bdindex: Where in the buffer to start the transfer
 *  -bdmax: Maximum index for the data descriptor buffer
 *  -block_size: Size in bytes for each PCI transaction (or block). Guaranteed 
 *  to be at least smaller that this value. Put 0 to use default.
 *  Default is maximum, which is 0x10000*4 bytes.
 * Returns:
 *  -WRONGPTR: Wrong input parameters
 *  -TOOMANY: Not enough data descriptors in the buffer
 *  -value > 0: A positive return value means the number of data descriptors 
 *  prepared/used in the buffer, starting from index.
 */
#define GRPCI2DMA_AHBTOPCI 1
#define GRPCI2DMA_PCITOAHB 0
#define GRPCI2DMA_LITTLEENDIAN 1
#define GRPCI2DMA_BIGENDIAN 0
extern int grpci2dma_prepare(
        uint32_t pci_start, uint32_t ahb_start, int dir, int endianness,
        int size, void * databd, int bdindex, int bdmax, int block_size);

/* Function:
 *  -grpci2dma_status
 * Description:
 *  -Status of an transfer:
 * Parameters:
 *  -databd: Pointer to the data descriptor buffer
 *  -bdindex: Where in the buffer starts the transfer
 *  -bdsize: Number of descriptors used by the transfer
 * Returns:
 *  -WRONGPTR: Wrong input parameters
 *  -GRPCI2DMA_BD_DATA_STATUS_ERR: If at least one of the descriptors has an
 *  error
 *  -GRPCI2DMA_BD_DATA_STATUS_ENABLED: If at least one of the descriptors is
 *  enabled, which means that the transfer is still not finished.
 *  -GRPCI2DMA_BD_DATA_STATUS_DISABLED: If all the descriptors are disabled,
 *  which means that either the transfer finished or it was never prepared.
 */
#define GRPCI2DMA_BD_STATUS_DISABLED 0
#define GRPCI2DMA_BD_STATUS_ENABLED 1
#define GRPCI2DMA_BD_STATUS_ERR 2
extern int grpci2dma_status(void *databd, int bdindex, int bdsize);

/* Function Interrupt-Code ISR callback prototype.
 * arg     - Custom arg provided by user
 * cid     - Channel ID that got the interrupt
 * status  - Error status of the DMA core
 */
typedef void (*grpci2dma_isr_t)(void *arg, int cid, unsigned int status);

/* Function:
 *  -grpci2dma_isr_register
 * Description:
 *  -Register an ISR for a channel (and enable interrupts if disabled)
 * Parameters:
 *  -chan_no: ID of the channel
 *  -dmaisr: ISR
 *  -arg: Argument to pass to the ISR when called
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -OK (=0): Done
 */
extern int grpci2dma_isr_register(
        int chan_no, grpci2dma_isr_t dmaisr, void *arg);

/* Function:
 *  -grpci2dma_isr_unregister
 * Description:
 *  -Unregister an ISR for a channel (and enable interrupts if disabled)
 * Parameters:
 *  -chan_no: ID of the channel
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -OK (=0): Done
 */
extern int grpci2dma_isr_unregister(int chan_no);

/* Function:
 *  -grpci2dma_open
 * Description:
 *  -Open a channel (and allocate the descriptor if the user does not provide
 *  one).
 * Parameters:
 *  -chan: Descriptor for the channel (must be aligned to 0x10)
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -TOOMANY: Maximum number of channels already opened.
 *  -WRONGPTR: Wrong input parameters
 *  -ERROR: Inconsistent state found in driver
 *  -value > 0: A positive return value means the id for the channel.
 */
extern int grpci2dma_open(void * chan);

/* Function:
 *  -grpci2dma_close
 * Description:
 *  -Stop and close a channel (and deallocate it if the user did not provide a
 *  pointer when opening it)
 * Parameters:
 *  -chan_no: Id of the channel
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -NOTFOUND: Channel not opened.
 *  -STOPDMA: Cannot stop channel.
 *  -WRONGPTR: Wrong input parameters
 *  -OK (=0): Done.
 */
extern int grpci2dma_close(int chan_no);

/* Function:
 *  -grpci2dma_start
 * Description:
 *  -Start a channel
 * Parameters:
 *  -chan_no: Id of the channel
 *  -options: Maximum number of data descriptors to be executed before moving
 *  to next channel (up to 0x10000)
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -ERROR: Inconsistent state found in driver
 *  -OK (=0): Done.
 */
extern int grpci2dma_start(int chan_no, int options);

/* Function:
 *  -grpci2dma_stop
 * Description:
 *  -Start a channel
 * Parameters:
 *  -chan_no: Id of the channel
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -ERROR: Inconsistent state found in driver
 *  -OK (=0): Done.
 */
extern int grpci2dma_stop(int chan_no);

/* Function:
 *  -grpci2dma_push
 * Description:
 *  -Push a transfer into a channel (already started or not)
 * Parameters:
 *  -chan_no: Id of the channel
 *  -databd: Pointer to the data descriptor buffer
 *  -bdindex: Where in the buffer starts the transfer
 *  -bdsize: Number of descriptors used by the transfer
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -NOTFOUND: Channel not opened.
 *  -OK (=0): Done.
 */
extern int grpci2dma_push(int chan_no, void *databd, int bdindex, int bdsize);

/* Function:
 *  -grpci2dma_active
 * Description:
 *  -Check if dma is active
 * Parameters:
 * Returns:
 *  -(!=0): Active.
 *  -(=0): Not active.
 */
extern int grpci2dma_active(void);

/* Function:
 *  -grpci2dma_interrupt_enable
 * Description:
 *  -Enable interrupt for a transfer
 * Parameters:
 *  -databd: Pointer to the data descriptor buffer
 *  -bdindex: Where in the buffer starts the transfer
 *  -bdmax: Upper limit for index. index < bdmax 
 *  -options: 
 *      (=GRPCI2DMA_OPTIONS_ALL)=Enable interrupt on all transfer descriptors.
 *      (=GRPCI2DMA_OPTIONS_ONE)=Enable interrupt on transfer descriptor
 *              indicated by bdindex.
 * Returns:
 *  -NOINIT: GRPCI2 DMA not initialized
 *  -WRONGPTR: Wrong input parameters
 *  -ERROR: Inconsistent state found in driver
 *  -OK (=0): Done.
 */
#define GRPCI2DMA_OPTIONS_ALL 1
#define GRPCI2DMA_OPTIONS_ONE 0
extern int grpci2dma_interrupt_enable(
        void *databd, int bdindex, int bdmax, int options);

/* Debug function: print dma channel and associated data descriptors.
 * Only prints if driver internal DEBUG flag is defined. */
extern int grpci2dma_print(int chan_no);
extern int grpci2dma_print_bd(void * data);

#ifdef __cplusplus
}
#endif

#endif /* __GRPCI2DMA_H__ */
