/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedFslEDMA
 */

/*
 * Copyright (C) 2011-2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

#ifndef LIBBSP_SHARED_FSL_REGS_EDMA_H
#define LIBBSP_SHARED_FSL_REGS_EDMA_H

#include <stdint.h>
#include <bsp.h>
#include <bspopts.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(MPC55XX_CHIP_FAMILY) && \
    (MPC55XX_CHIP_FAMILY == 566 || MPC55XX_CHIP_FAMILY == 567))
  #define EDMA_HAS_CR_EBW 1
  #define EDMA_HAS_CR_CX_ECX 1
  #define EDMA_HAS_CR_EMLM_CLM_HALT_HOE 1
  #define EDMA_HAS_ESR_ECX 1
  #define EDMA_HAS_HRS 1
  #define EDMA_HAS_CPR_DPA 1
#endif

#if defined(LIBBSP_ARM_IMXRT_BSP_H)
  #define EDMA_HAS_CR_CX_ECX 1
  #define EDMA_HAS_CR_EMLM_CLM_HALT_HOE 1
  #define EDMA_HAS_CR_ACTIVE 1
  #define EDMA_HAS_CR_VERSION 1
  #define EDMA_HAS_ESR_ECX 1
  #define EDMA_HAS_HRS 1
  #define EDMA_HAS_EARS 1
  #define EDMA_HAS_CPR_DPA 1
#endif

struct fsl_edma {
  uint32_t CR;
#ifdef EDMA_HAS_CR_EBW
  #define EDMA_CR_EBW               (1 << 0)
#endif
#define EDMA_CR_EDBG                (1 << 1)
#define EDMA_CR_ERCA                (1 << 2)
#define EDMA_CR_ERGA                (1 << 3)
#ifdef EDMA_HAS_CR_EMLM_CLM_HALT_HOE
  #define EDMA_CR_HOE               (1 << 4)
  #define EDMA_CR_HALT              (1 << 5)
  #define EDMA_CR_CLM               (1 << 6)
  #define EDMA_CR_EMLM              (1 << 7)
#endif
#define EDMA_CR_GRPxPRI_SHIFT(x)    (8 + (x) * 2)
#define EDMA_CR_GRPxPRI_MASK(x)     (0x3 << EDMA_CR_GRPxPRI_SHIFT(x))
#define EDMA_CR_GRPxPRI(x,val)      (((val) << EDMA_CR_GRPxPRI_SHIFT(x)) & EDMA_CR_GRPxPRI_MASK(x))
#define EDMA_CR_GRPxPRI_GET(x,reg)  (((reg) & EDMA_CR_GRPxPRI_MASK(x)) >> EDMA_CR_GRPxPRI_SHIFT(x))
#ifdef EDMA_HAS_CR_CX_ECX
  #define EDMA_CR_ECX               (1 << 16)
  #define EDMA_CR_CX                (1 << 17)
#endif
#ifdef EDMA_HAS_CR_VERSION
  #define EDMA_CR_VERSION_SHIFT     (24)
  #define EDMA_CR_VERSION_MASK      (0x7F << EDMA_CR_VERSION_SHIFT)
  #define EDMA_CR_VERSION(val)      (((val) << EDMA_CR_VERSION_SHIFT) & EDMA_CR_VERSION_MASK)
  #define EDMA_CR_VERSION_GET(reg)  (((reg) & EDMA_CR_VERSION_MASK) >> EDMA_CR_VERSION_SHIFT)
#endif
#ifdef EDMA_HAS_CR_ACTIVE
  #define EDMA_CR_ACTIVE            (1 << 31)
#endif

  uint32_t ESR;
#define EDMA_ESR_DBE                (1 << 0)
#define EDMA_ESR_SBE                (1 << 1)
#define EDMA_ESR_SGE                (1 << 2)
#define EDMA_ESR_NCE                (1 << 3)
#define EDMA_ESR_DOE                (1 << 4)
#define EDMA_ESR_DAE                (1 << 5)
#define EDMA_ESR_SOE                (1 << 6)
#define EDMA_ESR_SAE                (1 << 7)
#define EDMA_ESR_ERRCHN_SHIFT       (8)
#define EDMA_ESR_ERRCHN_MASK        (0x3F << EDMA_ESR_ERRCHN_SHIFT)
#define EDMA_ESR_ERRCHN(val)        (((val) << EDMA_ESR_ERRCHN_SHIFT) & EDMA_ESR_ERRCHN_MASK)
#define EDMA_ESR_ERRCHN_GET(reg)    (((reg) & EDMA_ESR_ERRCHN_MASK) >> EDMA_ESR_ERRCHN_SHIFT)
#define EDMA_ESR_CPE                (1 << 14)
#define EDMA_ESR_GPE                (1 << 15)
#ifdef EDMA_HAS_ESR_ECX
  #define EDMA_ESR_ECX              (1 << 16)
#endif
#define EDMA_ESR_VLD                (1 << 31)

  uint32_t ERRQH;
#define EDMA_ERRQH_ERRQ(x)          (1 << ((x) - 32))

  uint32_t ERRQL;
#define EDMA_ERRQL_ERRQ(x)          (1 << (x))

  uint32_t EEIH;
#define EDMA_EEIH_EEI(x)            (1 << ((x) - 32))

  uint32_t EEIL;
#define EDMA_EEIL_EEI(x)            (1 << (x))

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint8_t SERQR;
  uint8_t CERQR;
  uint8_t SEEIR;
  uint8_t CEEIR;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t CEEIR;
  uint8_t SEEIR;
  uint8_t CERQR;
  uint8_t SERQR;
#endif
#define EDMA_SERQR_NOP            (1 << 7)
#define EDMA_SERQR_SAER           (1 << 6)
#define EDMA_SERQR_SERQ_SHIFT     (0)
#define EDMA_SERQR_SERQ_MASK      (0x1F << EDMA_CIRQR_SERQ_SHIFT)
#define EDMA_SERQR_SERQ(val)      (((val) << EDMA_CIRQR_SERQ_SHIFT) & EDMA_CIRQR_SERQ_MASK)
#define EDMA_SERQR_SERQ_GET(reg)  (((reg) & EDMA_CIRQR_SERQ_MASK) >> EDMA_CIRQR_SERQ_SHIFT)
#define EDMA_CERQR_NOP            (1 << 7)
#define EDMA_CERQR_CAER           (1 << 6)
#define EDMA_CERQR_CERQ_SHIFT     (0)
#define EDMA_CERQR_CERQ_MASK      (0x1F << EDMA_CIRQR_CERQ_SHIFT)
#define EDMA_CERQR_CERQ(val)      (((val) << EDMA_CIRQR_CERQ_SHIFT) & EDMA_CIRQR_CERQ_MASK)
#define EDMA_CERQR_CERQ_GET(reg)  (((reg) & EDMA_CIRQR_CERQ_MASK) >> EDMA_CIRQR_CERQ_SHIFT)
#define EDMA_SEEIR_NOP            (1 << 7)
#define EDMA_SEEIR_SAEE           (1 << 6)
#define EDMA_SEEIR_SEEI_SHIFT     (0)
#define EDMA_SEEIR_SEEI_MASK      (0x1F << EDMA_CIRQR_SEEI_SHIFT)
#define EDMA_SEEIR_SEEI(val)      (((val) << EDMA_CIRQR_SEEI_SHIFT) & EDMA_CIRQR_SEEI_MASK)
#define EDMA_SEEIR_SEEI_GET(reg)  (((reg) & EDMA_CIRQR_SEEI_MASK) >> EDMA_CIRQR_SEEI_SHIFT)
#define EDMA_CEEIR_NOP            (1 << 7)
#define EDMA_CEEIR_CAEE           (1 << 6)
#define EDMA_CEEIR_CEEI_SHIFT     (0)
#define EDMA_CEEIR_CEEI_MASK      (0x1F << EDMA_CIRQR_CEEI_SHIFT)
#define EDMA_CEEIR_CEEI(val)      (((val) << EDMA_CIRQR_CEEI_SHIFT) & EDMA_CIRQR_CEEI_MASK)
#define EDMA_CEEIR_CEEI_GET(reg)  (((reg) & EDMA_CIRQR_CEEI_MASK) >> EDMA_CIRQR_CEEI_SHIFT)

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint8_t CIRQR;
  uint8_t CER;
  uint8_t SSBR;
  uint8_t CDSBR;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t CDSBR;
  uint8_t SSBR;
  uint8_t CER;
  uint8_t CIRQR;
#endif
#define EDMA_CIRQR_NOP            (1 << 7)
#define EDMA_CIRQR_CAIR           (1 << 6)
#define EDMA_CIRQR_CINT_SHIFT     (0)
#define EDMA_CIRQR_CINT_MASK      (0x1F << EDMA_CIRQR_CINT_SHIFT)
#define EDMA_CIRQR_CINT(val)      (((val) << EDMA_CIRQR_CINT_SHIFT) & EDMA_CIRQR_CINT_MASK)
#define EDMA_CIRQR_CINT_GET(reg)  (((reg) & EDMA_CIRQR_CINT_MASK) >> EDMA_CIRQR_CINT_SHIFT)
#define EDMA_CER_NOP              (1 << 7)
#define EDMA_CER_CAEI             (1 << 6)
#define EDMA_CER_CERR_SHIFT       (0)
#define EDMA_CER_CERR_MASK        (0x1F << EDMA_CIRQR_CERR_SHIFT)
#define EDMA_CER_CERR(val)        (((val) << EDMA_CIRQR_CERR_SHIFT) & EDMA_CIRQR_CERR_MASK)
#define EDMA_CER_CERR_GET(reg)    (((reg) & EDMA_CIRQR_CERR_MASK) >> EDMA_CIRQR_CERR_SHIFT)
#define EDMA_SSBR_NOP             (1 << 7)
#define EDMA_SSBR_SAST            (1 << 6)
#define EDMA_SSBR_SSB_SHIFT       (0)
#define EDMA_SSBR_SSB_MASK        (0x1F << EDMA_CIRQR_SSB_SHIFT)
#define EDMA_SSBR_SSB(val)        (((val) << EDMA_CIRQR_SSB_SHIFT) & EDMA_CIRQR_SSB_MASK)
#define EDMA_SSBR_SSB_GET(reg)    (((reg) & EDMA_CIRQR_SSB_MASK) >> EDMA_CIRQR_SSB_SHIFT)
#define EDMA_CDSBR_NOP            (1 << 7)
#define EDMA_CDSBR_CADN           (1 << 6)
#define EDMA_CDSBR_CDSB_SHIFT     (0)
#define EDMA_CDSBR_CDSB_MASK      (0x1F << EDMA_CIRQR_CDSB_SHIFT)
#define EDMA_CDSBR_CDSB(val)      (((val) << EDMA_CIRQR_CDSB_SHIFT) & EDMA_CIRQR_CDSB_MASK)
#define EDMA_CDSBR_CDSB_GET(reg)  (((reg) & EDMA_CIRQR_CDSB_MASK) >> EDMA_CIRQR_CDSB_SHIFT)

  uint32_t IRQH;
#define EDMA_IRQH_INT(x)         (1 << ((x) - 32))

  uint32_t IRQL;
#define EDMA_IRQL_INT(x)         (1 << ((x)))

  uint32_t ERH;
#define EDMA_ERH_ERR(x)          (1 << ((x) - 32))

  uint32_t ERL;
#define EDMA_ERL_ERR(x)          (1 << ((x)))

#ifdef EDMA_HAS_HRS
  uint32_t HRS;
#define EDMA_HRSH_HRS(x)         (1 << ((x) - 32))

  uint32_t HRSL;
#define EDMA_HRSL_HRS(x)         (1 << ((x)))
#else
  uint32_t reserved0030[2];
#endif

  uint32_t reserved0038[(0x44-0x38)/4];

#ifdef EDMA_HAS_EARS
  uint32_t EARSL;
#define EDMA_EARSL_EDREQ(x)      (1 << ((x)))       /* iMXRT only */
#else
  uint32_t reserved0044;
