/*===============================================================*\
| Project: display driver for HCMS29xx                            |
+-----------------------------------------------------------------+
| File: disp_hcms29xx.c                                           |
+-----------------------------------------------------------------+
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
| http://www.rtems.com/license/LICENSE.                           |
+-----------------------------------------------------------------+
| this file contains the SPI based driver for a HCMS29xx 4 digit  |
| alphanumeric LED display                                        |
\*===============================================================*/

#include <string.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp.h>
#include <rtems/libi2c.h>
#include <libchip/disp_hcms29xx.h>
#include "font_hcms29xx.h"
#define FONT_BASE font_hcms29xx_base


#define DISP_HCMS29XX_DIGIT_CNT (4)
#define DISP_HCMS29XX_SEMA_NAME      rtems_build_name('D','4','I','Q')
#define DISP_HCMS29XX_TRNS_SEMA_NAME rtems_build_name('D','4','T','R')
#define DISP_HCMS29XX_TIMER_NAME     rtems_build_name('D','4','T','M')
#define DISP_HCMS29XX_TASK_NAME      rtems_build_name('D','4','T','A')

#define DISP_HCMS29XX_EVENT_TIMER  RTEMS_EVENT_1
#define DISP_HCMS29XX_EVENT_NEWSTR RTEMS_EVENT_2


static disp_font_t disp_hcms29xx_font_normal;
static disp_font_t disp_hcms29xx_font_rotate;
const rtems_libi2c_tfr_mode_t spi_disphcms29xx_tfr_mode = {
  .baudrate = 1000000,
  .bits_per_char = 8,
  .lsb_first = true,
  .clock_inv = true,
  .clock_phs = true,
  .idle_char = 0
};

static disp_hcms29xx_drv_t disp_hcms29xx_drv_tbl;

