// DSWifi Project - socket emulation layer defines/prototypes (netinet/in.h)
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
/****************************************************************************** 
DSWifi Lib and test materials are licenced under the MIT open source licence:
Copyright (c) 2005-2006 Stephen Stair

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef NETINET_IN_H
#define NETINET_IN_H

#include "sys/socket.h"

#define INADDR_ANY			0x00000000
#define INADDR_BROADCAST	0xFFFFFFFF
#define INADDR_NONE			0xFFFFFFFF


struct in_addr {
	unsigned long s_addr;
};

struct sockaddr_in {
	unsigned short		sin_family;
	unsigned short		sin_port;
	struct in_addr		sin_addr;
	unsigned char		sin_zero[8];
};

#ifdef __cplusplus
extern "C" {
#endif

	// actually from arpa/inet.h - but is included through netinet/in.h
	unsigned long inet_addr(const char *cp);
	int inet_aton(const char *cp, struct in_addr *inp);
	char *inet_ntoa(struct in_addr in);

#ifdef __cplusplus
};
#endif


#endif
