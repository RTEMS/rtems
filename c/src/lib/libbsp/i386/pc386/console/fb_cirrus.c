/*
 *  FB driver for Cirrus GD5446 graphic hardware.
 *  Tested to be compatible with QEMU GD5446 emulation but not on real HW.
 *
 *  Copyright (c) 2012 - Alexandru-Sever Horin (alex.sever.h@gmail.com).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  The code is based on next information sources:
 *    - CL-GD5446 Technical Reference Manual, 1996, Second Edition
 *    - RTEMS fb_vga.c - Rosimildo da Silva ( rdasilva@connecttel.com )
 *    - Cirrus xf86 driver - used as VGA hardware setup sequence documentation
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <rtems/pci.h>

#include <rtems/fb.h>
#include <rtems/framebuffer.h>

/* mutex to limit driver to protect against multiple opens */
static pthread_mutex_t cirrus_mutex = PTHREAD_MUTEX_INITIALIZER;

/* screen information for the VGA driver
 * standard structures
 */
static struct fb_var_screeninfo fb_var;
static struct fb_fix_screeninfo fb_fix;

#define CIRRUS_VENDOR_ID         0x1013
#define CIRRUS_GD5446_DEVICE_ID  0x00b8

typedef struct _DisplayModeRec {
  struct _DisplayModeRec *prev;
  struct _DisplayModeRec *next;
  char *name;                 /* identifier for the mode */
  int type;

  /* These are the values that the user sees/provides */
  int Clock;                  /* pixel clock freq (kHz) */
  int HDisplay;               /* horizontal timing */
  int HSyncStart;
  int HSyncEnd;
  int HTotal;
  int HSkew;
  int VDisplay;               /* vertical timing */
  int VSyncStart;
  int VSyncEnd;
  int VTotal;
  int VScan;
  int Flags;

  /* These are the values the hardware uses */
  int ClockIndex;
  int SynthClock;             /* Actual clock freq to
   * be programmed  (kHz) */
  int CrtcHDisplay;
  int CrtcHBlankStart;
  int CrtcHSyncStart;
  int CrtcHSyncEnd;
  int CrtcHBlankEnd;
  int CrtcHTotal;
  int CrtcHSkew;
  int CrtcVDisplay;
  int CrtcVBlankStart;
  int CrtcVSyncStart;
  int CrtcVSyncEnd;
  int CrtcVBlankEnd;
  int CrtcVTotal;
  int CrtcHAdjusted;
  int CrtcVAdjusted;
  int PrivSize;
  int32_t *Private;
  int PrivFlags;

  float HSync, VRefresh;
} DisplayModeRec, *DisplayModePtr;

static DisplayModeRec available_modes[] = {
    {
        .Clock      = 31500 ,
        .HDisplay   = 640 ,
        .HSyncStart = 664 ,
        .HSyncEnd   = 704 ,
        .HTotal     = 832 ,
        .HSkew      = 0 ,
        .VDisplay   = 480 ,       /* vertical timing */
        .VSyncStart = 489 ,
        .VSyncEnd   = 491 ,
        .VTotal     = 520 ,
        .VScan      = 0,
        .Flags      = 0
    },
    {
        .Clock      = 40000 ,
        .HDisplay   = 800 ,
        .HSyncStart = 840 ,
        .HSyncEnd   = 968 ,
        .HTotal     = 1056 ,
        .HSkew      = 0 ,
        .VDisplay   = 600 ,       /* vertical timing */
        .VSyncStart = 601 ,
        .VSyncEnd   = 605 ,
        .VTotal     = 628 ,
        .VScan      = 0,
        .Flags      = 0
    },
};
static DisplayModePtr active_mode;

/* The display mode used for the board hardcoded in the following define
 * Index in above structure
 */
#define CIRRUS_DISPLAY_MODE  0

/* The display bytes per pixel used for the board hardcoded in the following define
 * Index in above structure
 */
