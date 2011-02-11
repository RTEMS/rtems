// DS Wifi interface code
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
// wifi_arm9.c - arm9 wifi support code
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

#include <unistd.h>

#include <nds.h>
#include "dsregs.h"

#include "wifi_arm9.h"
#include <stdarg.h>
#include <stdlib.h>
#include <sys/socket.h>









#ifdef WIFI_USE_TCP_SGIP

#include "sgIP.h"


sgIP_Hub_HWInterface * wifi_hw;


int sgIP_DisableInterrupts() {
	int a;
	a=REG_IME;
	REG_IME=0;
	return a;
}
void sgIP_RestoreInterrupts(int old_ime) {
	REG_IME=old_ime;
}

void sgIP_IntrWaitEvent() {
 //  __asm__ ( ".ARM\n swi 0x060000\n" );
	int i,j;
	j=0;
	for(i=0;i<20000;i++) {
		j+=i;
	}
}

void * sgIP_malloc(int size) __attribute__((weak));
void sgIP_free(void * ptr) __attribute__((weak));



//////////////////////////////////////////////////////////////////////////
// wifi heap allocator system

#define WHEAP_RECORD_FLAG_INUSE     0
#define WHEAP_RECORD_FLAG_UNUSED    1
#define WHEAP_RECORD_FLAG_FREED     2

typedef struct WHEAP_RECORD {
    struct WHEAP_RECORD * next;
    unsigned short flags, unused;
    int size;
} wHeapRecord;

#ifdef SGIP_DEBUG
#define WHEAP_FILL_START    0xAA
#define WHEAP_FILL_END      0xBB
#define WHEAP_PAD_START     4
#define WHEAP_PAD_END       4
#define WHEAP_DO_PAD
#else
#define WHEAP_PAD_START     0
#define WHEAP_PAD_END       0
#undef WHEAP_DO_PAD
#endif
#define WHEAP_RECORD_SIZE   (sizeof(wHeapRecord))
#define WHEAP_PAD_SIZE      ((WHEAP_PAD_START)+(WHEAP_PAD_END))
#define WHEAP_SIZE_CUTOFF   ((WHEAP_RECORD_SIZE)+64)


int wHeapsize;
wHeapRecord * wHeapStart; // start of heap
wHeapRecord * wHeapFirst; // first free block
void wHeapAllocInit(int size) {
    wHeapStart=(wHeapRecord *)malloc(size);
    wHeapFirst=wHeapStart;
    wHeapStart->flags=WHEAP_RECORD_FLAG_UNUSED;
    wHeapStart->next=0;
    wHeapStart->size=size-sizeof(wHeapRecord);
}


void * wHeapAlloc(int size) {
    wHeapRecord * rec = wHeapFirst;
    void * voidptr;
    int n;
    size=(size+3)&(~3);
    if(size==0) size=4;
    size+=WHEAP_PAD_SIZE;
    if(!rec) { SGIP_DEBUG_MESSAGE(("wHeapAlloc: heap full!")); return 0; } // should not happen given normal use.
    while(rec->size<size) {
        if(!rec->next) { SGIP_DEBUG_MESSAGE(("wHeapAlloc: heap too full!")); return 0; } // cannot alloc
        if(rec->next->flags!=WHEAP_RECORD_FLAG_INUSE) { // try to merge with next one
            rec->size+=rec->next->size+WHEAP_RECORD_SIZE;
            rec->next=rec->next->next;
        } else { // skip ahead to more friendly waters
            rec=rec->next;
            while(rec->next) {
                if(rec->flags!=WHEAP_RECORD_FLAG_INUSE) break;
                rec=rec->next;
            }
            if(rec->flags==WHEAP_RECORD_FLAG_INUSE) { SGIP_DEBUG_MESSAGE(("wHeapAlloc: heap too full!")); return 0; } // no empty slots :(
        }
    }
    rec->flags=WHEAP_RECORD_FLAG_INUSE;
    n=rec->size-size;
    voidptr = ((char *)rec)+WHEAP_RECORD_SIZE+WHEAP_PAD_START;
    if(n<WHEAP_SIZE_CUTOFF) { // pad to include unused portion
        rec->unused=n;
    } else { // chop block into 2
        wHeapRecord * rec2;
        rec2=(wHeapRecord *)(((char *)rec)+WHEAP_RECORD_SIZE+size);
        rec2->flags=WHEAP_RECORD_FLAG_UNUSED;
        rec2->size=rec->size-size-WHEAP_RECORD_SIZE;
        rec->size=size;
        rec2->next=rec->next;
        rec->next=rec2;
        rec->unused=0;
    }
    if(rec==wHeapFirst) {
        while(wHeapFirst->next && wHeapFirst->flags==WHEAP_RECORD_FLAG_INUSE) wHeapFirst=wHeapFirst->next;
        if(wHeapFirst->flags==WHEAP_RECORD_FLAG_INUSE) wHeapFirst=0;
    }
#ifdef WHEAP_DO_PAD
    {
        int i;
        for(i=0;i<WHEAP_PAD_START;i++) {
            (((unsigned char *)rec)+WHEAP_RECORD_SIZE)[i]=WHEAP_FILL_START;
        }
        for(i=0;i<WHEAP_PAD_END;i++) {
            (((unsigned char *)rec)+WHEAP_RECORD_SIZE+size-WHEAP_PAD_END)[i]=WHEAP_FILL_END;
        }
    }
#endif
    return voidptr;
}

