/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBasedefsValBasedefs
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "tc-basedefs-pendant.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsBasedefsValBasedefs spec:/rtems/basedefs/val/basedefs
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the basedefs macros of the Classic API.
 *
 * This test case performs the following actions:
 *
 * - Use the RTEMS_ALIAS() macro.
 *
 *   - Check that ori_func() and alias_func() are the same function.
 *
 * - Use the RTEMS_ALIGN_DOWN() macro in various examples.
 *
 *   - Check that RTEMS_ALIGN_DOWN() calculates the expected result and is
 *     side-effect free.
 *
 * - Use the RTEMS_ALIGN_UP() macro in various examples.
 *
 *   - Check that RTEMS_ALIGN_UP() calculates the expected result and is
 *     side-effect free.
 *
 * - Use the RTEMS_ALIGNED() macro.
 *
 *   - Check that RTEMS_ALIGNED() correctly aligns a variable on the stack and
 *     a structure member.
 *
 * - Use the RTEMS_ALIGNOF() macro.
 *
 *   - Check that the RTEMS_ALIGNOF() macro results in the alignment of the
 *     type.
 *
 *   - If the RTEMS_ALIGNOF() macro would evaluate its argument, it could not
 *     figure out how much menory to reserve for it.
 *
 *   - If the RTEMS_ALIGNOF() macro would evaluate the size expression, the
 *     division by zero would cause an error.
 *
 *   - Ensure the constant value of the RTEMS_ALIGNOF() macro is of type
 *     size_t.
 *
 * - Use a function declared with the RTEMS_ALLOC_ALIGN() macro.
 *
 *   - It cannot be checked that the RTEMS_ALLOC_ALIGN() macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and that it can
 *     be used on such a memory function and that the argument counting starts
 *     at 1.
 *
 * - Use a function declared with the RTEMS_ALLOC_SIZE() macro.
 *
 *   - It cannot be checked that the RTEMS_ALLOC_SIZE() macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and that it can
 *     be used on such a memory function and that the argument counting starts
 *     at 1.
 *
 * - Use a function declared with the RTEMS_ALLOC_SIZE_2() macro.
 *
 *   - It cannot be checked that the RTEMS_ALLOC_SIZE_2() macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and that it can
 *     be used on such a memory function and that the argument counting starts
 *     at 1.
 *
 * - Use the RTEMS_ARRAY_SIZE() macro.
 *
 *   - Check that the calculated size of the arrays fit their definition.
 *
 * - Use the RTEMS_COMPILER_DEPRECATED_ATTRIBUTE macro.
 *
 *   - It cannot automatically be checked that the
 *     RTEMS_COMPILER_DEPRECATED_ATTRIBUTE macro has the desired effect. The
 *     gcc compiler should issue a warning about the use of a deprecated
 *     variable on the above line where the ``compiler_deprecated_attribute``
 *     is used.
 *
 * - Use the RTEMS_COMPILER_MEMORY_BARRIER() macro.
 *
 *   - It cannot be checked that the RTEMS_COMPILER_MEMORY_BARRIER() macro has
 *     the desired effect. It is only checked that such a macro exists.
 *
 * - Use of the RTEMS_COMPILER_NO_RETURN_ATTRIBUTE macro at the beginning of
 *   this file.
 *
 *   - It cannot be checked that the RTEMS_COMPILER_NO_RETURN_ATTRIBUTE macro
 *     has the desired effect. It is only checked that such a macro exists.
 *
 * - Use the RTEMS_COMPILER_PACKED_ATTRIBUTE macro.
 *
 *   - Check that RTEMS_COMPILER_PACKED_ATTRIBUTE correctly aligns a structure
 *     member.
 *
 * - Use the RTEMS_COMPILER_PURE_ATTRIBUTE macro at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_COMPILER_PURE_ATTRIBUTE macro has
 *     the desired effect. It is checked that such a macro exists.
 *
 * - Use the RTEMS_COMPILER_UNUSED_ATTRIBUTE macro.
 *
 *   - It cannot automatically be checked that the
 *     RTEMS_COMPILER_UNUSED_ATTRIBUTE macro has the desired effect. It is
 *     checked that such a macro exists and one can manually check that no
 *     compiler warnings are produced for the compiler_unused_attribute_var.
 *
 * - Invoke the  RTEMS_CONCAT() macro on examples.
 *
 *   - Check that the two arguments of RTEMS_CONCAT() are concatenated to a new
 *     token.
 *
 *   - Check that the result of the RTEMS_CONCAT() expansion is subject to a
 *     further pre-processor substitution.
 *
 * - Use the RTEMS_CONST macro at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_CONST macro has the desired effect.
 *     It is checked that such a macro exists.
 *
 * - Use the RTEMS_CONTAINER_OF() macro.
 *
 *   - Check that the RTEMS_CONTAINER_OF() macro evaluates to a pointer to
 *     container_of_struct_var.
 *
 * - Use the RTEMS_DECLARE_GLOBAL_SYMBOL() macro in the file
 *   tc-basedefs-pendant.h.
 *
 *   - Check that the RTEMS_DECLARE_GLOBAL_SYMBOL() macro declares a global
 *     symbol which can be accessed by function basedefs_get_global_symbol()
 *     which is defined in a file different from the file in which the gobal
 *     symbol is defined.
 *
 * - Use the RTEMS_DECONST() macro.
 *
 *   - Check that the RTEMS_DECONST() macro returns a pointer which allows to
 *     write into an otherwise const value.
 *
 * - Use the RTEMS_DEFINE_GLOBAL_SYMBOL() macro at the beginning of this file.
 *
 *   - Check that the RTEMS_DEFINE_GLOBAL_SYMBOL() macro defines a global
 *     symbol with the correct value.
 *
 * - Use a function declared with the RTEMS_DEPRECATED macro.
 *
 *   - It cannot automatically be checked that the RTEMS_DEPRECATED macro has
 *     the desired effect. The gcc compiler should issue a warning about the
 *     use of a deprecated function on the above line where the
 *     ``deprecated_func`` is used.
 *
 * - Use the RTEMS_DEQUALIFY_DEPTHX() macro.
 *
 *   - Check that the RTEMS_DEQUALIFY_DEPTHX() macro returns a pointer which
 *     allows to write into an otherwise const (volatile) value.
 *
 * - Use the RTEMS_DEQUALIFY() macro.
 *
 *   - Check that the RTEMS_DEQUALIFY() macro returns a pointer which allows to
 *     write into an otherwise const volatile value.
 *
 * - Use the RTEMS_DEVOLATILE() macro.
 *
 *   - Check that the RTEMS_DEVOLATILE() macro returns a pointer which allows
 *     to write into an otherwise volatile value.
 *
 * - Invoke the  RTEMS_EXPAND() macro on an example.
 *
 *   - Check that the argument of RTEMS_EXPAND() is expanded and returned.
 *
 * - Invoke the  FALSE macro on an example.
 *
 *   - Check that of FALSE is substituted by 0.
 *
 * - Invoke the  RTEMS_HAVE_MEMBER_SAME_TYPE() macro on examples.
 *
 *   - Check that of RTEMS_HAVE_MEMBER_SAME_TYPE() returns 0 and 1 depending on
 *     whether these types are compatible.
 *
 * - Use the RTEMS_INLINE_ROUTINE in the definition of function
 *   inline_routine_func() at the beginning of this file. Obtain the text the
 *   macro RTEMS_INLINE_ROUTINE produces.
 *
 *   - Check that the RTEMS_INLINE_ROUTINE exists and that it produces the
 *     desired text.
 *
 * - Use a function declared with the RTEMS_MALLOCLIKE macro.
 *
 *   - It cannot be checked that the RTEMS_MALLOCLIKE macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and that it can
 *     be used on such a memory function and that it produces the correct code.
 *
 * - Use a function declared with the RTEMS_NO_INLINE macro.
 *
 *   - It cannot be checked that the RTEMS_NO_INLINE macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and that it can
 *     be used on such a function and that it produces the correct code.
 *
 * - Use of the RTEMS_NO_RETURN macro at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_NO_RETURN macro has the desired
 *     effect. It is only checked that such a macro exists.
 *
 * - Use the RTEMS_NOINIT macro on ``noinit_variable`` at the beginning of this
 *   file.
 *
 *   - It cannot be checked that the RTEMS_NOINIT macro has the desired effect.
 *     Yet, the check confirms that such a macro exists and can be used.
 *
 * - Use the RTEMS_OBFUSCATE_VARIABLE() macro.
 *
 *   - It cannot be checked that the RTEMS_OBFUSCATE_VARIABLE() macro has the
 *     desired effect. Yet, the check confirms that such a macro exists and can
 *     be used.
 *
 * - Use the RTEMS_PACKED macro.
 *
 *   - Check that RTEMS_PACKED correctly aligns a structure member.
 *
 *   - Check that RTEMS_PACKED correctly aligns all structure members.
 *
 *   - Check that RTEMS_PACKED correctly enforces a minimal enum type.
 *
 * - Use the RTEMS_PREDICT_FALSE() macro.
 *
 *   - It cannot be checked that the RTEMS_PREDICT_FALSE() macro has the
 *     desired effect. Yet, the check confirms that such a macro exists and can
 *     be used.
 *
 * - Use the RTEMS_PREDICT_TRUE() macro.
 *
 *   - It cannot be checked that the RTEMS_PREDICT_TRUE() macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and can be
 *     used.
 *
 * - Use a function declared with the RTEMS_PRINTFLIKE() macro.
 *
 *   - It cannot automatically be checked that the RTEMS_PRINTFLIKE() macro has
 *     the desired effect. Yet, the check confirms that such a macro exists and
 *     that it can be used on such a printf-like function and that the argument
 *     numbers are correct.
 *
 * - Use the RTEMS_PURE macro at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_PURE macro has the desired effect.
 *     It is checked that such a macro exists.
 *
 * - Get the code the RTEMS_RETURN_ADDRESS() macro produces as string.
 *
 *   - The check confirms that a RTEMS_RETURN_ADDRESS() macro exists and that
 *     it produces the correct code.
 *
 * - Use the RTEMS_SECTION() macro on ``section_variable`` and ``section_func``
 *   at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_SECTION() macro has the desired
 *     effect. Yet, the check confirms that such a macro exists and can be
 *     used.
 *
 * - Evaluate if RTEMS_STATIC_ANALYSIS is defined.
 *
 *   - Check that RTEMS_STATIC_ANALYSIS was not defined.
 *
 * - Use the RTEMS_STATIC_ASSERT() macro.
 *
 *   - It cannot be automatically check that the RTEMS_STATIC_ASSERT() macro
 *     has the desired effect. Yet, it can be checked that the macro exists and
 *     accepts the specified arguments.
 *
 * - Use the RTEMS_STRING() macro.
 *
 *   - Check that the RTEMS_STRING() macro converts its arguments into a single
 *     string without applying pre-processor substitutions on its arguments.
 *
 * - Use the RTEMS_SYMBOL_NAME() macro with an example object.
 *
 *   - Check that the RTEMS_SYMBOL_NAME() macro expands to the expected symbol
 *     name.
 *
 * - Invoke the  TRUE macro on an example.
 *
 *   - Check that of TRUE is substituted by 0.
 *
 * - Use of the  RTEMS_TYPEOF_REFX() macro on several examples. This use is
 *   already the test as the statements will not compile without error if the
 *   macro did not evaluate to the correct type.
 *
 *   - The checks here are proforma. The macro is tested by the fact that the
 *     action will not compile if the macro returns a wrong result.
 *
 * - Use the RTEMS_UNUSED macro. See also unused_func() at the beginning of
 *   this file.
 *
 *   - It cannot automatically be checked that the RTEMS_UNUSED macro has the
 *     desired effect. It is checked that such a macro exists and one can
 *     manually check that no compiler warnings are produced for the
 *     unused_func().
 *
 *   - It cannot automatically be checked that the RTEMS_UNUSED macro has the
 *     desired effect. It is checked that such a macro exists and one can
 *     manually check that no compiler warnings are produced for the
 *     unused_lable.
 *
 *   - It cannot automatically be checked that the RTEMS_UNUSED macro has the
 *     desired effect. It is checked that such a macro exists and one can
 *     manually check that no compiler warnings are produced for the
 *     unused_struct.
 *
 *   - It cannot automatically be checked that the RTEMS_UNUSED macro has the
 *     desired effect. It is checked that such a macro exists and one can
 *     manually check that no compiler warnings are produced for the unused
 *     items unused_var and the unused argument and variable in unused_func().
 *
 * - Use of the RTEMS_UNREACHABLE() macro in function definition of
 *   unreachable_func() at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_UNREACHABLE() macro has the desired
 *     effect. It is checked that such a macro exists and the compiler warning
 *     about the missing return statement is suppressed.
 *
 * - Use of the RTEMS_USED macro in function definition of used_func() at the
 *   beginning of this file and with used_var above.
 *
 *   - It cannot be checked that the RTEMS_USED macro has the desired effect.
 *     It is checked that such a macro exists.
 *
 * - Use of the RTEMS_WARN_UNUSED_RESULT macro in function definition of
 *   warn_unused_func() at the beginning of this file.
 *
 *   - It cannot be checked that the RTEMS_WARN_UNUSED_RESULT macro has the
 *     desired effect. The GNU C compiler should issue a warning about the
 *     disregarded result returned by the call to the ``warn_unused_func()``
 *     function.
 *
 * - Use of ``basedefs_weak_alias_0/1_func()`` which are defined with the
 *   RTEMS_WEAK_ALIAS() macro at the beginning of this file.
 *
 *   - There exists no strong alias for basedefs_weak_alias_0_func(). Check
 *     that ori_func() and basedefs_weak_alias_0_func() are the same function.
 *
 *   - File ``tc_basedefs_pndant.c`` defines a strong function for
 *     basedefs_weak_alias_1_func(). Check that ori_func() and
 *     basedefs_weak_alias_1_func() are not the same function.
 *
 * - Use of ``basedefs_weak_0/1_var`` and ``basedefs_weak_0/1_func()`` which
 *   are defined with the RTEMS_WEAK macro at the beginning of this file.
 *
 *   - For ``basedefs_weak_0_var`` and ``basedefs_weak_0_func()`` there exists
 *     no other symbols with the same name. Hence, the checks test that the
 *     weak symbols are used.
 *
 *   - ``basedefs_weak_1_var`` and ``basedefs_weak_1_func()`` are overwritten
 *     by strong symbols defined in file ``tc_basedefs_pendant.c``. Hence, the
 *     checks test that the strong variants are used.
 *
 * - Invoke the RTEMS_XCONCAT() macro on examples.
 *
 *   - Check that the two arguments of RTEMS_XCONCAT() are concatenated without
 *     inserting new characters.
 *
 *   - Check that the two arguments of RTEMS_XCONCAT() are substituted before
 *     they are concatenated.
 *
 *   - Check that the two arguments of RTEMS_XCONCAT() are can be the macro
 *     itself.
 *
 *   - Check that the result of the RTEMS_XCONCAT() expansion is subject to a
 *     further pre-processor substitution.
 *
 * - Use the RTEMS_XSTRING() macro.
 *
 *   - Check that the RTEMS_XSTRING() macro applies pre-processor substitutions
 *     on its arguments and converts its arguments into a single string.
 *
 * - Use of the RTEMS_ZERO_LENGTH_ARRAY macro in a declaration of a structure.
 *
 *   - Checked that the RTEMS_ZERO_LENGTH_ARRAY macro produces a structure
 *     similar to a structure with one element.
 *
 * - Use the RTEMS_DEFINE_GLOBAL_SYMBOL() macro at the beginning of this file
 *   and assign the address of the symbol to an object.
 *
 *   - Check that the RTEMS_DEFINE_GLOBAL_SYMBOL() macro defines a global
 *     symbol with the correct value.
 *
 * @{
 */

#define WHITE_SPACE_STRING_MAX_LENGTH 80
#define abccat concat
#define abc ABC
#define CON con
#define CAT cat
#define defcat concat
#define GLOBAL_SYMBOL_VALULE( _hex ) 0x ## _hex
#define EXPAND expand
#define PREDICT_FALSE 1 -
#define SECTION_NAME ".rtemsroset.test"
#define STATIC_ASSERT_COND 0 +
#define STRING_PREFIX str
#define SYMBOL_NAME global_object
#define _TO_STR2( _text ) #_text
#define _TO_STR( _text ) _TO_STR2( _text )

int global_object;

extern int address_of_global_object;

__asm__(
  "\n\t.set " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( address_of_global_object ) )
  ", " RTEMS_XSTRING( RTEMS_SYMBOL_NAME( SYMBOL_NAME ) ) "\n"
);