#define CIRRUS_DEFAULT_BPP 24

/* cirrus board information */
struct cirrus_board_str{
  int    pci_bus;
  int    pci_device;
  int    pci_function;
  void  *reg_base;
};

static struct cirrus_board_str cirrus_board_info;

/*
 * get information from the board
 */
static int
cirrus_pci_read( struct cirrus_board_str *cirrus_board, uint32_t *mem_base, uint32_t *cirrus_register_base)
{
  int r;

  r = pci_read_config_dword(
      cirrus_board->pci_bus, cirrus_board->pci_device, cirrus_board->pci_function,
      PCI_BASE_ADDRESS_0, mem_base);
  if( r != PCIB_ERR_SUCCESS)
    return RTEMS_UNSATISFIED;

  r = pci_read_config_dword(
      cirrus_board->pci_bus, cirrus_board->pci_device, cirrus_board->pci_function,
      PCI_BASE_ADDRESS_1, cirrus_register_base);
  if( r != PCIB_ERR_SUCCESS)
    return RTEMS_UNSATISFIED;

  *mem_base &= PCI_BASE_ADDRESS_MEM_MASK;
  *cirrus_register_base     &= PCI_BASE_ADDRESS_MEM_MASK;

  return RTEMS_SUCCESSFUL;
}

static inline int
fb_cirrus_read_config_dword(
    struct cirrus_board_str *fbst,
    unsigned char where,
    uint32_t     *pval)
{
  return pci_read_config_dword(
      fbst->pci_bus, fbst->pci_device, fbst->pci_function,
      where, pval);
}

static inline int
fb_cirrus_write_config_dword(
    struct cirrus_board_str *fbst,
    unsigned char where,
    uint32_t     val)
{
  return pci_write_config_dword(
      fbst->pci_bus, fbst->pci_device, fbst->pci_function,
      where, val);
}

static inline void
fb_cirrus_write_reg8 (
    const struct cirrus_board_str *fbst,
    unsigned int reg,
    unsigned int val)
{
  *(volatile uint8_t*)((char *)fbst->reg_base + reg) = val;
}

static inline unsigned int
fb_cirrus_read_reg8 (
    const struct cirrus_board_str *fbst,
    unsigned int reg)
{
  return *(volatile uint8_t*)((char *)fbst->reg_base + reg);
}

#define SEQ_INDEX 0x04
#define SEQ_DATA 0x05

static inline void
fb_cirrus_write_seq_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg,
    unsigned int val)
{
  fb_cirrus_write_reg8(fbst, SEQ_INDEX, reg);
  fb_cirrus_write_reg8(fbst, SEQ_DATA, val);
}

static inline unsigned int
fb_cirrus_read_seq_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg)
{
  fb_cirrus_write_reg8(fbst, SEQ_INDEX, reg);
  return fb_cirrus_read_reg8(fbst, SEQ_DATA);
}

#define CRT_INDEX 0x14
#define CRT_DATA 0x15

static inline void
fb_cirrus_write_crt_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg,
    unsigned int val)
{
  fb_cirrus_write_reg8(fbst, CRT_INDEX, reg);
  fb_cirrus_write_reg8(fbst, CRT_DATA, val);
}

static inline unsigned int
fb_cirrus_read_crt_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg)
{
  fb_cirrus_write_reg8(fbst, CRT_INDEX, reg);
  return fb_cirrus_read_reg8(fbst, CRT_DATA);
}

#define GDC_INDEX 0x0E
#define GDC_DATA 0x0F

static inline void
fb_cirrus_write_gdc_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg,
    unsigned int val)
{
  fb_cirrus_write_reg8(fbst, GDC_INDEX, reg);
  fb_cirrus_write_reg8(fbst, GDC_DATA, val);
}

static inline unsigned int
fb_cirrus_read_gdc_reg (
    const struct cirrus_board_str *fbst,
    unsigned int reg)
{
  fb_cirrus_write_reg8(fbst, GDC_INDEX, reg);
  return fb_cirrus_read_reg8(fbst, GDC_DATA);
}

