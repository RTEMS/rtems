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

#include "sgIP_DNS.h"
#include "sgIP_Hub.h"
#include "netinet/in.h"
#include "sys/socket.h"

int   dns_sock;
int   time_count;
int   last_id;
int   query_time_start;
extern unsigned long volatile sgIP_timems;

// cache record data
sgIP_DNS_Record dnsrecords[SGIP_DNS_MAXRECORDSCACHE];

// data to return via hostent
volatile sgIP_DNS_Record  dnsrecord_return;
volatile char *           alias_list[SGIP_DNS_MAXALIASES+1];
volatile char *           addr_list[SGIP_DNS_MAXRECORDADDRS+1];
char             ipaddr_alias[256];
unsigned long    ipaddr_ip;
volatile sgIP_DNS_Hostent dnsrecord_hostent;

unsigned char querydata[512];
unsigned char responsedata[512];

void sgIP_DNS_Init() {
   int i;
   for(i=0;i<SGIP_DNS_MAXRECORDSCACHE;i++) dnsrecords[i].flags=0;
   dns_sock=-1;
   time_count=0;
}

void sgIP_DNS_Timer1000ms() {
   int i;
   time_count++;
   for(i=0;i<SGIP_DNS_MAXRECORDSCACHE;i++) {
      if(dnsrecords[i].flags & SGIP_DNS_FLAG_RESOLVED) {
         dnsrecords[i].TTL-=1;
         if(dnsrecords[i].TTL<=0) {
            dnsrecords[i].flags=0;
         }
      }
   }
}

int sgIP_DNS_isipaddress(const char * name, unsigned long * ipdest) {
	int i,j,t,ndot;
	unsigned long out_addr, g[4];
	const char * c;

	ndot=0;
	c=name;
	while(*c) { // scan for invalid characters
		if(!((*c>='0' && *c<='9') || (*c>='A' && *c<='F') || (*c>='a' && *c<='f') || *c=='.' || *c=='x' || *c=='X')) return 0;
		if(*c=='.') ndot++;
		c++;
	}
	if(ndot>3) return 0;
	c=name;
	for(i=0;i<=ndot;i++) {
		g[i]=0;
		t=0;
		j=0;
		while(*c && *c!='.') {
			if(j==0 && *c=='0') { t++; }
			else if(j==1 && t==1 && (*c=='x' || *c=='X')) { t++; }
			else {
				switch(t) {
				case 0: // decimal
					if(*c=='x' || *c=='X' || (*c>='A' && *c<='F') || (*c>='a' && *c<='f')) return 0;
					g[i]=(g[i]*10)+(*c-'0');
					break;
				case 1: // octal
					if(*c=='x' || *c=='X' || (*c>='A' && *c<='F') || (*c>='a' && *c<='f') || *c=='8' || *c=='9') return 0;
					g[i]=(g[i]<<3)+(*c-'0');
					break;
				case 2: // hex
					if(*c=='x' || *c=='X') return 0;
					if(*c>='0' && *c<='9') {
						g[i]=(g[i]<<4)+(*c-'0');
					} else {
						g[i]=(g[i]<<4)+(*c&0xDF)+9;
					}
					break;
				}
			}
			j++; c++;
		}
		if(*c) c++; else break;
	}
	out_addr=0;
	switch(ndot) {
	case 0:
		out_addr=g[0];
		break;
	case 1:
		if(g[0]>=0x100 || g[1]>=0x1000000) return 0;
		out_addr= (g[0]<<24) | g[1];
		break;
	case 2:
		if(g[0]>=0x100 || g[1]>=0x100 || g[2]>=0x10000) return 0;
		out_addr= (g[0]<<24) | (g[1]<<16) | g[2];
		break;
	case 3:
		if(g[0]>=0x100 || g[1]>=0x100 || g[2]>=0x100 || g[3]>=0x100) return 0;
		out_addr= (g[0]<<24) | (g[1]<<16) | (g[2]<<8) | g[3];
		break;
	}
	*ipdest=htonl(out_addr);
	return 1;
}

