/*! \file
 *  Altera - SoC FPGA Address Space Manager
 */

/******************************************************************************
*
* Copyright 2013 Altera Corporation. All Rights Reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
* 
* 3. The name of the author may not be used to endorse or promote products
* derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED. IN NO
* EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
* 
******************************************************************************/

#ifndef __ALT_ADDR_SPACE_H__
#define __ALT_ADDR_SPACE_H__

#include <stdbool.h>
#include "hwlib.h"
#include "socal/hps.h"

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/******************************************************************************/
// ARM Level 2 Cache Controller L2C-310 Register Interface

// Address Filtering Start Register
// The Address Filtering Start Register is a read and write register.
// Bits     Field                       Description
// :-------|:--------------------------|:-----------------------------------------
// [31:20] | address_filtering_start   | Address filtering start address for
//         |                           | bits [31:20] of the filtering address.
// [19:1]  | Reserved                  | SBZ/RAZ
// [0]     | address_filtering_enable  | 0 - address filtering disabled
//         |                           | 1 - address filtering enabled.

// Address Filtering Start Register Address
#define L2_CACHE_ADDR_FILTERING_START_OFST      0xC00
#define L2_CACHE_ADDR_FILTERING_START_ADDR      (ALT_MPUL2_OFST + L2_CACHE_ADDR_FILTERING_START_OFST)
// Address Filtering Start Register - Start Value Mask
#define L2_CACHE_ADDR_FILTERING_START_ADDR_MASK 0xFFF00000
// Address Filtering Start Register - Reset Start Address Value (1 MB)
#define L2_CACHE_ADDR_FILTERING_START_RESET     0x100000
// Address Filtering Start Register - Enable Flag Mask
#define L2_CACHE_ADDR_FILTERING_ENABLE_MASK     0x00000001
// Address Filtering Start Register - Reset Enable Flag Value (Enabled)
#define L2_CACHE_ADDR_FILTERING_ENABLE_RESET    0x1

// Address Filtering End Register
// The Address Filtering End Register is a read and write register.
// Bits     Field                       Description
// :-------|:--------------------------|:-----------------------------------------
// [31:20] | address_filtering_end     | Address filtering end address for bits
//         |                           | [31:20] of the filtering address.
// [19:0]  | Reserved                  | SBZ/RAZ

// Address Filtering End Register Address
#define L2_CACHE_ADDR_FILTERING_END_OFST        0xC04
#define L2_CACHE_ADDR_FILTERING_END_ADDR        (ALT_MPUL2_OFST + L2_CACHE_ADDR_FILTERING_END_OFST)
// Address Filtering End Register - End Value Mask
#define L2_CACHE_ADDR_FILTERING_END_ADDR_MASK   0xFFF00000
// Address Filtering End Register - Reset End Address Value (3 GB)
#define L2_CACHE_ADDR_FILTERING_END_RESET       0xC0000000

#ifndef __ASSEMBLY__

/******************************************************************************/
/*! \addtogroup ADDR_SPACE_MGR The Address Space Manager
 *
 * This module contains group APIs for managing the HPS address space. This
 * module contains group APIs to manage:
 * * Memory Map Control
 * * Memory Coherence
 * * Cache Managment
 * * MMU Managment
 *
 * @{
 */

