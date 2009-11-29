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

#include "sgIP_TCP.h"
#include "sgIP_IP.h"
#include "sgIP_Hub.h"
#include "sys/socket.h"

sgIP_Record_TCP * tcprecords;
int port_counter;
unsigned long lasttime;
extern unsigned long volatile sgIP_timems;
sgIP_TCP_SYNCookie synlist[SGIP_TCP_MAXSYNS];

int numsynlist; // number of active entries in synlist (earliest first)

void sgIP_TCP_Init() {
	tcprecords=0;
   numsynlist=0;
	port_counter=SGIP_TCP_FIRSTOUTGOINGPORT;
	lasttime=sgIP_timems;
}

void sgIP_TCP_Timer() { // scan through tcp records and resend anything necessary
   sgIP_Record_TCP * rec;
   int time,i,j;
   time=sgIP_timems-lasttime;
   lasttime=sgIP_timems;
   for(i=0;i<numsynlist;i++) {
	   if(synlist[i].timenext<=time) {
		   j=time-synlist[i].timenext;
		   synlist[i].timebackoff*=2;
		   if(synlist[i].timebackoff>SGIP_TCP_BACKOFFMAX) synlist[i].timebackoff=SGIP_TCP_BACKOFFMAX;
		   if(j>synlist[i].timebackoff) synlist[i].timenext=0; else synlist[i].timenext=synlist[i].timebackoff-j;
		   // resend SYN
		   sgIP_TCP_SendSynReply(SGIP_TCP_FLAG_SYN|SGIP_TCP_FLAG_ACK,synlist[i].localseq,synlist[i].remoteseq,synlist[i].localip,synlist[i].remoteip,synlist[i].localport,synlist[i].remoteport,-1);
	   } else {
		   synlist[i].timenext-=time;
	   }
   }
   rec=tcprecords;
   while(rec) {
      time=sgIP_timems-rec->time_last_action;
      switch(rec->tcpstate) {
      case SGIP_TCP_STATE_NODATA: // newly allocated [do nothing]
      case SGIP_TCP_STATE_UNUSED: // allocated & BINDed [do nothing]
      case SGIP_TCP_STATE_CLOSED: // Block is unused. [do nothing]
      case SGIP_TCP_STATE_LISTEN: // listening [do nothing]
      case SGIP_TCP_STATE_FIN_WAIT_2: // got ACK for our FIN, haven't got FIN yet. [do nothing]
         break;
      case SGIP_TCP_STATE_SYN_SENT: // connect initiated [resend syn]
         if(time>rec->time_backoff) {
            rec->retrycount++;
            if(rec->retrycount>=SGIP_TCP_MAXRETRY) {
               //error
               rec->errorcode=ECONNABORTED;
               rec->tcpstate=SGIP_TCP_STATE_CLOSED;
               break;
            }
			j=rec->time_backoff;
			j*=2;
			if(j>SGIP_TCP_BACKOFFMAX) j=SGIP_TCP_BACKOFFMAX;
            sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_SYN,0);
			rec->time_backoff=j; // preserve backoff
         }
         break;
	  case SGIP_TCP_STATE_CLOSE_WAIT: // got FIN, wait for user code to close socket & send FIN [do nothing]
      case SGIP_TCP_STATE_ESTABLISHED: // syns have been exchanged [check for data in buffer, send]
		 if(rec->want_shutdown==1 && rec->buf_tx_out==rec->buf_tx_in) { // oblige & shutdown
			 sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_FIN | SGIP_TCP_FLAG_ACK,0);
			 if(rec->tcpstate==SGIP_TCP_STATE_CLOSE_WAIT) {
				 rec->tcpstate=SGIP_TCP_STATE_CLOSING;
			 } else {
				 rec->tcpstate=SGIP_TCP_STATE_FIN_WAIT_1;
			 }
			 rec->want_shutdown=2;
			 break;
		 }
         j=rec->buf_tx_out-rec->buf_tx_in;
         if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
         j+=(int)(rec->sequence-rec->sequence_next);
         if(j>0) {// never-sent bytes
            if(time>SGIP_TCP_TRANSMIT_DELAY) { // 1000 is an arbitrary constant.
               j=rec->buf_tx_out-rec->buf_tx_in;
               if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
               i=(int)(rec->txwindow-rec->sequence);
               if(j>i) j=i;
               i=sgIP_IP_MaxContentsSize(rec->destip)-20; // max tcp data size
               if(j>i) j=i;
               sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,i);
               break;
            }
         }
         if(time>rec->time_backoff && rec->buf_tx_out!=rec->buf_tx_in) { // resend last packet
            j=rec->buf_tx_out-rec->buf_tx_in;
            if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
            i=(int)(rec->txwindow-rec->sequence);
            if(j>i) j=i;
            i=sgIP_IP_MaxContentsSize(rec->destip)-20; // max tcp data size
            if(j>i) j=i;
			j=rec->time_backoff;
			j*=2;
			if(j>SGIP_TCP_BACKOFFMAX) j=SGIP_TCP_BACKOFFMAX;
            sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,i);
			rec->time_backoff=j; // preserve backoff
            break;
         }
         break;
      case SGIP_TCP_STATE_FIN_WAIT_1: // sent a FIN, haven't got FIN or ACK yet. [resend fin]
         if(time>rec->time_backoff) {
            rec->retrycount++;
            if(rec->retrycount>=SGIP_TCP_MAXRETRY) {
               //error
               rec->errorcode=ETIMEDOUT;
               rec->tcpstate=SGIP_TCP_STATE_CLOSED;
               break;
            }
			j=rec->time_backoff;
			j*=2;
			if(j>SGIP_TCP_BACKOFFMAX) j=SGIP_TCP_BACKOFFMAX;
            sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_FIN,0);
			rec->time_backoff=j; // preserve backoff
         }
         break;
      case SGIP_TCP_STATE_CLOSING: // got FIN, waiting for ACK of our FIN [resend FINACK]
         if(time>rec->time_backoff) {
            rec->retrycount++;
            if(rec->retrycount>=SGIP_TCP_MAXRETRY) {
               //error
               rec->errorcode=ETIMEDOUT;
               rec->tcpstate=SGIP_TCP_STATE_CLOSED;
               break;
            }
			j=rec->time_backoff;
			j*=2;
			if(j>SGIP_TCP_BACKOFFMAX) j=SGIP_TCP_BACKOFFMAX;
            sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_FIN | SGIP_TCP_FLAG_ACK,0);
			rec->time_backoff=j; // preserve backoff
         }
         break;
      case SGIP_TCP_STATE_LAST_ACK: // wait for ACK of our last FIN [resend FIN]
         if(time>rec->time_backoff) {
            rec->retrycount++;
            if(rec->retrycount>=SGIP_TCP_MAXRETRY) {
               //error
               rec->errorcode=ETIMEDOUT;
               rec->tcpstate=SGIP_TCP_STATE_CLOSED;
               break;
            }
			j=rec->time_backoff;
			j*=2;
			if(j>SGIP_TCP_BACKOFFMAX) j=SGIP_TCP_BACKOFFMAX;
            sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_FIN,0);
			rec->time_backoff=j; // preserve backoff
         }
         break;
      case SGIP_TCP_STATE_TIME_WAIT: // wait to ensure remote tcp knows it's been terminated. [reset in 2MSL]
         if(time>SGIP_TCP_TIMEMS_2MSL) {
            rec->errorcode=ESHUTDOWN;
            rec->tcpstate=SGIP_TCP_STATE_CLOSED;
         }
         break;
      }


      rec=rec->next;
   }
}