/*
 * For some reasons - which I fail to fully understand - _TO_STR()
 * seems to remove spaces around `()` at times and at other times
 * not. For example, I get
 *
 *   *  "__attribute__(( __malloc__ ))" or
 *   *  "__attribute__((__malloc__))".
 *
 * To avoid trouble, the function below returns a version of a
 * string without any spaces. Albeit, the implementation is rather
 * brute and raw. It returns a pointer to a static buffer of fixed
 * size. That will do for tests but not serve as generic function.
 */
static const char *remove_white_space( const char *str )
{
  char c;
  int i = 0;
  static char buffer[WHITE_SPACE_STRING_MAX_LENGTH] = {};

  /* Sanity check */
  if( strlen( str ) >= sizeof( buffer ) ) {
    T_assert_true( false,
      "Buffer too small; increase WHITE_SPACE_STRING_MAX_LENGTH" );
  }

  /* Copy string but skip white spaces */
  do {
    c = *( str++ );
    if ( ' ' != c && '\t' !=c ) {
      buffer[i++] = c;
    }
  } while ( '\0' != c );

  return buffer;
}

static int alias_func( int i ) RTEMS_ALIAS( ori_func );

typedef struct {
  uint8_t c;
  uint8_t aligned_member RTEMS_ALIGNED( 8 );
} aligned_member_struct;

