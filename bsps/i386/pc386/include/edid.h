/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief VESA EDID definitions.
 *
 * This file contains definitions for constants related to
 * VESA Extended Display Identification Data.
 *         More information can be found at
 *     <http://www.vesa.org/vesa-standards/free-standards/>
 *         VESA public standards may be found at
 *     <http://www.vesa.org/wp-content/uploads/2010/12/thankspublic.htm>
 */

/*
 * Copyright (C) 2014  Jan Dolezal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _EDID_H
#define _EDID_H

#ifndef ASM /* ASM */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/score/basedefs.h>
#define EDID_INLINE_ROUTINE     static inline

/*  VESA Enhanced Extended Display Identification Data (E-EDID) Proposed
    Release A, March 27, 2007 */

/* *** Detailed Timing Descriptor Flags *** */
#define EDID1_DTD_Flag_InterlacedOff            7
#define EDID1_DTD_Flag_InterlacedMask           0x1
#define EDID1_DTD_Flag_StereoModeOff            0
#define EDID1_DTD_Flag_StereoModeMask           0xC1
/* values for stereo flag */
#define EDID1_DTD_Stereo_FldSeqRightOnSync      0x40
#define EDID1_DTD_Stereo_FldSeqLeftOnSync       0x80
#define EDID1_DTD_Stereo_2wItlvdRightOnEven     0x41
#define EDID1_DTD_Stereo_2wItlvdLeftOnEven      0x81
#define EDID1_DTD_Stereo_4wInterleaved          0xC0
#define EDID1_DTD_Stereo_SideBySideItlvd        0xC1
/* Analog = 0, Digital = 1 */
#define EDID1_DTD_Flag_DigitalOff               4
#define EDID1_DTD_Flag_DigitalMask              0x1
/* Analog */
#define EDID1_DTD_BipolarAnalogComposSyncOff    3
#define EDID1_DTD_BipolarAnalogComposSyncMask   0x1
#define EDID1_DTD_WithSerrationsOff             2
#define EDID1_DTD_WithSerrationsMask            0x1
/* Digital */
#define EDID1_DTD_DigitalSeparateSyncOff        3
#define EDID1_DTD_DigitalSeparateSyncMask       0x1
    /* when DigitalSeparateSync == 0 -> it is composite
       and WithSerrations defined up in Analog part applies */
#define EDID1_DTD_VerticalSyncIsPositiveOff     2
#define EDID1_DTD_VerticalSyncIsPositiveMask    0x1
#define EDID1_DTD_HorizontalSyncIsPositiveOff   1
#define EDID1_DTD_HorizontalSyncIsPositiveMask  0x1

typedef struct {
    uint8_t PixelClock_div10000[2];
    uint8_t HorizontalActiveLow;
    uint8_t HorizontalBlankingLow;
    uint8_t HorizontalBlanking_ActiveHigh;
    uint8_t VerticalActiveLow;
    uint8_t VerticalBlankingLow;
    uint8_t VerticalBlanking_ActiveHigh;
    uint8_t HorizontalSyncOffsetLow;
    uint8_t HorizontalSyncPulseWidthLow;
    uint8_t VerticalSyncPulseWidth_OffsetLow;
    uint8_t Vert_Hor_SyncPulseWidth_Offset_High;
    uint8_t HorizontalImageSizeLow;
    uint8_t VerticalImageSizeLow;
    uint8_t Vertical_HorizontalImageSizeHigh;
    uint8_t HorizontalBorder;
    uint8_t VerticalBorder;
    uint8_t Flags;
} RTEMS_PACKED EDID_detailed_timing_descriptor;

EDID_INLINE_ROUTINE uint16_t DTD_horizontal_active (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->HorizontalActiveLow |
        (dtd->HorizontalBlanking_ActiveHigh & 0xF0) << 4);
}

EDID_INLINE_ROUTINE uint16_t DTD_horizontal_blanking (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->HorizontalBlankingLow |
        (dtd->HorizontalBlanking_ActiveHigh & 0xF) << 8);
}

EDID_INLINE_ROUTINE uint16_t DTD_vertical_active (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->VerticalActiveLow |
        (dtd->VerticalBlanking_ActiveHigh & 0xF0) << 4);
}

EDID_INLINE_ROUTINE uint16_t DTD_vertical_blanking (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->VerticalBlankingLow |
        (dtd->VerticalBlanking_ActiveHigh & 0xF) << 8);
}