unsigned long sgIP_TCP_support_seqhash(unsigned long srcip, unsigned long destip, unsigned short srcport, unsigned short destport) {
	unsigned long hash;
	hash=destip;
	hash ^= destport * (0x02041089+sgIP_timems);
	hash ^= srcport * (0x080810422+(sgIP_timems<<1));
	hash ^= srcip * (0x48841221+(sgIP_timems<<3));
	hash ^= destip * (0x04020108+(sgIP_timems<<5));
	return hash;
}
int sgIP_TCP_GetUnusedOutgoingPort() {
	int myport,clear;
	sgIP_Record_TCP * rec;
   port_counter+=(sgIP_timems&1023); // semi-random
   if(port_counter>SGIP_TCP_LASTOUTGOINGPORT) port_counter=SGIP_TCP_FIRSTOUTGOINGPORT;
	while(1) {
		rec = tcprecords;
		myport=port_counter++;
		if(port_counter>SGIP_TCP_LASTOUTGOINGPORT) port_counter=SGIP_TCP_FIRSTOUTGOINGPORT;
		clear=1;
		while(rec) {
			if(rec->srcport==myport && rec->tcpstate!=SGIP_TCP_STATE_CLOSED && rec->tcpstate!=SGIP_TCP_STATE_NODATA) { clear=0; break; }
			rec=rec->next;
		}
		if(clear) return myport;
	}
}

int sgIP_TCP_CalcChecksum(sgIP_memblock * mb, unsigned long srcip, unsigned long destip, int totallength) {
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
	checksum+=(6)<<8;
   checksum= (checksum&0xFFFF) +(checksum>>16);
   checksum= (checksum&0xFFFF) +(checksum>>16);

	return checksum;
}


