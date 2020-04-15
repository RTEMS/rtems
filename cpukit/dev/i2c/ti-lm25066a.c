/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include <dev/i2c/i2c.h>
#include <dev/i2c/ti-lm25066a.h>

/*
 * Commands.
 *
 * The commands are listed in Table 1 in the datasheet. These are the mapped to
 * command ids in the intreface. Commands have different data size, a mix of
 * read and write amd formats.
 *
 * The formats for values is listed in Table 41 in the datasheet.
 */

#define CMD_RD         (1 << 0)
#define CMD_WR         (1 << 1)
#define CMD_RW         (CMD_RD | CMD_WR)
#define CMD_STR        (1 << 2)
#define CMD_VAL_       (1 << 3)
#define CMD_BLK_       (1 << 4)
#define CMD_INDEX_BASE (16UL)
#define CMD_INDEX_MASK (0xfUL)
#define CMD_VAL(l)     (CMD_VAL_ | (((uint32_t) (l)) << CMD_INDEX_BASE))
#define CMD_BLK(l)     (CMD_BLK_ | (((uint32_t) (l)) << CMD_INDEX_BASE))
#define CMD_INDEX(f)   ((((uint32_t) (f)) >> CMD_INDEX_BASE) & CMD_INDEX_MASK)

/*
 * Number of bits in the ADC.
 */
#define ADC_BITS (12)

/*
 * Blocks of values that can be read.
 */
static const ti_lm25066a_cmd block_read[] =
{
  TI_LM25066A_MFR_DIAGNOSTIC_WORD_READ,
  TI_LM25066A_MFR_READ_IIN,
  TI_LM25066A_READ_VOUT,
  TI_LM25066A_READ_VIN,
  TI_LM25066A_MFR_READ_PIN,
  TI_LM25066A_READ_TEMPERATURE_1
};

static const ti_lm25066a_cmd avg_block_read[] =
{
  TI_LM25066A_MFR_DIAGNOSTIC_WORD_READ,
  TI_LM25066A_MFR_READ_AVG_IIN,
  TI_LM25066A_MFR_READ_AVG_VOUT,
  TI_LM25066A_MFR_READ_AVG_VIN,
  TI_LM25066A_MFR_READ_AVG_PIN,
  TI_LM25066A_READ_TEMPERATURE_1
};

static const ti_lm25066a_cmd* blocks[] =
{
  block_read,
  avg_block_read
};

typedef struct
{
  uint8_t  cmd;
  uint16_t size;
  uint32_t flags;
} ti_lm25066a_i2c_cmd;

/*
 * Table 1.
 */
