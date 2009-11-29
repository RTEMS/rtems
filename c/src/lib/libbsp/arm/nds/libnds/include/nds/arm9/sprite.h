/*---------------------------------------------------------------------------------

	sprite.h -- definitions for DS sprites

	Copyright (C) 2007
		Liran Nuna	(LiraNuna)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
	must not claim that you wrote the original software. If you use
	this software in a product, an acknowledgment in the product
	documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
	distribution.

---------------------------------------------------------------------------------*/
#ifndef _libnds_sprite_h_
#define _libnds_sprite_h_

#ifndef ARM9
#error Sprites are only available on the ARM9
#endif

#include <nds/jtypes.h>

// Sprite control defines

// Attribute 0 consists of 8 bits of Y plus the following flags:
#define ATTR0_NORMAL			(0<<8)
#define ATTR0_ROTSCALE			(1<<8)
#define ATTR0_DISABLED			(2<<8)
#define ATTR0_ROTSCALE_DOUBLE	(3<<8)

#define ATTR0_TYPE_NORMAL		(0<<10)
#define ATTR0_TYPE_BLENDED		(1<<10)
#define ATTR0_TYPE_WINDOWED		(2<<10)
#define ATTR0_BMP				(3<<10)

#define ATTR0_MOSAIC			(1<<12)

#define ATTR0_COLOR_16		(0<<13) //16 color in tile mode...16 bit in bitmap mode
#define ATTR0_COLOR_256		(1<<13)

#define ATTR0_SQUARE		(0<<14)
#define ATTR0_WIDE			(1<<14)
#define ATTR0_TALL			(2<<14)

#define OBJ_Y(m)			((m)&0x00ff)

// Atribute 1 consists of 9 bits of X plus the following flags:
#define ATTR1_ROTDATA(n)      ((n)<<9)  // note: overlaps with flip flags
#define ATTR1_FLIP_X          (1<<12)
#define ATTR1_FLIP_Y          (1<<13)
#define ATTR1_SIZE_8          (0<<14)
#define ATTR1_SIZE_16         (1<<14)
#define ATTR1_SIZE_32         (2<<14)
#define ATTR1_SIZE_64         (3<<14)

#define OBJ_X(m)			((m)&0x01ff)

// Atribute 2 consists of the following:
#define ATTR2_PRIORITY(n)     ((n)<<10)
#define ATTR2_PALETTE(n)      ((n)<<12)
#define ATTR2_ALPHA(n)		  ((n)<<12)

/**
 * @enum tObjMode
 * @brief Sprite display mode.
 */
typedef enum
{
	OBJMODE_NORMAL,		/**< No special mode is on - Normal sprite state. */
	OBJMODE_BLENDED,	/**< Color blending is on - Sprite can use HW blending features. */
	OBJMODE_WINDOWED,	/**< Sprite can be seen only inside the sprite window. */
	OBJMODE_BITMAP,		/**< Sprite is not using tiles - per pixel image data. */

} tObjMode;

/**
 * @enum tObjShape
 * @brief Sprite shape mode.
 */
typedef enum {
	OBJSHAPE_SQUARE,	/**< Sprite shape is NxN (Height == Width). */
	OBJSHAPE_WIDE,		/**< Sprite shape is NxM with N > M (Height < Width). */
	OBJSHAPE_TALL,		/**< Sprite shape is NxM with N < M (Height > Width). */
	OBJSHAPE_FORBIDDEN,	/**< Sprite shape is undefined. */
} tObjShape;

/**
 * @enum tObjSize
 * @brief Object shape mode.
 */
typedef enum {
	OBJSIZE_8,		/**< Major sprite size is 8px. */
	OBJSIZE_16,		/**< Major sprite size is 16px. */
	OBJSIZE_32,		/**< Major sprite size is 32px. */
	OBJSIZE_64,		/**< Major sprite size is 64px. */
} tObjSize;

/**
 * @enum tObjColMode
 * @brief Object color mode.
 */
typedef enum {
	OBJCOLOR_16,		/**< sprite has 16 colors. */
	OBJCOLOR_256,		/**< sprite has 256 colors. */
} tObjColMode;

/**
 * @enum tObjColMode
 * @brief Object color mode.
 */
typedef enum {
	OBJPRIORITY_0,		/**< sprite priority level 0 - highest. */
	OBJPRIORITY_1,		/**< sprite priority level 1. */
	OBJPRIORITY_2,		/**< sprite priority level 2. */
	OBJPRIORITY_3,		/**< sprite priority level 3 - lowest. */
} tObjPriority;

// Sprite structures

typedef union {
	struct {

		struct {
			u16 posY						:8;	/**< Sprite Y position. */
			union {
				struct {
					u8 						:1;
					bool isHidden 			:1;	/**< Sprite is hidden (isRotoscale cleared). */
					u8						:6;
				};
				struct {
					bool isRotoscale		:1;	/**< Sprite uses affine parameters if set. */
					bool rsDouble			:1;	/**< Sprite bounds is doubled (isRotoscale set). */
					tObjMode objMode		:2;	/**< Sprite object mode. */
					bool isMosaic			:1;	/**< Enables mosaic effect if set. */
					tObjColMode colMode		:1;	/**< Sprite color mode. */
					tObjShape objShape		:2;	/**< Sprite shape. */
				};
			};
		};

		union {
			struct {
				u16 posX					:9;	/**< Sprite X position. */
				u8 							:7;
			};
			struct {
				u8							:8;
				union {
					struct {
						u8					:4;
						bool hFlip			:1; /**< Flip sprite horizontally (isRotoscale cleared). */
						bool vFlip			:1; /**< Flip sprite vertically (isRotoscale cleared).*/
						u8					:2;
					};
					struct {
						u8					:1;
						u8 rsMatrixIdx		:5; /**< Affine parameter number to use (isRotoscale set). */
						tObjSize objSize	:2; /**< Sprite size. */
					};
				};
			};
		};

		struct {
			u16 tileIdx						:10;/**< Upper-left tile index. */
			tObjPriority objPriority		:2;	/**< Sprite priority. */
			u8 objPal						:4;	/**< Sprite palette to use in paletted color modes. */
		};

		u16 attribute3;							/**< Four of those are used as a sprite rotation matrice */
	};

	struct {
		uint16 attribute[3];
		uint16 filler;
	};

} SpriteEntry, * pSpriteEntry;


typedef struct sSpriteRotation {
  uint16 filler1[3];
  int16 hdx;

  uint16 filler2[3];
  int16 hdy;

  uint16 filler3[3];
  int16 vdx;

  uint16 filler4[3];
  int16 vdy;
} SpriteRotation, * pSpriteRotation;

#define SPRITE_COUNT 128
#define MATRIX_COUNT 32

/**
 * @union tOAM
 * @brief Final OAM representation in memory
*/
typedef union {
	SpriteEntry spriteBuffer[SPRITE_COUNT];
	SpriteRotation matrixBuffer[MATRIX_COUNT];
} tOAM;


#endif // _libnds_sprite_h_
