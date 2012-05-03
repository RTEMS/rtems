/*
 * residual.c : function used to parse residual data.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
