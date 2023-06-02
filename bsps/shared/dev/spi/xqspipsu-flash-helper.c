/******************************************************************************
* Copyright (C) 2018 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/**
 * @file xqspipsu_flash_helper.c
 *
 * This file contains flash helper functions for the QSPIPSU driver. It
 * consists of modified functions from Xilinx's flash example in
 * examples/xqspipsu_generic_flash_interrupt_example.c of the qspipsu driver.
 *
 */

#include "xqspipsu_flash_config.h"
#include "xqspipsu-flash-helper.h"

#include <rtems.h>

/*
 * Number of flash pages to be written.
 */
#define PAGE_COUNT		32

/*
 * Max page size to initialize write and read buffer
 */
#define MAX_PAGE_SIZE 1024

#define TEST_ADDRESS		0x000000

#define ENTER_4B	1
#define EXIT_4B		0

u8 ReadCmd;
u8 WriteCmd;
u8 StatusCmd;
u8 SectorEraseCmd;
u8 FSRFlag;

static int FlashReadID(XQspiPsu *QspiPsuPtr);

static int MultiDieRead(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 Command,
  u8 *WriteBfrPtr,
  u8 *ReadBfrPtr
);

static u32 GetRealAddr(
  XQspiPsu *QspiPsuPtr,
  u32 Address
);

static int BulkErase(
  XQspiPsu *QspiPsuPtr,
  u8 *WriteBfrPtr
);

static int DieErase(
  XQspiPsu *QspiPsuPtr,
  u8 *WriteBfrPtr
);

static int QspiPsuSetupIntrSystem(
  XQspiPsu *QspiPsuInstancePtr,
  u16 QspiPsuIntrId
);

static void QspiPsuHandler(
  void *CallBackRef,
  u32 StatusEvent,
  unsigned int ByteCount
);

static int FlashEnterExit4BAddMode(
  XQspiPsu *QspiPsuPtr,
  unsigned int Enable
);

static int FlashEnableQuadMode(XQspiPsu *QspiPsuPtr);

u8 TxBfrPtr;
u8 ReadBfrPtr[3];
u32 FlashMake;
u32 FCTIndex;	/* Flash configuration table index */

static XQspiPsu_Msg FlashMsg[5];

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */
volatile int TransferInProgress;

/*
 * The following variable tracks any errors that occur during interrupt
 * processing
 */
int Error;

/*
 * The following variable allows a test value to be added to the values that
 * are written to the Flash such that unique values can be generated to
 * guarantee the writes to the Flash were successful
 */
int Test = 1;

/*
 * The following variables are used to read and write to the flash and they
 * are global to avoid having large buffers on the stack
 * The buffer size accounts for maximum page size and maximum banks -
 * for each bank separate read will be performed leading to that many
 * (overhead+dummy) bytes
 */
#ifdef __ICCARM__
#pragma data_alignment = 32
u8 ReadBuffer[(PAGE_COUNT * MAX_PAGE_SIZE) + (DATA_OFFSET + DUMMY_SIZE)*8];
#else
u8 ReadBuffer[(PAGE_COUNT * MAX_PAGE_SIZE) + (DATA_OFFSET + DUMMY_SIZE)*8] __attribute__ ((aligned(64)));
#endif
u8 WriteBuffer[(PAGE_COUNT * MAX_PAGE_SIZE) + DATA_OFFSET];
u8 CmdBfr[8];

/*
 * The following constants specify the max amount of data and the size of the
 * the buffer required to hold the data and overhead to transfer the data to
 * and from the Flash. Initialized to single flash page size.
 */
u32 MaxData = PAGE_COUNT*256;

int QspiPsu_NOR_Initialize(
  XQspiPsu *QspiPsuInstancePtr,
  u16 QspiPsuIntrId
)
{
  int Status;

  if (QspiPsuInstancePtr == NULL) {
    return XST_FAILURE;
  }

  /*
   * Connect the QspiPsu device to the interrupt subsystem such that
   * interrupts can occur. This function is application specific
   */
  Status = QspiPsuSetupIntrSystem(QspiPsuInstancePtr, QspiPsuIntrId);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

    /*
   * Setup the handler for the QSPIPSU that will be called from the
   * interrupt context when an QSPIPSU status occurs, specify a pointer to
   * the QSPIPSU driver instance as the callback reference
   * so the handler is able to access the instance data
   */
  XQspiPsu_SetStatusHandler(QspiPsuInstancePtr, QspiPsuInstancePtr,
         (XQspiPsu_StatusHandler) QspiPsuHandler);

  /*
   * Set Manual Start
   */
  XQspiPsu_SetOptions(QspiPsuInstancePtr, XQSPIPSU_MANUAL_START_OPTION);

  /*
   * Set the prescaler for QSPIPSU clock
   */
  XQspiPsu_SetClkPrescaler(QspiPsuInstancePtr, XQSPIPSU_CLK_PRESCALE_8);

  XQspiPsu_SelectFlash(QspiPsuInstancePtr,
    XQSPIPSU_SELECT_FLASH_CS_LOWER,
    XQSPIPSU_SELECT_FLASH_BUS_LOWER);

  /*
   * Read flash ID and obtain all flash related information
   * It is important to call the read id function before
   * performing proceeding to any operation, including
   * preparing the WriteBuffer
   */

  Status = FlashReadID(QspiPsuInstancePtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Some flash needs to enable Quad mode before using
   * quad commands.
   */
  Status = FlashEnableQuadMode(QspiPsuInstancePtr);
  if (Status != XST_SUCCESS)
    return XST_FAILURE;

  /*
   * Address size and read command selection
   * Micron flash on REMUS doesn't support these 4B write/erase commands
   */
  if(QspiPsuInstancePtr->Config.BusWidth == BUSWIDTH_SINGLE)
    ReadCmd = FAST_READ_CMD;
  else if(QspiPsuInstancePtr->Config.BusWidth == BUSWIDTH_DOUBLE)
    ReadCmd = DUAL_READ_CMD;
  else
    ReadCmd = QUAD_READ_CMD;

  WriteCmd = WRITE_CMD;
  SectorEraseCmd = SEC_ERASE_CMD;

  if ((Flash_Config_Table[FCTIndex].NumDie > 1) &&
      (FlashMake == MICRON_ID_BYTE0)) {
    StatusCmd = READ_FLAG_STATUS_CMD;
    FSRFlag = 1;
  } else {
    StatusCmd = READ_STATUS_CMD;
    FSRFlag = 0;
  }

  if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
    Status = FlashEnterExit4BAddMode(QspiPsuInstancePtr, ENTER_4B);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    if (FlashMake == SPANSION_ID_BYTE0) {
      if(QspiPsuInstancePtr->Config.BusWidth == BUSWIDTH_SINGLE)
        ReadCmd = FAST_READ_CMD_4B;
      else if(QspiPsuInstancePtr->Config.BusWidth == BUSWIDTH_DOUBLE)
        ReadCmd = DUAL_READ_CMD_4B;
      else
        ReadCmd = QUAD_READ_CMD_4B;

      WriteCmd = WRITE_CMD_4B;
      SectorEraseCmd = SEC_ERASE_CMD_4B;
    }
  }

  return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * Callback handler.
 *
 * @param	CallBackRef is the upper layer callback reference passed back
 *		when the callback function is invoked.
 * @param	StatusEvent is the event that just occurred.
 * @param	ByteCount is the number of bytes transferred up until the event
 *		occurred.
 *
 * @return	None
 *
 * @note	None.
 *
 *****************************************************************************/
static void QspiPsuHandler(
  void *CallBackRef,
  u32 StatusEvent,
  unsigned int ByteCount
)
{
  /*
   * Indicate the transfer on the QSPIPSU bus is no longer in progress
   * regardless of the status event
   */
  TransferInProgress = FALSE;

  /*
   * If the event was not transfer done, then track it as an error
   */
  if (StatusEvent != XST_SPI_TRANSFER_DONE) {
    Error++;
  }
}

int QspiPsu_NOR_RDSFDP(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 **ReadBfrPtr
)
{
  int Status;

  *ReadBfrPtr = ReadBuffer;

  CmdBfr[COMMAND_OFFSET]   = READ_SFDP;
  CmdBfr[ADDRESS_1_OFFSET] =
      (u8)((Address & 0xFF0000) >> 16);
  CmdBfr[ADDRESS_2_OFFSET] =
      (u8)((Address & 0xFF00) >> 8);
  CmdBfr[ADDRESS_3_OFFSET] =
      (u8)(Address & 0xFF);

  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].TxBfrPtr = CmdBfr;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 4;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[1].TxBfrPtr = NULL;
  FlashMsg[1].RxBfrPtr = NULL;
  FlashMsg[1].ByteCount = DUMMY_CLOCKS;
  FlashMsg[1].Flags = 0;

  FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[2].TxBfrPtr = NULL;
  FlashMsg[2].RxBfrPtr = *ReadBfrPtr;
  FlashMsg[2].ByteCount = ByteCount;
  FlashMsg[2].Flags = XQSPIPSU_MSG_FLAG_RX;

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 3);
  if (Status != XST_SUCCESS)
    return XST_FAILURE;

  while (TransferInProgress);

  rtems_cache_invalidate_multiple_data_lines(ReadBuffer, ByteCount);
  return 0;
}

