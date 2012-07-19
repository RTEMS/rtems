/*
 *  polled_io.c -- Basic input/output for early boot
 *
 *  Copyright (C) 1998, 1999 Gabriel Paubert, paubert@iram.es
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <sys/types.h>
#include <libcpu/byteorder.h>
#include <libcpu/page.h>
#include <libcpu/mmu.h>
#include <libcpu/io.h>
#include <string.h>
#include <stdarg.h>
#include <bsp/consoleIo.h>
#include <bsp.h>
#include <libcpu/spr.h>

#if 0
#ifdef BSP_KBD_IOBASE
#define USE_KBD_SUPPORT
#endif
#ifdef BSP_VGA_IOBASE
#define USE_VGA_SUPPORT
#endif

#ifdef USE_KBD_SUPPORT
#include "keyboard.h"
#endif
#include "console.inl"

#ifdef __BOOT__
extern void boot_udelay();
void * __palloc(u_long);
void  pfree(void *);
#else
#include <rtems/bspIo.h>
#endif

typedef unsigned long long u64;
typedef long long s64;
typedef unsigned int u32;

#ifndef __BOOT__
BSP_output_char_function_type	  BSP_output_char = debug_putc_onlcr;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
#endif

#ifdef USE_KBD_SUPPORT
unsigned short plain_map[NR_KEYS] = {
	0xf200,	0xf01b,	0xf031,	0xf032,	0xf033,	0xf034,	0xf035,	0xf036,
	0xf037,	0xf038,	0xf039,	0xf030,	0xf02d,	0xf03d,	0xf07f,	0xf009,
	0xfb71,	0xfb77,	0xfb65,	0xfb72,	0xfb74,	0xfb79,	0xfb75,	0xfb69,
	0xfb6f,	0xfb70,	0xf05b,	0xf05d,	0xf201,	0xf702,	0xfb61,	0xfb73,
	0xfb64,	0xfb66,	0xfb67,	0xfb68,	0xfb6a,	0xfb6b,	0xfb6c,	0xf03b,
	0xf027,	0xf060,	0xf700,	0xf05c,	0xfb7a,	0xfb78,	0xfb63,	0xfb76,
	0xfb62,	0xfb6e,	0xfb6d,	0xf02c,	0xf02e,	0xf02f,	0xf700,	0xf30c,
	0xf703,	0xf020,	0xf207,	0xf100,	0xf101,	0xf102,	0xf103,	0xf104,
	0xf105,	0xf106,	0xf107,	0xf108,	0xf109,	0xf208,	0xf209,	0xf307,
	0xf308,	0xf309,	0xf30b,	0xf304,	0xf305,	0xf306,	0xf30a,	0xf301,
	0xf302,	0xf303,	0xf300,	0xf310,	0xf206,	0xf200,	0xf03c,	0xf10a,
	0xf10b,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf01c,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf601,	0xf602,	0xf117,	0xf600,	0xf119,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short shift_map[NR_KEYS] = {
	0xf200,	0xf01b,	0xf021,	0xf040,	0xf023,	0xf024,	0xf025,	0xf05e,
	0xf026,	0xf02a,	0xf028,	0xf029,	0xf05f,	0xf02b,	0xf07f,	0xf009,
	0xfb51,	0xfb57,	0xfb45,	0xfb52,	0xfb54,	0xfb59,	0xfb55,	0xfb49,
	0xfb4f,	0xfb50,	0xf07b,	0xf07d,	0xf201,	0xf702,	0xfb41,	0xfb53,
	0xfb44,	0xfb46,	0xfb47,	0xfb48,	0xfb4a,	0xfb4b,	0xfb4c,	0xf03a,
	0xf022,	0xf07e,	0xf700,	0xf07c,	0xfb5a,	0xfb58,	0xfb43,	0xfb56,
	0xfb42,	0xfb4e,	0xfb4d,	0xf03c,	0xf03e,	0xf03f,	0xf700,	0xf30c,
	0xf703,	0xf020,	0xf207,	0xf10a,	0xf10b,	0xf10c,	0xf10d,	0xf10e,
	0xf10f,	0xf110,	0xf111,	0xf112,	0xf113,	0xf213,	0xf203,	0xf307,
	0xf308,	0xf309,	0xf30b,	0xf304,	0xf305,	0xf306,	0xf30a,	0xf301,
	0xf302,	0xf303,	0xf300,	0xf310,	0xf206,	0xf200,	0xf03e,	0xf10a,
	0xf10b,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf200,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf20b,	0xf601,	0xf602,	0xf117,	0xf600,	0xf20a,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short altgr_map[NR_KEYS] = {
	0xf200,	0xf200,	0xf200,	0xf040,	0xf200,	0xf024,	0xf200,	0xf200,
	0xf07b,	0xf05b,	0xf05d,	0xf07d,	0xf05c,	0xf200,	0xf200,	0xf200,
	0xfb71,	0xfb77,	0xf918,	0xfb72,	0xfb74,	0xfb79,	0xfb75,	0xfb69,
	0xfb6f,	0xfb70,	0xf200,	0xf07e,	0xf201,	0xf702,	0xf914,	0xfb73,
	0xf917,	0xf919,	0xfb67,	0xfb68,	0xfb6a,	0xfb6b,	0xfb6c,	0xf200,
	0xf200,	0xf200,	0xf700,	0xf200,	0xfb7a,	0xfb78,	0xf916,	0xfb76,
	0xf915,	0xfb6e,	0xfb6d,	0xf200,	0xf200,	0xf200,	0xf700,	0xf30c,
	0xf703,	0xf200,	0xf207,	0xf50c,	0xf50d,	0xf50e,	0xf50f,	0xf510,
	0xf511,	0xf512,	0xf513,	0xf514,	0xf515,	0xf208,	0xf202,	0xf911,
	0xf912,	0xf913,	0xf30b,	0xf90e,	0xf90f,	0xf910,	0xf30a,	0xf90b,
	0xf90c,	0xf90d,	0xf90a,	0xf310,	0xf206,	0xf200,	0xf07c,	0xf516,
	0xf517,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf200,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf601,	0xf602,	0xf117,	0xf600,	0xf119,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short ctrl_map[NR_KEYS] = {
	0xf200,	0xf200,	0xf200,	0xf000,	0xf01b,	0xf01c,	0xf01d,	0xf01e,
	0xf01f,	0xf07f,	0xf200,	0xf200,	0xf01f,	0xf200,	0xf008,	0xf200,
	0xf011,	0xf017,	0xf005,	0xf012,	0xf014,	0xf019,	0xf015,	0xf009,
	0xf00f,	0xf010,	0xf01b,	0xf01d,	0xf201,	0xf702,	0xf001,	0xf013,
	0xf004,	0xf006,	0xf007,	0xf008,	0xf00a,	0xf00b,	0xf00c,	0xf200,
	0xf007,	0xf000,	0xf700,	0xf01c,	0xf01a,	0xf018,	0xf003,	0xf016,
	0xf002,	0xf00e,	0xf00d,	0xf200,	0xf20e,	0xf07f,	0xf700,	0xf30c,
	0xf703,	0xf000,	0xf207,	0xf100,	0xf101,	0xf102,	0xf103,	0xf104,
	0xf105,	0xf106,	0xf107,	0xf108,	0xf109,	0xf208,	0xf204,	0xf307,
	0xf308,	0xf309,	0xf30b,	0xf304,	0xf305,	0xf306,	0xf30a,	0xf301,
	0xf302,	0xf303,	0xf300,	0xf310,	0xf206,	0xf200,	0xf200,	0xf10a,
	0xf10b,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf01c,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf601,	0xf602,	0xf117,	0xf600,	0xf119,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short shift_ctrl_map[NR_KEYS] = {
	0xf200,	0xf200,	0xf200,	0xf000,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf01f,	0xf200,	0xf200,	0xf200,
	0xf011,	0xf017,	0xf005,	0xf012,	0xf014,	0xf019,	0xf015,	0xf009,
	0xf00f,	0xf010,	0xf200,	0xf200,	0xf201,	0xf702,	0xf001,	0xf013,
	0xf004,	0xf006,	0xf007,	0xf008,	0xf00a,	0xf00b,	0xf00c,	0xf200,
	0xf200,	0xf200,	0xf700,	0xf200,	0xf01a,	0xf018,	0xf003,	0xf016,
	0xf002,	0xf00e,	0xf00d,	0xf200,	0xf200,	0xf200,	0xf700,	0xf30c,
	0xf703,	0xf200,	0xf207,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf208,	0xf200,	0xf307,
	0xf308,	0xf309,	0xf30b,	0xf304,	0xf305,	0xf306,	0xf30a,	0xf301,
	0xf302,	0xf303,	0xf300,	0xf310,	0xf206,	0xf200,	0xf200,	0xf200,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf200,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf601,	0xf602,	0xf117,	0xf600,	0xf119,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short alt_map[NR_KEYS] = {
	0xf200,	0xf81b,	0xf831,	0xf832,	0xf833,	0xf834,	0xf835,	0xf836,
	0xf837,	0xf838,	0xf839,	0xf830,	0xf82d,	0xf83d,	0xf87f,	0xf809,
	0xf871,	0xf877,	0xf865,	0xf872,	0xf874,	0xf879,	0xf875,	0xf869,
	0xf86f,	0xf870,	0xf85b,	0xf85d,	0xf80d,	0xf702,	0xf861,	0xf873,
	0xf864,	0xf866,	0xf867,	0xf868,	0xf86a,	0xf86b,	0xf86c,	0xf83b,
	0xf827,	0xf860,	0xf700,	0xf85c,	0xf87a,	0xf878,	0xf863,	0xf876,
	0xf862,	0xf86e,	0xf86d,	0xf82c,	0xf82e,	0xf82f,	0xf700,	0xf30c,
	0xf703,	0xf820,	0xf207,	0xf500,	0xf501,	0xf502,	0xf503,	0xf504,
	0xf505,	0xf506,	0xf507,	0xf508,	0xf509,	0xf208,	0xf209,	0xf907,
	0xf908,	0xf909,	0xf30b,	0xf904,	0xf905,	0xf906,	0xf30a,	0xf901,
	0xf902,	0xf903,	0xf900,	0xf310,	0xf206,	0xf200,	0xf83c,	0xf50a,
	0xf50b,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf01c,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf210,	0xf211,	0xf117,	0xf600,	0xf119,	0xf115,	0xf116,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

unsigned short ctrl_alt_map[NR_KEYS] = {
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf811,	0xf817,	0xf805,	0xf812,	0xf814,	0xf819,	0xf815,	0xf809,
	0xf80f,	0xf810,	0xf200,	0xf200,	0xf201,	0xf702,	0xf801,	0xf813,
	0xf804,	0xf806,	0xf807,	0xf808,	0xf80a,	0xf80b,	0xf80c,	0xf200,
	0xf200,	0xf200,	0xf700,	0xf200,	0xf81a,	0xf818,	0xf803,	0xf816,
	0xf802,	0xf80e,	0xf80d,	0xf200,	0xf200,	0xf200,	0xf700,	0xf30c,
	0xf703,	0xf200,	0xf207,	0xf500,	0xf501,	0xf502,	0xf503,	0xf504,
	0xf505,	0xf506,	0xf507,	0xf508,	0xf509,	0xf208,	0xf200,	0xf307,
	0xf308,	0xf309,	0xf30b,	0xf304,	0xf305,	0xf306,	0xf30a,	0xf301,
	0xf302,	0xf303,	0xf300,	0xf20c,	0xf206,	0xf200,	0xf200,	0xf50a,
	0xf50b,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
	0xf30e,	0xf702,	0xf30d,	0xf200,	0xf701,	0xf205,	0xf114,	0xf603,
	0xf118,	0xf601,	0xf602,	0xf117,	0xf600,	0xf119,	0xf115,	0xf20c,
	0xf11a,	0xf10c,	0xf10d,	0xf11b,	0xf11c,	0xf110,	0xf311,	0xf11d,
	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,	0xf200,
};

ushort *key_maps[MAX_NR_KEYMAPS] = {
	plain_map, shift_map, altgr_map, 0,
	ctrl_map, shift_ctrl_map, 0, 0,
	alt_map, 0, 0, 0,
	ctrl_alt_map,	0
};

unsigned int keymap_count = 7;

/*
 * Philosophy: most people do not define more strings, but they who do
 * often want quite a lot of string space. So, we statically allocate
 * the default and allocate dynamically in chunks of 512 bytes.
 */

