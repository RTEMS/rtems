/*  system_conf.h
 *  Global System conf
 *
 *  Milkymist port of RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#ifndef __SYSTEM_CONFIG_H_
#define __SYSTEM_CONFIG_H_

#define UART_BAUD_RATE          (115200)

/* Clock frequency */
#define MM_FREQUENCY            (0xe0001074)

/* FML bridge */
#define FMLBRG_FLUSH_BASE       (0xc8000000)
#define FMLBRG_LINE_LENGTH      (32)
#define FMLBRG_LINE_COUNT       (512)

/* UART */
#define MM_UART_RXTX            (0xe0000000)
#define MM_UART_DIV             (0xe0000004)
#define MM_UART_STAT            (0xe0000008)
#define MM_UART_CTRL            (0xe000000c)

#define UART_STAT_THRE          (0x1)
#define UART_STAT_RX_EVT        (0x2)
#define UART_STAT_TX_EVT        (0x4)

#define UART_CTRL_RX_INT        (0x1)
#define UART_CTRL_TX_INT        (0x2)
#define UART_CTRL_THRU          (0x4)

/* Timers */
#define MM_TIMER1_COMPARE       (0xe0001024)
#define MM_TIMER1_COUNTER       (0xe0001028)
#define MM_TIMER1_CONTROL       (0xe0001020)

#define MM_TIMER0_COMPARE       (0xe0001014)
#define MM_TIMER0_COUNTER       (0xe0001018)
#define MM_TIMER0_CONTROL       (0xe0001010)

#define TIMER_ENABLE            (0x01)
#define TIMER_AUTORESTART       (0x02)

/* GPIO */
#define MM_GPIO_IN              (0xe0001000)
#define MM_GPIO_OUT             (0xe0001004)
#define MM_GPIO_INTEN           (0xe0001008)

#define GPIO_BTN1               (0x00000001)
#define GPIO_BTN2               (0x00000002)
#define GPIO_BTN3               (0x00000004)
#define GPIO_PCBREV0            (0x00000008)
#define GPIO_PCBREV1            (0x00000010)
#define GPIO_PCBREV2            (0x00000020)
#define GPIO_PCBREV3            (0x00000040)
#define GPIO_LED1               (0x00000001)
#define GPIO_LED2               (0x00000002)

/* System ID and reset */
#define MM_SYSTEM_ID            (0xe000107c)

/* ICAP */
#define MM_ICAP                 (0xe0001040)

#define ICAP_READY              (0x01)
#define ICAP_CE                 (0x10000)
#define ICAP_WRITE              (0x20000)

/* VGA */
#define MM_VGA_RESET            (0xe0003000)

#define MM_VGA_HRES             (0xe0003004)
#define MM_VGA_HSYNC_START      (0xe0003008)
#define MM_VGA_HSYNC_END        (0xe000300C)
#define MM_VGA_HSCAN            (0xe0003010)

#define MM_VGA_VRES             (0xe0003014)
#define MM_VGA_VSYNC_START      (0xe0003018)
#define MM_VGA_VSYNC_END        (0xe000301C)
#define MM_VGA_VSCAN            (0xe0003020)

#define MM_VGA_BASEADDRESS      (0xe0003024)
#define MM_VGA_BASEADDRESS_ACT  (0xe0003028)

#define MM_VGA_BURST_COUNT      (0xe000302C)

#define MM_VGA_DDC              (0xe0003030)

#define MM_VGA_CLKSEL           (0xe0003034)

#define VGA_RESET               (0x01)
#define VGA_DDC_SDAIN           (0x1)
#define VGA_DDC_SDAOUT          (0x2)
#define VGA_DDC_SDAOE           (0x4)
#define VGA_DDC_SDC             (0x8)

/* Ethernet */
#define MM_MINIMAC_SETUP        (0xe0008000)
#define MM_MINIMAC_MDIO         (0xe0008004)

#define MM_MINIMAC_STATE0       (0xe0008008)
#define MM_MINIMAC_COUNT0       (0xe000800C)
#define MM_MINIMAC_STATE1       (0xe0008010)
#define MM_MINIMAC_COUNT1       (0xe0008014)

#define MM_MINIMAC_TXCOUNT      (0xe0008018)

#define MINIMAC_RX0_BASE        (0xb0000000)
#define MINIMAC_RX1_BASE        (0xb0000800)
#define MINIMAC_TX_BASE         (0xb0001000)

#define MINIMAC_SETUP_PHYRST    (0x1)

