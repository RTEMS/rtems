/**
 * @file vbe3.h
 *
 * @ingroup i386_pc386
 *
 * @brief VESA Bios Extension definitions.
 */

/*
 * vbe3.h  - This file contains definitions for constants related to VBE.
 *          More information can be found at
 *      <http://www.vesa.org/vesa-standards/free-standards/>
 *          VESA public standards may be found at
 *      <http://www.vesa.org/wp-content/uploads/2010/12/thankspublic.htm>
 *
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

/* VBE RETURN STATUS */
/* AL == 4Fh: Function is supported */
/* AL != 4Fh: Function is not supported */
#define VBE_functionSupported        0x4F
/* AH == 00h: Function call successful */
#define VBE_callSuccessful           0x00
/* AH == 01h: Function call failed */
#define VBE_callFailed               0x01
/* AH == 02h: Function is not supported in the current hardware configuration */
#define VBE_notSupportedInCurHWConf  0x02
/* AH == 03h: Function call invalid in current video mode */
#define VBE_callInvalid              0x03

/* VBE Mode Numbers */
/* D0-D8 =  Mode number */
#define VBE_modeNumberMask         0x01FF
#define VBE_modeNumberShift          0x00
/* If D8 == 0, this is not a VESA defined VBE mode */
#define VBE_VESAmodeMask           0x0100
/* If D8 == 1, this is a VESA defined VBE mode */
/* D9-D12 =  Reserved by VESA for future expansion (= 0) */
#define VBE_VESAmodeShift            0x08
/* If D11 == 0, Use current BIOS default refresh rate */
#define VBE_refreshRateCtrlMask    0x0800
/* If D11 == 1, Use user specified CRTC values for refresh rate */
/* D12-13 = Reserved for VBE/AF (must be 0) */
#define VBE_refreshRateCtrlShift     0x0B
/* If D14 == 0, Use Banked/Windowed Frame Buffer */
#define VBE_linearFlatFrameBufMask 0x4000
/* If D14 == 1, Use Linear/Flat Frame Buffer */
#define VBE_linearFlatFrameBufShift  0x0E
/* If D15 == 0, Clear display memory */
#define VBE_preserveDispMemMask    0x8000
/* If D15 == 1, Preserve display memory */
#define VBE_preserveDispMemShift     0x0F

