/******************************************************************************
* Copyright (C) 2018 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include "xqspipsu.h"

int QspiPsu_NOR_Initialize(
  XQspiPsu *QspiPsuInstancePtr,
  u16 QspiPsuIntrId
);

/*****************************************************************************/
/**
 *
 * This function erases the sectors in the  serial Flash connected to the
 * QSPIPSU interface.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int QspiPsu_NOR_Erase(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount
);

/*****************************************************************************/
/**
 *
 * This function writes to the  serial Flash connected to the QSPIPSU interface.
 * All the data put into the buffer must be in the same page of the device with
 * page boundaries being on 256 byte boundaries.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address to write data to in the Flash.
 * @param	ByteCount contains the number of bytes to write.
 * @param	WriteBfrPtr is pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int QspiPsu_NOR_Write_Page(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr
);

/*****************************************************************************/
/**
 *
 * This function writes to the serial Flash connected to the QSPIPSU interface.
 * Writes will be broken into device page sized and aligned writes as necessary.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address to write data to in the Flash.
 * @param	ByteCount contains the number of bytes to write.
 * @param	WriteBfrPtr is pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int QspiPsu_NOR_Write(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr
);

/*****************************************************************************/
/**
 *
 * This function performs a read. Default setting is in DMA mode.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	ReadBfrPtr is pointer to the read buffer to which valid received data
 *		should be written
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int QspiPsu_NOR_Read(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 **ReadBfrPtr
);

/*****************************************************************************/
/**
 *
 * This function performs a read of the ECC Status Register for a given address.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the ECC unit for which the ECCSR
 *		needs to be read. The ECC unit contains 16 bytes of user data
 *		and all bytes in an ECC unit will return the same ECCSR.
 * @param	ReadBfrPtr is a pointer to a single byte to which the ECCSR will
 * 		be written.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	Only the three least significant bits of the returned byte are
 * 		meaningful. If all bits are 0, ECC is enabled for this unit and
 * 		no errors have been encountered.
 * 		Bit 0 is 1: ECC is disabled for the requested unit.
 * 		Bit 1 is 1: A single bit error has been corrected in user data.
 * 		Bit 2 is 1: A single bit error has been found in the ECC data
 * 		            and may indicate user data corruption.
 *
 ******************************************************************************/
int QspiPsu_NOR_Read_Ecc(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u8 *ReadBfrPtr
);

/*****************************************************************************/
/**
 *
 * This function returns the size of attached flash parts.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return	The size of attached flash in bytes.
 *
 ******************************************************************************/
u32 QspiPsu_NOR_Get_Device_Size(XQspiPsu *QspiPsuPtr);

/*****************************************************************************/
/**
 *
 * This function returns the sector size of attached flash parts.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return	The sector size of attached flash in bytes.
 *
 ******************************************************************************/
u32 QspiPsu_NOR_Get_Sector_Size(XQspiPsu *QspiPsuPtr);

/*****************************************************************************/
/**
 *
 * This function performs a read of the RDID configuration space.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	ReadBfrPtr is a pointer to a buffer to be filled with
 * 		configuration data.
 * @param	ReadLen is the total length of the configuration space to read.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 ******************************************************************************/
int QspiPsu_NOR_RDID(XQspiPsu *QspiPsuPtr, u8 *ReadBfrPtr, u32 ReadLen);

/*****************************************************************************/
/**
 *
 * This function performs a read of the SFDP configuration space.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	ReadBfrPtr is a pointer to a buffer to be filled with
 * 		configuration data.
 * @param	ReadLen is the total length of the configuration space to read.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 ******************************************************************************/
int QspiPsu_NOR_RDSFDP(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 **ReadBfrPtr
);
