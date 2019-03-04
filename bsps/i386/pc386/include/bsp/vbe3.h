/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief VESA Bios Extension definitions.
 *
 * This file contains definitions for constants related to VBE.
 *         More information can be found at
 *     <http://www.vesa.org/vesa-standards/free-standards/>.
 *         VESA public standards may be found at
 *     <http://www.vesa.org/wp-content/uploads/2010/12/thankspublic.htm>.
 */

/*
 * Copyright (C) 2014  Jan Doležal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _VBE_H
#define _VBE_H

#ifndef ASM /* ASM */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/score/basedefs.h>

/*  VESA BIOS EXTENSION (VBE) Core functions Standard
    Version: 3.0 Date: September 16, 1998               */

/** AL == 4Fh: Function is supported \n
 *  AL != 4Fh: Function is not supported */
#define VBE_functionSupported        0x4F
/** AH == 00h: Function call successful */
#define VBE_callSuccessful           0x00
/** AH == 01h: Function call failed */
#define VBE_callFailed               0x01
/** AH == 02h: Function is not supported in the current hardware configuration*/
#define VBE_notSupportedInCurHWConf  0x02
/** AH == 03h: Function call invalid in current video mode */
#define VBE_callInvalid              0x03

/* VBE Mode Numbers */
/** D0-D8 =  Mode number */
#define VBE_modeNumberMask         0x01FF
#define VBE_modeNumberShift          0x00
/** If D8 == 0, this is not a VESA defined VBE mode \n
    If D8 == 1, this is a VESA defined VBE mode */
#define VBE_VESAmodeMask           0x0100
#define VBE_VESAmodeShift            0x08
/* D9-D10 =  Reserved by VESA for future expansion (= 0) */
/** If D11 == 0, Use current BIOS default refresh rate \n
    If D11 == 1, Use user specified CRTC values for refresh rate */
#define VBE_refreshRateCtrlMask    0x0800
#define VBE_refreshRateCtrlShift     0x0B
/* D12-13 = Reserved for VBE/AF (must be 0) */
/** If D14 == 0, Use Banked/Windowed Frame Buffer \n
    If D14 == 1, Use Linear/Flat Frame Buffer */
#define VBE_linearFlatFrameBufMask 0x4000
#define VBE_linearFlatFrameBufShift  0x0E
/** If D15 == 0, Clear display memory \n
    If D15 == 1, Preserve display memory */
#define VBE_preserveDispMemMask    0x8000
#define VBE_preserveDispMemShift     0x0F

/* Graphics modes */
/** 15-bit mode, Resolution: 640x400,   Colors: 256  */
#define VBE_R640x400C256      0x100
/** 15-bit mode, Resolution: 640x480,   Colors: 256  */
#define VBE_R640x480C256      0x101
/** 15-bit mode, Resolution: 800x600,   Colors: 16   */
#define VBE_R800x600C16       0x102
/** 7-bit mode,  Resolution: 800x600,   Colors: 16   */
#define VBE_b7R800x600C16     0x6A
/** 15-bit mode, Resolution: 800x600,   Colors: 256  */
#define VBE_R800x600C256      0x103
/** 15-bit mode, Resolution: 1024x768,  Colors: 16   */
#define VBE_R1024x768C16      0x104
/** 15-bit mode, Resolution: 1024x768,  Colors: 256  */
#define VBE_R1024x768C256     0x105
/** 15-bit mode, Resolution: 1280x1024, Colors: 16   */
#define VBE_R1280x1024C16     0x106
/** 15-bit mode, Resolution: 1280x1024, Colors: 256  */
#define VBE_R1280x1024C256    0x107
/** 15-bit mode, Resolution: 320x200,   Colors: 32K   (1:5:5:5) */
#define VBE_R320x200C32K      0x10D
/** 15-bit mode, Resolution: 320x200,   Colors: 64K   (5:6:5)   */
#define VBE_R320x200C64K      0x10E
/** 15-bit mode, Resolution: 320x200,   Colors: 16.8M (8:8:8)   */
#define VBE_R320x200C17M      0x10F
/** 15-bit mode, Resolution: 640x480,   Colors: 32K   (1:5:5:5) */
#define VBE_R640x480C32K      0x110
/** 15-bit mode, Resolution: 640x480,   Colors: 64K   (5:6:5)   */
#define VBE_R640x480C64K      0x111
/** 15-bit mode, Resolution: 640x480,   Colors: 16.8M (8:8:8)   */
#define VBE_R640x480C17M      0x112
/** 15-bit mode, Resolution: 800x600,   Colors: 32K   (1:5:5:5) */
#define VBE_R800x600C32K      0x113
/** 15-bit mode, Resolution: 800x600,   Colors: 64K   (5:6:5)   */
#define VBE_R800x600C64K      0x114
/** 15-bit mode, Resolution: 800x600,   Colors: 16.8M (8:8:8)   */
#define VBE_R800x600C17M      0x115
/** 15-bit mode, Resolution: 1024x768,  Colors: 32K   (1:5:5:5) */
#define VBE_R1024x768C32K     0x116
/** 15-bit mode, Resolution: 1024x768,  Colors: 64K   (5:6:5)   */
#define VBE_R1024x768C64K     0x117
/** 15-bit mode, Resolution: 1024x768,  Colors: 16.8M (8:8:8)   */
#define VBE_R1024x768C17M     0x118
/** 15-bit mode, Resolution: 1280x1024, Colors: 32K   (1:5:5:5) */
#define VBE_R1280x1024C32K    0x119
/** 15-bit mode, Resolution: 1280x1024, Colors: 64K   (5:6:5)   */
#define VBE_R1280x1024C64K    0x11A
/** 15-bit mode, Resolution: 1280x1024, Colors: 16.8M (8:8:8)   */
#define VBE_R1280x1024C17M    0x11B
#define VBE_SpecialMode       0x81FF

