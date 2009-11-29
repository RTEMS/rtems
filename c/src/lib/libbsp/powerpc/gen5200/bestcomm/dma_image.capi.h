#ifndef __DMA_IMAGE_CAPI_H
#define __DMA_IMAGE_CAPI_H 1

/******************************************************************************
*
* Copyright (c) 2004 Freescale Semiconductor, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/


#include "dma_image.h"

typedef enum {
	TASK_PCI_TX,
	TASK_PCI_RX,
	TASK_FEC_TX,
	TASK_FEC_RX,
	TASK_LPC,
	TASK_ATA,
	TASK_CRC16_DP_0,
	TASK_CRC16_DP_1,
	TASK_GEN_DP_0,
	TASK_GEN_DP_1,
	TASK_GEN_DP_2,
	TASK_GEN_DP_3,
	TASK_GEN_TX_BD,
	TASK_GEN_RX_BD,
	TASK_GEN_DP_BD_0,
	TASK_GEN_DP_BD_1
} TaskName_t;

TaskId TaskSetup_TASK_PCI_TX    (TASK_PCI_TX_api_t    *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_PCI_RX    (TASK_PCI_RX_api_t    *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_FEC_TX    (TASK_FEC_TX_api_t    *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_FEC_RX    (TASK_FEC_RX_api_t    *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_LPC       (TASK_LPC_api_t       *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_ATA       (TASK_ATA_api_t       *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_CRC16_DP_0(TASK_CRC16_DP_0_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_CRC16_DP_1(TASK_CRC16_DP_1_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_0  (TASK_GEN_DP_0_api_t  *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_1  (TASK_GEN_DP_1_api_t  *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_2  (TASK_GEN_DP_2_api_t  *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_3  (TASK_GEN_DP_3_api_t  *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_TX_BD (TASK_GEN_TX_BD_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_RX_BD (TASK_GEN_RX_BD_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_BD_0(TASK_GEN_DP_BD_0_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);
TaskId TaskSetup_TASK_GEN_DP_BD_1(TASK_GEN_DP_BD_1_api_t *TaskAPI,
                                 TaskSetupParamSet_t  *TaskSetupParams);

#endif	/* __DMA_IMAGE_CAPI_H */