static int concat( void )
{
  return 91;
}

RTEMS_CONST static int const_func( int arg )
{
  return 4 * arg;
}

RTEMS_COMPILER_NO_RETURN_ATTRIBUTE
  static void compiler_no_return_attribute_func( int i );
static void compiler_no_return_attribute_func( int i )
{
  while ( true ) {
    /* Loop forever */
  }
}

RTEMS_COMPILER_PURE_ATTRIBUTE static int compiler_pure_attribute_func( void )
{
  return 21;
}

static int global_symbol_base;

RTEMS_DEFINE_GLOBAL_SYMBOL(
  GLOBAL_SYMBOL,
  RTEMS_SYMBOL_NAME( global_symbol_base ) + GLOBAL_SYMBOL_VALULE( abc )
);

RTEMS_DECLARE_GLOBAL_SYMBOL( global_symbol_2 );

RTEMS_DEFINE_GLOBAL_SYMBOL( global_symbol_2, 0x123 );

static const char * const volatile global_symbol_2_object = global_symbol_2;

static int deprecated_func( int i ) RTEMS_DEPRECATED;
static int deprecated_func( int i )
{
  return 3 * i;
}

static int expand( void )
{
  return 82;
}

RTEMS_INLINE_ROUTINE int inline_routine_func( int arg )
{
  return 1 << arg;
}

RTEMS_NO_INLINE static int no_inline_func( void )
{
  asm ("");
  return 75;
}

RTEMS_NO_RETURN static void no_return_func( int i )
{
  while ( true ) {
    /* Loop forever */
  }
}

RTEMS_NOINIT static uint32_t noinit_variable;

static int ori_func( int x )
{
   return 2 * x;
}

RTEMS_PRINTFLIKE(2, 3) static int printflike_func(
  const char *prefix,
  const char *fmt,
  ...
)
{
  int result;
  va_list va_list;

  T_printf( "%s: ", prefix );
  va_start( va_list, fmt );
  result = T_vprintf( fmt, va_list );
  va_end( va_list );

  return result;
}

RTEMS_PURE static int pure_func( void )
{
  return 21;
}

RTEMS_SECTION( ".rtemsrwset.test" ) static int section_var = 28;
RTEMS_SECTION( SECTION_NAME ) static int section_func( int arg )
{
  return arg % 100;
}

static int unreachable_func( int arg )
{
  if ( 1 == arg % 100 ) {
    return arg;
  } else {
    T_assert_true( false,
      "Oops! Function caled with bad argument." );
    RTEMS_UNREACHABLE();
  }
}

RTEMS_USED static int used_var = 4711;
RTEMS_USED static int used_func( void )
{
  return 35;
}

static int warn_unused_func( int arg ) RTEMS_WARN_UNUSED_RESULT;
static int warn_unused_func( int arg )
{
  return arg / 3;
}

int basedefs_weak_alias_0_func( int i ) RTEMS_WEAK_ALIAS( ori_func );
int basedefs_weak_alias_1_func( int i ) RTEMS_WEAK_ALIAS( ori_func );

RTEMS_WEAK const volatile int basedefs_weak_0_var = 60;
RTEMS_WEAK const volatile int basedefs_weak_1_var = 61;
RTEMS_WEAK int basedefs_weak_0_func( void )
{
  return 63;
}

RTEMS_WEAK int basedefs_weak_1_func( void )
{
  return 64;
}

/**
 * @brief Use the RTEMS_ALIAS() macro.
 */
static void RtemsBasedefsValBasedefs_Action_0( void )
{
  int alias_result;

  alias_result = ori_func( 3 ) + alias_func( 5 );

  /*
   * Check that ori_func() and alias_func() are the same function.
   */
  T_step_eq_int( 0, alias_result, 16 );
}

/**
 * @brief Use the RTEMS_ALIGN_DOWN() macro in various examples.
 */
static void RtemsBasedefsValBasedefs_Action_1( void )
{
  int align_down0_result;
  int align_down1_result;
  int align_down2_result;
  int align_down3_result;
  int align_down4_result;
  int align_down5_result;
  int align_down6_result;
  int align_down7_result;
  int align_down8_result;
  int align_down9_result;

  align_down0_result = RTEMS_ALIGN_DOWN( 0, 1 );
  align_down1_result = RTEMS_ALIGN_DOWN( 0, 4 );
  align_down2_result = RTEMS_ALIGN_DOWN( 1, 2 );
  align_down3_result = RTEMS_ALIGN_DOWN( 2, 2 );
  align_down4_result = RTEMS_ALIGN_DOWN( 3, 2 );
  align_down5_result = RTEMS_ALIGN_DOWN( 4, 2 );
  align_down6_result = RTEMS_ALIGN_DOWN( 5, 2 );
  align_down7_result = RTEMS_ALIGN_DOWN( 255, 16 );
  align_down8_result = RTEMS_ALIGN_DOWN( 256, 16 );
  align_down9_result = RTEMS_ALIGN_DOWN( 257, 16 );

  /*
   * Check that RTEMS_ALIGN_DOWN() calculates the expected result and is
   * side-effect free.
   */
  T_step_eq_int( 1, align_down0_result, 0 );
  T_step_eq_int( 2, align_down1_result, 0 );
  T_step_eq_int( 3, align_down2_result, 0 );
  T_step_eq_int( 4, align_down3_result, 2 );
  T_step_eq_int( 5, align_down4_result, 2 );
  T_step_eq_int( 6, align_down5_result, 4 );
  T_step_eq_int( 7, align_down6_result, 4 );
  T_step_eq_int( 8, align_down7_result, 240 );
  T_step_eq_int( 9, align_down8_result, 256 );
  T_step_eq_int( 10, align_down9_result, 256 );
}

/**
 * @brief Use the RTEMS_ALIGN_UP() macro in various examples.
 */
static void RtemsBasedefsValBasedefs_Action_2( void )
{
  int align_up0_result;
  int align_up1_result;
  int align_up2_result;
  int align_up3_result;
  int align_up4_result;
  int align_up5_result;
  int align_up6_result;
  int align_up7_result;
  int align_up8_result;
  int align_up9_result;

  align_up0_result = RTEMS_ALIGN_UP( 0, 1 );
  align_up1_result = RTEMS_ALIGN_UP( 0, 4 );
  align_up2_result = RTEMS_ALIGN_UP( 1, 2 );
  align_up3_result = RTEMS_ALIGN_UP( 2, 2 );
  align_up4_result = RTEMS_ALIGN_UP( 3, 2 );
  align_up5_result = RTEMS_ALIGN_UP( 4, 2 );
  align_up6_result = RTEMS_ALIGN_UP( 5, 2 );
  align_up7_result = RTEMS_ALIGN_UP( 255, 16 );
  align_up8_result = RTEMS_ALIGN_UP( 256, 16 );
  align_up9_result = RTEMS_ALIGN_UP( 257, 16 );

  /*
   * Check that RTEMS_ALIGN_UP() calculates the expected result and is
   * side-effect free.
   */
  T_step_eq_int( 11, align_up0_result, 0 );
  T_step_eq_int( 12, align_up1_result, 0 );
  T_step_eq_int( 13, align_up2_result, 2 );
  T_step_eq_int( 14, align_up3_result, 2 );
  T_step_eq_int( 15, align_up4_result, 4 );
  T_step_eq_int( 16, align_up5_result, 4 );
  T_step_eq_int( 17, align_up6_result, 6 );
  T_step_eq_int( 18, align_up7_result, 256 );
  T_step_eq_int( 19, align_up8_result, 256 );
  T_step_eq_int( 20, align_up9_result, 272 );
}

/**
 * @brief Use the RTEMS_ALIGNED() macro.
 */
static void RtemsBasedefsValBasedefs_Action_3( void )
{
  char unaligned_var = 'c';
  char aligned_var RTEMS_ALIGNED( 8 ) = 'd';

  (void) unaligned_var;

  /*
   * Check that RTEMS_ALIGNED() correctly aligns a variable on the stack and a
   * structure member.
   */
  T_step_eq_int( 21, ( ( uintptr_t ) &aligned_var ) % 8, 0 );
  T_step_eq_int( 22,
    offsetof( aligned_member_struct, aligned_member ) % 8, 0 );
}

/**
 * @brief Use the RTEMS_ALIGNOF() macro.
 */
