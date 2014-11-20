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
uint32_t VBEControllerInformation (
    struct VBE_VbeInfoBlock *infoBlock,
    uint16_t queriedVBEVersion
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
uint32_t VBEModeInformation (
    struct VBE_ModeInfoBlock *infoBlock,
    uint16_t modeNumber
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
uint32_t VBESetMode (
    uint16_t modeNumber,
    struct VBE_CRTCInfoBlock *infoBlock
);

/**
 * Get currently set mode number.
 *
 * @param modeNumber variable to be filled with current mode number
 * @retval  register ax content as defined in VBE RETURN STATUS paragraph
 *          -1 error calling graphical bios
 */
uint32_t VBECurrentMode (
    uint16_t *modeNumber
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
uint32_t VBEReportDDCCapabilities (
    uint16_t controllerUnitNumber,
    uint8_t *secondsToTransferEDIDBlock,
    uint8_t *DDCLevelSupported
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
uint32_t VBEReadEDID (
    uint16_t controllerUnitNumber,
    uint16_t EDIDBlockNumber,
    struct edid1 *buffer
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _FB_VESA_H */