/******************************************************************************/
/*! \addtogroup ADDR_SPACE_MGR_REMAP Address Space Mapping Control
 *
 * This group API provides functions to map and remap selected address ranges
 * into the accessible (visible) views of the MPU and non MPU address spaces.
 *
 * \b Caveats
 *
 * \b NOTE: Caution should be observed when remapping address 0 to different
 * memory.  The code performing the remapping operation should not be executing
 * in the address range being remapped to different memory.
 *
 * For example, if address 0 is presently mapped to OCRAM and the code is
 * preparing to remap address 0 to SDRAM, then the code must not be executing in
 * the range 0 to 64 KB as this address space is about to be remapped to
 * different memory. If the code performing the remap operation is executing
 * from OCRAM then it needs to be executing from its permanently mapped OCRAM
 * address range in upper memory (i.e. ALT_OCRAM_LB_ADDR to ALT_OCRAM_UB_ADDR).
 *
 * \b NOTE: The MPU address space view is controlled by two disparate hardware
 * control interfaces: the L3 remap register and the L2 cache address filtering
 * registers. To complicate matters, the L3 remap register is write-only which
 * means not only that current remap register state cannot be read but also that
 * a read-modify-write operation cannot be performed on the register.
 *
 * This should not present a problem in most use case scenarios except for the
 * case where a current MPU address space mapping of 0 to SDRAM is being changed
 * to to a mapping of 0 to Boot ROM or OCRAM.
 *
 * In this case, a two step process whereby the L3 remap register is first set
 * to the new desired MPU address 0 mapping and then the L2 cache address
 * filtering registers have their address ranges adjusted accordingly must be
 * followed. An example follows:
\verbatim
// 1 MB reset default value for address filtering start
#define L2_CACHE_ADDR_FILTERING_START_RESET     0x100000
uint32_t addr_filt_start;
uint32_t addr_filt_end;

// Perform L3 remap register programming first by setting the desired new MPU
// address space 0 mapping. Assume OCRAM for the example.
alt_addr_space_remap(ALT_ADDR_SPACE_MPU_ZERO_AT_OCRAM, ...);

// Next, adjust the L2 cache address filtering range. Set the start address to
// the default reset value and retain the existing end address configuration.
alt_l2_addr_filter_cfg_get(&addr_filt_start, &addr_filt_end);
if (addr_filt_start != L2_CACHE_ADDR_FILTERING_START_RESET)
{
    alt_l2_addr_filter_cfg_set(L2_CACHE_ADDR_FILTERING_START_RESET, addr_filt_end);
}
\endverbatim
 * @{
 */

/******************************************************************************/
/*!
 * This type definition enumerates the MPU address space attributes.
 *
 * The MPU address space consists of the ARM Cortex A9 processors and associated
 * processor peripherals (cache, MMU).
 */
typedef enum ALT_ADDR_SPACE_MPU_ATTR_e
{
    ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,     /*!< Maps the Boot ROM to address
                                             *   0x0 for the MPU L3 master. Note
                                             *   that the Boot ROM is also
                                             *   always mapped to address
                                             *   0xfffd_0000 for the MPU L3
                                             *   master independent of
                                             *   attribute.
                                             */

    ALT_ADDR_SPACE_MPU_ZERO_AT_OCRAM        /*!< Maps the On-chip RAM to address
                                             *   0x0 for the MPU L3 master. Note
                                             *   that the On-chip RAM is also
                                             *   always mapped to address
                                             *   0xffff_0000 for the MPU L3
                                             *   master independent of this
                                             *   attribute.
                                             */
} ALT_ADDR_SPACE_MPU_ATTR_t;

/******************************************************************************/
/*!
 * This type definition enumerates the non-MPU address space attributes.
 *
 * The non-MPU address space consists of the non-MPU L3 masters including the
 * DMA controllers (standalone and those built into peripherals), the F2H AXI
 * Bridge, and the DAP.
 */
typedef enum ALT_ADDR_SPACE_NONMPU_ATTR_e
{
    ALT_ADDR_SPACE_NONMPU_ZERO_AT_OCRAM,    /*!< Maps the SDRAM to address 0x0
                                             *   for the non-MPU L3 masters.
                                             */
    ALT_ADDR_SPACE_NONMPU_ZERO_AT_SDRAM     /*!< Maps the On-chip RAM to address
                                             *   0x0 for the non-MPU L3
                                             *   masters. Note that the On-chip
                                             *   RAM is also always mapped to
                                             *   address 0xffff_0000 for the
                                             *   non-MPU L3 masters independent
                                             *   of this attribute.
                                             */
} ALT_ADDR_SPACE_NONMPU_ATTR_t;

