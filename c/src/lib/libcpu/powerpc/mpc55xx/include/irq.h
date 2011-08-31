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

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Interrupt numbers
 */

#define MPC55XX_IRQ_INVALID 0x10000U
#define MPC55XX_IRQ_MIN 0U

/* Software interrupts */
#define MPC55XX_IRQ_SOFTWARE_MIN 0U
#define MPC55XX_IRQ_SOFTWARE_MAX 7U
#define MPC55XX_IRQ_SOFTWARE_GET_INDEX(v) (v)
#define MPC55XX_IRQ_SOFTWARE_GET_REQUEST(i) (i)
#define MPC55XX_IRQ_SOFTWARE_NUMBER (MPC55XX_IRQ_SOFTWARE_MAX + 1U)

#if MPC55XX_CHIP_TYPE / 10 == 551
  #define MPC55XX_IRQ_MAX 293U

  /* eDMA */
  #define MPC55XX_IRQ_EDMA_ERROR(group) \
    ((group) == 0 ? 10U : MPC55XX_IRQ_INVALID)
  #define MPC55XX_IRQ_EDMA(ch) \
    ((unsigned) (ch) < 16U ? 11U + (ch) : MPC55XX_IRQ_INVALID)

  /* I2C */
  #define MPC55XX_IRQ_I2C(mod) \
    ((mod) == 0 ? 48U : MPC55XX_IRQ_INVALID)

  /* SIU external interrupts */
  #define MPC55XX_IRQ_SIU_EXTERNAL_0 53U
  #define MPC55XX_IRQ_SIU_EXTERNAL_1 54U
  #define MPC55XX_IRQ_SIU_EXTERNAL_2 55U
  #define MPC55XX_IRQ_SIU_EXTERNAL_3 56U
  #define MPC55XX_IRQ_SIU_EXTERNAL_4_15 57U

  /* PIT */
  #define MPC55XX_IRQ_RTI 148U
  #define MPC55XX_IRQ_PIT(timer) (148U + (timer))

  /* eTPU */
  #define MPC55XX_IRQ_ETPU_BASE(mod) MPC55XX_IRQ_INVALID

  /* DSPI */
  #define MPC55XX_IRQ_DSPI_BASE(mod) \
    ((mod) == 0 ? 117U : \
      ((mod) == 1 ? 122U : \
        ((mod) == 2 ? 274U : \
          ((mod) == 3 ? 279U : MPC55XX_IRQ_INVALID))))

  /* eMIOS */
  #define MPC55XX_IRQ_EMIOS(ch) \
    ((unsigned) (ch) < 24U ? 58U + (ch) : MPC55XX_IRQ_INVALID)

  /* eQADC */
  #define MPC55XX_IRQ_EQADC_BASE(mod) \
    ((mod) == 0 ? 82U : MPC55XX_IRQ_INVALID)

  /* eSCI */
  #define MPC55XX_IRQ_ESCI_BASE(mod) \
    ((mod) == 0 ? 113U : \
      ((mod) == 1 ? 114U : \
        ((mod) == 2 ? 115U : \
          ((mod) == 3 ? 116U : \
            ((mod) == 4 ? 270U : \
              ((mod) == 5 ? 271U : \
                ((mod) == 6 ? 272U : \
                  ((mod) == 7 ? 273U : MPC55XX_IRQ_INVALID))))))))

  /* FlexCAN */
  #define MPC55XX_IRQ_CAN_BASE(mod) \
    ((mod) == 0 ? 127U : \
      ((mod) == 1 ? 157U : \
        ((mod) == 2 ? 178U : \
          ((mod) == 3 ? 199U : \
            ((mod) == 4 ? 220U : \
              ((mod) == 5 ? 241U : MPC55XX_IRQ_INVALID))))))

  /* FlexRay */
  #define MPC55XX_IRQ_FLEXRAY_BASE(mod) \
    ((mod) == 0 ? 284U : MPC55XX_IRQ_INVALID)