EDID_INLINE_ROUTINE uint16_t DTD_vertical_sync_pulse_width (
    EDID_detailed_timing_descriptor *dtd)
{
    return ((dtd->VerticalSyncPulseWidth_OffsetLow & 0xF) |
        (dtd->Vert_Hor_SyncPulseWidth_Offset_High & 0x3) << 4);
}

EDID_INLINE_ROUTINE uint16_t DTD_vertical_sync_offset (
    EDID_detailed_timing_descriptor *dtd)
{
    return ((dtd->VerticalSyncPulseWidth_OffsetLow >> 4) |
        (dtd->Vert_Hor_SyncPulseWidth_Offset_High & 0xC) << 2);
}

EDID_INLINE_ROUTINE uint16_t DTD_horizontal_sync_pulse_width (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->HorizontalSyncPulseWidthLow |
        (dtd->Vert_Hor_SyncPulseWidth_Offset_High & 0x30) << 4);
}

EDID_INLINE_ROUTINE uint16_t DTD_horizontal_sync_offset (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->HorizontalSyncOffsetLow |
        (dtd->Vert_Hor_SyncPulseWidth_Offset_High & 0xC0) << 2);
}

EDID_INLINE_ROUTINE uint16_t DTD_vertical_image_size (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->VerticalImageSizeLow |
        (dtd->Vertical_HorizontalImageSizeHigh & 0xF) << 8);
}

EDID_INLINE_ROUTINE uint16_t DTD_horizontal_image_size (
    EDID_detailed_timing_descriptor *dtd)
{
    return (dtd->HorizontalImageSizeLow |
        (dtd->Vertical_HorizontalImageSizeHigh & 0xF0) << 4);
}

typedef struct {
    uint8_t ColorPointWhitePointIndexNumber;
    uint8_t ColorPointWhiteLowBits;
    uint8_t ColorPointWhite_x;
    uint8_t ColorPointWhite_y;
    uint8_t ColorPointWhiteGamma;
} RTEMS_PACKED EDID_color_point_data;

/* Basic Display Parameters */
/* Monitor Descriptor - Data Type Tag */
#define EDID_DTT_MonitorSerialNumber        0xFF

#define EDID_DTT_ASCIIString                0xFE

#define EDID_DTT_MonitorRangeLimits         0xFD
typedef struct {
    uint8_t MinVerticalRateInHz;
    uint8_t MaxVerticalRateInHz;
    uint8_t MinHorizontalInKHz;
    uint8_t MaxHorizontalInKHz;
    uint8_t MaxSupportedPixelClockIn10MHz;
/* see  VESA, Generalized Timing Formula Standard - GTF
        Version 1.0, December 18, 1996 */
    uint8_t GTFStandard[8];
} RTEMS_PACKED EDID_monitor_range_limits;

#define EDID_DTT_MonitorName                0xFC

#define EDID_DTT_AdditionalColorPointData   0xFB
/* Standard Timing Identification */
#define EDID_DTT_AdditionalSTI              0xFA

#define EDID_DTT_DisplayColorManagement     0xF9

#define EDID_DTT_CVT3ByteTimingCodes        0xF8

#define EDID1_CVT_AspectRatioOff            2
#define EDID1_CVT_AspectRatioMask           0x3
#define EDID1_CVT_AddressableLinesHighOff   4
#define EDID1_CVT_AddressableLinesHighMask  0xF
    /* next 5 bits indicate supported vertical rates */
#define EDID1_CVT_VerticalRate60HzRBOff     0
#define EDID1_CVT_VerticalRate60HzRBMask    0x1
#define EDID1_CVT_VerticalRate85HzOff       1
#define EDID1_CVT_VerticalRate85HzMask      0x1
#define EDID1_CVT_VerticalRate75HzOff       2
#define EDID1_CVT_VerticalRate75HzMask      0x1
#define EDID1_CVT_VerticalRate60HzOff       3
#define EDID1_CVT_VerticalRate60HzMask      0x1
#define EDID1_CVT_VerticalRate50HzOff       4
#define EDID1_CVT_VerticalRate50HzMask      0x1
#define EDID1_CVT_PreferredVerticalRateOff  5
#define EDID1_CVT_PreferredVerticalRateMask 0x3

#define EDID_CVT_AspectRatio_4_3            0
#define EDID_CVT_AspectRatio_16_9           1
#define EDID_CVT_AspectRatio_16_10          2
#define EDID_CVT_AspectRatio_15_9           3
#define EDID_CVT_PrefVertRate50Hz           0
#define EDID_CVT_PrefVertRate60Hz           1
#define EDID_CVT_PrefVertRate75Hz           2
#define EDID_CVT_PrefVertRate85Hz           3
typedef struct {
    uint8_t AddressableLinesLow;
    uint8_t AspectRatio_AddressableLinesHigh;
    uint8_t VerticalRate_PreferredVerticalRate;
} RTEMS_PACKED EDID_CVT_3_byte_code_descriptor;
typedef struct {
    uint8_t VersionNumber;
    EDID_CVT_3_byte_code_descriptor cvt[4];
} RTEMS_PACKED EDID_CVT_timing_codes_3B;

