/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * TI ADS1113 and ADS1115.
 *   http://www.ti.com/product/ads1113/description
 */

#ifndef TI_ADS1113_ADS1115_H
#define TI_ADS1113_ADS1115_H

#include <dev/i2c/i2c.h>

/*
 * Supported devices. Please others once tested.
 */
typedef enum {
  TI_ADS1113,
  TI_ADS1114,
  TI_ADS1115
} ti_ads_adc;

/*
 * Multiplexer interface. Avalable on ADS1115.
 */
typedef enum {
  TI_ADS_MUX_ApA0_AnA1  = 0,    /* default */
  TI_ADS_MUX_ApA0_AnA3  = 1,
  TI_ADS_MUX_ApA1_AnA3  = 2,
  TI_ADS_MUX_ApA2_AnA3  = 3,
  TI_ADS_MUX_ApA0_AnGND = 4,
  TI_ADS_MUX_ApA1_AnGND = 5,
  TI_ADS_MUX_ApA2_AnGND = 6,
  TI_ADS_MUX_ApA3_AnGND = 7
} ti_ads_adc_mux;

/*
 * Programmable Gain Amplifier. Avalable on ADS1114 and ADS1115.
 */
typedef enum {
  TI_ADS_PGA_FS_6_144V   = 0,
  TI_ADS_PGA_FS_4_096V   = 1,
  TI_ADS_PGA_FS_2_048V   = 2,    /* default */
  TI_ADS_PGA_FS_1_024V   = 3,
  TI_ADS_PGA_FS_0_512V   = 4,
  TI_ADS_PGA_FS_0_256V   = 5,
  TI_ADS_PGA_FS_0_256V_2 = 6,
  TI_ADS_PGA_FS_0_256V_3 = 7,
} ti_ads_adc_pga;

/*
 * Mode.
 */
typedef enum {
  TI_ADS_MODE_CONTINUOUS  = 0,
  TI_ADS_MODE_SINGLE_SHOT = 1,    /* default */
} ti_ads_adc_mode;

/*
 * Data rate.
 */
typedef enum {
  TI_ADS_DATARATE_8SPS   = 0,
  TI_ADS_DATARATE_16SPS  = 1,
  TI_ADS_DATARATE_32SPS  = 2,
  TI_ADS_DATARATE_64SPS  = 3,
  TI_ADS_DATARATE_128SPS = 4,    /* default */
  TI_ADS_DATARATE_250SPS = 5,
  TI_ADS_DATARATE_475SPS = 6,
  TI_ADS_DATARATE_860SPS = 7,
} ti_ads_adc_data_rate;

/*
 * Comparitor interface. Avalable on ADS1114 and ADS1115.
 *
 * Create a value to write.
 */
#define TI_ADS_COMP_MODE_HYSTERESIS  (0 << 4)    /* default */
#define TI_ADS_COMP_MODE_WINDOW      (1 << 4)
#define TI_ADS_COMP_POL_ACTIVE_LOW   (0 << 3)    /* default */
#define TI_ADS_COMP_POL_ACTIVE_HIGH  (1 << 3)
#define TI_ADS_COMP_LAT_NON_LATCHING (0 << 2)    /* default */
#define TI_ADS_COMP_LAT_LATCHING     (1 << 2)
#define TI_ADS_COMP_QUE_DISABLE_COMP (3 << 0)    /* default */
#define TI_ADS_COMP_QUE_AFTER_4      (2 << 0)
#define TI_ADS_COMP_QUE_AFTER_2      (1 << 0)
#define TI_ADS_COMP_QUE_AFTER_1      (0 << 0)

/*
 * IO control interface.
 *
 * Note: if in Power-down single-shot mode (default) a conversion requires the
 *       device to power up and perform a conversion and this can take
 *       while. The TI_ADS_ADC_GET_CONV_WAIT call sets the micro-seconds to
 *       wait between polls. The actual rate will depend on the system tick.
 */
#define TI_ADS_ADC_GET_CONVERSION  (I2C_DEV_IO_CONTROL + 0)
#define TI_ADS_ADC_SET_MUX         (I2C_DEV_IO_CONTROL + 1)
#define TI_ADS_ADC_SET_PGA         (I2C_DEV_IO_CONTROL + 2)
#define TI_ADS_ADC_SET_MODE        (I2C_DEV_IO_CONTROL + 3)
#define TI_ADS_ADC_SET_DATA_RATE   (I2C_DEV_IO_CONTROL + 4)
#define TI_ADS_ADC_SET_COMP        (I2C_DEV_IO_CONTROL + 5)
#define TI_ADS_ADC_SET_LO_THRESH   (I2C_DEV_IO_CONTROL + 6)
#define TI_ADS_ADC_SET_HI_THRESH   (I2C_DEV_IO_CONTROL + 7)
#define TI_ADS_ADC_SET_CONV_WAIT   (I2C_DEV_IO_CONTROL + 8)

/*
 * Register the device.
 */
int i2c_dev_register_ti_ads_adc(const char* bus_path,
                                const char* dev_path,
                                uint16_t    address,
                                ti_ads_adc  device);

/*
 * Perform a conversion. If the mode is single shot a single shot conversion is
 * started and the call waits for the conversion to complete.
 */
static inline int
ti_ads_adc_convert(int fd, uint16_t* sample)
{
  return ioctl(fd, TI_ADS_ADC_GET_CONVERSION, sample);
}

/*
 * Set the multipler.
 */
static inline int
ti_ads_adc_set_mux(int fd, ti_ads_adc_mux mux)
{
  return ioctl(fd, TI_ADS_ADC_SET_MUX, (void *)(uintptr_t) mux);
}

/*
 * Set the PGA.
 */
static inline int
ti_ads_adc_set_pga(int fd, ti_ads_adc_pga pga)
{
  return ioctl(fd, TI_ADS_ADC_SET_PGA, (void *)(uintptr_t) pga);
}

/*
 * Set the mode.
 */
static inline int
ti_ads_adc_set_mode(int fd, ti_ads_adc_mode mode)
{
  return ioctl(fd, TI_ADS_ADC_SET_MODE, (void *)(uintptr_t) mode);
}

/*
 * Set the data rate.
 */
static inline int
ti_ads_adc_set_data_rate(int fd, ti_ads_adc_data_rate rate)
{
  return ioctl(fd, TI_ADS_ADC_SET_DATA_RATE, (void *)(uintptr_t) rate);
}

/*
 * Configure the comparator.
 */
static inline int
ti_ads_adc_set_comparator(int fd, uint16_t comp)
{
  return ioctl(fd, TI_ADS_ADC_SET_COMP, (void *)(uintptr_t) comp);
}

/*
 * Set the lower threshold.
 */
static inline int
ti_ads_adc_set_low_threshold(int fd, uint16_t level)
{
  return ioctl(fd, TI_ADS_ADC_SET_LO_THRESH, (void *)(uintptr_t) level);
}

/*
 * Set the upper threshold.
 */
static inline int
ti_ads_adc_set_high_threshold(int fd, uint16_t level)
{
  return ioctl(fd, TI_ADS_ADC_SET_HI_THRESH, (void *)(uintptr_t) level);
}

/*
 * Set the conversion poll wait period.
 */
static inline int
ti_ads_adc_set_conversion_poll_wait(int fd, uint32_t micro_seconds)
{
  return ioctl(fd, TI_ADS_ADC_SET_CONV_WAIT, (void *)(uintptr_t) micro_seconds);
}

#endif