int sgIP_TCP_ReceivePacket(sgIP_memblock * mb, unsigned long srcip, unsigned long destip) {
	if(!mb) return 0;
	sgIP_Header_TCP * tcp;
   int delta1,delta2, delta3,datalen, shouldReply;
   unsigned long tcpack,tcpseq;
	tcp = (sgIP_Header_TCP *) mb->datastart;
	//                   01234567890123456789012345678901
	//                   TCPxxxxxxxx-xxxxxxxx,xxxx-xxxx
	//SGIP_DEBUG_MESSAGE(("TCP%08X-%08X,%04X-%04X",srcip,destip,tcp->srcport,tcp->destport));
	//                   -Lxxxx,Cxxxx,Fxx,hx,Axxxxxxxx
	//SGIP_DEBUG_MESSAGE(("-L%04X,C%04X,F%02X,h%X,A%08X",mb->totallength,tcp->checksum,tcp->tcpflags,tcp->dataofs_>>4,tcp->acknum));
	if(tcp->checksum!=0x0000 && sgIP_TCP_CalcChecksum(mb,srcip,destip,mb->totallength)!=0xFFFF) { // checksum is invalid!
		SGIP_DEBUG_MESSAGE(("TCP receive checksum incorrect"));
		sgIP_memblock_free(mb);
		return 0;
	}
	sgIP_Record_TCP * rec;
	rec=tcprecords;
	// find associated block.
	while(rec) {
		if(rec->srcport==tcp->destport && (rec->srcip==destip || rec->srcip==0)) {
			if((rec->tcpstate==SGIP_TCP_STATE_LISTEN && (tcp->tcpflags&SGIP_TCP_FLAG_SYN)) || rec->destport==tcp->srcport) break;
		}
		rec=rec->next;
	}
   if(!rec) { // could be completion of an incoming connection?
      tcpack=htonl(tcp->acknum);
      if(tcp->tcpflags&SGIP_TCP_FLAG_ACK) {
         int i,j;
         for(i=0;i<numsynlist;i++) {
            if(synlist[i].localseq+1==tcpack) { // oki! this is probably legit ;)
               rec=synlist[i].linked; // we have the data we need.
               // remove entry from synlist
               numsynlist--;
               i*=3;
               for(;i<numsynlist;i++) {
                  synlist[i]=synlist[i+1]; // assume struct copy
               }
               for(j=0;j<rec->maxlisten;j++) if(!rec->listendata[j]) break; // find last entry in listen queue
               if(j==rec->maxlisten) { rec=0; break; } // discard this connection! we have no space in the listen queue.

               rec->listendata[j]=sgIP_TCP_AllocRecord();
               j++;
               if(j!=rec->maxlisten) rec->listendata[j]=0;

               rec=rec->listendata[j-1];

               // fill in data about the connection.
               rec->tcpstate=SGIP_TCP_STATE_ESTABLISHED;
               rec->time_last_action=sgIP_timems;
			   rec->time_backoff=SGIP_TCP_GENRETRYMS; // backoff timer
               rec->srcip=destip;
               rec->destip=srcip;
               rec->srcport=tcp->destport;
               rec->destport=tcp->srcport;
               rec->sequence=htonl(tcp->acknum);
               rec->ack=htonl(tcp->seqnum);
               rec->sequence_next=rec->sequence;
               rec->rxwindow=rec->ack+1400; // last byte in receive window
               rec->txwindow=rec->sequence+htons(tcp->window);

               sgIP_memblock_free(mb);
               return 0;
            }
         }
      }
   }
	if(!rec) { // we don't have a clue what this one is.
#ifndef SGIP_TCP_STEALTH
		// send a RST
		sgIP_TCP_SendSynReply(SGIP_TCP_FLAG_RST,ntohl(tcp->acknum),0,destip,srcip,tcp->destport,tcp->srcport,0);
#endif
		sgIP_memblock_free(mb);
		return 0;
	}
	// check sequence and ACK numbers, to ensure they're in range.
   tcpack=htonl(tcp->acknum);
   tcpseq=htonl(tcp->seqnum);
   datalen=mb->totallength-(tcp->dataofs_>>4)*4;
   shouldReply=0;
   if(tcp->tcpflags&SGIP_TCP_FLAG_RST) { // verify if rst is legit, and act on it.
      // check seq against receive window
      delta1=(int)(tcpseq-rec->ack);
      delta2=(int)(rec->rxwindow-tcpseq);
      if(delta1<0 || delta2<0 || rec->tcpstate==SGIP_TCP_STATE_LISTEN) {
         // out of range, ignore
      } else {
         // in range! reset connection.
         rec->errorcode=ECONNRESET;
         rec->tcpstate=SGIP_TCP_STATE_CLOSED;
      }
      sgIP_memblock_free(mb);
      return 0;
   }

   if((tcp->tcpflags&SGIP_TCP_FLAG_ACK) && !(tcp->tcpflags&SGIP_TCP_FLAG_SYN)) { // doesn't work very well with SYN.
      // verify ack value (checking ack sequence vs transmit window)
      delta1=(int)(tcpack-rec->sequence);
      delta2=(int)(rec->txwindow-tcpack);
      if(delta1<0 || delta2<0) { // invalid ack range, discard packet
         sgIP_memblock_free(mb);
         return 0;
      }
	  delta2=tcpack-rec->sequence;
      rec->sequence=tcpack;
	  delta2+=rec->buf_tx_in;
	  if(delta2>=SGIP_TCP_TRANSMITBUFFERLENGTH) delta2-=SGIP_TCP_TRANSMITBUFFERLENGTH;
	  rec->buf_tx_in=delta2;
      if(delta1>0) shouldReply=1;
   }
   rec->txwindow=rec->sequence+htons(tcp->window);

	// now, decide what to do with our nice new shiny memblock...

   // for most states, receive data
   switch(rec->tcpstate) {
	case SGIP_TCP_STATE_NODATA: // newly allocated
	case SGIP_TCP_STATE_UNUSED: // allocated & BINDed
	case SGIP_TCP_STATE_CLOSED: // Block is unused.
	case SGIP_TCP_STATE_LISTEN: // listening
	case SGIP_TCP_STATE_TIME_WAIT: // wait to ensure remote tcp knows it's been terminated.
	case SGIP_TCP_STATE_SYN_SENT: // connect initiated
	case SGIP_TCP_STATE_CLOSE_WAIT: // got FIN, wait for user code to close socket & send FIN
	case SGIP_TCP_STATE_CLOSING: // got FIN, waiting for ACK of our FIN
	case SGIP_TCP_STATE_LAST_ACK: // wait for ACK of our last FIN
		break;
	case SGIP_TCP_STATE_SYN_RECEIVED: // spawned from listen socket; or from syn sent.
	case SGIP_TCP_STATE_ESTABLISHED: // syns have been exchanged
	case SGIP_TCP_STATE_FIN_WAIT_1: // sent a FIN, haven't got FIN or ACK yet.
	case SGIP_TCP_STATE_FIN_WAIT_2: // got ACK for our FIN, haven't got FIN yet.
		if(tcp->tcpflags&SGIP_TCP_FLAG_ACK) {
			// check end of incoming data against receive window
			delta1=(int)(tcpseq+datalen-rec->ack); // check end of data vs start of window (>=0, end of data is equal to or after start of unreceived data)
			delta2=(int)(rec->rxwindow-tcpseq-datalen); // check end of data vs end of window (>=0, end of data is equal to or before end of rx window)
			delta3=(int)(rec->ack-tcpseq); // check start of data vs start of window (>=0, start of data is equal or before the next expected byte)
			if(delta1<0 || delta2<0 || delta3<0) {
				if(delta1>-SGIP_TCP_RECEIVEBUFFERLENGTH) { // ack it anyway, they got lost on the retard bus.
					sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
				}
				break; // out of range, they should know better.
			}
			{
				int datastart=(tcp->dataofs_>>4)*4;
				delta1=(int)(tcpseq-rec->ack);
				if(delta1<0) { // data is partly ack'd...just copy what we need.
					datastart-=delta1;
					datalen+=delta1;
				}
				// copy data into the fifo
				rec->ack+=datalen;
				delta1=datalen;
				while(datalen>0) { // don't actually need to check the rx buffer length, if the ack check approved it, it will be in range (not overflow) by default
					delta2=SGIP_TCP_RECEIVEBUFFERLENGTH-rec->buf_rx_out; // number of bytes til the end of the buffer
					if(datalen<delta2) delta2=datalen;
					sgIP_memblock_CopyToLinear(mb,rec->buf_rx+rec->buf_rx_out,datastart,delta2);
					datalen-=delta2;
					datastart+=delta2;
					rec->buf_rx_out += delta2;
					if(rec->buf_rx_out>=SGIP_TCP_RECEIVEBUFFERLENGTH) rec->buf_rx_out-=SGIP_TCP_RECEIVEBUFFERLENGTH;
				}
				if(rec->tcpstate==SGIP_TCP_STATE_FIN_WAIT_1 || rec->tcpstate==SGIP_TCP_STATE_FIN_WAIT_2) break;
				if(shouldReply || delta1>=0) { // send a packet in reply, ha!
					delta1=rec->buf_tx_out-rec->buf_tx_in;
					if(delta1<0) delta1+=SGIP_TCP_TRANSMITBUFFERLENGTH;
					delta2=(int)(rec->txwindow-rec->sequence);
					if(delta1>delta2) delta1=delta2;
					delta2=sgIP_IP_MaxContentsSize(rec->destip)-20; // max tcp data size
					if(delta1>delta2) delta1=delta2;
					if(delta1>=0) { // could be less than 0, but very odd.
						sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,delta1);
					}
				}
			}
		}
   }

   // decide what to do with the others
	switch(rec->tcpstate) {
	case SGIP_TCP_STATE_NODATA: // newly allocated
	case SGIP_TCP_STATE_UNUSED: // allocated & BINDed
	case SGIP_TCP_STATE_CLOSED: // Block is unused.
		break; // can't do anything in these states.
	case SGIP_TCP_STATE_LISTEN: // listening
      if(tcp->tcpflags&SGIP_TCP_FLAG_SYN) { // other end requesting a connection
         if(numsynlist==SGIP_TCP_MAXSYNS) {
            numsynlist--;
            for(delta1=0;delta1<numsynlist;delta1++) {
               synlist[delta1]=synlist[delta1+1]; // assume struct copy
            }
         }
         {
            unsigned long myseq,myport;
            myport=tcp->destport;
            myseq=sgIP_TCP_support_seqhash(srcip,destip,tcp->srcport,myport);
            // send relevant synack
			sgIP_TCP_SendSynReply(SGIP_TCP_FLAG_SYN|SGIP_TCP_FLAG_ACK,myseq,tcpseq+1,destip,srcip,myport,tcp->srcport,-1);
            synlist[numsynlist].localseq=myseq;
			synlist[numsynlist].timebackoff=SGIP_TCP_SYNRETRYMS;
			synlist[numsynlist].timenext=SGIP_TCP_SYNRETRYMS;
            synlist[numsynlist].linked=rec;
			synlist[numsynlist].remoteseq=tcpseq+1;
			synlist[numsynlist].remoteip=srcip;
			synlist[numsynlist].localip=destip;
			synlist[numsynlist].localport=myport;
			synlist[numsynlist].remoteport=tcp->srcport;
            numsynlist++;
         }
      }
		break;
	case SGIP_TCP_STATE_SYN_SENT: // connect initiated
      switch(tcp->tcpflags&(SGIP_TCP_FLAG_SYN|SGIP_TCP_FLAG_ACK)) {
      case SGIP_TCP_FLAG_SYN | SGIP_TCP_FLAG_ACK: // both flags set
         rec->ack=tcpseq+1;
         rec->sequence=tcpack;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
         rec->tcpstate=SGIP_TCP_STATE_ESTABLISHED;
         rec->retrycount=0;
         break;
      case SGIP_TCP_FLAG_SYN: // just got a syn...
         rec->ack=tcpseq+1;
         rec->sequence=tcpack;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
         rec->tcpstate=SGIP_TCP_STATE_SYN_RECEIVED;
         rec->retrycount=0;
         break;
      }
		break;
	case SGIP_TCP_STATE_SYN_RECEIVED: // spawned from listen socket; or from syn sent.
      if(tcp->tcpflags&SGIP_TCP_FLAG_ACK) {
         rec->tcpstate=SGIP_TCP_STATE_ESTABLISHED;
         rec->retrycount=0;
      }
		break;
	case SGIP_TCP_STATE_ESTABLISHED: // syns have been exchanged
      if(tcp->tcpflags&SGIP_TCP_FLAG_FIN) {
         // check sequence against next ack number
         delta1=(int)(tcpseq-rec->ack);
         //delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<0 || delta1>0) break; // out of range, they should know better.
         // this is the end...
         rec->tcpstate=SGIP_TCP_STATE_CLOSE_WAIT;
         rec->ack=tcpseq+datalen+1;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);

      }
		break;
	case SGIP_TCP_STATE_FIN_WAIT_1: // sent a FIN, haven't got FIN or ACK yet.
      switch(tcp->tcpflags&(SGIP_TCP_FLAG_FIN|SGIP_TCP_FLAG_ACK)) {
      case SGIP_TCP_FLAG_FIN:
         // check sequence against next ack number
         delta1=(int)(tcpseq-rec->ack);
         //delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<0 || delta1>0) break; // out of range, they should know better.

         rec->tcpstate=SGIP_TCP_STATE_CLOSING;
         rec->ack=tcpseq+datalen+1;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
         break;
      case SGIP_TCP_FLAG_ACK: // already checked ack against appropriate window
         rec->tcpstate=SGIP_TCP_STATE_FIN_WAIT_2;
         break;
      case (SGIP_TCP_FLAG_FIN | SGIP_TCP_FLAG_ACK): // already checked ack, check sequence though
         // check sequence against next ack number
         delta1=(int)(tcpseq-rec->ack);
         //delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<0 || delta1>0) break; // out of range, they should know better.
         rec->tcpstate=SGIP_TCP_STATE_TIME_WAIT;
		 rec->ack=tcpseq+datalen+1;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
         break;
      }
      break;
	case SGIP_TCP_STATE_FIN_WAIT_2: // got ACK for our FIN, haven't got FIN yet.
      if(tcp->tcpflags&SGIP_TCP_FLAG_FIN) {
          // check sequence against next ack number
          delta1=(int)(tcpseq-rec->ack);
          //delta2=(int)(rec->rxwindow-tcpseq);
          if(delta1<0 || delta1>0) break; // out of range, they should know better.

         rec->tcpstate=SGIP_TCP_STATE_TIME_WAIT;
		 rec->ack=tcpseq+datalen+1;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
      }
		break;
	case SGIP_TCP_STATE_CLOSE_WAIT: // got FIN, wait for user code to close socket & send FIN
      if(tcp->tcpflags&SGIP_TCP_FLAG_FIN) {
          // check sequence against next ack number
          delta1=(int)(tcpseq-rec->ack);
          //delta2=(int)(rec->rxwindow-tcpseq);
          if(delta1<0 || delta1>0) break; // out of range, they should know better.

         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0); // they still don't seem to have got our ack, we'll send it again.
      }
      break;
	case SGIP_TCP_STATE_CLOSING: // got FIN, waiting for ACK of our FIN
      switch(tcp->tcpflags&(SGIP_TCP_FLAG_FIN|SGIP_TCP_FLAG_ACK)) {
      case SGIP_TCP_FLAG_FIN:
         // check sequence against receive window
         delta1=(int)(tcpseq-rec->ack);
         delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<1 || delta2<0) break; // out of range, they should know better.

         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0); // resend their ack.
         break;
      case SGIP_TCP_FLAG_ACK: // already checked ack against appropriate window
         rec->tcpstate=SGIP_TCP_STATE_TIME_WAIT;
         break;
      case (SGIP_TCP_FLAG_FIN | SGIP_TCP_FLAG_ACK): // already checked ack, check sequence though
         // check sequence against receive window
         delta1=(int)(tcpseq-rec->ack);
         delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<1 || delta2<0) break; // out of range, they should know better.
         rec->tcpstate=SGIP_TCP_STATE_TIME_WAIT;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
         break;
      }
      break;

	case SGIP_TCP_STATE_LAST_ACK: // wait for ACK of our last FIN
      if(tcp->tcpflags&SGIP_TCP_FLAG_ACK) {
         rec->tcpstate=SGIP_TCP_STATE_TIME_WAIT;
      }
		break;
	case SGIP_TCP_STATE_TIME_WAIT: // wait to ensure remote tcp knows it's been terminated.
      if(tcp->tcpflags&SGIP_TCP_FLAG_FIN) {
         // check sequence against receive window
         delta1=(int)(tcpseq-rec->ack);
         delta2=(int)(rec->rxwindow-tcpseq);
         if(delta1<1 || delta2<0) break; // out of range, they should know better.

         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0); // send 'em a grat ACK
      }
		break;
	}
	sgIP_memblock_free(mb);
	return 0;
}

