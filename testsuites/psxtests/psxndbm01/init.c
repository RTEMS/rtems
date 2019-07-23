/**
 *  @file
 *  @brief Test suite for ndbm.h methods
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 Vaibhav Gupta
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* header files are listed in lexical/lexicographical/alphabetical order */

#include <errno.h>
#include <fcntl.h>      /* contains definitions of 'open_flags' */
#include <limits.h>
#include <ndbm.h>       /* contains declarations of ndbm methods */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>   /* contains definitions of 'file_mode' */
#include <string.h>
#include <rtems/test.h>
#include <tmacros.h>

const char rtems_test_name[] = "PSXNDBM 01";

#define NAME      "VARoDeK"
#define PHONE_NO  "123-321-777-888"
#define DB_NAME   "phones_test"
#define NAME2     "VG"
#define PHONE_NO2 "321-123-888-777"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

/*
* This Function takes DBM* as a argument and count the number of records in the
* database pointed by it.
*/
static int count_no_of_records( DBM *db_local )
{
  int count = 0;
  datum temp;

  for (
    temp = dbm_firstkey( db_local );
    temp.dptr != NULL;
    temp = dbm_nextkey( db_local ), count++
  );

  return count;
}

/* Test Function Begins */
rtems_task Init(rtems_task_argument ignored)
{
  datum name          = { NAME, sizeof( NAME ) };
  datum put_phone_no  = { PHONE_NO, sizeof( PHONE_NO ) };
  datum name2         = { NAME2, sizeof( NAME2 ) };
  datum put_phone_no2 = { PHONE_NO2, sizeof( PHONE_NO2 ) };

  datum get_phone_no, key;

  int i;
  char *test_strings;

  DBM *db;

  TEST_BEGIN();

/* A Simple test to check if ndbm methods are call-able */

/*
 * A Simple test to check if NDBM methods are call-able
 *
 * We will try to open a database and then close it.
 * If it successful, hence we can have further tests.
 * Also, while opening it for first time, will create that database,
 * hence we will be able to test for 'O_RDWR | O_EXCL' case later.
 * Meanwhile we will also store one record, this record will be helpful in
 * further tests.
 * And fetch it, to make sure if basic NDBM methods are working correctly.
 */

  puts( "\nOpen Database." );
  db = dbm_open( DB_NAME, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
  rtems_test_assert( db != NULL );

  /* This data will be useful in further tests */
  puts( "Store Records in Database." );
  dbm_store( db, name, put_phone_no, DBM_INSERT );

  puts( "Fetch Records from Database and check." );
  get_phone_no = dbm_fetch( db, name );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO ) == 0 );

  puts( "Close Database." );
  dbm_close( db );

/* dbm_open() */

  puts( "\nTestcases for 'dbm_open()'." );

/* The 'DB_NAME' is already created, hence 'O_RDWR | O_EXCL' should fail. */
  puts( "Use 'O_CREAT | O_EXCL' to open existing file and confirm error." );
  db = dbm_open( DB_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU );
  rtems_test_assert( db == NULL );
  rtems_test_assert( errno == EEXIST );

/* Some implementations use 3 characters for the suffix and others use
 * 4 characters for the suffix, applications should ensure that the maximum
 * portable pathname length passed to dbm_open() is no greater than
 * {PATH_MAX}-4 bytes, with the last component of the pathname no greater
 * than {NAME_MAX}-4 bytes.
 */

/* inside 'ndbm.h' ; '#define	DBM_SUFFIX	".db"' ;
 * 2 alphabets and 1 period, hence 3 characters are used for suffix
 * in this implementation.
 */

  puts( "Use path name larger than '{PATH_MAX}-3 bytes.' and confirm error." );
  test_strings = (char*)malloc( PATH_MAX - 2 );
  for ( i = 0; i < PATH_MAX - 3; i++ ) {
    test_strings[i] = 'r';
  }
  test_strings[i] = '\0';
  db = dbm_open(
                (const char*)test_strings,
                O_RDWR | O_CREAT | O_TRUNC,
                S_IRWXU
  );
  rtems_test_assert( db == NULL );
  rtems_test_assert( errno == ENAMETOOLONG );
  free( test_strings );

/* database opened for write-only access opens the files for read and
 * write access or it will fail.
 */