void wHeapFree(void * data) {
    wHeapRecord * rec = (wHeapRecord *)(((char *)data)-WHEAP_RECORD_SIZE-WHEAP_PAD_START);
#ifdef WHEAP_DO_PAD
    {
        int size=rec->size-rec->unused;
        int i;
        for(i=0;i<WHEAP_PAD_START;i++) {
            if((((unsigned char *)rec)+WHEAP_RECORD_SIZE)[i]!=WHEAP_FILL_START) break;
        }
        if(i!=WHEAP_PAD_START) { // note heap error
            SGIP_DEBUG_MESSAGE(("wHeapFree: Corruption found before allocated data! 0x%X",data));
        }
        for(i=0;i<WHEAP_PAD_END;i++) {
            if((((unsigned char *)rec)+WHEAP_RECORD_SIZE+size-WHEAP_PAD_END)[i]!=WHEAP_FILL_END) break;
        }
        if(i!=WHEAP_PAD_END) { // note heap error
            SGIP_DEBUG_MESSAGE(("wHeapFree: Corruption found after allocated data! 0x%x",data));
        }
    }
#endif
    if(rec->flags!=WHEAP_RECORD_FLAG_INUSE) { // note heap error
        SGIP_DEBUG_MESSAGE(("wHeapFree: Data already freed! 0x%X",data));
    }
    rec->flags=WHEAP_RECORD_FLAG_FREED;
    if(rec<wHeapFirst || !wHeapFirst) wHeapFirst=rec; // reposition the "starting" pointer.
}


//////////////////////////////////////////////////////////////////////////





void * sgIP_malloc(int size) { return wHeapAlloc(size); }
void sgIP_free(void * ptr) { wHeapFree(ptr); }


#endif





void ethhdr_print(char f, void * d) {
	char buffer[33];
	int i;
	int t,c;
	buffer[0]=f;
	buffer[1]=':';
	buffer[14]=' ';
	buffer[27]=' ';
	buffer[32]=0;
	for(i=0;i<6;i++) {
		t=((u8 *)d)[i];
		c=t&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[3+i*2]=c;
		c=(t>>4)&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[2+i*2]=c;

		t=((u8 *)d)[i+6];
		c=t&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[16+i*2]=c;
		c=(t>>4)&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[15+i*2]=c;
	}
	for(i=0;i<2;i++) {
		t=((u8 *)d)[i+12];
		c=t&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[29+i*2]=c;
		c=(t>>4)&15;
		if(c>9) c+='A'-10; else c+='0';
		buffer[28+i*2]=c;
	}
	SGIP_DEBUG_MESSAGE((buffer));
}




Wifi_MainStruct Wifi_Data_Struct;

volatile Wifi_MainStruct * WifiData = 0;

WifiPacketHandler packethandler = 0;
WifiSyncHandler synchandler = 0;

void erasemem(void * mem, int length) {
	int i;
	char * m = (char *)mem;
	for(i=0;i<length;i++)
		m[i]=0;
}

void Wifi_CopyMacAddr(volatile void * dest, volatile void * src) {
	((u16 *)dest)[0]=((u16 *)src)[0];
	((u16 *)dest)[1]=((u16 *)src)[1];
	((u16 *)dest)[2]=((u16 *)src)[2];
}

int Wifi_CmpMacAddr(volatile void * mac1,volatile  void * mac2) {
	return (((u16 *)mac1)[0]==((u16 *)mac2)[0]) && (((u16 *)mac1)[1]==((u16 *)mac2)[1]) && (((u16 *)mac1)[2]==((u16 *)mac2)[2]);
}



u32 Wifi_TxBufferWordsAvailable(void) {
	s32 size=WifiData->txbufIn-WifiData->txbufOut-1;
	if(size<0) size += WIFI_TXBUFFER_SIZE/2;
	return size;
}
void Wifi_TxBufferWrite(s32 start, s32 len, u16 * data) {
	int writelen;
	while(len>0) {
		writelen=len;
		if(writelen>(WIFI_TXBUFFER_SIZE/2)-start) writelen=(WIFI_TXBUFFER_SIZE/2)-start;
		len-=writelen;
		while(writelen) {
			WifiData->txbufData[start++]=*(data++);
			writelen--;
		}
		start=0;
	}
}

int Wifi_RxRawReadPacket(s32 packetID, s32 readlength, u16 * data) {
	int readlen,read_data;
	readlength= (readlength+1)/2;
	read_data=0;
	while(readlength>0) {
		readlen=readlength;
		if(readlen>(WIFI_RXBUFFER_SIZE/2)-packetID) readlen=(WIFI_RXBUFFER_SIZE/2)-packetID;
		readlength-=readlen;
		read_data+=readlen;
		while(readlen>0) {
			*(data++) = WifiData->rxbufData[packetID++];
			readlen--;
		}
		packetID=0;
	}
	return read_data;
}