sgIP_memblock * sgIP_TCP_GenHeader(sgIP_Record_TCP * rec, int flags, int datalength) {
	sgIP_memblock * mb = sgIP_memblock_alloc(datalength+20+sgIP_IP_RequiredHeaderSize());
	int windowlen;
	if(!mb) return 0;
	sgIP_memblock_exposeheader(mb,-sgIP_IP_RequiredHeaderSize()); // hide IP header space for later
	sgIP_Header_TCP * tcp = (sgIP_Header_TCP *) mb->datastart;
	tcp->srcport=rec->srcport;
	tcp->destport=rec->destport;
	tcp->seqnum=htonl(rec->sequence);
	tcp->acknum=htonl(rec->ack);
	tcp->tcpflags=flags;
	tcp->urg_ptr=0; // no support for URG data atm.
	tcp->checksum=0;
	tcp->dataofs_=5<<4; // header length == 20 (5*32bit)
	windowlen=rec->buf_rx_out-rec->buf_rx_in;
	if(windowlen<0) windowlen+=SGIP_TCP_RECEIVEBUFFERLENGTH; // we now have the amount in the buffer
	windowlen = SGIP_TCP_RECEIVEBUFFERLENGTH-windowlen-1;
	if(windowlen<0) windowlen=0;
    if(flags&SGIP_TCP_FLAG_ACK) rec->want_reack = windowlen<SGIP_TCP_REACK_THRESH; // indicate an additional ack should be sent when we have more space in the buffer.
	if(windowlen>65535) windowlen=65535;
   if(windowlen>1400) windowlen=1400; // don't want to deal with IP fragmentation.
   rec->rxwindow=rec->ack+windowlen; // last byte in receive window
	tcp->window=htons(windowlen);
	return mb;
}
void sgIP_TCP_FixChecksum(unsigned long srcip, unsigned long destip, sgIP_memblock * mb) {
	int checksum;
	if(!mb) return;
   sgIP_Header_TCP * tcp;
   tcp = (sgIP_Header_TCP *) mb->datastart;
   tcp->checksum=0;
   checksum=sgIP_memblock_IPChecksum(mb,0,mb->totallength);

	// add in checksum of "faux header"
	checksum+=(destip&0xFFFF);
	checksum+=(destip>>16);
	checksum+=(srcip&0xFFFF);
	checksum+=(srcip>>16);
	checksum+=htons(mb->totallength);
	checksum+=(6)<<8;
   checksum=(checksum&0xFFFF) + (checksum>>16);
   checksum=(checksum&0xFFFF) + (checksum>>16);

	checksum = ~checksum;
	if(checksum==0) checksum=0xFFFF;
	tcp->checksum=checksum;
}

