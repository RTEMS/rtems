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
#include "sgIP_UDP.h"
#include "sgIP_IP.h"

sgIP_Record_UDP * udprecords;
int udpport_counter;
extern unsigned long volatile sgIP_timems;

void sgIP_UDP_Init() {
	udprecords=0;
   udpport_counter=SGIP_UDP_FIRSTOUTGOINGPORT;
}


int sgIP_UDP_GetUnusedOutgoingPort() {
   int myport,clear;
   sgIP_Record_UDP * rec;
   udpport_counter+=(sgIP_timems&1023); // semi-random
   if(udpport_counter>SGIP_UDP_LASTOUTGOINGPORT) udpport_counter=SGIP_UDP_FIRSTOUTGOINGPORT;
   while(1) {
      rec = udprecords;
      myport=udpport_counter++;
      if(udpport_counter>SGIP_UDP_LASTOUTGOINGPORT) udpport_counter=SGIP_UDP_FIRSTOUTGOINGPORT;
      clear=1;
      while(rec) {
         if(rec->srcport==myport) { clear=0; break; }
         rec=rec->next;
      }
      if(clear) return myport;
   }
}

int sgIP_UDP_CalcChecksum(sgIP_memblock * mb, unsigned long srcip, unsigned long destip, int totallength) {
	int checksum;
	if(!mb) return 0;
	if(mb->totallength&1) mb->datastart[mb->totallength]=0;
	checksum=sgIP_memblock_IPChecksum(mb,0,mb->totallength);
	// add in checksum of "faux header"
	checksum+=(destip&0xFFFF);
	checksum+=(destip>>16);
	checksum+=(srcip&0xFFFF);
	checksum+=(srcip>>16);
	checksum+=htons(totallength);
	checksum+=(17)<<8;
    checksum=(checksum&0xFFFF) + (checksum>>16);
    checksum=(checksum&0xFFFF) + (checksum>>16);

    checksum = (~checksum)&0xFFFF;
    if(checksum==0) checksum=0xFFFF;
	return checksum;
}

int sgIP_UDP_ReceivePacket(sgIP_memblock * mb, unsigned long srcip, unsigned long destip) {
	if(!mb) return 0;
	int chk = sgIP_UDP_CalcChecksum(mb,srcip,destip,mb->totallength);
	if(chk!=0xFFFF) {
		SGIP_DEBUG_MESSAGE(("UDP receive checksum incorrect"));
		sgIP_memblock_free(mb);
		return 0; // checksum error
	}
	sgIP_Header_UDP * udp;
	udp=(sgIP_Header_UDP *)mb->datastart;
	sgIP_Record_UDP * rec;
	sgIP_memblock *tmb;
	SGIP_INTR_PROTECT();
	rec=udprecords;

	while(rec) {
		if((rec->srcip==destip || rec->srcip==0) && rec->srcport==udp->destport && rec->state!=SGIP_UDP_STATE_UNUSED) break; // a match!
		rec=rec->next;
	}
	if(!rec) { // no matching records
		sgIP_memblock_free(mb);
		SGIP_INTR_UNPROTECT();
		return 0;
	}
	// we have a record and a packet for it; add some data to the record and stuff it into the record queue.
	sgIP_memblock_exposeheader(mb,4);
	*((unsigned long *)mb->datastart)=srcip; // keep srcip around.
	if(rec->incoming_queue==0) {
		rec->incoming_queue=mb;
	} else {
		rec->incoming_queue_end->next=mb;
	}

	tmb=mb;
	while(tmb->next) tmb=tmb->next;
	rec->incoming_queue_end=tmb;
	// ok, data added to queue - yay!
	// that means... we're done.

	SGIP_INTR_UNPROTECT();
	return 0;
}