int QspiPsu_NOR_RDID(XQspiPsu *QspiPsuPtr, u8 *ReadBfrPtr, u32 ReadLen)
{
  int Status;

  /*
   * Read ID
   */
  TxBfrPtr = READ_ID;
  FlashMsg[0].TxBfrPtr = &TxBfrPtr;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 1;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  FlashMsg[1].TxBfrPtr = NULL;
  FlashMsg[1].RxBfrPtr = ReadBfrPtr;
  FlashMsg[1].ByteCount = ReadLen;
  FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }
  while (TransferInProgress);

  rtems_cache_invalidate_multiple_data_lines(ReadBfrPtr, ReadLen);
  return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * Reads the flash ID and identifies the flash in FCT table.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 *****************************************************************************/
static int FlashReadID(XQspiPsu *QspiPsuPtr)
{
  u32 ReadId = 0;
  u32 ReadLen = 3;
  int Status;

  Status = QspiPsu_NOR_RDID(QspiPsuPtr, ReadBfrPtr, ReadLen);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /* In case of dual, read both and ensure they are same make/size */

  /*
   * Deduce flash make
   */
  FlashMake = ReadBfrPtr[0];

  ReadId = ((ReadBfrPtr[0] << 16) | (ReadBfrPtr[1] << 8) | ReadBfrPtr[2]);
  /*
   * Assign corresponding index in the Flash configuration table
   */
  Status = CalculateFCTIndex(ReadId, &FCTIndex);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

int QspiPsu_NOR_Write_Page(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr
)
{
  u8 WriteEnableCmd;
  u8 ReadStatusCmd;
  u8 FlashStatus[2];
  u8 WriteCmdBfr[5];
  u32 RealAddr;
  u32 CmdByteCount;
  int Status;

  WriteEnableCmd = WRITE_ENABLE_CMD;
  /*
   * Translate address based on type of connection
   * If stacked assert the slave select based on address
   */
  RealAddr = GetRealAddr(QspiPsuPtr, Address);

  /*
   * Send the write enable command to the Flash so that it can be
   * written to, this needs to be sent as a separate transfer before
   * the write
   */
  FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 1;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  TransferInProgress = TRUE;

  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);

  WriteCmdBfr[COMMAND_OFFSET]   = WriteCmd;

  /* To be used only if 4B address program cmd is supported by flash */
  if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
    WriteCmdBfr[ADDRESS_1_OFFSET] =
        (u8)((RealAddr & 0xFF000000) >> 24);
    WriteCmdBfr[ADDRESS_2_OFFSET] =
        (u8)((RealAddr & 0xFF0000) >> 16);
    WriteCmdBfr[ADDRESS_3_OFFSET] =
        (u8)((RealAddr & 0xFF00) >> 8);
    WriteCmdBfr[ADDRESS_4_OFFSET] =
        (u8)(RealAddr & 0xFF);
    CmdByteCount = 5;
  } else {
    WriteCmdBfr[ADDRESS_1_OFFSET] =
        (u8)((RealAddr & 0xFF0000) >> 16);
    WriteCmdBfr[ADDRESS_2_OFFSET] =
        (u8)((RealAddr & 0xFF00) >> 8);
    WriteCmdBfr[ADDRESS_3_OFFSET] =
        (u8)(RealAddr & 0xFF);
    CmdByteCount = 4;
  }

  FlashMsg[0].TxBfrPtr = WriteCmdBfr;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = CmdByteCount;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  FlashMsg[1].TxBfrPtr = WriteBfrPtr;
  FlashMsg[1].RxBfrPtr = NULL;
  FlashMsg[1].ByteCount = ByteCount;
  FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
  if (QspiPsuPtr->Config.ConnectionMode ==
      XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
  }

  TransferInProgress = TRUE;

  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  while (TransferInProgress);

  /*
   * Wait for the write command to the Flash to be completed, it takes
   * some time for the data to be written
   */
  while (1) {
    ReadStatusCmd = StatusCmd;
    FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = FlashStatus;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      if (FSRFlag) {
        FlashStatus[1] &= FlashStatus[0];
      } else {
        FlashStatus[1] |= FlashStatus[0];
      }
    }

    if (FSRFlag) {
      if ((FlashStatus[1] & 0x80) != 0) {
        break;
      }
    } else {
      if ((FlashStatus[1] & 0x01) == 0) {
        break;
      }
    }
  }

  return 0;
}