#else
  #if MPC55XX_CHIP_TYPE / 10 == 555
    #define MPC55XX_IRQ_MAX 307U
  #elif MPC55XX_CHIP_TYPE / 10 == 556
    #define MPC55XX_IRQ_MAX 360U
  #elif MPC55XX_CHIP_TYPE / 10 == 567
    #define MPC55XX_IRQ_MAX 479U
  #else
    #error "unsupported chip type"
  #endif

  /* eDMA */
  #define MPC55XX_IRQ_EDMA_ERROR(group) \
    ((group) == 0 ? 10U : \
      ((group) == 1 ? 210U : \
        ((group) == 2 ? 425U : MPC55XX_IRQ_INVALID)))
  #define MPC55XX_IRQ_EDMA(ch) \
    ((unsigned) (ch) < 32U ? 11U + (ch) : \
      ((unsigned) (ch) < 64U ? 179U + (ch) : \
        ((unsigned) (ch) < 96U ? 362U + (ch) : MPC55XX_IRQ_INVALID)))

  /* I2C */
  #define MPC55XX_IRQ_I2C(mod) MPC55XX_IRQ_INVALID

  /* SIU external interrupts */
  #define MPC55XX_IRQ_SIU_EXTERNAL_0 46U
  #define MPC55XX_IRQ_SIU_EXTERNAL_1 47U
  #define MPC55XX_IRQ_SIU_EXTERNAL_2 48U
  #define MPC55XX_IRQ_SIU_EXTERNAL_3 49U
  #define MPC55XX_IRQ_SIU_EXTERNAL_4_15 50U

  /* PIT */
  #define MPC55XX_IRQ_RTI 305U
  #define MPC55XX_IRQ_PIT(ch) (301U + (ch))

  /* eTPU */
  #define MPC55XX_IRQ_ETPU_BASE(mod) \
    ((mod) == 0 ? 67U : \
      ((mod) == 1 ? 243U : MPC55XX_IRQ_INVALID))

  /* DSPI */
  #define MPC55XX_IRQ_DSPI_BASE(mod) \
    ((mod) == 0 ? 275U : \
      ((mod) == 1 ? 131U : \
        ((mod) == 2 ? 136U : \
          ((mod) == 3 ? 141U : MPC55XX_IRQ_INVALID))))

  /* eMIOS */
  #define MPC55XX_IRQ_EMIOS(ch) \
    ((unsigned) (ch) < 16U ? 51U + (ch) : \
      ((unsigned) (ch) < 24U ? 186U + (ch) : \
        ((unsigned) (ch) < 32U ? 435U + (ch) : MPC55XX_IRQ_INVALID)))

  /* eQADC */
  #define MPC55XX_IRQ_EQADC_BASE(mod) \
    ((mod) == 0 ? 100U : \
      ((mod) == 1 ? 394U : MPC55XX_IRQ_INVALID))

  /* eSCI */
  #define MPC55XX_IRQ_ESCI_BASE(mod) \
    ((mod) == 0 ? 146U : \
      ((mod) == 1 ? 149U : \
        ((mod) == 2 ? 473U : MPC55XX_IRQ_INVALID)))

  /* FlexCAN */
  #define MPC55XX_IRQ_CAN_BASE(mod) \
    ((mod) == 0 ? 152U : \
      ((mod) == 1 ? 280U : \
        ((mod) == 2 ? 173U : \
          ((mod) == 3 ? 308U : \
            ((mod) == 4 ? 329U : MPC55XX_IRQ_INVALID)))))

  /* FlexRay */
  #define MPC55XX_IRQ_FLEXRAY_BASE(mod) \
    ((mod) == 0 ? 350U : MPC55XX_IRQ_INVALID)
#endif

#define MPC55XX_IRQ_NUMBER (MPC55XX_IRQ_MAX + 1U)

/* eTPU */
#define MPC55XX_IRQ_ETPU(mod) \
  (MPC55XX_IRQ_ETPU_BASE(mod) + 0U)
#define MPC55XX_IRQ_ETPU_CHANNEL(mod, ch) \
  (MPC55XX_IRQ_ETPU_BASE(mod) + 1U + (ch))

/* DSPI */
#define MPC55XX_IRQ_DSPI_TFUF_RFOF(mod) (MPC55XX_IRQ_DSPI_BASE(mod) + 0U)
#define MPC55XX_IRQ_DSPI_EOQF(mod) (MPC55XX_IRQ_DSPI_BASE(mod) + 1U)
#define MPC55XX_IRQ_DSPI_TFFF(mod) (MPC55XX_IRQ_DSPI_BASE(mod) + 2U)
#define MPC55XX_IRQ_DSPI_TCF(mod) (MPC55XX_IRQ_DSPI_BASE(mod) + 3U)
#define MPC55XX_IRQ_DSPI_RFDF(mod) (MPC55XX_IRQ_DSPI_BASE(mod) + 4U)

/* eQADC */
#define MPC55XX_IRQ_EQADC_TORF_RFOF_CFUF(mod) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 0U)
#define MPC55XX_IRQ_EQADC_NCF(mod, fifo) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 1U + (fifo) * 5U + 0U)
#define MPC55XX_IRQ_EQADC_PF(mod, fifo) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 1U + (fifo) * 5U + 1U)
#define MPC55XX_IRQ_EQADC_EOQF(mod, fifo) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 1U + (fifo) * 5U + 2U)
#define MPC55XX_IRQ_EQADC_CFFF(mod, fifo) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 1U + (fifo) * 5U + 3U)
#define MPC55XX_IRQ_EQADC_RFDF(mod, fifo) \
  (MPC55XX_IRQ_EQADC_BASE(mod) + 1U + (fifo) * 5U + 4U)

/* eSCI */
#define MPC55XX_IRQ_ESCI(mod) (MPC55XX_IRQ_ESCI_BASE(mod) + 0U)