u16 Wifi_RxReadOffset(s32 base, s32 offset) {
	base+=offset;
	if(base>=(WIFI_RXBUFFER_SIZE/2)) base -= (WIFI_RXBUFFER_SIZE/2);
	return WifiData->rxbufData[base];
}

// datalen = size of packet from beginning of 802.11 header to end, but not including CRC.
int Wifi_RawTxFrame(u16 datalen, u16 rate, u16 * data) {
	Wifi_TxHeader txh;
	int sizeneeded;
	int base;
	sizeneeded=((datalen+12+4+3)/4)*2;
	if(sizeneeded>Wifi_TxBufferWordsAvailable()) {WifiData->stats[WSTAT_TXQUEUEDREJECTED]++; return -1; }
	txh.tx_rate=rate;
	txh.tx_length=datalen+4;
	base = WifiData->txbufOut;
	Wifi_TxBufferWrite(base,6,(u16 *)&txh);
	base += 6;
	if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;
	Wifi_TxBufferWrite(base,((datalen+3)/4)*2,data);
	base += ((datalen+3)/4)*2;
	if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;
	WifiData->txbufOut=base;
	WifiData->stats[WSTAT_TXQUEUEDPACKETS]++;
	WifiData->stats[WSTAT_TXQUEUEDBYTES]+=sizeneeded;
   if(synchandler) synchandler();
   return 0;
}


void Wifi_RawSetPacketHandler(WifiPacketHandler wphfunc) {
	packethandler=wphfunc;
}
void Wifi_SetSyncHandler(WifiSyncHandler wshfunc) {
   synchandler=wshfunc;
}

void Wifi_DisableWifi(void) {
	WifiData->reqMode=WIFIMODE_DISABLED;
	WifiData->reqReqFlags &= ~WFLAG_REQ_APCONNECT;
}
void Wifi_EnableWifi(void) {
	WifiData->reqMode=WIFIMODE_NORMAL;
	WifiData->reqReqFlags &= ~WFLAG_REQ_APCONNECT;
}
void Wifi_SetPromiscuousMode(int enable) {
	if(enable) WifiData->reqReqFlags |= WFLAG_REQ_PROMISC;
	else WifiData->reqReqFlags &= ~WFLAG_REQ_PROMISC;
}

void Wifi_ScanMode(void) {
	WifiData->reqMode=WIFIMODE_SCAN;
	WifiData->reqReqFlags &= ~WFLAG_REQ_APCONNECT;
}
void Wifi_SetChannel(int channel) {
	if(channel<1 || channel>13) return;
	if(WifiData->reqMode==WIFIMODE_NORMAL || WifiData->reqMode==WIFIMODE_SCAN) {
		WifiData->reqChannel=channel;
	}
}


int Wifi_GetNumAP(void) {
	int i,j;
	j=0;
	for(i=0;i<WIFI_MAX_AP;i++) if(WifiData->aplist[i].flags&WFLAG_APDATA_ACTIVE) j++;
	return j;
}

int Wifi_GetAPData(int apnum, Wifi_AccessPoint * apdata) {
	int i,j;
	if(!apdata) return WIFI_RETURN_PARAMERROR;
	j=0;
	for(i=0;i<WIFI_MAX_AP;i++){
		if(WifiData->aplist[i].flags&WFLAG_APDATA_ACTIVE) {
			if(j==apnum) {
				while(Spinlock_Acquire(WifiData->aplist[i])!=SPINLOCK_OK);
				{
					// additionally calculate average RSSI here
					WifiData->aplist[i].rssi=0;
					for(j=0;j<8;j++) {
						WifiData->aplist[i].rssi+=WifiData->aplist[i].rssi_past[j];
					}
					WifiData->aplist[i].rssi = WifiData->aplist[i].rssi >> 3;
					*apdata = WifiData->aplist[i]; // yay for struct copy!
					Spinlock_Release(WifiData->aplist[i]);
					return WIFI_RETURN_OK;
				}
			}
			j++;
		}
	}
	return WIFI_RETURN_ERROR;
}