EDID_INLINE_ROUTINE uint16_t edid1_CVT_addressable_lines_high (
    EDID_CVT_3_byte_code_descriptor *cvt)
{
    return (cvt->AddressableLinesLow |
        (cvt->VerticalRate_PreferredVerticalRate &
         (EDID1_CVT_AddressableLinesHighMask<<EDID1_CVT_AddressableLinesHighOff)
        ) << (8-EDID1_CVT_AddressableLinesHighOff) );
}

EDID_INLINE_ROUTINE uint8_t edid1_CVT_aspect_ratio (
    EDID_CVT_3_byte_code_descriptor *cvt)
{
    return (cvt->AspectRatio_AddressableLinesHigh >> EDID1_CVT_AspectRatioOff) &
        EDID1_CVT_AspectRatioMask;
}

#define EDID_DTT_EstablishedTimingsIII      0xF7
typedef struct {
    uint8_t RevisionNumber;
    uint8_t EST_III[12];
} RTEMS_PACKED EDID_established_timings_III;
enum EST_III {
    EST_1152x864_75Hz   = 0,
    EST_1024x768_85Hz   = 1,
    EST_800x600_85Hz    = 2,
    EST_848x480_60Hz    = 3,
    EST_640x480_85Hz    = 4,
    EST_720x400_85Hz    = 5,
    EST_640x400_85Hz    = 6,
    EST_640x350_85Hz    = 7,

    EST_1280x1024_85Hz  = 8,
    EST_1280x1024_60Hz  = 9,
    EST_1280x960_85Hz   = 10,
    EST_1280x960_60Hz   = 11,
    EST_1280x768_85Hz   = 12,
    EST_1280x768_75Hz   = 13,
    EST_1280x768_60Hz   = 14,
    EST_1280x768_60HzRB = 15,

    EST_1400x1050_75Hz  = 16,
    EST_1400x1050_60Hz  = 17,
    EST_1400x1050_60HzRB= 18,
    EST_1400x900_85Hz   = 19,
    EST_1400x900_75Hz   = 20,
    EST_1400x900_60Hz   = 21,
    EST_1400x900_60HzRB = 22,
    EST_1360x768_60Hz   = 23,

    EST_1600x1200_70Hz  = 24,
    EST_1600x1200_65Hz  = 25,
    EST_1600x1200_60Hz  = 26,
    EST_1680x1050_85Hz  = 27,
    EST_1680x1050_75Hz  = 28,
    EST_1680x1050_60Hz  = 29,
    EST_1680x1050_60HzRB= 30,
    EST_1400x1050_85Hz  = 31,

    EST_1920x1200_60Hz  = 32,
    EST_1920x1200_60HzRB= 33,
    EST_1856x1392_75Hz  = 34,
    EST_1856x1392_60Hz  = 35,
    EST_1792x1344_75Hz  = 36,
    EST_1792x1344_60Hz  = 37,
    EST_1600x1200_85Hz  = 38,
    EST_1600x1200_75Hz  = 39,

    EST_1920x1440_75Hz  = 44,
    EST_1920x1440_60Hz  = 45,
    EST_1920x1200_85Hz  = 46,
    EST_1920x1200_75Hz  = 47,
};

#define EDID_DTT_DescriptorSpaceUnused      0x10
/* DTT 0x0 - 0xF are manufacturer specific */

typedef struct {
    uint8_t Flag0[2];
    uint8_t Flag1;
    uint8_t DataTypeTag;
    uint8_t Flag2;
    uint8_t DescriptorData[13];
} RTEMS_PACKED EDID_monitor_descriptor;

union EDID_DTD_MD {
    EDID_detailed_timing_descriptor dtd;
    EDID_monitor_descriptor md;
} RTEMS_PACKED;

#define EDID1_STI_ImageAspectRatioOff           0
#define EDID1_STI_ImageAspectRatioMask          0x3
#define EDID1_STI_RefreshRateOff                2
#define EDID1_STI_RefreshRateMask               0x3F

