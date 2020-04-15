/*
 * Copyright (c) 2016-2016 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <dev/i2c/i2c.h>
#include <dev/i2c/ti-ads-16bit-adc.h>

/*
 * Registers.
 */
#define ADS_CONVERSION (0)
#define ADS_CONFIG     (1)
#define ADS_LO_THRESH  (2)
#define ADS_HI_THRESH  (3)

/*
 * Configuration register.
 */
#define CFG_OS_NOT_CONVERTING     (1 << 15)   /* read */
#define CFG_OS_START_SSHOT        (1 << 15)   /* write */
#define CFG_MUX_BASE              (12)
#define CFG_MUX_MASK              (7)
#define CFG_PGA_BASE              (9)
#define CFG_PGA_MASK              (7)
#define CFG_MODE_BASE             (8)
#define CFG_MODE_MASK             (1)
#define CFG_DATA_RATE_BASE        (5)
#define CFG_DATA_RATE_MASK        (7)
#define CFG_COMP_BASE             (0)
#define CFG_COMP_MASK             (0x1f)
#define CFG_COMP_MODE_ACTIVE_LOW  (0 << 4)
#define CFG_COMP_MODE_ACTIVE_HIGH (1 << 4)

#define CFG_MODE_CONT             (0 << CFG_MODE_BASE)
#define CFG_MODE_SSHOT            (1 << CFG_MODE_BASE)

typedef struct {
  i2c_dev    base;
  ti_ads_adc device;
  uint32_t   poll_wait_period;
  int        reg;
  uint16_t   config_shadow;
} ti_ads;

static int
ti_ads_reg_write(ti_ads* dev, int reg, uint16_t value)
{
  uint8_t out[3];
  i2c_msg msgs[1] = {
    {
      .addr = dev->base.address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }
  };
  out[0] = (uint8_t) reg;
  out[1] = (uint8_t) (value >> 8);
  out[2] = (uint8_t) value;
  return i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
}

static int
ti_ads_reg_read(ti_ads* dev, int reg, uint16_t* value)
{
  uint8_t in[2] = { 0, 0 };
  uint8_t out[1] = { (uint8_t) reg };
  i2c_msg msgs[2] = {
    {
      .addr = dev->base.address,
      .flags = 0,
      .len = (uint16_t) sizeof(out),
      .buf = &out[0]
    }, {
      .addr = dev->base.address,
      .flags = I2C_M_RD,
      .len = (uint16_t) sizeof(in),
      .buf = &in[0]
    }
  };
  int err;
  err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  *value = (((uint16_t) in[0]) << 8) | in[1];
  return err;
}

static int
ti_ads_set_config(ti_ads* dev, uint16_t value, int base, uint16_t mask)
{
  int err;
  dev->config_shadow &= ~(mask << base);
  dev->config_shadow |= (value & mask) << base;
  err = ti_ads_reg_write(dev, ADS_CONFIG, dev->config_shadow);
  return err;
}

static int
ti_ads_sample(ti_ads* dev, uint16_t* value)
{
  int err;
  if ((dev->config_shadow & CFG_MODE_SSHOT) == CFG_MODE_SSHOT) {
    i2c_bus_obtain(dev->base.bus);
    err = ti_ads_reg_write(dev,
                           ADS_CONFIG,
                           dev->config_shadow | CFG_OS_START_SSHOT);
    if (err == 0) {
      uint16_t config = 0;
      while (err == 0 && (config & CFG_OS_NOT_CONVERTING) == 0) {
        err = ti_ads_reg_read(dev, ADS_CONFIG, &config);
        if (dev->poll_wait_period && (config & CFG_OS_NOT_CONVERTING) == 0)
          usleep(dev->poll_wait_period);
      }
      err = ti_ads_reg_read(dev, ADS_CONVERSION, value);
    }
    i2c_bus_release(dev->base.bus);
  }
  else {
    err = ti_ads_reg_read(dev, ADS_CONVERSION, value);
  }
  return err;
}

