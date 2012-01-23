/**
 * @file
 *
 * @ingroup mpc55xx
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*********************************************************************
 *
 * Copyright:
 *  Freescale Semiconductor, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Freescale
 *  Semiconductor, Inc. This software is provided on an "AS IS"
 *  basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, Freescale
 *  Semiconductor DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,
 *  INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *  PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH
 *  REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)
 *  AND ANY ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL Freescale Semiconductor BE LIABLE FOR ANY DAMAGES WHATSOEVER
 *  (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
 *  BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER
 *  PECUNIARY LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Freescale Semiconductor assumes no responsibility for the
 *  maintenance and support of this software
 *
 ********************************************************************/

#ifndef LIBCPU_POWERPC_MPC55XX_REGS_MMU_H
#define LIBCPU_POWERPC_MPC55XX_REGS_MMU_H

#include <stdint.h>

#include <bspopts.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*                          MMU                                             */
/****************************************************************************/
    struct MMU_tag {
        union {
            uint32_t R;
            struct {
                uint32_t : 2;
                uint32_t TLBSEL : 2;
                uint32_t : 7;
                uint32_t ESEL : 5;
                uint32_t : 11;
                uint32_t NV : 5;
            } B;
        } MAS0;

        union {
            uint32_t R;
            struct {
                uint32_t VALID : 1;
                uint32_t IPROT : 1;
                uint32_t : 6;
                uint32_t TID : 8;
                uint32_t : 3;
                uint32_t TS : 1;
                uint32_t TSIZE : 5;
                uint32_t : 7;
            } B;
        } MAS1;

        union {
            uint32_t R;
            struct {
                uint32_t EPN : 22;
                uint32_t : 4;
                uint32_t VLE : 1;
                uint32_t W : 1;
                uint32_t I : 1;
                uint32_t M : 1;
                uint32_t G : 1;
                uint32_t E : 1;
            } B;
        } MAS2;

        union {
            uint32_t R;
            struct {
                uint32_t RPN : 22;
                uint32_t U0 : 1;
                uint32_t U1 : 1;
                uint32_t U2 : 1;
                uint32_t U3 : 1;
                uint32_t UX : 1;
                uint32_t SX : 1;
                uint32_t UW : 1;
                uint32_t SW : 1;
                uint32_t UR : 1;
                uint32_t SR : 1;
            } B;
        } MAS3;
    };

        union MMU_MAS4_tag {
            uint32_t R;
            struct {
                uint32_t : 2;
                uint32_t TLBSELD : 2;
                uint32_t : 10;
                uint32_t TIDSELD : 2;
                uint32_t : 4;
                uint32_t TSIZED : 4;
                uint32_t : 3;
                uint32_t WD : 1;
                uint32_t ID : 1;
                uint32_t MD : 1;
                uint32_t GD : 1;
                uint32_t ED : 1;
            } B;
        };

        union MMU_MAS6_tag {
            uint32_t R;
            struct {
                uint32_t : 8;
                uint32_t SPID : 8;
                uint32_t : 15;
                uint32_t SAS : 1;
            } B;
        };

#define MPC55XX_MMU_TAG_INITIALIZER(idx, addr, size, x, w, r, io) \
  { \
    .MAS0 = { .B = { .TLBSEL = 1, .ESEL = (idx) } }, \
    .MAS1 = { .B = { \
      .VALID = 1, .IPROT = 1, .TID = 0, .TS = 0, .TSIZE = (size) } \
    }, \
    .MAS2 = { .B = { \
      .EPN = (addr) >> 10, .VLE = 0, .W = 0, .I = (io), .M = 0, .G = (io), .E = 0 } \
    }, \
    .MAS3 = { .B = { \
      .RPN = (addr) >> 10, .U0 = 0, .U1 = 0, .U2 = 0, .U3 = 0, .UX = 0, \
      .SX = (x), .UW = 0, .SW = (w), .UR = 0, .SR = (r) } \
    } \
  }

#define MPC55XX_MMU_1K 0
#define MPC55XX_MMU_2K 1
#define MPC55XX_MMU_4K 2
#define MPC55XX_MMU_8K 3
#define MPC55XX_MMU_16K 4
#define MPC55XX_MMU_32K 5
#define MPC55XX_MMU_64K 6
#define MPC55XX_MMU_128K 7
#define MPC55XX_MMU_256K 8
#define MPC55XX_MMU_512K 9
#define MPC55XX_MMU_1M 10
#define MPC55XX_MMU_2M 11
#define MPC55XX_MMU_4M 12
#define MPC55XX_MMU_8M 13
#define MPC55XX_MMU_16M 14
#define MPC55XX_MMU_32M 15
#define MPC55XX_MMU_64M 16
#define MPC55XX_MMU_128M 17
#define MPC55XX_MMU_256M 18
#define MPC55XX_MMU_512M 19
#define MPC55XX_MMU_1G 20
#define MPC55XX_MMU_2G 21
#define MPC55XX_MMU_4G 22

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_REGS_MMU_H */