char func_buf[] = {
	'\033', '[', '[', 'A', 0,
	'\033', '[', '[', 'B', 0,
	'\033', '[', '[', 'C', 0,
	'\033', '[', '[', 'D', 0,
	'\033', '[', '[', 'E', 0,
	'\033', '[', '1', '7', '~', 0,
	'\033', '[', '1', '8', '~', 0,
	'\033', '[', '1', '9', '~', 0,
	'\033', '[', '2', '0', '~', 0,
	'\033', '[', '2', '1', '~', 0,
	'\033', '[', '2', '3', '~', 0,
	'\033', '[', '2', '4', '~', 0,
	'\033', '[', '2', '5', '~', 0,
	'\033', '[', '2', '6', '~', 0,
	'\033', '[', '2', '8', '~', 0,
	'\033', '[', '2', '9', '~', 0,
	'\033', '[', '3', '1', '~', 0,
	'\033', '[', '3', '2', '~', 0,
	'\033', '[', '3', '3', '~', 0,
	'\033', '[', '3', '4', '~', 0,
	'\033', '[', '1', '~', 0,
	'\033', '[', '2', '~', 0,
	'\033', '[', '3', '~', 0,
	'\033', '[', '4', '~', 0,
	'\033', '[', '5', '~', 0,
	'\033', '[', '6', '~', 0,
	'\033', '[', 'M', 0,
	'\033', '[', 'P', 0,
};

