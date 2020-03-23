/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate User Extensions Configuration Options
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_EXTENSIONS_H
#define _RTEMS_CONFDEFS_EXTENSIONS_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/bsp.h>
#include <rtems/confdefs/newlib.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

#ifndef CONFIGURE_MAXIMUM_USER_EXTENSIONS
  #define CONFIGURE_MAXIMUM_USER_EXTENSIONS 0
#endif

#if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
  #include <rtems/extensiondata.h>
#endif

#ifdef _CONFIGURE_ENABLE_NEWLIB_REENTRANCY
  #include <rtems/libcsupport.h>
#endif

#if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS > 0
  #if (CONFIGURE_RECORD_PER_PROCESSOR_ITEMS & (CONFIGURE_RECORD_PER_PROCESSOR_ITEMS - 1)) != 0
    #error "CONFIGURE_RECORD_PER_PROCESSOR_ITEMS must be a power of two"
  #endif

  #if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS < 16
    #error "CONFIGURE_RECORD_PER_PROCESSOR_ITEMS must be at least 16"
  #endif

  #if defined(CONFIGURE_RECORD_EXTENSIONS_ENABLED) \
    || defined(CONFIGURE_RECORD_FATAL_DUMP_BASE64) \
    || defined(CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB)
    #define _CONFIGURE_RECORD_NEED_EXTENSION
  #endif

  #include <rtems/confdefs/percpu.h>
  #include <rtems/record.h>
#else
  #ifdef CONFIGURE_RECORD_EXTENSIONS_ENABLED
    #warning "CONFIGURE_RECORD_EXTENSIONS_ENABLED defined without CONFIGURE_RECORD_PER_PROCESSOR_ITEMS"
  #endif
  #ifdef CONFIGURE_RECORD_FATAL_DUMP_BASE64
    #warning "CONFIGURE_RECORD_FATAL_DUMP_BASE64 defined without CONFIGURE_RECORD_PER_PROCESSOR_ITEMS"
  #endif
  #ifdef CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB
    #warning "CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB defined without CONFIGURE_RECORD_PER_PROCESSOR_ITEMS"
  #endif
#endif

#ifdef CONFIGURE_STACK_CHECKER_ENABLED
  #include <rtems/stackchk.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_CONFIGURE_RECORD_NEED_EXTENSION) \
  || defined(_CONFIGURE_ENABLE_NEWLIB_REENTRANCY) \
  || defined(CONFIGURE_STACK_CHECKER_ENABLED) \
  || defined(CONFIGURE_INITIAL_EXTENSIONS) \
  || defined(BSP_INITIAL_EXTENSION)
  const User_extensions_Table _User_extensions_Initial_extensions[] = {
    #ifdef _CONFIGURE_RECORD_NEED_EXTENSION
      {
        #ifdef CONFIGURE_RECORD_EXTENSIONS_ENABLED
          _Record_Thread_create,
          _Record_Thread_start,
          _Record_Thread_restart,
          _Record_Thread_delete,
          _Record_Thread_switch,
          _Record_Thread_begin,
          _Record_Thread_exitted,
        #else
           NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        #endif
        #ifdef CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB
          _Record_Fatal_dump_base64_zlib,
        #elif defined(CONFIGURE_RECORD_FATAL_DUMP_BASE64)
          _Record_Fatal_dump_base64,
        #else
          NULL,
        #endif
        #ifdef CONFIGURE_RECORD_EXTENSIONS_ENABLED
          _Record_Thread_terminate
        #else
          NULL
        #endif
      },
    #endif
    #ifdef _CONFIGURE_ENABLE_NEWLIB_REENTRANCY
      RTEMS_NEWLIB_EXTENSION,
    #endif
    #ifdef CONFIGURE_STACK_CHECKER_ENABLED
      RTEMS_STACK_CHECKER_EXTENSION,
    #endif
    #ifdef CONFIGURE_INITIAL_EXTENSIONS
      CONFIGURE_INITIAL_EXTENSIONS,
    #endif
    #ifdef BSP_INITIAL_EXTENSION
      BSP_INITIAL_EXTENSION
    #endif
  };

  const size_t _User_extensions_Initial_count =
    RTEMS_ARRAY_SIZE( _User_extensions_Initial_extensions );

  User_extensions_Switch_control _User_extensions_Initial_switch_controls[
    RTEMS_ARRAY_SIZE( _User_extensions_Initial_extensions )
  ];

  RTEMS_SYSINIT_ITEM(
    _User_extensions_Handler_initialization,
    RTEMS_SYSINIT_INITIAL_EXTENSIONS,
    RTEMS_SYSINIT_ORDER_MIDDLE
  );
#endif

#if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
  EXTENSION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_USER_EXTENSIONS );
#endif

#if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS > 0
  typedef struct {
    Record_Control    Control;
    rtems_record_item Items[ CONFIGURE_RECORD_PER_PROCESSOR_ITEMS ];
  } Record_Configured_control;

  static Record_Configured_control _Record_Controls[ _CONFIGURE_MAXIMUM_PROCESSORS ];

  const Record_Configuration _Record_Configuration = {
    CONFIGURE_RECORD_PER_PROCESSOR_ITEMS,
    &_Record_Controls[ 0 ].Control
  };

  RTEMS_SYSINIT_ITEM(
    _Record_Initialize,
    RTEMS_SYSINIT_RECORD,
    RTEMS_SYSINIT_ORDER_MIDDLE
  );
#endif

#ifdef CONFIGURE_VERBOSE_SYSTEM_INITIALIZATION
  RTEMS_SYSINIT_ITEM(
    _Sysinit_Verbose,
    RTEMS_SYSINIT_RECORD,
    RTEMS_SYSINIT_ORDER_LAST
  );
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_EXTENSIONS_H */
