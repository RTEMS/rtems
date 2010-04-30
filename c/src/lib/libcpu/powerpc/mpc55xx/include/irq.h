/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief IRQ
 */

/*
 * Copyright (c) 2008, 2010
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#include <rtems/irq-extension.h>
#include <rtems/irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Interrupt numbers
 */

/* Basics */
#define MPC55XX_IRQ_MIN 0U
#define MPC55XX_IRQ_MAX 328U
#define MPC55XX_IRQ_MIN 0U
#define MPC55XX_IRQ_MAX 328U
#define MPC55XX_IRQ_BASE MPC55XX_IRQ_MIN
#define MPC55XX_IRQ_NUMBER (MPC55XX_IRQ_MAX + 1U)

/* Software interrupts */
#define MPC55XX_IRQ_SOFTWARE_MIN 0U
#define MPC55XX_IRQ_SOFTWARE_MAX 7U
#define MPC55XX_IRQ_SOFTWARE_GET_INDEX( v) (v)
#define MPC55XX_IRQ_SOFTWARE_GET_REQUEST( i) (i)
#define MPC55XX_IRQ_SOFTWARE_NUMBER (MPC55XX_IRQ_SOFTWARE_MAX + 1U)

#if ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))
#else  /*((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))*/
#endif /*((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))*/

#if ((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))

/* eDMA interrupts */
#define MPC55XX_IRQ_EDMA_ERROR_LOW 10U
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN 11U
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MAX 26U

#define MPC55XX_IRQ_EDMA_GET_CHANNEL( v) \
  ((v) - MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN)
#define MPC55XX_IRQ_EDMA_GET_REQUEST( c) \
  ((c) + MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN)

/* SIU external interrupts */
#define MPC55XX_IRQ_SIU_EXTERNAL_0 53U
#define MPC55XX_IRQ_SIU_EXTERNAL_1 54U
#define MPC55XX_IRQ_SIU_EXTERNAL_2 55U
#define MPC55XX_IRQ_SIU_EXTERNAL_3 56U
#define MPC55XX_IRQ_SIU_EXTERNAL_4_15 57U

/* eMIOS interrupts */
#define MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN 58U
#define MPC55XX_IRQ_EMIOS_REQUEST_LOW_MAX 81U
#define MPC55XX_IRQ_EMIOS_GET_CHANNEL( v) \
  ((v) - MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN)
#define MPC55XX_IRQ_EMIOS_GET_REQUEST( c) \
  ((c) + MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN)

#else  /*((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))*/

/* eDMA interrupts */
#define MPC55XX_IRQ_EDMA_ERROR_LOW 10U
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN 11U
#define MPC55XX_IRQ_EDMA_REQUEST_LOW_MAX 42U

#define MPC55XX_IRQ_EDMA_ERROR_HIGH 210U
#define MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN 211U
#define MPC55XX_IRQ_EDMA_REQUEST_HIGH_MAX 242U

#define MPC55XX_IRQ_EDMA_GET_CHANNEL( v) \
  (((v) > MPC55XX_IRQ_EDMA_REQUEST_LOW_MAX)		\
   ? ((v) + 32U - MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN)	\
   : ((v) - MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN))
#define MPC55XX_IRQ_EDMA_GET_REQUEST( c) \
  (((c) >= 32U)					     \
   ? ((c) - 32U + MPC55XX_IRQ_EDMA_REQUEST_HIGH_MIN) \
   : ((c) + MPC55XX_IRQ_EDMA_REQUEST_LOW_MIN))

/* SIU external interrupts */
#define MPC55XX_IRQ_SIU_EXTERNAL_0 46U
#define MPC55XX_IRQ_SIU_EXTERNAL_1 47U
#define MPC55XX_IRQ_SIU_EXTERNAL_2 48U
#define MPC55XX_IRQ_SIU_EXTERNAL_3 49U
#define MPC55XX_IRQ_SIU_EXTERNAL_4_15 50U

/* eMIOS interrupts */
#define MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN 51U
#define MPC55XX_IRQ_EMIOS_REQUEST_LOW_MAX 66U
#define MPC55XX_IRQ_EMIOS_REQUEST_HIGH_MIN 202U
#define MPC55XX_IRQ_EMIOS_REQUEST_HIGH_MAX 209U

#define MPC55XX_IRQ_EMIOS_GET_CHANNEL( v)	      \
  (((v) > MPC55XX_IRQ_EMIOS_REQUEST_LOW_MAX)	      \
   ? ((v) + 16U - MPC55XX_IRQ_EMIOS_REQUEST_HIGH_MIN) \
   : ((v) - MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN))

#define MPC55XX_IRQ_EMIOS_GET_REQUEST( c)	      \
  (((c) >= 16U)					      \
   ? ((c) - 16U + MPC55XX_IRQ_EMIOS_REQUEST_HIGH_MIN) \
   : ((c) + MPC55XX_IRQ_EMIOS_REQUEST_LOW_MIN))

#endif /*((MPC55XX_CHIP_TYPE >= 5510) && (MPC55XX_CHIP_TYPE <= 5517))*/

/* Checks */
#define MPC55XX_IRQ_IS_VALID(v) \
  ((v) >= MPC55XX_IRQ_MIN &&	\
   (v) <= MPC55XX_IRQ_MAX)
#define MPC55XX_IRQ_IS_SOFTWARE(v) \
  ((v) >= MPC55XX_IRQ_SOFTWARE_MIN && \
   (v) <= MPC55XX_IRQ_SOFTWARE_MAX)

/*
 * Interrupt controller
 */

#define MPC55XX_INTC_MIN_PRIORITY 1U
#define MPC55XX_INTC_MAX_PRIORITY 15U
#define MPC55XX_INTC_DISABLED_PRIORITY 0U
#define MPC55XX_INTC_INVALID_PRIORITY (MPC55XX_INTC_MAX_PRIORITY + 1)
#define MPC55XX_INTC_DEFAULT_PRIORITY (MPC55XX_INTC_MIN_PRIORITY + 1)
#define MPC55XX_INTC_IS_VALID_PRIORITY(p) ((p) >= MPC55XX_INTC_DISABLED_PRIORITY && (p) <= MPC55XX_INTC_MAX_PRIORITY)

rtems_status_code mpc55xx_interrupt_handler_install(
	rtems_vector_number vector,
	const char *info,
	rtems_option options,
	unsigned priority,
	rtems_interrupt_handler handler,
	void *arg
);

rtems_status_code mpc55xx_intc_get_priority( rtems_vector_number vector, unsigned *priority);

rtems_status_code mpc55xx_intc_set_priority( rtems_vector_number vector, unsigned priority);

rtems_status_code mpc55xx_intc_raise_software_irq( rtems_vector_number vector);

rtems_status_code mpc55xx_intc_clear_software_irq( rtems_vector_number vector);

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX 328

#define BSP_INTERRUPT_USE_INDEX_TABLE

#define BSP_INTERRUPT_NO_HEAP_USAGE

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  #define BSP_INTERRUPT_HANDLER_TABLE_SIZE 63
  typedef uint8_t bsp_interrupt_handler_index_type;
#endif

/** @} */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_IRQ_H */
