/*
 *  COPYRIGHT (c) 2004.
 *  Ralf Corsepius, Ulm/Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __rtems_inttypes_h
#define __rtems_inttypes_h

#include <sys/_stdint.h>

#define __STRINGIFY(a) #a

/* 8-bit types */
#define __PRI8(x) __STRINGIFY(x)

#define PRId8		__PRI8(d)
#define PRIi8		__PRI8(i)
#define PRIo8		__PRI8(o)
#define PRIu8		__PRI8(u)
#define PRIx8		__PRI8(x)
#define PRIX8		__PRI8(X)

#define PRIdLEAST8	__PRI8(d)
#define PRIiLEAST8	__PRI8(i)
#define PRIoLEAST8	__PRI8(o)
#define PRIuLEAST8	__PRI8(u)
#define PRIxLEAST8	__PRI8(x)
#define PRIXLEAST8	__PRI8(X)

#define PRIdFAST8	__PRI8(d)
#define PRIiFAST8	__PRI8(i)
#define PRIoFAST8	__PRI8(o)
#define PRIuFAST8	__PRI8(u)
#define PRIxFAST8	__PRI8(x)
#define PRIXFAST8	__PRI8(X)

/* 16-bit types */
#define __PRI16(x) __STRINGIFY(x)

#define PRId16		__PRI16(d)
#define PRIi16		__PRI16(i)
#define PRIo16		__PRI16(o)
#define PRIu16		__PRI16(u)
#define PRIx16		__PRI16(x)
#define PRIX16		__PRI16(X)

#define PRIdLEAST16	__PRI16(d)
#define PRIiLEAST16	__PRI16(i)
#define PRIoLEAST16	__PRI16(o)
#define PRIuLEAST16	__PRI16(u)
#define PRIxLEAST16	__PRI16(x)
#define PRIXLEAST16	__PRI16(X)

#define PRIdFAST16	__PRI16(d)
#define PRIiFAST16	__PRI16(i)
#define PRIoFAST16	__PRI16(o)
#define PRIuFAST16	__PRI16(u)
#define PRIxFAST16	__PRI16(x)
#define PRIXFAST16	__PRI16(X)

/* 32-bit types */
#if defined(__rtems_long32)
#define __PRI32(x) __STRINGIFY(l##x)
#else
#define __PRI32(x) __STRINGIFY(x)
#endif

#define PRId32		__PRI32(d)
#define PRIi32		__PRI32(i)
#define PRIo32		__PRI32(o)
#define PRIu32		__PRI32(u)
#define PRIx32		__PRI32(x)
#define PRIX32		__PRI32(X)

#define PRIdLEAST32	__PRI32(d)
#define PRIiLEAST32	__PRI32(i)
#define PRIoLEAST32	__PRI32(o)
#define PRIuLEAST32	__PRI32(u)
#define PRIxLEAST32	__PRI32(x)
#define PRIXLEAST32	__PRI32(X)

#define PRIdFAST32	__PRI32(d)
#define PRIiFAST32	__PRI32(i)
#define PRIoFAST32	__PRI32(o)
#define PRIuFAST32	__PRI32(u)
#define PRIxFAST32	__PRI32(x)
#define PRIXFAST32	__PRI32(X)

/* 64-bit types */
#if defined(__rtems_longlong64)
#define __PRI64(x) __STRINGIFY(ll##x)
#elif defined(__rtems_long64)
#define __PRI64(x) __STRINGIFY(l##x)
#else
#define __PRI64(x) __STRINGIFY(x)
#endif

#define PRId64		__PRI64(d)
#define PRIi64		__PRI64(i)
#define PRIo64		__PRI64(o)
#define PRIu64		__PRI64(u)
#define PRIx64		__PRI64(x)
#define PRIX64		__PRI64(X)

#define PRIdLEAST64	__PRI64(d)
#define PRIiLEAST64	__PRI64(i)
#define PRIoLEAST64	__PRI64(o)
#define PRIuLEAST64	__PRI64(u)
#define PRIxLEAST64	__PRI64(x)
#define PRIXLEAST64	__PRI64(X)

#define PRIdFAST64	__PRI64(d)
#define PRIiFAST64	__PRI64(i)
#define PRIoFAST64	__PRI64(o)
#define PRIuFAST64	__PRI64(u)
#define PRIxFAST64	__PRI64(x)
#define PRIXFAST64	__PRI64(X)

/* max-bit types */
#if defined(__rtems_longlong64)
#define __PRIMAX(x) __STRINGIFY(ll##x)
#elif defined(__rtems_long64)
#define __PRIMAX(x) __STRINGIFY(l##x)
#else
#define __PRIMAX(x) __STRINGIFY(x)
#endif

#define PRIdMAX		__PRIMAX(d)
#define PRIiMAX		__PRIMAX(i)
#define PRIoMAX		__PRIMAX(o)
#define PRIuMAX		__PRIMAX(u)
#define PRIxMAX		__PRIMAX(x)
#define PRIXMAX		__PRIMAX(X)

/* ptr types */
#if defined(__rtems_longlong64)
#define __PRIPTR(x) __STRINGIFY(ll##x)
#elif defined(__rtems_long64)
#define __PRIPTR(x) __STRINGIFY(l##x)
#else
#define __PRIPTR(x) __STRINGIFY(x)
#endif

#define PRIdPTR		__PRIPTR(d)
#define PRIiPTR		__PRIPTR(i)
#define PRIoPTR		__PRIPTR(o)
#define PRIuPTR		__PRIPTR(u)
#define PRIxPTR		__PRIPTR(x)
#define PRIXPTR		__PRIPTR(X)

typedef struct {
  intmax_t	quot;
  intmax_t	rem;
} imaxdiv_t;

#ifdef __cplusplus
extern "C" {
#endif

extern intmax_t  imaxabs(intmax_t j);
extern imaxdiv_t imaxdiv(intmax_t numer, intmax_t denomer);
extern intmax_t  strtoimax(const char *__restrict, char **__restrict, int);
extern uintmax_t strtoumax(const char *__restrict, char **__restrict, int);
extern intmax_t  wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int);
extern uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int);

#ifdef __cplusplus
}
#endif

#endif /* __rtems_inttypes_h */