int Wifi_FindMatchingAP(int numaps, Wifi_AccessPoint * apdata, Wifi_AccessPoint * match_dest) {
	int ap_match,i,j,n;
	Wifi_AccessPoint ap;
	u16 macaddrzero[3] = {0,0,0}; // check for empty mac addr
	ap_match=-1;
	for(i=0;i<Wifi_GetNumAP();i++){
		Wifi_GetAPData(i,&ap);
		for(j=0;j<numaps;j++) {
			if(apdata[j].ssid_len>32 || ((signed char)apdata[j].ssid_len)<0) continue;
			if(apdata[j].ssid_len>0) { // compare SSIDs
				if(apdata[j].ssid_len!=ap.ssid_len) continue;
				for(n=0;n<apdata[j].ssid_len;n++) {
					if(apdata[j].ssid[n]!=ap.ssid[n]) break;
				}
				if(n!=apdata[j].ssid_len) continue;
			}
			if(!Wifi_CmpMacAddr(apdata[j].macaddr,macaddrzero)) { // compare mac addr
				if(!Wifi_CmpMacAddr(apdata[j].macaddr,ap.macaddr)) continue;
			}
			if(apdata[j].channel!=0) { // compare channels
				if(apdata[j].channel!=ap.channel) continue;
			}
			if(j<ap_match || ap_match==-1) {
				ap_match=j;
				if(match_dest) *match_dest = ap;
			}
			if(ap_match==0) return ap_match;
		}
	}
	return ap_match;
}

int wifi_connect_state = 0; // -1==error, 0==searching, 1==associating, 2==dhcp'ing, 3==done, 4=searching wfc data
Wifi_AccessPoint wifi_connect_point;
int Wifi_ConnectAP(Wifi_AccessPoint * apdata, int wepmode, int wepkeyid, u8 * wepkey) {
	int i;
	Wifi_AccessPoint ap;
	wifi_connect_state=-1;
	if(!apdata) return -1;
	if(((signed char)apdata->ssid_len)<0 || apdata->ssid_len>32) return -1;

	Wifi_DisconnectAP();

	wifi_connect_state=0;
	WifiData->wepmode9=wepmode; // copy data
	WifiData->wepkeyid9=wepkeyid;
	for(i=0;i<20;i++) {
		WifiData->wepkey9[i]=wepkey[i];
	}


	i=Wifi_FindMatchingAP(1,apdata,&ap);
	if(i==0) {
		Wifi_CopyMacAddr(WifiData->bssid9, ap.bssid);
		Wifi_CopyMacAddr(WifiData->apmac9, ap.bssid);
		WifiData->ssid9[0]=ap.ssid_len;
		for(i=0;i<32;i++) {
			WifiData->ssid9[i+1]=ap.ssid[i];
		}
		WifiData->apchannel9=ap.channel;
		for(i=0;i<16;i++) WifiData->baserates9[i]=ap.base_rates[i];
		WifiData->reqMode=WIFIMODE_NORMAL;
		WifiData->reqReqFlags |= WFLAG_REQ_APCONNECT | WFLAG_REQ_APCOPYVALUES;
		wifi_connect_state=1;
	} else {
		WifiData->reqMode=WIFIMODE_SCAN;
		wifi_connect_point = *apdata;
	}
	return 0;
}
void Wifi_AutoConnect(void) {
	if(!(WifiData->wfc_enable[0]&0x80)) {
		wifi_connect_state=ASSOCSTATUS_CANNOTCONNECT;
	} else {
		wifi_connect_state=4;
		WifiData->reqMode=WIFIMODE_SCAN;
	}
}

static
void sgIP_DNS_Record_Localhost(void)
{
    sgIP_DNS_Record *rec;
    const unsigned char * resdata_c = (unsigned char *)&(wifi_hw->ipaddr);
    rec = sgIP_DNS_GetUnusedRecord();
    rec->flags=SGIP_DNS_FLAG_ACTIVE | SGIP_DNS_FLAG_BUSY;

    rec->addrlen    = 4;
    rec->numalias   = 1;
    gethostname(rec->aliases[0], 256);
    gethostname(rec->name, 256);
    rec->numaddr    = 1;
    rec->addrdata[0] = resdata_c[0];
    rec->addrdata[1] = resdata_c[1];
    rec->addrdata[2] = resdata_c[2];
    rec->addrdata[3] = resdata_c[3];
    rec->addrclass = AF_INET;
    rec->TTL = 0;

    rec->flags=SGIP_DNS_FLAG_ACTIVE | SGIP_DNS_FLAG_BUSY|SGIP_DNS_FLAG_RESOLVED;
}

