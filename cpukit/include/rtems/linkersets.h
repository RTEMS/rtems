/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPILinkerSets
 *
 * @brief This header file provides the linker sets API.
 */

/*
 * Copyright (C) 2015, 2020 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_LINKERSET_H
#define _RTEMS_LINKERSET_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup RTEMSImpl
 *
 * @brief Obfuscates a pointer to prevent compiler optimizations.
 *
 * @param ptr is the pointer to obfuscate.
 *
 * @return Returns the unsigned integer representation of the obfuscated
 *   pointer.
 */
static inline uintptr_t _Linker_set_Obfuscate( const void *ptr )
{
  uintptr_t addr;

  addr = (uintptr_t) ptr;
  RTEMS_OBFUSCATE_VARIABLE( addr );

  return addr;
}

/**
 * @defgroup RTEMSAPILinkerSets Linker Sets
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains the linker sets API.
 *
 * @{
 */

#define RTEMS_LINKER_SET_BEGIN( set ) \
  _Linker_set_##set##_begin

#define RTEMS_LINKER_SET_END( set ) \
  _Linker_set_##set##_end

/*
 * Modern GCC optimizes for speed which may insert padding between linker
 * sections that previous versions avoided. Alignment must now be explicit to
 * maintain the behavior of previous versions.
 */
#define RTEMS_LINKER_SET_ALIGN( type ) \
  RTEMS_ALIGNED( RTEMS_ALIGNOF( type ) )

#define RTEMS_LINKER_ROSET_DECLARE( set, type ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type	\
  const RTEMS_LINKER_SET_BEGIN( set )[];	\
  extern RTEMS_LINKER_SET_ALIGN( type ) type	\
  const RTEMS_LINKER_SET_END( set )[]

#define RTEMS_LINKER_ROSET( set, type ) \
  RTEMS_LINKER_SET_ALIGN( type ) type const RTEMS_LINKER_SET_BEGIN( set )[ 0 ] \
  RTEMS_SECTION( ".rtemsroset." #set ".begin" ) RTEMS_USED; \
  RTEMS_LINKER_SET_ALIGN( type ) type const RTEMS_LINKER_SET_END( set )[ 0 ] \
  RTEMS_SECTION( ".rtemsroset." #set ".end" ) RTEMS_USED

#define RTEMS_LINKER_ROSET_ITEM_ORDERED_DECLARE( set, type, item, order ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type const _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsroset." #set ".content.0." RTEMS_XSTRING( order ) )

#define RTEMS_LINKER_ROSET_ITEM_DECLARE( set, type, item ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type const _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsroset." #set ".content.1" )

#define RTEMS_LINKER_ROSET_ITEM_REFERENCE( set, type, item )	\
  static RTEMS_LINKER_SET_ALIGN( type ) type			\
  const * const _Set_reference_##set##_##item			\
  RTEMS_SECTION( ".rtemsroset.reference" ) RTEMS_USED =		\
  &_Linker_set_##set##_##item

#define RTEMS_LINKER_ROSET_ITEM_ORDERED( set, type, item, order ) \
  RTEMS_LINKER_SET_ALIGN( type ) type const _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsroset." #set ".content.0." RTEMS_XSTRING( order ) ) \
  RTEMS_USED

#define RTEMS_LINKER_ROSET_ITEM( set, type, item ) \
  RTEMS_LINKER_SET_ALIGN( type ) type const _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsroset." #set ".content.1" ) RTEMS_USED

#define RTEMS_LINKER_ROSET_CONTENT( set, decl ) \
  decl \
  RTEMS_SECTION( ".rtemsroset." #set ".content" )

#define RTEMS_LINKER_RWSET_DECLARE( set, type ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type RTEMS_LINKER_SET_BEGIN( set )[]; \
  extern RTEMS_LINKER_SET_ALIGN( type ) type RTEMS_LINKER_SET_END( set )[]

#define RTEMS_LINKER_RWSET( set, type ) \
  RTEMS_LINKER_SET_ALIGN( type ) type RTEMS_LINKER_SET_BEGIN( set )[ 0 ] \
  RTEMS_SECTION( ".rtemsrwset." #set ".begin" ) RTEMS_USED; \
  RTEMS_LINKER_SET_ALIGN( type ) type RTEMS_LINKER_SET_END( set )[ 0 ] \
  RTEMS_SECTION( ".rtemsrwset." #set ".end" ) RTEMS_USED

#define RTEMS_LINKER_RWSET_ITEM_ORDERED_DECLARE( set, type, item, order ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsrwset." #set ".content.0." RTEMS_XSTRING( order ) )

#define RTEMS_LINKER_RWSET_ITEM_DECLARE( set, type, item ) \
  extern RTEMS_LINKER_SET_ALIGN( type ) type _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsrwset." #set ".content.1" )

/*
 * The .rtemsroset is here not a typo.  We must ensure that the references are
 * not a victim of the garbage collection of the linker.  Thus, we place them
 * in a dedicated area of the RTEMS read-only linker set section.
 */
#define RTEMS_LINKER_RWSET_ITEM_REFERENCE( set, type, item ) \
  static RTEMS_LINKER_SET_ALIGN( type ) type			\
  * const _Set_reference_##set##_##item				\
  RTEMS_SECTION( ".rtemsroset.reference" ) RTEMS_USED = \
  &_Linker_set_##set##_##item

#define RTEMS_LINKER_RWSET_ITEM_ORDERED( set, type, item, order ) \
  RTEMS_LINKER_SET_ALIGN( type ) type _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsrwset." #set ".content.0." RTEMS_XSTRING( order ) ) \
  RTEMS_USED

#define RTEMS_LINKER_RWSET_ITEM( set, type, item ) \
  RTEMS_LINKER_SET_ALIGN( type ) type _Linker_set_##set##_##item \
  RTEMS_SECTION( ".rtemsrwset." #set ".content.1" ) RTEMS_USED

#define RTEMS_LINKER_RWSET_CONTENT( set, decl ) \
  decl \
  RTEMS_SECTION( ".rtemsrwset." #set ".content" )

#define RTEMS_LINKER_SET_SIZE( set ) \
  ( _Linker_set_Obfuscate( RTEMS_LINKER_SET_END( set ) ) \
    - _Linker_set_Obfuscate( RTEMS_LINKER_SET_BEGIN( set ) ) )

#define RTEMS_LINKER_SET_ITEM_COUNT( set ) \
  ( RTEMS_LINKER_SET_SIZE( set ) \
    / sizeof( RTEMS_LINKER_SET_BEGIN( set )[ 0 ] ) )

#define RTEMS_LINKER_SET_IS_EMPTY( set ) \
  ( RTEMS_LINKER_SET_SIZE( set ) == 0 )

#define RTEMS_LINKER_SET_FOREACH( set, item ) \
  for ( \
    item = (void *) _Linker_set_Obfuscate( RTEMS_LINKER_SET_BEGIN( set ) ) ; \
    item != RTEMS_LINKER_SET_END( set ) ; \
    ++item \
  )

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_LINKERSET_H */