char *funcbufptr = func_buf;
int funcbufsize = sizeof(func_buf);
int funcbufleft = 0;          /* space left */

char *func_table[MAX_NR_FUNC] = {
	func_buf + 0,
	func_buf + 5,
	func_buf + 10,
	func_buf + 15,
	func_buf + 20,
	func_buf + 25,
	func_buf + 31,
	func_buf + 37,
	func_buf + 43,
	func_buf + 49,
	func_buf + 55,
	func_buf + 61,
	func_buf + 67,
	func_buf + 73,
	func_buf + 79,
	func_buf + 85,
	func_buf + 91,
	func_buf + 97,
	func_buf + 103,
	func_buf + 109,
	func_buf + 115,
	func_buf + 120,
	func_buf + 125,
	func_buf + 130,
	func_buf + 135,
	func_buf + 140,
	func_buf + 145,
	0,
	0,
	func_buf + 149,
	0,
};

struct kbdiacr {
        unsigned char diacr, base, result;
};

struct kbdiacr accent_table[MAX_DIACR] = {
	{'`', 'A', '\300'},	{'`', 'a', '\340'},
	{'\'', 'A', '\301'},	{'\'', 'a', '\341'},
	{'^', 'A', '\302'},	{'^', 'a', '\342'},
	{'~', 'A', '\303'},	{'~', 'a', '\343'},
	{'"', 'A', '\304'},	{'"', 'a', '\344'},
	{'O', 'A', '\305'},	{'o', 'a', '\345'},
	{'0', 'A', '\305'},	{'0', 'a', '\345'},
	{'A', 'A', '\305'},	{'a', 'a', '\345'},
	{'A', 'E', '\306'},	{'a', 'e', '\346'},
	{',', 'C', '\307'},	{',', 'c', '\347'},
	{'`', 'E', '\310'},	{'`', 'e', '\350'},
	{'\'', 'E', '\311'},	{'\'', 'e', '\351'},
	{'^', 'E', '\312'},	{'^', 'e', '\352'},
	{'"', 'E', '\313'},	{'"', 'e', '\353'},
	{'`', 'I', '\314'},	{'`', 'i', '\354'},
	{'\'', 'I', '\315'},	{'\'', 'i', '\355'},
	{'^', 'I', '\316'},	{'^', 'i', '\356'},
	{'"', 'I', '\317'},	{'"', 'i', '\357'},
	{'-', 'D', '\320'},	{'-', 'd', '\360'},
	{'~', 'N', '\321'},	{'~', 'n', '\361'},
	{'`', 'O', '\322'},	{'`', 'o', '\362'},
	{'\'', 'O', '\323'},	{'\'', 'o', '\363'},
	{'^', 'O', '\324'},	{'^', 'o', '\364'},
	{'~', 'O', '\325'},	{'~', 'o', '\365'},
	{'"', 'O', '\326'},	{'"', 'o', '\366'},
	{'/', 'O', '\330'},	{'/', 'o', '\370'},
	{'`', 'U', '\331'},	{'`', 'u', '\371'},
	{'\'', 'U', '\332'},	{'\'', 'u', '\372'},
	{'^', 'U', '\333'},	{'^', 'u', '\373'},
	{'"', 'U', '\334'},	{'"', 'u', '\374'},
	{'\'', 'Y', '\335'},	{'\'', 'y', '\375'},
	{'T', 'H', '\336'},	{'t', 'h', '\376'},
	{'s', 's', '\337'},	{'"', 'y', '\377'},
	{'s', 'z', '\337'},	{'i', 'j', '\377'},
};