#define MINIMAC_STATE_EMPTY     (0x0)
#define MINIMAC_STATE_LOADED    (0x1)
#define MINIMAC_STATE_PENDING   (0x2)

/* AC97 */
#define MM_AC97_CRCTL           (0xe0005000)

#define AC97_CRCTL_RQEN         (0x01)
#define AC97_CRCTL_WRITE        (0x02)

#define MM_AC97_CRADDR          (0xe0005004)
#define MM_AC97_CRDATAOUT       (0xe0005008)
#define MM_AC97_CRDATAIN        (0xe000500C)

#define MM_AC97_DCTL            (0xe0005010)
#define MM_AC97_DADDRESS        (0xe0005014)
#define MM_AC97_DREMAINING      (0xe0005018)

#define MM_AC97_UCTL            (0xe0005020)
#define MM_AC97_UADDRESS        (0xe0005024)
#define MM_AC97_UREMAINING      (0xe0005028)

#define AC97_SCTL_EN            (0x01)

#define AC97_MAX_DMASIZE        (0x3fffc)

/* SoftUSB */
#define MM_SOFTUSB_CONTROL      (0xe000f000)

#define SOFTUSB_CONTROL_RESET   (0x1)

#define MM_SOFTUSB_PMEM_BASE    (0xa0000000)
#define MM_SOFTUSB_DMEM_BASE    (0xa0020000)

#define SOFTUSB_PMEM_SIZE       (1 << 13)
#define SOFTUSB_DMEM_SIZE       (1 << 13)

/* PFPU */
#define MM_PFPU_CTL             (0xe0006000)
#define PFPU_CTL_START          (0x01)
#define PFPU_CTL_BUSY           (0x01)

#define MM_PFPU_MESHBASE        (0xe0006004)
#define MM_PFPU_HMESHLAST       (0xe0006008)
#define MM_PFPU_VMESHLAST       (0xe000600C)

#define MM_PFPU_CODEPAGE        (0xe0006010)

#define MM_PFPU_DREGBASE        (0xe0006400)
#define MM_PFPU_CODEBASE        (0xe0006800)

#define PFPU_PAGESIZE           (512)
#define PFPU_SPREG_COUNT        (2)
#define PFPU_REG_X              (0)
#define PFPU_REG_Y              (1)

/* TMU */
#define MM_TMU_CTL              (0xe0007000)
#define TMU_CTL_START           (0x01)
#define TMU_CTL_BUSY            (0x01)
#define TMU_CTL_CHROMAKEY       (0x02)

#define MM_TMU_HMESHLAST        (0xe0007004)
#define MM_TMU_VMESHLAST        (0xe0007008)
#define MM_TMU_BRIGHTNESS       (0xe000700C)
#define MM_TMU_CHROMAKEY        (0xe0007010)

#define MM_TMU_VERTICESADR      (0xe0007014)
#define MM_TMU_TEXFBUF          (0xe0007018)
#define MM_TMU_TEXHRES          (0xe000701C)
#define MM_TMU_TEXVRES          (0xe0007020)
#define MM_TMU_TEXHMASK         (0xe0007024)
#define MM_TMU_TEXVMASK         (0xe0007028)

#define MM_TMU_DSTFBUF          (0xe000702C)
#define MM_TMU_DSTHRES          (0xe0007030)
#define MM_TMU_DSTVRES          (0xe0007034)
#define MM_TMU_DSTHOFFSET       (0xe0007038)
#define MM_TMU_DSTVOFFSET       (0xe000703C)
#define MM_TMU_DSTSQUAREW       (0xe0007040)
#define MM_TMU_DSTSQUAREH       (0xe0007044)

#define MM_TMU_ALPHA            (0xe0007048)

/* Memory card */
#define MM_MEMCARD_CLK2XDIV     (0xe0004000)

#define MM_MEMCARD_ENABLE       (0xe0004004)

#define MEMCARD_ENABLE_CMD_TX   (0x1)
#define MEMCARD_ENABLE_CMD_RX   (0x2)
#define MEMCARD_ENABLE_DAT_TX   (0x4)
#define MEMCARD_ENABLE_DAT_RX   (0x8)

#define MM_MEMCARD_PENDING      (0xe0004008)

#define MEMCARD_PENDING_CMD_TX  (0x1)
#define MEMCARD_PENDING_CMD_RX  (0x2)
#define MEMCARD_PENDING_DAT_TX  (0x4)
#define MEMCARD_PENDING_DAT_RX  (0x8)