/* Text modes */
#define VBE_C80R60            0x108 /**< 15-bit mode, Columns: 80,  Rows: 60 */
#define VBE_C132R25           0x109 /**< 15-bit mode, Columns: 132, Rows: 25 */
#define VBE_C132R43           0x10A /**< 15-bit mode, Columns: 132, Rows: 43 */
#define VBE_C132R50           0x10B /**< 15-bit mode, Columns: 132, Rows: 50 */
#define VBE_C132R60           0x10C /**< 15-bit mode, Columns: 132, Rows: 60 */

/* VBE function numbers - passed in AX register */
/** VBE function - Return VBE Controller Information */
#define VBE_RetVBEConInf       0x4F00
/** VBE function - Return VBE Mode Information */
#define VBE_RetVBEModInf       0x4F01
/** VBE function - Set VBE Mode */
#define VBE_SetVBEMod          0x4F02
/** VBE function - Return Current VBE Mode */
#define VBE_RetCurVBEMod       0x4F03
/** VBE function - Save/Restore State */
#define VBE_SavResSta          0x4F04
/** VBE function - Display Window Control */
#define VBE_DisWinCon          0x4F05
/** VBE function - Set/Get Logical Scan Line Length */
#define VBE_SetGetLogScaLinLen 0x4F06
/** VBE function - Set/Get Display Start */
#define VBE_SetGetDisSta       0x4F07
/** VBE function - Set/Get DAC Palette Format */
#define VBE_SetGetDACPalFor    0x4F08
/** VBE function - Set/Get Palette Data */
#define VBE_SetGetPalDat       0x4F09
/** VBE function - Return VBE Protected Mode Interface */
#define VBE_RetVBEProModInt    0x4F0A
/** VBE function - Get/Set pixel clock */
#define VBE_GetSetpixclo       0x4F0B
/** VBE function - Power Management Extensions (PM) */
#define VBE_PowManExt          0x4F10
/** VBE function - Flat Panel Interface Extensions (FP) */
#define VBE_FlaPanIntExt       0x4F11
/** VBE function - Audio Interface Extensions (AI) */
#define VBE_AudIntExt          0x4F13
/** VBE function - OEM Extensions */
#define VBE_OEMExt             0x4F14
/** VBE function - Display Data Channel (DDC), Serial Control Interface (SCI) */
#define VBE_DisDatCha          0x4F15

/* VBE subfunction numbers - passed in BL register */
#define VBE_RetVBESupSpeInf    0x00  /**< Return VBE Supplemental
                                      * Specification Information */
/* *** Structures *** */
/**
 * @brief Far pointer as defined by VBE standard.
 */
typedef struct {
    /** @brief Offset to segment described by \a selector. */
    uint16_t offset;
    /** @brief Selector or Segment depending on whether this is used from 16bit
        protected mode or from real mode. */
    uint16_t selector;
} RTEMS_PACKED VBE_far_pointer;

/**
 * @brief Protected mode info block as defined by VBE standard.
 */