int Wifi_AssocStatus(void) {
	switch(wifi_connect_state) {
		case -1: // error
			return ASSOCSTATUS_CANNOTCONNECT;
		case 0: // searching
			{
				int i;
				Wifi_AccessPoint ap;
				i=Wifi_FindMatchingAP(1,&wifi_connect_point,&ap);
				if(i==0) {
					Wifi_CopyMacAddr(WifiData->bssid9, ap.bssid);
					Wifi_CopyMacAddr(WifiData->apmac9, ap.bssid);
					WifiData->ssid9[0]=ap.ssid_len;
					for(i=0;i<32;i++) {
						WifiData->ssid9[i+1]=ap.ssid[i];
					}
					WifiData->apchannel9=ap.channel;
					for(i=0;i<16;i++) WifiData->baserates9[i]=ap.base_rates[i];
					WifiData->reqMode=WIFIMODE_NORMAL;
					WifiData->reqReqFlags |= WFLAG_REQ_APCONNECT | WFLAG_REQ_APCOPYVALUES;
					wifi_connect_state=1;
				}
			}
			return ASSOCSTATUS_SEARCHING;
		case 1: // associating
			switch(WifiData->curMode) {
			case WIFIMODE_DISABLED:
			case WIFIMODE_NORMAL:
			case WIFIMODE_DISASSOCIATE:
				return ASSOCSTATUS_DISCONNECTED;
            case WIFIMODE_SCAN:
                if(WifiData->reqReqFlags&WFLAG_REQ_APCONNECT) return ASSOCSTATUS_AUTHENTICATING;
                return ASSOCSTATUS_DISCONNECTED;
			case WIFIMODE_ASSOCIATE:
				switch(WifiData->authlevel) {
				case WIFI_AUTHLEVEL_DISCONNECTED:
					return ASSOCSTATUS_AUTHENTICATING;
				case WIFI_AUTHLEVEL_AUTHENTICATED:
				case WIFI_AUTHLEVEL_DEASSOCIATED:
					return ASSOCSTATUS_ASSOCIATING;
				case WIFI_AUTHLEVEL_ASSOCIATED:
#ifdef WIFI_USE_TCP_SGIP
					if(wifi_hw) {
						if(!(wifi_hw->ipaddr)) {
							sgIP_DHCP_Start(wifi_hw,wifi_hw->dns[0]==0);
							wifi_connect_state=2;
							return ASSOCSTATUS_ACQUIRINGDHCP;
						}
					}
					sgIP_ARP_SendGratARP(wifi_hw);
#endif
					wifi_connect_state=3;
					WifiData->flags9|=WFLAG_ARM9_NETREADY;
					return ASSOCSTATUS_ASSOCIATED;
				}
				break;
			case WIFIMODE_ASSOCIATED:
#ifdef WIFI_USE_TCP_SGIP
				if(wifi_hw) {
					if(!(wifi_hw->ipaddr)) {
						sgIP_DHCP_Start(wifi_hw,wifi_hw->dns[0]==0);
						wifi_connect_state=2;
						return ASSOCSTATUS_ACQUIRINGDHCP;
					}
				}
				sgIP_ARP_SendGratARP(wifi_hw);
#endif
				wifi_connect_state=3;
				WifiData->flags9|=WFLAG_ARM9_NETREADY;
				return ASSOCSTATUS_ASSOCIATED;
			case WIFIMODE_CANNOTASSOCIATE:
				return ASSOCSTATUS_CANNOTCONNECT;
			}
			return ASSOCSTATUS_DISCONNECTED;
		case 2: // dhcp'ing
#ifdef WIFI_USE_TCP_SGIP
			{
				int i;
				i=sgIP_DHCP_Update();
				if(i!=SGIP_DHCP_STATUS_WORKING) {
					switch(i) {
					case SGIP_DHCP_STATUS_SUCCESS:
						wifi_connect_state=3;
						WifiData->flags9|=WFLAG_ARM9_NETREADY;
						sgIP_ARP_SendGratARP(wifi_hw);
                        sgIP_DNS_Record_Localhost();
						return ASSOCSTATUS_ASSOCIATED;
					default:
					case SGIP_DHCP_STATUS_IDLE:
					case SGIP_DHCP_STATUS_FAILED:
						Wifi_DisconnectAP();
						wifi_connect_state=-1;
						return ASSOCSTATUS_CANNOTCONNECT;

					}
				}
			}
#else
			// should never get here (dhcp state) without sgIP!
			Wifi_DisconnectAP();
			wifi_connect_state=-1;
			return ASSOCSTATUS_CANNOTCONNECT;
#endif
			return ASSOCSTATUS_ACQUIRINGDHCP;
		case 3: // connected!
			return ASSOCSTATUS_ASSOCIATED;
		case 4: // search nintendo WFC data for a suitable AP
			{
				int n,i;
				for(n=0;n<3;n++) if(!(WifiData->wfc_enable[n]&0x80)) break;
				Wifi_AccessPoint ap;
				n=Wifi_FindMatchingAP(n,WifiData->wfc_ap,&ap);
				if(n!=-1) {
#ifdef WIFI_USE_TCP_SGIP
					Wifi_SetIP(WifiData->wfc_config[n][0],WifiData->wfc_config[n][1],WifiData->wfc_config[n][2],WifiData->wfc_config[n][3],WifiData->wfc_config[n][4]);
#endif
					WifiData->wepmode9=WifiData->wfc_enable[n]&0x03; // copy data
					WifiData->wepkeyid9=(WifiData->wfc_enable[n]>>4)&7;
					for(i=0;i<16;i++) {
						WifiData->wepkey9[i]=WifiData->wfc_wepkey[n][i];
					}

					Wifi_CopyMacAddr(WifiData->bssid9, ap.bssid);
					Wifi_CopyMacAddr(WifiData->apmac9, ap.bssid);
					WifiData->ssid9[0]=ap.ssid_len;
					for(i=0;i<32;i++) {
						WifiData->ssid9[i+1]=ap.ssid[i];
					}
					WifiData->apchannel9=ap.channel;
					for(i=0;i<16;i++) WifiData->baserates9[i]=ap.base_rates[i];
					WifiData->reqMode=WIFIMODE_NORMAL;
					WifiData->reqReqFlags |= WFLAG_REQ_APCONNECT | WFLAG_REQ_APCOPYVALUES;
					wifi_connect_state=1;
					return ASSOCSTATUS_SEARCHING;

				}

			}
			return ASSOCSTATUS_SEARCHING;
	}
	return ASSOCSTATUS_CANNOTCONNECT;
}