#define VGA_DAC_MASK 0x06

static inline void
fb_cirrus_write_hdr_reg (
    const struct cirrus_board_str *fbst,
    unsigned int val)
{
  volatile unsigned int dummy __attribute__((unused));
  dummy = fb_cirrus_read_reg8(fbst, VGA_DAC_MASK);
  dummy = fb_cirrus_read_reg8(fbst, VGA_DAC_MASK);
  dummy = fb_cirrus_read_reg8(fbst, VGA_DAC_MASK);
  dummy = fb_cirrus_read_reg8(fbst, VGA_DAC_MASK);
  fb_cirrus_write_reg8(fbst, VGA_DAC_MASK, val);
}

/* Functionality to support multiple VGA frame buffers can be added easily,
 * but is not supported at this moment because there is no need for two or
 * more "classic" VGA adapters.  Multiple frame buffer drivers may be
 * implemented and If we had implement it they would be named as "/dev/fb0",
 * "/dev/fb1", "/dev/fb2" and so on.
 */

/*
 * fb_cirrus device driver INITIALIZE entry point.
 */
rtems_device_driver
frame_buffer_initialize(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  rtems_status_code status;
  int res;

  printk( "FB_CIRRUS -- driver initializing..\n" );

  res = pci_find_device(
      CIRRUS_VENDOR_ID,
      CIRRUS_GD5446_DEVICE_ID,
      minor,
      &cirrus_board_info.pci_bus,
      &cirrus_board_info.pci_device,
      &cirrus_board_info.pci_function
  );

  if ( res != PCIB_ERR_SUCCESS ) {
      printk( "FB_CIRRUS initialize -- device not found\n" );

      return RTEMS_UNSATISFIED;
  }
  else{
      printk( "FB_CIRRUS -- driver initializing..\n" );
      /*
       * Register the device
       */
      status = rtems_io_register_name (FRAMEBUFFER_DEVICE_0_NAME, major, 0);
      if (status != RTEMS_SUCCESSFUL) {
          printk("Error registering " FRAMEBUFFER_DEVICE_0_NAME
                 " FB_CIRRUS framebuffer device!\n");
          rtems_fatal_error_occurred( status );
      }

      return RTEMS_SUCCESSFUL;
  }
}

/*
 * This function is used to initialize the Start Address - the first
 * displayed location in the video memory.
 * Usually mandatory
 */
static void
cirrus_adjust_frame( struct cirrus_board_str *board, int x, int y)
{
  uint32_t Base;
  uint8_t tmp;

  Base = ((y * fb_var.xres + x) >> 3);
  if (fb_var.bits_per_pixel != 1)
    Base *= (fb_var.bits_per_pixel >> 2);

  printk("FB_CIRRUS: cirrus_adjust_frame %d %d >>> %d %x\n", x, y, Base, Base);

  if ((Base & ~0x000FFFFF) != 0) {
      printk("FB_CIRRUS: Internal error: cirrus_adjust_frame: cannot handle overflow\n");
      return;
  }

  fb_cirrus_write_crt_reg( board,  0x0C, (Base >> 8) & 0xff);
  fb_cirrus_write_crt_reg( board,  0x0D, Base & 0xff);

  tmp = fb_cirrus_read_crt_reg( board,  0x1B);
  tmp &= 0xF2;
  tmp |= (Base >> 16) & 0x01;
  tmp |= (Base >> 15) & 0x0C;
  fb_cirrus_write_crt_reg( board,  0x1B, tmp);

  tmp = fb_cirrus_read_crt_reg( board, 0x1D);
  tmp &= 0x7F;
  tmp |= (Base >> 12) & 0x80;
  fb_cirrus_write_crt_reg( board,  0x1D, tmp);
}