int sgIP_UDP_SendPacket(sgIP_Record_UDP * rec, const char * data, int datalen, unsigned long destip, int destport) {
	if(!rec || !data) return SGIP_ERROR(EINVAL);
   if(rec->state!=SGIP_UDP_STATE_BOUND) {
      rec->srcip=0;
      rec->srcport=sgIP_UDP_GetUnusedOutgoingPort();
      rec->state=SGIP_UDP_STATE_BOUND;
   }
	sgIP_memblock * mb = sgIP_memblock_alloc(sgIP_IP_RequiredHeaderSize()+8+datalen);
	if(!mb) return SGIP_ERROR(ENOMEM);
	sgIP_memblock_exposeheader(mb,-sgIP_IP_RequiredHeaderSize()); // hide IP header space for later

	SGIP_INTR_PROTECT();
	unsigned long srcip = sgIP_IP_GetLocalBindAddr(rec->srcip,destip);
	sgIP_Header_UDP * udp = (sgIP_Header_UDP *) mb->datastart;
	udp->srcport=rec->srcport;
	udp->destport=destport;
	udp->length=htons(datalen+8);
	udp->checksum=0;
	int i;
	for(i=0;i<datalen;i++) {
		mb->datastart[i+8]=data[i];
	}
	udp->checksum=sgIP_UDP_CalcChecksum(mb,srcip,destip,mb->totallength);
	sgIP_IP_SendViaIP(mb,17,srcip,destip);

	SGIP_INTR_UNPROTECT();
	return datalen;
}

sgIP_Record_UDP * sgIP_UDP_AllocRecord() {
	SGIP_INTR_PROTECT();
	sgIP_Record_UDP * rec;
	rec = (sgIP_Record_UDP *)sgIP_malloc(sizeof(sgIP_Record_UDP));
	if(rec) {
		rec->destip=0;
		rec->destport=0;
		rec->incoming_queue=0;
		rec->incoming_queue_end=0;
		rec->srcip=0;
		rec->srcport=0;
		rec->state=0;
		rec->next=udprecords;
		udprecords=rec;
	}
	SGIP_INTR_UNPROTECT();
	return rec;
}
void sgIP_UDP_FreeRecord(sgIP_Record_UDP * rec) {
	if(!rec) return;
	SGIP_INTR_PROTECT();
	sgIP_Record_UDP * t;
	// incoming queue is all clumped together as a single memblock, so, time to free it all in one call :)
	if(rec->incoming_queue) sgIP_memblock_free(rec->incoming_queue); // woohoo!
	rec->state=0;
	if(udprecords==rec) {
		udprecords=rec->next;
	} else {
		t=udprecords;
		while(t) {
			if(t->next==rec) {
				t->next=rec->next;
				break;
			}
			t=t->next;
		}
	}
	sgIP_free(rec);

	SGIP_INTR_UNPROTECT();
}

int sgIP_UDP_Bind(sgIP_Record_UDP * rec, int srcport, unsigned long srcip) {
	if(!rec) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	if(rec->state!=SGIP_UDP_STATE_UNUSED) {
		rec->srcip=srcip;
		rec->srcport=srcport;
		if(rec->state==SGIP_UDP_STATE_UNBOUND) rec->state=SGIP_UDP_STATE_BOUND;
	}
	SGIP_INTR_UNPROTECT();
	return 0;
}

int sgIP_UDP_RecvFrom(sgIP_Record_UDP * rec, char * destbuf, int buflength, int flags, unsigned long * sender_ip, unsigned short * sender_port) {
	if(!rec || !destbuf || !sender_ip || !sender_port || buflength==0) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	if(rec->incoming_queue==0) {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EWOULDBLOCK);
	}
	int packetlen=rec->incoming_queue->totallength-12;
	if(packetlen>buflength) {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EMSGSIZE);
	}
	sgIP_memblock * mb;
	*sender_ip=*((unsigned long *)rec->incoming_queue->datastart);
	*sender_port=((unsigned short *)rec->incoming_queue->datastart)[2];
	int totlen,first, buf_start,i;
	totlen=rec->incoming_queue->totallength;
	first=12;
	buf_start=0;

	while(totlen>0 && rec->incoming_queue) {
		totlen-=rec->incoming_queue->thislength;
		for(i=first;i<rec->incoming_queue->thislength;i++) {
			destbuf[buf_start+i-first]=rec->incoming_queue->datastart[i];
		}
		buf_start+=rec->incoming_queue->thislength-first;
		first=0;
		mb=rec->incoming_queue;
		rec->incoming_queue=rec->incoming_queue->next;
		mb->next=0;
		sgIP_memblock_free(mb);
	}
	if(!(rec->incoming_queue)) rec->incoming_queue_end=0;

	SGIP_INTR_UNPROTECT();
	return buf_start;
}

int sgIP_UDP_SendTo(sgIP_Record_UDP * rec, const char * buf, int buflength, int flags, unsigned long dest_ip, int dest_port) {
	return sgIP_UDP_SendPacket(rec,buf,buflength,dest_ip,dest_port);
}

