/*
 * Copyright (c) 2000 - Rosimildo da Silva
 *
 * MODULE DESCRIPTION:
 * Micro FrameBuffer interface for Embedded Systems.
 *
 * $Id$
 */

#ifndef _MW_FB_H
#define _MW_FB_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* ioctls
   0x46 is 'F'                                */
#define FB_SCREENINFO             0x4601
#define FB_GETPALETTE             0x4602
#define FB_SETPALETTE             0x4603
#define FB_EXEC_FUNCTION          0x4604


#define FB_TYPE_PACKED_PIXELS          0    /* Packed Pixels    */
#define FB_TYPE_PLANES                 1    /* Non interleaved planes */
#define FB_TYPE_INTERLEAVED_PLANES     2    /* Interleaved planes    */
#define FB_TYPE_TEXT                   3    /* Text/attributes    */
#define FB_TYPE_VGA_PLANES             4    /* EGA/VGA planes    */
#define FB_TYPE_VIRTUAL_BUFFER         5    /* Virtual Buffer */


#define FB_VISUAL_MONO01               0    /* Monochr. 1=Black 0=White */
#define FB_VISUAL_MONO10               1    /* Monochr. 1=White 0=Black */
#define FB_VISUAL_TRUECOLOR            2    /* True color    */
#define FB_VISUAL_PSEUDOCOLOR          3    /* Pseudo color (like atari) */
#define FB_VISUAL_DIRECTCOLOR          4    /* Direct color */
#define FB_VISUAL_STATIC_PSEUDOCOLOR   5    /* Pseudo color readonly */

#define FB_ACCEL_NONE                  0    /* no hardware accelerator    */

struct fb_screeninfo {
    uint32_t xres;                  /* visible resolution        */
    uint32_t yres;
    uint32_t bits_per_pixel;        /* guess what            */
	 uint32_t line_length;          /* number of chars per line */
    volatile char *smem_start; /* Start of frame buffer mem  */
                                /* (physical address)         */
    uint32_t smem_len;             /* Length of frame buffer mem */
    uint32_t type;                 /* see FB_TYPE_*              */
    uint32_t visual;               /* see FB_VISUAL_*            */

};

struct fb_cmap {
    uint32_t start;                /* First entry    */
    uint32_t len;                  /* Number of entries */
    uint16_t *red;                 /* Red values    */
    uint16_t *green;
    uint16_t *blue;
    uint16_t *transp;              /* transparency, can be NULL */
};

/* type of function to be executed at the driver level */
#define FB_FUNC_ENTER_GRAPHICS   0
#define FB_FUNC_EXIT_GRAPHICS    1
#define FB_FUNC_IS_DIRTY         2
#define FB_FUNC_GET_MODE         3

struct fb_exec_function
{
    int    func_no;
    void  *param;
};


/* Micro Framebuffer API Wrapper */

/*
 * This function returns the information regarding the display.
 * It is called just after the driver be opened to get all needed
 * information about the driver. No change in the mode of operation
 * of the driver is done with this call.
 */
extern int ufb_get_screen_info( int fd, struct fb_screeninfo *info );


/*
 * Returns the mode of the graphics subsystem
 */
extern int ufb_get_mode( int fd, int *mode );


/*
 * Returns the current collor pallete
 */
extern int ufb_get_palette( int fd, struct fb_cmap *color );

/*
 * Set the current collor pallete
 */
extern int ufb_set_palette( int fd, struct fb_cmap *color );

/*
 * Does all necessary initialization to put the device in
 * graphics mode
 */
extern int ufb_enter_graphics( int fd, int mode );


/*
 * Switch the device back to the default mode of operation.
 * In most cases it put the device back to plain text mode.
 */
extern int ufb_exit_graphics( int fd );


/*
 * Tell the driver that the "virtual buffer" is dirty, and an update
 * of it to the real device, maybe a serial/parallel LCD or whatever
 * is required
 */
extern int ufb_buffer_is_dirty( int fd );


/*
 * This function maps the physical ( kernel mode ) address of the framebuffer device
 * and maps it to the user space address.
 */
 int ufb_mmap_to_user_space( int fd, void **fb_addr, void *physical_addr, unsigned long size );



/*
 * This function unmaps memory of the FB from the user's space
 */
 int ufb_unmmap_from_user_space( int fd, void *addr );

#ifdef	__cplusplus
}
#endif

#endif /* _MW_FB_H */