int QspiPsu_NOR_Write(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr
)
{
  int Status;
  size_t ByteCountRemaining = ByteCount;
  unsigned char *WriteBfrPartial = WriteBfrPtr;
  uint32_t AddressPartial = Address;
  uint32_t PageSize = Flash_Config_Table[FCTIndex].PageSize;
  if(QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    PageSize *= 2;
  }

  while (ByteCountRemaining > 0) {
    /* Get write boundary */
    size_t WriteChunkLen = RTEMS_ALIGN_UP(AddressPartial + 1, PageSize);

    /* Get offset to write boundary */
    WriteChunkLen -= (size_t)AddressPartial;

    /* Cap short writes */
    if (WriteChunkLen > ByteCountRemaining) {
      WriteChunkLen = ByteCountRemaining;
    }

    Status = QspiPsu_NOR_Write_Page(
      QspiPsuPtr,
      AddressPartial,
      WriteChunkLen,
      WriteBfrPartial
    );
    if ( Status != XST_SUCCESS ) {
      return Status;
    }

    ByteCountRemaining -= WriteChunkLen;
    AddressPartial += WriteChunkLen;
    WriteBfrPartial += WriteChunkLen;
  }
  return Status;
}

int QspiPsu_NOR_Erase(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount
)
{
  u8 WriteEnableCmd;
  u8 ReadStatusCmd;
  u8 FlashStatus[2];
  int Sector;
  u32 RealAddr;
  u32 NumSect;
  int Status;
  u32 SectSize;

  WriteEnableCmd = WRITE_ENABLE_CMD;

  if(QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    SectSize = (Flash_Config_Table[FCTIndex]).SectSize * 2;
    NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
  } else if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
    NumSect = (Flash_Config_Table[FCTIndex]).NumSect * 2;
    SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
  } else {
    SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
    NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
  }

  /*
   * If erase size is same as the total size of the flash, use bulk erase
   * command or die erase command multiple times as required
   */
  if (ByteCount == NumSect * SectSize) {

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_STACKED) {
      XQspiPsu_SelectFlash(QspiPsuPtr,
        XQSPIPSU_SELECT_FLASH_CS_LOWER,
        XQSPIPSU_SELECT_FLASH_BUS_LOWER);
    }

    if (Flash_Config_Table[FCTIndex].NumDie == 1) {
      /*
       * Call Bulk erase
       */
      BulkErase(QspiPsuPtr, CmdBfr);
    }

    if (Flash_Config_Table[FCTIndex].NumDie > 1) {
      /*
       * Call Die erase
       */
      DieErase(QspiPsuPtr, CmdBfr);
    }
    /*
     * If stacked mode, bulk erase second flash
     */
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_STACKED) {

      XQspiPsu_SelectFlash(QspiPsuPtr,
        XQSPIPSU_SELECT_FLASH_CS_UPPER,
        XQSPIPSU_SELECT_FLASH_BUS_LOWER);

      if (Flash_Config_Table[FCTIndex].NumDie == 1) {
        /*
         * Call Bulk erase
         */
        BulkErase(QspiPsuPtr, CmdBfr);
      }

      if (Flash_Config_Table[FCTIndex].NumDie > 1) {
        /*
         * Call Die erase
         */
        DieErase(QspiPsuPtr, CmdBfr);
      }
    }

    return 0;
  }

  /*
   * If the erase size is less than the total size of the flash, use
   * sector erase command
   */

  /*
   * Calculate no. of sectors to erase based on byte count
   */
  u32 SectorStartBase = RTEMS_ALIGN_DOWN(Address, SectSize);
  u32 SectorEndTop = RTEMS_ALIGN_UP(Address + ByteCount, SectSize);
  NumSect = (SectorEndTop - SectorStartBase)/SectSize;

  for (Sector = 0; Sector < NumSect; Sector++) {

    /*
     * Translate address based on type of connection
     * If stacked assert the slave select based on address
     */
    RealAddr = GetRealAddr(QspiPsuPtr, Address);

    /*
     * Send the write enable command to the Flash so that it can be
     * written to, this needs to be sent as a separate
     * transfer before the write
     */
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    CmdBfr[COMMAND_OFFSET]   = SectorEraseCmd;

    /*
     * To be used only if 4B address sector erase cmd is
     * supported by flash
     */
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
      CmdBfr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF000000) >> 24);
      CmdBfr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      CmdBfr[ADDRESS_3_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      CmdBfr[ADDRESS_4_OFFSET] =
          (u8)(RealAddr & 0xFF);
      FlashMsg[0].ByteCount = 5;
    } else {
      CmdBfr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      CmdBfr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      CmdBfr[ADDRESS_3_OFFSET] =
          (u8)(RealAddr & 0xFF);
      FlashMsg[0].ByteCount = 4;
    }

    FlashMsg[0].TxBfrPtr = CmdBfr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    /*
     * Wait for the erase command to be completed
     */
    while (1) {
      ReadStatusCmd = StatusCmd;
      FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
      FlashMsg[0].RxBfrPtr = NULL;
      FlashMsg[0].ByteCount = 1;
      FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = FlashStatus;
      FlashMsg[1].ByteCount = 2;
      FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
      if (QspiPsuPtr->Config.ConnectionMode ==
          XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
      }

      TransferInProgress = TRUE;
      Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
          FlashMsg, 2);
      if (Status != XST_SUCCESS) {
        return XST_FAILURE;
      }
      while (TransferInProgress);

      if (QspiPsuPtr->Config.ConnectionMode ==
          XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        if (FSRFlag) {
          FlashStatus[1] &= FlashStatus[0];
        } else {
          FlashStatus[1] |= FlashStatus[0];
        }
      }

      if (FSRFlag) {
        if ((FlashStatus[1] & 0x80) != 0) {
          break;
        }
      } else {
        if ((FlashStatus[1] & 0x01) == 0) {
          break;
        }
      }
    }
    Address += SectSize;
  }

  return 0;
}