static const ti_lm25066a_i2c_cmd commands[] =
{
  { 0x01,  1, CMD_RW              }, /* OPERATION */
  { 0x03,  0, CMD_WR              }, /* CLEAR_FAULTS */
  { 0x19,  1, CMD_RD              }, /* CAPABILITY */
  { 0x43,  2, CMD_RW | CMD_VAL(1) }, /* VOUT_UV_WARN_LIMIT */
  { 0x4f,  2, CMD_RW | CMD_VAL(5) }, /* OT_FAULT_LIMIT */
  { 0x51,  2, CMD_RW | CMD_VAL(5) }, /* OT_WARN_LIMIT */
  { 0x57,  2, CMD_RD | CMD_VAL(0) }, /* VIN_OV_WARN_LIMIT */
  { 0x58,  2, CMD_RW | CMD_VAL(0) }, /* VIN_UV_WARN_LIMIT */
  { 0x78,  1, CMD_RD              }, /* STATUS_BYTE */
  { 0x79,  2, CMD_RD              }, /* STATUS_WORD */
  { 0x7a,  1, CMD_RD              }, /* STATUS_VOUT */
  { 0x7c,  1, CMD_RD              }, /* STATUS_INPUT */
  { 0x7d,  1, CMD_RD              }, /* STATUS_TEMPERATURE */
  { 0x7e,  1, CMD_RD              }, /* STATUS_CML */
  { 0x80,  1, CMD_RD              }, /* STATUS_MFR_SPECIFIC */
  { 0x88,  2, CMD_RD | CMD_VAL(0) }, /* READ_VIN */
  { 0x8b,  2, CMD_RD | CMD_VAL(1) }, /* READ_VOUT */
  { 0x8d,  2, CMD_RD | CMD_VAL(5) }, /* READ_TEMPERATURE_1 */
  { 0x99,  4, CMD_RD | CMD_STR    }, /* MFR_ID */
  { 0x9a,  9, CMD_RD | CMD_STR    }, /* MFR_MODEL */
  { 0x9b,  2, CMD_RD | CMD_STR    }, /* MFR_REVISION */
  { 0xd0,  2, CMD_RD | CMD_VAL(2) }, /* MFR_READ_VAUX */
  { 0xd1,  2, CMD_RD | CMD_VAL(3) }, /* MFR_READ_IIN */
  { 0xd2,  2, CMD_RD | CMD_VAL(4) }, /* MFR_READ_PIN */
  { 0xd3,  2, CMD_RW | CMD_VAL(3) }, /* MFR_IIN_OC_WARN_LIMIT */
  { 0xd4,  2, CMD_RW | CMD_VAL(4) }, /* MFR_PIN_OP_WARN_LIMIT */
  { 0xd5,  2, CMD_RD | CMD_VAL(4) }, /* MFR_PIN_PEAK */
  { 0xd6,  0, CMD_WR              }, /* MFR_CLEAR_PIN_PEAK */
  { 0xd7,  1, CMD_RW              }, /* MFR_GATE_MASK */
  { 0xd8,  2, CMD_RW              }, /* MFR_ALERT_MASK */
  { 0xd9,  1, CMD_RD              }, /* MFR_DEVICE_SETUP */
  { 0xda, 13, CMD_RD | CMD_BLK(0) }, /* MFR_BLOCK_READ */
  { 0xdb,  1, CMD_RW              }, /* MFR_SAMPLES_FOR_AVG */
  { 0xdc,  2, CMD_RD | CMD_VAL(0) }, /* MFR_READ_AVG_VIN */
  { 0xdd,  2, CMD_RD | CMD_VAL(1) }, /* MFR_READ_AVG_VOUT */
  { 0xde,  2, CMD_RD | CMD_VAL(3) }, /* MFR_READ_AVG_IIN */
  { 0xdf,  2, CMD_RD | CMD_VAL(4) }, /* MFR_READ_AVG_PIN */
  { 0xe0, 13, CMD_RD | CMD_BLK(0) }, /* MFR_BLACK_BOX_READ */
  { 0xe1,  2, CMD_RD              }, /* MFR_DIAGNOSTIC_WORD_READ */
  { 0xe2, 13, CMD_RD | CMD_BLK(1) }, /* MFR_AVG_BLOCK_READ */
};

#define IO_CMDS          (sizeof(commands) / sizeof(commands[0]))
#define IO_INTS          (6)
#define IO_MESSAGE_SIZE  ((sizeof(uint16_t) * IO_INTS) + 2)

typedef struct {
  i2c_dev                       base;
  uint8_t                       pointer;
  uint16_t                      config_shadow;
  const ti_lm25066a_conversion* conversions;
  int                           scale;
  uint8_t                       buffer[IO_MESSAGE_SIZE];
} ti_lm25066a;

/*
 * Convert a value using table 41 in the data sheet.
 */
static int
ti_lm25066a_io_convert_to_real(ti_lm25066a* dev, uint16_t word, int conversion)
{
  const ti_lm25066a_conversion* const con = &dev->conversions[conversion];
  uint32_t                            u = word;
  int                                 value;
  value = ((int) (u & ((1 << ADC_BITS) - 1))) * dev->scale;
  value = ((value * con->R) - con->b) / con->m;
  return value;
}

static void
ti_lm25066a_io_convert_block(ti_lm25066a*    dev,
                             const uint16_t* words,
                             int*            values,
                             int             block)
{
  const ti_lm25066a_cmd* cmds = blocks[block];
  int                    c;
  /*
   * A block is always 6 values.
   */
  values[0] = words[0];
  for (c = 1; c < 6; ++c) {
    const ti_lm25066a_i2c_cmd* cmd = &commands[cmds[c]];
    if ((cmd->flags & CMD_VAL_) != 0) {
      values[c] =
        ti_lm25066a_io_convert_to_real(dev, words[c], CMD_INDEX(cmd->flags));
    } else {
      values[c] = words[c];
    }
  }
}

