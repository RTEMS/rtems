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

#include "sgIP_DHCP.h"
#include "sgIP_DNS.h"
#include <string.h>
#include "sys/socket.h"
#include "netinet/in.h"

extern unsigned long volatile sgIP_timems;
int dhcp_socket;
char dhcp_hostname[64];
int dhcp_tid;
unsigned long dhcp_timestart, dhcp_timelastaction;
sgIP_DHCP_Packet * dhcp_p;
sgIP_Hub_HWInterface * dhcp_int;
int dhcp_optionptr;
int dhcp_requestDNS;
int dhcp_status;
int dhcp_state; // 0== send DHCPDISCOVER wait for DHCPOFFER, 1== send DHCPREQUEST wait for DHCPACK
unsigned long dhcp_rcvd_ip, dhcp_rcvd_gateway, dhcp_rcvd_snmask, dhcp_rcvd_dns[3], dhcp_serverip;


void sgIP_DHCP_Init() {
   dhcp_socket=0;
   dhcp_p=0;
   dhcp_int=0;
   dhcp_rcvd_ip=0;
   strcpy(dhcp_hostname,SGIP_DHCP_DEFAULTHOSTNAME);
   dhcp_status=SGIP_DHCP_STATUS_IDLE;
}
void sgIP_DHCP_SetHostName(char * s) {
   strncpy(dhcp_hostname,s,63);
   dhcp_hostname[63]=0;
}
int sgIP_DHCP_IsDhcpIp(unsigned long ip) { // check if the IP address was assigned via dhcp.
	return ip==dhcp_rcvd_ip;
}

void sgIP_DHCP_SendDgram() {
   struct sockaddr_in sain;
   int len_dhcp;
   sain.sin_port=htons(67); // bootp server port
   sain.sin_addr.s_addr=0xFFFFFFFF; // broadcast
   dhcp_p->options[dhcp_optionptr++]=0xFF; // terminate options list.
   //sendto(dhcp_socket,dhcp_p,sizeof(sgIP_DHCP_Packet)-312+dhcp_optionptr,0,(struct sockaddr *)&sain,sizeof(sain));
   len_dhcp = sizeof(sgIP_DHCP_Packet)-312+dhcp_optionptr;
   if(len_dhcp<300) len_dhcp=300;
   sendto(dhcp_socket,dhcp_p,len_dhcp,0,(struct sockaddr *)&sain,sizeof(sain));
   sgIP_free(dhcp_p);
   dhcp_p=0;
   dhcp_timelastaction=sgIP_timems;
}