unsigned int accent_table_size = 68;




/* These #defines have been copied from drivers/char/pc_keyb.h, by
 * Martin Mares (mj@ucw.cz).
 * converted to offsets by Till Straumann <strauman@slac.stanford.edu>
 */
#define KBD_STATUS_REG		0x4	/* Status register (R) */
#define KBD_CNTL_REG		0x4	/* Controller command register (W) */
#define KBD_DATA_REG		0x0	/* Keyboard data register (R/W) */

/*
 *	Keyboard Controller Commands
 */

#define KBD_CCMD_WRITE_MODE	0x60	/* Write mode bits */
#define KBD_CCMD_GET_VERSION	0xA1	/* Get controller version */
#define KBD_CCMD_MOUSE_DISABLE	0xA7	/* Disable mouse interface */
#define KBD_CCMD_MOUSE_ENABLE	0xA8	/* Enable mouse interface */
#define KBD_CCMD_TEST_MOUSE	0xA9	/* Mouse interface test */
#define KBD_CCMD_SELF_TEST	0xAA	/* Controller self test */
#define KBD_CCMD_KBD_TEST	0xAB	/* Keyboard interface test */
#define KBD_CCMD_KBD_DISABLE	0xAD	/* Keyboard interface disable */
#define KBD_CCMD_KBD_ENABLE	0xAE	/* Keyboard interface enable */

/*
 *	Keyboard Commands
 */

#define KBD_CMD_ENABLE		0xF4	/* Enable scanning */
#define KBD_CMD_DISABLE		0xF5	/* Disable scanning */
#define KBD_CMD_RESET		0xFF	/* Reset */

/*
 *	Keyboard Replies
 */

#define KBD_REPLY_POR		0xAA	/* Power on reset */
#define KBD_REPLY_ACK		0xFA	/* Command ACK */
#define KBD_REPLY_RESEND	0xFE	/* Command NACK, send the cmd again */

/*
 *	Status Register Bits
 */

#define KBD_STAT_OBF 		0x01	/* Keyboard output buffer full */
#define KBD_STAT_IBF 		0x02	/* Keyboard input buffer full */
#define KBD_STAT_UNLOCKED	0x10	/* Zero if keyboard locked */
#define KBD_STAT_GTO 		0x40	/* General receive/xmit timeout */
#define KBD_STAT_PERR 		0x80	/* Parity error */