int Wifi_DisconnectAP() {
	WifiData->reqMode=WIFIMODE_NORMAL;
	WifiData->reqReqFlags &= ~WFLAG_REQ_APCONNECT;
	WifiData->flags9&=~WFLAG_ARM9_NETREADY;

	wifi_connect_state=-1;
	return 0;
}


#ifdef WIFI_USE_TCP_SGIP



int Wifi_TransmitFunction(sgIP_Hub_HWInterface * hw, sgIP_memblock * mb) {
	// convert ethernet frame into wireless frame and output.
	// ethernet header: 6byte dest, 6byte src, 2byte protocol_id
	// assumes individual pbuf len is >=14 bytes, it's pretty likely ;) - also hopes pbuf len is a multiple of 2 :|
	int base,framelen, hdrlen, writelen;
   int copytotal, copyexpect;
	u16 framehdr[6+12+2];
	sgIP_memblock * t;
   framelen=mb->totallength-14+8 + (WifiData->wepmode7?4:0);

   if(!(WifiData->flags9&WFLAG_ARM9_NETUP)) {
	   SGIP_DEBUG_MESSAGE(("Transmit:err_netdown"));
	   sgIP_memblock_free(mb);
	   return 0; //?
   }
	if(framelen+40>Wifi_TxBufferWordsAvailable()*2) { // error, can't send this much!
		SGIP_DEBUG_MESSAGE(("Transmit:err_space"));
      sgIP_memblock_free(mb);
		return 0; //?
	}

	ethhdr_print('T',mb->datastart);
	framehdr[0]=0;
	framehdr[1]=0;
	framehdr[2]=0;
	framehdr[3]=0;
	framehdr[4]=0; // rate, will be filled in by the arm7.
	hdrlen=18;
	framehdr[7]=0;

	if(WifiData->curReqFlags&WFLAG_REQ_APADHOC) { // adhoc mode
		framehdr[6]=0x0008;
		Wifi_CopyMacAddr(framehdr+14,WifiData->bssid7);
		Wifi_CopyMacAddr(framehdr+11,WifiData->MacAddr);
		Wifi_CopyMacAddr(framehdr+8,((u8 *)mb->datastart));
	} else {
		framehdr[6]=0x0108;
		Wifi_CopyMacAddr(framehdr+8,WifiData->bssid7);
		Wifi_CopyMacAddr(framehdr+11,WifiData->MacAddr);
		Wifi_CopyMacAddr(framehdr+14,((u8 *)mb->datastart));
	}
	if(WifiData->wepmode7)  { framehdr[6] |=0x4000; hdrlen=20; }
	framehdr[17] = 0;
	framehdr[18] = 0; // wep IV, will be filled in if needed on the arm7 side.
	framehdr[19] = 0;

   framehdr[5]=framelen+hdrlen*2-12+4;
   copyexpect= ((framelen+hdrlen*2-12+4) +12 -4 +1)/2;
   copytotal=0;

	WifiData->stats[WSTAT_TXQUEUEDPACKETS]++;
	WifiData->stats[WSTAT_TXQUEUEDBYTES]+=framelen+hdrlen*2;

	base = WifiData->txbufOut;
	Wifi_TxBufferWrite(base,hdrlen,framehdr);
	base += hdrlen;
   copytotal+=hdrlen;
	if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;

	// add LLC header
	framehdr[0]=0xAAAA;
	framehdr[1]=0x0003;
	framehdr[2]=0x0000;
	framehdr[3]=((u16 *)mb->datastart)[6]; // frame type

	Wifi_TxBufferWrite(base,4,framehdr);
	base += 4;
   copytotal+=4;
	if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;

	t=mb;
	writelen=(mb->thislength-14);
	if(writelen) {
		Wifi_TxBufferWrite(base,(writelen+1)/2,((u16 *)mb->datastart)+7);
		base+=(writelen+1)/2;
      copytotal+=(writelen+1)/2;
		if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;
	}
	while(mb->next) {
		mb=mb->next;
		writelen=mb->thislength;
		Wifi_TxBufferWrite(base,(writelen+1)/2,((u16 *)mb->datastart));
		base+=(writelen+1)/2;
      copytotal+=(writelen+1)/2;
		if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;
	}
   if(WifiData->wepmode7) { // add required extra bytes
      base+=2;
      copytotal+=2;
      if(base>=(WIFI_TXBUFFER_SIZE/2)) base -= WIFI_TXBUFFER_SIZE/2;
   }
	WifiData->txbufOut=base; // update fifo out pos, done sending packet.

	sgIP_memblock_free(t); // free packet, as we're the last stop on this chain.

   if(copytotal!=copyexpect) {
      SGIP_DEBUG_MESSAGE(("Tx exp:%i que:%i",copyexpect,copytotal));
   }
   if(synchandler) synchandler();
	return 0;
}