#define EDID_STI_DescriptorUnused           0x0101
#define EDID_STI_AspectRatio_16_10          0
#define EDID_STI_AspectRatio_4_3            1
#define EDID_STI_AspectRatio_5_4            2
#define EDID_STI_AspectRatio_16_9           3
typedef struct {
    uint8_t HorizontalActivePixels;
    uint8_t ImageAspectRatio_RefreshRate;
} RTEMS_PACKED EDID_standard_timing_identification;

/* Video Input Definition */
/* Analog = 0, Digital = 1 */
#define EDID1_VID_DigitalSignalLevelOff         7
#define EDID1_VID_DigitalSignalLevelMask        0x1
/* for EDID1_VID_DigitalSignalLevelOff = 1 (Digital) */
#define EDID1_VID_ColorBitDepthOff              4
#define EDID1_VID_ColorBitDepthMask             0x7 /* see CBD */
#define EDID1_VID_DigitalVideoStandardSuppOff   0
#define EDID1_VID_DigitalVideoStandardSuppMask  0xF /* see DVS */
/* for EDID1_VID_DigitalSignalLevelOff = 0 (Analog) */
#define EDID1_VID_SignalLevelStandardOff        5
#define EDID1_VID_SignalLevelStandardMask       0x3
#define EDID1_VID_VideoSetupBlankOff            4
#define EDID1_VID_VideoSetupBlankMask           0x1
#define EDID1_VID_SeparateSyncHandVSignalsOff   3
#define EDID1_VID_SeparateSyncHandVSignalsMask  0x1
#define EDID1_VID_SyncSignalOnHorizontalOff     2
#define EDID1_VID_SyncSignalOnHorizontalMask    0x1
#define EDID1_VID_SyncSignalOnGreenOff          1
#define EDID1_VID_SyncSignalOnGreenMask         0x1
#define EDID1_VID_SerationOnVerticalSyncOff     0
#define EDID1_VID_SerationOnVerticalSyncMask    0x1
/* Analog Interface Data Format - Signal Level Standard */
#define EDID_SLS_0700_0300_1000Vpp             0x0
#define EDID_SLS_0714_0286_1000Vpp             0x1
#define EDID_SLS_1000_0400_1400Vpp             0x2
#define EDID_SLS_0700_0000_0700Vpp             0x3

/* Color Bit Depths */
#define CBD_undef               0x0
#define CBD_6bPerPrimaryColor   0x1
#define CBD_8bPerPrimaryColor   0x2
#define CBD_10bPerPrimaryColor  0x3
#define CBD_12bPerPrimaryColor  0x4
#define CBD_14bPerPrimaryColor  0x5
#define CBD_16bPerPrimaryColor  0x6
#define CBD_reserved            0x7

/* Digital Video Standard Supported */
#define DVS_undef               0x0
#define DVS_DVI                 0x1
#define DVS_HDMI_a              0x2
#define DVS_HDMI_b              0x3
#define DVS_MDDI                0x4
#define DVS_DiplayPort          0x5

/* Feature Support */
#define EDID1_Feature_GTFSupported_mask              0x1
#define EDID1_Feature_GTFSupported_off               0
#define EDID1_Feature_PreferredTimingMode_mask       0x1
#define EDID1_Feature_PreferredTimingMode_off        1
#define EDID1_Feature_StandardDefaultColorSpace_mask 0x1
#define EDID1_Feature_StandardDefaultColorSpace_off  2
#define EDID1_Feature_DisplayType_mask               0x2
#define EDID1_Feature_DisplayType_off                3
        /* Refer to VESA DPMS Specification */
#define EDID1_Feature_ActiveOff_mask                 0x1
#define EDID1_Feature_ActiveOff_off                  5
#define EDID1_Feature_Suspend_mask                   0x1
#define EDID1_Feature_Suspend_off                    6
#define EDID1_Feature_StandBy_mask                   0x1
#define EDID1_Feature_StandBy_off                    7
    /* analog - Display Color Type */
#define EDID_DisplayType_Monochrome                 0
#define EDID_DisplayType_RGBcolor                   1
#define EDID_DisplayType_nonRGBcolor                2
#define EDID_DisplayType_undef                      3
    /* digital - Supported Color Encoding Formats */
#define EDID_DisplayType_RGB444                     0
#define EDID_DisplayType_RGB444YCrCb444             1
#define EDID_DisplayType_RGB444YCrCb422             2
#define EDID_DisplayType_RGB444YCrCb444YCrCb422     3