int sgIP_TCP_SendPacket(sgIP_Record_TCP * rec, int flags, int datalength) { // data sent is taken directly from the TX fifo.
   int i,j,k;
	if(!rec) return 0;
	SGIP_INTR_PROTECT();

   j=rec->buf_tx_out-rec->buf_tx_in;
   if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
   if(datalength>j) datalength=j;
   sgIP_memblock * mb =sgIP_TCP_GenHeader(rec,flags,datalength);
	if(!mb) {
		SGIP_INTR_UNPROTECT();
		return 0;
	}
   j=20; // destination offset in memblock for data
   rec->sequence_next=rec->sequence+datalength;
   k=rec->buf_tx_in;
   while(datalength>0) {
      i=SGIP_TCP_TRANSMITBUFFERLENGTH-rec->buf_tx_in;
      if(i>datalength)i=datalength;
      sgIP_memblock_CopyFromLinear(mb,rec->buf_tx+k,j,i);
      k+=i;
      if(k>=SGIP_TCP_TRANSMITBUFFERLENGTH) k-=SGIP_TCP_TRANSMITBUFFERLENGTH;
      j+=i;
      datalength-=i;
   }

	sgIP_TCP_FixChecksum(rec->srcip,rec->destip,mb);
	sgIP_IP_SendViaIP(mb,6,rec->srcip,rec->destip);

   rec->time_last_action=sgIP_timems; // semi-generic timer.
   rec->time_backoff=SGIP_TCP_GENRETRYMS; // backoff timer
	SGIP_INTR_UNPROTECT();
   return 0;
}