static void RtemsBasedefsValBasedefs_Action_4( void )
{
  size_t alignof_char = RTEMS_ALIGNOF( char );
  size_t alignof_long = RTEMS_ALIGNOF( long );
  size_t alignof_long_array = RTEMS_ALIGNOF( long[3] );
  size_t alignof_not_eval_array = RTEMS_ALIGNOF( long[7 / 0] );

  /*
   * Check that the RTEMS_ALIGNOF() macro results in the alignment of the type.
   */
  T_step_eq_sz( 23, alignof_char, 1 );
  T_step_eq_sz( 24, alignof_long, alignof_long_array );

  /*
   * If the RTEMS_ALIGNOF() macro would evaluate its argument, it could not
   * figure out how much menory to reserve for it.
   */
  T_step_eq_sz( 25, alignof_long, alignof_not_eval_array );

  /*
   * If the RTEMS_ALIGNOF() macro would evaluate the size expression, the
   * division by zero would cause an error.
   */
  T_step_eq_sz( 26, alignof_long, alignof_not_eval_array );

  /*
   * Ensure the constant value of the RTEMS_ALIGNOF() macro is of type size_t.
   */
  T_step_true( 27,
    __builtin_types_compatible_p( __typeof__( RTEMS_ALIGNOF( char ) ),
    size_t ) );
}

/**
 * @brief Use a function declared with the RTEMS_ALLOC_ALIGN() macro.
 */
static void RtemsBasedefsValBasedefs_Action_5( void )
{
  void *free_ptr;
  void *alloc_align_ptr;
  alloc_align_ptr = basedefs_alloc_align_func( 1024, &free_ptr, 64 );
  basedefs_free( free_ptr );

  /*
   * It cannot be checked that the RTEMS_ALLOC_ALIGN() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and that it can
   * be used on such a memory function and that the argument counting starts at
   * 1.
   */
  T_step_not_null( 28, alloc_align_ptr );
  T_step_eq_int( 29, ( ( uintptr_t ) alloc_align_ptr ) % 64, 0 );
  T_step_ge_uptr( 30, ( ( uintptr_t ) alloc_align_ptr ),
    ( ( uintptr_t ) free_ptr ) );
  T_step_lt_uptr( 31, ( ( uintptr_t ) alloc_align_ptr ),
    ( ( uintptr_t ) free_ptr ) + 64 );
}

/**
 * @brief Use a function declared with the RTEMS_ALLOC_SIZE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_6( void )
{
  void *alloc_size_ptr;
  alloc_size_ptr = basedefs_alloc_size_func( 1024 );
  basedefs_free( alloc_size_ptr );

  /*
   * It cannot be checked that the RTEMS_ALLOC_SIZE() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and that it can
   * be used on such a memory function and that the argument counting starts at
   * 1.
   */
  T_step_not_null( 32, alloc_size_ptr );
}

/**
 * @brief Use a function declared with the RTEMS_ALLOC_SIZE_2() macro.
 */
static void RtemsBasedefsValBasedefs_Action_7( void )
{
  void *alloc_size_2_ptr;
  alloc_size_2_ptr = basedefs_alloc_size_2_func( 8, 128 );
  basedefs_free( alloc_size_2_ptr );

  /*
   * It cannot be checked that the RTEMS_ALLOC_SIZE_2() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and that it can
   * be used on such a memory function and that the argument counting starts at
   * 1.
   */
  T_step_not_null( 33, alloc_size_2_ptr );
}

/**
 * @brief Use the RTEMS_ARRAY_SIZE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_8( void )
{
  int array[] = { 10, 20, 30, 40, 50 };
  unsigned char array2[12];
  int array_size = RTEMS_ARRAY_SIZE(array);
  int array2_size = RTEMS_ARRAY_SIZE(array2);

  /*
   * Check that the calculated size of the arrays fit their definition.
   */
  T_step_eq_sz( 34, array_size, 5 );
  T_step_eq_sz( 35, array2_size, 12 );
}

/**
 * @brief Use the RTEMS_COMPILER_DEPRECATED_ATTRIBUTE macro.
 */
static void RtemsBasedefsValBasedefs_Action_9( void )
{
  int compiler_deprecated_attribute RTEMS_COMPILER_DEPRECATED_ATTRIBUTE = 42;

  /*
   * It cannot automatically be checked that the
   * RTEMS_COMPILER_DEPRECATED_ATTRIBUTE macro has the desired effect. The gcc
   * compiler should issue a warning about the use of a deprecated variable on
   * the above line where the ``compiler_deprecated_attribute`` is used.
   */
  /*
   * Derivation from Coding Style:
   * The following code suppresses a compiler warning (instead of fixing
   * it).
   * Rational: The variable compiler_deprecated_attribute is not really
   * deprecated but its purpose is to test the RTEMS_DEPRECATED macro.
   * The RTEMS_DEPRECATED macro must result in a compiler warning here.
   */
  _Pragma( "GCC diagnostic push" )
  _Pragma( "GCC diagnostic ignored \"-Wdeprecated-declarations\"" )
  T_step_eq_int( 36, compiler_deprecated_attribute, 42 );
  _Pragma( "GCC diagnostic pop" )
}

/**
 * @brief Use the RTEMS_COMPILER_MEMORY_BARRIER() macro.
 */
static void RtemsBasedefsValBasedefs_Action_10( void )
{
  RTEMS_COMPILER_MEMORY_BARRIER();

  /*
   * It cannot be checked that the RTEMS_COMPILER_MEMORY_BARRIER() macro has
   * the desired effect. It is only checked that such a macro exists.
   */

}

/**
 * @brief Use of the RTEMS_COMPILER_NO_RETURN_ATTRIBUTE macro at the beginning
 *   of this file.
 */
static void RtemsBasedefsValBasedefs_Action_11( void )
{
  (void) compiler_no_return_attribute_func;

  /*
   * It cannot be checked that the RTEMS_COMPILER_NO_RETURN_ATTRIBUTE macro has
   * the desired effect. It is only checked that such a macro exists.
   */

}

/**
 * @brief Use the RTEMS_COMPILER_PACKED_ATTRIBUTE macro.
 */
static void RtemsBasedefsValBasedefs_Action_12( void )
{
  typedef struct {
    uint8_t c;
    RTEMS_COMPILER_PACKED_ATTRIBUTE uint32_t i;
  } compiler_packed_attribute_struct;
  int compiler_packed_attribute_offset =
    offsetof( compiler_packed_attribute_struct, i );

  /*
   * Check that RTEMS_COMPILER_PACKED_ATTRIBUTE correctly aligns a structure
   * member.
   */
  T_step_eq_int( 37, compiler_packed_attribute_offset, 1 );
}

/**
 * @brief Use the RTEMS_COMPILER_PURE_ATTRIBUTE macro at the beginning of this
 *   file.
 */
static void RtemsBasedefsValBasedefs_Action_13( void )
{
  int compiler_pure_attribute_result;
  int compiler_pure_attribute_result_2;
  compiler_pure_attribute_result = compiler_pure_attribute_func();
  compiler_pure_attribute_result_2 =
    compiler_pure_attribute_func();

  /*
   * It cannot be checked that the RTEMS_COMPILER_PURE_ATTRIBUTE macro has the
   * desired effect. It is checked that such a macro exists.
   */
  T_step_eq_int( 38, compiler_pure_attribute_result, 21 );
  T_step_eq_int( 39, compiler_pure_attribute_result_2, 21 );
}

/**
 * @brief Use the RTEMS_COMPILER_UNUSED_ATTRIBUTE macro.
 */
static void RtemsBasedefsValBasedefs_Action_14( void )
{
  int compiler_unused_attribute_var RTEMS_COMPILER_UNUSED_ATTRIBUTE;

  /*
   * It cannot automatically be checked that the
   * RTEMS_COMPILER_UNUSED_ATTRIBUTE macro has the desired effect. It is
   * checked that such a macro exists and one can manually check that no
   * compiler warnings are produced for the compiler_unused_attribute_var.
   */

}

/**
 * @brief Invoke the  RTEMS_CONCAT() macro on examples.
 */
static void RtemsBasedefsValBasedefs_Action_15( void )
{
  int concat0_result;
  int concat1_result;
  concat0_result = RTEMS_CONCAT( con, cat )();
  concat1_result = RTEMS_CONCAT( abc, cat )();

  /*
   * Check that the two arguments of RTEMS_CONCAT() are concatenated to a new
   * token.
   */
  T_step_eq_int( 40, concat0_result, 91 );

  /*
   * Check that the result of the RTEMS_CONCAT() expansion is subject to a
   * further pre-processor substitution.
   */
  T_step_eq_int( 41, concat1_result, 91 );
}

/**
 * @brief Use the RTEMS_CONST macro at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_16( void )
{
  int const_result;
  int const_result_2;
  const_result = const_func( 7 );
  const_result_2 = const_func( 7 );

  /*
   * It cannot be checked that the RTEMS_CONST macro has the desired effect. It
   * is checked that such a macro exists.
   */
  T_step_eq_int( 42, const_result, 28 );
  T_step_eq_int( 43, const_result_2, 28 );
}

