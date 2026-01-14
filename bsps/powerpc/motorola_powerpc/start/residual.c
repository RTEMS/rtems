/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Function used to parse residual data
 *
 * This code is heavily inspired by the public specification of STREAM V2
 * that can be found at:
 *
 *  - <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 */

/*
 * Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
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

#include <string.h>

#include <bsp/residual.h>
#include <libcpu/io.h>
#include <libcpu/byteorder.h>

static int same_DevID(unsigned short vendor,
	       unsigned short Number,
	       unsigned char * str)
{
	static unsigned const char hexdigit[]="0123456789ABCDEF";
	if (strlen((char*)str)!=7) return 0;
	if ( ( ((vendor>>10)&0x1f)+'A'-1 == str[0])  &&
	     ( ((vendor>>5)&0x1f)+'A'-1 == str[1])   &&
	     ( (vendor&0x1f)+'A'-1 == str[2])        &&
	     (hexdigit[(Number>>12)&0x0f] == str[3]) &&
	     (hexdigit[(Number>>8)&0x0f] == str[4])  &&
	     (hexdigit[(Number>>4)&0x0f] == str[5])  &&
	     (hexdigit[Number&0x0f] == str[6]) ) return 1;
	return 0;
}

PPC_DEVICE *residual_find_device(RESIDUAL *res,unsigned long BusMask,
				 unsigned char * DevID,
				 int BaseType,
				 int SubType,
				 int Interface,
				 int n)
{
	int i;
	if ( !res || !res->ResidualLength ) return NULL;
	for (i=0; i<res->ActualNumDevices; i++) {
#define Dev res->Devices[i].DeviceId
		if ( (Dev.BusId&BusMask)                                  &&
		     (BaseType==-1 || Dev.BaseType==BaseType)             &&
		     (SubType==-1 || Dev.SubType==SubType)                &&
		     (Interface==-1 || Dev.Interface==Interface)          &&
		     (DevID==NULL || same_DevID((Dev.DevId>>16)&0xffff,
						Dev.DevId&0xffff, DevID)) &&
		     !(n--) ) return res->Devices+i;
#undef Dev
	}
	return 0;
}

PnP_TAG_PACKET *PnP_find_packet(unsigned char *p,
				unsigned packet_tag,
				int n)
{
	unsigned mask, masked_tag, size;
	if(!p) return 0;
	if (tag_type(packet_tag)) mask=0xff; else mask=0xF8;
	masked_tag = packet_tag&mask;
	for(; *p != END_TAG; p+=size) {
		if ((*p & mask) == masked_tag && !(n--))
			return (PnP_TAG_PACKET *) p;
		if (tag_type(*p))
			size=ld_le16((unsigned short *)(p+1))+3;
		else
			size=tag_small_count(*p)+1;
	}
	return 0; /* not found */
}

PnP_TAG_PACKET *PnP_find_small_vendor_packet(unsigned char *p,
					     unsigned packet_type,
					     int n)
{
	int next=0;
	while (p) {
		p = (unsigned char *) PnP_find_packet(p, 0x70, next);
		if (p && p[1]==packet_type && !(n--))
			return (PnP_TAG_PACKET *) p;
		next = 1;
	};
	return 0; /* not found */
}

PnP_TAG_PACKET *PnP_find_large_vendor_packet(unsigned char *p,
					   unsigned packet_type,
					   int n)
{
	int next=0;
	while (p) {
		p = (unsigned char *) PnP_find_packet(p, 0x84, next);
		if (p && p[3]==packet_type && !(n--))
			return (PnP_TAG_PACKET *) p;
		next = 1;
	};
	return 0; /* not found */
}
