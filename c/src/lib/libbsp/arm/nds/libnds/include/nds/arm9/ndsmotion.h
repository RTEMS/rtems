/*---------------------------------------------------------------------------------
	DS Motion Card/DS Motion Pak functionality

	Copyright (C) 2007
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Keith Epstein (KeithE)

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

/*! \file ndsmotion.h
\brief interface code for the ds motion card, ds motion pak, MK6

*/
#ifndef NDS_MOTION_INCLUDE
#define NDS_MOTION_INCLUDE
//---------------------------------------------------------------------------------


typedef struct
{
	short xoff, yoff, zoff, goff;
	short xsens, ysens, zsens, gsens;
}MotionCalibration;

#ifdef __cplusplus
extern "C" {
#endif

/*! \fn int motion_init(void)
\brief  Initializes the DS Motion Sensor.
Run this before using any of the DS Motion Sensor functions
save the return value and pass it to the other motion_ functions
*/
int motion_init(void);

/*! \fn int motion_deinit(void)
\brief  Deinitializes the DS Motion Sensor
*/
void motion_deinit(void);

/*! \fn signed int motion_read_x(void)
\brief  read the X acceleration
*/
signed int motion_read_x(void);

/*! \fn signed int motion_read_y(void)
\brief  read the Y acceleration
*/
signed int motion_read_y(void);

/*! \fn signed int motion_read_z(void)
\brief  read the Z acceleration
*/
signed int motion_read_z(void);

/*! \fn signed int motion_read_gyro(void)
\brief  read the Z rotational speed
*/
signed int motion_read_gyro(void);

/*! \fn int motion_acceleration_x(void)
\brief gets acceleration value to mili G (where g is 9.8 m/s*s)
*/
int motion_acceleration_x(void);

/*! \fn int motion_acceleration_y(void)
\brief gets acceleration value to mili G (where g is 9.8 m/s*s)
*/
int motion_acceleration_y(void);

/*! \fn int motion_acceleration_z(void)
\brief gets acceleration value to mili G (where g is 9.8 m/s*s)
*/
int motion_acceleration_z(void);

/*! \fn void motion_set_sens_x(int sens)
\brief this should be passed the raw reading at 1g for accurate
acceleration calculations.  Default is 819
*/
void motion_set_sens_x(int sens);

/*! \fn void motion_set_sens_y(int sens)
\brief this should be passed the raw reading at 1g for accurate
acceleration calculations.  Default is 819
*/
void motion_set_sens_y(int sens);

/*! \fn void motion_set_sens_z(int sens)
\brief this should be passed the raw reading at 1g for accurate
acceleration calculations.  Default is 819
*/
void motion_set_sens_z(int sens);

/*! \fn void motion_set_sens_x(int sens)
\brief this should be passed the raw reading at 1g for accurate
acceleration calculations.  Default is 825
*/
void motion_set_sens_gyro(int sens);

/*! \fn void motion_set_offs_x(void)
\brief this should be called when the axis is under no acceleration
default is 2048
*/
void motion_set_offs_x(void);

/*! \fn void motion_set_offs_y(void)
\brief this should be called when the axis is under no acceleration
default is 2048
*/
void motion_set_offs_y(void);

/*! \fn void motion_set_offs_z(void)
\brief this should be called when the axis is under no acceleration
default is 2048
*/
void motion_set_offs_z(void);

/*! \fn void motion_set_offs_gyro(void)
\brief this should be called when the axis is under no rotation
default is 1680
*/
void motion_set_offs_gyro(void);

/*! \fn int motion_rotation(void)
\brief converts raw rotation value to degrees per second
*/
int motion_rotation(void);

/*! \fn MotionCalibration* motion_get_calibration(void)void
\brief This returns the current calibration settings for saving
*/
MotionCalibration* motion_get_calibration(void);

/*! \fn void motion_set_calibration(MotionCalibration* cal)
\brief This sets the calibration settings.  Intended
to restore saved calibration settings
*/
void motion_set_calibration(MotionCalibration* cal);

/*! \fn MotionCalibration* motion_enable_ain_1(void)
\brief This enables the analog input number 1.
Required before reading analog input number 1.
*/
void motion_enable_ain_1(void);

/*! \fn MotionCalibration* motion_enable_ain_2(void)
\brief This enables the analog input number 2.
Required before reading analog input number 2.
*/
void motion_enable_ain_2(void);

/*! \fn MotionCalibration* motion_read_ain_1(void)
\brief This reads the analog input number 1.
analog input number 1 needs to be enabled before reading.
*/
int motion_read_ain_1(void);

/*! \fn MotionCalibration* motion_read_ain_2(void)
\brief This reads the analog input number 2.
analog input number 2 needs to be enabled before reading.
*/
int motion_read_ain_2(void);

#ifdef __cplusplus
}
#endif

#endif



