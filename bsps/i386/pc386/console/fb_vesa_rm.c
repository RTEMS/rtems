/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief FB driver for graphic hardware compatible with VESA Bios Extension
 *     Real mode interface utilized
 *     Tested on real HW.
 *
 * Public sources related:
 *   - VESA BIOS EXTENSION (VBE) Core Function Standard, Ver: 3.0, Sep 16, 1998
 *   - VESA Enhanced Extended Display Identification Data (E-EDID) Standard
 *     Release A, Revision 2, September 25, 2006
 *
 * Hardware is completely initialized upon boot of the system.
 * Therefore there is no way to change graphics mode later.
 *
 * Interrupt 0x10 is used for entering graphics BIOS.
 *
 * Driver reads parameter from multiboot command line to setup video:
 * "--video=<resX>x<resY>[-<bpp>]"
 * "--video=auto" - try EDID to find mode that fits the display attached best
 * "--video=none" / "--video=off" - do not initialize the driver
 * If cmdline parameter is not specified the rtems_fb_default_mode
 * variable content is tested (see doc below).
 * Command line option has higher priority. rtems_fb_default_mode is probed
 * only if cmdline "--video=" is not specified at all.
 *
 * If neither of the above options is specified the driver is not initialized.
 */

/*
 * Copyright (c) 2014 - CTU in Prague
 *                      Jan Dolezal ( dolezj21@fel.cvut.cz )
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * The code for rtems_buffer_* functions was greatly
 * inspired or coppied from:
 *   - RTEMS fb_cirrus.c - Alexandru-Sever Horin (alex.sever.h@gmail.com)
 */

#include <inttypes.h>

#include <bsp.h>

#include <bsp/fb_default_mode.h>
#include <bsp/fb_vesa.h>
#include <bsp/realmode_int.h>

#include <pthread.h>

#include <rtems/libio.h>

#include <rtems/fb.h>
#include <rtems/framebuffer.h>

#include <rtems/score/atomic.h>

#include <stdlib.h>

#define FB_VESA_NAME    "FB_VESA_RM"

/*
 * GCC complains that access to packed data may not be aligned and
 * fair enough. The warning is:
 *
 *   warning: taking address of packed member of 'struct <anonymous>' may
 *   result in an unaligned pointer value [-Waddress-of-packed-member]
 *
 * Disable the warning.
 */
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

/**
 * @brief Allows to enable initialization of VESA real mode driver from
 * an application by setting the value of this variable to non null value in
 * user's module. The value of this variable will be then updated
 * when linked with application's object.
 *
 * Further the value should point to string in the following format:
 * "<resX>x<resY>[-<bpp>]" - e.g. "1024x768-32"
 * "auto" - try EDID to find mode that fits the display attached best
 * "none" / "off" - do not initialize the driver
 * the given parameters are used if applicable.
 *
 * Command line argument "--video=" has priority over this string.
 */
const char * const rtems_fb_default_mode;

/**
 * @brief Initializes VBE framebuffer during bootup.
 *
 * utilizes switches to real mode interrupts and therefore must be
 * called during bootup before tick is set up and real-time
 * interrupt vectors utilized
 */
void vesa_realmode_bootup_init(void);

/* flag to limit driver to protect against multiple opens */
static Atomic_Flag driver_mutex;

/* screen information for the VGA driver
 * standard structures - from RTEMS fb interface
 */
static struct fb_var_screeninfo fb_var;
static struct fb_fix_screeninfo fb_fix;

static int32_t vbe_used_mode;

uint32_t VBE_controller_information( VBE_vbe_info_block *info_block,
                                            uint16_t queried_VBE_Version)
{
    uint16_t size;
    VBE_vbe_info_block *VBE_buffer =
        (VBE_vbe_info_block *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetVBEConInf;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    /* indicate to graphic's bios that VBE2.0 extended information is desired */
    if (queried_VBE_Version >= 0x200)
    {
        memcpy(
            &VBE_buffer->VbeSignature,
            VBE20plus_SIGNATURE,
            sizeof(VBE_buffer->VbeSignature)
        );
    }
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    if ((parret.reg_eax & 0xFFFF) ==
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        *info_block = *VBE_buffer;
    }
    return (parret.reg_eax & 0xFFFF);
}

uint32_t VBE_mode_information( VBE_mode_info_block *info_block,
                                    uint16_t mode_number)
{
    uint16_t size;
    VBE_mode_info_block *VBE_buffer =
        (VBE_mode_info_block *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetVBEModInf;
    parret.reg_ecx = mode_number;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    if ((parret.reg_eax & 0xFFFF) ==
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        *info_block = *VBE_buffer;
    }
    return (parret.reg_eax & 0xFFFF);
}

uint32_t VBE_set_mode( uint16_t mode_number,
                            VBE_CRTC_info_block *info_block)
{
    uint16_t size;
    VBE_CRTC_info_block *VBE_buffer =
        (VBE_CRTC_info_block *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    /* copy CRTC */
    *VBE_buffer = *info_block;
    parret.reg_eax = VBE_SetVBEMod;
    parret.reg_ebx = mode_number;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    return (parret.reg_eax & 0xFFFF);
}

uint32_t VBE_current_mode(uint16_t *mode_number)
{
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetCurVBEMod;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    *mode_number = (uint16_t)parret.reg_ebx;
    return (parret.reg_eax & 0xFFFF);
}

uint32_t VBE_report_DDC_capabilities(uint16_t controller_unit_number,
                                        uint8_t *seconds_to_transfer_EDID_block,
                                        uint8_t *DDC_level_supported)
{
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_DisDatCha;
    parret.reg_ebx = VBEDDC_Capabilities;
    parret.reg_ecx = controller_unit_number;
    parret.reg_edi = 0;
    parret.reg_es = 0;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    *seconds_to_transfer_EDID_block = (uint8_t)parret.reg_ebx >> 8;
    *DDC_level_supported = (uint8_t)parret.reg_ebx;
    return (parret.reg_eax & 0xFFFF);
}

uint32_t VBE_read_EDID(uint16_t controller_unit_number,
                            uint16_t EDID_block_number,
                            EDID_edid1 *buffer)
{
    uint16_t size;
    EDID_edid1 *VBE_buffer = (EDID_edid1*)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_DisDatCha;
    parret.reg_ebx = VBEDDC_ReadEDID;
    parret.reg_ecx = controller_unit_number;
    parret.reg_edx = EDID_block_number;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    if ((parret.reg_eax & 0xFFFF) ==
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        *buffer = *VBE_buffer;
    }
    return (parret.reg_eax & 0xFFFF);
}

/**
 * @brief Basic graphic's mode parameters
 */
typedef struct {
    /** number of the graphic's mode */
    uint16_t mode_number;
    /** number of pixels in one line */
    uint16_t resX;
    /** number of lines */
    uint16_t resY;
    /** bits per pixel */
    uint8_t bpp;
} Mode_params;

typedef enum {
    NO_SUITABLE_MODE    = -1,
    BAD_FORMAT          = -2,
    AUTO_SELECT         = -3,
    DONT_INIT           = -4,
    NO_MODE_REQ         = -5,
} mode_err_ret_val;

/**
 * @brief Find mode by resolution in the given list of modes
 *
 * finds mode in \p mode_list of \p list_length length according to resolution
 * given in \p searched_resolution . If bpp is given in that struct as well
 * mode with such color depth and resolution is searched for. Otherwise bpp
 * has to be zero. Mode number found is returned and also filled into
 * \p searched_resolution . bpp is also filled into \p searchedResolution if it
 * was 0 before call.
 *
 * @param[in] mode_list list of modes to be searched
 * @param[in] list_length number of modes in the list
 * @param[in,out] searched_resolution element filled with searched resolution
 *                or/and bpp; mode_number is filled in if appropriate mode found
 * @retval mode number satisfying given parameters
 * @retval -1 no suitable mode found
 */
static int32_t find_mode_by_resolution(Mode_params *mode_list,
                                        uint8_t list_length,
                                        Mode_params *searched_resolution)
{
    uint8_t i = 0;
    while (i < list_length)
    {
        if (searched_resolution->resX == mode_list[i].resX &&
            searched_resolution->resY == mode_list[i].resY)
        {
            if (searched_resolution->bpp==0 ||
                searched_resolution->bpp==mode_list[i].bpp)
            {
                searched_resolution->bpp = mode_list[i].bpp;
                searched_resolution->mode_number = mode_list[i].mode_number;
                return mode_list[i].mode_number;
            }
        }
        i++;
    }
    return NO_SUITABLE_MODE;
}

/**
 * @brief Find mode given in string format.
 *
 *  expected format
 *  <resX>x<resY>[-<bpp>]
 *  numbers <resX>, <resY> and <bpp> are decadic
 *
 * @param[in] mode_list list of modes to be searched
 * @param[in] list_length number of modes in the list
 * @param[in] video_string string to be parsed
 * @retval video mode number to be set
 * @retval -1 no suitable mode found
 * @retval -2 bad format of the video_string
 * @retval -3 automatic mode selection requested
 * @retval -4 request to not initialize graphics
 * @retval -5 no mode requested/empty video string
 */
static int32_t find_mode_from_string(Mode_params *mode_list,
                                      uint8_t list_length,
                                      const char *video_string)
{
    const char* opt;
    Mode_params cmdline_mode;
    char* endptr;
    cmdline_mode.bpp = 16; /* default bpp */
    opt = video_string;
    if (opt)
    {
        if (strncmp(opt, "auto", 4) == 0)
            return AUTO_SELECT;
        if (strncmp(opt, "none", 4) == 0 ||
            strncmp(opt, "off", 3) == 0)
        {
            return DONT_INIT;
        }
        cmdline_mode.resX = strtol(opt, &endptr, 10);
        if (*endptr != 'x')
        {
            return BAD_FORMAT;
        }
        opt = endptr+1;
        cmdline_mode.resY = strtol(opt, &endptr, 10);
        switch (*endptr)
        {
            case '-':
                opt = endptr+1;
                if (strlen(opt) <= 2)
                    cmdline_mode.bpp = strtol(opt, &endptr, 10);
                else
                {
                    cmdline_mode.bpp = strtol(opt, &endptr, 10);
                    if (*endptr != ' ')
                    {
                        return BAD_FORMAT;
                    }
                }
            case ' ':
            case 0:
                break;
            default:
                return BAD_FORMAT;
        }

        return find_mode_by_resolution(mode_list, list_length, &cmdline_mode);
    }
    return NO_MODE_REQ;
}

/**
 * @brief Find mode given within command line.
 *
 * Parse command line option "--video=" if available.
 *  expected format
 *  --video=<resX>x<resY>[-<bpp>]
 *  numbers <resX>, <resY> and <bpp> are decadic
 *
 * @param[in] mode_list list of modes to be searched
 * @param[in] list_length number of modes in the list
 * @retval video mode number to be set
 * @retval -1 no suitable mode found
 * @retval -2 bad format of the video_string
 * @retval -3 automatic mode selection requested
 * @retval -4 request to not initialize graphics
 * @retval -5 no mode requested/empty video string
 */
static int32_t find_mode_using_cmdline(Mode_params *mode_list,
                                        uint8_t list_length)
{
    const char* opt;
    opt = bsp_cmdline_arg("--video=");
    if (opt)
    {
        opt += sizeof("--video=")-1;
        return find_mode_from_string(mode_list, list_length, opt);
    }
    return NO_MODE_REQ;
}

/**
 * @brief Find mode number best fitting to monitor attached
 *
 * @param[in] mode_list list of modes to be searched
 * @param[in] list_length number of modes in the list
 * @retval video mode number to be set
 * @retval -1 on parsing error or when no suitable mode found
 */
static int32_t find_mode_using_EDID( Mode_params *mode_list,
                                      uint8_t list_length)
{
    EDID_edid1 edid;
    uint8_t checksum, iterator;
    uint8_t index, j;
    Mode_params EDIDmode;
    checksum = 0;
    iterator = 0;
    EDIDmode.bpp = 0;
    if (VBE_read_EDID(0, 0, &edid) !=
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        printk(FB_VESA_NAME " Function 15h (read EDID) not supported.\n");
        return -1;
    }
/* version of EDID structure */
    if (edid.Version == 1)
    { /* EDID version 1 */
        while (iterator < sizeof(EDID_edid1))
        {
            checksum += *((uint8_t *)&edid+iterator);
            iterator++;
        }
        if (checksum)
            /* not implemented: try to read EDID again */
            printk(FB_VESA_NAME " EDID v1 checksum failed\n");

        /* try to find Detailed Timing Descriptor (defined in BASE EDID)
           in controller mode list; first should be preffered mode */
        index = 0;
        while (index < 4)
        {
            /* skip if it is monitor descriptor */
            if (edid.dtd_md[index].md.Flag0[0] == 0 &&
                edid.dtd_md[index].md.Flag0[1] == 0 &&
                edid.dtd_md[index].md.Flag1 == 0)
            {
                index++;
                continue;
            }
            EDIDmode.resX = DTD_horizontal_active(&edid.dtd_md[0].dtd);
            EDIDmode.resY = DTD_vertical_active(&edid.dtd_md[0].dtd);
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;

            index++;
        }
        /* try to find Detailed Timing Descriptor (defined in optional EXTENSION
        Blocks) in controller mode list */
        if (edid.ExtensionFlag > 0)
        {
            /* not implemented */
        }
        /* try to find CVT (defined in BASE EDID) in controller mode list */
        index = 1;
        while (index < 4)
        {
            if (edid.dtd_md[index].md.DataTypeTag ==
                    EDID_DTT_CVT3ByteTimingCodes     &&
                edid.dtd_md[index].md.Flag0[0] == 0  &&
                edid.dtd_md[index].md.Flag0[1] == 0  &&
                edid.dtd_md[index].md.Flag1 == 0     &&
                edid.dtd_md[index].md.Flag2 == 0)
            {
                EDID_CVT_timing_codes_3B *cvt = (EDID_CVT_timing_codes_3B *)
                    &edid.dtd_md[index].md.DescriptorData[0];
                j = 0;
                while (j < 4)
                {
                    EDIDmode.resY = edid1_CVT_addressable_lines_high(
                        &cvt->cvt[j]
                    );
                    switch (edid1_CVT_aspect_ratio(&cvt->cvt[j]))
                    {
                        case EDID_CVT_AspectRatio_4_3:
                            EDIDmode.resX = (EDIDmode.resY*4)/3;
                            break;
                        case EDID_CVT_AspectRatio_16_9:
                            EDIDmode.resX = (EDIDmode.resY*16)/9;
                            break;
                        case EDID_CVT_AspectRatio_16_10:
                            EDIDmode.resX = (EDIDmode.resY*16)/10;
                            break;
                        case EDID_CVT_AspectRatio_15_9:
                            EDIDmode.resX = (EDIDmode.resY*15)/9;
                            break;
                    }
                    EDIDmode.resX = (EDIDmode.resX/8)*8;
                    if (find_mode_by_resolution(
                            mode_list, list_length, &EDIDmode) != -1)
                        return EDIDmode.mode_number;

                    j++;
                }
            }
            index++;
        }
        /* try to find CVT (defined in optional EXTENSION Blocks)
        in controller mode list */
        /* not implemented */
        /* try to find Standard Timings (listed in BASE EDID)
        in controller mode list */
        index = 0;
        while (index < 8)
        {
            /* check if descriptor is unused */
            if (edid1_STI_is_unused(&edid.STI[index]))
            {
                index++;
                continue;
            }
            EDIDmode.resX = (edid.STI[index].HorizontalActivePixels+31)*8;
            switch (edid.STI[index].ImageAspectRatio_RefreshRate &
                    EDID1_STI_ImageAspectRatioMask)
            {
                case EDID_STI_AspectRatio_16_10:
                    EDIDmode.resY = (EDIDmode.resX*10)/16;
                    break;
                case EDID_STI_AspectRatio_4_3:
                    EDIDmode.resY = (EDIDmode.resX*3)/4;
                    break;
                case EDID_STI_AspectRatio_5_4:
                    EDIDmode.resY = (EDIDmode.resX*4)/5;
                    break;
                case EDID_STI_AspectRatio_16_9:
                    EDIDmode.resY = (EDIDmode.resX*9)/16;
                    break;
            }
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;

            index++;
        }
        /* try to find Standard Timings (listed in optional EXTENSION Blocks)
        in controller mode list */
        /* not implemented */
        /* use Established Timings */
        if (edid1_established_tim(&edid, EST_1280x1024_75Hz))
        {
            EDIDmode.resX = 1280;
            EDIDmode.resY = 1024;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_1152x870_75Hz))
        {
            EDIDmode.resX = 1152;
            EDIDmode.resY = 870;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_1024x768_75Hz) ||
            edid1_established_tim(&edid, EST_1024x768_70Hz) ||
            edid1_established_tim(&edid, EST_1024x768_60Hz) ||
            edid1_established_tim(&edid, EST_1024x768_87Hz))
        {
            EDIDmode.resX = 1024;
            EDIDmode.resY = 768;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_832x624_75Hz))
        {
            EDIDmode.resX = 832;
            EDIDmode.resY = 624;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_800x600_60Hz) ||
            edid1_established_tim(&edid, EST_800x600_56Hz) ||
            edid1_established_tim(&edid, EST_800x600_75Hz) ||
            edid1_established_tim(&edid, EST_800x600_72Hz))
        {
            EDIDmode.resX = 800;
            EDIDmode.resY = 600;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_720x400_88Hz) ||
            edid1_established_tim(&edid, EST_720x400_70Hz))
        {
            EDIDmode.resX = 720;
            EDIDmode.resY = 400;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
        if (edid1_established_tim(&edid, EST_640x480_75Hz) ||
            edid1_established_tim(&edid, EST_640x480_72Hz) ||
            edid1_established_tim(&edid, EST_640x480_67Hz) ||
            edid1_established_tim(&edid, EST_640x480_60Hz))
        {
            EDIDmode.resX = 640;
            EDIDmode.resY = 480;
            EDIDmode.bpp = 0;
            if (find_mode_by_resolution(mode_list, list_length, &EDIDmode) !=
                -1)
                return EDIDmode.mode_number;
        }
    }
    else
        printk(FB_VESA_NAME " error reading EDID: unsupported version\n");
    return -1;
}

void vesa_realmode_bootup_init(void)
{
    uint32_t vbe_ret_val;
    uint16_t size;
    VBE_vbe_info_block *vib = (VBE_vbe_info_block *)
        i386_get_default_rm_buffer(&size);
    vbe_ret_val = VBE_controller_information(vib, 0x300);
    if (vbe_ret_val == -1)
    {
        printk(FB_VESA_NAME " error calling real mode interrupt.\n");
        return;
    }
    if (vbe_ret_val != (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        printk(FB_VESA_NAME " Function 00h (read VBE info block)"
            "not supported.\n");
    }
/*  Helper array is later filled with mode numbers and their parameters
    sorted from the biggest values to the smalest where priorities of
    parameters are from the highest to the lowest: resolution X,
    resolution Y, bits per pixel.
    The array is used for search the monitor provided parameters in EDID
    structure and if found we set such mode using corresponding
    VESA function. */
#define MAX_NO_OF_SORTED_MODES 100
    Mode_params sorted_mode_params[MAX_NO_OF_SORTED_MODES];

    uint16_t *vmpSegOff = (uint16_t *)&vib->VideoModePtr;
    uint16_t *modeNOPtr = (uint16_t*)
        i386_Real_to_physical(*(vmpSegOff+1), *vmpSegOff);
    uint16_t iterator = 0;

    if (*(uint16_t*)vib->VideoModePtr == VBE_STUB_VideoModeList)
    {
        printk(FB_VESA_NAME " VBE Core not implemented!\n");
    }
    else
    {
        /* prepare list of modes */
        while (*(modeNOPtr+iterator) != VBE_END_OF_VideoModeList &&
            *(modeNOPtr+iterator) != 0)
        { /* some bios implementations ends the list incorrectly with 0 */
            if (iterator < MAX_NO_OF_SORTED_MODES)
            {
                sorted_mode_params[iterator].mode_number =*(modeNOPtr+iterator);
                iterator ++;
            }
            else
                break;
        }
        if (iterator < MAX_NO_OF_SORTED_MODES)
            sorted_mode_params[iterator].mode_number = 0;
    }

    VBE_mode_info_block *mib = (VBE_mode_info_block *)
        i386_get_default_rm_buffer(&size);
    iterator = 0;
    uint8_t nextFilteredMode = 0;
    uint16_t required_mode_attributes = VBE_modSupInHWMask |
        VBE_ColorModeMask | VBE_GraphicsModeMask | VBE_LinFraBufModeAvaiMask;
    /* get parameters of modes and filter modes according to set
        required parameters */
    while (iterator < MAX_NO_OF_SORTED_MODES &&
        sorted_mode_params[iterator].mode_number!=0)
    {
        VBE_mode_information(mib, sorted_mode_params[iterator].mode_number);
        if ((mib->ModeAttributes&required_mode_attributes) ==
            required_mode_attributes)
        {
            sorted_mode_params[nextFilteredMode].mode_number =
                sorted_mode_params[iterator].mode_number;
            sorted_mode_params[nextFilteredMode].resX = mib->XResolution;
            sorted_mode_params[nextFilteredMode].resY = mib->YResolution;
            sorted_mode_params[nextFilteredMode].bpp  = mib->BitsPerPixel;
            nextFilteredMode ++;
        }
        iterator ++;
    }
    sorted_mode_params[nextFilteredMode].mode_number = 0;

    uint8_t number_of_modes = nextFilteredMode;

    /* first search for video argument in multiboot options */
    vbe_used_mode = find_mode_using_cmdline(sorted_mode_params,
                                            number_of_modes);

    if (vbe_used_mode == NO_MODE_REQ) {
        vbe_used_mode = find_mode_from_string(sorted_mode_params,
                            number_of_modes, rtems_fb_default_mode);
        if (vbe_used_mode != NO_MODE_REQ) {
            printk(FB_VESA_NAME " using application option to select"
                " video mode\n");
        }
    }
    else
    {
        printk(FB_VESA_NAME " using command line option '--video='"
            "to select video mode\n");
    }

    switch (vbe_used_mode) {
        case NO_SUITABLE_MODE:
            printk(FB_VESA_NAME " requested mode not found\n");
            return;
        case BAD_FORMAT:
            printk(FB_VESA_NAME " bad format of video requested\n");
            return;
        case DONT_INIT:
            printk(FB_VESA_NAME " selected not to initialize graphics\n");
            return;
        case NO_MODE_REQ:
            printk(FB_VESA_NAME " not initialized, no video selected\n");
            return;
    }

    /* sort filtered modes */
    Mode_params modeXchgPlace;
    iterator = 0;
    uint8_t j;
    uint8_t idxBestMode;
    while (iterator < number_of_modes)
    {
        idxBestMode = iterator;
        j = iterator+1;
        while (j < number_of_modes)
        {
            if (sorted_mode_params[j].resX >
                    sorted_mode_params[idxBestMode].resX)
                idxBestMode = j;
            else if (sorted_mode_params[j].resX ==
                     sorted_mode_params[idxBestMode].resX)
            {
                if (sorted_mode_params[j].resY >
                        sorted_mode_params[idxBestMode].resY)
                    idxBestMode = j;
                else if (sorted_mode_params[j].resY ==
                    sorted_mode_params[idxBestMode].resY)
                {
                    if (sorted_mode_params[j].bpp >
                            sorted_mode_params[idxBestMode].bpp)
                        idxBestMode = j;
                }
            }
            j++;
        }
        if (idxBestMode != iterator)
        {
            modeXchgPlace = sorted_mode_params[iterator];
            sorted_mode_params[iterator] = sorted_mode_params[idxBestMode];
            sorted_mode_params[idxBestMode] = modeXchgPlace;
        }
        iterator++;
    }

    if (vbe_used_mode == AUTO_SELECT)
    {
        printk(FB_VESA_NAME " auto video mode selected"
            "\n\ttrying EDID ...\n");
        /* second search monitor for good resolution */
        vbe_used_mode = find_mode_using_EDID(sorted_mode_params,
                                             number_of_modes);
        if (vbe_used_mode == -1)
        {
            printk(FB_VESA_NAME" monitor's EDID video parameters not supported"
                               "\n\tusing mode with highest resolution, bpp\n");
            /* third set highest values */
            vbe_used_mode = sorted_mode_params[0].mode_number;
        }
    }

    /* fill framebuffer structs with info about selected mode */
    vbe_ret_val = VBE_mode_information(mib, vbe_used_mode);
    if ((vbe_ret_val&0xff)!=VBE_functionSupported ||
        (vbe_ret_val>>8)!=VBE_callSuccessful)
    {
        printk(FB_VESA_NAME " Cannot get mode info anymore. ax=0x%lx\n",
               (uintptr_t) vbe_ret_val);
    }

    fb_var.xres = mib->XResolution;
    fb_var.yres = mib->YResolution;
    fb_var.bits_per_pixel = mib->BitsPerPixel;
    fb_var.red.offset =      mib->LinRedFieldPosition;
    fb_var.red.length =      mib->LinRedMaskSize;
    fb_var.red.msb_right =   0;
    fb_var.green.offset =    mib->LinGreenFieldPosition;
    fb_var.green.length =    mib->LinGreenMaskSize;
    fb_var.green.msb_right = 0;
    fb_var.blue.offset =     mib->LinBlueFieldPosition;
    fb_var.blue.length =     mib->LinBlueMaskSize;
    fb_var.blue.msb_right =  0;
    fb_var.transp.offset =   mib->LinRsvdFieldPosition;
    fb_var.transp.length =   mib->LinRsvdMaskSize;
    fb_var.transp.msb_right =0;

    fb_fix.smem_start  = (char *)mib->PhysBasePtr;
    fb_fix.line_length = mib->LinBytesPerScanLine;
    fb_fix.smem_len    = fb_fix.line_length*fb_var.yres;
    fb_fix.type        = FB_TYPE_PACKED_PIXELS;
    if (fb_var.bits_per_pixel < 24)
        fb_fix.visual  = FB_VISUAL_DIRECTCOLOR;
    else
        fb_fix.visual  = FB_VISUAL_TRUECOLOR;

    /* set selected mode */
    vbe_ret_val = VBE_set_mode(vbe_used_mode | VBE_linearFlatFrameBufMask,
        (VBE_CRTC_info_block *)(i386_get_default_rm_buffer(&size)));
    if (vbe_ret_val>>8 == VBE_callFailed)
        printk(FB_VESA_NAME " VBE: Requested mode is not available.");

    if ((vbe_ret_val&0xff)!= (VBE_functionSupported | VBE_callSuccessful<<8))
        printk(FB_VESA_NAME " Call to function 2h (set VBE mode) failed. "
            "ax=0x%" PRIx32 "\n", vbe_ret_val);

    vib = (void *) 0;
    mib = (void *) 0;
}

/*
 * fb_vesa device driver INITIALIZE entry point.
 */
rtems_device_driver
frame_buffer_initialize(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
    rtems_status_code status;

    printk(FB_VESA_NAME " frame buffer -- driver initializing..\n" );

   /*
    * Register the device.
    */
    status = rtems_io_register_name(FRAMEBUFFER_DEVICE_0_NAME, major, 0);
    if (status != RTEMS_SUCCESSFUL)
    {
        printk("Error registering " FRAMEBUFFER_DEVICE_0_NAME
        " - " FB_VESA_NAME " frame buffer device!\n");
        rtems_fatal_error_occurred( status );
    }

    _Atomic_Flag_clear(&driver_mutex, ATOMIC_ORDER_RELEASE);

    return RTEMS_SUCCESSFUL;
}

/*
 * fb_vesa device driver OPEN entry point
 */
rtems_device_driver
frame_buffer_open(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
    printk( FB_VESA_NAME " open device\n" );

    if (_Atomic_Flag_test_and_set(&driver_mutex, ATOMIC_ORDER_ACQUIRE) != 0 )
    {
        printk( FB_VESA_NAME " could not lock vesa_mutex\n" );

        return RTEMS_UNSATISFIED;
    }

    return RTEMS_SUCCESSFUL;

}

/*
 * fb_vesa device driver CLOSE entry point
 */
rtems_device_driver
frame_buffer_close(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  printk( FB_VESA_NAME " close device\n" );
  _Atomic_Flag_clear(&driver_mutex, ATOMIC_ORDER_RELEASE);
  /* restore previous state.  for VGA this means return to text mode.
   * leave out if graphics hardware has been initialized in
   * frame_buffer_initialize() */

  printk(FB_VESA_NAME ": close called.\n" );
  return RTEMS_SUCCESSFUL;
}

/*
 * fb_vesa device driver READ entry point.
 */
rtems_device_driver
frame_buffer_read(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  printk( FB_VESA_NAME " read device\n" );
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved =
    ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ?
        (fb_fix.smem_len - rw_args->offset) :
        rw_args->count;
  memcpy(rw_args->buffer, (const void *)
    (fb_fix.smem_start + rw_args->offset), rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

/*
 * frame_vesa device driver WRITE entry point.
 */
rtems_device_driver
frame_buffer_write(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  printk( FB_VESA_NAME " write device\n" );
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved =
    ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ?
        (fb_fix.smem_len - rw_args->offset) :
        rw_args->count;
  memcpy( (void *) (fb_fix.smem_start + rw_args->offset),
    rw_args->buffer, rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

static int get_fix_screen_info( struct fb_fix_screeninfo *info )
{
  *info = fb_fix;
  return 0;
}

static int get_var_screen_info( struct fb_var_screeninfo *info )
{
  *info =  fb_var;
  return 0;
}

/*
 * IOCTL entry point -- This method is called to carry
 * all services of this interface.
 */
rtems_device_driver
frame_buffer_control(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  printk( FB_VESA_NAME " ioctl called, cmd=%lx\n", (uintptr_t) args->command  );
  printk("fbxres %d, fbyres %d\n", (int) fb_var.xres, (int) fb_var.yres);
  printk("fbbpp %d\n", (int) fb_var.bits_per_pixel);

  switch (args->command)
  {
  case FBIOGET_FSCREENINFO:
      args->ioctl_return =
        get_fix_screen_info( ( struct fb_fix_screeninfo * ) args->buffer );
      break;
  case FBIOGET_VSCREENINFO:
      args->ioctl_return =
        get_var_screen_info( ( struct fb_var_screeninfo * ) args->buffer );
      break;
  case FBIOPUT_VSCREENINFO:
    /* not implemented yet */
    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  case FBIOGETCMAP:
    /* no palette - truecolor mode */
    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  case FBIOPUTCMAP:
    /* no palette - truecolor mode */
    args->ioctl_return = -1;
    return RTEMS_UNSATISFIED;
  default:
    args->ioctl_return = 0;
    break;
  }
  return RTEMS_SUCCESSFUL;
}