int Wifi_Interface_Init(sgIP_Hub_HWInterface * hw) {
	hw->MTU=2300;
	hw->ipaddr=(192)|(168<<8)|(1<<16)|(151<<24);
	hw->snmask=0x00FFFFFF;
	hw->gateway=(192)|(168<<8)|(1<<16)|(1<<24);
	hw->dns[0]=(192)|(168<<8)|(1<<16)|(1<<24);
	hw->hwaddrlen=6;
	Wifi_CopyMacAddr(hw->hwaddr,WifiData->MacAddr);
	hw->userdata=0;
	return 0;
}

void Wifi_Timer(int num_ms) {
   Wifi_Update();
   sgIP_Timer(num_ms);
}

#endif

unsigned long Wifi_Init(int initflags) {
	erasemem(&Wifi_Data_Struct,sizeof(Wifi_Data_Struct));
    DC_FlushAll();
	WifiData = (Wifi_MainStruct *) (((u32)&Wifi_Data_Struct)| 0x00400000); // should prevent the cache from eating us alive.

#ifdef WIFI_USE_TCP_SGIP
    switch(initflags & WIFIINIT_OPTION_HEAPMASK) {
    case WIFIINIT_OPTION_USEHEAP_128:
        wHeapAllocInit(128*1024);
        break;
    case WIFIINIT_OPTION_USEHEAP_64:
        wHeapAllocInit(64*1024);
        break;
    case WIFIINIT_OPTION_USEHEAP_256:
        wHeapAllocInit(256*1024);
        break;
    case WIFIINIT_OPTION_USEHEAP_512:
        wHeapAllocInit(512*1024);
        break;
    case WIFIINIT_OPTION_USECUSTOMALLOC:
        break;
    }
    sgIP_Init();

#endif

	WifiData->flags9 = WFLAG_ARM9_ACTIVE | (initflags & WFLAG_ARM9_INITFLAGMASK) ;
	return (u32) &Wifi_Data_Struct;
}

int Wifi_CheckInit(void) {
	if(!WifiData) return 0;
	return ((WifiData->flags7 & WFLAG_ARM7_ACTIVE) && (WifiData->flags9 & WFLAG_ARM9_ARM7READY));
}