/*
 *	Controller Mode Register Bits
 */

#define KBD_MODE_KBD_INT	0x01	/* Keyboard data generate IRQ1 */
#define KBD_MODE_SYS 		0x04	/* The system flag (?) */
#define KBD_MODE_NO_KEYLOCK	0x08	/* The keylock doesn't affect the keyboard if set */
#define KBD_MODE_DISABLE_KBD	0x10	/* Disable keyboard interface */
#define KBD_MODE_DISABLE_MOUSE	0x20	/* Disable mouse interface */
#define KBD_MODE_KCC 		0x40	/* Scan code conversion to PC format */
#define KBD_MODE_RFU		0x80

#endif /* USE_KBD_SUPPORT */


/* Early messages after mm init but before console init are kept in log
 * buffers.
 */
#define PAGE_LOG_CHARS (PAGE_SIZE-sizeof(int)-sizeof(u_long)-1)

typedef struct _console_log {
	struct _console_log *next;
	int offset;
	u_char data[PAGE_LOG_CHARS];
} console_log;

#ifdef STATIC_LOG_ALLOC

#define STATIC_LOG_DATA_PAGE_NB	3

static  u_char 	log_page_pool	[STATIC_LOG_DATA_PAGE_NB * PAGE_SIZE];

#endif
#endif

static board_memory_map mem_map = {
  (__io_ptr) _IO_BASE,		/* from libcpu/io.h */
  (__io_ptr) _ISA_MEM_BASE,
};

board_memory_map *ptr_mem_map = &mem_map;

#if 0
struct _console_global_data {
	console_log *log;
	int vacuum_sent;
	int lines;
	int cols;
	int orig_x;
	int orig_y;
	u_char shfts, ctls, alts, caps;
} console_global_data = {NULL, 0, 25, 80, 0, 24, 0, 0, 0, 0};

typedef struct console_io {
	void 	(*putc)	(const u_char);
	int 	(*getc)	(void);
	int 	(*tstc)	(void);
}console_io;

extern console_io* curIo;

void debug_putc(const u_char c)
{
  curIo->putc(c);
}

/* const char arg to be compatible with BSP_output_char decl. */
void
debug_putc_onlcr(const char c)
{
	if ('\n'==c)
		debug_putc('\r');
	debug_putc(c);
}

int debug_getc(void)
{
  return curIo->getc();
}

int debug_tstc(void)
{
  return curIo->tstc();
}

#define vidmem ((__io_ptr)(ptr_mem_map->isa_mem_base+0xb8000))

void vacuum_putc(u_char c) {
	console_global_data.vacuum_sent++;
}

int vacuum_getc(void) {
	return -1;
}

int vacuum_tstc(void) {
	return 0;
}

/*
 * COM1 NS16550 support
 */

#define rbr 0
#define ier 1
#define fcr 2
#define lcr 3
#define mcr 4
#define lsr 5
#define msr 6
#define scr 7
#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

#define LSR_DR   0x01  /* Data ready */
#define LSR_OE   0x02  /* Overrun */
#define LSR_PE   0x04  /* Parity error */
#define LSR_FE   0x08  /* Framing error */
#define LSR_BI   0x10  /* Break */
#define LSR_THRE 0x20  /* Xmit holding register empty */
#define LSR_TEMT 0x40  /* Xmitter empty */
#define LSR_ERR  0x80  /* Error */


#ifdef STATIC_LOG_ALLOC
static int global_index = 0;

static void *__palloc(int s)
{
  if (global_index ==( STATIC_LOG_DATA_PAGE_NB - 1) ) return (void*) 0;
  return  (void*) &(log_page_pool [PAGE_SIZE * global_index++]);
}

static void pfree(void* p)
{
  --global_index;
}
#endif


void log_putc(const u_char c) {
	console_log *l;
	for(l=console_global_data.log; l; l=l->next) {
		if (l->offset<PAGE_LOG_CHARS) break;
	}
	if (!l) {
		l=__palloc(sizeof(console_log));
		memset(l, 0, sizeof(console_log));
		if (!console_global_data.log)
			console_global_data.log = l;
		else {
			console_log *p;
			for (p=console_global_data.log;
			     p->next; p=p->next);
			p->next = l;
		}
	}
	l->data[l->offset++] = c;
}

/* This puts is non standard since it does not automatically add a newline
 * at the end. So it is made private to avoid confusion in other files.
 */
static
void puts(const u_char *s)
{
        char c;

        while ( ( c = *s++ ) != '\0' ) {
				debug_putc_onlcr((const char)c);
        }
}


static
void flush_log(void) {
	console_log *p, *next;
	if (console_global_data.vacuum_sent) {
#ifdef TRACE_FLUSH_LOG
		printk("%d characters sent into oblivion before MM init!\n",
		       console_global_data.vacuum_sent);
#endif
	}
	for(p=console_global_data.log; p; p=next) {
		puts(p->data);
		next = p->next;
		pfree(p);
	}
}