int QspiPsu_NOR_Read(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 **ReadBfrPtr
)
{
  u32 RealAddr;
  u32 DiscardByteCnt;
  u32 FlashMsgCnt;
  int Status;

  *ReadBfrPtr = ReadBuffer;

  /* Check die boundary conditions if required for any flash */
  if (Flash_Config_Table[FCTIndex].NumDie > 1) {

    Status = MultiDieRead(QspiPsuPtr, Address, ByteCount, ReadCmd,
              CmdBfr, *ReadBfrPtr);
    if (Status != XST_SUCCESS)
      return XST_FAILURE;
  } else {
    /* For Dual Stacked, split and read for boundary crossing */
    /*
     * Translate address based on type of connection
     * If stacked assert the slave select based on address
     */
    RealAddr = GetRealAddr(QspiPsuPtr, Address);

    CmdBfr[COMMAND_OFFSET]   = ReadCmd;
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
      CmdBfr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF000000) >> 24);
      CmdBfr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      CmdBfr[ADDRESS_3_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      CmdBfr[ADDRESS_4_OFFSET] =
          (u8)(RealAddr & 0xFF);
      DiscardByteCnt = 5;
    } else {
      CmdBfr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      CmdBfr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      CmdBfr[ADDRESS_3_OFFSET] =
          (u8)(RealAddr & 0xFF);
      DiscardByteCnt = 4;
    }

    FlashMsg[0].TxBfrPtr = CmdBfr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = DiscardByteCnt;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsgCnt = 1;

    /* It is recommended to have a separate entry for dummy */
    if (ReadCmd == FAST_READ_CMD || ReadCmd == DUAL_READ_CMD ||
        ReadCmd == QUAD_READ_CMD || ReadCmd == FAST_READ_CMD_4B ||
        ReadCmd == DUAL_READ_CMD_4B ||
        ReadCmd == QUAD_READ_CMD_4B) {
      /* Update Dummy cycles as per flash specs for QUAD IO */

      /*
       * It is recommended that Bus width value during dummy
       * phase should be same as data phase
       */
      if (ReadCmd == FAST_READ_CMD ||
          ReadCmd == FAST_READ_CMD_4B) {
        FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      }

      if (ReadCmd == DUAL_READ_CMD ||
          ReadCmd == DUAL_READ_CMD_4B) {
        FlashMsg[1].BusWidth =
          XQSPIPSU_SELECT_MODE_DUALSPI;
      }

      if (ReadCmd == QUAD_READ_CMD ||
          ReadCmd == QUAD_READ_CMD_4B) {
        FlashMsg[1].BusWidth =
          XQSPIPSU_SELECT_MODE_QUADSPI;
      }

      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = NULL;
      FlashMsg[1].ByteCount = DUMMY_CLOCKS;
      FlashMsg[1].Flags = 0;

      FlashMsgCnt++;
    }

    /* Dummy cycles need to be changed as per flash specs
     * for QUAD IO
     */
    if (ReadCmd == FAST_READ_CMD || ReadCmd == FAST_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_SPI;

    if (ReadCmd == DUAL_READ_CMD || ReadCmd == DUAL_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_DUALSPI;

    if (ReadCmd == QUAD_READ_CMD || ReadCmd == QUAD_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_QUADSPI;

    FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].RxBfrPtr = *ReadBfrPtr;
    FlashMsg[FlashMsgCnt].ByteCount = ByteCount;
    FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[FlashMsgCnt].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg,
                FlashMsgCnt + 1);
    if (Status != XST_SUCCESS)
      return XST_FAILURE;

    while (TransferInProgress);

  }
  rtems_cache_invalidate_multiple_data_lines(ReadBuffer, ByteCount);
  return 0;
}

/*****************************************************************************/
/**
 *
 * This function performs a read operation for multi die flash devices.
 * Default setting is in DMA mode.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	Command is the command used to read data from the flash.
 *		Supports normal, fast, dual and quad read commands.
 * @param	WriteBfrPtr is pointer to the write buffer which contains data to be
 *		transmitted
 * @param	ReadBfrPtr is pointer to the read buffer to which valid received data
 *		should be written
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
static int MultiDieRead(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 Command,
  u8 *WriteBfrPtr,
  u8 *ReadBfrPtr
)
{
  u32 RealAddr;
  u32 DiscardByteCnt;
  u32 FlashMsgCnt;
  int Status;
  u32 cur_bank = 0;
  u32 nxt_bank = 0;
  u32 bank_size;
  u32 remain_len = ByteCount;
  u32 data_len;
  u32 transfer_len;
  u8 *ReadBuffer = ReadBfrPtr;

  /*
   * Some flash devices like N25Q512 have multiple dies
   * in it. Read operation in these devices is bounded
   * by its die segment. In a continuous read, across
   * multiple dies, when the last byte of the selected
   * die segment is read, the next byte read is the
   * first byte of the same die segment. This is Die
   * cross over issue. So to handle this issue, split
   * a read transaction, that spans across multiple
   * banks, into one read per bank. Bank size is 16MB
   * for single and dual stacked mode and 32MB for dual
   * parallel mode.
   */
  if (QspiPsuPtr->Config.ConnectionMode ==
      XQSPIPSU_CONNECTION_MODE_PARALLEL)
    bank_size = SIXTEENMB << 1;
  else
    bank_size = SIXTEENMB;

  while (remain_len) {
    cur_bank = Address / bank_size;
    nxt_bank = (Address + remain_len) / bank_size;

    if (cur_bank != nxt_bank) {
      transfer_len = (bank_size * (cur_bank  + 1)) - Address;
      if (remain_len < transfer_len)
        data_len = remain_len;
      else
        data_len = transfer_len;
    } else {
      data_len = remain_len;
    }
    /*
     * Translate address based on type of connection
     * If stacked assert the slave select based on address
     */
    RealAddr = GetRealAddr(QspiPsuPtr, Address);

    WriteBfrPtr[COMMAND_OFFSET]   = Command;
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
      WriteBfrPtr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF000000) >> 24);
      WriteBfrPtr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      WriteBfrPtr[ADDRESS_3_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      WriteBfrPtr[ADDRESS_4_OFFSET] =
          (u8)(RealAddr & 0xFF);
      DiscardByteCnt = 5;
    } else {
      WriteBfrPtr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      WriteBfrPtr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      WriteBfrPtr[ADDRESS_3_OFFSET] =
          (u8)(RealAddr & 0xFF);
      DiscardByteCnt = 4;
    }

    FlashMsg[0].TxBfrPtr = WriteBfrPtr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = DiscardByteCnt;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsgCnt = 1;

    /* It is recommended to have a separate entry for dummy */
    if (Command == FAST_READ_CMD || Command == DUAL_READ_CMD ||
        Command == QUAD_READ_CMD || Command == FAST_READ_CMD_4B ||
        Command == DUAL_READ_CMD_4B ||
        Command == QUAD_READ_CMD_4B) {
      /* Update Dummy cycles as per flash specs for QUAD IO */

      /*
       * It is recommended that Bus width value during dummy
       * phase should be same as data phase
       */
      if (Command == FAST_READ_CMD ||
          Command == FAST_READ_CMD_4B) {
        FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      }

      if (Command == DUAL_READ_CMD ||
          Command == DUAL_READ_CMD_4B) {
        FlashMsg[1].BusWidth =
          XQSPIPSU_SELECT_MODE_DUALSPI;
      }

      if (Command == QUAD_READ_CMD ||
          Command == QUAD_READ_CMD_4B) {
        FlashMsg[1].BusWidth =
          XQSPIPSU_SELECT_MODE_QUADSPI;
      }

      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = NULL;
      FlashMsg[1].ByteCount = DUMMY_CLOCKS;
      FlashMsg[1].Flags = 0;

      FlashMsgCnt++;
    }

    /* Dummy cycles need to be changed as per flash
     * specs for QUAD IO
     */
    if (Command == FAST_READ_CMD || Command == FAST_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_SPI;

    if (Command == DUAL_READ_CMD || Command == DUAL_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_DUALSPI;

    if (Command == QUAD_READ_CMD || Command == QUAD_READ_CMD_4B)
      FlashMsg[FlashMsgCnt].BusWidth =
        XQSPIPSU_SELECT_MODE_QUADSPI;

    FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].RxBfrPtr = ReadBuffer;
    FlashMsg[FlashMsgCnt].ByteCount = data_len;
    FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL)
      FlashMsg[FlashMsgCnt].Flags |=
        XQSPIPSU_MSG_FLAG_STRIPE;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg,
                FlashMsgCnt + 1);
    if (Status != XST_SUCCESS)
      return XST_FAILURE;

    while (TransferInProgress);


    ReadBuffer += data_len;
    Address += data_len;
    remain_len -= data_len;
  }
  rtems_cache_invalidate_multiple_data_lines(ReadBfrPtr, ByteCount);
  return 0;
}