typedef struct {
    /** PM Info Block Signature */
    uint8_t   Signature[4];
    /** Offset of PM entry point within BIOS */
    uint16_t  EntryPoint;
    /** Offset of PM initialization entry point */
    uint16_t  PMInitialize;
    /** Selector to BIOS data area emulation block */
    uint16_t  BIOSDataSel;
    /** Selector to access A0000h physical memmory */
    uint16_t  A0000Sel;
    /** Selector to access B0000h physical memmory */
    uint16_t  B0000Sel;
    /** Selector to access B8000h physical memmory */
    uint16_t  B8000Sel;
    /** Selector to access code segment as data */
    uint16_t  CodeSegSel;
    /** Set to 1 when in protected mode */
    uint8_t   InProtectMode;
    /** Checksum byte for structure. Sum over all structure bytes gives 0. */
    uint8_t   Checksum;
} RTEMS_PACKED VBE_protected_mode_info_block;

/** General VBE signature */
#define VBE_SIGNATURE "VESA"
/** Signature for VBE 2.0 and higher */
#define VBE20plus_SIGNATURE "VBE2"
/** for STUB see VBE CORE FUNCTIONS VERSION 3.0 - Appendix 1 */
#define VBE_STUB_VideoModeList 0xFFFF
#define VBE_END_OF_VideoModeList 0xFFFF
/**
 * @brief Information about VBE implementation.
 */
typedef struct {
    /** VBE Signature */
    uint8_t   VbeSignature[4];
    /** VBE Version */
    uint16_t  VbeVersion;
    /** VBE_far_pointer to OEM String */
    uint8_t  *OemStringPtr;
    /** Capabilities of graphics controller */
    uint8_t   Capabilities[4];
    /** VBE_far_pointer to VideoModeList */
    uint32_t *VideoModePtr;
    /** Number of 64kb memory blocks    */
    uint16_t  TotalMemory;
    /*  Added for VBE 2.0+ */
    /** VBE implementation Software revision */
    uint16_t  OemSoftwareRev;
    /** VBE_far_pointer to Vendor Name String */
    uint8_t  *OemVendorNamePtr;
    /** VBE_far_pointer to Product Name String */
    uint8_t  *OemProductNamePtr;
    /** VBE_far_pointer to Product Revision String */
    uint8_t  *OemProductRevPtr;
    /** Reserved for VBE implementation scratch */
    uint8_t   Reserved[222];
    /** Data Area for OEM Strings */
    uint8_t   OemData[256];
} RTEMS_PACKED VBE_vbe_info_block;

/**
 * @brief Describes graphic's mode parameter.
 */