int sgIP_TCP_SendSynReply(int flags,unsigned long seq, unsigned long ack, unsigned long srcip, unsigned long destip, int srcport, int destport, int windowlen) {
   SGIP_INTR_PROTECT();

   sgIP_memblock * mb = sgIP_memblock_alloc(20+sgIP_IP_RequiredHeaderSize());
   if(!mb) {
      SGIP_INTR_UNPROTECT();
      return 0;
   }
   sgIP_memblock_exposeheader(mb,-sgIP_IP_RequiredHeaderSize()); // hide IP header space for later
   sgIP_Header_TCP * tcp = (sgIP_Header_TCP *) mb->datastart;
   tcp->srcport=srcport;
   tcp->destport=destport;
   tcp->seqnum=htonl(seq);
   tcp->acknum=htonl(ack);
   tcp->tcpflags=flags;
   tcp->urg_ptr=0; // no support for URG data atm.
   tcp->checksum=0;
   tcp->dataofs_=5<<4; // header length == 20 (5*32bit)

   if(windowlen<0 || windowlen>1400) windowlen=1400; // don't want to deal with IP fragmentation.
   tcp->window=htons(windowlen);

   sgIP_TCP_FixChecksum(srcip,destip,mb);
   sgIP_IP_SendViaIP(mb,6,srcip,destip);

   SGIP_INTR_UNPROTECT();
   return 0;
}