/******************************************************************************/
/*!
 * This type definition enumerates the HPS to FPGA bridge accessiblity
 * attributes.
 */
typedef enum ALT_ADDR_SPACE_H2F_BRIDGE_ATTR_e
{
    ALT_ADDR_SPACE_H2F_INACCESSIBLE,        /*!< The H2F AXI Bridge is not
                                             *   visible to L3 masters. Accesses
                                             *   to the associated address range
                                             *   return an AXI decode error to
                                             *   the master.
                                             */
    ALT_ADDR_SPACE_H2F_ACCESSIBLE           /*!< The H2F AXI Bridge is visible
                                             *   to L3 masters.
                                             */
} ALT_ADDR_SPACE_H2F_BRIDGE_ATTR_t;

/******************************************************************************/
/*!
 * This type definition enumerates the Lightweight HPS to FPGA bridge
 * accessiblity attributes.
 */
typedef enum ALT_ADDR_SPACE_LWH2F_BRIDGE_ATTR_e
{
    ALT_ADDR_SPACE_LWH2F_INACCESSIBLE,      /*!< The LWH2F AXI Bridge is not
                                             *   visible to L3 masters. Accesses
                                             *   to the associated address range
                                             *   return an AXI decode error to
                                             *   the master.
                                             */
    ALT_ADDR_SPACE_LWH2F_ACCESSIBLE         /*!< The LWH2F AXI Bridge is visible
                                             *   to L3 masters.
                                             */
} ALT_ADDR_SPACE_LWH2F_BRIDGE_ATTR_t;

/******************************************************************************/
/*!
 * Configures the mapped and accessible (visible) address ranges for the HPS
 * MPU, non-MPU, and Bridge address spaces.
 *
 * \param       mpu_attr
 *              The MPU address space configuration attributes.
 *              
 * \param       nonmpu_attr
 *              The non-MPU address space configuration attributes.
 *              
 * \param       h2f_attr
 *              The H2F Bridge attribute mapping and accessibility attributes.
 *              
 * \param       lwh2f_attr
 *              The Lightweight H2F Bridge attribute mapping and accessibility
 *              attributes.
 *              
 * 
 * \retval      ALT_E_SUCCESS       The operation was succesful.
 * \retval      ALT_E_ERROR         The operation failed.
 * \retval      ALT_E_INV_OPTION    One or more invalid attribute options were
 *                                  specified.
 */
ALT_STATUS_CODE alt_addr_space_remap(ALT_ADDR_SPACE_MPU_ATTR_t mpu_attr,
                                     ALT_ADDR_SPACE_NONMPU_ATTR_t nonmpu_attr,
                                     ALT_ADDR_SPACE_H2F_BRIDGE_ATTR_t h2f_attr,
                                     ALT_ADDR_SPACE_LWH2F_BRIDGE_ATTR_t lwh2f_attr);

/******************************************************************************/
/*!
 * Maps SDRAM to address 0x0 for the MPU address space view.
 *
 * When address 0x0 is mapped to the Boot ROM or on-chip RAM, only the lowest
 * 64KB of the boot region are accessible because the size of the Boot ROM and
 * on-chip RAM are only 64KB.  Addresses in the range 0x100000 (1MB) to
 * 0xC0000000 (3GB) access SDRAM and addresses in the range 0xC0000000 (3GB) to
 * 0xFFFFFFFF access the L3 interconnect. Thus, the lowest 1MB of SDRAM is not
 * accessible to the MPU unless address 0 is remapped to SDRAM after reset.
 *
 * This function remaps the addresses between 0x0 to 0x100000 (1MB) to access
 * SDRAM.
 *
 * \internal
 * The remap to address 0x0 is achieved by configuring the L2 cache Address
 * Filtering Registers to redirect address 0x0 to \e sdram_end_addr to the SDRAM
 * AXI (M1) master port by calling:
 *
 * alt_l2_addr_filter_cfg_set(0x0, <current_addr_filt_end_value>);
 * 
 * See: <em>ARM DDI 0246F, CoreLink Level 2 Cache Controller L2C-310 Technical
 * Reference Manual, Section 3.3.12 Address Filtering </em>.
 * \endinternal
 *
 * \retval      ALT_E_SUCCESS   The operation was succesful.
 * \retval      ALT_E_ERROR     The operation failed.
 */