typedef struct {
    /*  Mandatory information for all VBE revisions */
    /** mode attributes */
    uint16_t  ModeAttributes;
    /** window A attributes */
    uint8_t   WinAAttributes;
    /** window B attributes */
    uint8_t   WinBAttributes;
    /** window granularity */
    uint16_t  WinGranularity;
    /** window size */
    uint16_t  WinSize;
    /** window A start segment */
    uint16_t  WinASegment;
    /** window B start segment */
    uint16_t  WinBSegment;
    /** real mode pointer to window function */
    uint32_t *WinFuncPtr;
    /** bytes per scan line */
    uint16_t  BytesPerScanLine;
    /*  Mandatory information for VBE 1.2 and above */
    /** horizontal resolution in px or chars */
    uint16_t  XResolution;
    /** vertical resolution in px or chars */
    uint16_t  YResolution;
    /** character cell width in pixels */
    uint8_t   XCharSize;
    /** character cell height in pixels */
    uint8_t   YCharSize;
    /** number of memory planes */
    uint8_t   NumberOfPlanes;
    /** bits per pixel */
    uint8_t   BitsPerPixel;
    /** number of banks */
    uint8_t   NumberOfBanks;
    /** memory model type */
    uint8_t   MemoryModel;
    /** bank size in KB */
    uint8_t   BankSize;
    /** number of images */
    uint8_t   NumberOfImagePages;
    /** reserved for page function */
    uint8_t   Reserved0;
    /*  Direct Color fields (required for direct/6 and YUV/7 memory models) */
    /** size of direct color red mask in bits */
    uint8_t   RedMaskSize;
    /** bit position of lsb of red mask */
    uint8_t   RedFieldPosition;
    /** size of direct color green mask in b */
    uint8_t   GreenMaskSize;
    /** bit position of lsb of green mask */
    uint8_t   GreenFieldPosition;
    /** size of direct color blue mask in b */
    uint8_t   BlueMaskSize;
    /** bit position of lsb of blue mask */
    uint8_t   BlueFieldPosition;
    /** size of direct color reserved mask */
    uint8_t   RsvdMaskSize;
    /** bit position of lsb of reserved mask */
    uint8_t   RsvdFieldPosition;
    /** direct color mode attributes */
    uint8_t   DirectColorModeInfo;
    /*  Mandatory information for VBE 2.0 and above */
    /** physical address for flat memory frame buffer */
    uint32_t *PhysBasePtr;
    /** Reserved - always set to 0 */
    uint32_t  Reserved1;
    /** Reserved - always set to 0 */
    uint16_t  Reserved2;
    /*  Mandatory information for VBE 3.0 and above */
    /** bytes per scan line for linear modes */
    uint16_t  LinBytesPerScanLine;
    /** number of images for banked modes */
    uint8_t   BnkNumberOfImagePages;
    /** number of images for linear modes */
    uint8_t   LinNumberOfImagePages;
        /* linear modes */
    /** size of direct color red mask */
    uint8_t   LinRedMaskSize;
    /** bit position of lsb of red mask */
    uint8_t   LinRedFieldPosition;
    /** size of direct color green mask  */
    uint8_t   LinGreenMaskSize;
    /** bit position of lsb of green mask */
    uint8_t   LinGreenFieldPosition;
    /** size of direct color blue mask  */
    uint8_t   LinBlueMaskSize;
    /** bit position of lsb of blue mask */
    uint8_t   LinBlueFieldPosition;
    /** size of direct color reserved mask */
    uint8_t   LinRsvdMaskSize;
    /** bit position of lsb of reserved mask */
    uint8_t   LinRsvdFieldPosition;
    /** maximum pixel clock (in Hz) for graphics mode */
    uint32_t  MaxPixelClock;
    /** remainder of VBE_mode_info_block */
    uint8_t   Reserved3[189];
} RTEMS_PACKED VBE_mode_info_block;

/**
 * @brief Describes monitor synchronization.
 */
typedef struct {
    /** Horizontal total in pixels */
    uint16_t  HorizontalTotal;
    /** Horizontal sync start in pixels */
    uint16_t  HorizontalSyncStart;
    /** Horizontal sync end in pixels */
    uint16_t  HorizontalSyncEnd;
    /** Vertical total in lines */
    uint16_t  VerticalTotal;
    /** Vertical sync start in lines */
    uint16_t  VerticalSyncStart;
    /** Vertical sync end in lines */
    uint16_t  VerticalSyncEnd;
    /** Flags (Interlaced, Double Scan etc) */
    uint8_t   Flags;
    /** Pixel clock in units of Hz */
    uint32_t  PixelClock;
    /** Refresh rate in units of 0.01 Hz */
    uint16_t  RefreshRate;
    /** remainder of VBE_mode_info_block */
    uint8_t   Reserved[40];
} RTEMS_PACKED VBE_CRTC_info_block;

/**
 * @brief Describes palette entry.
 */
typedef struct {
    /** Blue channel value (6 or 8 bits) */
    uint8_t   Blue;
    /** Green channel value (6 or 8 bits) */
    uint8_t   Green;
    /** Red channel value(6 or 8 bits) */
    uint8_t   Red;
    /** DWORD alignment byte (unused) */
    uint8_t   Alignment;
} RTEMS_PACKED VBE_palette_entry;

/**
 * @brief Supplemental VBE info block.
 */
typedef struct {
    /** Supplemental VBE Signature */
    uint8_t   SupVbeSignature[7];
    /** Supplemental VBE Version */
    uint16_t  SupVbeVersion;
    /** Bitfield of supported subfunctions */
    uint8_t   SupVbeSubFunc[8];
    /** OEM Software revision */
    uint16_t  OemSoftwareRev;
    /** VBE_far_pointer to Vendor Name String */
    uint8_t  *OemVendorNamePtr;
    /** VBE_far_pointer to Product Name String */
    uint8_t  *OemProductNamePtr;
    /** VBE_far_pointer to Product Revision String */
    uint8_t  *OemProductRevPtr;
    /** VBE_far_pointer to OEM String */
    uint8_t  *OemStringPtr;
    /** Reserved for description strings and future expansion */
    uint8_t   Reserved[221];
} RTEMS_PACKED VBE_supplemental_vbe_info_block;