static int
cirrus_set_mode(DisplayModePtr mode)
{
  int depthcode = fb_var.bits_per_pixel;;
  int width;
  int HDiv2 = 0, VDiv2 = 0;
  const struct cirrus_board_str *cirrus_board_ptr = &cirrus_board_info;
  int temp;
  int hdr = -1;

  printk("FB_CIRRUS: mode  %d bpp, %d Hz    %d %d %d %d   %d %d %d %d\n",
      fb_var.bits_per_pixel,
      mode->Clock,
      mode->HDisplay,
      mode->HSyncStart,
      mode->HSyncEnd,
      mode->HTotal,
      mode->VDisplay,
      mode->VSyncStart,
      mode->VSyncEnd,
      mode->VTotal);

  if ( mode->Clock >  85500 ) {
      /* The actual DAC register value is set later. */
      /* The CRTC is clocked at VCLK / 2, so we must half the */
      /* horizontal timings. */
      if (!mode->CrtcHAdjusted) {
          mode->HDisplay >>= 1;
          mode->HSyncStart >>= 1;
          mode->HTotal >>= 1;
          mode->HSyncEnd >>= 1;
          mode->SynthClock >>= 1;
          mode->CrtcHAdjusted = TRUE;
      }
      depthcode += 64;
      HDiv2 = 1;
  }
  if (mode->VTotal >= 1024 ) {
      /* For non-interlaced vertical timing >= 1024, the vertical timings */
      /* are divided by 2 and VGA CRTC 0x17 bit 2  is set. */
      if (!mode->CrtcVAdjusted) {
          mode->VDisplay >>= 1;
          mode->VSyncStart >>= 1;
          mode->VSyncEnd >>= 1;
          mode->VTotal >>= 1;
          mode->CrtcVAdjusted = TRUE;
      }
      VDiv2 = 1;
  }

  /****************************************************
   * Sequential registers
   */
  fb_cirrus_write_seq_reg(cirrus_board_ptr, 0x00, 0x00);
  fb_cirrus_write_seq_reg(cirrus_board_ptr, 0x01, 0x01);
  fb_cirrus_write_seq_reg(cirrus_board_ptr, 0x02, 0x0F);
  fb_cirrus_write_seq_reg(cirrus_board_ptr, 0x03, 0x00);
  fb_cirrus_write_seq_reg(cirrus_board_ptr, 0x04, 0x0E);

  /****************************************************
   * CRTC Controller Registers
   */
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x00, (mode->HTotal >> 3) - 5 );
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x01, (mode->HDisplay >> 3) - 1);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x02, (mode->HSyncStart >> 3) - 1);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x03, ((mode->HSyncEnd >> 3) & 0x1F) | 0x80);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x04, (mode->HSyncStart >> 3));
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x05,
      (((mode->HSyncEnd >> 3) & 0x20 ) << 2 )
      | (((mode->HSyncEnd >> 3)) & 0x1F));
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x06, (mode->VTotal - 2) & 0xFF);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x07,
      (((mode->VTotal -2) & 0x100) >> 8 )
      | (((mode->VDisplay -1) & 0x100) >> 7 )
      | ((mode->VSyncStart & 0x100) >> 6 )
      | (((mode->VSyncStart) & 0x100) >> 5 )
      | 0x10
      | (((mode->VTotal -2) & 0x200)   >> 4 )
      | (((mode->VDisplay -1) & 0x200) >> 3 )
      | ((mode->VSyncStart & 0x200) >> 2 ));
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x08, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x09, ((mode->VSyncStart & 0x200) >>4) | 0x40);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0A, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0B, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0C, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0D, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0E, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x0F, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x10, mode->VSyncStart & 0xFF);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x11, (mode->VSyncEnd & 0x0F) | 0x20);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x12, (mode->VDisplay -1) & 0xFF);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x13, 0x00);  /* no interlace */
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x14, 0x00);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x15, mode->VSyncStart & 0xFF);
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x16, (mode->VSyncStart +1) & 0xFF);

  temp = 0xAF;
  if(VDiv2)
    temp |= 0x04;
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x17, temp);

  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x18, 0xFF);

  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x1A ,
      (((mode->HTotal >> 3) & 0xC0 ) >> 2)
      | (((mode->VTotal - 2) & 0x300 ) >> 2));

  width = fb_fix.line_length >> 3;
  if (fb_var.bits_per_pixel == 1)
    width <<= 2;
  if(width >= 0xFF)
    printk("FB_CIRRUS: Warning line size over the limit ... reduce bpp or width resolution");
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x13,  width);
  /* Offset extension (see CR13) */
  temp = fb_cirrus_read_crt_reg( cirrus_board_ptr, 0x1B);
  temp &= 0xAF;
  temp |= (width >> (3+4)) & 0x10;
  temp |= (width >> (3+3)) & 0x40;
  temp |= 0x22;
  fb_cirrus_write_crt_reg( cirrus_board_ptr, 0x1B,  temp);

  /****************************************************
   * Sequential register
   * Enable linear mode and high-res packed pixel mode
   */
  temp = fb_cirrus_read_seq_reg( cirrus_board_ptr, 0x07);
  temp &= 0xe0;
  switch (depthcode) {
  case 1:
  case 4:
    temp |= 0x10;
    break;
  case 8:
    temp |= 0x11;
    break;
  case 64+8:
  temp |= 0x17;
  break;
  case 15:
    temp |= 0x17;
    hdr = 0xC0; /* 5:5:5 Sierra */
    break;
  case 16:
    temp |= 0x17;
    hdr = 0xC1; /* 5:6:5 XGA mode */
    break;
  case 24:
    temp |= 0x15;
    hdr = 0xC5; /* 8:8:8 16M colors */
    break;
  case 32:
    temp |= 0x19;
    hdr = 0xC5; /* 8:8:8 16M colors */
    break;
  default:
    printk("FB_CIRRUS: Cannot Initialize display to requested mode\n");
    printk("FB_CIRRUS: returning RTEMS_UNSATISFIED on depthcode %d\n", depthcode);
    return RTEMS_UNSATISFIED;
  }
  fb_cirrus_write_seq_reg( cirrus_board_ptr, 0x07, temp);
  /* this just set  packed pixel mode with according bpp */

  /****************************************************
   * HDR Register
   */
  if(hdr > 0)
    fb_cirrus_write_hdr_reg( cirrus_board_ptr, hdr);

  /****************************************************
   * Graphic Data Controller Registers
   */
  temp = fb_cirrus_read_gdc_reg( cirrus_board_ptr, 0x12);
  if (HDiv2)
    temp |= 0x20;
  else
    temp &= ~0x20;
  fb_cirrus_write_gdc_reg( cirrus_board_ptr, 0x12, temp);

  /* Enable high-color modes */
  fb_cirrus_write_gdc_reg(cirrus_board_ptr, 0x05, 0x40);

  /* VGA graphics mode */
  fb_cirrus_write_gdc_reg(cirrus_board_ptr, 0x06, 0x01);

  return TRUE;
}