#ifndef INL_CONSOLE_INB
#error "BSP probably didn't define a console port"
#endif

void serial_putc(const u_char c)
{
	while ((INL_CONSOLE_INB(lsr) & LSR_THRE) == 0) ;
	INL_CONSOLE_OUTB(thr, c);
}

int serial_getc(void)
{
	while ((INL_CONSOLE_INB(lsr) & LSR_DR) == 0) ;
	return (INL_CONSOLE_INB(rbr));
}

int serial_tstc(void)
{
	return ((INL_CONSOLE_INB(lsr) & LSR_DR) != 0);
}

#ifdef USE_VGA_SUPPORT
static void scroll(void)
{
	int i;

	memcpy ( (u_char *)vidmem, (u_char *)vidmem + console_global_data.cols * 2,
		 ( console_global_data.lines - 1 ) * console_global_data.cols * 2 );
	for ( i = ( console_global_data.lines - 1 ) * console_global_data.cols * 2;
	      i < console_global_data.lines * console_global_data.cols * 2;
	      i += 2 )
		vidmem[i] = ' ';
}

/*
 * cursor() sets an offset (0-1999) into the 80x25 text area
 */
static void
cursor(int x, int y)
{
	int pos = console_global_data.cols*y + x;
	vga_outb(14, 0x14);
	vga_outb(0x15, pos>>8);
	vga_outb(0x14, 15);
	vga_outb(0x15, pos);
}

void
vga_putc(const u_char c)
{
	int x,y;

	x = console_global_data.orig_x;
	y = console_global_data.orig_y;

	if ( c == '\n' ) {
		if ( ++y >= console_global_data.lines ) {
			scroll();
			y--;
		}
	} else if (c == '\b') {
		if (x > 0) {
			x--;
		}
	} else if (c == '\r') {
		x = 0;
	} else {
		vidmem [ ( x + console_global_data.cols * y ) * 2 ] = c;
		if ( ++x >= console_global_data.cols ) {
			x = 0;
			if ( ++y >= console_global_data.lines ) {
				scroll();
				y--;
			}
		}
	}

	cursor(x, y);

	console_global_data.orig_x = x;
	console_global_data.orig_y = y;
}
#endif /* USE_VGA_SUPPORT */

#ifdef USE_KBD_SUPPORT
/* Keyboard support */
static int kbd_getc(void)
{
	unsigned char dt, brk, val;
	unsigned code;
loop:
	while((kbd_inb(KBD_STATUS_REG) & KBD_STAT_OBF) == 0) ;

	dt = kbd_inb(KBD_DATA_REG);

	brk = dt & 0x80;	/* brk == 1 on key release */
	dt = dt & 0x7f;		/* keycode */

	if (console_global_data.shfts)
	    code = shift_map[dt];
	else if (console_global_data.ctls)
	    code = ctrl_map[dt];
	else
	    code = plain_map[dt];

	val = KVAL(code);
	switch (KTYP(code) & 0x0f) {
	    case KT_LATIN:
		if (brk)
		    break;
		if (console_global_data.alts)
		    val |= 0x80;
		if (val == 0x7f)	/* map delete to backspace */
		    val = '\b';
		return val;

	    case KT_LETTER:
		if (brk)
		    break;
		if (console_global_data.caps)
		    val -= 'a'-'A';
		return val;

	    case KT_SPEC:
		if (brk)
		    break;
		if (val == KVAL(K_CAPS))
		    console_global_data.caps = !console_global_data.caps;
		else if (val == KVAL(K_ENTER)) {
enter:		    /* Wait for key up */
		    while (1) {
			while((kbd_inb(KBD_STATUS_REG) & KBD_STAT_OBF) == 0) ;
			dt = kbd_inb(KBD_DATA_REG);
			if (dt & 0x80) /* key up */ break;
		    }
		    return 10;
		}
		break;

	    case KT_PAD:
		if (brk)
		    break;
		if (val < 10)
		    return val;
		if (val == KVAL(K_PENTER))
		    goto enter;
		break;

	    case KT_SHIFT:
		switch (val) {
		    case KG_SHIFT:
		    case KG_SHIFTL:
		    case KG_SHIFTR:
			console_global_data.shfts = brk ? 0 : 1;
			break;
		    case KG_ALT:
		    case KG_ALTGR:
			console_global_data.alts = brk ? 0 : 1;
			break;
		    case KG_CTRL:
		    case KG_CTRLL:
		    case KG_CTRLR:
			console_global_data.ctls = brk ? 0 : 1;
			break;
		}
		break;

	    case KT_LOCK:
		switch (val) {
		    case KG_SHIFT:
		    case KG_SHIFTL:
		    case KG_SHIFTR:
			if (brk)
			    console_global_data.shfts = !console_global_data.shfts;
			break;
		    case KG_ALT:
		    case KG_ALTGR:
			if (brk)
			    console_global_data.alts = !console_global_data.alts;
			break;
		    case KG_CTRL:
		    case KG_CTRLL:
		    case KG_CTRLR:
			if (brk)
			    console_global_data.ctls = !console_global_data.ctls;
			break;
		}
		break;
	}
	/* if (brk) return (0); */  /* Ignore initial 'key up' codes */
	goto loop;
}