/* VBE_vbe_info_block Capabilities */
/** VBE Info Block - Capabilities\n
    D0 = 0  DAC is fixed width, with 6 bits per primary color \n
    D0 = 1 DAC width is switchable to 8 bits per primary color */
#define VBE_DACswitchableMask     0x0001
/** VBE Info Block - Capabilities\n
    D1 = 0 Controller is VGA compatible \n
    D1 = 1 Controller is not VGA compatible */
#define VBE_notVGAcompatibleMask  0x0002
/** VBE Info Block - Capabilities\n
    D2 = 0 Normal RAMDAC operation \n
    D2 = 1 When programming large blocks of information to the RAMDAC,
            use the blank bit in Function 09h. */
#define VBE_specialRAMDACopMask   0x0004
/** VBE Info Block - Capabilities\n
    D3 = 0 No hardware stereoscopic signaling support \n
    D3 = 1 Hardware stereoscopic signaling supported by controller */
#define VBE_hwStereoscopicMask    0x0008
/** VBE Info Block - Capabilities\n
    D4 = 0 Stereo signaling supported via external VESA stereo connector \n
    D4 = 1 Stereo signaling supported via VESA EVC connector */
#define VBE_supportEVCconnMask    0x0010
/*  D5-31 = Reserved */

/* VBE_mode_info_block ModeAttributes */
/** Mode Info Block - Mode Attributes\n
    D0 =  Mode supported by hardware configuration.
     0 =  Mode not supported in hardware\n
     1 =  Mode supported in hardware */
#define VBE_modSupInHWMask        0x0001
/*  D1 =  1 (Reserved) */
/** Mode Info Block - Mode Attributes\n
    D2 =  TTY Output functions supported by BIOS.
     0 =  TTY Output functions not supported by BIOS\n
     1 =  TTY Output functions supported by BIOS */
#define VBE_TTYOutSupByBIOSMask   0x0004
/** Mode Info Block - Mode Attributes\n
    D3 =  Monochrome/color mode (see note below).
     0 =  Monochrome mode\n
     1 =  Color mode */
#define VBE_ColorModeMask         0x0008
/** Mode Info Block - Mode Attributes\n
    D4 =  Mode type.
     0 =  Text mode\n
     1 =  Graphics mode */
#define VBE_GraphicsModeMask      0x0010
/** Mode Info Block - Mode Attributes\n
    D5 =  VGA compatible mode.
     0 =  Yes\n
     1 =  No */
#define VBE_VGACompModeMask       0x0020
/** Mode Info Block - Mode Attributes\n
    D6 =  VGA compatible windowed memory mode is available.
     0 =  Yes\n
     1 =  No */
#define VBE_VGACompWinMemModeMask 0x0040
/** Mode Info Block - Mode Attributes\n
    D7 =  Linear frame buffer mode is available.
     0 =  No\n
     1 =  Yes */
#define VBE_LinFraBufModeAvaiMask 0x0080
/** Mode Info Block - Mode Attributes\n
    D8 =  Double scan mode is available.
     0 =  No\n
     1 =  Yes */
#define VBE_DblScnModeAvaiMask    0x0100
/** Mode Info Block - Mode Attributes\n
    D9 =  Interlaced mode is available.
     0 =  No\n
     1 =  Yes */
#define VBE_InterlModeAvaiMask    0x0200
/** Mode Info Block - Mode Attributes\n
    D10 =  Hardware triple buffering support.
     0 =  No\n
     1 =  Yes */
#define VBE_HWTripBufSupMask      0x0400
/** Mode Info Block - Mode Attributes\n
    D11 =  Hardware stereoscopic display support.
     0 =  No\n
     1 =  Yes */
#define VBE_HWSterDispSupMask     0x0800
/** Mode Info Block - Mode Attributes\n
    D12 =  Dual display start address support.
     0 =  No\n
     1 =  Yes */
#define VBE_DualDispStAdrSupMask  0x1000
/*  D13-D15 =  Reserved */

/* VBE_mode_info_block WinXAttributes */
/** D0 =  Relocatable window(s) supported.
     0 =  Single non-relocatable window only\n
     1 =  Relocatable window(s) are supported */
#define VBE_RelocWinSupMask       0x01
/** D1 =  Window readable.
     0 =  Window is not readable\n
     1 =  Window is readable */
#define VBE_WinReadableMask       0x02
/** D2 =  Window writeable.
     0 =  Window is not writeable\n
     1 =  Window is writeable */