static void
cirrus_prepare_mode( void )
{

  active_mode = &available_modes[CIRRUS_DISPLAY_MODE];

  fb_var.bits_per_pixel = CIRRUS_DEFAULT_BPP;

  fb_var.xres  = active_mode->HDisplay;
  fb_var.yres  = active_mode->VDisplay;

  fb_fix.line_length = (fb_var.xres * fb_var.bits_per_pixel + 7) / 8;

  fb_fix.type   = FB_TYPE_PACKED_PIXELS;
  fb_fix.visual = FB_VISUAL_TRUECOLOR;

}

/*
 * fb_cirrus device driver OPEN entry point
 */
rtems_device_driver
frame_buffer_open(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  int r;
  uint32_t smem_start, regs_start;

  if (pthread_mutex_trylock(&cirrus_mutex)!= 0){
      printk( "FB_CIRRUS could not lock cirrus_mutex\n" );

      return RTEMS_UNSATISFIED;
  }

  r = cirrus_pci_read(&cirrus_board_info, &smem_start, &regs_start);
  if ( r == RTEMS_UNSATISFIED )
    return RTEMS_UNSATISFIED;

  fb_fix.smem_start  = (volatile char *)smem_start;
  fb_fix.smem_len    = 0x1000000;
  cirrus_board_info.reg_base = (void *)regs_start;

  cirrus_prepare_mode();

  cirrus_set_mode( active_mode );

  cirrus_adjust_frame( &cirrus_board_info, 0, 0);

  if (1) {
      uint32_t pixmask;
      int x, y;

      if(fb_var.bits_per_pixel == 32)
        pixmask = 0xffffff;
      else
        pixmask = (1 << fb_var.bits_per_pixel) - 1;

      printk("FB_CIRRUS: mode set, test patter output\n");

      for(y = 0; y < fb_var.yres; y++) {
          for(x = 0; x < fb_var.xres; x++) {
              uint32_t color;
              char *addr = (char *)fb_fix.smem_start;
              addr += y * fb_fix.line_length;
              addr += x * fb_var.bits_per_pixel / 8;
              color = x & 1 ? 0 : y & 1 ? pixmask & 0x000ff00f : pixmask;
              if(y == fb_var.yres - 1) {
                  if((x > 0) && (x < fb_var.xres-1))
                    color = pixmask & 0x00555555;
              }
              switch (fb_var.bits_per_pixel) {
              case 8:  *(volatile uint8_t*) addr = color;
              break;
              case 16: *(volatile uint16_t*) addr = color;
              break;
              case 24: *(volatile uint32_t*) addr =
                  (*(volatile uint32_t*) addr & 0xff000000) | color;
              break;
              case 32: *(volatile uint32_t*) addr = color;
              break;
              }
          }
      }
  }

  return RTEMS_SUCCESSFUL;

}