/* FlexCAN */
#define MPC55XX_IRQ_CAN_BOFF_TWRN_RWRN(mod) (MPC55XX_IRQ_CAN(mod) + 0U)
#define MPC55XX_IRQ_CAN_ERR(mod) (MPC55XX_IRQ_CAN(mod) + 1U)
#define MPC55XX_IRQ_CAN_BUF_0(mod) (MPC55XX_IRQ_CAN(mod) + 3U)
#define MPC55XX_IRQ_CAN_BUF_1(mod) (MPC55XX_IRQ_CAN(mod) + 4U)
#define MPC55XX_IRQ_CAN_BUF_2(mod) (MPC55XX_IRQ_CAN(mod) + 5U)
#define MPC55XX_IRQ_CAN_BUF_3(mod) (MPC55XX_IRQ_CAN(mod) + 6U)
#define MPC55XX_IRQ_CAN_BUF_4(mod) (MPC55XX_IRQ_CAN(mod) + 7U)
#define MPC55XX_IRQ_CAN_BUF_5(mod) (MPC55XX_IRQ_CAN(mod) + 8U)
#define MPC55XX_IRQ_CAN_BUF_6(mod) (MPC55XX_IRQ_CAN(mod) + 9U)
#define MPC55XX_IRQ_CAN_BUF_7(mod) (MPC55XX_IRQ_CAN(mod) + 10U)
#define MPC55XX_IRQ_CAN_BUF_8(mod) (MPC55XX_IRQ_CAN(mod) + 12U)
#define MPC55XX_IRQ_CAN_BUF_9(mod) (MPC55XX_IRQ_CAN(mod) + 12U)
#define MPC55XX_IRQ_CAN_BUF_10(mod) (MPC55XX_IRQ_CAN(mod) + 13U)
#define MPC55XX_IRQ_CAN_BUF_11(mod) (MPC55XX_IRQ_CAN(mod) + 14U)
#define MPC55XX_IRQ_CAN_BUF_12(mod) (MPC55XX_IRQ_CAN(mod) + 15U)
#define MPC55XX_IRQ_CAN_BUF_13(mod) (MPC55XX_IRQ_CAN(mod) + 16U)
#define MPC55XX_IRQ_CAN_BUF_14(mod) (MPC55XX_IRQ_CAN(mod) + 17U)
#define MPC55XX_IRQ_CAN_BUF_15(mod) (MPC55XX_IRQ_CAN(mod) + 18U)
#define MPC55XX_IRQ_CAN_BUF_16_31(mod) (MPC55XX_IRQ_CAN(mod) + 19U)
#define MPC55XX_IRQ_CAN_BUF_32_63(mod) (MPC55XX_IRQ_CAN(mod) + 20U)

/* FlexRay */
#define MPC55XX_IRQ_FLEXRAY_MIF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 0U)
#define MPC55XX_IRQ_FLEXRAY_PRIF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 1U)
#define MPC55XX_IRQ_FLEXRAY_CHIF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 2U)
#define MPC55XX_IRQ_FLEXRAY_WUP_IF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 3U)
#define MPC55XX_IRQ_FLEXRAY_FBNE_F(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 4U)
#define MPC55XX_IRQ_FLEXRAY_FANE_F(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 5U)
#define MPC55XX_IRQ_FLEXRAY_RBIF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 6U)
#define MPC55XX_IRQ_FLEXRAY_TBIF(mod) (MPC55XX_IRQ_FLEXRAY_BASE(mod) + 7U)

/* Checks */
#define MPC55XX_IRQ_IS_VALID(v) \
  ((v) >= MPC55XX_IRQ_MIN && \
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
#define MPC55XX_INTC_IS_VALID_PRIORITY(p) \
  ((p) >= MPC55XX_INTC_DISABLED_PRIORITY && (p) <= MPC55XX_INTC_MAX_PRIORITY)

rtems_status_code mpc55xx_interrupt_handler_install(
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  unsigned priority,
  rtems_interrupt_handler handler,
  void *arg
);

rtems_status_code mpc55xx_intc_get_priority(
  rtems_vector_number vector,
  unsigned *priority
);

rtems_status_code mpc55xx_intc_set_priority(
  rtems_vector_number vector,
  unsigned priority
);

rtems_status_code mpc55xx_intc_raise_software_irq(rtems_vector_number vector);

rtems_status_code mpc55xx_intc_clear_software_irq(rtems_vector_number vector);

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN MPC55XX_IRQ_MIN

#define BSP_INTERRUPT_VECTOR_MAX MPC55XX_IRQ_MAX

#define BSP_INTERRUPT_USE_INDEX_TABLE

#define BSP_INTERRUPT_NO_HEAP_USAGE

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE
  #define BSP_INTERRUPT_HANDLER_TABLE_SIZE 63
  typedef uint8_t bsp_interrupt_handler_index_type;
#endif

/** @} */

/* Legacy API */
#define MPC55XX_IRQ_EDMA_GET_REQUEST(ch) MPC55XX_IRQ_EDMA(ch)
#define MPC55XX_IRQ_EMIOS_GET_REQUEST(ch) MPC55XX_IRQ_EMIOS(ch)

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_IRQ_H */