static int
ti_lm25066a_io_read(ti_lm25066a* dev, ti_lm25066a_io* io)
{
  const ti_lm25066a_i2c_cmd* cmd;
  uint8_t                    out[1];
  uint8_t*                   in = dev->buffer;
  i2c_msg                    msgs[2];
  int                        err;

  if (io->cmd >= IO_CMDS)
    return -EIO;

  cmd = &commands[io->cmd];

  if ((cmd->flags & CMD_RD) == 0)
    return -EIO;

  out[0] = cmd->cmd;
  msgs[0].addr = dev->base.address;
  msgs[0].flags = 0;
  msgs[0].len = (uint16_t) sizeof(out);
  msgs[0].buf = &out[0];
  msgs[1].addr = dev->base.address;
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = (uint16_t) cmd->size;
  msgs[1].buf = &in[0];

  err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  if (err != 0)
    return err;

  err = -EIO;

  switch (io->type) {
    case TI_LM25066A_8BIT:
      io->data.u8 = in[0];
      err = 0;
      break;
    case TI_LM25066A_16BIT:
      io->data.u16 = (((uint16_t) in[0]) << 8) | in[1];
      err = 0;
      break;
    case TI_LM25066A_VALUE:
      if ((cmd->flags & CMD_VAL_) != 0) {
        uint16_t data = ((uint16_t) in[1]) << 8 | in[0];
        io->data.value =
          ti_lm25066a_io_convert_to_real(dev, data, CMD_INDEX(cmd->flags));
      }
      else {
        io->data.value = ((uint16_t) in[1]) << 8 | in[0];;
      }
      err = 0;
      break;
    case TI_LM25066A_VALUES:
      if ((cmd->flags & CMD_BLK_) != 0) {
        uint16_t  data[in[0] / 2];
        uint16_t* d = &data[0];
        uint8_t*  i = &in[1];
        int       w;
        for (w = 0; w < (in[0] / 2); ++w, ++d, i += 2) {
          *d = (((uint16_t) i[1]) << 8) | i[0] ;
        }
        ti_lm25066a_io_convert_block(dev,
                                     &data[0],
                                     &io->data.values[0],
                                     CMD_INDEX(cmd->flags));
        err = 0;
      }
      break;
    case TI_LM25066A_STRING:
      memcpy(&io->data.string[0], &in[0], cmd->size);
      err = 0;
      break;
    case TI_LM25066A_RAW:
      memcpy(io->data.raw, &in[0], cmd->size);
      err = 0;
      break;
    default:
      break;
  }

  return err;
}

static int
ti_lm25066a_io_write(ti_lm25066a* dev, ti_lm25066a_io* io)
{
  const ti_lm25066a_i2c_cmd* cmd;
  uint8_t*                   out = dev->buffer;
  uint16_t                   length = 0;
  int                        err;

  if (io->cmd >= IO_CMDS)
    return -EIO;

  cmd = &commands[io->cmd];

  if ((cmd->flags & CMD_WR) == 0)
    return -EIO;

  out[0] = cmd->cmd;

  if (cmd->size == 0) {
    out[1] = 0;
    length = 1;
  }
  else {
    switch (io->type) {
      case TI_LM25066A_8BIT:
        out[1] = io->data.u8;
        length = 1 + 1;
        break;
      case TI_LM25066A_16BIT:
        out[1] = io->data.u16 >> 8;
        out[2] = io->data.u16;
        length = 2 + 1;
        break;
      case TI_LM25066A_VALUE:
        break;
      case TI_LM25066A_VALUES:
        break;
      case TI_LM25066A_STRING:
        break;
      case TI_LM25066A_RAW:
        memcpy(&out[1], io->data.raw, cmd->size);
        length = cmd->size + 1;
        err = 0;
      break;
      default:
        break;
    }
  }

  if (length == 0)
    err = -EIO;
  else {
    i2c_msg msgs[1] = {
      {
        .addr = dev->base.address,
        .flags = 0,
        .len = length,
        .buf = &out[0]
      }
    };
    err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
  }

  return err;
}

static int
ti_lm25066a_ioctl(i2c_dev* iic_dev, ioctl_command_t command, void* arg)
{
  ti_lm25066a*    dev = (ti_lm25066a*) iic_dev;
  ti_lm25066a_io* io = (ti_lm25066a_io*) arg;
  int        err;

  switch (command) {
    case TI_LM25066A_GET:
      err = ti_lm25066a_io_read(dev, io);
      break;
    case TI_LM25066A_SET:
      err = ti_lm25066a_io_write(dev, io);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  return err;
}

int
i2c_dev_register_ti_lm25066a(const char*                         bus_path,
                             const char*                         dev_path,
                             uint16_t                            address,
                             const ti_lm25066a_conversion* const conversions,
                             const int                           decimal_points)
{
  ti_lm25066a* dev;
  int          i;

  dev = (ti_lm25066a*)
    i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) {
    return -1;
  }

  dev->base.ioctl = ti_lm25066a_ioctl;
  dev->pointer = -1;
  dev->config_shadow = 0;
  dev->conversions = conversions;
  dev->scale = 1;
  for (i = 0; i < decimal_points; ++i)
    dev->scale *= 10;

  return i2c_dev_register(&dev->base, dev_path);
}
