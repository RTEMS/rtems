/*---------------------------------------------------------------------------------
BoxTest.h -- Code for performing hardware box test against viewing frustrum

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
#ifndef BOX_TEST_INCLUDE
#define BOX_TEST_INCLUDE

#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>

/*! \file boxtest.h
\brief Box Test Functions.

*/

#ifdef __cplusplus
extern "C" {
#endif
/*! \fn int BoxTest(v16 x, v16 y, v16 z, v16 width, v16 height, v16 depth)
\brief Performs a test to determine if the provided box is in the view frustrum.
\param x (x, y, z) point of a vertex on the box
\param y (x, y, z) point of a vertex on the box
\param z (x, y, z) point of a vertex on the box
\param height (height, width, depth) describe the size of the box referenced from (x, y, z)
\param width (height, width, depth) describe the size of the box referenced from (x, y, z)
\param depth (height, width, depth) describe the size of the box referenced from (x, y, z)

\return non zero if any or all of the box is in the view frustum
*/
int BoxTest(v16 x, v16 y, v16 z, v16 width, v16 height, v16 depth);

/*! \fn int BoxTestf(float x, float y, float z, float width, float height, float depth)
\brief Performs a test to determine if the provided box is in the view frustum.
\param x (x, y, z) point of a vertex on the box
\param y (x, y, z) point of a vertex on the box
\param z (x, y, z) point of a vertex on the box
\param width (width, height, depth) describe the size of the box referenced from (x, y, z)
\param height (width, height, depth) describe the size of the box referenced from (x, y, z)
\param depth (width, height, depth) describe the size of the box referenced from (x, y, z)

\return non zero if any or all of the box is in the view frustum
*/
int BoxTestf(float x, float y, float z, float width, float height, float depth);

/*! \fn void BoxTest_Asynch(v16 x, v16 y, v16 z, v16 width, v16 height, v16 depth)
\brief Performs a test to determine if the provided box is in the view frustum.
\param x (x, y, z) point of a vertex on the box
\param y (x, y, z) point of a vertex on the box
\param z (x, y, z) point of a vertex on the box
\param width (width, height, depth) describe the size of the box referenced from (x, y, z)
\param height (width, height, depth) describe the size of the box referenced from (x, y, z)
\param depth (width, height, depth) describe the size of the box referenced from (x, y, z)

Performs a test to determine if the provided box is in the view frustum.  BoxTestResult must be called to get the result of this
operation.
*/
void BoxTest_Asynch(v16 x, v16 y, v16 z, v16 height, v16 width, v16 depth);

/*! \fn void BoxTestf_Asynch(float x, float y, float z, float width, float height, float depth)
\brief Performs a test to determine if the provided box is in the view frustum.
\param x (x, y, z) point of a vertex on the box
\param y (x, y, z) point of a vertex on the box
\param z (x, y, z) point of a vertex on the box
\param width (width, height, depth) describe the size of the box referenced from (x, y, z)
\param height (width, height, depth) describe the size of the box referenced from (x, y, z)
\param depth (width, height, depth) describe the size of the box referenced from (x, y, z)

Performs a test to determine if the provided box is in the view frustum.  BoxTestResult must be called to get the result of this
operation.
*/
void BoxTestf_Asynch(float x, float y, float z, float width, float height, float depth);

/*! \fn void BoxTestResult(void)
\brief Gets the result of the last box test.  Needed for asynch box test calls.
\return non zero if any or all of the box is in the view frustum

*/
int BoxTestResult(void);

#ifdef __cplusplus
}
#endif
#endif