/*****************************************************************************/
/**
 *
 * This functions performs a bulk erase operation when the
 * flash device has a single die. Works for both Spansion and Micron
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WriteBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
static int BulkErase(
  XQspiPsu *QspiPsuPtr,
  u8 *WriteBfrPtr
)
{
  u8 WriteEnableCmd;
  u8 ReadStatusCmd;
  u8 FlashStatus[2];
  int Status;

  WriteEnableCmd = WRITE_ENABLE_CMD;
  /*
   * Send the write enable command to the Flash so that it can be
   * written to, this needs to be sent as a separate transfer before
   * the write
   */
  FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 1;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }
  while (TransferInProgress);

  WriteBfrPtr[COMMAND_OFFSET]   = BULK_ERASE_CMD;
  FlashMsg[0].TxBfrPtr = WriteBfrPtr;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 1;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }
  while (TransferInProgress);

  /*
   * Wait for the write command to the Flash to be completed, it takes
   * some time for the data to be written
   */
  while (1) {
    ReadStatusCmd = StatusCmd;
    FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = FlashStatus;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      if (FSRFlag) {
        FlashStatus[1] &= FlashStatus[0];
      } else {
        FlashStatus[1] |= FlashStatus[0];
      }
    }

    if (FSRFlag) {
      if ((FlashStatus[1] & 0x80) != 0) {
        break;
      }
    } else {
      if ((FlashStatus[1] & 0x01) == 0) {
        break;
      }
    }
  }

  return 0;
}

/*****************************************************************************/
/**
 *
 * This functions performs a die erase operation on all the die in
 * the flash device. This function uses the die erase command for
 * Micron 512Mbit and 1Gbit
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	WriteBfrPtr is the pointer to command+address to be sent
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
static int DieErase(
  XQspiPsu *QspiPsuPtr,
  u8 *WriteBfrPtr
)
{
  u8 WriteEnableCmd;
  u8 DieCnt;
  u8 ReadStatusCmd;
  u8 FlashStatus[2];
  int Status;
  u32 DieSize = 0;
  u32 Address;
  u32 RealAddr;
  u32 SectSize = 0;
  u32 NumSect = 0;

  WriteEnableCmd = WRITE_ENABLE_CMD;

  if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    SectSize = (Flash_Config_Table[FCTIndex]).SectSize * 2;
  } else if (QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
    NumSect = (Flash_Config_Table[FCTIndex]).NumSect * 2;
  } else {
    SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
    NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
  }
  DieSize = (NumSect * SectSize) / Flash_Config_Table[FCTIndex].NumDie;

  for (DieCnt = 0;
    DieCnt < Flash_Config_Table[FCTIndex].NumDie;
    DieCnt++) {
    /*
     * Send the write enable command to the Flash so that it can be
     * written to, this needs to be sent as a separate transfer
     * before the write
     */
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    WriteBfrPtr[COMMAND_OFFSET]   = DIE_ERASE_CMD;

    Address = DieSize * DieCnt;
    RealAddr = GetRealAddr(QspiPsuPtr, Address);
    /*
     * To be used only if 4B address sector erase cmd is
     * supported by flash
     */
    if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
      WriteBfrPtr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF000000) >> 24);
      WriteBfrPtr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      WriteBfrPtr[ADDRESS_3_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      WriteBfrPtr[ADDRESS_4_OFFSET] =
          (u8)(RealAddr & 0xFF);
      FlashMsg[0].ByteCount = 5;
    } else {
      WriteBfrPtr[ADDRESS_1_OFFSET] =
          (u8)((RealAddr & 0xFF0000) >> 16);
      WriteBfrPtr[ADDRESS_2_OFFSET] =
          (u8)((RealAddr & 0xFF00) >> 8);
      WriteBfrPtr[ADDRESS_3_OFFSET] =
          (u8)(RealAddr & 0xFF);
      FlashMsg[0].ByteCount = 4;
    }
    FlashMsg[0].TxBfrPtr = WriteBfrPtr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    /*
     * Wait for the write command to the Flash to be completed,
     * it takes some time for the data to be written
     */
    while (1) {
      ReadStatusCmd = StatusCmd;
      FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
      FlashMsg[0].RxBfrPtr = NULL;
      FlashMsg[0].ByteCount = 1;
      FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = FlashStatus;
      FlashMsg[1].ByteCount = 2;
      FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
      if (QspiPsuPtr->Config.ConnectionMode ==
          XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
      }

      TransferInProgress = TRUE;
      Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
          FlashMsg, 2);
      if (Status != XST_SUCCESS) {
        return XST_FAILURE;
      }
      while (TransferInProgress);

      if (QspiPsuPtr->Config.ConnectionMode ==
          XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        if (FSRFlag) {
          FlashStatus[1] &= FlashStatus[0];
        } else {
          FlashStatus[1] |= FlashStatus[0];
        }
      }

      if (FSRFlag) {
        if ((FlashStatus[1] & 0x80) != 0) {
          break;
        }
      } else {
        if ((FlashStatus[1] & 0x01) == 0) {
          break;
        }
      }
    }
  }

  return 0;
}

/*****************************************************************************/
/**
 *
 * This functions translates the address based on the type of interconnection.
 * In case of stacked, this function asserts the corresponding slave select.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address which is to be accessed (for erase, write or read)
 *
 * @return	RealAddr is the translated address - for single it is unchanged;
 *		for stacked, the lower flash size is subtracted;
 *		for parallel the address is divided by 2.
 *
 * @note	In addition to get the actual address to work on flash this
 *		function also selects the CS and BUS based on the configuration
 *		detected.
 *
 ******************************************************************************/
