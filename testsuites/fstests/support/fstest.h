/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __FSTEST_H
#define __FSTEST_H

#define TIME_PRECISION  (2)
#define TIME_EQUAL(x,y) (abs((x)-(y))<TIME_PRECISION)


#define FS_PASS() do {puts("PASS");} while (0)
#define FS_FAIL() do {\
  printf( "FAIL   %s: %d \n", __FILE__, __LINE__ );\
 } while (0)


#define SHOW_MESSAGE(e, func, ...) printf(\
    "Testing %-10s with arguments: %-20s EXPECT %s\n",\
    #func,#__VA_ARGS__,#e)

#define EXPECT_EQUAL(expect, function, ...)  do { \
  SHOW_MESSAGE(#expect,function,__VA_ARGS__);\
 if (expect==function(__VA_ARGS__)) \
     FS_PASS();\
 else \
     FS_FAIL();\
   } while (0)

#define EXPECT_UNEQUAL(expect, function, ...)  do { \
  SHOW_MESSAGE(#expect,function,__VA_ARGS__);\
 if (expect!=function(__VA_ARGS__)) \
     FS_PASS();\
 else\
     FS_FAIL();\
   } while (0)

#define EXPECT_ERROR(ERROR, function, ...)  do { \
  SHOW_MESSAGE(#ERROR,function,#__VA_ARGS__);\
 if ((-1==function(__VA_ARGS__)) && (errno==ERROR)) \
     FS_PASS();\
 else \
     FS_FAIL();\
   } while (0)

void test(void);

#define BASE_FOR_TEST "/mnt"
#endif