#endif

  uint32_t reserved0048[(0x100-0x48)/4];

  uint8_t CPR[64];
#define EDMA_CPR_CHPRI_SHIFT       (0)
#define EDMA_CPR_CHPRI_MASK        (0xF << EDMA_CPR_CHPRI_SHIFT)
#define EDMA_CPR_CHPRI(val)        (((val) << EDMA_CPR_CHPRI_SHIFT) & EDMA_CPR_CHPRI_MASK)
#define EDMA_CPR_CHPRI_GET(reg)    (((reg) & EDMA_CPR_CHPRI_MASK) >> EDMA_CPR_CHPRI_SHIFT)
#define EDMA_CPR_GRPPRI_SHIFT      (0)
#define EDMA_CPR_GRPPRI_MASK       (0xF << EDMA_CPR_GRPPRI_SHIFT)
#define EDMA_CPR_GRPPRI(val)       (((val) << EDMA_CPR_GRPPRI_SHIFT) & EDMA_CPR_GRPPRI_MASK)
#define EDMA_CPR_GRPPRI_GET(reg)   (((reg) & EDMA_CPR_GRPPRI_MASK) >> EDMA_CPR_GRPPRI_SHIFT)
#ifdef EDMA_HAS_CPR_DPA
  #define EDMA_CPR_DPA             (1 << 6)
