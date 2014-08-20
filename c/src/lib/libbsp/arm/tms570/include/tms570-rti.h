/**
 * @file tms570-rti.h
 *
 * @ingroup tms570
 *
 * @brief Real Time Interrupt module (RTI) header file.
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

#ifndef LIBBSP_ARM_TMS570_RTI_H
#define LIBBSP_ARM_TMS570_RTI_H

#ifndef ASM

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t RTIGCTRL;       /* RTIGlobalControlRegister */
  uint32_t RTITBCTRL;      /* RTITimebaseControlRegister */
  uint32_t RTICAPCTRL;     /* RTICaptureControlRegister */
  uint32_t RTICOMPCTRL;    /* RTICompareControlRegister */
  uint32_t RTIFRC0;        /* RTIFreeRunningCounter0Register */
  uint32_t RTIUC0;         /* RTIUpCounter0Register */
  uint32_t RTICPUC0;       /* RTICompareUpCounter0Register */
  uint32_t reserved1 [0x4/4];
  uint32_t RTICAFRC0;      /* RTICaptureFreeRunningCounter0Register */
  uint32_t RTICAUC0;       /* RTICaptureUpCounter0Register */
  uint32_t reserved2 [0x8/4];
  uint32_t RTIFRC1;        /* RTIFreeRunningCounter1Register */
  uint32_t RTIUC1;         /* RTIUpCounter1Register */
  uint32_t RTICPUC1;       /* RTICompareUpCounter1Register */
  uint32_t reserved3 [0x4/4];
  uint32_t RTICAFRC1;      /* RTICaptureFreeRunningCounter1Register */
  uint32_t RTICAUC1;       /* RTICaptureUpCounter1Register */
  uint32_t reserved4 [0x8/4];
  uint32_t RTICOMP0;       /* RTICompare0Register */
  uint32_t RTIUDCP0;       /* RTIUpdateCompare0Register */
  uint32_t RTICOMP1;       /* RTICompare1Register */
  uint32_t RTIUDCP1;       /* RTIUpdateCompare1Register */
  uint32_t RTICOMP2;       /* RTICompare2Register */
  uint32_t RTIUDCP2;       /* RTIUpdateCompare2Register */
  uint32_t RTICOMP3;       /* RTICompare3Register */
  uint32_t RTIUDCP3;       /* RTIUpdateCompare3Register */
  uint32_t RTITBLCOMP;     /* RTITimebaseLowCompareRegister */
  uint32_t RTITBHCOMP;     /* RTITimebaseHighCompareRegister */
  uint32_t reserved5 [0x8/4];
  uint32_t RTISETINTENA;   /* RTISetInterruptEnableRegister */
  uint32_t RTICLEARINTENA; /* RTIClearInterruptEnableRegister */
  uint32_t RTIINTFLAG;     /* RTIInterruptFlagRegister */
  uint32_t reserved6 [0x4/4];
  uint32_t RTIDWDCTRL;     /* DigitalWatchdogControlRegister */
  uint32_t RTIDWDPRLD;     /* DigitalWatchdogPreloadRegister */
  uint32_t RTIWDSTATUS;    /* WatchdogStatusRegister */
  uint32_t RTIWDKEY;       /* RTIWatchdogKeyRegister */
  uint32_t RTIDWDCNTR;     /* RTIDigitalWatchdogDownCounterRegister */
  uint32_t RTIWWDRXNCTRL;  /* DigitalWindowedWatchdogReactionControlRegister */
  uint32_t RTIWWDSIZECTRL; /* DigitalWindowedWatchdogWindowSizeControlRegister */
  uint32_t RTIINTCLRENABLE;/* RTICompareInterruptClearEnableRegister */
  uint32_t RTICOMP0CLR;    /* RTICompare0ClearRegister */
  uint32_t RTICOMP1CLR;    /* RTICompare1ClearRegister */
  uint32_t RTICOMP2CLR;    /* RTICompare2ClearRegister */
  uint32_t RTICOMP3CLR;    /* RTICompare3ClearRegister */
}tms570_rti_t;

#define TMS570_RTI (*(volatile tms570_rti_t*)0xFFFFFC00)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */
