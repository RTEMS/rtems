/*
 *  FB driver for graphic hardware compatible with VESA Bios Extension
 *  Real mode interface utilized
 *  Tested on real HW.
 *
 *  Copyright (c) 2014 - CTU in Prague
 *                       Jan Doležal ( dolezj21@fel.cvut.cz )
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  The code for rtems_buffer_* functions were greatly
 *  inspired or coppied from:
 *    - RTEMS fb_cirrus.c - Alexandru-Sever Horin (alex.sever.h@gmail.com)
 *
 *  Public sources related:
 *    - VESA BIOS EXTENSION (VBE) Core Function Standard, Ver: 3.0, Sep 16, 1998
 *    - VESA Enhanced Extended Display Identification Data (E-EDID) Standard
 *      Release A, Revision 2, September 25, 2006
 */

/*
 *  Hardware is completely initialized upon boot of the system.
 *  Therefore there is no way to change graphics mode later.
 *
 *  Interrupt 0x10 is used for entering graphics BIOS.
 *
 *  Driver reads parameter from multiboot command line to setup video:
 *  "--video=<resX>x<resY>[-<bpp>]"
 *  If cmdline parameter is not specified an attempt for obtaining
 *  resolution from display attached is made.
 */

#include <bsp.h>

#include <bsp/fb_vesa.h>
#include <bsp/realmode_int.h>

#include <pthread.h>

#include <rtems/libio.h>

#include <rtems/fb.h>
#include <rtems/framebuffer.h>

#include <stdlib.h>

#define FB_VESA_NAME    "FB_VESA_RM"

void vesa_realmode_bootup_init(void);

/* mutex for protection against multiple opens, when called frame_buffer_open */
static pthread_mutex_t vesa_mutex = PTHREAD_MUTEX_INITIALIZER;

/* screen information for the VGA driver
 * standard structures - from RTEMS fb interface
 */
static struct fb_var_screeninfo fb_var;
static struct fb_fix_screeninfo fb_fix;

static uint16_t vbe_usedMode;

inline uint32_t VBEControllerInformation(   struct VBE_VbeInfoBlock *infoBlock,
                                            uint16_t queriedVBEVersion)
{
    uint16_t size;
    struct VBE_VbeInfoBlock *VBE_buffer =
        (struct VBE_VbeInfoBlock *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetVBEConInf;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    /* indicate to graphic's bios that VBE2.0 extended information is desired */
    if (queriedVBEVersion >= 0x200)
    {
        strncpy(
            (char *)&VBE_buffer->VbeSignature,
            VBE20plus_SIGNATURE,
            4*sizeof(size_t)
        );
    }
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    if ((parret.reg_eax & 0xFFFF) ==
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        *infoBlock = *VBE_buffer;
    }
    return (parret.reg_eax & 0xFFFF);
}

inline uint32_t VBEModeInformation( struct VBE_ModeInfoBlock *infoBlock,
                                    uint16_t modeNumber)
{
    uint16_t size;
    struct VBE_ModeInfoBlock *VBE_buffer =
        (struct VBE_ModeInfoBlock *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetVBEModInf;
    parret.reg_ecx = modeNumber;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    if ((parret.reg_eax & 0xFFFF) ==
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        *infoBlock = *VBE_buffer;
    }
    return (parret.reg_eax & 0xFFFF);
}

inline uint32_t VBESetMode( uint16_t modeNumber,
                            struct VBE_CRTCInfoBlock *infoBlock)
{
    uint16_t size;
    struct VBE_CRTCInfoBlock *VBE_buffer =
        (struct VBE_CRTCInfoBlock *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    /* copy CRTC */
    *VBE_buffer = *infoBlock;
    parret.reg_eax = VBE_SetVBEMod;
    parret.reg_ebx = modeNumber;
    uint16_t seg, off;
    i386_Physical_to_real(VBE_buffer, &seg, &off);
    parret.reg_edi = (uint32_t)off;
    parret.reg_es = seg;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    return (parret.reg_eax & 0xFFFF);
}

inline uint32_t VBECurrentMode(uint16_t *modeNumber)
{
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_RetCurVBEMod;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    *modeNumber = (uint16_t)parret.reg_ebx;
    return (parret.reg_eax & 0xFFFF);
}

inline uint32_t VBEReportDDCCapabilities(   uint16_t controllerUnitNumber,
                                            uint8_t *secondsToTransferEDIDBlock,
                                            uint8_t *DDCLevelSupported)
{
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_DisDatCha;
    parret.reg_ebx = VBEDDC_Capabilities;
    parret.reg_ecx = controllerUnitNumber;
    parret.reg_edi = 0;
    parret.reg_es = 0;
    if (i386_real_interrupt_call(INTERRUPT_NO_VIDEO_SERVICES, &parret) == 0)
        return -1;
    *secondsToTransferEDIDBlock = (uint8_t)parret.reg_ebx >> 8;
    *DDCLevelSupported = (uint8_t)parret.reg_ebx;
    return (parret.reg_eax & 0xFFFF);
}

inline uint32_t VBEReadEDID(uint16_t controllerUnitNumber,
                            uint16_t EDIDBlockNumber,
                            struct edid1 *buffer)
{
    uint16_t size;
    struct edid1 *VBE_buffer = (struct edid1 *)i386_get_default_rm_buffer(&size);
    i386_realmode_interrupt_registers parret;
    parret.reg_eax = VBE_DisDatCha;
    parret.reg_ebx = VBEDDC_ReadEDID;
    parret.reg_ecx = controllerUnitNumber;
    parret.reg_edx = EDIDBlockNumber;
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

typedef struct {
    uint16_t modeNumber;
    uint16_t resX;
    uint16_t resY;
    uint8_t bpp;
} modeParams;

/* finds mode in 'modeList' of 'listLength' length according to resolution
    given in 'searchedResolution'. If bpp is given in that struct as well
    mode with such color depth and resolution is searched for. Otherwise bpp
    has to be zero. Mode number found is returned and also filled into
    'searchedResolution'. bpp is also filled into 'searchedResolution' if it
    was 0 before call. */
static uint16_t findModeByResolution(   modeParams *modeList,
                                        uint8_t listLength,
                                        modeParams *searchedResolution)
{
    uint8_t i = 0;
    while (i < listLength)
    {
        if (searchedResolution->resX == modeList[i].resX &&
            searchedResolution->resY == modeList[i].resY)
        {
            if (searchedResolution->bpp==0 ||
                searchedResolution->bpp==modeList[i].bpp)
            {
                searchedResolution->bpp = modeList[i].bpp;
                searchedResolution->modeNumber = modeList[i].modeNumber;
                return modeList[i].modeNumber;
            }
        }
        i++;
    }
    return -1;
}

/*
 * Parse comandline option "--video=" if available.
 *  expected format
 *  --video=<resX>x<resY>[-<bpp>]
 *  numbers <resX>, <resY> and <bpp> are decadic
 *
 * @retval video mode number to be set
 *         -1 on parsing error or when no suitable mode found
 */
static uint16_t findModeUsingCmdline(   modeParams *modeList,
                                        uint8_t listLength)
{
    const char* opt;
    modeParams cmdlineMode;
    char* endptr;
    cmdlineMode.bpp = 0;
    opt = bsp_cmdline_arg("--video=");
    if (opt)
    {
        opt += sizeof("--video=")-1;
        cmdlineMode.resX = strtol(opt, &endptr, 10);
        if (*endptr != 'x')
        {
            return -1;
        }
        opt = endptr+1;
        cmdlineMode.resY = strtol(opt, &endptr, 10);
        switch (*endptr)
        {
            case '-':
                opt = endptr+1;
                if (strlen(opt) <= 2)
                    cmdlineMode.bpp = strtol(opt, &endptr, 10);
                else
                {
                    cmdlineMode.bpp = strtol(opt, &endptr, 10);
                    if (*endptr != ' ')
                    {
                        return -1;
                    }
                }
            case ' ':
            case 0:
                break;
            default:
                return -1;
        }

        if (findModeByResolution(modeList, listLength, &cmdlineMode) !=
            (uint16_t)-1)
            return cmdlineMode.modeNumber;
    }
    return -1;
}

/*
 * returns mode number best fitting to monitor attached
 *
 * @retval video mode number to be set
 *         -1 on parsing error or when no suitable mode found
 */
static uint16_t findModeUsingEDID(  modeParams *modeList,
                                    uint8_t listLength)
{
    struct edid1 edid;
    uint8_t checksum, iterator;
    uint8_t index, j;
    modeParams EDIDmode;
    checksum = 0;
    iterator = 0;
    EDIDmode.bpp = 0;
    if (VBEReadEDID(0, 0, &edid) !=
        (VBE_callSuccessful<<8 | VBE_functionSupported))
    {
        printk(FB_VESA_NAME " Function 15h (read EDID) not supported.\n");
        return -1;
    }
/* version of EDID structure */
    if (edid.Version == 1)
    { /* EDID version 1 */
        while (iterator < sizeof(struct edid1))
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
            EDIDmode.resX = DTD_HorizontalActive(&edid.dtd_md[0].dtd);
            EDIDmode.resY = DTD_VerticalActive(&edid.dtd_md[0].dtd);
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;

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
                struct CVTTimingCodes3B *cvt = (struct CVTTimingCodes3B *)
                    &edid.dtd_md[index].md.DescriptorData[0];
                j = 0;
                while (j < 4)
                {
                    EDIDmode.resY = edid1_CVT_AddressableLinesHigh(&cvt->cvt[j]);
                    switch (edid1_CVT_AspectRatio(&cvt->cvt[j]))
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
                    if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                        (uint16_t)-1)
                        return EDIDmode.modeNumber;

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
            if (*(uint16_t*)&edid.STI[index] == EDID_STI_DescriptorUnused)
            {
                index++;
                continue;
            }
            EDIDmode.resX = (edid.STI[index].HorizontalActivePixels+31)*8;
            switch (edid.STI[index].ImageAspectRatio_RefreshRate & EDID1_STI_ImageAspectRatioMask)
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
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;

            index++;
        }
        /* try to find Standard Timings (listed in optional EXTENSION Blocks)
        in controller mode list */
        /* not implemented */
        /* use Established Timings */
        if (edid1_EstablishedTim(&edid, EST_1280x1024_75Hz))
        {
            EDIDmode.resX = 1280;
            EDIDmode.resY = 1024;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_1152x870_75Hz))
        {
            EDIDmode.resX = 1152;
            EDIDmode.resY = 870;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_1024x768_75Hz) ||
            edid1_EstablishedTim(&edid, EST_1024x768_70Hz) ||
            edid1_EstablishedTim(&edid, EST_1024x768_60Hz) ||
            edid1_EstablishedTim(&edid, EST_1024x768_87Hz))
        {
            EDIDmode.resX = 1024;
            EDIDmode.resY = 768;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_832x624_75Hz))
        {
            EDIDmode.resX = 832;
            EDIDmode.resY = 624;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_800x600_60Hz) ||
            edid1_EstablishedTim(&edid, EST_800x600_56Hz) ||
            edid1_EstablishedTim(&edid, EST_800x600_75Hz) ||
            edid1_EstablishedTim(&edid, EST_800x600_72Hz))
        {
            EDIDmode.resX = 800;
            EDIDmode.resY = 600;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_720x400_88Hz) ||
            edid1_EstablishedTim(&edid, EST_720x400_70Hz))
        {
            EDIDmode.resX = 720;
            EDIDmode.resY = 400;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
        if (edid1_EstablishedTim(&edid, EST_640x480_75Hz) ||
            edid1_EstablishedTim(&edid, EST_640x480_72Hz) ||
            edid1_EstablishedTim(&edid, EST_640x480_67Hz) ||
            edid1_EstablishedTim(&edid, EST_640x480_60Hz))
        {
            EDIDmode.resX = 640;
            EDIDmode.resY = 480;
            EDIDmode.bpp = 0;
            if (findModeByResolution(modeList, listLength, &EDIDmode) !=
                (uint16_t)-1)
                return EDIDmode.modeNumber;
        }
    }
    else
        printk(FB_VESA_NAME " error reading EDID: unsupported version\n");
    return (uint16_t)-1;
}

void vesa_realmode_bootup_init(void)
{
    uint32_t vbe_ret_val;
    uint16_t size;
    struct VBE_VbeInfoBlock *vib = (struct VBE_VbeInfoBlock *)
        i386_get_default_rm_buffer(&size);
    vbe_ret_val = VBEControllerInformation(vib, 0x300);
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
    modeParams sortModeParams[MAX_NO_OF_SORTED_MODES];

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
                sortModeParams[iterator].modeNumber = *(modeNOPtr+iterator);
                iterator ++;
            }
            else
                break;
        }
        if (iterator < MAX_NO_OF_SORTED_MODES)
            sortModeParams[iterator].modeNumber = 0;
    }

    struct VBE_ModeInfoBlock *mib = (struct VBE_ModeInfoBlock *)
        i386_get_default_rm_buffer(&size);
    iterator = 0;
    uint8_t nextFilteredMode = 0;
    uint16_t required_mode_attributes = VBE_modSupInHWMask |
        VBE_ColorModeMask | VBE_GraphicsModeMask | VBE_LinFraBufModeAvaiMask;
    /* get parameters of modes and filter modes according to set
        required parameters */
    while (iterator < MAX_NO_OF_SORTED_MODES &&
        sortModeParams[iterator].modeNumber!=0)
    {
        VBEModeInformation(mib, sortModeParams[iterator].modeNumber);
        if ((mib->ModeAttributes&required_mode_attributes) ==
            required_mode_attributes)
        {
            sortModeParams[nextFilteredMode].modeNumber =
                sortModeParams[iterator].modeNumber;
            sortModeParams[nextFilteredMode].resX = mib->XResolution;
            sortModeParams[nextFilteredMode].resY = mib->YResolution;
            sortModeParams[nextFilteredMode].bpp  = mib->BitsPerPixel;
            nextFilteredMode ++;
        }
        iterator ++;
    }
    sortModeParams[nextFilteredMode].modeNumber = 0;

    uint8_t numberOfModes = nextFilteredMode;
    /* sort filtered modes */
    modeParams modeXchgPlace;
    iterator = 0;
    uint8_t j;
    uint8_t idxBestMode;
    while (iterator < numberOfModes)
    {
        idxBestMode = iterator;
        j = iterator+1;
        while (j < numberOfModes)
        {
            if (sortModeParams[j].resX > sortModeParams[idxBestMode].resX)
                idxBestMode = j;
            else if (sortModeParams[j].resX == sortModeParams[idxBestMode].resX)
            {
                if (sortModeParams[j].resY > sortModeParams[idxBestMode].resY)
                    idxBestMode = j;
                else if (sortModeParams[j].resY ==
                    sortModeParams[idxBestMode].resY)
                {
                    if (sortModeParams[j].bpp > sortModeParams[idxBestMode].bpp)
                        idxBestMode = j;
                }
            }
            j++;
        }
        if (idxBestMode != iterator)
        {
            modeXchgPlace = sortModeParams[iterator];
            sortModeParams[iterator] = sortModeParams[idxBestMode];
            sortModeParams[idxBestMode] = modeXchgPlace;
        }
        iterator++;
    }

    /* first search for video argument in multiboot options */
    vbe_usedMode = findModeUsingCmdline(sortModeParams, numberOfModes);
    if (vbe_usedMode == (uint16_t)-1)
    {
        printk(FB_VESA_NAME " video on command line not provided"
            "\n\ttrying EDID ...\n");
        /* second search monitor for good resolution */
        vbe_usedMode = findModeUsingEDID(sortModeParams, numberOfModes);
        if (vbe_usedMode == (uint16_t)-1)
        {
            printk(FB_VESA_NAME" monitor's EDID video parameters not supported"
                               "\n\tusing mode with highest resolution, bpp\n");
            /* third set highest values */
            vbe_usedMode = sortModeParams[0].modeNumber;
        }
    }

    /* fill framebuffer structs with info about selected mode */
    vbe_ret_val = VBEModeInformation(mib, vbe_usedMode);
    if ((vbe_ret_val&0xff)!=VBE_functionSupported ||
        (vbe_ret_val>>8)!=VBE_callSuccessful)
    {
        printk(FB_VESA_NAME " Cannot get mode info anymore. ax=0x%x\n",
            vbe_ret_val);
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
    vbe_ret_val = VBESetMode(vbe_usedMode | VBE_linearFlatFrameBufMask,
        (struct VBE_CRTCInfoBlock *)(i386_get_default_rm_buffer(&size)));
    if (vbe_ret_val>>8 == VBE_callFailed)
        printk(FB_VESA_NAME " VBE: Requested mode is not available.");

    if ((vbe_ret_val&0xff)!= (VBE_functionSupported | VBE_callSuccessful<<8))
        printk(FB_VESA_NAME " Call to function 2h (set VBE mode) failed. "
            "ax=0x%x\n", vbe_ret_val);

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

    if (pthread_mutex_trylock(&vesa_mutex) != 0)
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
  if (pthread_mutex_unlock(&vesa_mutex) == 0)
  {
      /* restore previous state.  for VGA this means return to text mode.
       * leave out if graphics hardware has been initialized in
       * frame_buffer_initialize() */

      printk(FB_VESA_NAME ": close called.\n" );
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
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
    printk("get_fix_screen_info\n");
  *info = fb_fix;
  return 0;
}

static int get_var_screen_info( struct fb_var_screeninfo *info )
{
    printk("get_var_screen_info\n");
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

  printk( FB_VESA_NAME " ioctl called, cmd=%x\n", args->command  );
    printk("fbxres %d, fbyres %d\n", fb_var.xres, fb_var.yres);
    printk("fbbpp %d\n", fb_var.bits_per_pixel);

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