/* Graphics modes */
/* 15-bit mode, Resolution: 640x400,   Colors: 256  */
#define VBE_R640x400C256      0x100
/* 15-bit mode, Resolution: 640x480,   Colors: 256  */
#define VBE_R640x480C256      0x101
/* 15-bit mode, Resolution: 800x600,   Colors: 16   */
#define VBE_R800x600C16       0x102
/* 7-bit mode,  Resolution: 800x600,   Colors: 16   */
#define VBE_b7R800x600C16     0x6A
/* 15-bit mode, Resolution: 800x600,   Colors: 256  */
#define VBE_R800x600C256      0x103
/* 15-bit mode, Resolution: 1024x768,  Colors: 16   */
#define VBE_R1024x768C16      0x104
/* 15-bit mode, Resolution: 1024x768,  Colors: 256  */
#define VBE_R1024x768C256     0x105
/* 15-bit mode, Resolution: 1280x1024, Colors: 16   */
#define VBE_R1280x1024C16     0x106
/* 15-bit mode, Resolution: 1280x1024, Colors: 256  */
#define VBE_R1280x1024C256    0x107
/* 15-bit mode, Resolution: 320x200,   Colors: 32K   (1:5:5:5) */
#define VBE_R320x200C32K      0x10D
/* 15-bit mode, Resolution: 320x200,   Colors: 64K   (5:6:5)   */
#define VBE_R320x200C64K      0x10E
/* 15-bit mode, Resolution: 320x200,   Colors: 16.8M (8:8:8)   */
#define VBE_R320x200C17M      0x10F
/* 15-bit mode, Resolution: 640x480,   Colors: 32K   (1:5:5:5) */
#define VBE_R640x480C32K      0x110
/* 15-bit mode, Resolution: 640x480,   Colors: 64K   (5:6:5)   */
#define VBE_R640x480C64K      0x111
/* 15-bit mode, Resolution: 640x480,   Colors: 16.8M (8:8:8)   */
#define VBE_R640x480C17M      0x112
/* 15-bit mode, Resolution: 800x600,   Colors: 32K   (1:5:5:5) */
#define VBE_R800x600C32K      0x113
/* 15-bit mode, Resolution: 800x600,   Colors: 64K   (5:6:5)   */
#define VBE_R800x600C64K      0x114
/* 15-bit mode, Resolution: 800x600,   Colors: 16.8M (8:8:8)   */
#define VBE_R800x600C17M      0x115
/* 15-bit mode, Resolution: 1024x768,  Colors: 32K   (1:5:5:5) */
#define VBE_R1024x768C32K     0x116
/* 15-bit mode, Resolution: 1024x768,  Colors: 64K   (5:6:5)   */
#define VBE_R1024x768C64K     0x117
/* 15-bit mode, Resolution: 1024x768,  Colors: 16.8M (8:8:8)   */
#define VBE_R1024x768C17M     0x118
/* 15-bit mode, Resolution: 1280x1024, Colors: 32K   (1:5:5:5) */
#define VBE_R1280x1024C32K    0x119
/* 15-bit mode, Resolution: 1280x1024, Colors: 64K   (5:6:5)   */
#define VBE_R1280x1024C64K    0x11A
/* 15-bit mode, Resolution: 1280x1024, Colors: 16.8M (8:8:8)   */
#define VBE_R1280x1024C17M    0x11B
#define VBE_SpecialMode       0x81FF

/* Text modes */
#define VBE_C80R60            0x108   /* 15-bit mode, Columns: 80,  Rows: 60 */
#define VBE_C132R25           0x109   /* 15-bit mode, Columns: 132, Rows: 25 */
#define VBE_C132R43           0x10A   /* 15-bit mode, Columns: 132, Rows: 43 */
#define VBE_C132R50           0x10B   /* 15-bit mode, Columns: 132, Rows: 50 */
#define VBE_C132R60           0x10C   /* 15-bit mode, Columns: 132, Rows: 60 */

/* VBE function numbers - passed in AX register */
#define VBE_RetVBEConInf       0x4F00 /* Return VBE Controller Information */
#define VBE_RetVBEModInf       0x4F01 /* Return VBE Mode Information */
#define VBE_SetVBEMod          0x4F02 /* Set VBE Mode */
#define VBE_RetCurVBEMod       0x4F03 /* Return Current VBE Mode */
#define VBE_SavResSta          0x4F04 /* Save/Restore State */
#define VBE_DisWinCon          0x4F05 /* Display Window Control */
#define VBE_SetGetLogScaLinLen 0x4F06 /* Set/Get Logical Scan Line Length */
#define VBE_SetGetDisSta       0x4F07 /* Set/Get Display Start */
#define VBE_SetGetDACPalFor    0x4F08 /* Set/Get DAC Palette Format */
#define VBE_SetGetPalDat       0x4F09 /* Set/Get Palette Data */
#define VBE_RetVBEProModInt    0x4F0A /* Return VBE Protected Mode Interface */
#define VBE_GetSetpixclo       0x4F0B /* Get/Set pixel clock */
#define VBE_PowManExt          0x4F10 /* Power Management Extensions (PM) */
#define VBE_FlaPanIntExt       0x4F11 /* Flat Panel Interface Extensions (FP) */
#define VBE_AudIntExt          0x4F13 /* Audio Interface Extensions (AI) */
#define VBE_OEMExt             0x4F14 /* OEM Extensions */
#define VBE_DisDatCha          0x4F15 /* Display Data Channel (DDC),
                                         Serial Control Interface (SCI) */