#define MM_MEMCARD_START        (0xe000400c)

#define MEMCARD_START_CMD_RX    (0x1)
#define MEMCARD_START_DAT_RX    (0x2)

#define MM_MEMCARD_CMD          (0xe0004010)
#define MM_MEMCARD_DAT          (0xe0004014)

/* DMX */
#define MM_DMX_TX(x)            (0xe000c000+4*(x))
#define MM_DMX_THRU             (0xe000c800)
#define MM_DMX_RX(x)            (0xe000d000+4*(x))

/* MIDI */
#define MM_MIDI_RXTX            (0xe000b000)
#define MM_MIDI_DIV             (0xe000b004)
#define MM_MIDI_STAT            (0xe000b008)
#define MM_MIDI_CTRL            (0xe000b00c)

#define MIDI_STAT_THRE          (0x1)
#define MIDI_STAT_RX_EVT        (0x2)
#define MIDI_STAT_TX_EVT        (0x4)

#define MIDI_CTRL_RX_INT        (0x1)
#define MIDI_CTRL_TX_INT        (0x2)
#define MIDI_CTRL_THRU          (0x4)

/* IR */
#define MM_IR_RX                (0xe000e000)

/* Video input */
#define MM_BT656_I2C            (0xe000a000)
#define MM_BT656_FILTERSTATUS   (0xe000a004)
#define MM_BT656_BASE           (0xe000a008)
#define MM_BT656_MAXBURSTS      (0xe000a00c)
#define MM_BT656_DONEBURSTS     (0xe000a010)

#define BT656_I2C_SDAIN         (0x1)
#define BT656_I2C_SDAOUT        (0x2)
#define BT656_I2C_SDAOE         (0x4)
#define BT656_I2C_SDC           (0x8)

#define BT656_FILTER_FIELD1     (0x1)
#define BT656_FILTER_FIELD2     (0x2)
#define BT656_FILTER_INFRAME    (0x4)

/* Interrupts */
#define MM_IRQ_UART             (0)
#define MM_IRQ_GPIO             (1)
#define MM_IRQ_TIMER0           (2)
#define MM_IRQ_TIMER1           (3)
#define MM_IRQ_AC97CRREQUEST    (4)
#define MM_IRQ_AC97CRREPLY      (5)
#define MM_IRQ_AC97DMAR         (6)
#define MM_IRQ_AC97DMAW         (7)
#define MM_IRQ_PFPU             (8)
#define MM_IRQ_TMU              (9)
#define MM_IRQ_ETHRX            (10)
#define MM_IRQ_ETHTX            (11)
#define MM_IRQ_VIDEOIN          (12)
#define MM_IRQ_MIDI             (13)
#define MM_IRQ_IR               (14)
#define MM_IRQ_USB              (15)

/* Flash layout */
#define FLASH_BASE                      (0x80000000)

#define FLASH_OFFSET_STANDBY_BITSTREAM  (0x80000000)

#define FLASH_OFFSET_RESCUE_BITSTREAM   (0x800A0000)
#define FLASH_OFFSET_RESCUE_BIOS        (0x80220000)
#define FLASH_OFFSET_MAC_ADDRESS        (0x802200E0)
#define FLASH_OFFSET_RESCUE_SPLASH      (0x80240000)
#define FLASH_OFFSET_RESCUE_APP         (0x802E0000)

#define FLASH_OFFSET_REGULAR_BITSTREAM  (0x806E0000)
#define FLASH_OFFSET_REGULAR_BIOS       (0x80860000)
#define FLASH_OFFSET_REGULAR_SPLASH     (0x80880000)
#define FLASH_OFFSET_REGULAR_APP        (0x80920000)

/* MMIO */
#define MM_READ(reg) (*((volatile unsigned int *)(reg)))
#define MM_WRITE(reg, val) *((volatile unsigned int *)(reg)) = val

/* Flash partitions */

#define FLASH_SECTOR_SIZE     (128*1024)

#define FLASH_PARTITION_COUNT (5)

#define FLASH_PARTITIONS { \
  { .start_address = 0x806E0000, .length = 0x0180000 }, \
  { .start_address = 0x80860000, .length = 0x0020000 }, \
  { .start_address = 0x80880000, .length = 0x00A0000 }, \
  { .start_address = 0x80920000, .length = 0x0400000 }, \
  { .start_address = 0x80D20000, .length = 0x12E0000 }, \
}

#endif /* __SYSTEM_CONFIG_H_ */
