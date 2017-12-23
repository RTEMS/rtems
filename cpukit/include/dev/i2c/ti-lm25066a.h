/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * TI LM25066A
 *   http://www.ti.com/product/LM25066A
 */

#ifndef TI_LM25066A_H
#define TI_LM25066A_H

#include <dev/i2c/i2c.h>

/*
 * PM Bus Command interface.
 *
 * The keys are:
 *   IO : IO direction R=read W=write
 *   SZ : Size in bytes.
 */
typedef enum
{                                            /* IO SZ Name  */
  TI_LM25066A_OPERATION                = 0,  /* R   1 PMBus */
  TI_LM25066A_CLEAR_FAULTS             = 1,  /*  W  0 PMBus */
  TI_LM25066A_CAPABILITY               = 2,  /* R   1 PMBus */
  TI_LM25066A_VOUT_UV_WARN_LIMIT       = 3,  /* RW  2 PMBus */
  TI_LM25066A_OT_FAULT_LIMIT           = 4,  /* RW  2 PMBus */
  TI_LM25066A_OT_WARN_LIMIT            = 5,  /* RW  2 PMBus */
  TI_LM25066A_VIN_OV_WARN_LIMIT        = 6,  /* RW  2 PMBus */
  TI_LM25066A_VIN_UV_WARN_LIMIT        = 7,  /* RW  2 PMBus */
  TI_LM25066A_STATUS_BYTE              = 8,  /* R   1 PMBus */
  TI_LM25066A_STATUS_WORD              = 9,  /* R   2 PMBus */
  TI_LM25066A_STATUS_VOUT              = 10, /* R   1 PMBus */
  TI_LM25066A_STATUS_INPUT             = 11, /* R   1 PMBus */
  TI_LM25066A_STATUS_TEMPERATURE       = 12, /* R   1 PMBus */
  TI_LM25066A_STATUS_CML               = 13, /* R   1 PMBus */
  TI_LM25066A_STATUS_MFR_SPECIFIC      = 14, /* R   1 PMBus */
  TI_LM25066A_READ_VIN                 = 15, /* R   2 PMBus */
  TI_LM25066A_READ_VOUT                = 16, /* R   2 PMBus */
  TI_LM25066A_READ_TEMPERATURE_1       = 17, /* R   2 PMBus */
  TI_LM25066A_MFR_ID                   = 18, /* R   3 PMBus */
  TI_LM25066A_MFR_MODEL                = 19, /* R   8 PMBus */
  TI_LM25066A_MFR_REVISION             = 20, /* R   2 PMBus */
  TI_LM25066A_MFR_READ_VAUX            = 21, /* R   2 MFR_SPECIFIC_00 */
  TI_LM25066A_MFR_READ_IIN             = 22, /* R   2 MFR_SPECIFIC_01 */
  TI_LM25066A_MFR_READ_PIN             = 23, /* R   2 MFR_SPECIFIC_02 */
  TI_LM25066A_MFR_IIN_OC_WARN_LIMIT    = 24, /* RW  2 MFR_SPECIFIC_03 */
  TI_LM25066A_MFR_PIN_OP_WARN_LIMIT    = 25, /* RW  2 MFR_SPECIFIC_04 */
  TI_LM25066A_MFR_PIN_PEAK             = 26, /* R   2 MFR_SPECIFIC_05 */
  TI_LM25066A_MFR_CLEAR_PIN_PEAK       = 27, /*  W  0 MFR_SPECIFIC_06 */
  TI_LM25066A_MFR_GATE_MASK            = 28, /* RW  1 MFR_SPECIFIC_07 */
  TI_LM25066A_MFR_ALERT_MASK           = 29, /* RW  2 MFR_SPECIFIC_08 */
  TI_LM25066A_MFR_DEVICE_SETUP         = 30, /* RW  1 MFR_SPECIFIC_09 */
  TI_LM25066A_MFR_BLOCK_READ           = 31, /* R  12 MFR_SPECIFIC_10 */
  TI_LM25066A_MFR_SAMPLES_FOR_AVG      = 32, /* RW  1 MFR_SPECIFIC_11 */
  TI_LM25066A_MFR_READ_AVG_VIN         = 33, /* R   2 MFR_SPECIFIC_12 */
  TI_LM25066A_MFR_READ_AVG_VOUT        = 34, /* R   2 MFR_SPECIFIC_13 */
  TI_LM25066A_MFR_READ_AVG_IIN         = 35, /* R   2 MFR_SPECIFIC_14 */
  TI_LM25066A_MFR_READ_AVG_PIN         = 36, /* R   2 MFR_SPECIFIC_15 */
  TI_LM25066A_MFR_BLACK_BOX_READ       = 37, /* R  12 MFR_SPECIFIC_16 */
  TI_LM25066A_MFR_DIAGNOSTIC_WORD_READ = 38, /* R   2 MFR_SPECIFIC_17 */
  TI_LM25066A_MFR_AVG_BLOCK_READ       = 39, /* R  12 MFR_SPECIFIC_18 */
} ti_lm25066a_cmd;

