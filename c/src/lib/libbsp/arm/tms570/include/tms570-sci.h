/**
 * @file tms570-sci.h
 *
 * @ingroup tms570
 *
 * @brief Serial Communication Interface (SCI) header file.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_SCI_H
#define LIBBSP_ARM_TMS570_SCI_H

#include <libchip/serial.h>

#include <rtems.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t SCIGCR0;         /*SCIGlobalControlRegister0*/
  uint32_t SCIGCR1;         /*SCIGlobalControlRegister1*/
  uint32_t reserved1 [0x4/4];
  uint32_t SCISETINT;       /*SCISetInterruptRegister*/
  uint32_t SCICLEARINT;     /*SCIClearInterruptRegister*/
  uint32_t SCISETINTLVL;    /*SCISetInterruptLevelRegister*/
  uint32_t SCICLEARINTLVL;  /*SCIClearInterruptLevelRegister*/
  uint32_t SCIFLR;          /*SCIFlagsRegister*/
  uint32_t SCIINTVECT0;     /*SCIInterruptVectorOffset0*/
  uint32_t SCIINTVECT1;     /*SCIInterruptVectorOffset1*/
  uint32_t SCIFORMAT;       /*SCIFormatControlRegister*/
  uint32_t BRS;             /*BaudRateSelectionRegister*/
  uint32_t SCIED;           /*ReceiverEmulationDataBuffer*/
  uint32_t SCIRD;           /*ReceiverDataBuffer*/
  uint32_t SCITD;           /*TransmitDataBuffer*/
  uint32_t SCIPIO0;         /*SCIPinI/OControlRegister0*/
  uint32_t SCIPIO1;         /*SCIPinI/OControlRegister1*/
  uint32_t SCIPIO2;         /*SCIPinI/OControlRegister2*/
  uint32_t SCIPIO3;         /*SCIPinI/OControlRegister3*/
  uint32_t SCIPIO4;         /*SCIPinI/OControlRegister4*/
  uint32_t SCIPIO5;         /*SCIPinI/OControlRegister5*/
  uint32_t SCIPIO6;         /*SCIPinI/OControlRegister6*/
  uint32_t SCIPIO7;         /*SCIPinI/OControlRegister7*/
  uint32_t SCIPIO8;         /*SCIPinI/OControlRegister8*/
  uint32_t reserved2 [0x30/4];
  uint32_t IODFTCTRL;       /*Input/OutputErrorEnableRegister*/
}tms570_sci_t;

#define TMS570_SCI (*(volatile tms570_sci_t*)0xFFF7E400U)
#define TMS570_SCI2 (*(volatile tms570_sci_t*)0xFFF7E500U)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
