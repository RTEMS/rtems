/* SPDX-License-Identifier: ISC */

/**
 * @file
 *
 * @ingroup RTEMSImplBase64
 *
 * @brief This source file contains the implementation of
 *   _Base64_Encode() and _Base64url_Encode().
 */

/*
 * Copyright (C) 2020, 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2004, 2005, 2007, 2009  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001, 2003  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <rtems/base64.h>

static void
_Base64_Put(int c, void *arg, IO_Put_char put_char)
{
	(*put_char)(c, arg);
}

static int
_Base64_Do_encode(IO_Put_char put_char, void *arg, const void *src,
    size_t srclen, const char *wordbreak, int wordlen, const uint8_t *encoding)
{
	unsigned int loops = 0;
	const unsigned char *in = src;
	int out = 0;

	if (wordlen < 4) {
		wordlen = 4;
	}

	while (srclen > 2) {
		_Base64_Put(encoding[(in[0]>>2)&0x3f], arg, put_char);
		_Base64_Put(encoding[((in[0]<<4)&0x30)|
				((in[1]>>4)&0x0f)], arg, put_char);
		_Base64_Put(encoding[((in[1]<<2)&0x3c)|
				((in[2]>>6)&0x03)], arg, put_char);
		_Base64_Put(encoding[in[2]&0x3f], arg, put_char);
		in += 3;
		srclen -= 3;
		out += 4;

		loops++;
		if (srclen != 0 &&
		    (int)((loops + 1) * 4) >= wordlen)
		{
			const char *w = wordbreak;
			loops = 0;
			while (*w != '\0') {
				_Base64_Put(*w, arg, put_char);
				++w;
				++out;
			}
		}
	}
	if (srclen == 2) {
		_Base64_Put(encoding[(in[0]>>2)&0x3f], arg, put_char);
		_Base64_Put(encoding[((in[0]<<4)&0x30)|
				((in[1]>>4)&0x0f)], arg, put_char);
		_Base64_Put(encoding[((in[1]<<2)&0x3c)], arg, put_char);
		_Base64_Put('=', arg, put_char);
		out += 4;
	} else if (srclen == 1) {
		_Base64_Put(encoding[(in[0]>>2)&0x3f], arg, put_char);
		_Base64_Put(encoding[((in[0]<<4)&0x30)], arg, put_char);
		_Base64_Put('=', arg, put_char);
		_Base64_Put('=', arg, put_char);
		out += 4;
	}
	return out;
}

const uint8_t _Base64_Encoding[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

int
_Base64_Encode(IO_Put_char put_char, void *arg, const void *src, size_t srclen,
    const char *wordbreak, int wordlen)
{
	return _Base64_Do_encode(put_char, arg, src, srclen, wordbreak,
	    wordlen, _Base64_Encoding);
}

const uint8_t _Base64url_Encoding[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_'
};

int
_Base64url_Encode(IO_Put_char put_char, void *arg, const void *src,
    size_t srclen, const char *wordbreak, int wordlen)
{
	return _Base64_Do_encode(put_char, arg, src, srclen, wordbreak,
	    wordlen, _Base64url_Encoding);
}