static int kbd_get(int ms) {
	int status, data;
	while(1) {
		status = kbd_inb(KBD_STATUS_REG);
		if (status & KBD_STAT_OBF) {
			data = kbd_inb(KBD_DATA_REG);
			if (status & (KBD_STAT_GTO | KBD_STAT_PERR))
				return -1;
			else
				return data;
		}
		if (--ms < 0) return -1;
#ifdef __BOOT__
		boot_udelay(1000);
#else
		rtems_bsp_delay(1000);
#endif
	}
}

static void kbd_put(u_char c, int ms, int port) {
  	while (kbd_inb(KBD_STATUS_REG) & KBD_STAT_IBF) {
		if (--ms < 0) return;
#ifdef __BOOT__
		boot_udelay(1000);
#else
		rtems_bsp_delay(1000);
#endif
	}
	kbd_outb(port, c);
}

int kbdreset(void)
{
	int c;

	/* Flush all pending data */
	while(kbd_get(10) != -1);

	/* Send self-test */
	kbd_put(KBD_CCMD_SELF_TEST, 10, KBD_CNTL_REG);
	c = kbd_get(1000);
	if (c != 0x55) return 1;

	/* Enable then reset the KB */
	kbd_put(KBD_CCMD_KBD_ENABLE, 10, KBD_CNTL_REG);

	while (1) {
		kbd_put(KBD_CMD_RESET, 10, KBD_DATA_REG);
		c = kbd_get(1000);
		if (c == KBD_REPLY_ACK) break;
		if (c != KBD_REPLY_RESEND) return 2;
	}

	if (kbd_get(1000) != KBD_REPLY_POR) return 3;

	/* Disable the keyboard while setting up the controller */
	kbd_put(KBD_CMD_DISABLE, 10, KBD_DATA_REG);
	if (kbd_get(10)!=KBD_REPLY_ACK) return 4;

	/* Enable interrupts and keyboard controller */
	kbd_put(KBD_CCMD_WRITE_MODE, 10, KBD_CNTL_REG);
	kbd_put(KBD_MODE_KBD_INT | KBD_MODE_SYS |
		KBD_MODE_DISABLE_MOUSE | KBD_MODE_KCC,
		10, KBD_DATA_REG);

	/* Reenable the keyboard */
	kbd_put(KBD_CMD_ENABLE, 10, KBD_DATA_REG);
	if (kbd_get(10)!=KBD_REPLY_ACK) return 5;

	return 0;
}

int kbd_tstc(void)
{
	return ((kbd_inb(KBD_STATUS_REG) & KBD_STAT_OBF) != 0);
}
#endif /* USE_KBD_SUPPORT */

const struct console_io
vacuum_console_functions = {
  	vacuum_putc,
	vacuum_getc,
	vacuum_tstc
};

static const struct console_io
log_console_functions = {
  	log_putc,
	vacuum_getc,
	vacuum_tstc
}
,
serial_console_functions = {
	serial_putc,
	serial_getc,
	serial_tstc
}
#if defined(USE_KBD_SUPPORT) && defined(USE_VGA_SUPPORT)
,
vga_console_functions = {
	vga_putc,
	kbd_getc,
	kbd_tstc
}
#endif
;

console_io* curIo = (console_io*) &vacuum_console_functions;

int select_console(ioType t) {
  static ioType curType = CONSOLE_VACUUM;

  switch (t) {
  case CONSOLE_VACUUM 	: curIo = (console_io*)&vacuum_console_functions; break;
  case CONSOLE_LOG 	: curIo = (console_io*)&log_console_functions; break;
  case CONSOLE_SERIAL 	: curIo = (console_io*)&serial_console_functions; break;
#if defined(USE_KBD_SUPPORT) && defined(USE_VGA_SUPPORT)
  case CONSOLE_VGA 	: curIo = (console_io*)&vga_console_functions; break;
#endif
  default	        : curIo = (console_io*)&vacuum_console_functions;break;
  }
  if (curType == CONSOLE_LOG) flush_log();
  curType = t;
  return 0;
}

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')


/* provide this for the bootloader only; otherwise
 * use libcpu implementation
 */
#if defined(__BOOT__)
static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

/* Based on linux/lib/vsprintf.c and modified to suit our needs,
 * bloat has been limited since we basically only need %u, %x, %s and %c.
 * But we need 64 bit values !
 */
int k_vsprintf(char *buf, const char *fmt, va_list args);