/* Implementation of __hash_open in newlib does not support `O_WRONLY` */

  puts( "Open file with write access only and confirm error." );
  db = dbm_open( DB_NAME, O_WRONLY, S_IRWXU );
  rtems_test_assert( db == NULL );
  rtems_test_assert( errno == EINVAL );

/* dbm_store() */

  puts( "\nTestcases for 'dbm_store()'" );
  db = dbm_open( DB_NAME, O_RDWR, S_IRWXU );
  rtems_test_assert( db != NULL );

  puts( "Insert new record with same key using 'DBM_INSERT' mode and "
        "confirm error." );
  rtems_test_assert( dbm_store( db, name, put_phone_no2, DBM_INSERT ) == 1 );

  get_phone_no = dbm_fetch( db, name );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO ) == 0 );

  puts( "Insert new record with same key using 'DBM_REPLACE' mode and "
        "confirm changes." );
  rtems_test_assert( dbm_store( db, name, put_phone_no2, DBM_REPLACE ) == 0 );

  get_phone_no = dbm_fetch( db, name );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO2 ) == 0 );

/* Revert for next tests */
  rtems_test_assert( dbm_store( db, name, put_phone_no, DBM_REPLACE ) == 0 );

  puts( "Store a new record and "
        "confirm that total number of records is successful 2." );
  rtems_test_assert( dbm_store( db, name2, put_phone_no2, DBM_INSERT ) == 0 );

/* Confirm number of records */
  rtems_test_assert( count_no_of_records( db ) == 2 );

  dbm_close( db );

/* dbm_fetch() */

  puts( "\nTestcases for 'dbm_fetch()'" );
  db = dbm_open( DB_NAME, O_RDONLY, S_IRWXU );
  rtems_test_assert( db != NULL );

  puts( "Fetch existing records and confirm results." );
  get_phone_no = dbm_fetch( db, name );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO ) == 0 );

  get_phone_no = dbm_fetch( db, name2 );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO2 ) == 0 );

  puts( "Fetch non-existing record and confirm error." );
  test_strings = (char*)malloc(6);
  strncpy( test_strings, "Hello", 5 );

  test_strings[5] = '\0';

/* The data pointed by test_string is now pointed by key.dptr */
  key.dptr = test_strings;
  key.dsize = sizeof( test_strings );
  get_phone_no = dbm_fetch( db, key );
  rtems_test_assert( get_phone_no.dptr == NULL );
  dbm_close( db );

/* We need the 'key' object, hence we cannot free 'test_strings' */

/* dbm_delete() */

  puts( "\nTestcases for 'dbm_delete()'" );
  db = dbm_open( DB_NAME, O_RDWR, S_IRWXU );
  rtems_test_assert( db != NULL );

  puts( "Delete non-existing record and confirm error." );
  rtems_test_assert( dbm_delete( db, key ) != 0 );
  free( test_strings );
  rtems_test_assert( count_no_of_records( db ) == 2);

  puts( "Delete existing record and "
        "confirm that total number of records is successful 1." );
  rtems_test_assert( dbm_delete( db, name ) == 0 );
  rtems_test_assert( count_no_of_records( db ) == 1);

  puts( "Confirm if correct record is deleted." );
  get_phone_no = dbm_fetch( db, name );
  rtems_test_assert( get_phone_no.dptr == NULL );

/* record returned by 'dbm_firstkey()' should be the only record
 * left, this should be checked to confirm correct working of
 * 'dbm_firstkey()'.
 * Check if the data is not corrupted after usage of 'dbm_delete()'
 */

  puts( "Check if the data is not corrupted after usage of 'dbm_delete()'." );
  get_phone_no = dbm_fetch( db, dbm_firstkey( db ) );
  rtems_test_assert( strcmp( (const char*)get_phone_no.dptr, PHONE_NO2 ) == 0 );

/* Empty the database and then try to use 'dbm_firstkey()', the
 * dptr pointer should point to NULL.
 */

  puts( "Empty records in database and check results of 'dbm_firstkey()'." );
  rtems_test_assert( dbm_delete( db, dbm_firstkey( db ) ) == 0 );
  key = dbm_firstkey( db );
  rtems_test_assert( key.dptr == NULL );
  dbm_close( db );

/*
* All cases for 'dbm_firstkey()' and 'dbm_nextkey()' were tested while
* performing other tests.
* One such case be found in count_number_of_records() function.
*/

  TEST_END();
  rtems_test_exit(0);
}

/* NOTICE: the clock driver is explicitly disabled */

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS                  1

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
