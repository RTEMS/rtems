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

#include "sgIP_ARP.h"

sgIP_ARP_Record ArpRecords[SGIP_ARP_MAXENTRIES];


int sgIP_FindArpSlot(sgIP_Hub_HWInterface * hw, unsigned long destip) {
	int i;
	for(i=0;i<SGIP_ARP_MAXENTRIES;i++) {
		if(ArpRecords[i].flags&SGIP_ARP_FLAG_ACTIVE) {
			if(ArpRecords[i].linked_interface==hw && ArpRecords[i].protocol_address==destip) return i;
		}
	}
	return -1;
}
int sgIP_GetArpSlot() {
	int i,m,midle;
	m=0;
	midle=0;
	for(i=0;i<SGIP_ARP_MAXENTRIES;i++) {
		if(ArpRecords[i].flags&SGIP_ARP_FLAG_ACTIVE) {
			if(ArpRecords[i].idletime>=midle) {
				midle=ArpRecords[i].idletime; m=i;
			}
		} else {
			return i;
		}
	}
	// this slot *was* in use, so let's fix that situation.
	if(ArpRecords[m].queued_packet) sgIP_memblock_free(ArpRecords[m].queued_packet);
	ArpRecords[m].flags=0;
	ArpRecords[m].retrycount=0;
	ArpRecords[m].idletime=0;
	ArpRecords[m].queued_packet=0;
	return m;
}

int sgIP_is_broadcast_address(sgIP_Hub_HWInterface * hw, unsigned long ipaddr) {
	if((hw->snmask | ipaddr) == 0xFFFFFFFF) return 1;
	return 0;
}

// this function will protect against malformed packets that could cause internal problems.
int sgIP_ARP_Check_isok(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb, sgIP_Header_ARP * arp) {
	return 1; // doesn't do anything yet ;)
}

void	sgIP_ARP_Init() {
	int i;
	for(i=0;i<SGIP_ARP_MAXENTRIES;i++) {
		ArpRecords[i].flags=0;
		ArpRecords[i].idletime=0;
		ArpRecords[i].queued_packet=0;
	}
}
void sgIP_ARP_Timer100ms() {
	int i;
	for(i=0;i<SGIP_ARP_MAXENTRIES;i++) {
		if(ArpRecords[i].flags & SGIP_ARP_FLAG_ACTIVE) {
			ArpRecords[i].idletime++;
			if(!(ArpRecords[i].flags&SGIP_ARP_FLAG_HAVEHWADDR)) {
				ArpRecords[i].retrycount++;
				if(ArpRecords[i].retrycount>125) { // it's a lost cause.
					if(ArpRecords[i].queued_packet) { // if there is already a queued packet, kill it.
						sgIP_memblock_free(ArpRecords[i].queued_packet);
					}
					ArpRecords[i].flags=0;
					continue;
				}
				if((ArpRecords[i].retrycount&7)==7) { // attempt retransmit of ARP frame.
					sgIP_ARP_SendARPRequest(ArpRecords[i].linked_interface, ArpRecords[i].linked_protocol, ArpRecords[i].protocol_address);
				}
			}
		}
	}
}

void sgIP_ARP_FlushInterface(sgIP_Hub_HWInterface * hw) {
	int i;
	for(i=0;i<SGIP_ARP_MAXENTRIES;i++) {
		if(ArpRecords[i].linked_interface==hw) ArpRecords[i].flags=0;
		if(hw==0) ArpRecords[i].flags=0; // flush all interfaces
	}
}
 // don't *really* need to process this, but it helps.
