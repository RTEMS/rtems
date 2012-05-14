/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if defined(STRING_TO_POINTER)
  #define GOOD_VALUE        0x123
  #define GOOD_VALUE_STRING "0x123"
  #define BAD_VALUE_STRING  "xxx"
#elif defined(STRING_TO_INTEGER)
  #define GOOD_VALUE        123
  #define GOOD_VALUE_STRING "123"
  #define BAD_VALUE_STRING  "YYY"
#elif defined(STRING_TO_FLOAT)
  #define GOOD_VALUE        1.23
  #define GOOD_VALUE_STRING "1.23"
  #define BAD_VALUE_STRING  "zzz"
#else
  #error "what type are we testing?"
#endif

/* forward declarations to avoid warnings */
void TEST_STRING_TO_NAME(void);

void TEST_STRING_TO_NAME(void)
{
  TEST_STRING_TO_TYPE  value;
  rtems_status_code    status;
  char                *endptr;

  puts( "\nTesting " STRING_TO_NAME_METHOD_STRING );

  /* Null pointer for return value */
  puts(
    STRING_TO_NAME_METHOD_STRING
    " - NULL return value - RTEMS_INVALID_ADDRESS"
  );
  #if defined(STRING_TO_INTEGER)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, NULL, &endptr, 10 );
  #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, NULL, &endptr );
  #endif
  rtems_test_assert( status == RTEMS_INVALID_ADDRESS );

  /* Basic conversion works for return value, return end pointer */
  puts(
    STRING_TO_NAME_METHOD_STRING " - " GOOD_VALUE_STRING
    " NULL endptr return value - RTEMS_SUCCESSFUL"
  );
  #if defined(STRING_TO_INTEGER)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, &value, NULL, 10 );
  #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, &value, NULL );
  #endif
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  rtems_test_assert( value == (TEST_STRING_TO_TYPE)GOOD_VALUE );

  #if defined(STRING_TO_MAX)
    /* Basic conversion works for return value */
    endptr = NULL;
    puts(
      STRING_TO_NAME_METHOD_STRING " - MAXIMUM VALUE"
      " w/endptr return value - RTEMS_SUCCESSFUL"
    );
    #if defined(STRING_TO_INTEGER)
      status = STRING_TO_NAME_METHOD(
        STRING_TO_MAX_STRING, &value, &endptr, 10 );
    #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
      status = STRING_TO_NAME_METHOD( STRING_TO_MAX_STRING, &value, &endptr );
    #endif
    rtems_test_assert( status == RTEMS_SUCCESSFUL );
    rtems_test_assert( endptr );
    rtems_test_assert( value == (TEST_STRING_TO_TYPE)STRING_TO_MAX );
  #endif

  /* Basic conversion works for return value */
  endptr = NULL;
  puts(
    STRING_TO_NAME_METHOD_STRING " - " GOOD_VALUE_STRING
    " w/endptr return value - RTEMS_SUCCESSFUL"
  );
  #if defined(STRING_TO_INTEGER)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, &value, &endptr, 10 );
  #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
    status = STRING_TO_NAME_METHOD( GOOD_VALUE_STRING, &value, &endptr );
  #endif
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  rtems_test_assert( endptr );
  rtems_test_assert( value == (TEST_STRING_TO_TYPE)GOOD_VALUE );

  /* Bad conversion works for return value */
  endptr = NULL;
  puts(
    STRING_TO_NAME_METHOD_STRING " - " BAD_VALUE_STRING
    " w/endptr return value - RTEMS_NOT_DEFINED"
  );
  #if defined(STRING_TO_INTEGER)
    status = STRING_TO_NAME_METHOD( BAD_VALUE_STRING, &value, &endptr, 10 );
  #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
    status = STRING_TO_NAME_METHOD( BAD_VALUE_STRING, &value, &endptr );
  #endif
  rtems_test_assert( status == RTEMS_NOT_DEFINED );
  rtems_test_assert( endptr );

  /* Conversion of empty string */
  endptr = NULL;
  value = 0;
  puts(
    STRING_TO_NAME_METHOD_STRING
    " - empty string - w/endptr return value - RTEMS_NOT_DEFINED"
  );
  #if defined(STRING_TO_INTEGER)
    status = STRING_TO_NAME_METHOD( "", &value, &endptr, 10 );
  #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
    status = STRING_TO_NAME_METHOD( "", &value, &endptr );
  #endif
  rtems_test_assert( status == RTEMS_NOT_DEFINED );
  rtems_test_assert( endptr );
  rtems_test_assert( value == (TEST_STRING_TO_TYPE)0 );

  /* Conversion of number that is too large */
  #if defined(TEST_TOO_LARGE_STRING)
    endptr = NULL;
    value = 0;
    puts(
    STRING_TO_NAME_METHOD_STRING " - overflow - RTEMS_INVALID_NUMBER" );
    #if defined(STRING_TO_INTEGER)
      status = STRING_TO_NAME_METHOD(
                 TEST_TOO_LARGE_STRING, &value, &endptr, 10 );
    #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
      status = STRING_TO_NAME_METHOD( TEST_TOO_LARGE_STRING, &value, &endptr );
    #endif
    if ( status != RTEMS_INVALID_NUMBER )
      printf( "ERROR = %s\n", rtems_status_text(status) );
    rtems_test_assert( status == RTEMS_INVALID_NUMBER );
    rtems_test_assert( endptr );
  #endif


  /* Conversion of number that is too large for unsigned char */
  #if defined(TEST_TOO_LARGE_FOR_UCHAR)
    endptr = NULL;
    value = 0;
    puts(
    STRING_TO_NAME_METHOD_STRING " - overflow - RTEMS_INVALID_NUMBER" );
    #if defined(STRING_TO_INTEGER)
      status = STRING_TO_NAME_METHOD(
                 TEST_TOO_LARGE_FOR_UCHAR, &value, &endptr, 10 );
    #endif
    if ( status != RTEMS_INVALID_NUMBER )
      printf( "ERROR = %s\n", rtems_status_text(status) );
    rtems_test_assert( status == RTEMS_INVALID_NUMBER );
    rtems_test_assert( endptr );
  #endif

  /* Conversion of number that is too small */
  #if defined(TEST_TOO_SMALL_STRING)
    endptr = NULL;
    value = 0;
    puts( STRING_TO_NAME_METHOD_STRING "- RTEMS_INVALID_NUMBER" );
    #if defined(STRING_TO_INTEGER)
      status = STRING_TO_NAME_METHOD(
                 TEST_TOO_SMALL_STRING, &value, &endptr, 10 );
    #elif defined(STRING_TO_POINTER) || defined(STRING_TO_FLOAT)
      status = STRING_TO_NAME_METHOD( TEST_TOO_SMALL_STRING, &value, &endptr );
    #endif
    rtems_test_assert( status == RTEMS_INVALID_NUMBER );
    rtems_test_assert( endptr );
  #endif
}

/* Now undefined everything that instantiates this */
#undef TEST_STRING_TO_TYPE
#undef TEST_STRING_TO_NAME
#undef STRING_TO_NAME_METHOD
#undef STRING_TO_NAME_METHOD_STRING
#undef STRING_TO_INTEGER
#undef STRING_TO_POINTER
#undef STRING_TO_FLOAT
#undef STRING_TO_MAX
#undef STRING_TO_MAX_STRING
#undef GOOD_VALUE
#undef GOOD_VALUE_STRING
#undef BAD_VALUE_STRING
#undef TEST_TOO_LARGE_STRING
#undef TEST_TOO_SMALL_STRING
#undef TEST_TOO_LARGE_FOR_UCHAR
