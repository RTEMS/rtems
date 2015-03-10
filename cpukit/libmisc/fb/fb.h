/** 
 * @file rtems/fb.h
 *
 * @brief Frame Buffer Device Driver
 *
 * This file defines the interface to a frame buffer device driver.
 */

/*
 * Copyright (c) 2000 - Rosimildo da Silva
 */

#ifndef _MW_FB_H
#define _MW_FB_H

#include <stdint.h>

/**
 * @defgroup libmisc_fb Frame Buffer Device Driver Interface
 *
 * @ingroup Device Drivers and Frameworks
 */
/**@{*/

#ifdef	__cplusplus
extern "C" {
#endif

/* ioctls
   0x46 is 'F'                                */
#define FBIOGET_VSCREENINFO	0x4600
#define FBIOPUT_VSCREENINFO	0x4601
#define FBIOGET_FSCREENINFO	0x4602
#define FBIOGETCMAP		0x4604
#define FBIOPUTCMAP		0x4605
#define FB_EXEC_FUNCTION	0x4606
#define FBIOSWAPBUFFERS         0x4607
#define FBIOSETBUFFERMODE       0x4608
#define FBIOSETVIDEOMODE	0x4609

#define FB_SINGLE_BUFFERED  0
#define FB_TRIPLE_BUFFERED  1

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

struct fb_bitfield {
	uint32_t offset;		/* beginning of bitfield	*/
	uint32_t length;		/* length of bitfield		*/
	uint32_t msb_right;		/* != 0 : Most significant bit is */
					/* right */
};

struct fb_var_screeninfo {
    uint32_t xres;                  /* visible resolution        */
    uint32_t yres;
    uint32_t bits_per_pixel;        /* guess what            */
    struct fb_bitfield red;	    /* bitfield in fb mem if true color, */
    struct fb_bitfield green;	    /* else only length is significant */
    struct fb_bitfield blue;
    struct fb_bitfield transp;	    /* transparency			*/
};

struct fb_fix_screeninfo {
    volatile char *smem_start; 	/* Start of frame buffer mem  */
                                /* (physical address)         */
    uint32_t smem_len;          /* Length of frame buffer mem */
    uint32_t type;              /* see FB_TYPE_*              */
    uint32_t visual;            /* see FB_VISUAL_*            */
    uint32_t line_length;       /* number of chars per line */
};

struct fb_cmap {
    uint32_t start;                /* First entry    */
    uint32_t len;                  /* Number of entries */
    uint16_t *red;                 /* Red values    */
    uint16_t *green;
    uint16_t *blue;
    uint16_t *transp;              /* transparency, can be NULL */
};

#ifdef	__cplusplus
}
#endif
/**@}*/

#endif /* _MW_FB_H */