/**
 * @brief Use the RTEMS_CONTAINER_OF() macro.
 */
static void RtemsBasedefsValBasedefs_Action_17( void )
{
  typedef struct {
    int a;
    int b;
  } container_of_struct;

  container_of_struct container_of_struct_var;
  int *container_of_struct_b_adr = &container_of_struct_var.b;
  container_of_struct *container_of_struct_adr;
  container_of_struct_adr =
    RTEMS_CONTAINER_OF( container_of_struct_b_adr, container_of_struct, b );

  /*
   * Check that the RTEMS_CONTAINER_OF() macro evaluates to a pointer to
   * container_of_struct_var.
   */
  T_step_eq_ptr( 44,
    container_of_struct_adr, &container_of_struct_var );
}

/**
 * @brief Use the RTEMS_DECLARE_GLOBAL_SYMBOL() macro in the file
 *   tc-basedefs-pendant.h.
 */
static void RtemsBasedefsValBasedefs_Action_18( void )
{
  /* No action */

  /*
   * Check that the RTEMS_DECLARE_GLOBAL_SYMBOL() macro declares a global
   * symbol which can be accessed by function basedefs_get_global_symbol()
   * which is defined in a file different from the file in which the gobal
   * symbol is defined.
   */
  T_step_eq_uptr(
    45,
    basedefs_get_global_symbol() - (uintptr_t) &global_symbol_base,
    0xabc
  );
}

/**
 * @brief Use the RTEMS_DECONST() macro.
 */
static void RtemsBasedefsValBasedefs_Action_19( void )
{
  const int deconst_array[] = { 52, 55 };
  int *deconst_pointer;
  deconst_pointer = RTEMS_DECONST( int *, deconst_array );

  /*
   * Check that the RTEMS_DECONST() macro returns a pointer which allows to
   * write into an otherwise const value.
   */
  T_step_eq_int( 46, deconst_pointer[0], 52 );
  T_step_eq_int( 47, deconst_pointer[1], 55 );
  deconst_pointer[1] = 13;
  T_step_eq_int( 48, deconst_pointer[1], 13 );
}

/**
 * @brief Use the RTEMS_DEFINE_GLOBAL_SYMBOL() macro at the beginning of this
 *   file.
 */
static void RtemsBasedefsValBasedefs_Action_20( void )
{
  /* No action */

  /*
   * Check that the RTEMS_DEFINE_GLOBAL_SYMBOL() macro defines a global symbol
   * with the correct value.
   */
  T_step_eq_uptr(
    49,
    (uintptr_t) global_symbol - (uintptr_t) &global_symbol_base,
    0xabc
  );
}

/**
 * @brief Use a function declared with the RTEMS_DEPRECATED macro.
 */
static void RtemsBasedefsValBasedefs_Action_21( void )
{
  int deprecated_result;
  /*
   * Derivation from Coding Style:
   * The following code suppresses a compiler warning (instead of fixing it).
   * Rational: The function deprecated_func() is not really deprecated
   * but its purpose is to test the RTEMS_DEPRECATED macro.
   * The RTEMS_DEPRECATED macro must result in a compiler warning here.
   */
  _Pragma( "GCC diagnostic push" )
  _Pragma( "GCC diagnostic ignored \"-Wdeprecated-declarations\"" )
  deprecated_result = deprecated_func( 5 );
  _Pragma( "GCC diagnostic pop" )

  /*
   * It cannot automatically be checked that the RTEMS_DEPRECATED macro has the
   * desired effect. The gcc compiler should issue a warning about the use of a
   * deprecated function on the above line where the ``deprecated_func`` is
   * used.
   */
  T_step_eq_int( 50, deprecated_result, 15 );
}

/**
 * @brief Use the RTEMS_DEQUALIFY_DEPTHX() macro.
 */
static void RtemsBasedefsValBasedefs_Action_22( void )
{
  const volatile int dequalify_depthx_array[] = { 52, 55 };
  const char dequalify_depthx_var = 'a';
  const char *dequalify_depthx_one_pointer = &dequalify_depthx_var;
  const char **dequalify_depthx_two_pointer =
    &dequalify_depthx_one_pointer;
  int *dequalify_depthx_pointer;
  volatile char **dequalify_depthx_twice_pointer;
  dequalify_depthx_pointer =
    RTEMS_DEQUALIFY_DEPTHX( *, int *, dequalify_depthx_array );
  dequalify_depthx_twice_pointer = RTEMS_DEQUALIFY_DEPTHX(
    **, volatile char **, dequalify_depthx_two_pointer );

  /*
   * Check that the RTEMS_DEQUALIFY_DEPTHX() macro returns a pointer which
   * allows to write into an otherwise const (volatile) value.
   */
  T_step_eq_int( 51, dequalify_depthx_pointer[0], 52 );
  T_step_eq_int( 52, dequalify_depthx_pointer[1], 55 );
  dequalify_depthx_pointer[0] = 13;
  T_step_eq_int( 53, dequalify_depthx_pointer[0], 13 );
  T_step_eq_char( 54, **dequalify_depthx_twice_pointer, 'a' );
  **dequalify_depthx_twice_pointer = 'Z';
  T_step_eq_char( 55, **dequalify_depthx_twice_pointer, 'Z' );
}

/**
 * @brief Use the RTEMS_DEQUALIFY() macro.
 */
static void RtemsBasedefsValBasedefs_Action_23( void )
{
  const volatile int dequalify_array[] = { 52, 55 };
  int *dequalify_pointer;
  dequalify_pointer = RTEMS_DECONST( int *, dequalify_array );

  /*
   * Check that the RTEMS_DEQUALIFY() macro returns a pointer which allows to
   * write into an otherwise const volatile value.
   */
  T_step_eq_int( 56, dequalify_pointer[0], 52 );
  T_step_eq_int( 57, dequalify_pointer[1], 55 );
  dequalify_pointer[0] = 13;
  T_step_eq_int( 58, dequalify_pointer[0], 13 );
}

/**
 * @brief Use the RTEMS_DEVOLATILE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_24( void )
{
  volatile int devolatile_array[] = { 52, 55 };
  int *devolatile_pointer;
  devolatile_pointer = RTEMS_DEVOLATILE( int *, devolatile_array );

  /*
   * Check that the RTEMS_DEVOLATILE() macro returns a pointer which allows to
   * write into an otherwise volatile value.
   */
  T_step_eq_int( 59, devolatile_pointer[0], 52 );
  T_step_eq_int( 60, devolatile_pointer[1], 55 );
  devolatile_pointer[1] = 13;
  T_step_eq_int( 61, devolatile_pointer[1], 13 );
}

/**
 * @brief Invoke the  RTEMS_EXPAND() macro on an example.
 */
static void RtemsBasedefsValBasedefs_Action_25( void )
{
  int expand_result;
  expand_result = RTEMS_EXPAND( EXPAND )();

  /*
   * Check that the argument of RTEMS_EXPAND() is expanded and returned.
   */
  T_step_eq_int( 62, expand_result, 82 );
}

/**
 * @brief Invoke the  FALSE macro on an example.
 */
static void RtemsBasedefsValBasedefs_Action_26( void )
{
  char *false_result;
  false_result = _TO_STR( FALSE );

  /*
   * Check that of FALSE is substituted by 0.
   */
  T_step_eq_str( 63, false_result, "0" );
}

/**
 * @brief Invoke the  RTEMS_HAVE_MEMBER_SAME_TYPE() macro on examples.
 */
static void RtemsBasedefsValBasedefs_Action_27( void )
{
  typedef union {
    short s;
    int **i;
    char *c;
    int a[5];
  } same_type_union;
  typedef struct {
    const short u;
    short v;
    int *w;
    char *x;
    volatile int y[5];
    int z;
  } same_type_struct;
  int same_type_result_0 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, s, same_type_struct, v );
  int same_type_result_1 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, s, same_type_struct, z );
  int same_type_result_2 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, i, same_type_struct, w );
  int same_type_result_3 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, c, same_type_struct, x );
  int same_type_result_4 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, a, same_type_struct, y );
  int same_type_result_5 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, s, same_type_union, s );
  int same_type_result_6 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, i, same_type_union, i );
  int same_type_result_7 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, s, same_type_struct, y );
  int same_type_result_8 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, a, same_type_struct, w );
  int same_type_result_9 = RTEMS_HAVE_MEMBER_SAME_TYPE(
    same_type_union, s, same_type_struct, u );

  /*
   * Check that of RTEMS_HAVE_MEMBER_SAME_TYPE() returns 0 and 1 depending on
   * whether these types are compatible.
   */
  T_step_eq_int( 64, same_type_result_0, 1 );
  T_step_eq_int( 65, same_type_result_1, 0 );
  T_step_eq_int( 66, same_type_result_2, 0 );
  T_step_eq_int( 67, same_type_result_3, 1 );
  T_step_eq_int( 68, same_type_result_4, 1 );
  T_step_eq_int( 69, same_type_result_5, 1 );
  T_step_eq_int( 70, same_type_result_6, 1 );
  T_step_eq_int( 71, same_type_result_7, 0 );
  T_step_eq_int( 72, same_type_result_8, 0 );
  T_step_eq_int( 73, same_type_result_9, 1 );
}