int sgIP_ARP_ProcessIPFrame(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb) {

	return 0;
}
int sgIP_ARP_ProcessARPFrame(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb) {
	int i, j, ip;
   if(!hw || !mb) return 0;
	sgIP_memblock_exposeheader(mb,-14); // hide 14 bytes at the start temporarily...
	// look at arp frame...
	sgIP_Header_ARP * arp = (sgIP_Header_ARP *) mb->datastart;

	if(!sgIP_ARP_Check_isok(hw,mb,arp)) {
		sgIP_memblock_free(mb);
		return 0; // error - arp header incorrect somehow.
	}
	sgIP_memblock_exposeheader(mb,14); // re-expose 14 bytes at the start...

	if(htons(arp->opcode)==1) { // request
		// requested IP
		ip = arp->addresses[arp->hw_addr_len*2+4+0]+(arp->addresses[arp->hw_addr_len*2+4+1]<<8)+(arp->addresses[arp->hw_addr_len*2+4+2]<<16)+(arp->addresses[arp->hw_addr_len*2+4+3]<<24);
		SGIP_DEBUG_MESSAGE(("ARP: request IP %08X",ip));
		if(ip==hw->ipaddr) {// someone's asking for our info, toss them a reply.
			sgIP_ARP_SendARPResponse(hw,mb);
			return 0;
		}
	}
	if(htons(arp->opcode)==2) { // response
		// sender IP
		ip = arp->addresses[arp->hw_addr_len+0]+(arp->addresses[arp->hw_addr_len+1]<<8)+(arp->addresses[arp->hw_addr_len+2]<<16)+(arp->addresses[arp->hw_addr_len+3]<<24);
		i=sgIP_FindArpSlot(hw,ip);
		if(i!=-1) { // we've been waiting for you...
			for(j=0;j<arp->hw_addr_len;j++) ArpRecords[i].hw_address[j]=arp->addresses[j];
			ArpRecords[i].flags|=SGIP_ARP_FLAG_HAVEHWADDR;
			sgIP_memblock * mb2;
			mb2=ArpRecords[i].queued_packet;
			ArpRecords[i].queued_packet=0;
			if(mb2) sgIP_ARP_SendProtocolFrame(hw,mb2,ArpRecords[i].linked_protocol,ip);
		}
	}

	sgIP_memblock_free(mb);
	return 0;
}
int sgIP_ARP_SendProtocolFrame(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb, unsigned short protocol, unsigned long destaddr) {
	int i,j;
	int m;
	sgIP_Header_Ethernet * ether;
   if(!hw || !mb) return 0;
	sgIP_memblock_exposeheader(mb,14); // add 14 bytes at the start for the header

	if(sgIP_is_broadcast_address(hw,destaddr)) {
		// construct ethernet header
		ether = (sgIP_Header_Ethernet *) mb->datastart;
		for(j=0;j<6;j++) {
			ether->src_mac[j] = hw->hwaddr[j];
			ether->dest_mac[j]= 0xFF; // broadcast destination
		}
		ether->protocol=protocol;
		return sgIP_Hub_SendRawPacket(hw,mb); // this function will free the memory block when it's done.
	}

	i=sgIP_FindArpSlot(hw,destaddr);
	if(i!=-1) {
		if(ArpRecords[i].flags & SGIP_ARP_FLAG_HAVEHWADDR) { // we have the adddress
			ArpRecords[i].idletime=0;
			// construct ethernet header
			ether = (sgIP_Header_Ethernet *) mb->datastart;
			for(j=0;j<6;j++) {
				ether->src_mac[j] = hw->hwaddr[j];
				ether->dest_mac[j]= ArpRecords[i].hw_address[j];
			}
			ether->protocol=protocol;
			return sgIP_Hub_SendRawPacket(hw,mb); // this function will free the memory block when it's done.
		} else { // we don't have the address, but are looking for it.
			if(ArpRecords[i].queued_packet) { // if there is already a queued packet, reject the new one.
				sgIP_memblock_free(mb);
				return 0; // couldn't send.
			} else {
				sgIP_memblock_exposeheader(mb,-14); // re-hide ethernet header.
				ArpRecords[i].queued_packet=mb; // queue packet.
				ArpRecords[i].linked_protocol=protocol; // queue packet.
				return 0;
			}
		}
	}
	m=sgIP_GetArpSlot(); // gets and cleans out an arp slot for us
		// build new record
	ArpRecords[m].flags=SGIP_ARP_FLAG_ACTIVE;
	ArpRecords[m].idletime=0;
	ArpRecords[m].retrycount=0;
	ArpRecords[m].linked_interface=hw;
	ArpRecords[m].protocol_address=destaddr;
	sgIP_memblock_exposeheader(mb,-14); // re-hide ethernet header.
	ArpRecords[m].queued_packet=mb;
	ArpRecords[m].linked_protocol=protocol;
	sgIP_ARP_SendARPRequest(hw,protocol,destaddr);
	return 0; // queued, but not sent yet.
}

