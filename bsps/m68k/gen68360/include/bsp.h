/**
 *  @file
 *
 *  @ingroup RTEMSBSPsM68kGen68360
 *
 *  @brief Board Support Package for `Generic' Motorola MC68360
 */

/*
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

/*  bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_GEN68360_BSP_H
#define LIBBSP_M68K_GEN68360_BSP_H

/**
 * @defgroup RTEMSBSPsM68kGen68360 Motorola 68360
 *
 * @ingroup RTEMSBSPsM68k
 *
 * @brief Motorola 68360 Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rtems_bsdnet_ifconfig;
extern int rtems_scc1_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"scc1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_scc1_driver_attach

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

void M360ExecuteRISC( uint16_t         command );
void *M360AllocateBufferDescriptors( int count );
void *M360AllocateRiscTimers( int count );
extern char M360DefaultWatchdogFeeder;

extern int m360_clock_rate; /* BRG clock rate, defined in console.c */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Definitions for Atlas Computer Equipment Inc. High Speed Bridge (HSB)
 */
#define ATLASHSB_ESR    0x20010000L
#define ATLASHSB_USICR  0x20010001L
#define ATLASHSB_DSRR   0x20010002L
#define ATLASHSB_LED4   0x20010004L
#define ATLASHSB_ROM_U6 0xFF080000L	/* U6 flash ROM socket */


/*
 * definitions for PGH360 board
 */
#if defined(PGH360)
/*
 * logical SPI addresses of SPI slaves available
 */
#define PGH360_SPI_ADDR_EEPROM     0
#define PGH360_SPI_ADDR_DISP4_DATA 1
#define PGH360_SPI_ADDR_DISP4_CTRL 2

/*
 * Port B bit locations of SPI slave selects
 */
#define PGH360_PB_SPI_DISP4_RS_MSK   (1<<15)
#define PGH360_PB_SPI_DISP4_CE_MSK   (1<<14)
#define PGH360_PB_SPI_EEP_CE_MSK     (1<< 0)
#endif /* defined(PGH360) */

/*
 * Prototypes for BSP methods which cross file boundaries
 */
void _Init68360(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