#endif
#define EDMA_CPR_ECP               (1 << 7)

  uint32_t reserved0140[(0x1000-0x140)/4];

  struct fsl_edma_tcd {
    uint32_t SADDR;
    uint32_t SDF;
#define EDMA_TCD_SDF_SMOD_SHIFT      (27)
#define EDMA_TCD_SDF_SMOD_MASK       (0x1F << EDMA_TCD_SDF_SMOD_SHIFT)
#define EDMA_TCD_SDF_SMOD(val)       (((val) << EDMA_TCD_SDF_SMOD_SHIFT) & EDMA_TCD_SDF_SMOD_MASK)
#define EDMA_TCD_SDF_SMOD_GET(reg)   (((reg) & EDMA_TCD_SDF_SMOD_MASK) >> EDMA_TCD_SDF_SMOD_SHIFT)
#define EDMA_TCD_SDF_SSIZE_SHIFT     (24)
#define EDMA_TCD_SDF_SSIZE_MASK      (0x7 << EDMA_TCD_SDF_SSIZE_SHIFT)
#define EDMA_TCD_SDF_SSIZE(val)      (((val) << EDMA_TCD_SDF_SSIZE_SHIFT) & EDMA_TCD_SDF_SSIZE_MASK)
#define EDMA_TCD_SDF_SSIZE_GET(reg)  (((reg) & EDMA_TCD_SDF_SSIZE_MASK) >> EDMA_TCD_SDF_SSIZE_SHIFT)
#define EDMA_TCD_SDF_SSIZE_8BIT      EDMA_TCD_SDF_SSIZE(0)
#define EDMA_TCD_SDF_SSIZE_16BIT     EDMA_TCD_SDF_SSIZE(1)
#define EDMA_TCD_SDF_SSIZE_32BIT     EDMA_TCD_SDF_SSIZE(2)
#define EDMA_TCD_SDF_SSIZE_64BIT     EDMA_TCD_SDF_SSIZE(3)
#define EDMA_TCD_SDF_SSIZE_32BYTE    EDMA_TCD_SDF_SSIZE(5)
#define EDMA_TCD_SDF_DMOD_SHIFT      (19)
#define EDMA_TCD_SDF_DMOD_MASK       (0x1F << EDMA_TCD_SDF_DMOD_SHIFT)
#define EDMA_TCD_SDF_DMOD(val)       (((val) << EDMA_TCD_SDF_DMOD_SHIFT) & EDMA_TCD_SDF_DMOD_MASK)
#define EDMA_TCD_SDF_DMOD_GET(reg)   (((reg) & EDMA_TCD_SDF_DMOD_MASK) >> EDMA_TCD_SDF_DMOD_SHIFT)
#define EDMA_TCD_SDF_DSIZE_SHIFT     (16)
#define EDMA_TCD_SDF_DSIZE_MASK      (0x7 << EDMA_TCD_SDF_DSIZE_SHIFT)
#define EDMA_TCD_SDF_DSIZE(val)      (((val) << EDMA_TCD_SDF_DSIZE_SHIFT) & EDMA_TCD_SDF_DSIZE_MASK)
#define EDMA_TCD_SDF_DSIZE_GET(reg)  (((reg) & EDMA_TCD_SDF_DSIZE_MASK) >> EDMA_TCD_SDF_DSIZE_SHIFT)
#define EDMA_TCD_SDF_DSIZE_8BIT      EDMA_TCD_SDF_DSIZE(0)
#define EDMA_TCD_SDF_DSIZE_16BIT     EDMA_TCD_SDF_DSIZE(1)
#define EDMA_TCD_SDF_DSIZE_32BIT     EDMA_TCD_SDF_DSIZE(2)
#define EDMA_TCD_SDF_DSIZE_64BIT     EDMA_TCD_SDF_DSIZE(3)
#define EDMA_TCD_SDF_DSIZE_32BYTE    EDMA_TCD_SDF_DSIZE(5)
#define EDMA_TCD_SDF_SOFF_SHIFT      (0)
#define EDMA_TCD_SDF_SOFF_MASK       (0xFFFF << EDMA_TCD_SDF_SOFF_SHIFT)
#define EDMA_TCD_SDF_SOFF(val)       (((val) << EDMA_TCD_SDF_SOFF_SHIFT) & EDMA_TCD_SDF_SOFF_MASK)
#define EDMA_TCD_SDF_SOFF_GET(reg)   (((reg) & EDMA_TCD_SDF_SOFF_MASK) >> EDMA_TCD_SDF_SOFF_SHIFT)

    uint32_t NBYTES;
#define EDMA_TCD_NBYTES_ALT_NBYTES_SHIFT    (0)
#define EDMA_TCD_NBYTES_ALT_NBYTES_MASK     (0x3FF << EDMA_TCD_NBYTES_ALT_NBYTES_SHIFT)
#define EDMA_TCD_NBYTES_ALT_NBYTES(val)     (((val) << EDMA_TCD_NBYTES_ALT_NBYTES_SHIFT) & EDMA_TCD_NBYTES_ALT_NBYTES_MASK)
#define EDMA_TCD_NBYTES_ALT_MLOFF_SHIFT     (10)
#define EDMA_TCD_NBYTES_ALT_MLOFF_MASK      (0xFFFFF << EDMA_TCD_NBYTES_ALT_MLOFF_SHIFT)
#define EDMA_TCD_NBYTES_ALT_MLOFF(val)      (((val) << EDMA_TCD_NBYTES_ALT_MLOFF_SHIFT) & EDMA_TCD_NBYTES_ALT_MLOFF_MASK)
#define EDMA_TCD_NBYTES_ALT_DMLOE           (1 << 30)
#define EDMA_TCD_NBYTES_ALT_SMLOE           (1 << 31)
    int32_t SLAST;
    uint32_t DADDR;
    uint32_t CDF;
#define EDMA_TCD_CDF_CITERE_LINK    (1 << 31)
#define EDMA_TCD_CDF_CITER_SHIFT    (16)
#define EDMA_TCD_CDF_CITER_MASK     (0x7FFF << EDMA_TCD_CDF_CITER_SHIFT)
#define EDMA_TCD_CDF_CITER(val)     (((val) << EDMA_TCD_CDF_CITER_SHIFT) & EDMA_TCD_CDF_CITER_MASK)
#define EDMA_TCD_CDF_CITER_GET(reg) (((reg) & EDMA_TCD_CDF_CITER_MASK) >> EDMA_TCD_CDF_CITER_SHIFT)
#define EDMA_TCD_CDF_DOFF_SHIFT     (0)
#define EDMA_TCD_CDF_DOFF_MASK      (0xFFFF << EDMA_TCD_CDF_DOFF_SHIFT)
#define EDMA_TCD_CDF_DOFF(val)      (((val) << EDMA_TCD_CDF_DOFF_SHIFT) & EDMA_TCD_CDF_DOFF_MASK)
#define EDMA_TCD_CDF_DOFF_GET(reg)  (((reg) & EDMA_TCD_CDF_DOFF_MASK) >> EDMA_TCD_CDF_DOFF_SHIFT)

#define EDMA_TCD_CDF_ALT_CITERLINKCH_SHIFT    (25)
#define EDMA_TCD_CDF_ALT_CITERLINKCH_MASK     (0x3F << EDMA_TCD_CDF_ALT_CITERLINKCH_SHIFT)
#define EDMA_TCD_CDF_ALT_CITERLINKCH(val)     (((val) << EDMA_TCD_CDF_ALT_CITERLINKCH_SHIFT) & EDMA_TCD_CDF_ALT_CITERLINKCH_MASK)
#define EDMA_TCD_CDF_ALT_CITERLINKCH_GET(reg) (((reg) & EDMA_TCD_CDF_ALT_CITERLINKCH_MASK) >> EDMA_TCD_CDF_ALT_CITERLINKCH_SHIFT)
#define EDMA_TCD_CDF_ALT_CITER_SHIFT          (16)
#define EDMA_TCD_CDF_ALT_CITER_MASK           (0x1FF << EDMA_TCD_CDF_ALT_CITER_SHIFT)
#define EDMA_TCD_CDF_ALT_CITER(val)           (((val) << EDMA_TCD_CDF_ALT_CITER_SHIFT) & EDMA_TCD_CDF_ALT_CITER_MASK)
#define EDMA_TCD_CDF_ALT_CITER_GET(reg)       (((reg) & EDMA_TCD_CDF_ALT_CITER_MASK) >> EDMA_TCD_CDF_ALT_CITER_SHIFT)

#define EDMA_TCD_CDF_NOLINK_CITER_SHIFT    (16)
#define EDMA_TCD_CDF_NOLINK_CITER_MASK     (0xFFFF << EDMA_TCD_CDF_NOLINK_CITER_SHIFT)
#define EDMA_TCD_CDF_NOLINK_CITER(val)     (((val) << EDMA_TCD_CDF_NOLINK_CITER_SHIFT) & EDMA_TCD_CDF_NOLINK_CITER_MASK)
#define EDMA_TCD_CDF_NOLINK_CITER_GET(reg) (((reg) & EDMA_TCD_CDF_NOLINK_CITER_MASK) >> EDMA_TCD_CDF_NOLINK_CITER_SHIFT)

    int32_t DLAST_SGA;
    uint32_t BMF;
#define EDMA_TCD_BMF_BITERE_LINK          (1 << 31)
#define EDMA_TCD_BMF_BITER_SHIFT          (16)
#define EDMA_TCD_BMF_BITER_MASK           (0x7FFF << EDMA_TCD_BMF_BITER_SHIFT)
#define EDMA_TCD_BMF_BITER(val)           (((val) << EDMA_TCD_BMF_BITER_SHIFT) & EDMA_TCD_BMF_BITER_MASK)
#define EDMA_TCD_BMF_BITER_GET(reg)       (((reg) & EDMA_TCD_BMF_BITER_MASK) >> EDMA_TCD_BMF_BITER_SHIFT)
#define EDMA_TCD_BMF_BWC_SHIFT            (14)
#define EDMA_TCD_BMF_BWC_MASK             (0x3 << EDMA_TCD_BMF_BWC_SHIFT)
#define EDMA_TCD_BMF_BWC(val)             (((val) << EDMA_TCD_BMF_BWC_SHIFT) & EDMA_TCD_BMF_BWC_MASK)
#define EDMA_TCD_BMF_BWC_GET(reg)         (((reg) & EDMA_TCD_BMF_BWC_MASK) >> EDMA_TCD_BMF_BWC_SHIFT)
#define EDMA_TCD_BMF_MAJORLINKCH_SHIFT    (8)
#define EDMA_TCD_BMF_MAJORLINKCH_MASK     (0x3F << EDMA_TCD_BMF_MAJORLINKCH_SHIFT)
#define EDMA_TCD_BMF_MAJORLINKCH(val)     (((val) << EDMA_TCD_BMF_MAJORLINKCH_SHIFT) & EDMA_TCD_BMF_MAJORLINKCH_MASK)
#define EDMA_TCD_BMF_MAJORLINKCH_GET(reg) (((reg) & EDMA_TCD_BMF_MAJORLINKCH_MASK) >> EDMA_TCD_BMF_MAJORLINKCH_SHIFT)
#define EDMA_TCD_BMF_DONE                 (1 << 7)
#define EDMA_TCD_BMF_ACTIVE               (1 << 6)
#define EDMA_TCD_BMF_MAJORE_LINK          (1 << 5)
#define EDMA_TCD_BMF_E_SG                 (1 << 4)
#define EDMA_TCD_BMF_D_REQ                (1 << 3)
#define EDMA_TCD_BMF_INT_HALF             (1 << 2)
#define EDMA_TCD_BMF_INT_MAJ              (1 << 1)
#define EDMA_TCD_BMF_START                (1 << 0)

#define EDMA_TCD_BMF_ALT_BITERLINKCH_SHIFT    (25)
#define EDMA_TCD_BMF_ALT_BITERLINKCH_MASK     (0x3F << EDMA_TCD_BMF_ALT_BITERLINKCH_SHIFT)
#define EDMA_TCD_BMF_ALT_BITERLINKCH(val)     (((val) << EDMA_TCD_BMF_ALT_BITERLINKCH_SHIFT) & EDMA_TCD_BMF_ALT_BITERLINKCH_MASK)
#define EDMA_TCD_BMF_ALT_BITERLINKCH_GET(reg) (((reg) & EDMA_TCD_BMF_ALT_BITERLINKCH_MASK) >> EDMA_TCD_BMF_ALT_BITERLINKCH_SHIFT)
#define EDMA_TCD_BMF_ALT_BITER_SHIFT          (16)
#define EDMA_TCD_BMF_ALT_BITER_MASK           (0x1FF << EDMA_TCD_BMF_ALT_BITER_SHIFT)
#define EDMA_TCD_BMF_ALT_BITER(val)           (((val) << EDMA_TCD_BMF_ALT_BITER_SHIFT) & EDMA_TCD_BMF_ALT_BITER_MASK)
#define EDMA_TCD_BMF_ALT_BITER_GET(reg)       (((reg) & EDMA_TCD_BMF_ALT_BITER_MASK) >> EDMA_TCD_BMF_ALT_BITER_SHIFT)

#define EDMA_TCD_BMF_NOLINK_BITER_SHIFT    (16)
#define EDMA_TCD_BMF_NOLINK_BITER_MASK     (0xFFFF << EDMA_TCD_BMF_NOLINK_BITER_SHIFT)
#define EDMA_TCD_BMF_NOLINK_BITER(val)     (((val) << EDMA_TCD_BMF_NOLINK_BITER_SHIFT) & EDMA_TCD_BMF_NOLINK_BITER_MASK)
#define EDMA_TCD_BMF_NOLINK_BITER_GET(reg) (((reg) & EDMA_TCD_BMF_NOLINK_BITER_MASK) >> EDMA_TCD_BMF_NOLINK_BITER_SHIFT)

  } TCD[64];
};

#ifndef  __cplusplus
static const struct fsl_edma_tcd EDMA_TCD_DEFAULT = {
    .SADDR = 0,
    .SDF = 0,
    .NBYTES = 0,
    .SLAST = 0,
    .DADDR = 0,
    .CDF = 0,
    .DLAST_SGA = 0,
    .BMF = 0,
};
#endif /* __cplusplus */

#define EDMA_TCD_BITER_MASK 0x7fff

#define EDMA_TCD_BITER_SIZE (EDMA_TCD_BITER_MASK + 1)

#define EDMA_TCD_BITER_LINKED_MASK 0x1ff

#define EDMA_TCD_BITER_LINKED_SIZE (EDMA_TCD_BITER_LINKED_MASK + 1)

#define EDMA_TCD_LINK_AND_BITER(link, biter) \
  (((link) << 9) + ((biter) & EDMA_TCD_BITER_LINKED_MASK))

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_FSL_REGS_EDMA_H */