static u32 GetRealAddr(
  XQspiPsu *QspiPsuPtr,
  u32 Address
)
{
  u32 RealAddr = 0;

  switch (QspiPsuPtr->Config.ConnectionMode) {
  case XQSPIPSU_CONNECTION_MODE_SINGLE:
    XQspiPsu_SelectFlash(QspiPsuPtr,
      XQSPIPSU_SELECT_FLASH_CS_LOWER,
      XQSPIPSU_SELECT_FLASH_BUS_LOWER);
    RealAddr = Address;
    break;
  case XQSPIPSU_CONNECTION_MODE_STACKED:
    /* Select lower or upper Flash based on sector address */
    if (Address & Flash_Config_Table[FCTIndex].FlashDeviceSize) {

      XQspiPsu_SelectFlash(QspiPsuPtr,
        XQSPIPSU_SELECT_FLASH_CS_UPPER,
        XQSPIPSU_SELECT_FLASH_BUS_LOWER);
      /*
       * Subtract first flash size when accessing second flash
       */
      RealAddr = Address &
        (~Flash_Config_Table[FCTIndex].FlashDeviceSize);
    }else{
      /*
       * Set selection to L_PAGE
       */
      XQspiPsu_SelectFlash(QspiPsuPtr,
        XQSPIPSU_SELECT_FLASH_CS_LOWER,
        XQSPIPSU_SELECT_FLASH_BUS_LOWER);

      RealAddr = Address;

    }
    break;
  case XQSPIPSU_CONNECTION_MODE_PARALLEL:
    /*
     * The effective address in each flash is the actual
     * address / 2
     */
    XQspiPsu_SelectFlash(QspiPsuPtr,
        XQSPIPSU_SELECT_FLASH_CS_BOTH,
        XQSPIPSU_SELECT_FLASH_BUS_BOTH);
    RealAddr = Address / 2;
    break;
  default:
    /* RealAddr wont be assigned in this case; */
  break;

  }

  return(RealAddr);
}

/*****************************************************************************/
/**
 *
 * This function setups the interrupt system for a QspiPsu device.
 *
 * @param	QspiPsuInstancePtr is a pointer to the instance of the
 *		QspiPsu device.
 * @param	QspiPsuIntrId is the interrupt Id for an QSPIPSU device.
 *
 * @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
static int QspiPsuSetupIntrSystem(
  XQspiPsu *QspiPsuInstancePtr,
  u16 QspiPsuIntrId
)
{
  return rtems_interrupt_handler_install(
    QspiPsuIntrId,
    NULL,
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) XQspiPsu_InterruptHandler,
    QspiPsuInstancePtr
  );
}

/*****************************************************************************/
/**
 * @brief
 * This API enters the flash device into 4 bytes addressing mode.
 * As per the Micron and ISSI spec, before issuing the command
 * to enter into 4 byte addr mode, a write enable command is issued.
 * For Macronix and Winbond flash parts write
 * enable is not required.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Enable is a either 1 or 0 if 1 then enters 4 byte if 0 exits.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if it fails.
 *
 *
 ******************************************************************************/
static int FlashEnterExit4BAddMode(
  XQspiPsu *QspiPsuPtr,
  unsigned int Enable
)
{
  int Status;
  u8 WriteEnableCmd;
  u8 Cmd;
  u8 WriteDisableCmd;
  u8 ReadStatusCmd;
  u8 WriteBuffer[2] = {0};
  u8 FlashStatus[2] = {0};

  if (Enable) {
    Cmd = ENTER_4B_ADDR_MODE;
  } else {
    if (FlashMake == ISSI_ID_BYTE0)
      Cmd = EXIT_4B_ADDR_MODE_ISSI;
    else
      Cmd = EXIT_4B_ADDR_MODE;
  }

  switch (FlashMake) {
  case ISSI_ID_BYTE0:
  case MICRON_ID_BYTE0:
    WriteEnableCmd = WRITE_ENABLE_CMD;
    GetRealAddr(QspiPsuPtr, TEST_ADDRESS);
    /*
     * Send the write enable command to the Flash so that it can be
     * written to, this needs to be sent as a separate transfer
     * before the write
     */
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    break;

  case SPANSION_ID_BYTE0:

    /* Read Extended Addres Register */
    WriteBuffer[0] = BANK_REG_RD;
    FlashMsg[0].TxBfrPtr = &WriteBuffer[0];
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = &WriteBuffer[1];
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    if (Enable) {
      WriteBuffer[0] = BANK_REG_WR;
      WriteBuffer[1] |= 1 << 7;
    } else {
      WriteBuffer[0] = BANK_REG_WR;
      WriteBuffer[1] &= ~(0x01 << 7);
    }

    FlashMsg[0].TxBfrPtr = &WriteBuffer[0];
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[1].TxBfrPtr = &WriteBuffer[1];
    FlashMsg[2].RxBfrPtr = NULL;
    FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[2].Flags = XQSPIPSU_MSG_FLAG_TX;
    FlashMsg[2].ByteCount = 1;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    WriteBuffer[0] = BANK_REG_RD;
    FlashMsg[0].TxBfrPtr = &WriteBuffer[0];
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = &FlashStatus[0];
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    return Status;

  default:
    /*
     * For Macronix and Winbond flash parts
     * Write enable command is not required.
     */
    break;
  }

  GetRealAddr(QspiPsuPtr, TEST_ADDRESS);

  FlashMsg[0].TxBfrPtr = &Cmd;
  FlashMsg[0].RxBfrPtr = NULL;
  FlashMsg[0].ByteCount = 1;
  FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }
  while (TransferInProgress);

  while (1) {
    ReadStatusCmd = StatusCmd;

    FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = FlashStatus;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      if (FSRFlag) {
        FlashStatus[1] &= FlashStatus[0];
      } else {
        FlashStatus[1] |= FlashStatus[0];
      }
    }

    if (FSRFlag) {
      if ((FlashStatus[1] & 0x80) != 0) {
        break;
      }
    } else {
      if ((FlashStatus[1] & 0x01) == 0) {
        break;
      }
    }
  }

  switch (FlashMake) {
  case ISSI_ID_BYTE0:
  case MICRON_ID_BYTE0:
    WriteDisableCmd = WRITE_DISABLE_CMD;
    GetRealAddr(QspiPsuPtr, TEST_ADDRESS);
    /*
     * Send the write enable command to the Flash so that it can be
     * written to, this needs to be sent as a separate transfer
     * before the write
     */
    FlashMsg[0].TxBfrPtr = &WriteDisableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    break;

  default:
    /*
     * For Macronix and Winbond flash parts
     * Write disable command is not required.
     */
    break;
  }
  return Status;
}

/*****************************************************************************/
/**
 * @brief
 * This API enables Quad mode for the flash parts which require to enable quad
 * mode before using Quad commands.
 * For S25FL-L series flash parts this is required as the default configuration
 * is x1/x2 mode.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if it fails.
 *
 *
 ******************************************************************************/
