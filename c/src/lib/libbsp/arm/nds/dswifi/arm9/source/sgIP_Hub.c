// DSWifi Project - sgIP Internet Protocol Stack Implementation
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
#include "sgIP_Hub.h"
#include "sgIP_ARP.h"

//////////////////////////////////////////////////////////////////////////
// Global vars

int NumHWInterfaces;
int NumProtocolInterfaces;
sgIP_Hub_Protocol ProtocolInterfaces[SGIP_HUB_MAXPROTOCOLINTERFACES];
sgIP_Hub_HWInterface HWInterfaces[SGIP_HUB_MAXHWINTERFACES];



//////////////////////////////////////////////////////////////////////////
// Private functions






//////////////////////////////////////////////////////////////////////////
// Public functions


void sgIP_Hub_Init() {
	NumHWInterfaces=0;
	NumProtocolInterfaces=0;

}

sgIP_Hub_Protocol * sgIP_Hub_AddProtocolInterface(int protocolID, int (*ReceivePacket)(sgIP_memblock *), int (*InterfaceInit)(sgIP_Hub_Protocol *)) {
	int n;
	if(NumProtocolInterfaces>=SGIP_HUB_MAXPROTOCOLINTERFACES) return 0;
	for(n=0;n<SGIP_HUB_MAXPROTOCOLINTERFACES;n++) {
		if(!(ProtocolInterfaces[n].flags&SGIP_FLAG_PROTOCOL_IN_USE)) break;
	}
	if(n==SGIP_HUB_MAXPROTOCOLINTERFACES) return 0;

	ProtocolInterfaces[n].flags=SGIP_FLAG_PROTOCOL_IN_USE | SGIP_FLAG_PROTOCOL_ENABLED;
	ProtocolInterfaces[n].protocol=protocolID;
	ProtocolInterfaces[n].ReceivePacket=ReceivePacket;
	if(InterfaceInit) InterfaceInit(ProtocolInterfaces+n);
	NumProtocolInterfaces++;
	return ProtocolInterfaces+n;
}


sgIP_Hub_HWInterface * sgIP_Hub_AddHardwareInterface(int (*TransmitFunction)(sgIP_Hub_HWInterface *, sgIP_memblock *), int (*InterfaceInit)(sgIP_Hub_HWInterface *)) {
	int n;
	if(NumHWInterfaces>=SGIP_HUB_MAXHWINTERFACES) return 0;
	for(n=0;n<SGIP_HUB_MAXHWINTERFACES;n++) {
		if(!(HWInterfaces[n].flags&SGIP_FLAG_HWINTERFACE_IN_USE)) break;
	}
	if(n==SGIP_HUB_MAXHWINTERFACES) return 0;

	HWInterfaces[n].flags = SGIP_FLAG_HWINTERFACE_IN_USE | SGIP_FLAG_HWINTERFACE_ENABLED;
	HWInterfaces[n].TransmitFunction=TransmitFunction;
	if(InterfaceInit) InterfaceInit(HWInterfaces+n);
	NumHWInterfaces++;
	return HWInterfaces+n;
}

extern void sgIP_Hub_RemoveProtocolInterface(sgIP_Hub_Protocol * protocol) {
	int n;
	for(n=0;n<SGIP_HUB_MAXPROTOCOLINTERFACES;n++) {
		if(ProtocolInterfaces+n ==protocol) break;
	}
	if(n==SGIP_HUB_MAXPROTOCOLINTERFACES) return;
	protocol->flags=0;
	NumProtocolInterfaces--;
}
extern void sgIP_Hub_RemoveHardwareInterface(sgIP_Hub_HWInterface * hw) {
	int n;
	for(n=0;n<SGIP_HUB_MAXHWINTERFACES;n++) {
		if(HWInterfaces+n == hw) break;
	}
	if(n==SGIP_HUB_MAXHWINTERFACES) return;
	hw->flags=0;
	NumHWInterfaces--;
}