sgIP_Record_TCP * sgIP_TCP_AllocRecord() {
	SGIP_INTR_PROTECT();
	sgIP_Record_TCP * rec;
	rec = sgIP_malloc(sizeof(sgIP_Record_TCP));
	if(rec) {
		rec->buf_oob_in=0;
		rec->buf_oob_out=0;
		rec->buf_rx_in=0;
		rec->buf_rx_out=0;
		rec->buf_tx_in=0;
		rec->buf_tx_out=0;
		rec->tcpstate=0;
		rec->next=tcprecords;
		tcprecords=rec;
		rec->maxlisten=0;
		rec->srcip=0;
      rec->retrycount=0;
      rec->errorcode=0;
      rec->listendata=0;
	  rec->want_shutdown=0;
      rec->want_reack=0;
	}
	SGIP_INTR_UNPROTECT();
	return rec;
}
void sgIP_TCP_FreeRecord(sgIP_Record_TCP * rec) {
	if(!rec) return;
	SGIP_INTR_PROTECT();
	sgIP_Record_TCP * t;
   int i,j;
	rec->tcpstate=0;
	if(tcprecords==rec) {
		tcprecords=rec->next;
	} else {
		t=tcprecords;
		while(t) {
			if(t->next==rec) {
				t->next=rec->next;
				break;
			}
			t=t->next;
		}
	}
   if(rec->listendata)  {
      for(i=0;i<rec->maxlisten;i++) {
         if(!rec->listendata[i]) break;
         sgIP_TCP_FreeRecord(rec->listendata[i]);
      }
      // kill any possible waiting elements in the SYN chain.
      j=0;
      for(i=0;i<numsynlist;i++) {
         if(j!=i) {
            synlist[j]=synlist[i];
         }
         if(synlist[i].linked==rec) j--;
         j++;
      }
      numsynlist=j;
      sgIP_free(rec->listendata);
   }
	sgIP_free(rec);

	SGIP_INTR_UNPROTECT();
}

int sgIP_TCP_Bind(sgIP_Record_TCP * rec, int srcport, unsigned long srcip) {
	if(!rec) return 0;
	SGIP_INTR_PROTECT();
	if(rec->tcpstate==SGIP_TCP_STATE_NODATA) {
		rec->srcip=srcip;
		rec->srcport=srcport;
		rec->tcpstate=SGIP_TCP_STATE_UNUSED;
	}
	SGIP_INTR_UNPROTECT();
	return 0;
}
int sgIP_TCP_Listen(sgIP_Record_TCP * rec, int maxlisten) {
	if(!rec) return SGIP_ERROR(EINVAL);
   if(rec->tcpstate!=SGIP_TCP_STATE_UNUSED) return SGIP_ERROR(EINVAL);
	int err;
	SGIP_INTR_PROTECT();
	if(rec->maxlisten!=0) { // we're *already* listening.
		err=0;
	} else {
		err=0;
      if(maxlisten<=0) maxlisten=1;
		rec->maxlisten=maxlisten;
		rec->listendata = (sgIP_Record_TCP **) sgIP_malloc(maxlisten*4); // pointers to TCP records, 0-terminated list.
		if(!rec->listendata) { rec->maxlisten=0; err=0; } else {rec->tcpstate=SGIP_TCP_STATE_LISTEN; rec->listendata[0]=0;}
	}
	SGIP_INTR_UNPROTECT();
	return err;
}

sgIP_Record_TCP * sgIP_TCP_Accept(sgIP_Record_TCP * rec) {
   if(!rec) return (sgIP_Record_TCP *)SGIP_ERROR0(EINVAL);
   if(rec->tcpstate!=SGIP_TCP_STATE_LISTEN) return (sgIP_Record_TCP *)SGIP_ERROR0(EINVAL);
   int err,i;
   sgIP_Record_TCP * t;
   SGIP_INTR_PROTECT();
   if(!rec->listendata) err=SGIP_ERROR0(EINVAL);
   else {
      if(!rec->listendata[0]) {
         err=SGIP_ERROR0(EWOULDBLOCK);
      } else {
         t=rec->listendata[0];
         for(i=1;i<rec->maxlisten;i++) {
            rec->listendata[i-1]=rec->listendata[i];
         }
         rec->listendata[i-1]=0;
         SGIP_INTR_UNPROTECT();
         return t;
      }
   }

   SGIP_INTR_UNPROTECT();
   return 0;
}

