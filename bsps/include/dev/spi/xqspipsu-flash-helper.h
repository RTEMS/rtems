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
