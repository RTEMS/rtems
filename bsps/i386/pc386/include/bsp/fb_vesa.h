/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Headers specific for framebuffer drivers utilizing VESA VBE.
 */

/*
 * Copyright (C) 2014  Jan Doležal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp/vbe3.h>
#include <edid.h>

#ifndef _FB_VESA_H
#define _FB_VESA_H

#ifndef ASM /* ASM */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ----- Prototypes ----- */

/**
 * @brief Returns information about graphic's controller in the \p info_block
 * structure.
 *
 * @param[out] info_block pointer to the struct to be filled with
 *                  controller information
 * @param[in] queried_VBE_Version if >0x200 then video bios is asked to fill in
 *                          parameters which appeared with second version
 *                          of VBE.
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_controller_information (
    VBE_vbe_info_block *info_block,
    uint16_t queried_VBE_Version
);

/**
 * @brief Fills structure \p info_block with informations about selected mode in
 * \p mode_number variable.
 *
 * @param[out] info_block pointer to the struct to be filled with
 *                        mode information
 * @param[in] mode_number detailes of this mode to be filled
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_mode_information (
    VBE_mode_info_block *info_block,
    uint16_t mode_number
);

/**
 * @brief Sets graphics mode selected. If mode has refreshRateCtrl bit set, than
 * the \p info_block must be filled accordingly.
 *
 * @param[in] mode_number number of mode to be set
 * @param[in] info_block pointer to struct containing refresh rate control info
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_set_mode (
    uint16_t mode_number,
    VBE_CRTC_info_block *info_block
);

/**
 * @brief Get currently set mode number.
 *
 * @param[out] mode_number variable to be filled with current mode number
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_current_mode (
    uint16_t *mode_number
);

/**
 * @brief Gets information about display data channel implemented in the
 * graphic's controller.
 *
 * @param[in] controller_unit_number
 * @param[out] seconds_to_transfer_EDID_block approximate time to transfer one
 *                                      EDID block rounded up to seconds
 * @param[out] DDC_level_supported contains DDC version supported and
 *                          screen blanking state during transfer
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_report_DDC_capabilities (
    uint16_t controller_unit_number,
    uint8_t *seconds_to_transfer_EDID_block,
    uint8_t *DDC_level_supported
);

/**
 * @brief Reads selected EDID block from display attached to controller's
 * interface.
 *
 * @param[in] controller_unit_number
 * @param[in] EDID_block_number block no. to be read from the display
 * @param[out] buffer place to store block fetched from the display
 * @retval  ax register content as defined in VBE RETURN STATUS paragraph
 * @retval  -1 error calling graphical bios
 */
uint32_t VBE_read_EDID (
    uint16_t controller_unit_number,
    uint16_t EDID_block_number,
    EDID_edid1 *buffer
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _FB_VESA_H */