ALT_STATUS_CODE alt_mpu_addr_space_remap_0_to_sdram(void);

/*! @} */

/******************************************************************************/
/*! \addtogroup L2_ADDR_FLTR L2 Cache Address Filter
 *
 * The L2 cache address filter controls where physical addresses within certain
 * ranges of the MPU address space are directed.
 *
 * The L2 cache has master port connections to the L3 interconnect and the SDRAM
 * controller. A programmable address filter controls which portions of the
 * 32-bit physical address space use each master.
 * 
 * When l2 address filtering is configured and enabled, a physical address will
 * be redirected to one master or the other based upon the address filter
 * configuration.
 *
 * If \b address_filter_start <= \e physical_address < \b address_filter_end:
 * * then redirect \e physical_address to AXI Master Port M1 (SDRAM controller)
 * * else redirect \e physical_address to AXI Master Port M0 (L3 interconnect)
 *
 * See: <em>ARM DDI 0246F, CoreLink Level 2 Cache Controller L2C-310 Technical
 * Reference Manual, Section 3.3.12 Address Filtering </em> for more information.
 * @{
 */

/******************************************************************************/
/*!
 * Get the L2 cache address filtering configuration settings.
 *
 * \param       addr_filt_start
 *              [out] An output parameter variable for the address filtering
 *              start address for the range of physical addresses redirected to
 *              the SDRAM AXI master port. The value returned is always a 1 MB
 *              aligned address.
 *              
 * \param       addr_filt_end
 *              [out] An output parameter variable for the address filtering
 *              end address for the range of physical addresses redirected to
 *              the SDRAM AXI master port. The value returned is always a 1 MB
 *              aligned address.
 *
 * \retval      ALT_E_SUCCESS   The operation was successful.
 * \retval      ALT_E_ERROR     The operation failed.
 * \retval      ALT_E_BAD_ARG   An bad argument was passed. Either \e addr_filt_start
 *                              or \e addr_filt_end or both are invalid addresses.
 */
ALT_STATUS_CODE alt_l2_addr_filter_cfg_get(uint32_t* addr_filt_start,
                                           uint32_t* addr_filt_end);

/******************************************************************************/
/*!
 * Set the L2 cache address filtering configuration settings.
 *
 * Address filtering start and end values must be 1 MB aligned.
 *
 * \param       addr_filt_start
 *              The address filtering start address for the range of physical
 *              addresses redirected to the SDRAM AXI master port. Only bits
 *              [31:20] of the address are valid. Any bits outside the range
 *              [31:20] are invalid and will cause an error status to be
 *              returned.
 *              
 * \param       addr_filt_end
 *              The address filtering end address for the range of physical
 *              addresses redirected to the SDRAM AXI master port. Only bits
 *              [31:20] of the address are valid. Any bits outside the range
 *              [31:20] are invalid and will cause an error status to be
 *              returned.
 *              
 * \retval      ALT_E_SUCCESS   The operation was succesful.
 * \retval      ALT_E_ERROR     The operation failed.
 * \retval      ALT_E_ARG_RANGE An argument violates a range constraint. One or
 *                              more address arguments do not satisfy the argument
 *                              constraints.
 */
ALT_STATUS_CODE alt_l2_addr_filter_cfg_set(uint32_t addr_filt_start,
                                           uint32_t addr_filt_end);

/*! @} */

/*! @} */

#endif  /* __ASSEMBLY__ */

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __ALT_ADDR_SPACE_H__ */
