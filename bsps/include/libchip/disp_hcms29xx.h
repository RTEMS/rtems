/*===============================================================*\
| Project: display driver for HCMS29xx                            |
+-----------------------------------------------------------------+
| File: disp_hcms29xx.h                                           |
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
| http://www.rtems.org/license/LICENSE.                           |
+-----------------------------------------------------------------+
| this file declares the SPI based driver for a HCMS29xx 4 digit  |
| alphanumeric LED display                                        |
\*===============================================================*/

#ifndef _DISP_HCMS29XX_H
#define _DISP_HCMS29XX_H
#include <rtems.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
#define DISP_HCMS29XX_TEXT_CNT (128)

  typedef struct {
    rtems_device_minor_number minor;   /* minor device number            */
    /*
     * in the disp_buffer, the string to be displayed is placed
     */
    char disp_buffer[DISP_HCMS29XX_TEXT_CNT];
    int  disp_buf_cnt; /* number of valid chars in disp_buffer */
    /*
     * in the trns buffer the string is transfered to display task
     */
    char trns_buffer[DISP_HCMS29XX_TEXT_CNT];
    /*
     * in the dev_buffer, characters will be accumulated before display...
     */
    char dev_buffer[DISP_HCMS29XX_TEXT_CNT];
    int  dev_buf_cnt; /* number of valid chars in dev_buffer */

    rtems_id trns_sema_id;  /* ID of disp trns buffer sema   */
    rtems_id task_id;       /* ID of disp task               */
    bool rotate;            /* FLAG: display is upside down       */
  } spi_disp_hcms29xx_param_t;

  typedef struct {
    rtems_libi2c_drv_t        libi2c_drv_entry;
    spi_disp_hcms29xx_param_t disp_param;
  } disp_hcms29xx_drv_t;
  /*
   * pass this descriptor pointer to rtems_libi2c_register_drv
   */
  extern rtems_libi2c_drv_t *disp_hcms29xx_driver_descriptor;

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
   );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

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
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

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
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

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
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/

#define DISP_HCMS29XX_DRIVER {			\
    disp_hcms29xx_dev_initialize,		\
      disp_hcms29xx_dev_open,			\
      NULL,					\
      disp_hcms29xx_dev_write,			\
      NULL,					\
      disp_hcms29xx_dev_close}
  
    
#ifdef __cplusplus
}
#endif

#endif /* _DISP_HCMS29XX_H */