sgIP_DNS_Record * sgIP_DNS_FindDNSRecord(const char * name) {
   int i,j,k,n,c,c2;
   SGIP_INTR_PROTECT();
   for(i=0;i<SGIP_DNS_MAXRECORDSCACHE;i++) {
      if((dnsrecords[i].flags&(SGIP_DNS_FLAG_ACTIVE|SGIP_DNS_FLAG_RESOLVED)) == (SGIP_DNS_FLAG_ACTIVE|SGIP_DNS_FLAG_RESOLVED)) {
         for(j=0;j<dnsrecords[i].numalias;j++) {
            k=0;
            for(n=0;name[n] && dnsrecords[i].aliases[j][n]; n++) { // obscure and complex case-insensitive string compare.
               c=name[n];
               c2=dnsrecords[i].aliases[j][n];
               if(c>='a' && c<='z') c+='A'-'a';
               if(c2>='a' && c2<='z') c2+='A'-'a';
               if(c==c2) {
                  k++;
               } else {
                  k=0; break;
               }
            }
            if(name[n] || dnsrecords[i].aliases[j][n]) k=0;
            if(k) {
               SGIP_INTR_UNPROTECT();
               return dnsrecords+i;
            }
         }
      }
   }

   SGIP_INTR_UNPROTECT();
   return 0;
}

sgIP_DNS_Record * sgIP_DNS_GetUnusedRecord() {
   int i,j,minttl;
   SGIP_INTR_PROTECT();
   for(i=0;i<SGIP_DNS_MAXRECORDSCACHE;i++) {
      if(!(dnsrecords[i].flags&SGIP_DNS_FLAG_ACTIVE))  {
         SGIP_INTR_UNPROTECT();
         return dnsrecords+i;
      }
   }
   minttl=dnsrecords[0].TTL; j=0;
   for(i=1;i<SGIP_DNS_MAXRECORDSCACHE;i++) {
      if(dnsrecords[i].TTL<minttl && !(dnsrecords[i].flags&SGIP_DNS_FLAG_BUSY)) {
         j=i;
         minttl=dnsrecords[i].TTL;
      }
   }
   dnsrecords[j].flags=0;
   SGIP_INTR_UNPROTECT();
   return dnsrecords+j;
}

static
void sgIP_ntoa(unsigned long ipaddr) {
   int c,i,j,n;
   c=0;
   for(j=0;j<4;j++) {
      if(j) ipaddr_alias[c++]='.';
      n=(ipaddr>>(j*8))&255;
      i=0;
      if(n>=100) { i=n/100; ipaddr_alias[c++]='0'+i; n-=i*100; }
      if(n>=10 || i) { i=n/10; ipaddr_alias[c++]='0'+i; n-=i*10; }
      ipaddr_alias[c++]='0'+n;
   }
   ipaddr_alias[c]=0;
}

static
sgIP_DNS_Hostent * sgIP_DNS_GenerateHostentIP(unsigned long ipaddr) {

   sgIP_ntoa(ipaddr);

   alias_list[0]=ipaddr_alias;
   alias_list[1]=0;
   ipaddr_ip=ipaddr;
   addr_list[0]=(char *)&ipaddr_ip;
   addr_list[1]=0;

   dnsrecord_hostent.h_addr_list=(char **)addr_list;
   dnsrecord_hostent.h_addrtype=1;
   dnsrecord_hostent.h_aliases=(char **)alias_list;
   dnsrecord_hostent.h_length=4;
   dnsrecord_hostent.h_name=ipaddr_alias;
   return (sgIP_DNS_Hostent *)&dnsrecord_hostent;
}

sgIP_DNS_Hostent * sgIP_DNS_GenerateHostent(sgIP_DNS_Record * dnsrec) {
   volatile int i;
   dnsrecord_return = *dnsrec; // copy struct
   for(i=0;i<dnsrecord_return.numalias;i++) {
      alias_list[i]=dnsrecord_return.aliases[i];
   }
   alias_list[i]=0;
   for(i=0;i<dnsrecord_return.numaddr;i++) {
      addr_list[i]=(char *)&(dnsrecord_return.addrdata[i*dnsrecord_return.addrlen]);
   }
   addr_list[i]=0;
   dnsrecord_hostent.h_addr_list=(char **)addr_list;
   dnsrecord_hostent.h_addrtype=dnsrecord_return.addrclass;
   dnsrecord_hostent.h_aliases=(char **)alias_list;
   dnsrecord_hostent.h_length=dnsrecord_return.addrlen;
   dnsrecord_hostent.h_name=(char *)dnsrecord_return.name;
   return (sgIP_DNS_Hostent *)&dnsrecord_hostent;
}