static int FlashEnableQuadMode(XQspiPsu *QspiPsuPtr)
{
  int Status;
  u8 WriteEnableCmd;
  u8 ReadStatusCmd;
  u8 FlashStatus[2];
  u8 StatusRegVal;
  u8 WriteBuffer[3] = {0};

  switch (FlashMake) {
  case SPANSION_ID_BYTE0:
    TxBfrPtr = READ_CONFIG_CMD;
    FlashMsg[0].TxBfrPtr = &TxBfrPtr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = &WriteBuffer[2];
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
        FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    WriteEnableCmd = WRITE_ENABLE_CMD;
    /*
     * Send the write enable command to the Flash
     * so that it can be written to, this needs
     * to be sent as a separate transfer before
     * the write
     */
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
        FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    GetRealAddr(QspiPsuPtr, TEST_ADDRESS);

    WriteBuffer[0] = WRITE_CONFIG_CMD;
    WriteBuffer[1] |= 0x02;
    WriteBuffer[2] |= 0x01 << 1;

    FlashMsg[0].TxBfrPtr = &WriteBuffer[0];
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[1].TxBfrPtr = &WriteBuffer[1];
    FlashMsg[1].RxBfrPtr = NULL;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
        FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    while (1) {
      TxBfrPtr = READ_STATUS_CMD;
      FlashMsg[0].TxBfrPtr = &TxBfrPtr;
      FlashMsg[0].RxBfrPtr = NULL;
      FlashMsg[0].ByteCount = 1;
      FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = FlashStatus;
      FlashMsg[1].ByteCount = 2;
      FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

      TransferInProgress = TRUE;
      Status = XQspiPsu_InterruptTransfer(QspiPsuPtr,
          FlashMsg, 2);
      if (Status != XST_SUCCESS) {
        return XST_FAILURE;
      }
      while (TransferInProgress);
      if (QspiPsuPtr->Config.ConnectionMode ==
            XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        if (FSRFlag) {
          FlashStatus[1] &= FlashStatus[0];
        }else {
          FlashStatus[1] |= FlashStatus[0];
        }
      }

      if ((FlashStatus[1] & 0x01) == 0x00)
        break;
    }
    TxBfrPtr = READ_CONFIG_CMD;
    FlashMsg[0].TxBfrPtr = &TxBfrPtr;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = ReadBfrPtr;
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    break;
  case ISSI_ID_BYTE0:
    /*
     * Read Status Register to a buffer
     */
    ReadStatusCmd = READ_STATUS_CMD;
    FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = FlashStatus;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      if (FSRFlag) {
        FlashStatus[1] &= FlashStatus[0];
      } else {
        FlashStatus[1] |= FlashStatus[0];
      }
    }
    /*
     * Set Quad Enable Bit in the buffer
     */
    StatusRegVal = FlashStatus[1];
    StatusRegVal |= 0x1 << QUAD_MODE_ENABLE_BIT;

    /*
     * Write enable
     */
    WriteEnableCmd = WRITE_ENABLE_CMD;
    /*
    * Send the write enable command to the Flash so that it can be
    * written to, this needs to be sent as a separate transfer
    * before the write
    */
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    /*
     * Write Status register
     */
    WriteBuffer[COMMAND_OFFSET] = WRITE_STATUS_CMD;
    FlashMsg[0].TxBfrPtr = WriteBuffer;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = &StatusRegVal;
    FlashMsg[1].RxBfrPtr = NULL;
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
    }
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    /*
     * Write Disable
     */
    WriteEnableCmd = WRITE_DISABLE_CMD;
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    break;

  case WINBOND_ID_BYTE0:
    ReadStatusCmd = READ_STATUS_REG_2_CMD;
    FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    FlashMsg[1].TxBfrPtr = NULL;
    FlashMsg[1].RxBfrPtr = FlashStatus;
    FlashMsg[1].ByteCount = 2;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    if (QspiPsuPtr->Config.ConnectionMode ==
      XQSPIPSU_CONNECTION_MODE_PARALLEL) {
      if (FSRFlag) {
        FlashStatus[1] &= FlashStatus[0];
      } else {
        FlashStatus[1] |= FlashStatus[0];
      }
    }
    /*
     * Set Quad Enable Bit in the buffer
     */
    StatusRegVal = FlashStatus[1];
    StatusRegVal |= 0x1 << WB_QUAD_MODE_ENABLE_BIT;
    /*
     * Write Enable
     */
    WriteEnableCmd = WRITE_ENABLE_CMD;
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 1);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    /*
     * Write Status register
     */
    WriteBuffer[COMMAND_OFFSET] = WRITE_STATUS_REG_2_CMD;
    FlashMsg[0].TxBfrPtr = WriteBuffer;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsg[1].TxBfrPtr = &StatusRegVal;
    FlashMsg[1].RxBfrPtr = NULL;
    FlashMsg[1].ByteCount = 1;
    FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);

    while (1) {
      ReadStatusCmd = READ_STATUS_CMD;
      FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
      FlashMsg[0].RxBfrPtr = NULL;
      FlashMsg[0].ByteCount = 1;
      FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
      FlashMsg[1].TxBfrPtr = NULL;
      FlashMsg[1].RxBfrPtr = FlashStatus;
      FlashMsg[1].ByteCount = 2;
      FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
      FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
      TransferInProgress = TRUE;
      Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
      if (Status != XST_SUCCESS) {
        return XST_FAILURE;
      }
      while (TransferInProgress);

      if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL) {
        if (FSRFlag) {
          FlashStatus[1] &= FlashStatus[0];
        } else {
          FlashStatus[1] |= FlashStatus[0];
        }
      }
      if ((FlashStatus[1] & 0x01) == 0x00) {
        break;
      }
    }
    /*
     * Write Disable
     */
    WriteEnableCmd = WRITE_DISABLE_CMD;
    FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
    FlashMsg[0].RxBfrPtr = NULL;
    FlashMsg[0].ByteCount = 1;
    FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg, 2);
    if (Status != XST_SUCCESS) {
      return XST_FAILURE;
    }
    while (TransferInProgress);
    break;

  default:
    /*
     * Currently only S25FL-L series requires the
     * Quad enable bit to be set to 1.
     */
    Status = XST_SUCCESS;
    break;
  }

  return Status;
}

static int MultiDieReadEcc(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr,
  u8 *ReadBfrPtr
);