/*
 * Real world converters. Page 46 of the datasheet discusses reading and
 * writing telemtry data and obtaining the real world values. There are 8
 * separate conversions using the same formula.
 *
 * The formula is:
 *
 *      1        -R
 *  X = - (Y x 10  - b)
 *      m
 *
 *  X: the calculated "real world" value (volts, amps, watt, etc.)
 *  m: the slope coefficient
 *  Y: a two byte two's complement integer received from device
 *  b: the offset, a two byte two's complement integer
 *  R: the exponent, a one byte two's complement integer
 *
 * R in the table is inverted because we cannot store 0.01 in an int. This
 * makes the equation:
 *
 *      1  Y
 *  X = - (- - b)
 *      m  R
 *
 * The R value lets the integer result have decimal places.
 *
 * There are 8 conversion table entries listed in Table 41 of the
 * data sheet. They are:
 *
 * 1.    READ_VIN, READ_AVG_VIN, VIN_OV_WARN_LIMIT, VIN_UV_WARN_LIMIT
 * 2.    READ_VOUT, READ_AVG_VOUT, VOUT_UV_WARN_LIMIT
 * 3.    READ_VAUX
 * 4.GND READ_IIN, READ_AVG_IIN, MFR_IIN_OC_WARN_LIMIT
 * 4.VDD READ_IIN, READ_AVG_IIN, MFR_IIN_OC_WARN_LIMIT
 * 5.GND READ_PIN, READ_AVG_PIN, READ_PIN_PEAK, MFR_PIN_OP_WARN_LIMIT
 * 5.VCC READ_PIN, READ_AVG_PIN, READ_PIN_PEAK, MFR_PIN_OP_WARN_LIMIT
 * 6.    READ_TEMPERATURE_1, OT_WARN_LIMIT, OT_FAULT_LIMIT
 *
 * You need to provide 6 sets of conversion factors. Row 4 and 5 depend on how
 * the device is wired. The driver will use the matching table row to convert
 * the 2-complement 12 bit to a real world value.
 */
#define TI_LM25066A_CONVERSION_SIZE (6)
typedef struct
{
  int         m;    /* The slope coefficient */
  int         b;    /* The offset */
  int         R;    /* The inverted power of 10 of -R */
} ti_lm25066a_conversion;

/*
 * IO control interface.
 */
#define TI_LM25066A_GET (I2C_DEV_IO_CONTROL + 0)
#define TI_LM25066A_SET (I2C_DEV_IO_CONTROL + 1)

/*
 * IO data types.
 */
typedef enum
{
  TI_LM25066A_8BIT   = 0,
  TI_LM25066A_16BIT  = 1,
  TI_LM25066A_VALUE  = 2,
  TI_LM25066A_VALUES = 3,
  TI_LM25066A_STRING = 4,
  TI_LM25066A_RAW    = 5
} ti_lm25066a_data;

/*
 * Struct to move data into and out of the driver.
 */
typedef struct
{
  ti_lm25066a_cmd  cmd;
  ti_lm25066a_data type;
  union {
    uint8_t  u8;
    uint16_t u16;
    int      value;
    int      values[6];
    char     string[9];
    uint8_t* raw;
  } data;
}  ti_lm25066a_io;

/*
 * Register the device.
 *
 * The conversions table has 6 columns.
 *
 * The values are an integer so the decimal_point value scales the value so it
 * can fit in an integer with the required number of decimal points.
 */
int i2c_dev_register_ti_lm25066a(const char*                         bus_path,
                                 const char*                         dev_path,
                                 uint16_t                            address,
                                 const ti_lm25066a_conversion* const conversions,
                                 const int                           decimal_points);

/*
 * Get.
 */
static inline int
ti_lm25066a_get(int fd, ti_lm25066a_io* io)
{
  return ioctl(fd, TI_LM25066A_GET, io);
}

/*
 * Set.
 */
static inline int
ti_lm25066a_set(int fd, ti_lm25066a_io* io)
{
  return ioctl(fd, TI_LM25066A_SET, io);
}


#endif