typedef struct {
    uint8_t Header[8];
/*  Vendor Product Identification */
    uint8_t IDManufacturerName[2];
    uint8_t IDProductCode[2];
    uint8_t IDSerialNumber[4];
    uint8_t WeekofManufacture;
    uint8_t YearofManufacture;
/*  EDID Structure Version Revision Level */
    uint8_t Version;
    uint8_t Revision;
/*  Basic Display Parameters Features */
    /* Video Input Definition */
    uint8_t VideoInputDefinition;
    uint8_t MaxHorizontalImageSize;
    uint8_t MaxVerticalImageSize;
    uint8_t DisplayTransferCharacteristic;
    /* Feature Support */
    uint8_t Features;
/*  Color Characteristics */
    uint8_t GreenRedLow;
    uint8_t WhiteBlueLow;
    uint8_t RedXHigh;
    uint8_t RedYHigh;
    uint8_t GreenXHigh;
    uint8_t GreenYHigh;
    uint8_t BlueXHigh;
    uint8_t BlueYHigh;
    uint8_t WhiteXHigh;
    uint8_t WhiteYHigh;
/*  Established Timings I, II, Manufacturer's */
    uint8_t EST_I_II_Man[3];
/*  Standard Timing Identification */
    EDID_standard_timing_identification STI[8];
/*  Detailed Timing Descriptions / Monitor Descriptions */
    union EDID_DTD_MD dtd_md[4];
    uint8_t ExtensionFlag;
    uint8_t Checksum;
} RTEMS_PACKED EDID_edid1;

EDID_INLINE_ROUTINE uint16_t edid1_RedX (EDID_edid1 *edid) {
    return (edid->RedXHigh<<2) | (edid->GreenRedLow>>6);
}
EDID_INLINE_ROUTINE uint16_t edid1_RedY (EDID_edid1 *edid) {
    return (edid->RedYHigh<<2) | (edid->GreenRedLow>>4)&&0x3;
}
EDID_INLINE_ROUTINE uint16_t edid1_GreenX (EDID_edid1 *edid) {
    return (edid->GreenXHigh<<2) | (edid->GreenRedLow>>2)&&0x3;
}
EDID_INLINE_ROUTINE uint16_t edid1_GreenY (EDID_edid1 *edid) {
    return (edid->GreenYHigh<<2) | (edid->GreenRedLow&0x3);
}
EDID_INLINE_ROUTINE uint16_t edid1_BlueX (EDID_edid1 *edid) {
    return (edid->BlueXHigh<<2)  | (edid->WhiteBlueLow>>6);
}
EDID_INLINE_ROUTINE uint16_t edid1_BlueY (EDID_edid1 *edid) {
    return (edid->BlueYHigh<<2)  | (edid->WhiteBlueLow>>4)&&0x3;
}
EDID_INLINE_ROUTINE uint16_t edid1_WhiteX (EDID_edid1 *edid) {
    return (edid->WhiteXHigh<<2) | (edid->WhiteBlueLow>>2)&&0x3;
}
EDID_INLINE_ROUTINE uint16_t edid1_WhiteY (EDID_edid1 *edid) {
    return (edid->WhiteYHigh<<2) | (edid->WhiteBlueLow&0x3);
}

EDID_INLINE_ROUTINE int edid1_STI_is_unused (
      const EDID_standard_timing_identification *edid_sti) {
    return (edid_sti->HorizontalActivePixels ==
                  (uint8_t)EDID_STI_DescriptorUnused) &&
           (edid_sti->ImageAspectRatio_RefreshRate ==
                  (uint8_t)(EDID_STI_DescriptorUnused >> 8));
}

enum edid1_established_timings {
/*  Established Timings I */
    EST_800x600_60Hz    = 0,
    EST_800x600_56Hz    = 1,
    EST_640x480_75Hz    = 2,
    EST_640x480_72Hz    = 3,
    EST_640x480_67Hz    = 4,
    EST_640x480_60Hz    = 5,
    EST_720x400_88Hz    = 6,
    EST_720x400_70Hz    = 7,
/*  Established Timings II */
    EST_1280x1024_75Hz  = 8,
    EST_1024x768_75Hz   = 9,
    EST_1024x768_70Hz   = 10,
    EST_1024x768_60Hz   = 11,
    EST_1024x768_87Hz   = 12,
    EST_832x624_75Hz    = 13,
    EST_800x600_75Hz    = 14,
    EST_800x600_72Hz    = 15,
/*  Manufacturer's Timings */
    EST_1152x870_75Hz   = 23,
};

EDID_INLINE_ROUTINE uint8_t edid1_established_tim (
    EDID_edid1 *edid,
    enum edid1_established_timings est)
{
    return (uint8_t)(edid->EST_I_II_Man[est/8] & (est%8));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _VBE_H */