/**
 * @brief Use the RTEMS_INLINE_ROUTINE in the definition of function
 *   inline_routine_func() at the beginning of this file. Obtain the text the
 *   macro RTEMS_INLINE_ROUTINE produces.
 */
static void RtemsBasedefsValBasedefs_Action_28( void )
{
  const int inline_routine_step = 74;
  int inline_routine_result;
  char *inline_routine_text;
  inline_routine_result = inline_routine_func( 3 );
  inline_routine_text = _TO_STR( RTEMS_INLINE_ROUTINE );

  /*
   * Check that the RTEMS_INLINE_ROUTINE exists and that it produces the
   * desired text.
   */
  if( 0 == strcmp( "static inline", inline_routine_text ) ) {
    T_step_eq_str( inline_routine_step,
      inline_routine_text, "static inline" );
  } else {
    T_step_eq_str( inline_routine_step,
      inline_routine_text, "static __inline__" );
  }
  T_step_eq_int( 75, inline_routine_result, 8 );
}

/**
 * @brief Use a function declared with the RTEMS_MALLOCLIKE macro.
 */
static void RtemsBasedefsValBasedefs_Action_29( void )
{
  void *malloclike_ptr;
  /*
   * If this code is not compiled using GNU C, I still have to run a check
   * to avoid trouble with the {step} counter of the checks.
   */
  const char *malloclike_text = "__attribute__((__malloc__))";
  malloclike_ptr = basedefs_malloclike_func( 102 );
  basedefs_free( malloclike_ptr );
  #if defined( __GNUC__ )
  malloclike_text = remove_white_space( _TO_STR( RTEMS_MALLOCLIKE ) );
  #endif

  /*
   * It cannot be checked that the RTEMS_MALLOCLIKE macro has the desired
   * effect. Yet, the check confirms that such a macro exists and that it can
   * be used on such a memory function and that it produces the correct code.
   */
  T_step_not_null( 76, malloclike_ptr );
  T_step_eq_str( 77, malloclike_text, "__attribute__((__malloc__))" );
}

/**
 * @brief Use a function declared with the RTEMS_NO_INLINE macro.
 */
static void RtemsBasedefsValBasedefs_Action_30( void )
{
  int no_inline_result;
  /*
   * If this code is not compiled using GNU C, I still have to run a check
   * to avoid trouble with the {step} counter of the checks.
   */
  const char *no_inline_text = "__attribute__((__noinline__))";
  no_inline_result = no_inline_func();
  #if defined( __GNUC__ )
  no_inline_text = remove_white_space( _TO_STR( RTEMS_NO_INLINE ) );
  #endif

  /*
   * It cannot be checked that the RTEMS_NO_INLINE macro has the desired
   * effect. Yet, the check confirms that such a macro exists and that it can
   * be used on such a function and that it produces the correct code.
   */
  T_step_eq_int( 78, no_inline_result, 75 );
  T_step_eq_str( 79, no_inline_text, "__attribute__((__noinline__))" );
}

/**
 * @brief Use of the RTEMS_NO_RETURN macro at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_31( void )
{
  (void) no_return_func;

  /*
   * It cannot be checked that the RTEMS_NO_RETURN macro has the desired
   * effect. It is only checked that such a macro exists.
   */

}

/**
 * @brief Use the RTEMS_NOINIT macro on ``noinit_variable`` at the beginning of
 *   this file.
 */
static void RtemsBasedefsValBasedefs_Action_32( void )
{
  /* No action */

  /*
   * It cannot be checked that the RTEMS_NOINIT macro has the desired effect.
   * Yet, the check confirms that such a macro exists and can be used.
   */
  T_step_not_null( 80, &noinit_variable );
}

/**
 * @brief Use the RTEMS_OBFUSCATE_VARIABLE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_33( void )
{
  short obfuscate_variable = 66;
  RTEMS_OBFUSCATE_VARIABLE( obfuscate_variable );

  /*
   * It cannot be checked that the RTEMS_OBFUSCATE_VARIABLE() macro has the
   * desired effect. Yet, the check confirms that such a macro exists and can
   * be used.
   */
  T_step_eq_int( 81, obfuscate_variable, 66 );
}

/**
 * @brief Use the RTEMS_PACKED macro.
 */
static void RtemsBasedefsValBasedefs_Action_34( void )
{
  int packed_offset;
  int packed_full_i_offset;
  int packed_full_j_offset;
  int packed_enum_size;
  typedef struct {
    uint8_t c;
    RTEMS_PACKED uint32_t i;
  } packed_struct;
  typedef struct RTEMS_PACKED {
    uint8_t c;
    uint32_t i;
    uint32_t j;
  } packed_full_struct;
  typedef enum RTEMS_PACKED {
    red = 1,
    green,
    yellow,
    blue = 255
  } packed_enum;
  packed_offset = offsetof( packed_struct, i );
  packed_full_i_offset = offsetof( packed_full_struct, i );
  packed_full_j_offset = offsetof( packed_full_struct, j );
  packed_enum_size = sizeof( packed_enum );

  /*
   * Check that RTEMS_PACKED correctly aligns a structure member.
   */
  T_step_eq_int( 82, packed_offset, 1 );

  /*
   * Check that RTEMS_PACKED correctly aligns all structure members.
   */
  T_step_eq_int( 83, packed_full_i_offset, 1 );
  T_step_eq_int( 84, packed_full_j_offset, 5 );

  /*
   * Check that RTEMS_PACKED correctly enforces a minimal enum type.
   */
  T_step_eq_int( 85, packed_enum_size, 1 );
}

/**
 * @brief Use the RTEMS_PREDICT_FALSE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_35( void )
{
  /* No action */

  /*
   * It cannot be checked that the RTEMS_PREDICT_FALSE() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and can be used.
   */
  T_step_eq_int( 86, RTEMS_PREDICT_FALSE( PREDICT_FALSE 1 ), 0 );
}

/**
 * @brief Use the RTEMS_PREDICT_TRUE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_36( void )
{
  /* No action */

  /*
   * It cannot be checked that the RTEMS_PREDICT_TRUE() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and can be used.
   */
  T_step_eq_int( 87, RTEMS_PREDICT_TRUE( 6 - 5 ), 1 );
}

/**
 * @brief Use a function declared with the RTEMS_PRINTFLIKE() macro.
 */
static void RtemsBasedefsValBasedefs_Action_37( void )
{
  int printflike_result;
  printflike_result = printflike_func(
    "RTEMS_PRINTFLIKE",
    "%d %lx %s\n",
    123,
    0xABCDEFL,
    "test output"
  );

  /*
   * It cannot automatically be checked that the RTEMS_PRINTFLIKE() macro has
   * the desired effect. Yet, the check confirms that such a macro exists and
   * that it can be used on such a printf-like function and that the argument
   * numbers are correct.
   */
  T_step_eq_int( 88, printflike_result, 23 );
}

/**
 * @brief Use the RTEMS_PURE macro at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_38( void )
{
  int pure_result;
  int pure_result_2;
  pure_result = pure_func();
  pure_result_2 = pure_func();

  /*
   * It cannot be checked that the RTEMS_PURE macro has the desired effect. It
   * is checked that such a macro exists.
   */
  T_step_eq_int( 89, pure_result, 21 );
  T_step_eq_int( 90, pure_result_2, 21 );
}

/**
 * @brief Get the code the RTEMS_RETURN_ADDRESS() macro produces as string.
 */
static void RtemsBasedefsValBasedefs_Action_39( void )
{
  /*
   * If this code is not compiled using GNU C, I still have to run a check
   * to avoid trouble with the {step} counter of the checks.
   */
  const char *return_address_text = "__builtin_return_address(0)";
  #if defined( __GNUC__ )
  return_address_text =
    remove_white_space( _TO_STR( RTEMS_RETURN_ADDRESS() ) );
  #endif

  /*
   * The check confirms that a RTEMS_RETURN_ADDRESS() macro exists and that it
   * produces the correct code.
   */
  T_step_eq_str( 91,
    return_address_text, "__builtin_return_address(0)" );
}

/**
 * @brief Use the RTEMS_SECTION() macro on ``section_variable`` and
 *   ``section_func`` at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_40( void )
{
  short section_result;
  section_result = section_func( 1234567 );

  /*
   * It cannot be checked that the RTEMS_SECTION() macro has the desired
   * effect. Yet, the check confirms that such a macro exists and can be used.
   */
  T_step_eq_int( 92, section_var, 28 );
  T_step_eq_int( 93, section_result, 67 );
}