void sgIP_DHCP_BeginDgram(int dgramtype) {
   int i;
   if(dhcp_p) sgIP_free(dhcp_p);
   dhcp_p = (sgIP_DHCP_Packet *) sgIP_malloc(sizeof(sgIP_DHCP_Packet));
   if(!dhcp_p) return;

   // ensure packet is zero'd.. seems to pacify some routers.  malloc doesn't initialise the memory returned.
   memset(dhcp_p,0,sizeof(sgIP_DHCP_Packet));

   dhcp_p->op=1;                 // 1==BOOTREQUEST
   dhcp_p->htype=1;              // 1== ethernet address type
   dhcp_p->hlen=6;               // hardware address length
   dhcp_p->hops=0;               // client sets to zero
   dhcp_p->xid=dhcp_tid;         // DHCP transaction ID
   dhcp_p->secs=(sgIP_timems-dhcp_timestart)/1000; // seconds since DHCP start
   dhcp_p->flags=htons(0x0000);  // top bit set = request broadcast response
   dhcp_p->ciaddr=0;
   dhcp_p->yiaddr=0;
   dhcp_p->siaddr=0;
   dhcp_p->giaddr=0;

   memcpy(dhcp_p->chaddr,dhcp_int->hwaddr,6);

   dhcp_optionptr=0;
   dhcp_p->options[dhcp_optionptr++]=0x63;
   dhcp_p->options[dhcp_optionptr++]=0x82;
   dhcp_p->options[dhcp_optionptr++]=0x53;
   dhcp_p->options[dhcp_optionptr++]=0x63; // 4-byte "magic cookie" (bleh!)
   // add some necessary options... - by default add the dhcp message type, host name, class id, and parameter request list
   dhcp_p->options[dhcp_optionptr++]=0x35; // DHCP Message type
   dhcp_p->options[dhcp_optionptr++]=0x01;
   dhcp_p->options[dhcp_optionptr++]=dgramtype;

   dhcp_p->options[dhcp_optionptr++]=0x3D; // DHCP client identifier
   dhcp_p->options[dhcp_optionptr++]=0x07; // length
   dhcp_p->options[dhcp_optionptr++]=0x01; // hw type
   for(i=0;i<6;i++)    dhcp_p->options[dhcp_optionptr++]=dhcp_int->hwaddr[i];

   dhcp_p->options[dhcp_optionptr++]=0x0C; // DHCP host name
   dhcp_p->options[dhcp_optionptr++]=strlen(dhcp_hostname);
   for(i=0;i<strlen(dhcp_hostname);i++) {
      dhcp_p->options[dhcp_optionptr++]=dhcp_hostname[i];
   }

   dhcp_p->options[dhcp_optionptr++]=0x37; // DHCP Parameter request list
   dhcp_p->options[dhcp_optionptr++]=2+dhcp_requestDNS;
   dhcp_p->options[dhcp_optionptr++]=1; // subnet mask
   dhcp_p->options[dhcp_optionptr++]=3; // router
   if(dhcp_requestDNS) dhcp_p->options[dhcp_optionptr++]=6; // dns server

   if(dgramtype==DHCP_TYPE_REQUEST) {
	   dhcp_p->options[dhcp_optionptr++]=0x32; // DHCP Requested IP address
	   dhcp_p->options[dhcp_optionptr++]=0x04;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_rcvd_ip)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_rcvd_ip>>8)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_rcvd_ip>>16)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_rcvd_ip>>24)&255;

	   dhcp_p->options[dhcp_optionptr++]=0x36; // DHCP Server identifier
	   dhcp_p->options[dhcp_optionptr++]=0x04;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_serverip)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_serverip>>8)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_serverip>>16)&255;
	   dhcp_p->options[dhcp_optionptr++]=(dhcp_serverip>>24)&255;
   }

   dhcp_p->options[dhcp_optionptr++]=0x3C; // DHCP Vendor Class ID
   dhcp_p->options[dhcp_optionptr++]=strlen(SGIP_DHCP_CLASSNAME);
   for(i=0;i<strlen(SGIP_DHCP_CLASSNAME);i++) {
      dhcp_p->options[dhcp_optionptr++]=(SGIP_DHCP_CLASSNAME)[i];
   }

   // reason we don't send it immediately is in case the calling code wants to modify some data or add some options.
}