#define VBE_WinWritableMask       0x04
/*  D3-D7 =  Reserved */

/* VBE_mode_info_block MemoryModel */
#define VBE_TextMode              0x00
#define VBE_CGAGraphics           0x01
#define VBE_HerculesGraphics      0x02
#define VBE_Planar                0x03
#define VBE_PackedPixel           0x04
#define VBE_NonChain4Color256     0x05
#define VBE_DirectColor           0x06
#define VBE_YUV                   0x07
    /* 0x08-0x0F Reserved, to be defined by VESA */
    /* 0x10-0xFF To be defined by OEM */

/* VBE_mode_info_block DirectColorModeInfo */
/** D0 =  Color ramp is fixed/programmable.
     0 =  Color ramp is fixed\n
     1 =  Color ramp is programmable */
#define VBE_ColRampProgMask       0x01
/** D1 =  Bits in Rsvd field are usable/reserved.
     0 =  Bits in Rsvd field are reserved\n
     1 =  Bits in Rsvd field are usable by the application */
#define VBE_RsvdBitsUsableMask    0x02

/* VBE_CRTC_info_block Flags */
/** CRTC Info Block - Flags\n
    D0 =  Double Scan Mode Enable.
     0 =  Graphics mode is not double scanned\n
     1 =  Graphics mode is double scanned */
#define VBE_GrModeDblScanMask     0x01
/** CRTC Info Block - Flags\n
    D1 =  Interlaced Mode Enable.
     0 =  Graphics mode is non-interlaced\n
     1 =  Graphics mode is interlaced */
#define VBE_GrModeInterlMask      0x02
/** CRTC Info Block - Flags\n
    D2 =  Horizontal sync polarity.
     0 =  Horizontal sync polarity is positive (+)\n
     1 =  Horizontal sync polarity is negative (-) */
#define VBE_HorSncPolNegMask      0x04
/** CRTC Info Block - Flags\n
    D3 =  Vertical sync polarity.
     0 =  Vertical sync polarity is positive (+)\n
     1 =  Vertical sync polarity is negative (-) */
#define VBE_VerSncPolNegMask      0x08


/*  VESA BIOS Extensions/Display Data Channel Standard
    Version: 1.1 November 18, 1999                      */

/* VBE/DDC subfunction numbers - passed in BL register */
/** VBE/DDC subfunction - Report VBE/DDC Capabilities */
#define VBEDDC_Capabilities    0x0
/** VBE/DDC subfunction - Read EDID */
#define VBEDDC_ReadEDID        0x1

/* DDC Capabilities */
/* DDC level supported - returned in BL register */
/** 0 - DDC1 not supported; 1 - DDC1 supported */
#define VBEDDC_1SupportedMask  0x1
/** 0 - DDC2 not supported; 1 - DDC2 supported */
#define VBEDDC_2SupportedMask  0x2
/** 0 - Screen not blanked during data transfer\n
    1 - Screen blanked during data transfer */
#define VBEDDC_scrBlnkDatTrMs  0x4


/*  VESA BIOS Extensions/Serial Control Interface Standard
    Version: 1.0 Revision: 2 Date: July 2, 1997         */

/* VBE/SCI subfunction numbers - passed in BL register */
/** VBE/SCI subfunction - Report VBE/SCI Capabilities */
#define VBESCI_ReportCapabil   0x10
/** VBE/SCI subfunction - Begin SCL/SDA control */
#define VBESCI_BegSCLSDACtrl   0x11
/** VBE/SCI subfunction - End SCL/SDA control */
#define VBESCI_EndSCLSDACtrl   0x12
/** VBE/SCI subfunction - Write SCL clock line */
#define VBESCI_WrtSCLClkLine   0x13
/** VBE/SCI subfunction - Write SDA data line */
#define VBESCI_WrtSDADatLine   0x14
/** VBE/SCI subfunction - Read SCL clock line */
#define VBESCI_RdySCLClkLine   0x15
/** VBE/SCI subfunction - Read SDA data line */
#define VBESCI_RdySDADatLine   0x16

/* SCI Capabilities */
/* I2C level supported - returned in BL register */
/** Can write to SCL clock line */
#define VBESCI_capSCLwrtMask   0x1
/** Can write to SDA data line */
#define VBESCI_capSDAwrtMask   0x2
/** Can read from SCL clock line */
#define VBESCI_capSCLrdyMask   0x4
/** Can read from SDA data line */
#define VBESCI_capSDArdyMask   0x8


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _VBE_H */