/**
 * @brief Evaluate if RTEMS_STATIC_ANALYSIS is defined.
 */
static void RtemsBasedefsValBasedefs_Action_41( void )
{
  #if defined(RTEMS_STATIC_ANALYSIS)
  bool defined = true;
  #else
  bool defined = false;
  #endif

  /*
   * Check that RTEMS_STATIC_ANALYSIS was not defined.
   */
  T_step_false( 94, defined );
}

/**
 * @brief Use the RTEMS_STATIC_ASSERT() macro.
 */
static void RtemsBasedefsValBasedefs_Action_42( void )
{
  RTEMS_STATIC_ASSERT( STATIC_ASSERT_COND 1, RTEMS_STATIC_ASSERT_test );

  /*
   * It cannot be automatically check that the RTEMS_STATIC_ASSERT() macro has
   * the desired effect. Yet, it can be checked that the macro exists and
   * accepts the specified arguments.
   */

}

/**
 * @brief Use the RTEMS_STRING() macro.
 */
static void RtemsBasedefsValBasedefs_Action_43( void )
{
  const char *string_var;
  const char *string_empty_var;
  const char *string_multi_args_var;
  /* strange spacing and tabs belong to the test */
  string_var = RTEMS_STRING( \\ STRING_PREFIX 		cat""\n   );
  string_empty_var = RTEMS_STRING();
  string_multi_args_var = RTEMS_STRING( STRING_PREFIX, "abc", DEF );

  /*
   * Check that the RTEMS_STRING() macro converts its arguments into a single
   * string without applying pre-processor substitutions on its arguments.
   */
  T_step_eq_str( 95, string_var, "\\ STRING_PREFIX cat\"\"\n" );
  T_step_eq_str( 96, string_empty_var, "" );
  T_step_eq_str( 97, string_multi_args_var,
    "STRING_PREFIX, \"abc\", DEF" );
}

/**
 * @brief Use the RTEMS_SYMBOL_NAME() macro with an example object.
 */
static void RtemsBasedefsValBasedefs_Action_44( void )
{
  /* Nothing to do */

  /*
   * Check that the RTEMS_SYMBOL_NAME() macro expands to the expected symbol
   * name.
   */
  T_step_eq_ptr( 98, &global_object, &address_of_global_object );
}

/**
 * @brief Invoke the  TRUE macro on an example.
 */
static void RtemsBasedefsValBasedefs_Action_45( void )
{
  char *true_result;
  true_result = _TO_STR( TRUE );

  /*
   * Check that of TRUE is substituted by 0.
   */
  T_step_eq_str( 99, true_result, "1" );
}

/**
 * @brief Use of the  RTEMS_TYPEOF_REFX() macro on several examples. This use
 *   is already the test as the statements will not compile without error if
 *   the macro did not evaluate to the correct type.
 */
static void RtemsBasedefsValBasedefs_Action_46( void )
{
  int type_refx_val = 7;
  char type_refx_chr = 'c';
  char *type_refx_chr_p = &type_refx_chr;
  char **type_refx_chr_pp = &type_refx_chr_p;
  const short type_refx_const_val = 333;
  RTEMS_TYPEOF_REFX( *, int *) type_refx_x_int = 8;
  RTEMS_TYPEOF_REFX( **, int **) type_refx_xx_int = 9;
  RTEMS_TYPEOF_REFX( ***, int ***) type_refx_xxx_int = 10;
  RTEMS_TYPEOF_REFX( **, int ***) type_refx_xxx_int_p = &type_refx_val;
  RTEMS_TYPEOF_REFX( **, &type_refx_chr_p) type_refx_ax_char = 'd';
  RTEMS_TYPEOF_REFX( *, type_refx_chr_p) type_refx_x_char = 'e';
  RTEMS_TYPEOF_REFX( , *type_refx_chr_p) type_refx_char = 'f';
  RTEMS_TYPEOF_REFX( *, type_refx_chr_pp[0]) type_refx_xx_char = 'g';
  RTEMS_TYPEOF_REFX( *, const short **)
    type_refx_xx_const_short_p = &type_refx_const_val;

  /*
   * The checks here are proforma. The macro is tested by the fact that the
   * action will not compile if the macro returns a wrong result.
   */
  T_step_eq_int( 100, type_refx_val, 7 );
  T_step_eq_int( 101, type_refx_x_int, 8 );
  T_step_eq_int( 102, type_refx_xx_int, 9 );
  T_step_eq_int( 103, type_refx_xxx_int, 10 );
  T_step_eq_int( 104, *type_refx_xxx_int_p, 7 );
  T_step_eq_char( 105, type_refx_chr, 'c' );
  T_step_eq_char( 106, type_refx_ax_char, 'd' );
  T_step_eq_char( 107, type_refx_x_char, 'e' );
  T_step_eq_char( 108, type_refx_char, 'f' );
  T_step_eq_char( 109, type_refx_xx_char, 'g' );
  T_step_eq_short( 110, *type_refx_xx_const_short_p, 333 );
}

/**
 * @brief Use the RTEMS_UNUSED macro. See also unused_func() at the beginning
 *   of this file.
 */
static void RtemsBasedefsValBasedefs_Action_47( void )
{
  int unused_var RTEMS_UNUSED;
  typedef struct RTEMS_UNUSED {
    char c;
    int i;
  } unused_struct_t;
  unused_struct_t unused_struct = { '@', 13 };

  /*
   * It cannot automatically be checked that the RTEMS_UNUSED macro has the
   * desired effect. It is checked that such a macro exists and one can
   * manually check that no compiler warnings are produced for the
   * unused_func().
   */


  /*
   * It cannot automatically be checked that the RTEMS_UNUSED macro has the
   * desired effect. It is checked that such a macro exists and one can
   * manually check that no compiler warnings are produced for the
   * unused_lable.
   */
  unused_lable:
    RTEMS_UNUSED;

  /*
   * It cannot automatically be checked that the RTEMS_UNUSED macro has the
   * desired effect. It is checked that such a macro exists and one can
   * manually check that no compiler warnings are produced for the
   * unused_struct.
   */


  /*
   * It cannot automatically be checked that the RTEMS_UNUSED macro has the
   * desired effect. It is checked that such a macro exists and one can
   * manually check that no compiler warnings are produced for the unused items
   * unused_var and the unused argument and variable in unused_func().
   */

}

/**
 * @brief Use of the RTEMS_UNREACHABLE() macro in function definition of
 *   unreachable_func() at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_48( void )
{
  int unreachable_result;
  unreachable_result = unreachable_func(2101);

  /*
   * It cannot be checked that the RTEMS_UNREACHABLE() macro has the desired
   * effect. It is checked that such a macro exists and the compiler warning
   * about the missing return statement is suppressed.
   */
  T_step_eq_int( 111, unreachable_result, 2101 );
}

/**
 * @brief Use of the RTEMS_USED macro in function definition of used_func() at
 *   the beginning of this file and with used_var above.
 */
static void RtemsBasedefsValBasedefs_Action_49( void )
{
  /* No action */

  /*
   * It cannot be checked that the RTEMS_USED macro has the desired effect. It
   * is checked that such a macro exists.
   */

}

/**
 * @brief Use of the RTEMS_WARN_UNUSED_RESULT macro in function definition of
 *   warn_unused_func() at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_50( void )
{
  int warn_unused_result;
  warn_unused_result = warn_unused_func( 33 );
  /*
   * Derivation from Coding Style:
   * The following code suppresses a compiler warning (instead of fixing
   * it).
   * Rational: Ignoring the function warn_unused_func() result is not really
   * a bug but its purpose is to test the RTEMS_WARN_UNUSED_RESULT macro.
   * The RTEMS_WARN_UNUSED_RESULT macro must result in a compiler warning
   * here.
   */
  _Pragma( "GCC diagnostic push" )
  _Pragma( "GCC diagnostic ignored \"-Wunused-result\"" )
  warn_unused_func( 66 );
  _Pragma( "GCC diagnostic pop" )

  /*
   * It cannot be checked that the RTEMS_WARN_UNUSED_RESULT macro has the
   * desired effect. The GNU C compiler should issue a warning about the
   * disregarded result returned by the call to the ``warn_unused_func()``
   * function.
   */
  T_step_eq_int( 112, warn_unused_result, 11 );
}

/**
 * @brief Use of ``basedefs_weak_alias_0/1_func()`` which are defined with the
 *   RTEMS_WEAK_ALIAS() macro at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_51( void )
{
  int weak_alias_0_result;
  int weak_alias_1_result;
  weak_alias_0_result = ori_func( 3 ) + basedefs_weak_alias_0_func( 5 );
  weak_alias_1_result = ori_func( 3 ) + basedefs_weak_alias_1_func( 5 );

  /*
   * There exists no strong alias for basedefs_weak_alias_0_func(). Check that
   * ori_func() and basedefs_weak_alias_0_func() are the same function.
   */
  T_step_eq_int( 113, weak_alias_0_result, 16 );

  /*
   * File ``tc_basedefs_pndant.c`` defines a strong function for
   * basedefs_weak_alias_1_func(). Check that ori_func() and
   * basedefs_weak_alias_1_func() are not the same function.
   */
  T_step_eq_int( 114, weak_alias_1_result, 56 );
}

