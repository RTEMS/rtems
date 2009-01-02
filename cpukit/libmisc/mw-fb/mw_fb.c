/*
 *
 * Copyright (c) 2000 - Rosimildo da Silva
 *
 * MODULE DESCRIPTION:
 * Wrapper API around the ioctls calls for the Micro FrameBuffer
 * interface for Embedded Systems
 *
 * All functions returns 0 on success. Any other value should be
 * decoded as an error. A list of errors will be created over time.
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/ioctl.h>
#include <rtems/mw_fb.h>


/*
 * This function returns the information regarding the display.
 * It is called just after the driver be opened to get all needed
 * information about the driver. No change in the mode of operation
 * of the driver is done with this call.
 */
 int ufb_get_screen_info( int fd, struct fb_screeninfo *info )
 {
    return ioctl( fd, FB_SCREENINFO, ( void *)info);
 }

/*
 * Returns the mode of the graphics subsystem
 */
 int ufb_get_mode( int fd, int *mode )
 {
    struct fb_exec_function exec;
    exec.func_no = FB_FUNC_GET_MODE;
    exec.param = ( void *)mode;
    return ioctl( fd, FB_EXEC_FUNCTION , ( void *)&exec );
 }

/*
 * Returns the current collor pallete
 */
 int ufb_get_palette( int fd, struct fb_cmap *color )
 {
     return ioctl( fd, FB_GETPALETTE, ( void *)color );
 }

/*
 * Set the current collor pallete
 */
 int ufb_set_palette( int fd, struct fb_cmap *color )
 {
    return ioctl( fd, FB_SETPALETTE, ( void *)color );
 }

/*
 * Does all necessary initialization to put the device in
 * graphics mode
 */
 int ufb_enter_graphics( int fd, int mode )
 {
    struct fb_exec_function exec;
    exec.func_no = FB_FUNC_ENTER_GRAPHICS;
    exec.param = ( void *)mode;
    return ioctl( fd, FB_EXEC_FUNCTION , ( void *)&exec );
 }

/*
 * Switch the device back to the default mode of operation.
 * In most cases it put the device back to plain text mode.
 */
 int ufb_exit_graphics( int fd )
 {
    struct fb_exec_function exec;
    exec.func_no = FB_FUNC_EXIT_GRAPHICS;
    exec.param = 0;
    return ioctl( fd, FB_EXEC_FUNCTION , ( void *)&exec );
 }

/*
 * Tell the driver that the "virtual buffer" is dirty, and an update
 * of it to the real device, maybe a serial/parallel LCD or whatever
 * is required
 */
 int ufb_buffer_is_dirty( int fd )
 {
    struct fb_exec_function exec;
    exec.func_no = FB_FUNC_IS_DIRTY;
    exec.param = 0;
    return ioctl( fd, FB_EXEC_FUNCTION , ( void *)&exec );
 }


/*
 * This function maps the physical ( kernel mode ) address of the framebuffer
 * device and maps it to the user space address.
 */
 int ufb_mmap_to_user_space(
    int fd __attribute__((unused)),
    void **fb_addr,
    void *physical_addr,
    unsigned long size __attribute__((unused))
 )
 {
 #ifdef __rtems__
    /* RTEMS runs in ring 0, and there is no distinction between
       user space and kernel space, so we just return the same
       pointer to the caller.
     */
      *fb_addr = physical_addr;
      return 0;
 #else
 /* other kernels might want to map it to the user space,
    maybe using mmap()
  */
      return 0;
 #endif

 }


/*
 * This function unmaps memory of the FB from the user's space
 */
 int ufb_unmmap_from_user_space( int fd __attribute__((unused)), void *addr __attribute__((unused)))
 {
    return 0;
 }