/* VBE subfunction numbers - passed in BL register */
#define VBE_RetVBESupSpeInf    0x00  /* Return VBE Supplemental
                                        Specification Information */
/* *** Structures *** */
typedef struct {
    uint16_t offset;
    uint16_t selector;
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_far_pointer;

typedef struct {
    uint8_t   Signature[4];    /*  PM Info Block Signature */
    uint16_t  EntryPoint;      /*  Offset of PM entry point within BIOS */
    uint16_t  PMInitialize;    /*  Offset of PM initialization entry point */
    uint16_t  BIOSDataSel;     /*  Selector to BIOS data area emulation block */
    uint16_t  A0000Sel;        /*  Selector to access A0000h physical mem */
    uint16_t  B0000Sel;        /*  Selector to access B0000h physical mem */
    uint16_t  B8000Sel;        /*  Selector to access B8000h physical mem */
    uint16_t  CodeSegSel;      /*  Selector to access code segment as data */
    uint8_t   InProtectMode;   /*  Set to 1 when in protected mode */
    uint8_t   Checksum;        /*  Checksum byte for structure */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_protected_mode_info_block;

/* General VBE signature */
#define VBE_SIGNATURE "VESA"
/* Signature for VBE 2.0 and higher */
#define VBE20plus_SIGNATURE "VBE2"
/* for STUB see VBE CORE FUNCTIONS VERSION 3.0 - Appendix 1 */
#define VBE_END_OF_VideoModeList 0xFFFF
#define VBE_STUB_VideoModeList 0xFFFF
typedef struct {
    uint8_t   VbeSignature[4];   /*  VBE Signature */
    uint16_t  VbeVersion;        /*  VBE Version */
    uint8_t  *OemStringPtr;      /*  VbeFarPtr to OEM String */
    uint8_t   Capabilities[4];   /*  Capabilities of graphics controller */
    uint32_t *VideoModePtr;      /*  VbeFarPtr to VideoModeList */
    uint16_t  TotalMemory;       /*  Number of 64kb memory blocks    */
    /*  Added for VBE 2.0+ */
    uint16_t  OemSoftwareRev;    /*  VBE implementation Software revision */
    uint8_t  *OemVendorNamePtr;  /*  VbeFarPtr to Vendor Name String */
    uint8_t  *OemProductNamePtr; /*  VbeFarPtr to Product Name String */
    uint8_t  *OemProductRevPtr;  /*  VbeFarPtr to Product Revision String */
    uint8_t   Reserved[222];     /*  Reserved for VBE implementation scratch */
    /*    area */
    uint8_t   OemData[256];      /*  Data Area for OEM Strings */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_vbe_info_block;

typedef struct {
    /*  Mandatory information for all VBE revisions */
    uint16_t  ModeAttributes;        /* mode attributes */
    uint8_t   WinAAttributes;        /* window A attributes */
    uint8_t   WinBAttributes;        /* window B attributes */
    uint16_t  WinGranularity;        /* window granularity */
    uint16_t  WinSize;               /* window size */
    uint16_t  WinASegment;           /* window A start segment */
    uint16_t  WinBSegment;           /* window B start segment */
    uint32_t *WinFuncPtr;            /* real mode pointer to window function */
    uint16_t  BytesPerScanLine;      /* bytes per scan line */
    /*  Mandatory information for VBE 1.2 and above */
    uint16_t  XResolution;           /* horizontal resolution in px or chars */
    uint16_t  YResolution;           /* vertical resolution in px or chars */
    uint8_t   XCharSize;             /* character cell width in pixels */
    uint8_t   YCharSize;             /* character cell height in pixels */
    uint8_t   NumberOfPlanes;        /* number of memory planes */
    uint8_t   BitsPerPixel;          /* bits per pixel */
    uint8_t   NumberOfBanks;         /* number of banks */
    uint8_t   MemoryModel;           /* memory model type */
    uint8_t   BankSize;              /* bank size in KB */
    uint8_t   NumberOfImagePages;    /* number of images */
    uint8_t   Reserved0;             /* reserved for page function */
    /*  Direct Color fields (required for direct/6 and YUV/7 memory models) */
    uint8_t   RedMaskSize;           /* size of direct color red mask in bits */
    uint8_t   RedFieldPosition;      /* bit position of lsb of red mask */
    uint8_t   GreenMaskSize;         /* size of direct color green mask in b */
    uint8_t   GreenFieldPosition;    /* bit position of lsb of green mask */
    uint8_t   BlueMaskSize;          /* size of direct color blue mask in b */
    uint8_t   BlueFieldPosition;     /* bit position of lsb of blue mask */
    uint8_t   RsvdMaskSize;          /* size of direct color reserved mask */
    uint8_t   RsvdFieldPosition;     /* bit position of lsb of reserved mask */
    uint8_t   DirectColorModeInfo;   /* direct color mode attributes */
    /*  Mandatory information for VBE 2.0 and above */
    uint32_t *PhysBasePtr;           /* physical address for
                                        flat memory frame buffer */
    uint32_t  Reserved1;             /* Reserved - always set to 0 */
    uint16_t  Reserved2;             /* Reserved - always set to 0 */
    /*  Mandatory information for VBE 3.0 and above */
    uint16_t  LinBytesPerScanLine;   /* bytes per scan line for linear modes */
    uint8_t   BnkNumberOfImagePages; /* number of images for banked modes */
    uint8_t   LinNumberOfImagePages; /* number of images for linear modes */
        /* linear modes */
    uint8_t   LinRedMaskSize;        /* size of direct color red mask */
    uint8_t   LinRedFieldPosition;   /* bit position of lsb of red mask */
    uint8_t   LinGreenMaskSize;      /* size of direct color green mask  */
    uint8_t   LinGreenFieldPosition; /* bit position of lsb of green mask */
    uint8_t   LinBlueMaskSize;       /* size of direct color blue mask  */
    uint8_t   LinBlueFieldPosition;  /* bit position of lsb of blue mask */
    uint8_t   LinRsvdMaskSize;       /* size of direct color reserved mask */
    uint8_t   LinRsvdFieldPosition;  /* bit position of lsb of reserved mask */
    uint32_t  MaxPixelClock;         /* maximum pixel clock
                                        (in Hz) for graphics mode */
    uint8_t   Reserved3[189];        /* remainder of ModeInfoBlock */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_mode_info_block;

typedef struct {
    uint16_t  HorizontalTotal;       /* Horizontal total in pixels */
    uint16_t  HorizontalSyncStart;   /* Horizontal sync start in pixels */
    uint16_t  HorizontalSyncEnd;     /* Horizontal sync end in pixels */
    uint16_t  VerticalTotal;         /* Vertical total in lines */
    uint16_t  VerticalSyncStart;     /* Vertical sync start in lines */
    uint16_t  VerticalSyncEnd;       /* Vertical sync end in lines */
    uint8_t   Flags;                 /* Flags (Interlaced, Double Scan etc) */
    uint32_t  PixelClock;            /* Pixel clock in units of Hz */
    uint16_t  RefreshRate;           /* Refresh rate in units of 0.01 Hz */
    uint8_t   Reserved[40];          /* remainder of ModeInfoBlock */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_CRTC_info_block;

typedef struct {
    uint8_t   Blue;                  /* Blue channel value (6 or 8 bits) */
    uint8_t   Green;                 /* Green channel value (6 or 8 bits) */
    uint8_t   Red;                   /* Red channel value(6 or 8 bits) */
    uint8_t   Alignment;             /* DWORD alignment byte (unused) */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_palette_entry;

typedef struct {
    uint8_t   SupVbeSignature[7];    /* Supplemental VBE Signature */
    uint16_t  SupVbeVersion;         /* Supplemental VBE Version */
    uint8_t   SupVbeSubFunc[8];      /* Bitfield of supported subfunctions */
    uint16_t  OemSoftwareRev;        /* OEM Software revision */
    uint8_t  *OemVendorNamePtr;      /* VbeFarPtr to Vendor Name String */
    uint8_t  *OemProductNamePtr;     /* VbeFarPtr to Product Name String */
    uint8_t  *OemProductRevPtr;      /* VbeFarPtr to Product Revision String */
    uint8_t  *OemStringPtr;          /* VbeFarPtr to OEM String */
    uint8_t   Reserved[221];         /* Reserved for description
                                        strings and future */
    /*  expansion */
} RTEMS_COMPILER_PACKED_ATTRIBUTE VBE_supplemental_vbe_info_block;

/* VbeInfoBlock Capabilities */
/*  D0 = 0  DAC is fixed width, with 6 bits per primary color */
/*     = 1 DAC width is switchable to 8 bits per primary color */
#define VBE_DACswitchableMask     0x0001
/*  D1 = 0 Controller is VGA compatible */
/*     = 1 Controller is not VGA compatible */
#define VBE_notVGAcompatibleMask  0x0002
/*  D2 = 0 Normal RAMDAC operation */
/*     = 1 When programming large blocks of information to the RAMDAC,
            use the blank bit in Function 09h. */
#define VBE_specialRAMDACopMask   0x0004
/*  D3 = 0 No hardware stereoscopic signaling support */
/*     = 1 Hardware stereoscopic signaling supported by controller */
#define VBE_hwStereoscopicMask    0x0008
/*  D4 = 0 Stereo signaling supported via external VESA stereo connector */
/*     = 1 Stereo signaling supported via VESA EVC connector */
#define VBE_supportEVCconnMask    0x0010
/*  D5-31 = Reserved */

/* ModeInfoBlock ModeAttributes */
/*  D0 =  Mode supported by hardware configuration */
/*   0 =  Mode not supported in hardware */
/*   1 =  Mode supported in hardware */
#define VBE_modSupInHWMask        0x0001
/*  D1 =  1 (Reserved) */
/*  D2 =  TTY Output functions supported by BIOS */
/*   0 =  TTY Output functions not supported by BIOS */
/*   1 =  TTY Output functions supported by BIOS */
#define VBE_TTYOutSupByBIOSMask   0x0004
/*  D3 =  Monochrome/color mode (see note below) */
/*   0 =  Monochrome mode */
/*   1 =  Color mode */
#define VBE_ColorModeMask         0x0008
/*  D4 =  Mode type */
/*   0 =  Text mode */
/*   1 =  Graphics mode */
#define VBE_GraphicsModeMask      0x0010
/*  D5 =  VGA compatible mode */
/*   0 =  Yes */
/*   1 =  No */
#define VBE_VGACompModeMask       0x0020
/*  D6 =  VGA compatible windowed memory mode is available */
/*   0 =  Yes */
/*   1 =  No */
#define VBE_VGACompWinMemModeMask 0x0040
/*  D7 =  Linear frame buffer mode is available */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_LinFraBufModeAvaiMask 0x0080
/*  D8 =  Double scan mode is available */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_DblScnModeAvaiMask    0x0100
/*  D9 =  Interlaced mode is available */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_InterlModeAvaiMask    0x0200
/*  D10 =  Hardware triple buffering support */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_HWTripBufSupMask      0x0400
/*  D11 =  Hardware stereoscopic display support */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_HWSterDispSupMask     0x0800
/*  D12 =  Dual display start address support */
/*   0 =  No */
/*   1 =  Yes */
#define VBE_DualDispStAdrSupMask  0x1000
/*  D13-D15 =  Reserved */

/* ModeInfoBlock WinXAttributes */
/*  D0 =  Relocatable window(s) supported */
/*   0 =  Single non-relocatable window only */
/*   1 =  Relocatable window(s) are supported */
#define VBE_RelocWinSupMask       0x01
/*  D1 =  Window readable */
/*   0 =  Window is not readable */
/*   1 =  Window is readable */
#define VBE_WinReadableMask       0x02
/*  D2 =  Window writeable */
/*   0 =  Window is not writeable */
/*   1 =  Window is writeable */
#define VBE_WinWritableMask       0x04
/*  D3-D7 =  Reserved */

/* ModeInfoBlock MemoryModel */
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

/* ModeInfoBlock DirectColorModeInfo */
/*  D0 =  Color ramp is fixed/programmable */
/*   0 =  Color ramp is fixed */
/*   1 =  Color ramp is programmable */
#define VBE_ColRampProgMask       0x01
/*  D1 =  Bits in Rsvd field are usable/reserved */
/*   0 =  Bits in Rsvd field are reserved */
/*   1 =  Bits in Rsvd field are usable by the application */
#define VBE_RsvdBitsUsableMask    0x02

/* CRTCInfoBlock Flags */
/*  D0 =  Double Scan Mode Enable */
/*   0 =  Graphics mode is not double scanned */
/*   1 =  Graphics mode is double scanned */
#define VBE_GrModeDblScanMask     0x01
/*  D1 =  Interlaced Mode Enable */
/*   0 =  Graphics mode is non-interlaced */
/*   1 =  Graphics mode is interlaced */
#define VBE_GrModeInterlMask      0x02
/*  D2 =  Horizontal sync polarity */
/*   0 =  Horizontal sync polarity is positive (+) */
/*   1 =  Horizontal sync polarity is negative (-) */
#define VBE_HorSncPolNegMask      0x04
/*  D3 =  Vertical sync polarity */
/*   0 =  Vertical sync polarity is positive (+) */
/*   1 =  Vertical sync polarity is negative (-) */
#define VBE_VerSncPolNegMask      0x08


/*  VESA BIOS Extensions/Display Data Channel Standard
    Version: 1.1 November 18, 1999                      */

/* VBE/DDC subfunction numbers - passed in BL register */
#define VBEDDC_Capabilities    0x0   /* Report VBE/DDC Capabilities */
#define VBEDDC_ReadEDID        0x1   /* Read EDID */

/* DDC Capabilities */
/* DDC level supported - returned in BL register */
/* 0 - DDC1 not supported; 1 - DDC1 supported */
#define VBEDDC_1SupportedMask  0x1
/* 0 - DDC2 not supported; 1 - DDC2 supported */
#define VBEDDC_2SupportedMask  0x2
/* 0 - Screen not blanked during data transfer;
   1 - Screen blanked during data transfer */
#define VBEDDC_scrBlnkDatTrMs  0x4


/*  VESA BIOS Extensions/Serial Control Interface Standard
    Version: 1.0 Revision: 2 Date: July 2, 1997         */

/* VBE/SCI subfunction numbers - passed in BL register */
#define VBESCI_ReportCapabil   0x10  /* Report VBE/SCI Capabilities */
#define VBESCI_BegSCLSDACtrl   0x11  /* Begin SCL/SDA control */
#define VBESCI_EndSCLSDACtrl   0x12  /* End SCL/SDA control */
#define VBESCI_WrtSCLClkLine   0x13  /* Write SCL clock line */
#define VBESCI_WrtSDADatLine   0x14  /* Write SDA data line */
#define VBESCI_RdySCLClkLine   0x15  /* Read SCL clock line */
#define VBESCI_RdySDADatLine   0x16  /* Read SDA data line */

/* SCI Capabilities */
/* I2C level supported - returned in BL register */
#define VBESCI_capSCLwrtMask   0x1   /* Can write to SCL clock line */
#define VBESCI_capSDAwrtMask   0x2   /* Can write to SDA data line */
#define VBESCI_capSCLrdyMask   0x4   /* Can read from SCL clock line */
#define VBESCI_capSDArdyMask   0x8   /* Can read from SDA data line */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _VBE_H */