static
int sgIP_DNS_genquery(const char * name) {
   int i,j,c,l;
   unsigned short * querydata_s = (unsigned short *) querydata;
   unsigned char * querydata_c = querydata;
   // header section
   querydata_s[0]=htons(time_count&0xFFFF);
   last_id=querydata_s[0];
   querydata_s[1]=htons(0x0100); // recursion desired, standard query
   querydata_s[2]=htons(1); // one QD (question)
   querydata_s[3]=0; // no resource records
   querydata_s[4]=0; // no nameserver records
   querydata_s[5]=0; // no additional records
   // question section

   querydata_c+=12;
   querydata_s+=6;
   j=0;
   i=0;
   while(1) {
      l=j;
      j++;
      c=0;
      while(name[i]!=0 && name[i]!='.' && i<255) {
         querydata_c[j++]=name[i++]; c++;
      }
      querydata_c[l]=c;
      if(name[i]==0 || i>=255) break;
      if(c==0) return 0; // this should never happen (unless there's really invalid input with 2 dots next to each other.)
      i++;
   }
   querydata_c[j++]=0; // terminating zero length
   // qtype
   querydata_c[j++]=0;
   querydata_c[j++]=1; // 00 01 "A" (address)
   // qclass
   querydata_c[j++]=0;
   querydata_c[j++]=1; // 00 01 "IN" (internet)

   return j+12; // length
}

void sgIP_DNS_CopyAliasAt(char * deststr,int offset) {
   char * c;
   int i,j;
   i=0;
   c=(char *)responsedata+offset;
   do {
      j=c[0];
      if(j>63) {
         j=((j&63)<<8) | c[1];
         c=(char *)responsedata+j;
         continue;
      }
      if(!j) break;
      c++;
      for(;j>0;j--) {
         deststr[i++]= *(c++);
      }
      deststr[i++]='.';
   } while(1);
   if(i>0) i--;
   deststr[i]=0;
}