void Wifi_Update(void) {
	int cnt;
	int base, base2, len, fulllen;
	if(!WifiData) return;

#ifdef WIFI_USE_TCP_SGIP

	if(!(WifiData->flags9&WFLAG_ARM9_ARM7READY)) {
		if(WifiData->flags7 & WFLAG_ARM7_ACTIVE) {
			WifiData->flags9 |=WFLAG_ARM9_ARM7READY;
			// add network interface.
			wifi_hw = sgIP_Hub_AddHardwareInterface(&Wifi_TransmitFunction,&Wifi_Interface_Init);
            sgIP_timems=WifiData->random; //hacky! but it should work just fine :)
		}
	}
	if(WifiData->authlevel!=WIFI_AUTHLEVEL_ASSOCIATED && WifiData->flags9&WFLAG_ARM9_NETUP) {
		WifiData->flags9 &= ~(WFLAG_ARM9_NETUP);
	} else if(WifiData->authlevel==WIFI_AUTHLEVEL_ASSOCIATED && !(WifiData->flags9&WFLAG_ARM9_NETUP)) {
		WifiData->flags9 |= (WFLAG_ARM9_NETUP);
	}

#endif

	// check for received packets, forward to whatever wants them.
	cnt=0;
	while(WifiData->rxbufIn!=WifiData->rxbufOut) {
		base = WifiData->rxbufIn;
		len=Wifi_RxReadOffset(base,4);
		fulllen=((len+3)&(~3))+12;
#ifdef WIFI_USE_TCP_SGIP
		// Do lwIP interfacing for rx here
		if((Wifi_RxReadOffset(base,6)&0x01CF)==0x0008) // if it is a non-null data packet coming from the AP (toDS==0)
		{
			u16 framehdr[6+12+2+4];
			sgIP_memblock * mb;
			int hdrlen;
			base2=base;
			Wifi_RxRawReadPacket(base,22*2,framehdr);

        // ethhdr_print('!',framehdr+8);
			if((framehdr[8]==((u16 *)WifiData->MacAddr)[0] && framehdr[9]==((u16 *)WifiData->MacAddr)[1] && framehdr[10]==((u16 *)WifiData->MacAddr)[2]) ||
				(framehdr[8]==0xFFFF && framehdr[9]==0xFFFF && framehdr[10]==0xFFFF)) {
				// destination matches our mac address, or the broadcast address.
				//if(framehdr[6]&0x4000) { // wep enabled (when receiving WEP packets, the IV is stripped for us! how nice :|
				//	base2+=24; hdrlen=28;  // base2+=[wifi hdr 12byte]+[802 header hdrlen]+[slip hdr 8byte]
				//} else {
					base2+=22; hdrlen=24;
				//}
          //  SGIP_DEBUG_MESSAGE(("%04X %04X %04X %04X %04X",Wifi_RxReadOffset(base2-8,0),Wifi_RxReadOffset(base2-7,0),Wifi_RxReadOffset(base2-6,0),Wifi_RxReadOffset(base2-5,0),Wifi_RxReadOffset(base2-4,0)));
				// check for LLC/SLIP header...
				if(Wifi_RxReadOffset(base2-4,0)==0xAAAA && Wifi_RxReadOffset(base2-4,1)==0x0003 && Wifi_RxReadOffset(base2-4,2)==0) {
					mb = sgIP_memblock_allocHW(14,len-8-hdrlen);
					if(mb) {
						if(base2>=(WIFI_RXBUFFER_SIZE/2)) base2-=(WIFI_RXBUFFER_SIZE/2);
						Wifi_RxRawReadPacket(base2,(len-8-hdrlen)&(~1),((u16 *)mb->datastart)+7);
						if(len&1) ((u8 *)mb->datastart)[len+14-1-8-hdrlen]=Wifi_RxReadOffset(base2,((len-8-hdrlen)/2))&255;
						Wifi_CopyMacAddr(mb->datastart,framehdr+8); // copy dest
						if(Wifi_RxReadOffset(base,6)&0x0200) { // from DS set?
							Wifi_CopyMacAddr(((u8 *)mb->datastart)+6,framehdr+14); // copy src from adrs3
						} else {
							Wifi_CopyMacAddr(((u8 *)mb->datastart)+6,framehdr+11); // copy src from adrs2
						}
						((u16 *)mb->datastart)[6]=framehdr[(hdrlen/2)+6+3]; // assume LLC exists and is 8 bytes.

						ethhdr_print('R',mb->datastart);

						// Done generating recieved data packet... now distribute it.
						sgIP_Hub_ReceiveHardwarePacket(wifi_hw,mb);

					}
				}
			}
		}

#endif

		// check if we have a handler
		if(packethandler) {
			base2=base+6;
			if(base2>=(WIFI_RXBUFFER_SIZE/2)) base2-=(WIFI_RXBUFFER_SIZE/2);
			(*packethandler)(base2,len);
		}

		base+=fulllen/2;
		if(base>=(WIFI_RXBUFFER_SIZE/2)) base-=(WIFI_RXBUFFER_SIZE/2);
		WifiData->rxbufIn=base;

		if(cnt++>80) break;
	}
}


//////////////////////////////////////////////////////////////////////////
// Ip addr get/set functions
#ifdef WIFI_USE_TCP_SGIP

u32 Wifi_GetIP(void) {
	if(wifi_hw) return wifi_hw->ipaddr;
	return 0;
}

unsigned long Wifi_GetIPInfo(unsigned long * pGateway,unsigned long * pSnmask,unsigned long * pDns1,unsigned long * pDns2) {
	if(wifi_hw) {
		if(pGateway) *pGateway=wifi_hw->gateway;
		if(pSnmask) *pSnmask=wifi_hw->snmask;
		if(pDns1) *pDns1=wifi_hw->dns[0];
		if(pDns2) *pDns2=wifi_hw->dns[1];
		return wifi_hw->ipaddr;
	}
	return 0;
}


void Wifi_SetIP(u32 IPaddr, u32 gateway, u32 subnetmask, u32 dns1, u32 dns2) {
	if(wifi_hw) {
		SGIP_DEBUG_MESSAGE(("SetIP%08X %08X %08X",IPaddr,gateway,subnetmask));
		wifi_hw->ipaddr=IPaddr;
		wifi_hw->gateway=gateway;
		wifi_hw->snmask=subnetmask;
		wifi_hw->dns[0]=dns1;
		wifi_hw->dns[1]=dns2;
		// reset arp cache...
		sgIP_ARP_FlushInterface(wifi_hw);
	}
}

void Wifi_SetDHCP(void) {


}

#endif


int Wifi_GetData(int datatype, int bufferlen, unsigned char * buffer) {
	int i;
	if(datatype<0 || datatype>=MAX_WIFIGETDATA) return -1;
	switch(datatype) {
	case WIFIGETDATA_MACADDRESS:
		if(bufferlen<6 || !buffer) return -1;
		for(i=0;i<6;i++) {
			buffer[i]=WifiData->MacAddr[i];
		}
		return 6;
	case WIFIGETDATA_NUMWFCAPS:
		for(i=0;i<3;i++) if(!(WifiData->wfc_enable[i]&0x80)) break;
		return i;
	}
	return -1;
}

u32 Wifi_GetStats(int statnum) {
	if(statnum<0 || statnum>=NUM_WIFI_STATS) return 0;
	return WifiData->stats[statnum];
}


//////////////////////////////////////////////////////////////////////////
// sync functions

void Wifi_Sync(void) {
   Wifi_Update();
}