int sgIP_ARP_SendARPResponse(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb) {
	int i;
   if(!hw || !mb) return 0;
	sgIP_memblock_exposeheader(mb,-14); // hide 14 bytes at the start temporarily...

	// Repurpose existing ARP packet
	sgIP_Header_ARP * arp = (sgIP_Header_ARP *) mb->datastart;
	if(!sgIP_ARP_Check_isok(hw,mb,arp)) {
		sgIP_memblock_free(mb);
		return 0; // error - arp header incorrect somehow.
	}

	if(arp->hw_addr_len!=hw->hwaddrlen || arp->protocol_addr_len!=4) {
		// eek! can't send it back in this sorry state!
		sgIP_memblock_free(mb);
		return 0;
	}

	arp->opcode=htons(2); // response
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i+4+hw->hwaddrlen]=arp->addresses[i]; // copy src hw addr
	for(i=0;i<4;i++) arp->addresses[i+(hw->hwaddrlen)*2+4]=arp->addresses[i+hw->hwaddrlen];
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i]=hw->hwaddr[i];
	for(i=0;i<4;i++) arp->addresses[i+(hw->hwaddrlen)]=(hw->ipaddr>>(i*8))&255;

	// construct ethernet header
	sgIP_memblock_exposeheader(mb,14); // add 14 bytes at the start for the header
	sgIP_Header_Ethernet * ether = (sgIP_Header_Ethernet *) mb->datastart;
	for(i=0;i<6;i++) {
		ether->src_mac[i] = hw->hwaddr[i];
		ether->dest_mac[i]= arp->addresses[i+4+hw->hwaddrlen]; // requesting party
	}
	ether->protocol=htons(0x0806); // ARP protocol

	// Send ethernet packet
	return sgIP_Hub_SendRawPacket(hw,mb); // this function will free the memory block when it's done.
}
int sgIP_ARP_SendGratARP(sgIP_Hub_HWInterface * hw) {
	int i;
   if(!hw) return 0;
	sgIP_memblock * mb = sgIP_memblock_alloc(SGIP_HEADER_ARP_BASESIZE+2*4 + 2*hw->hwaddrlen);
	if(!mb) return 0;

	// Construct ARP packet
	sgIP_Header_ARP * arp = (sgIP_Header_ARP *) mb->datastart;
	arp->hwspace=htons(1); // ethernet
	arp->protocol=htons(0x0800);
	arp->opcode=htons(2); // response
	arp->hw_addr_len=hw->hwaddrlen;
	arp->protocol_addr_len= 4;
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i]=hw->hwaddr[i];
	for(i=0;i<4;i++) arp->addresses[i+hw->hwaddrlen]=(hw->ipaddr>>(i*8))&255;
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i+4+hw->hwaddrlen]=hw->hwaddr[i];
	for(i=0;i<4;i++) arp->addresses[i+hw->hwaddrlen*2+4]=(hw->ipaddr>>(i*8))&255;

	// construct ethernet header
	sgIP_memblock_exposeheader(mb,14); // add 14 bytes at the start for the header
	sgIP_Header_Ethernet * ether = (sgIP_Header_Ethernet *) mb->datastart;
	for(i=0;i<6;i++) {
		ether->src_mac[i] = hw->hwaddr[i];
		ether->dest_mac[i]= 0xFF; // broadcast packet
	}
	ether->protocol=htons(0x0806); // ARP protocol

	// Send ethernet packet
	return sgIP_Hub_SendRawPacket(hw,mb); // this function will free the memory block when it's done.
}
int sgIP_ARP_SendARPRequest(sgIP_Hub_HWInterface * hw, int protocol, unsigned long protocol_addr) {
	int i;
   if(!hw) return 0;
	sgIP_memblock * mb = sgIP_memblock_alloc(SGIP_HEADER_ARP_BASESIZE+2*4 + 2*hw->hwaddrlen);
	if(!mb) return 0;

	// Construct ARP packet
	sgIP_Header_ARP * arp = (sgIP_Header_ARP *) mb->datastart;
	arp->hwspace=htons(1); // 1=ethernet
	arp->protocol=(protocol);
	arp->opcode=htons(1); // 1=request
	arp->hw_addr_len=hw->hwaddrlen;
	arp->protocol_addr_len= 4;
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i]=hw->hwaddr[i];
	for(i=0;i<4;i++) arp->addresses[i+hw->hwaddrlen]=(hw->ipaddr>>(i*8))&255;
	for(i=0;i<hw->hwaddrlen;i++) arp->addresses[i+4+hw->hwaddrlen]=0;
	for(i=0;i<4;i++) arp->addresses[i+hw->hwaddrlen*2+4]=(protocol_addr>>(i*8))&255;

	// construct ethernet header
	sgIP_memblock_exposeheader(mb,14); // add 14 bytes at the start for the header
	sgIP_Header_Ethernet * ether = (sgIP_Header_Ethernet *) mb->datastart;
	for(i=0;i<6;i++) {
		ether->src_mac[i] = hw->hwaddr[i];
		ether->dest_mac[i]= 0xFF; // broadcast packet
	}
	ether->protocol=htons(0x0806); // ARP protocol

	// Send ethernet packet
	return sgIP_Hub_SendRawPacket(hw,mb); // this function will free the memory block when it's done.
}