int sgIP_TCP_Close(sgIP_Record_TCP * rec) {
	if(!rec) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	if(rec->want_shutdown==0) rec->want_shutdown=1;
	SGIP_INTR_UNPROTECT();
	return 0;
}
int sgIP_TCP_Connect(sgIP_Record_TCP * rec, unsigned long destip, int destport) {
	if(!rec) return SGIP_ERROR(EINVAL);
	SGIP_INTR_PROTECT();
	if(rec->tcpstate==SGIP_TCP_STATE_NODATA) { // need to bind a local address
		rec->srcip=sgIP_IP_GetLocalBindAddr(0,destip);
		rec->srcport=htons(sgIP_TCP_GetUnusedOutgoingPort());
		rec->destip=destip;
		rec->destport=destport;
	} else if(rec->tcpstate==SGIP_TCP_STATE_UNUSED) { // already bound to a local address.
      rec->srcip=sgIP_IP_GetLocalBindAddr(rec->srcip,destip);
		rec->destip=destip;
		rec->destport=destport;
	} else {
		SGIP_INTR_UNPROTECT();
		return SGIP_ERROR(EINVAL);
	}

	// send a SYN packet, and advance the state of the connection
	rec->sequence=sgIP_TCP_support_seqhash(rec->srcip,rec->destip,rec->srcport,rec->destport);
	sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_SYN,0);
   rec->retrycount=0;
	rec->tcpstate=SGIP_TCP_STATE_SYN_SENT;

	SGIP_INTR_UNPROTECT();
	return 0;
}
int sgIP_TCP_Send(sgIP_Record_TCP * rec, const char * datatosend, int datalength, int flags) {
	if(!rec || !datatosend) return SGIP_ERROR(EINVAL);
	if(rec->want_shutdown) return SGIP_ERROR(ESHUTDOWN);
	SGIP_INTR_PROTECT();
	int bufsize;
	bufsize=rec->buf_tx_out-rec->buf_tx_in;
	if(bufsize<0) bufsize+=SGIP_TCP_TRANSMITBUFFERLENGTH;
	if(bufsize==0) { rec->time_last_action=sgIP_timems; 	rec->time_backoff=SGIP_TCP_GENRETRYMS; } // first byte sent, set up delay before sending
	bufsize=SGIP_TCP_TRANSMITBUFFERLENGTH-bufsize-1; // space left in buffer
	if(datalength>bufsize) datalength=bufsize;
	int i,j;
	j=rec->buf_tx_out;
	for(i=0;i<datalength;i++) {
		rec->buf_tx[j++]=datatosend[i];
		if(j==SGIP_TCP_TRANSMITBUFFERLENGTH) j=0;
	}
	rec->buf_tx_out = j;
   // check for immediate transmit
   j=rec->buf_tx_out-rec->buf_tx_in;
   if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
   j+=(int)(rec->sequence-rec->sequence_next);
   if(j>SGIP_TCP_TRANSMIT_IMMTHRESH && rec->tcpstate==SGIP_TCP_STATE_ESTABLISHED) {
      j=(int)(rec->sequence_next-rec->sequence);
      if(j<1000) { // arbitrary constant.
         j=rec->buf_tx_out-rec->buf_tx_in;
         if(j<0) j+=SGIP_TCP_TRANSMITBUFFERLENGTH;
         i=(int)(rec->txwindow-rec->sequence);
         if(j>i) j=i;
         i=sgIP_IP_MaxContentsSize(rec->destip)-20; // max tcp data size
         if(j>i) j=i;
         sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,i);
         rec->retrycount=0;
      }
   }
	SGIP_INTR_UNPROTECT();
   if(datalength==0) return SGIP_ERROR(EWOULDBLOCK);
	return datalength;
}
int sgIP_TCP_Recv(sgIP_Record_TCP * rec, char * databuf, int buflength, int flags) {
	if(!rec || !databuf) return SGIP_ERROR(EINVAL); //error
   if(rec->buf_rx_in==rec->buf_rx_out) {
      if(rec->tcpstate==SGIP_TCP_STATE_CLOSED || rec->tcpstate==SGIP_TCP_STATE_CLOSE_WAIT) {
         if(rec->errorcode) return SGIP_ERROR(rec->errorcode);
         return SGIP_ERROR0(ESHUTDOWN);
      }
      return SGIP_ERROR(EWOULDBLOCK); //error no data
   }
	SGIP_INTR_PROTECT();
	int rxlen = rec->buf_rx_out - rec->buf_rx_in;
	if(rxlen<0) rxlen+=SGIP_TCP_RECEIVEBUFFERLENGTH;
	if(buflength>rxlen) buflength=rxlen;
	int i,j;
	j=rec->buf_rx_in;
	for(i=0;i<buflength;i++) {
		databuf[i]=rec->buf_rx[j++];
		if(j==SGIP_TCP_RECEIVEBUFFERLENGTH) j=0;
	}

    if(!(flags&MSG_PEEK)) {
	    rec->buf_rx_in=j;

        if(rec->want_reack) {
            i=rec->buf_rx_out-rec->buf_rx_in;
            if(i<0) i+=SGIP_TCP_RECEIVEBUFFERLENGTH; // we now have the amount in the buffer
            i = SGIP_TCP_RECEIVEBUFFERLENGTH-i-1;
            if(i<0) i=0;
            if(i>SGIP_TCP_REACK_THRESH) {
                rec->want_reack=0;
                sgIP_TCP_SendPacket(rec,SGIP_TCP_FLAG_ACK,0);
            }
        }
    }
	SGIP_INTR_UNPROTECT();
	return buflength;
}
