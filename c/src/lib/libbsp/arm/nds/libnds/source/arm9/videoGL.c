/*---------------------------------------------------------------------------------
	Video API vaguely similar to OpenGL

  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>

// this is the actual data of the globals for videoGL
//   Please use the glGlob pointer to access this data since that makes it easier to move stuff in/out of the header.
static gl_hidden_globals glGlobalData;

// This returns the pointer to the globals for videoGL
gl_hidden_globals* glGetGlobals() {
	return &glGlobalData;
}

//---------------------------------------------------------------------------------
void glRotatef32i(int angle, int32 x, int32 y, int32 z) {
//---------------------------------------------------------------------------------
	int32 axis[3];
	int32 sine = SIN[angle &  LUT_MASK];
	int32 cosine = COS[angle & LUT_MASK];
	int32 one_minus_cosine = inttof32(1) - cosine;

	axis[0]=x;
	axis[1]=y;
	axis[2]=z;

	normalizef32(axis);   // should require passed in normalized?

	MATRIX_MULT3x3 = cosine + mulf32(one_minus_cosine, mulf32(axis[0], axis[0]));
	MATRIX_MULT3x3 = mulf32(one_minus_cosine, mulf32(axis[0], axis[1])) - mulf32(axis[2], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) + mulf32(axis[1], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]),  axis[1]) + mulf32(axis[2], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[1]), axis[1]);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) - mulf32(axis[0], sine);

	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) - mulf32(axis[1], sine);
	MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) + mulf32(axis[0], sine);
	MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[2]), axis[2]);
}




//---------------------------------------------------------------------------------
void glMaterialf(GL_MATERIALS_ENUM mode, rgb color) {
//---------------------------------------------------------------------------------
	static uint32 diffuse_ambient = 0;
	static uint32 specular_emission = 0;

	switch(mode) {
		case GL_AMBIENT:
			diffuse_ambient = (color << 16) | (diffuse_ambient & 0xFFFF);
			break;
		case GL_DIFFUSE:
			diffuse_ambient = color | (diffuse_ambient & 0xFFFF0000);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			diffuse_ambient= color + (color << 16);
			break;
		case GL_SPECULAR:
			specular_emission = color | (specular_emission & 0xFFFF0000);
			break;
		case GL_SHININESS:
			break;
		case GL_EMISSION:
			specular_emission = (color << 16) | (specular_emission & 0xFFFF);
			break;
	}

	GFX_DIFFUSE_AMBIENT = diffuse_ambient;
	GFX_SPECULAR_EMISSION = specular_emission;
}

//---------------------------------------------------------------------------------
void glInit_C(void) {
//---------------------------------------------------------------------------------
	glGlob = glGetGlobals();

	glGlob->clearColor = 0;

	// init texture globals
	glGlob->activeTexture = 0;
	glGlob->nextBlock = (uint32*)0x06800000;
	glGlob->nextPBlock = 0;
	glGlob->nameCount = 1;

	while (GFX_STATUS & (1<<27)); // wait till gfx engine is not busy

	// Clear the FIFO
	GFX_STATUS |= (1<<29);

	// Clear overflows from list memory
	glResetMatrixStack();

	// prime the vertex/polygon buffers
	glFlush(0);

	// reset the control bits
	GFX_CONTROL = 0;

	// reset the rear-plane(a.k.a. clear color) to black, ID=0, and opaque
	glClearColor(0,0,0,31);
	glClearPolyID(0);

	// reset stored texture locations
	glResetTextures();

	// reset the depth to it's max
	glClearDepth(GL_MAX_DEPTH);

	GFX_TEX_FORMAT = 0;
	GFX_POLY_FORMAT = 0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
}

//---------------------------------------------------------------------------------
void glResetTextures(void) {
//---------------------------------------------------------------------------------
	glGlob->activeTexture = 0;
	glGlob->nextBlock = (uint32*)0x06800000;
	glGlob->nextPBlock = 0;
	glGlob->nameCount = 1;
}

//---------------------------------------------------------------------------------
//	glGenTextures creates integer names for your table
//	takes n as the number of textures to generate and
//	a pointer to the names array that it needs to fill.
//  Returns 1 if succesful and 0 if out of texture names
//---------------------------------------------------------------------------------

int glGenTextures(int n, int *names) {
//---------------------------------------------------------------------------------
	int index = 0;
	for(index = 0; index < n; index++) {
		if(glGlob->nameCount >= MAX_TEXTURES)
			return 0;
		else
			names[index] = glGlob->nameCount++;
	}
	return 1;
}

//---------------------------------------------------------------------------------
// glBindTexure sets the current named
//	texture to the active texture.  Target
//	is ignored as all DS textures are 2D
//---------------------------------------------------------------------------------
void glBindTexture(int target, int name) {
//---------------------------------------------------------------------------------
	if (name == 0)
		GFX_TEX_FORMAT = 0;
	else
		GFX_TEX_FORMAT = glGlob->textures[name];


	glGlob->activeTexture = name;
}
//---------------------------------------------------------------------------------
// glColorTable establishes the location of the current palette.
//	Roughly follows glColorTableEXT. Association of palettes with
//	named textures is left to the application.
//---------------------------------------------------------------------------------
void glColorTable( uint8 format, uint32 addr ) {
//---------------------------------------------------------------------------------
	GFX_PAL_FORMAT = addr>>(4-(format==GL_RGB4));
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void glTexCoord2f32(int32 u, int32 v) {
//---------------------------------------------------------------------------------
	int x, y;

	x = ((glGlob->textures[glGlob->activeTexture]) >> 20) & 7;
	y = ((glGlob->textures[glGlob->activeTexture]) >> 23) & 7;

	glTexCoord2t16(f32tot16 (mulf32(u,inttof32(8<<x))), f32tot16 (mulf32(v,inttof32(8<<y))));
}

//---------------------------------------------------------------------------------
// glTexParameter although named the same
//	as its gl counterpart it is not compatible
//	Effort may be made in the future to make it so.
//---------------------------------------------------------------------------------
void glTexParameter(	uint8 sizeX, uint8 sizeY,
						const uint32* addr,
						GL_TEXTURE_TYPE_ENUM mode,
						uint32 param) {
//---------------------------------------------------------------------------------
	glGlob->textures[glGlob->activeTexture] = param | (sizeX << 20) | (sizeY << 23) | (((uint32)addr >> 3) & 0xFFFF) | (mode << 26);
}
//---------------------------------------------------------------------------------
//glGetTexturePointer gets a pointer to vram which contains the texture
//
//---------------------------------------------------------------------------------
void* glGetTexturePointer(	int name) {
//---------------------------------------------------------------------------------
	return (void*) ((glGlob->textures[name] & 0xFFFF) << 3);
}

//---------------------------------------------------------------------------------
u32 glGetTexParameter(){
//---------------------------------------------------------------------------------
	return glGlob->textures[glGlob->activeTexture];
}


//---------------------------------------------------------------------------------
inline uint32 alignVal( uint32 val, uint32 to ) {
	return (val & (to-1))? (val & ~(to-1)) + to : val;
}

//---------------------------------------------------------------------------------
int getNextPaletteSlot(u16 count, uint8 format) {
//---------------------------------------------------------------------------------
	// ensure the result aligns on a palette block for this format
	uint32 result = alignVal(glGlob->nextPBlock, 1<<(4-(format==GL_RGB4)));

	// convert count to bytes and align to next (smallest format) palette block
	count = alignVal( count<<1, 1<<3 );

	// ensure that end is within palette video mem
	if( result+count > 0x10000 )   // VRAM_F - VRAM_E
		return -1;

	glGlob->nextPBlock = result+count;
	return (int)result;
}

//---------------------------------------------------------------------------------
uint16* vramGetBank(uint16 *addr) {
//---------------------------------------------------------------------------------
	if(addr >= VRAM_A && addr < VRAM_B)
		return VRAM_A;
	else if(addr >= VRAM_B && addr < VRAM_C)
		return VRAM_B;
	else if(addr >= VRAM_C && addr < VRAM_D)
		return VRAM_C;
	else if(addr >= VRAM_D && addr < VRAM_E)
		return VRAM_D;
	else if(addr >= VRAM_E && addr < VRAM_F)
		return VRAM_E;
	else if(addr >= VRAM_F && addr < VRAM_G)
		return VRAM_F;
	else if(addr >= VRAM_G && addr < VRAM_H)
		return VRAM_H;
	else if(addr >= VRAM_H && addr < VRAM_I)
		return VRAM_H;
	else return VRAM_I;
}


//---------------------------------------------------------------------------------
int vramIsTextureBank(uint16 *addr) {
//---------------------------------------------------------------------------------
	uint16* vram = vramGetBank(addr);

	if(vram == VRAM_A)
	{
		if((VRAM_A_CR & 3) == ((VRAM_A_TEXTURE) & 3))
			return 1;
		else return 0;
	}
	else if(vram == VRAM_B)
	{
		if((VRAM_B_CR & 3) == ((VRAM_B_TEXTURE) & 3))
			return 1;
		else return 0;
	}
	else if(vram == VRAM_C)
	{
		if((VRAM_C_CR & 3) == ((VRAM_C_TEXTURE) & 3))
			return 1;
		else return 0;
	}
	else if(vram == VRAM_D)
	{
		if((VRAM_D_CR & 3) == ((VRAM_D_TEXTURE) & 3))
			return 1;
		else return 0;
	}
	else
		return 0;
}
//---------------------------------------------------------------------------------
uint32* getNextTextureSlot(int size) {
//---------------------------------------------------------------------------------
	uint32* result = glGlob->nextBlock;
	glGlob->nextBlock += size >> 2;

	//uh-oh...out of texture memory in this bank...find next one assigned to textures
	while(!vramIsTextureBank((uint16*)glGlob->nextBlock - 1) && glGlob->nextBlock <= (uint32*)VRAM_E)
	{
		glGlob->nextBlock = (uint32*)vramGetBank((uint16*)result) + (0x20000 >> 2); //next bank
		result = glGlob->nextBlock;
		glGlob->nextBlock += size >> 2;
	}

	if(glGlob->nextBlock > (uint32*)VRAM_E) {
		result = 0;
	}
	return result;
}

//---------------------------------------------------------------------------------
// Similer to glTextImage2D from gl it takes a pointer to data
//  Empty fields and target are unused but provided for code compatibility.
//  type is simply the texture type (GL_RGB, GL_RGB8 ect...)
//---------------------------------------------------------------------------------
int glTexImage2D(int target, int empty1, GL_TEXTURE_TYPE_ENUM type, int sizeX, int sizeY, int empty2, int param, const uint8* texture) {
//---------------------------------------------------------------------------------
	uint32 size = 0;
	uint32* addr;
	uint32 vramTemp;

	size = 1 << (sizeX + sizeY + 6);


	switch (type) {
		case GL_RGB:
		case GL_RGBA:
			size = size << 1;
			break;
		case GL_RGB4:
			size = size >> 2;
			break;
		case GL_RGB16:
			size = size >> 1;
			break;
		default:
			break;
	}

	addr = getNextTextureSlot(size);

	if(!addr)
	return 0;

	// unlock texture memory
	vramTemp = vramSetMainBanks(VRAM_A_LCD,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);

	if (type == GL_RGB) {
		// We do GL_RGB as GL_RGBA, but we set each alpha bit to 1 during the copy
		u16 * src = (u16*)texture;
		u16 * dest = (u16*)addr;

		glTexParameter(sizeX, sizeY, addr, GL_RGBA, param);

		while (size--) {
			*dest++ = *src | (1 << 15);
			src++;
		}
	} else {
		// For everything else, we do a straight copy
		glTexParameter(sizeX, sizeY, addr, type, param);
		swiCopy((uint32*)texture, addr , size / 4 | COPY_MODE_WORD);
	}
	vramRestoreMainBanks(vramTemp);
	return 1;
}

//---------------------------------------------------------------------------------
void glTexLoadPal(const u16* pal, u16 count, u32 addr) {
//---------------------------------------------------------------------------------
	vramSetBankE(VRAM_E_LCD);
	swiCopy( pal, &VRAM_E[addr>>1] , count / 2 | COPY_MODE_WORD);
	vramSetBankE(VRAM_E_TEX_PALETTE);
}

//---------------------------------------------------------------------------------
int gluTexLoadPal(const u16* pal, u16 count, uint8 format) {
//---------------------------------------------------------------------------------
	int addr = getNextPaletteSlot(count, format);
	if( addr>=0 )
		glTexLoadPal(pal, count, (u32) addr);

	return addr;
}