static int
ti_ads_ioctl(i2c_dev* iic_dev, ioctl_command_t command, void* arg)
{
  ti_ads*  dev = (ti_ads*) iic_dev;
  uint16_t value;
  int      err;

  switch (command) {
    case TI_ADS_ADC_GET_CONVERSION:
      value = 0;
      err = ti_ads_sample(dev, &value);
      if (err == 0)
        *((uint16_t*) arg) = value;
      break;
    case TI_ADS_ADC_SET_MUX:
      if (dev->device == TI_ADS1115) {
        value = (uint16_t)(uintptr_t) arg;
        err = ti_ads_set_config(dev, value, CFG_MUX_BASE, CFG_MUX_MASK);
      }
      else {
        err = -ENOTTY;
      }
      break;
    case TI_ADS_ADC_SET_MODE:
      value = (uint16_t)(uintptr_t) arg;
      err = ti_ads_set_config(dev, value, CFG_MODE_BASE, CFG_MODE_MASK);
      break;
    case TI_ADS_ADC_SET_PGA:
      if (dev->device == TI_ADS1114 || dev->device == TI_ADS1115) {
        value = (uint16_t)(uintptr_t) arg;
        err = ti_ads_set_config(dev, value, CFG_PGA_BASE, CFG_PGA_MASK);
      }
      else {
        err = -ENOTTY;
      }
      break;
    case TI_ADS_ADC_SET_COMP:
      if (dev->device == TI_ADS1114 || dev->device == TI_ADS1115) {
        value = (uint16_t)(uintptr_t) arg;
        err = ti_ads_set_config(dev, value, CFG_COMP_BASE, CFG_COMP_MASK);
      }
      else {
        err = -ENOTTY;
      }
      break;
    case TI_ADS_ADC_SET_LO_THRESH:
      value = (uint16_t)(uintptr_t) arg;
      err = ti_ads_reg_write(dev, ADS_LO_THRESH, value);
      break;
    case TI_ADS_ADC_SET_HI_THRESH:
      value = (uint16_t)(uintptr_t) arg;
      err = ti_ads_reg_write(dev, ADS_HI_THRESH, value);
      break;
    case TI_ADS_ADC_SET_CONV_WAIT:
      dev->poll_wait_period = (uint32_t)(uintptr_t) arg;
      err = 0;
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int
i2c_dev_register_ti_ads_adc(const char* bus_path,
                            const char* dev_path,
                            uint16_t    address,
                            ti_ads_adc  device)
{
  ti_ads* dev;

  dev = (ti_ads*)
    i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) {
    return -1;
  }

  dev->base.ioctl = ti_ads_ioctl;
  dev->device = device;
  dev->config_shadow = 0;

  switch (device) {
    default:
      errno = EIO;
      return -1;
    case TI_ADS1115:
      dev->config_shadow |= ((TI_ADS_MUX_ApA0_AnA1 << CFG_MUX_BASE) |
                             (TI_ADS_PGA_FS_2_048V << CFG_PGA_BASE) |
                             ((TI_ADS_COMP_MODE_HYSTERESIS |
                               TI_ADS_COMP_POL_ACTIVE_LOW |
                               TI_ADS_COMP_LAT_NON_LATCHING |
                               TI_ADS_COMP_QUE_DISABLE_COMP) << CFG_COMP_BASE));
      /* FALLTHRU */
    case TI_ADS1114:
      dev->config_shadow |= TI_ADS_PGA_FS_2_048V << CFG_PGA_BASE;
      /* FALLTHRU */
    case TI_ADS1113:
      dev->config_shadow |= (CFG_MODE_SSHOT |
                             (TI_ADS_DATARATE_8SPS  << CFG_DATA_RATE_BASE));
      break;
  }

  return i2c_dev_register(&dev->base, dev_path);
}