int sgIP_Hub_ReceiveHardwarePacket(sgIP_Hub_HWInterface * hw, sgIP_memblock * packet) {
	if(!hw || !packet) return 0;
	if(hw->flags & SGIP_FLAG_HWINTERFACE_ENABLED) {
		int n;
		int protocol;

		protocol = ((unsigned short *)packet->datastart)[6];
//		SGIP_DEBUG_MESSAGE(("hub: rx packet %04X %X",protocol,packet->totallength));
		if(protocol==PROTOCOL_ETHER_ARP) { // arp
			sgIP_ARP_ProcessARPFrame(hw,packet);
			return 0;
		}
		if(protocol==PROTOCOL_ETHER_IP) { // IP, forward to the ARP system.

		}

		// hide ethernet header for higher-level protocols
		sgIP_memblock_exposeheader(packet,-14);
		for(n=0;n<SGIP_HUB_MAXPROTOCOLINTERFACES;n++) {
			if(ProtocolInterfaces[n].flags&SGIP_FLAG_PROTOCOL_ENABLED && ProtocolInterfaces[n].protocol==protocol) { // this protocol handler
				return ProtocolInterfaces[n].ReceivePacket(packet);
			}
		}
	}
	// hrmm, packet is unhandled.  Ignore it for now.
	sgIP_memblock_free(packet);
	return 0;
}
// send packet from a protocol interface, resolve the requisite hardware interface addresses and send it.
int sgIP_Hub_SendProtocolPacket(int protocol, sgIP_memblock * packet, unsigned long dest_address, unsigned long src_address) {
	if(!packet) return 0;
	sgIP_Hub_HWInterface * hw;
	int i;
	hw=0;
	// figure out what hardware interface is in use.
	for(i=0;i<SGIP_HUB_MAXHWINTERFACES;i++) if(HWInterfaces[i].ipaddr==src_address) {hw=HWInterfaces+i; break;}
	if(!hw) {
		sgIP_memblock_free(packet);
		return 0;
	}
	// resolve protocol address to hardware address & send packet
	if( (src_address & hw->snmask) == (dest_address & hw->snmask)	// on same network
	 	|| dest_address == 0xFFFFFFFF )				// or broadcast address, send directly.
	{
		return sgIP_ARP_SendProtocolFrame(hw,packet,protocol,dest_address);
	} else { // eek, on different network. Send to gateway
		return sgIP_ARP_SendProtocolFrame(hw,packet,protocol,hw->gateway);
	}
}
// send packet on a hardware interface.
int sgIP_Hub_SendRawPacket(sgIP_Hub_HWInterface * hw, sgIP_memblock * packet) {
	if(!hw || !packet) return 0;
	if(hw->flags&SGIP_FLAG_HWINTERFACE_ENABLED) {
		return hw->TransmitFunction(hw,packet);
	}
	sgIP_memblock_free(packet);
	return 0;
}

int sgIP_Hub_IPMaxMessageSize(unsigned long ipaddr) {
	return SGIP_MTU_OVERRIDE; // hack - make this more accurate soon!
}

unsigned long sgIP_Hub_GetCompatibleIP(unsigned long destIP) {
	int n;
	for(n=0;n<SGIP_HUB_MAXHWINTERFACES;n++) {
		if((HWInterfaces[n].flags&SGIP_FLAG_HWINTERFACE_IN_USE)) {
			if((HWInterfaces[n].ipaddr & HWInterfaces[n].snmask) == (destIP & HWInterfaces[n].snmask)) return HWInterfaces[n].ipaddr;
		}
	}
	for(n=0;n<SGIP_HUB_MAXHWINTERFACES;n++) {
		if((HWInterfaces[n].flags&SGIP_FLAG_HWINTERFACE_IN_USE)) {
			return HWInterfaces[n].ipaddr;
		}
	}
	return 0;
}

extern sgIP_Hub_HWInterface * sgIP_Hub_GetDefaultInterface() {
   int n;
   for(n=0;n<SGIP_HUB_MAXHWINTERFACES;n++) {
      if((HWInterfaces[n].flags&SGIP_FLAG_HWINTERFACE_IN_USE)) {
         return HWInterfaces+n;
      }
   }
   return 0;
}


#ifdef SGIP_LITTLEENDIAN
unsigned short htons(unsigned short num) {
	return ((num<<8)&0xFF00) | (num>>8);
}
unsigned long htonl(unsigned long num) {
	return (num<<24) | ((num&0xFF00)<<8) | ((num&0xFF0000)>>8) | (num>>24);
}
#else
unsigned short htons(unsigned short num) {
	return num;
}
unsigned long htonl(unsigned long num) {
	return num;
}
#endif

