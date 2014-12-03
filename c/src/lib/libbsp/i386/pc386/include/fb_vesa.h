/**
 * @file fb_vesa.h
 *
 * @ingroup i386_pc386
 *
 * @brief Definitioins for vesa based framebuffer drivers.
 */

/*
 * Headers specific for framebuffer drivers utilizing VESA VBE.
 *
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
 * Returns information about graphic's controller in the infoBlock structure.
 *
 * @param infoBlock pointer to the struct to be filled with
                    controller information
 * @param queriedVBEVersion if >0x200 then video bios is asked to fill in
 *                          parameters which appeared with second version
 *                          of VBE.
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBE_controller_information (
    VBE_vbe_info_block *info_block,
    uint16_t queried_VBE_Version
);

/**
 * Fills structure infoBlock with informations about selected mode in
 * modeNumber variable.
 *
 * @param infoBlock pointer to the struct to be filled with mode information
 * @param modeNumber detailes of this mode to be filled
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBE_mode_information (
    VBE_mode_info_block *info_block,
    uint16_t mode_number
);

/**
 * Sets graphics mode selected. If mode has refreshRateCtrl bit set, than the
 * infoBlock must be filled accordingly.
 *
 * @param modeNumber number of mode to be set
 * @param infoBlock pointer to struct containing refresh rate control info
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBE_set_mode (
    uint16_t mode_number,
    VBE_CRTC_info_block *info_block
);

/**
 * Get currently set mode number.
 *
 * @param modeNumber variable to be filled with current mode number
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBE_current_mode (
    uint16_t *mode_number
);

/**
 * Gets information about display data channel implemented in the
 * graphic's controller.
 *
 * @param controllerUnitNumber
 * @param secondsToTransferEDIDBlock approximate time to transfer one EDID block
 *                                   rounded up to seconds
 * @param DDCLevelSupported after call contains DDC version supported and
 *                          screen blanking state during transfer
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBE_report_DDC_capabilities (
    uint16_t controller_unit_number,
    uint8_t *seconds_to_transfer_EDID_block,
    uint8_t *DDC_level_supported
);

/**
 * Reads selected EDID block from display attached to controller's interface.
 *
 * @param controllerUnitNumber
 * @param EDIDBlockNumber block no. to be read from the display
 * @param buffer place to store block fetched from the display
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
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