int printk(const char *fmt, ...) {
	va_list args;
	int i;
	/* Should not be a problem with 8kB of stack */
	char buf[1024];

	va_start(args, fmt);
	i = k_vsprintf(buf, fmt, args);
	va_end(args);
	puts(buf);
	return  i;
}

#endif

/* Necessary to avoid including a library, and GCC won't do this inline. */
#define div10(num, rmd)							 \
do {	u32 t1, t2, t3;							 \
	__asm__ ("lis %4,0xcccd; "						 \
	    "addi %4,%4,0xffffcccd; "	/* Build 0xcccccccd */		 \
	    "mulhwu %3,%0+1,%4; "	/* (num.l*cst.l).h  */		 \
	    "mullw %2,%0,%4; "		/* (num.h*cst.l).l  */		 \
	    "addc %3,%3,%2; "						 \
	    "mulhwu %2,%0,%4; "		/* (num.h*cst.l).h  */		 \
	    "addi %4,%4,-1; " 		/* Build 0xcccccccc */		 \
	    "mullw %1,%0,%4; "		/* (num.h*cst.h).l  */		 \
	    "adde %2,%2,%1; "						 \
	    "mulhwu %1,%0,%4; "		/* (num.h*cst.h).h  */		 \
	    "addze %1,%1; "						 \
	    "mullw %0,%0+1,%4; "	/* (num.l*cst.h).l  */		 \
	    "addc %3,%3,%0; "						 \
	    "mulhwu %0,%0+1,%4; "	/* (num.l*cst.h).h  */		 \
	    "adde %2,%2,%0; "						 \
	    "addze %1,%1; "						 \
	    "srwi %2,%2,3; "						 \
	    "srwi %0,%1,3; "						 \
	    "rlwimi %2,%1,29,0,2; "					 \
	    "mulli %4,%2,10; "						 \
	    "sub %4,%0+1,%4; "						 \
	    "mr %0+1,%2; " :						 \
	    "=r" (num), "=&r" (t1), "=&r" (t2), "=&r"(t3), "=&b" (rmd) : \
	    "0" (num));							 \
									 \
} while(0);

#define SIGN	1		/* unsigned/signed long */
#define LARGE	2		/* use 'ABCDEF' instead of 'abcdef' */
#define HEX	4		/* hexadecimal instead of decimal */
#define ADDR	8               /* Value is an addres (p) */
#define ZEROPAD	16		/* pad with zero */
#define HALF    32
#define LONG	64		/* long argument */
#define LLONG	128		/* 64 bit argument */

#if defined(__BOOT__)
static char * number(char * str, int size, int type, u64 num)
{
	char fill,sign,tmp[24];
	const char *digits="0123456789abcdef";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEF";
	fill = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if ((s64)num <0) {
			sign = '-';
			num = -num;
			size--;
		}
	}

	i = 0;
	do {
		unsigned rem;
		if (type&HEX) {
			rem = num & 0x0f;
			num >>=4;
		} else {
			div10(num, rem);
		}
		tmp[i++] = digits[rem];
	} while (num != 0);

	size -= i;
	if (!(type&(ZEROPAD)))
		while(size-->0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;

	while (size-- > 0)
		*str++ = fill;
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}

int k_vsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	u64 num;
	int i;
	char * str;
	const char *s;

	int flags;		/* flags to number() and private */

	int field_width;	/* width of output field */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags, only 0 padding needed */
		flags = 0;
		if (*++fmt == '0' ) {
			flags |= ZEROPAD;
			fmt++;
		}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);

		/* get the conversion qualifier */
		if (*fmt == 'h') {
			flags |= HALF;
			fmt++;
		} else if (*fmt == 'L') {
			flags |= LLONG;
			fmt++;
		} else if (*fmt == 'l') {
			flags |= LONG;
			fmt++;
		}

		switch (*fmt) {
		case 'c':
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			len = strlen(s);

			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
			}
			flags |= ZEROPAD|HEX|ADDR;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			flags |= HEX;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		/* This ugly code tries to minimize the number of va_arg()
		 * since they expand to a lot of code on PPC under the SYSV
		 * calling conventions (but not with -mcall-aix which has
		 * other problems). Arguments have at least the size of a
		 * long allocated, and we use this fact to minimize bloat.
		 * (and pointers are assimilated to unsigned long too).
		 */
		if (sizeof(long long) > sizeof(long) && flags & LLONG)
			num = va_arg(args, unsigned long long);
		else {
			u_long n = va_arg(args, unsigned long);
			if (flags & HALF) {
			  	if (flags & SIGN)
					n = (short) n;
				else
				  	n = (unsigned short) n;
			} else if (! flags & LONG) {
				/* Here the compiler correctly removes this
				 * do nothing code on 32 bit PPC.
				 */
				if (flags & SIGN)
					n = (int) n;
				else
					n = (unsigned) n;
			}
			if (flags & SIGN)  num = (long) n; else num = n;
		}
		str = number(str, field_width, flags, num);
	}
	*str = '\0';
	return str-buf;
}
#endif
#endif
