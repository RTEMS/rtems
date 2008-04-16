#pragma pack(1)

#include "types.h"

#ifndef BYTE_ORDER
	#error "BYTE_ORDER not defined"
#endif

#if BYTE_ORDER != BIG_ENDIAN
	#define NOSWAP
#endif

//template <typename T> struct Blah32
//{
//	unsigned T i;
//	#ifdef NOSWAP
//		operator T () { return i; }
//		T & operator = (T i) { return this->i = i; }
//	#else
//		operator T () { return i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
//		T & operator = (T i) { return this->i = i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
//	#endif
//	Blah() {}
//	Blah(T i) { *this = i; }
//};

struct unsigned_int
{
	unsigned int i;
	#ifdef NOSWAP
		operator unsigned int () { return i; }
		unsigned int & operator = (unsigned int i) { return this->i = i; }
	#else
		operator unsigned int () { return i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
		unsigned int & operator = (unsigned int i) { return this->i = i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
	#endif
	unsigned_int() {}
	unsigned_int(unsigned int i) { *this = i; }
};

struct unsigned_long
{
	unsigned long i;
	#ifdef NOSWAP
		operator unsigned long () { return i; }
		unsigned long & operator = (unsigned long i) { return this->i = i; }
	#else
		operator unsigned long () { return i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
		unsigned long & operator = (unsigned long i) { return this->i = i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
	#endif
	unsigned_long() {}
	unsigned_long(unsigned long i) { *this = i; }
};

struct unsigned_short
{
	unsigned short i;
	#ifdef NOSWAP
		operator unsigned short () { return i; }
		unsigned short & operator = (unsigned short i) { return this->i = i; }
	#else
		operator unsigned short () { return i>>8 | i<<8; }
		unsigned short & operator = (unsigned short i) { return this->i = i>>8 | i<<8; }
	#endif
	unsigned_short() {}
	unsigned_short(unsigned short i) { *this = i; }
};

struct signed_int
{
	unsigned int i;
	#ifdef NOSWAP
		operator signed int () { return i; }
		signed int & operator = (signed int i) { return (signed int &)this->i = i; }
	#else
		operator signed int () { return i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
		signed int & operator = (signed int i) { return (signed int &)this->i = i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
	#endif
	signed_int() {}
	signed_int(signed int i) { *this = i; }
};

struct signed_long
{
	unsigned long i;
	#ifdef NOSWAP
		operator signed long () { return i; }
		signed long & operator = (signed long i) { return (signed long &)this->i = i; }
	#else
		operator signed long () { return i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
		signed long & operator = (signed long i) { return (signed long &)this->i = i<<24 | i<<8&0xFF0000 | i>>8&0xFF00 | i>>24; }
	#endif
	signed_long() {}
	signed_long(signed long i) { *this = i; }
};

struct signed_short
{
	unsigned short i;
	#ifdef NOSWAP
		operator signed short () { return i; }
		signed short & operator = (unsigned short i) { return (signed short &)this->i = i; }
	#else
		operator signed short () { return i>>8 | i<<8; }
		signed short & operator = (unsigned short i) { return (signed short &)this->i = i>>8 | i<<8; }
	#endif
	signed_short() {}
	signed_short(signed short i) { *this = i; }
};

#pragma pack()