/**
 * @brief Use of ``basedefs_weak_0/1_var`` and ``basedefs_weak_0/1_func()``
 *   which are defined with the RTEMS_WEAK macro at the beginning of this file.
 */
static void RtemsBasedefsValBasedefs_Action_52( void )
{
  int weak_0_result;
  int weak_1_result;
  weak_0_result = basedefs_weak_0_func();
  weak_1_result = basedefs_weak_1_func();

  /*
   * For ``basedefs_weak_0_var`` and ``basedefs_weak_0_func()`` there exists no
   * other symbols with the same name. Hence, the checks test that the weak
   * symbols are used.
   */
  T_step_eq_int( 115, basedefs_weak_0_var, 60 );
  T_step_eq_int( 116, weak_0_result, 63 );

  /*
   * ``basedefs_weak_1_var`` and ``basedefs_weak_1_func()`` are overwritten by
   * strong symbols defined in file ``tc_basedefs_pendant.c``. Hence, the
   * checks test that the strong variants are used.
   */
  T_step_eq_int( 117, basedefs_weak_1_var, 62 );
  T_step_eq_int( 118, weak_1_result, 65 );
}

/**
 * @brief Invoke the RTEMS_XCONCAT() macro on examples.
 */
static void RtemsBasedefsValBasedefs_Action_53( void )
{
  int xconcat0_result;
  int xconcat1_result;
  int xconcat2_result;
  int xconcat3_result;
  xconcat0_result = RTEMS_XCONCAT( con, cat )();
  xconcat1_result = RTEMS_XCONCAT( CON, CAT )();
  xconcat2_result =
    RTEMS_XCONCAT( RTEMS_XCONCAT( CO, N ), RTEMS_XCONCAT( ca, t ) )();
  xconcat3_result = RTEMS_CONCAT( def, cat )();

  /*
   * Check that the two arguments of RTEMS_XCONCAT() are concatenated without
   * inserting new characters.
   */
  T_step_eq_int( 119, xconcat0_result, 91 );

  /*
   * Check that the two arguments of RTEMS_XCONCAT() are substituted before
   * they are concatenated.
   */
  T_step_eq_int( 120, xconcat1_result, 91 );

  /*
   * Check that the two arguments of RTEMS_XCONCAT() are can be the macro
   * itself.
   */
  T_step_eq_int( 121, xconcat2_result, 91 );

  /*
   * Check that the result of the RTEMS_XCONCAT() expansion is subject to a
   * further pre-processor substitution.
   */
  T_step_eq_int( 122, xconcat3_result, 91 );
}

/**
 * @brief Use the RTEMS_XSTRING() macro.
 */
static void RtemsBasedefsValBasedefs_Action_54( void )
{
  const char *xstring_var;
  const char *xstring_empty_var;
  const char *string_multi_args_var;
  /* strange spacing and tabs belong to the test */
  xstring_var = RTEMS_XSTRING( \\ STRING_PREFIX 		cat""\n   );
  xstring_empty_var = RTEMS_XSTRING();
  string_multi_args_var = RTEMS_XSTRING( STRING_PREFIX, abc, "abc", DEF );

  /*
   * Check that the RTEMS_XSTRING() macro applies pre-processor substitutions
   * on its arguments and converts its arguments into a single string.
   */
  T_step_eq_str( 123, xstring_var, "\\ str cat\"\"\n" );
  T_step_eq_str( 124, xstring_empty_var, "" );
  T_step_eq_str( 125, string_multi_args_var,
    "str, ABC, \"abc\", DEF" );
}

/**
 * @brief Use of the RTEMS_ZERO_LENGTH_ARRAY macro in a declaration of a
 *   structure.
 */
static void RtemsBasedefsValBasedefs_Action_55( void )
{
  typedef struct {
    char chr;
    int array[RTEMS_ZERO_LENGTH_ARRAY];
  } zero_length_struct_0;
  typedef struct {
    char chr;
    int array[1];
  } zero_length_struct_1;

  /*
   * Checked that the RTEMS_ZERO_LENGTH_ARRAY macro produces a structure
   * similar to a structure with one element.
   */
  T_step_eq_sz( 126, sizeof( zero_length_struct_0 ),
    sizeof( zero_length_struct_1 ) - sizeof( int ) );
  T_step_eq_sz( 127, offsetof( zero_length_struct_0, chr ),
    offsetof( zero_length_struct_1, chr ) );
  T_step_eq_sz( 128, offsetof( zero_length_struct_0, array ),
    offsetof( zero_length_struct_1, array ) );
}

/**
 * @brief Use the RTEMS_DEFINE_GLOBAL_SYMBOL() macro at the beginning of this
 *   file and assign the address of the symbol to an object.
 */
static void RtemsBasedefsValBasedefs_Action_56( void )
{
  /* No action */

  /*
   * Check that the RTEMS_DEFINE_GLOBAL_SYMBOL() macro defines a global symbol
   * with the correct value.
   */
  T_step_eq_uptr( 129, (uintptr_t) global_symbol_2_object, 0x123 );
}

/**
 * @fn void T_case_body_RtemsBasedefsValBasedefs( void )
 */
T_TEST_CASE( RtemsBasedefsValBasedefs )
{
  T_plan( 130 );

  RtemsBasedefsValBasedefs_Action_0();
  RtemsBasedefsValBasedefs_Action_1();
  RtemsBasedefsValBasedefs_Action_2();
  RtemsBasedefsValBasedefs_Action_3();
  RtemsBasedefsValBasedefs_Action_4();
  RtemsBasedefsValBasedefs_Action_5();
  RtemsBasedefsValBasedefs_Action_6();
  RtemsBasedefsValBasedefs_Action_7();
  RtemsBasedefsValBasedefs_Action_8();
  RtemsBasedefsValBasedefs_Action_9();
  RtemsBasedefsValBasedefs_Action_10();
  RtemsBasedefsValBasedefs_Action_11();
  RtemsBasedefsValBasedefs_Action_12();
  RtemsBasedefsValBasedefs_Action_13();
  RtemsBasedefsValBasedefs_Action_14();
  RtemsBasedefsValBasedefs_Action_15();
  RtemsBasedefsValBasedefs_Action_16();
  RtemsBasedefsValBasedefs_Action_17();
  RtemsBasedefsValBasedefs_Action_18();
  RtemsBasedefsValBasedefs_Action_19();
  RtemsBasedefsValBasedefs_Action_20();
  RtemsBasedefsValBasedefs_Action_21();
  RtemsBasedefsValBasedefs_Action_22();
  RtemsBasedefsValBasedefs_Action_23();
  RtemsBasedefsValBasedefs_Action_24();
  RtemsBasedefsValBasedefs_Action_25();
  RtemsBasedefsValBasedefs_Action_26();
  RtemsBasedefsValBasedefs_Action_27();
  RtemsBasedefsValBasedefs_Action_28();
  RtemsBasedefsValBasedefs_Action_29();
  RtemsBasedefsValBasedefs_Action_30();
  RtemsBasedefsValBasedefs_Action_31();
  RtemsBasedefsValBasedefs_Action_32();
  RtemsBasedefsValBasedefs_Action_33();
  RtemsBasedefsValBasedefs_Action_34();
  RtemsBasedefsValBasedefs_Action_35();
  RtemsBasedefsValBasedefs_Action_36();
  RtemsBasedefsValBasedefs_Action_37();
  RtemsBasedefsValBasedefs_Action_38();
  RtemsBasedefsValBasedefs_Action_39();
  RtemsBasedefsValBasedefs_Action_40();
  RtemsBasedefsValBasedefs_Action_41();
  RtemsBasedefsValBasedefs_Action_42();
  RtemsBasedefsValBasedefs_Action_43();
  RtemsBasedefsValBasedefs_Action_44();
  RtemsBasedefsValBasedefs_Action_45();
  RtemsBasedefsValBasedefs_Action_46();
  RtemsBasedefsValBasedefs_Action_47();
  RtemsBasedefsValBasedefs_Action_48();
  RtemsBasedefsValBasedefs_Action_49();
  RtemsBasedefsValBasedefs_Action_50();
  RtemsBasedefsValBasedefs_Action_51();
  RtemsBasedefsValBasedefs_Action_52();
  RtemsBasedefsValBasedefs_Action_53();
  RtemsBasedefsValBasedefs_Action_54();
  RtemsBasedefsValBasedefs_Action_55();
  RtemsBasedefsValBasedefs_Action_56();
}

/** @} */