void sgIP_DHCP_Start(sgIP_Hub_HWInterface * interface, int getDNS) { // begin dhcp transaction to get IP and maybe DNS data.
   struct sockaddr_in sain;
   int i;
   SGIP_DEBUG_MESSAGE(("sgIP_DHCP_Start()"));
   sgIP_DHCP_Terminate();
   dhcp_requestDNS=getDNS?1:0;
   dhcp_int=interface;
   dhcp_timestart=sgIP_timems;
   dhcp_timelastaction=sgIP_timems;
   dhcp_tid=sgIP_timems;
   dhcp_status=SGIP_DHCP_STATUS_WORKING;
   dhcp_state=0;

   dhcp_rcvd_ip = 0;
   dhcp_rcvd_gateway=0;
   dhcp_rcvd_snmask=0;
   dhcp_rcvd_dns[0]=0;
   dhcp_rcvd_dns[1]=0;
   dhcp_rcvd_dns[2]=0;

   dhcp_socket=socket(AF_INET,SOCK_DGRAM,0);
   sain.sin_addr.s_addr=0;
   sain.sin_port=htons(68); // BOOTP client
   bind(dhcp_socket,(struct sockaddr *)&sain,sizeof(sain));
   i=1;
   ioctl(dhcp_socket,FIONBIO,&i);

   sgIP_DHCP_BeginDgram(DHCP_TYPE_DISCOVER);
   sgIP_DHCP_SendDgram();
}
void sgIP_DHCP_Release() { // call to dump our DHCP address and leave.
   if(dhcp_status==SGIP_DHCP_STATUS_WORKING) {
      sgIP_DHCP_Terminate();
   } else {
      sgIP_DHCP_BeginDgram(DHCP_TYPE_RELEASE);
      dhcp_p->ciaddr=dhcp_int->ipaddr;
      sgIP_DHCP_SendDgram();

   }
}
int  sgIP_DHCP_Update() { // MUST be called periodicly; returns status - call until it returns SGIP_DHCP_STATUS_SUCCESS or _FAILED.
	sgIP_DHCP_Packet * p;
	struct sockaddr_in * sain;
	int i,j,n,l;
	if(dhcp_status!=SGIP_DHCP_STATUS_WORKING) return dhcp_status;
	int send = 0;

	p=(sgIP_DHCP_Packet *)sgIP_malloc(sizeof(sgIP_DHCP_Packet));
	if(p) {

		while(1) {
			l=recvfrom(dhcp_socket,p,sizeof(sgIP_DHCP_Packet),0,(struct sockaddr *)&sain,&n);
			if(l==-1) break;
			if(p->op!=2 || p->htype!=1 || p->hlen!=6 || p->xid!=dhcp_tid ) continue;
			// check magic cookie
			if(p->options[0]!=0x63 || p->options[1]!=0x82 || p->options[2]!=0x53 || p->options[3]!=0x63) continue;
			i=4; // yay, the cookie is valid.
			l -= (sizeof(sgIP_DHCP_Packet)-312); // number of bytes remaining in the options
			while(i<l) {
				n=p->options[i++];
				switch(n) {
				case 0: // ignore
					break;
				case 255: // end-of-options marker.
					l=0;
					break;
				case 53: // message type, variable length, 2+n
					j=p->options[i++];
					if(dhcp_state==0) {
						if(p->options[i]!=DHCP_TYPE_OFFER) l=-1;
					} else {
						if(p->options[i]==DHCP_TYPE_ACK) {
							sgIP_free(p);
							sgIP_DHCP_Terminate();
							dhcp_int->ipaddr=dhcp_rcvd_ip;
							dhcp_int->gateway=dhcp_rcvd_gateway;
							dhcp_int->snmask=dhcp_rcvd_snmask;
							SGIP_DEBUG_MESSAGE(("DHCP Configured!"));
							SGIP_DEBUG_MESSAGE(("IP%08X SM%08X GW%08X",dhcp_rcvd_ip,dhcp_rcvd_snmask,dhcp_rcvd_gateway));
							if(dhcp_requestDNS) {
								dhcp_int->dns[0]=dhcp_rcvd_dns[0];
								dhcp_int->dns[1]=dhcp_rcvd_dns[1];
								dhcp_int->dns[2]=dhcp_rcvd_dns[2];
								SGIP_DEBUG_MESSAGE(("DNS %08X %08X %08X",dhcp_rcvd_dns[0],dhcp_rcvd_dns[1],dhcp_rcvd_dns[2]));
							}

							dhcp_status=SGIP_DHCP_STATUS_SUCCESS;
							return dhcp_status;
						} else {
							l=-1;
						}
					}
					i+=j;

					break;
				case 1: // subnet mask field, variable length 2+n
					j=p->options[i++];
					if(dhcp_state==1 || j<4) {i+=j; break; }
					dhcp_rcvd_snmask = (dhcp_rcvd_snmask>>8) | (p->options[i++]<<24);
					dhcp_rcvd_snmask = (dhcp_rcvd_snmask>>8) | (p->options[i++]<<24);
					dhcp_rcvd_snmask = (dhcp_rcvd_snmask>>8) | (p->options[i++]<<24);
					dhcp_rcvd_snmask = (dhcp_rcvd_snmask>>8) | (p->options[i++]<<24);
					i+=j-4;
					break;
				case 3: // gateway, variable length 2+n
					j=p->options[i++];
					if(dhcp_state==1 || j<4) {i+=j; break; }
					dhcp_rcvd_gateway = (dhcp_rcvd_gateway>>8) | (p->options[i++]<<24);
					dhcp_rcvd_gateway = (dhcp_rcvd_gateway>>8) | (p->options[i++]<<24);
					dhcp_rcvd_gateway = (dhcp_rcvd_gateway>>8) | (p->options[i++]<<24);
					dhcp_rcvd_gateway = (dhcp_rcvd_gateway>>8) | (p->options[i++]<<24);
					i+=j-4;
					break;
				case 54: // server ID, variable length 2+n
					j=p->options[i++];
					if(dhcp_state==1 || j<4) {i+=j; break; }
					dhcp_serverip = (dhcp_serverip>>8) | (p->options[i++]<<24);
					dhcp_serverip = (dhcp_serverip>>8) | (p->options[i++]<<24);
					dhcp_serverip = (dhcp_serverip>>8) | (p->options[i++]<<24);
					dhcp_serverip = (dhcp_serverip>>8) | (p->options[i++]<<24);
					i+=j-4;
					break;
				case 6: // dns servers, variable length 2+n
					if(dhcp_requestDNS && !dhcp_state) {
						j=p->options[i++];
						n=0;
						while(n<3 && 4*n+3<j) {
							dhcp_rcvd_dns[n] = (dhcp_rcvd_dns[n]>>8) | (p->options[i++]<<24);
							dhcp_rcvd_dns[n] = (dhcp_rcvd_dns[n]>>8) | (p->options[i++]<<24);
							dhcp_rcvd_dns[n] = (dhcp_rcvd_dns[n]>>8) | (p->options[i++]<<24);
							dhcp_rcvd_dns[n] = (dhcp_rcvd_dns[n]>>8) | (p->options[i++]<<24);
							n++;
						}
						i+=j-4*n;
						break;
					}
				default:
					j=p->options[i++];
					i+=j;
				}
			}
			if(l==-1) continue;
			dhcp_rcvd_ip=(p->yiaddr);

		 // discover succeeded.  increment transaction id.  force sending REQUEST message next.
         dhcp_state=1;
		 dhcp_tid += ( sgIP_timems-dhcp_timestart ) + 1;
		 send = 1;
         break;
		}

		sgIP_free(p);
		// has timeout expired?
		if( (sgIP_timems-dhcp_timestart) > SGIP_DHCP_ERRORTIMEOUT) {
            SGIP_DEBUG_MESSAGE(("sgIP DHCP error timeout!"));
			sgIP_DHCP_Terminate();
			dhcp_status=SGIP_DHCP_STATUS_FAILED;
			return dhcp_status;
		}
		if( send || (sgIP_timems-dhcp_timelastaction) > SGIP_DHCP_RESENDTIMEOUT )
		{
			if(dhcp_state==0) sgIP_DHCP_BeginDgram(DHCP_TYPE_DISCOVER); else sgIP_DHCP_BeginDgram(DHCP_TYPE_REQUEST);
			sgIP_DHCP_SendDgram();
		}
	} else {
        SGIP_DEBUG_MESSAGE(("sgIP DHCP alloc failed!"));
		sgIP_DHCP_Terminate();
		dhcp_status=SGIP_DHCP_STATUS_FAILED;
	}

	return dhcp_status;
}
void sgIP_DHCP_Terminate() { // kill the process where it stands; deallocate all DHCP resources.
   if(dhcp_socket) closesocket(dhcp_socket);
   dhcp_socket=0;
   if(dhcp_p) sgIP_free(dhcp_p);
   dhcp_p=0;
   dhcp_status=SGIP_DHCP_STATUS_IDLE;
}

int gethostname(char *name, size_t len)
{
    int size = sizeof(dhcp_hostname);
    if (name == NULL)
        return SGIP_ERROR(EFAULT);

    if ( len <= size )
        return SGIP_ERROR(EINVAL);

    strncpy(name, dhcp_hostname, size);
    name[size]=0;
    return 0;
}

int sethostname(const char *name, size_t len)
{
    sgIP_DNS_Record *rec;

    int size = sizeof(dhcp_hostname);
    if (name == NULL)
        return SGIP_ERROR(EFAULT);

    if ( len > size - 1)
        return SGIP_ERROR(EINVAL);

   rec = sgIP_DNS_FindDNSRecord(dhcp_hostname);

   strncpy(dhcp_hostname, name, len);
   dhcp_hostname[len]=0;
   strncpy(rec->aliases[0], name, len);
   rec->aliases[0][len]=0;
   strncpy(rec->name, name, len);
   rec->name[len]=0;

   return 0;
}