int QspiPsu_NOR_Read_Ecc(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u8 *ReadBfrPtr
)
{
  u32 RealAddr;
  u32 DiscardByteCnt;
  u32 FlashMsgCnt;
  u8  EccBuffer[16];
  int ByteCount = sizeof(EccBuffer);
  int Status;

  /* Check die boundary conditions if required for any flash */
  if (Flash_Config_Table[FCTIndex].NumDie > 1) {

    Status = MultiDieReadEcc(QspiPsuPtr, Address, ByteCount,
              CmdBfr, EccBuffer);
    if (Status == XST_SUCCESS) {
      /* All bytes are the same, so copy one return byte into the output buffer */
      *ReadBfrPtr = EccBuffer[0];
    }
    return Status;
  }

  /* For Dual Stacked, split and read for boundary crossing */
  /*
   * Translate address based on type of connection
   * If stacked assert the slave select based on address
   */
  RealAddr = GetRealAddr(QspiPsuPtr, Address);

  CmdBfr[COMMAND_OFFSET]   = READ_ECCSR;
  CmdBfr[ADDRESS_1_OFFSET] =
      (u8)((RealAddr & 0xFF000000) >> 24);
  CmdBfr[ADDRESS_2_OFFSET] =
      (u8)((RealAddr & 0xFF0000) >> 16);
  CmdBfr[ADDRESS_3_OFFSET] =
      (u8)((RealAddr & 0xFF00) >> 8);
  CmdBfr[ADDRESS_4_OFFSET] =
      (u8)(RealAddr & 0xF0);
  DiscardByteCnt = 5;

  FlashMsgCnt = 0;

  FlashMsg[FlashMsgCnt].TxBfrPtr = CmdBfr;
  FlashMsg[FlashMsgCnt].RxBfrPtr = NULL;
  FlashMsg[FlashMsgCnt].ByteCount = DiscardByteCnt;
  FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_TX;

  FlashMsgCnt++;

  FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
  FlashMsg[FlashMsgCnt].RxBfrPtr = NULL;
  FlashMsg[FlashMsgCnt].ByteCount = DUMMY_CLOCKS;
  FlashMsg[FlashMsgCnt].Flags = 0;

  FlashMsgCnt++;

  FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
  FlashMsg[FlashMsgCnt].RxBfrPtr = EccBuffer;
  FlashMsg[FlashMsgCnt].ByteCount = ByteCount;
  FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
  FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

  if (QspiPsuPtr->Config.ConnectionMode ==
      XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    FlashMsg[FlashMsgCnt].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
  }

  TransferInProgress = TRUE;
  Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg,
              FlashMsgCnt + 1);
  if (Status == XST_SUCCESS) {
    while (TransferInProgress);

    /* All bytes are the same, so copy one return byte into the output buffer */
    *ReadBfrPtr = EccBuffer[0];
  }

  return Status;
}

/*****************************************************************************/
/**
 *
 * This function performs an ECC read operation for multi die flash devices.
 * Default setting is in DMA mode.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 * @param	WriteBfrPtr is pointer to the write buffer which contains data to be
 *		transmitted
 * @param	ReadBfrPtr is pointer to the read buffer to which valid received data
 *		should be written
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
static int MultiDieReadEcc(
  XQspiPsu *QspiPsuPtr,
  u32 Address,
  u32 ByteCount,
  u8 *WriteBfrPtr,
  u8 *ReadBuffer
)
{
  u32 RealAddr;
  u32 DiscardByteCnt;
  u32 FlashMsgCnt;
  int Status;
  u32 cur_bank = 0;
  u32 nxt_bank = 0;
  u32 bank_size;
  u32 remain_len = ByteCount;
  u32 data_len;
  u32 transfer_len;

  /*
   * Some flash devices like N25Q512 have multiple dies
   * in it. Read operation in these devices is bounded
   * by its die segment. In a continuous read, across
   * multiple dies, when the last byte of the selected
   * die segment is read, the next byte read is the
   * first byte of the same die segment. This is Die
   * cross over issue. So to handle this issue, split
   * a read transaction, that spans across multiple
   * banks, into one read per bank. Bank size is 16MB
   * for single and dual stacked mode and 32MB for dual
   * parallel mode.
   */
  if (QspiPsuPtr->Config.ConnectionMode ==
      XQSPIPSU_CONNECTION_MODE_PARALLEL)
    bank_size = SIXTEENMB << 1;
  else
    bank_size = SIXTEENMB;

  while (remain_len) {
    cur_bank = Address / bank_size;
    nxt_bank = (Address + remain_len) / bank_size;

    if (cur_bank != nxt_bank) {
      transfer_len = (bank_size * (cur_bank  + 1)) - Address;
      if (remain_len < transfer_len)
        data_len = remain_len;
      else
        data_len = transfer_len;
    } else {
      data_len = remain_len;
    }
    /*
     * Translate address based on type of connection
     * If stacked assert the slave select based on address
     */
    RealAddr = GetRealAddr(QspiPsuPtr, Address);

    WriteBfrPtr[COMMAND_OFFSET]   = READ_ECCSR;
    WriteBfrPtr[ADDRESS_1_OFFSET] =
        (u8)((RealAddr & 0xFF000000) >> 24);
    WriteBfrPtr[ADDRESS_2_OFFSET] =
        (u8)((RealAddr & 0xFF0000) >> 16);
    WriteBfrPtr[ADDRESS_3_OFFSET] =
        (u8)((RealAddr & 0xFF00) >> 8);
    WriteBfrPtr[ADDRESS_4_OFFSET] =
        (u8)(RealAddr & 0xF0);
    DiscardByteCnt = 5;

    FlashMsgCnt = 0;

    FlashMsg[FlashMsgCnt].TxBfrPtr = WriteBfrPtr;
    FlashMsg[FlashMsgCnt].RxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].ByteCount = DiscardByteCnt;
    FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_TX;

    FlashMsgCnt++;

    FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].RxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].ByteCount = DUMMY_CLOCKS;
    FlashMsg[FlashMsgCnt].Flags = 0;

    FlashMsgCnt++;

    FlashMsg[FlashMsgCnt].TxBfrPtr = NULL;
    FlashMsg[FlashMsgCnt].RxBfrPtr = ReadBuffer;
    FlashMsg[FlashMsgCnt].ByteCount = data_len;
    FlashMsg[FlashMsgCnt].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
    FlashMsg[FlashMsgCnt].Flags = XQSPIPSU_MSG_FLAG_RX;

    if (QspiPsuPtr->Config.ConnectionMode ==
        XQSPIPSU_CONNECTION_MODE_PARALLEL)
      FlashMsg[FlashMsgCnt].Flags |=
        XQSPIPSU_MSG_FLAG_STRIPE;

    TransferInProgress = TRUE;
    Status = XQspiPsu_InterruptTransfer(QspiPsuPtr, FlashMsg,
                FlashMsgCnt + 1);
    if (Status != XST_SUCCESS)
      return XST_FAILURE;

    while (TransferInProgress);

    ReadBuffer += data_len;
    Address += data_len;
    remain_len -= data_len;
  }
  return 0;
}

u32 QspiPsu_NOR_Get_Sector_Size(XQspiPsu *QspiPsuPtr)
{
  if(QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
    return Flash_Config_Table[FCTIndex].SectSize * 2;
  }
  return Flash_Config_Table[FCTIndex].SectSize;
}

u32 QspiPsu_NOR_Get_Device_Size(XQspiPsu *QspiPsuPtr)
{
  if(QspiPsuPtr->Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
    return Flash_Config_Table[FCTIndex].FlashDeviceSize * 2;
  }
  return Flash_Config_Table[FCTIndex].FlashDeviceSize;
}
