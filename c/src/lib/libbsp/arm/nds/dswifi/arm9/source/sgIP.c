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

#include "sgIP.h"

unsigned long volatile sgIP_timems;
int sgIP_errno;

// sgIP_Init(): Initializes sgIP hub and sets up a default surrounding interface (ARP and IP)
void sgIP_Init() {
   sgIP_timems = 0;
	sgIP_memblock_Init();
	sgIP_Hub_Init();
	sgIP_ARP_Init();
	sgIP_TCP_Init();
	sgIP_UDP_Init();
   sgIP_DNS_Init();
   sgIP_DHCP_Init();
	sgIP_Hub_AddProtocolInterface(PROTOCOL_ETHER_IP,&sgIP_IP_ReceivePacket,0);
}


unsigned long count_100ms;
unsigned long count_1000ms;
void sgIP_Timer(int num_ms) {
   sgIP_timems+=num_ms;
   count_100ms+=num_ms;
   if(count_100ms>=100) {
      count_100ms-=100;
      if(count_100ms>=100) count_100ms=0;
      sgIP_ARP_Timer100ms();
   }
   count_1000ms+=num_ms;
   if(count_1000ms>=1000) {
      count_1000ms-=1000;
      if(count_1000ms>=1000) count_1000ms=0;
      sgIP_DNS_Timer1000ms();
   }
   sgIP_TCP_Timer();
}