/*
 * fb_cirrus device driver CLOSE entry point
 */
rtems_device_driver
frame_buffer_close(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  if (pthread_mutex_unlock(&cirrus_mutex) == 0){
      /* restore previous state.  for VGA this means return to text mode.
       * leave out if graphics hardware has been initialized in
       * frame_buffer_initialize() */

      /* VGA text mode */
      fb_cirrus_write_gdc_reg(&cirrus_board_info, 0x06, 0x00);

      printk( "FB_CIRRUS: close called.\n" );
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}

/*
 * fb_cirrus device driver READ entry point.
 */
rtems_device_driver
frame_buffer_read(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy(rw_args->buffer, (const void *) (fb_fix.smem_start + rw_args->offset), rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

/*
 * frame_buffer device driver WRITE entry point.
 */
rtems_device_driver
frame_buffer_write(
    rtems_device_major_number  major,
    rtems_device_minor_number  minor,
    void                      *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  rw_args->bytes_moved = ((rw_args->offset + rw_args->count) > fb_fix.smem_len ) ? (fb_fix.smem_len - rw_args->offset) : rw_args->count;
  memcpy( (void *) (fb_fix.smem_start + rw_args->offset), rw_args->buffer, rw_args->bytes_moved);
  return RTEMS_SUCCESSFUL;
}

static int
get_fix_screen_info( struct fb_fix_screeninfo *info )
{
  *info = fb_fix;
  return 0;
}

static int
get_var_screen_info( struct fb_var_screeninfo *info )
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

  printk( "FB_CIRRUS ioctl called, cmd=%x\n", args->command  );

  switch( args->command ) {
  case FBIOGET_FSCREENINFO:
    args->ioctl_return =  get_fix_screen_info( ( struct fb_fix_screeninfo * ) args->buffer );
    break;
  case FBIOGET_VSCREENINFO:
    args->ioctl_return =  get_var_screen_info( ( struct fb_var_screeninfo * ) args->buffer );
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