/*=========================================
 * font management functions
 */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_font_struct_size
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   compute size of font data structure tree                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  disp_font_t            src,           /* source font                     */
  size_t                *dst_size       /* destination: size of font struct*/
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;
  size_t font_size = 0;
  size_t glyph_idx;
  /*
   * check parameters
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      (src == NULL)) {
    rc = RTEMS_INVALID_ADDRESS;
  }
  if (rc == RTEMS_SUCCESSFUL) {
    font_size =
      sizeof(*src);                      /* font_base structure       */
  }
  glyph_idx = 0;
  while ((rc == RTEMS_SUCCESSFUL) &&
	 (glyph_idx < (sizeof(src->latin1)/sizeof(src->latin1[0])))) {
    if (src->latin1[glyph_idx] != NULL) {
      font_size += sizeof(*(src->latin1[glyph_idx]))
	+ (size_t) src->latin1[glyph_idx]->bb.w;
    }
    glyph_idx++;
  }
  *dst_size = font_size;

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static inline unsigned char disp_hcms29xx_bitswap
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   swap data bits in byte (7<->0 , 6<->1 etc)                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  unsigned char byte
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  unsigned char result = 0;
  int smsk,dmsk;
  for (smsk =  0x01,dmsk=0x80;
       smsk < 0x100;
       smsk<<=1   ,dmsk>>=1) {
    if ((byte & smsk) != 0) {
      result |= (unsigned char) dmsk;
    }
  }
  return result;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_copy_font
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   copy font data from source to dest font structure                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  disp_font_t src,                      /* source font                     */
  struct disp_font_base *dst,           /* ptr to destination font         */
  int shift_cnt,                        /* shift count for font            */
  bool do_rotate                        /* rotate font, if true            */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;
  char *alloc_next = (char *)dst;
  size_t glyph_idx = 0;
  int glyph_size;
  unsigned char byte;
  int bcnt;

  /*
   * check parameters
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((src == NULL) ||
       (dst == NULL))) {
    rc = RTEMS_INVALID_ADDRESS;
  }
  /*
   * copy font_base structure
   */
  if (rc == RTEMS_SUCCESSFUL) {
    *dst = *src;
    alloc_next += sizeof(*dst);
  }
  /*
   * for all glyphs: assign own glyph memory
   */
  glyph_idx = 0;
  while ((rc == RTEMS_SUCCESSFUL) &&
	 glyph_idx < (sizeof(src->latin1)/sizeof(src->latin1[0]))) {
    if (src->latin1[glyph_idx] != NULL) {
      /*
       * allocate space for glyph
       */
      dst->latin1[glyph_idx] = (struct disp_font_glyph *)alloc_next;
      alloc_next += sizeof(*(dst->latin1[glyph_idx]));
      /*
       * copy source values.
       * Note: bitmap will be reassigned later
       */
      *(struct disp_font_glyph *)(dst->latin1[glyph_idx]) =
	*(src->latin1[glyph_idx]);
    }
    else {
      dst->latin1[glyph_idx] = NULL;
    }
    glyph_idx++;
  }

  /*
   * for all glyphs: reassign bitmap
   */
  glyph_idx = 0;
  while ((rc == RTEMS_SUCCESSFUL) &&
	 glyph_idx < (sizeof(src->latin1)/sizeof(src->latin1[0]))) {
    if (src->latin1[glyph_idx] != NULL) {
      glyph_size = src->latin1[glyph_idx]->bb.w;
      /*
       * allocate space for glyph_bitmap
       */
      dst->latin1[glyph_idx]->bitmap = (const unsigned char *) alloc_next;
      alloc_next += glyph_size;
      /*
       * copy/transform bitmap
       */
      for (bcnt = 0;bcnt < glyph_size;bcnt++) {
	if (do_rotate) {
	  byte = src->latin1[glyph_idx]->bitmap[glyph_size - 1 - bcnt];
	  byte = disp_hcms29xx_bitswap(byte);
	}
	else {
	  byte = src->latin1[glyph_idx]->bitmap[bcnt];
	}
	if (shift_cnt < 0) {
	  byte = byte >> shift_cnt;
	}
	else if (shift_cnt > 0) {
	  byte = byte >> shift_cnt;
	}
	((unsigned char *)(dst->latin1[glyph_idx]->bitmap))[bcnt] = byte;
      }
    }
    glyph_idx++;
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_alloc_copy_font
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   copy font data from source to dest font structure, alloc all data       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  const disp_font_t src,                /* source font                     */
  disp_font_t *dst,                     /* ptr to destination font         */
  int shift_cnt,                        /* shift count for font            */
  bool do_rotate                        /* rotate font, if true            */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  rtems_status_code rc = RTEMS_SUCCESSFUL;
  size_t src_size = 0;
  /*
   * check parameters
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      ((src == NULL)
       || (dst == NULL))) {
    rc = RTEMS_INVALID_ADDRESS;
  }
  /*
   * determine size of source data
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_font_struct_size(src,&src_size);
  }
  /*
   * allocate proper data area
   */
  if (rc == RTEMS_SUCCESSFUL) {
    *dst = malloc(src_size);
    if (*dst == NULL) {
      rc = RTEMS_UNSATISFIED;
    }
  }
  /*
   * scan through source data, copy to dest
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_copy_font(src,*dst,shift_cnt,do_rotate);
  }
  return rc;
}

/*=========================================
 * SPI communication functions
 */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_send_to_display
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   request access semaphore to SPI, prepare buffer descriptors, start      |
|   transfer via SPI to display                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
   disp_hcms29xx_drv_t *softc_ptr,
   const volatile char *disp_buffer /* start of chars to display (4 chars or 'til \0)*/
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  bool char_avail;
  const struct disp_font_glyph *glyph_ptr;
  disp_font_t curr_font;
  int i,digit,ret_cnt;
  unsigned char c;

  /*
   * select device, set transfer mode, address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_start(softc_ptr->disp_param.minor);
  }
  /*
   * set transfer mode
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = -rtems_libi2c_ioctl(softc_ptr->disp_param.minor,
			     RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
			     &spi_disphcms29xx_tfr_mode);
  }

  /*
   * address device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_addr(softc_ptr->disp_param.minor,true);
  }

  /*
   * send data
   */
  if (rc == RTEMS_SUCCESSFUL) {
    curr_font =
      softc_ptr->disp_param.rotate
      ? disp_hcms29xx_font_rotate
      : disp_hcms29xx_font_normal;

    char_avail = true;
    /*
     * FIXME: for rotated display, write last character first...
     * maybe we should copy everything to a common buffer and use
     * ONE SPI transfer?
     */
    for (i = 0;
	 ((rc == RTEMS_SUCCESSFUL) &&
	  (i < DISP_HCMS29XX_DIGIT_CNT));
	  i++) {
      /* test for end of string... */
      c = disp_buffer[i]; /* perform consistent read of disp_buffer */
      if (char_avail && (c == '\0')) {
	char_avail = false;
      }
      glyph_ptr = (char_avail
		   ? curr_font->latin1[c]
		   : NULL);
      if (glyph_ptr == NULL) {
	glyph_ptr = curr_font->latin1[' '];
      }

      digit = (softc_ptr->disp_param.rotate
	       ? DISP_HCMS29XX_DIGIT_CNT-1-i
	       : i);
      /*
       * send 5 bytes from (char *)glyph_ptr->bitmap to SPI
       */
      if (rc == RTEMS_SUCCESSFUL) {
	ret_cnt = rtems_libi2c_write_bytes(softc_ptr->disp_param.minor,
					   glyph_ptr->bitmap,5);
	if (ret_cnt < 0) {
	  rc = -ret_cnt;
	}
      }
    }
  }
  /*
   * finish transfer
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_stop(softc_ptr->disp_param.minor);
  }

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_send_to_control
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   request access semaphore to SPI, prepare buffer descriptors, start      |
|   transfer via SPI to display                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
   disp_hcms29xx_drv_t *softc_ptr,
   int pwm,   /* value for pwm of LEDs, 0..15 */
   int peak,  /* value for peak current for LEDs, 0..3 */
   int sleep, /* value to make display "sleep" (0..1 */
   int div,   /* divider for external osc input, unused here */
   int chain  /* mode to drive other displays, unused here */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  int run, ret_cnt;
  uint8_t ctrl_buffer;

  /* two accesses, control word 0 and 1 */
  for (run = 0;
       ((rc == RTEMS_SUCCESSFUL) && (run <= 1));
       run++) {
    if (rc == RTEMS_SUCCESSFUL) {
      if (run == 0) {
	ctrl_buffer =
	  (0              << 7) |
	  ((sleep & 0x01) << 6) |
	  ((peak  & 0x03) << 4) |
	  ((pwm & 0x0f)   << 0);
      }
      else {
	ctrl_buffer =
	  (1              << 7) |
	  ((div   & 0x01) << 1) |
	  ((chain & 0x01) << 0);
      }
      /*
       * select device, set transfer mode, address device
       */
      if (rc == RTEMS_SUCCESSFUL) {
	rc = rtems_libi2c_send_start(softc_ptr->disp_param.minor);
      }
      /*
       * set transfer mode
       */
      if (rc == RTEMS_SUCCESSFUL) {
	rc = -rtems_libi2c_ioctl(softc_ptr->disp_param.minor,
				 RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
				 &spi_disphcms29xx_tfr_mode);
      }

      /*
       * address device
       */
      if (rc == RTEMS_SUCCESSFUL) {
	rc = rtems_libi2c_send_addr(softc_ptr->disp_param.minor,true);
      }

      /*
       * send 1 byte from ctrl_buffer
       */
      if (rc == RTEMS_SUCCESSFUL) {
	ret_cnt = rtems_libi2c_write_bytes(softc_ptr->disp_param.minor,
					   &ctrl_buffer,1);
	if (ret_cnt < 0) {
	  rc = -ret_cnt;
	}
      }
    }
  } /* next run ... */

  /*
   * finish transfer
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_libi2c_send_stop(softc_ptr->disp_param.minor);
  }

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_timer_service_routine disp_hcms29xx_timer_sr
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   this task updates the string in the display                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 rtems_id id, /* ID of timer, not used  */
 void * arg   /* calling arg: softc_ptr */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none used>                                                            |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  disp_hcms29xx_drv_t *softc_ptr = arg;


  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_event_send(softc_ptr->disp_param.task_id,
			  DISP_HCMS29XX_EVENT_TIMER);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_task disp_hcms29xx_update_task
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   this task updates the string in the display                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
   rtems_task_argument argument 
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <never exits>                                                          |
\*=========================================================================*/
{
  rtems_event_set  my_events;
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  int disp_offset = 0;
  rtems_id disp_hcms29xx_timer_id;
  disp_hcms29xx_drv_t *softc_ptr = &disp_hcms29xx_drv_tbl;

  /*
   * initialize display:
   */
  /*
   * set control attributes for display
   * maximum brightness...
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_send_to_control(softc_ptr,
				       14,3,1,0,0);/* pwm/peak/nosleep/div/chain */
  }

  /*
   * set display to blank
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_send_to_display(softc_ptr,
				       "");
  }

  /*
   * create timer for scrolling
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_timer_create(DISP_HCMS29XX_TIMER_NAME,
			    &disp_hcms29xx_timer_id);
  }

  while (rc == RTEMS_SUCCESSFUL) {
    /*
     * wait for any event
     */
    rc = rtems_event_receive(DISP_HCMS29XX_EVENT_NEWSTR |
			     DISP_HCMS29XX_EVENT_TIMER ,
			     RTEMS_WAIT | RTEMS_EVENT_ANY,
			     RTEMS_NO_TIMEOUT,
			     &my_events);
    if (my_events & DISP_HCMS29XX_EVENT_NEWSTR) {
      /*
       * fetch new string consistently into local buffer
       */
      if (rc == RTEMS_SUCCESSFUL) {
	rc = rtems_semaphore_obtain(softc_ptr->disp_param.trns_sema_id,
				    RTEMS_WAIT,RTEMS_NO_TIMEOUT);
      }
      if (rc == RTEMS_SUCCESSFUL) {
	strncpy(softc_ptr->disp_param.disp_buffer,
		softc_ptr->disp_param.trns_buffer,
		sizeof(softc_ptr->disp_param.disp_buffer));
	softc_ptr->disp_param.disp_buffer[sizeof(softc_ptr->disp_param.disp_buffer)-1] = '\0';
	softc_ptr->disp_param.disp_buf_cnt =
	  (int) strlen(softc_ptr->disp_param.disp_buffer);
      }
      if (rc == RTEMS_SUCCESSFUL) {
	rc = rtems_semaphore_release(softc_ptr->disp_param.trns_sema_id);
      }
      /*
       * set initial offset to negative value
       * to make string static for some ticks
       */
      disp_offset = -4;
    }
    if (my_events & DISP_HCMS29XX_EVENT_TIMER) {
      /*
       * increase disp_offset, if possible, otherwise reset it
       */
      if ((disp_offset < 0) ||
	  (disp_offset < softc_ptr->disp_param.disp_buf_cnt-
	   DISP_HCMS29XX_DIGIT_CNT/2)) {
	disp_offset++;
      }
      else {
	disp_offset = -4;
      }
    }
    /*
     * display string, starting from disp_offset
     */
    if (disp_offset < 0) {
      rc = disp_hcms29xx_send_to_display(softc_ptr,
					 softc_ptr->disp_param.disp_buffer);
    }
    else if (disp_offset
	     < (softc_ptr->disp_param.disp_buf_cnt - DISP_HCMS29XX_DIGIT_CNT)) {
      rc = disp_hcms29xx_send_to_display(softc_ptr,
					 softc_ptr->disp_param.disp_buffer+disp_offset);
    }
    else {
      rc = disp_hcms29xx_send_to_display(softc_ptr,
					 softc_ptr->disp_param.disp_buffer
					 + softc_ptr->disp_param.disp_buf_cnt
					 - DISP_HCMS29XX_DIGIT_CNT);
    }
    /*
     * activate timer, if needed
     */
    if (rc == RTEMS_SUCCESSFUL) {
      if (softc_ptr->disp_param.disp_buf_cnt > DISP_HCMS29XX_DIGIT_CNT) {
	rc = rtems_timer_fire_after(disp_hcms29xx_timer_id,
				    50,
				    disp_hcms29xx_timer_sr,
				    NULL);
      }
      else {
	rc = rtems_timer_cancel(disp_hcms29xx_timer_id);
      }
    }
  }
  /*
   * FIXME: display task is dead...
   */
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code disp_hcms29xx_update
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   move given string to display task                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
   disp_hcms29xx_drv_t *softc_ptr,
   const char *src
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /*
   * obtain trns semaphore
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_semaphore_obtain(softc_ptr->disp_param.trns_sema_id,
				RTEMS_WAIT,RTEMS_NO_TIMEOUT);
  }
  /*
   * copy string...
   */
  strncpy(softc_ptr->disp_param.trns_buffer,src,
	  sizeof(softc_ptr->disp_param.trns_buffer));
  softc_ptr->disp_param.trns_buffer[sizeof(softc_ptr->disp_param.trns_buffer)-1] = '\0';

  /*
   * release trns semaphore
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_semaphore_release(softc_ptr->disp_param.trns_sema_id);
  }

  /*
   * send event to task
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_event_send(softc_ptr->disp_param.task_id,
			  DISP_HCMS29XX_EVENT_NEWSTR);
  }

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_device_driver disp_hcms29xx_dev_initialize
  (
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   prepare the display device driver to accept write calls                 |
|   register device with its name                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
/*
 * Initialize and register the device
 */
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  disp_hcms29xx_drv_t *softc_ptr = &disp_hcms29xx_drv_tbl;

  /*
   * initialize font management
   * FIXME: check, that default glyph exists
   * FIXME: check font size to be 5x7
   */
  /*
   * translate font according to direction/baseline
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_alloc_copy_font(
			       &FONT_BASE,
			       &disp_hcms29xx_font_normal,
			       FONT_BASE.descent, /* shift to visibility... */
			       FALSE); /* do not rotate */
  }
  /* FIXME: translate font for rotation */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = disp_hcms29xx_alloc_copy_font(&FONT_BASE,
			       &disp_hcms29xx_font_rotate,
			       0, /* do not shift */
			       true); /* rotate font */
  }
  /*
   * create the trns_buffer semaphore
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_semaphore_create (DISP_HCMS29XX_TRNS_SEMA_NAME,1,
				     RTEMS_PRIORITY
				     |RTEMS_BINARY_SEMAPHORE
				     |RTEMS_INHERIT_PRIORITY
				     |RTEMS_NO_PRIORITY_CEILING
				     |RTEMS_LOCAL,
				     0,
				     &softc_ptr->disp_param.trns_sema_id);
  }

  /*
   * create and start display task
   */
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_task_create(DISP_HCMS29XX_TASK_NAME,
			   20,
			   RTEMS_MINIMUM_STACK_SIZE,
			   RTEMS_INTERRUPT_LEVEL(0) | RTEMS_TIMESLICE,
			   RTEMS_DEFAULT_ATTRIBUTES,
			   &softc_ptr->disp_param.task_id);
  }
  if (rc == RTEMS_SUCCESSFUL) {
    rc = rtems_task_start(softc_ptr->disp_param.task_id,
			  disp_hcms29xx_update_task,0);
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_device_driver disp_hcms29xx_dev_open
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   open the display device                                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  disp_hcms29xx_drv_t *softc_ptr = &disp_hcms29xx_drv_tbl;
  /*
   * ensure, that disp_hcms29xx device is assumed to be empty
   */
  softc_ptr->disp_param.dev_buf_cnt = 0;

  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_device_driver disp_hcms29xx_dev_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write to display device                                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  rtems_libio_rw_args_t *args = arg;
  uint32_t cnt;
  disp_hcms29xx_drv_t *softc_ptr = &disp_hcms29xx_drv_tbl;

  for (cnt = 0;cnt < args->count;cnt++) {
    /*
     * accumulate characters written into display dev buffer
     */
    if (((softc_ptr->disp_param.dev_buf_cnt > 0)
	&&((args->buffer[cnt] == '\n')
	   || (args->buffer[cnt] == '\0'))
	 )
	||( softc_ptr->disp_param.dev_buf_cnt >=
	    (int) sizeof(softc_ptr->disp_param.dev_buffer) - 1)) {
      softc_ptr->disp_param.dev_buffer[softc_ptr->disp_param.dev_buf_cnt] = '\0';
      /*
       * transfer string to display string, redisplay it...
       */
      disp_hcms29xx_update(softc_ptr,softc_ptr->disp_param.dev_buffer);
      softc_ptr->disp_param.dev_buf_cnt = 0;
    }
    /*
     * write to dev_buf, if '\n' occured or display device buffer is full
     */
    if ((args->buffer[cnt] != '\n') &&
	(args->buffer[cnt] != '\0')) {
      softc_ptr->disp_param.dev_buffer[softc_ptr->disp_param.dev_buf_cnt++] =
	args->buffer[cnt];
    }
  }
  args->bytes_moved = args->count;

  return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_device_driver disp_hcms29xx_dev_close
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   close the display device                                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{

  return RTEMS_SUCCESSFUL;
}

/*
 * driver operation tables
 */
static rtems_driver_address_table disp_hcms29xx_ops = {
  .initialization_entry = disp_hcms29xx_dev_initialize,
  .open_entry =           disp_hcms29xx_dev_open,
  .write_entry =          disp_hcms29xx_dev_write,
  .close_entry =          disp_hcms29xx_dev_close
};


static disp_hcms29xx_drv_t disp_hcms29xx_drv_tbl = {
  {/* public fields */
    .ops =         &disp_hcms29xx_ops, 
    .size =        sizeof (disp_hcms29xx_drv_t),
  },
  { /* our private fields */
    0,
    { 0 },
    0,
    { 0 },
    { 0 },
    0,
    0,
    0,
    false
  }
};

rtems_libi2c_drv_t *disp_hcms29xx_driver_descriptor = 
  &disp_hcms29xx_drv_tbl.libi2c_drv_entry;