sgIP_DNS_Hostent * sgIP_DNS_gethostbyname(const char * name) {
   sgIP_DNS_Record * rec;
   sgIP_DNS_Hostent * he;
   sgIP_Hub_HWInterface * hw;
   int len,i,sainlen;
   int retries,dtime;
   unsigned long serverip;
   struct sockaddr_in sain;
   unsigned long IP;
   SGIP_INTR_PROTECT();

   // is name an IP address?
   if(sgIP_DNS_isipaddress(name,&IP)) {
      SGIP_INTR_UNPROTECT();
      return sgIP_DNS_GenerateHostentIP(IP);
   }

   // check cache, return if value required is in cache...
   rec=sgIP_DNS_FindDNSRecord(name);
   if(rec) {
      he=sgIP_DNS_GenerateHostent(rec);
      SGIP_INTR_UNPROTECT();
      return he;
   }

   // not in cache? generate a query...
   len=sgIP_DNS_genquery(name);

   // send off the query, handle retransmit and trying other dns servers.
   if(dns_sock==-1) {
      hw=sgIP_Hub_GetDefaultInterface();
      serverip=hw->dns[0];

      dns_sock=socket(AF_INET,SOCK_DGRAM,0);
      i=1;
      i=ioctl(dns_sock,FIONBIO,&i); // set non-blocking

      retries=0;
      do {
         query_time_start=sgIP_timems;
         sain.sin_addr.s_addr=serverip;
         sain.sin_port=htons(53);
         i=sendto(dns_sock,querydata,len,0,(struct sockaddr *)&sain,sizeof(sain));
dns_listenonly:

         do {
            i=recvfrom(dns_sock,responsedata,512,0,(struct sockaddr *)&sain,&sainlen);
            if(i!=-1) break;
            dtime=sgIP_timems-query_time_start;
            if(dtime>SGIP_DNS_TIMEOUTMS) break;
            SGIP_INTR_UNPROTECT();
            SGIP_WAITEVENT();
            SGIP_INTR_REPROTECT();
         } while(1);

         if(i==-1) { // no reply, retry
            retries++;
            if(retries>=SGIP_DNS_MAXRETRY) { // maybe try another server? for now just quit.
               closesocket(dns_sock);
               dns_sock=-1;
               SGIP_INTR_UNPROTECT();

               return NULL;
            }
            continue; // send again
         }

         // got something, is it what we want?
         if(i<12 || sain.sin_addr.s_addr!=serverip || sain.sin_port!=htons(53)) { // suspicious.
            goto dns_listenonly; // yay! a goto! - go back and see if we can get a more official response.
         }

         // parse response.
         {
            const unsigned short * resdata_s = (unsigned short *) responsedata;
            const unsigned char * resdata_c = responsedata;
            const char * c;
            int j,q,a, nalias,naddr;
            if(last_id!=resdata_s[0]) { // bad.
               goto dns_listenonly;
            }
            q=htons(resdata_s[2]);
            a=htons(resdata_s[3]);
            // no answer.
            if (a == 0)
            {
               closesocket(dns_sock);
               dns_sock=-1;
               SGIP_INTR_UNPROTECT();

               return NULL;
            }

            resdata_c+=12;
            while(q) { // ignore questions
               do {
                  j=resdata_c[0];
                  if(j>63) { resdata_c+=2; break; }
                  resdata_c += j+1;
               } while(j);
               resdata_c+=4;
               q--;
            }

            nalias=0;
            naddr=0;
            rec=sgIP_DNS_GetUnusedRecord();
            rec->flags=SGIP_DNS_FLAG_ACTIVE | SGIP_DNS_FLAG_BUSY;
            while(a) {
               if(nalias<SGIP_DNS_MAXALIASES) sgIP_DNS_CopyAliasAt(rec->aliases[nalias++],resdata_c-responsedata);
               do {
                  j=resdata_c[0];
                  if(j>63) { resdata_c+=2; break; }
                  resdata_c += j+1;
               } while(j);
               // CNAME=5, A=1
               j=resdata_c[1];
               rec->addrclass=(resdata_c[2]<<8)|resdata_c[3];
               rec->TTL = (resdata_c[4]<<24)|(resdata_c[5]<<16)|(resdata_c[6]<<8)|resdata_c[7];
               if(j==1) { // A
                  if(naddr<SGIP_DNS_MAXRECORDADDRS) {
                     rec->addrdata[naddr*4] = resdata_c[10];
                     rec->addrdata[naddr*4+1] = resdata_c[11];
                     rec->addrdata[naddr*4+2] = resdata_c[12];
                     rec->addrdata[naddr*4+3] = resdata_c[13];
                     naddr++;
                  }
               }
               j=(resdata_c[8]<<8)|resdata_c[9];
               resdata_c+=10+j;
               a--;
            }

            // likely we have all the data we care for now.
            rec->addrlen=4;
            rec->numaddr=naddr;
            rec->numalias=nalias;
            for(c=name,i=0;*c;c++,i++) rec->name[i]=*c;
            rec->name[i]=0;
            rec->flags=SGIP_DNS_FLAG_ACTIVE | SGIP_DNS_FLAG_RESOLVED;
            break; // we got our answer, let's get out of here!
         }
      } while(1);

      closesocket(dns_sock);
      dns_sock=-1;
   } else {
      SGIP_INTR_UNPROTECT();
      return NULL;
   }

   // received response, return data
   he=sgIP_DNS_GenerateHostent(rec);
   SGIP_INTR_UNPROTECT();
   return he;
}

unsigned long inet_addr(const char *cp) {
	unsigned long IP;
	if(sgIP_DNS_isipaddress(cp,&IP)) {
		return IP;
	}
	return 0xFFFFFFFF;
}

int inet_aton(const char *cp, struct in_addr *inp) {
	unsigned long IP;

	if(sgIP_DNS_isipaddress(cp,&IP)) {
		inp->s_addr = IP;
		return 1;
	}

	return 0;
}


char *inet_ntoa(struct in_addr in) {
	sgIP_ntoa(in.s_addr);
	return (char *)ipaddr_alias;
}



