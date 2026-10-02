/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsReqIdent
 */

/*
 * Copyright (C) 2020, 2026 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TR_OBJECT_IDENT_H
#define _TR_OBJECT_IDENT_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsReqIdent
 *
 * @{
 */

typedef enum {
  RtemsReqIdent_Pre_Name_Invalid,
  RtemsReqIdent_Pre_Name_Valid,
  RtemsReqIdent_Pre_Name_NA
} RtemsReqIdent_Pre_Name;

typedef enum {
  RtemsReqIdent_Pre_Node_Local,
  RtemsReqIdent_Pre_Node_Remote,
  RtemsReqIdent_Pre_Node_Invalid,
  RtemsReqIdent_Pre_Node_SearchAll,
  RtemsReqIdent_Pre_Node_SearchOther,
  RtemsReqIdent_Pre_Node_SearchLocal,
  RtemsReqIdent_Pre_Node_NA
} RtemsReqIdent_Pre_Node;

typedef enum {
  RtemsReqIdent_Pre_RemoteObj_Absent,
  RtemsReqIdent_Pre_RemoteObj_Exists,
  RtemsReqIdent_Pre_RemoteObj_NA
} RtemsReqIdent_Pre_RemoteObj;

typedef enum {
  RtemsReqIdent_Pre_Id_Valid,
  RtemsReqIdent_Pre_Id_Null,
  RtemsReqIdent_Pre_Id_NA
} RtemsReqIdent_Pre_Id;

typedef enum {
  RtemsReqIdent_Post_Status_Ok,
  RtemsReqIdent_Post_Status_InvAddr,
  RtemsReqIdent_Post_Status_InvName,
  RtemsReqIdent_Post_Status_InvNode,
  RtemsReqIdent_Post_Status_NA
} RtemsReqIdent_Post_Status;

typedef enum {
  RtemsReqIdent_Post_Id_Nop,
  RtemsReqIdent_Post_Id_Null,
  RtemsReqIdent_Post_Id_LocalObj,
  RtemsReqIdent_Post_Id_NA
} RtemsReqIdent_Post_Id;

/**
 * @brief Runs the parameterized test case.
 *
 * @param id_local_object is the identifier of an active object of the class
 *   under test.
 *
 * @param name_local_object is the name of the active object of the class under
 *   test.
 *
 * @param action is the action handler.
 */
void RtemsReqIdent_Run(
  rtems_id   id_local_object,
  rtems_name name_local_object,
  rtems_status_code ( *action )( rtems_name, uint32_t, rtems_id * )
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_OBJECT_IDENT_H */
