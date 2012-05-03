/*---------------------------------------------------------------------------------
	BoxTest.c -- Code for performing hardware box test against viewing frustrum

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

	1.	The origin of this software must not be misrepresented; you
	must not claim that you wrote the original software. If you use
	this software in a product, an acknowledgment in the product
	documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
	must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
	distribution.

---------------------------------------------------------------------------------*/

#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>

//---------------------------------------------------------------------------------
void BoxTest_Asynch(v16 x, v16 y, v16 z, v16 width, v16 height, v16 depth)
//---------------------------------------------------------------------------------
{
  glPolyFmt(BIT(12) | BIT(13));
  glBegin(GL_TRIANGLES);
  glEnd();

  GFX_BOX_TEST = VERTEX_PACK(x, y);
  GFX_BOX_TEST = VERTEX_PACK(z, width);
  GFX_BOX_TEST = VERTEX_PACK(height, depth);
}

//---------------------------------------------------------------------------------
void BoxTestf_Asynch(float x, float y, float z, float width, float height, float depth)
//---------------------------------------------------------------------------------
{
  BoxTest_Asynch(floattov16(x), floattov16(y), floattov16(z),
                 floattov16(width), floattov16(height), floattov16(depth));
}

//---------------------------------------------------------------------------------
int BoxTestResult(void)
//---------------------------------------------------------------------------------
{
  while(GFX_STATUS & BIT(0));

  return (GFX_STATUS & BIT(1));
}

//---------------------------------------------------------------------------------
int BoxTest(v16 x, v16 y, v16 z, v16 width, v16 height, v16 depth)
//---------------------------------------------------------------------------------
{
  glPolyFmt(BIT(12) | BIT(13));
  glBegin(GL_TRIANGLES);
  glEnd();

  GFX_BOX_TEST = VERTEX_PACK(x, y);
  GFX_BOX_TEST = VERTEX_PACK(z, width);
  GFX_BOX_TEST = VERTEX_PACK(height, depth);

  while(GFX_STATUS & BIT(0));

  return (GFX_STATUS & BIT(1));
}

//---------------------------------------------------------------------------------
int BoxTestf(float x, float y, float z, float width, float height, float depth)
//---------------------------------------------------------------------------------
{
  return BoxTest(floattov16(x), floattov16(y), floattov16(z),
                 floattov16(width), floattov16(height), floattov16(depth));
}
