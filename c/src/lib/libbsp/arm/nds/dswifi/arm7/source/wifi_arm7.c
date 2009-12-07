// DS Wifi interface code
// Copyright (C) 2005-2006 Stephen Stair - sgstair@akkit.org - http://www.akkit.org
// wifi_arm7.c - arm7 wifi interface code
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


#include <nds.h>
#include "dsregs.h"
#include "wifi_arm7.h"

#include "spinlock.h" // .h file with code for spinlocking in it.

volatile Wifi_MainStruct * WifiData = 0;
WifiSyncHandler synchandler = 0;
int keepalive_time = 0;
int chdata_save5 = 0;

//////////////////////////////////////////////////////////////////////////
//
//  Flash support functions
//
char FlashData[512];
void Read_Flash(int address, char * destination, int length) {
	int i;
	while(SPI_CR&0x80);
	SPI_CR=0x8900;
	SPI_DATA=3;
	while(SPI_CR&0x80);
	SPI_DATA=(address>>16)&255;
	while(SPI_CR&0x80);
	SPI_DATA=(address>>8)&255;
	while(SPI_CR&0x80);
	SPI_DATA=(address)&255;
	while(SPI_CR&0x80);
	for(i=0;i<length;i++) {
		SPI_DATA=0;
		while(SPI_CR&0x80);
		destination[i]=SPI_DATA;
	}
	SPI_CR=0;
}

void InitFlashData() {
	Read_Flash(0,FlashData,512);
}

int ReadFlashByte(int address) {
	if(address<0 || address>511) return 0;
	return FlashData[address];
}

int ReadFlashBytes(int address, int numbytes) {
	int dataout=0;
	int i;
	for(i=0;i<numbytes;i++) {
		dataout |= ReadFlashByte(i+address)<<(i*8);
	}
	return dataout;
}
int ReadFlashHWord(int address) {
	if(address<0 || address>510) return 0;
	return ReadFlashBytes(address,2);
}

//////////////////////////////////////////////////////////////////////////
//
// WFC data loading
//
/*
int crc16_slow(u8 * data, int length) {
   int i,j,d, crc;
   crc=0x0000;
   for(i=0;i<length;i++) {
      d=data[i];
      for(j=0;j<8;j++) {
         if(((d)^(crc>>15))&1) crc = (crc<<1)^0x8005; else crc=crc<<1;
         d=d>>1;
      }
   }
   crc &=0xFFFF;
   return crc;
}
*/
int crc16_slow(u8 * data, int length) {
    int i,j, crc;
    crc=0x0000;
    for(i=0;i<length;i++) {
        crc ^=data[i];
        for(j=0;j<8;j++) {
            if((crc)&1) crc = (crc>>1)^0xA001; else crc=crc>>1;
        }
    }
    crc &=0xFFFF;
    return crc;
}

void GetWfcSettings(void) {
   u8 data[256];
   int i,n, c;
   unsigned long s;
   c=0;
   u32 wfcBase = ReadFlashBytes(0x20, 2) * 8 - 0x400;
   for(i=0;i<3;i++) WifiData->wfc_enable[i]=0;
   for(i=0;i<3;i++) {
      Read_Flash( wfcBase +(i<<8),(char *)data,256);
      // check for validity (crc16)
	  if(crc16_slow(data,256)==0x0000 && data[0xE7]==0x00) { // passed the test
		  WifiData->wfc_enable[c] = 0x80 | (data[0xE6]&0x0F);
		  WifiData->wfc_ap[c].channel=0;
		  for(n=0;n<6;n++) WifiData->wfc_ap[c].bssid[n]=0;
		  for(n=0;n<16;n++) WifiData->wfc_wepkey[c][n]=data[0x80+n];
		  for(n=0;n<32;n++) WifiData->wfc_ap[c].ssid[n]=data[0x40+n];
		  for(n=0;n<32;n++) if(!data[0x40+n]) break;
		  WifiData->wfc_ap[c].ssid_len=n;
		  WifiData->wfc_config[c][0]=((unsigned long *)(data+0xC0))[0];
		  WifiData->wfc_config[c][1]=((unsigned long *)(data+0xC0))[1];
		  WifiData->wfc_config[c][3]=((unsigned long *)(data+0xC0))[2];
		  WifiData->wfc_config[c][4]=((unsigned long *)(data+0xC0))[3];
		  s=0;
		  for(n=0;n<data[0xD0];n++) {
			s |= 1<<(31-n);
		  }
		  s= (s<<24) | (s>>24) | ((s&0xFF00)<<8) | ((s&0xFF0000)>>8); // htonl
		  WifiData->wfc_config[c][2]=s;
		  c++;
	  }
   }
}


//////////////////////////////////////////////////////////////////////////
//
//  Other support
//

int Wifi_BBRead(int a) {
	while(W_BBSIOBUSY&1);
	W_BBSIOCNT=a | 0x6000;
	while(W_BBSIOBUSY&1);
	return W_BBSIOREAD;
}
int Wifi_BBWrite(int a, int b) {
	int i;
	i=0x2710;
	while((W_BBSIOBUSY&1)) {
		if(!i--) return -1;
	}
	W_BBSIOWRITE=b;
	W_BBSIOCNT=a | 0x5000;
	i=0x2710;
	while((W_BBSIOBUSY&1)) {
		if(!i--) return 0;
	}
	return 0;
}
void Wifi_RFWrite(int writedata) {
	while(W_RFSIOBUSY&1);
	W_RFSIODATA1=writedata;
	W_RFSIODATA2=writedata>>16;
	while(W_RFSIOBUSY&1);
}

int PowerChip_ReadWrite(int cmd, int data) {
	if(cmd&0x80) data=0;
	while(SPI_CR&0x80);
	SPI_CR=0x8802;
	SPI_DATA=cmd;
	while(SPI_CR&0x80);
	SPI_CR=0x8002;
	SPI_DATA=data;
	while(SPI_CR&0x80);
	data=SPI_DATA;
	SPI_CR=0;
	return data;
}

#define LED_LONGBLINK	1
#define LED_SHORTBLINK	3
#define LED_ON			0
int led_state=0;
void SetLedState(int state) {
	int i;
	if(state>3 || state<0) return;
	if(state!=led_state) {
		led_state=state;
		i=PowerChip_ReadWrite(0x80,0);
		i=i&0xCF;
		i |= state<<4;
		PowerChip_ReadWrite(0,i);
	}
}

void ProxySetLedState(int state) {
	if(WifiData->flags9&WFLAG_ARM9_USELED) SetLedState(state);
}

//////////////////////////////////////////////////////////////////////////
//
//  Main functionality
//

int RF_Reglist[] = { 0x146, 0x148, 0x14A, 0x14C, 0x120, 0x122, 0x154, 0x144, 0x130, 0x132, 0x140, 0x142, 0x38, 0x124, 0x128, 0x150 };

void Wifi_RFInit() {
	int i,j;
	int channel_extrabits;
	int numchannels;
	int channel_extrabytes;
	int temp;
	for(i=0;i<16;i++) {
		WIFI_REG(RF_Reglist[i])=ReadFlashHWord(0x44+i*2);
	}
	numchannels=ReadFlashByte(0x42);
	channel_extrabits=ReadFlashByte(0x41);
	channel_extrabytes=(channel_extrabits+7)/8;
	WIFI_REG(0x184)=((channel_extrabits>>7)<<8) | (channel_extrabits&0x7F);
	j=0xCE;
	if(ReadFlashByte(0x40)==3) {
		for(i=0;i<numchannels;i++) {
			Wifi_RFWrite(ReadFlashByte(j++)|(i<<8)|0x50000);
		}
	} else if(ReadFlashByte(0x40)==2) {
		for(i=0;i<numchannels;i++) {
			temp = ReadFlashBytes(j,channel_extrabytes);
			Wifi_RFWrite(temp);
			j+=channel_extrabytes;
			if( (temp>>18)==9 ) {
				chdata_save5 = temp&(~0x7C00);
			}
		}
	} else {
		for(i=0;i<numchannels;i++) {
			Wifi_RFWrite(ReadFlashBytes(j,channel_extrabytes));
			j+=channel_extrabytes;
		}
	}
}

void Wifi_BBInit() {
	int i;
	WIFI_REG(0x160)=0x0100;
	for(i=0;i<0x69;i++) {
		Wifi_BBWrite(i,ReadFlashByte(0x64+i));
	}
}

// 22 entry list
int MAC_Reglist[] = { 0x04, 0x08, 0x0A, 0x12, 0x10, 0x254, 0xB4, 0x80, 0x2A, 0x28, 0xE8, 0xEA, 0xEE, 0xEC, 0x1A2, 0x1A0, 0x110, 0xBC, 0xD4, 0xD8, 0xDA, 0x76 };
int MAC_Vallist[] = { 0, 0, 0, 0, 0xffff, 0, 0xffff, 0, 0, 0, 0, 0, 1, 0x3F03, 1, 0, 0x0800, 1, 3, 4, 0x0602, 0};
void Wifi_MacInit() {
	int i;
	for(i=0;i<22;i++) {
		WIFI_REG(MAC_Reglist[i]) = MAC_Vallist[i];
	}
}


void Wifi_TxSetup() {
/*	switch(WIFI_REG(0x8006)&7) {
	case 0: //
		// 4170,  4028, 4000
		// TxqEndData, TxqEndManCtrl, TxqEndPsPoll
		WIFI_REG(0x4024)=0xB6B8;
		WIFI_REG(0x4026)=0x1D46;
		WIFI_REG(0x416C)=0xB6B8;
		WIFI_REG(0x416E)=0x1D46;
		WIFI_REG(0x4790)=0xB6B8;
		WIFI_REG(0x4792)=0x1D46;
		WIFI_REG(0x80AE) = 1;
		break;
	case 1: //
		// 4AA0, 4958, 4334
		// TxqEndData, TxqEndManCtrl, TxqEndBroadCast
		// 4238, 4000
		WIFI_REG(0x4234)=0xB6B8;
		WIFI_REG(0x4236)=0x1D46;
		WIFI_REG(0x4330)=0xB6B8;
		WIFI_REG(0x4332)=0x1D46;
		WIFI_REG(0x4954)=0xB6B8;
		WIFI_REG(0x4956)=0x1D46;
		WIFI_REG(0x4A9C)=0xB6B8;
		WIFI_REG(0x4A9E)=0x1D46;
		WIFI_REG(0x50C0)=0xB6B8;
		WIFI_REG(0x50C2)=0x1D46;
		//...
		break;
	case 2:
		// 45D8, 4490, 4468
		// TxqEndData, TxqEndManCtrl, TxqEndPsPoll

		WIFI_REG(0x4230)=0xB6B8;
		WIFI_REG(0x4232)=0x1D46;
		WIFI_REG(0x4464)=0xB6B8;
		WIFI_REG(0x4466)=0x1D46;
		WIFI_REG(0x448C)=0xB6B8;
		WIFI_REG(0x448E)=0x1D46;
		WIFI_REG(0x45D4)=0xB6B8;
		WIFI_REG(0x45D6)=0x1D46;
		WIFI_REG(0x4BF8)=0xB6B8;
		WIFI_REG(0x4BFA)=0x1D46;
*/
		WIFI_REG(0x80AE)=0x000D;
//	}
}

void Wifi_RxSetup() {
	WIFI_REG(0x8030) = 0x8000;
/*	switch(WIFI_REG(0x8006)&7) {
	case 0:
		WIFI_REG(0x8050) = 0x4794;
		WIFI_REG(0x8056) = 0x03CA;
		// 17CC ?
		break;
	case 1:
		WIFI_REG(0x8050) = 0x50C4;
		WIFI_REG(0x8056) = 0x0862;
		// 0E9C ?
		break;
	case 2:
		WIFI_REG(0x8050) = 0x4BFC;
		WIFI_REG(0x8056) = 0x05FE;
		// 1364 ?
		break;
	case 3:
		WIFI_REG(0x8050) = 0x4794;
		WIFI_REG(0x8056) = 0x03CA;
		// 17CC ?
		break;
	}
	*/
	WIFI_REG(0x8050) = 0x4C00;
	WIFI_REG(0x8056) = 0x0600;

	WIFI_REG(0x8052) = 0x5F60;
	WIFI_REG(0x805A) = (WIFI_REG(0x8050)&0x3FFF)>>1;
	WIFI_REG(0x8062) = 0x5F5E;
	WIFI_REG(0x8030) = 0x8001;
}


void Wifi_WakeUp() {
	u32 i;
	WIFI_REG(0x8036)=0;

	swiDelay( 67109 ); // 8ms delay

	WIFI_REG(0x8168)=0;

	i=Wifi_BBRead(1);
	Wifi_BBWrite(1,i&0x7f);
	Wifi_BBWrite(1,i);

	swiDelay( 335544 ); // 40ms delay

	Wifi_RFInit();
}
void Wifi_Shutdown() {
	int a;
	if(ReadFlashByte(0x40)==2) {
		Wifi_RFWrite(0xC008);
	}
	a=Wifi_BBRead(0x1E);
	Wifi_BBWrite(0x1E,a|0x3F);
	WIFI_REG(0x168)=0x800D;
	WIFI_REG(0x36)=1;
}


void Wifi_CopyMacAddr(volatile void * dest, volatile void * src) {
	((u16 *)dest)[0]=((u16 *)src)[0];
	((u16 *)dest)[1]=((u16 *)src)[1];
	((u16 *)dest)[2]=((u16 *)src)[2];
}

int Wifi_CmpMacAddr(volatile void * mac1,volatile  void * mac2) {
	return (((u16 *)mac1)[0]==((u16 *)mac2)[0]) && (((u16 *)mac1)[1]==((u16 *)mac2)[1]) && (((u16 *)mac1)[2]==((u16 *)mac2)[2]);
}

//////////////////////////////////////////////////////////////////////////
//
//  MAC Copy functions
//

u16 inline Wifi_MACRead(u32 MAC_Base, u32 MAC_Offset) {
	MAC_Base += MAC_Offset;
	if(MAC_Base>=(WIFI_REG(0x52)&0x1FFE)) MAC_Base -= ((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
	return WIFI_REG(0x4000+MAC_Base);
}

void Wifi_MACCopy(u16 * dest, u32 MAC_Base, u32 MAC_Offset, u32 length) {
	int endrange,subval;
	int thislength;
	endrange = (WIFI_REG(0x52)&0x1FFE);
	subval=((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
	MAC_Base += MAC_Offset;
	if(MAC_Base>=endrange) MAC_Base -= subval;
	while(length>0) {
		thislength=length;
		if(thislength>(endrange-MAC_Base)) thislength=endrange-MAC_Base;
		length-=thislength;
		while(thislength>0) {
			*(dest++) = WIFI_REG(0x4000+MAC_Base);
			MAC_Base+=2;
			thislength-=2;
		}
		MAC_Base-=subval;
	}
}

void Wifi_MACWrite(u16 * src, u32 MAC_Base, u32 MAC_Offset, u32 length) {
	int endrange,subval;
	int thislength;
	endrange = (WIFI_REG(0x52)&0x1FFE);
	subval=((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
	MAC_Base += MAC_Offset;
	if(MAC_Base>=endrange) MAC_Base -= subval;
	while(length>0) {
		thislength=length;
		if(length>(endrange-MAC_Base)) length=endrange-MAC_Base;
		length-=thislength;
		while(thislength>0) {
			WIFI_REG(0x4000+MAC_Base) = *(src++);
			MAC_Base+=2;
			thislength-=2;
		}
		MAC_Base-=subval;
	}
}
int Wifi_QueueRxMacData(u32 base, u32 len) {
	int buflen, temp,macofs, tempout;
	macofs=0;
	buflen=(WifiData->rxbufIn-WifiData->rxbufOut-1)*2;
	if(buflen<0) buflen += WIFI_RXBUFFER_SIZE;
	if(buflen<len) { WifiData->stats[WSTAT_RXQUEUEDLOST]++; return 0; }
	WifiData->stats[WSTAT_RXQUEUEDPACKETS]++;
	WifiData->stats[WSTAT_RXQUEUEDBYTES]+=len;
	temp=WIFI_RXBUFFER_SIZE-(WifiData->rxbufOut*2);
    tempout=WifiData->rxbufOut;
	if(len>temp) {
		Wifi_MACCopy((u16*)WifiData->rxbufData+tempout,base,macofs,temp);
		macofs+=temp;
		len-=temp;
		tempout=0;
	}
	Wifi_MACCopy((u16*)WifiData->rxbufData+tempout,base,macofs,len);
	tempout+=len/2;
	if(tempout>=(WIFI_RXBUFFER_SIZE/2)) tempout-=(WIFI_RXBUFFER_SIZE/2);
    WifiData->rxbufOut=tempout;
   if(synchandler) synchandler();
	return 1;
}

int Wifi_CheckTxBuf(s32 offset) {
	offset+=WifiData->txbufIn;
	if(offset>=WIFI_TXBUFFER_SIZE/2) offset-=WIFI_TXBUFFER_SIZE/2;
	return WifiData->txbufData[offset];
}

// non-wrapping function.
int Wifi_CopyFirstTxData(s32 macbase) {
	int seglen, readbase,max, packetlen,length;
	packetlen=Wifi_CheckTxBuf(5);
	readbase=WifiData->txbufIn;
	length = (packetlen+12-4+1)/2;
	max=WifiData->txbufOut-WifiData->txbufIn;
	if(max<0) max+=WIFI_TXBUFFER_SIZE/2;
	if(max<length) return 0;
	while(length>0) {
		seglen=length;
		if(readbase+seglen>WIFI_TXBUFFER_SIZE/2) seglen=WIFI_TXBUFFER_SIZE/2-readbase;
		length-=seglen;
		while(seglen--) { WIFI_REG(0x4000+macbase)=WifiData->txbufData[readbase++]; macbase+=2; }
		if(readbase>=WIFI_TXBUFFER_SIZE/2) readbase-=WIFI_TXBUFFER_SIZE/2;
	}
	WifiData->txbufIn=readbase;

	WifiData->stats[WSTAT_TXPACKETS]++;
	WifiData->stats[WSTAT_TXBYTES]+=packetlen+12-4;
	WifiData->stats[WSTAT_TXDATABYTES]+=packetlen-4;

	return packetlen;
}



u16 arm7q[1024];
u16 arm7qlen = 0;

void Wifi_TxRaw(u16 * data, int datalen) {
	datalen = (datalen+3)&(~3);
	Wifi_MACWrite(data, 0, 0, datalen);
//	WIFI_REG(0xB8)=0x0001;
	WIFI_REG(0xA8)=0x8000;
	WifiData->stats[WSTAT_TXPACKETS]++;
	WifiData->stats[WSTAT_TXBYTES]+=datalen;
	WifiData->stats[WSTAT_TXDATABYTES]+=datalen-12;
}

int Wifi_TxCheck(void) {
	if(WIFI_REG(0xA8)&0x8000) return 0;
	return 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Wifi Interrupts
//

void Wifi_Intr_RxEnd(void) {
	int base;
	int packetlen;
	int full_packetlen;
	int cut, temp;
	int tIME;
	tIME=REG_IME;
	REG_IME=0;
	cut=0;

	while(WIFI_REG(0x54)!=WIFI_REG(0x5A)) {
		base = WIFI_REG(0x5A)<<1;
		packetlen=Wifi_MACRead(base,8);
		full_packetlen=12+((packetlen+3)&(~3));
		WifiData->stats[WSTAT_RXPACKETS]++;
		WifiData->stats[WSTAT_RXBYTES]+=full_packetlen;
		WifiData->stats[WSTAT_RXDATABYTES]+=full_packetlen-12;


		// process packet here
		temp=Wifi_ProcessReceivedFrame(base,full_packetlen); // returns packet type
		if(temp&WifiData->reqPacketFlags || WifiData->reqReqFlags&WFLAG_REQ_PROMISC) { // if packet type is requested, forward it to the rx queue
			keepalive_time=0;
			if(!Wifi_QueueRxMacData(base,full_packetlen)) {
				// failed, ignore for now.
			}
		}

		base += full_packetlen;
		if(base>=(WIFI_REG(0x52)&0x1FFE)) base -= ((WIFI_REG(0x52)&0x1FFE)-(WIFI_REG(0x50)&0x1FFE));
		WIFI_REG(0x5A)=base>>1;

		if(cut++>5) break;
	}
	REG_IME=tIME;
}

#define CNT_STAT_START WSTAT_HW_1B0
#define CNT_STAT_NUM 18
u16 count_ofs_list[CNT_STAT_NUM] = {
	0x1B0, 0x1B2, 0x1B4, 0x1B6, 0x1B8, 0x1BA, 0x1BC, 0x1BE, 0x1C0, 0x1C4, 0x1D0, 0x1D2, 0x1D4, 0x1D6, 0x1D8, 0x1DA, 0x1DC, 0x1DE
};
void Wifi_Intr_CntOverflow(void) {
	int i;
	int s,d;
	s=CNT_STAT_START;
	for(i=0;i<CNT_STAT_NUM;i++) {
		d=WIFI_REG(count_ofs_list[i]);
		WifiData->stats[s++]+=(d&0xFF);
		WifiData->stats[s++]+=((d>>8)&0xFF);
	}
}

void Wifi_Intr_StartTx(void) {
	if(WifiData->reqReqFlags&WFLAG_REQ_PROMISC) { // attempt to ensure packet is received

	}
}

void Wifi_Intr_TxEnd(void) { // assume that we can now tx something new.
	if(arm7qlen) {
		Wifi_TxRaw(arm7q, arm7qlen);
		keepalive_time=0;
		arm7qlen=0;
		return;
	}
	if((WifiData->txbufOut!=WifiData->txbufIn) && (!(WifiData->curReqFlags&WFLAG_REQ_APCONNECT) || WifiData->authlevel==WIFI_AUTHLEVEL_ASSOCIATED)) {
		if(Wifi_CopyFirstTxData(0)) {
			keepalive_time=0;
			if(WIFI_REG(0x4008)==0) { WIFI_REG(0x4008)=WifiData->maxrate7; } // if rate dne, fill it in.
			if(WIFI_REG(0x400C)&0x4000) { // wep is enabled, fill in the IV.
				WIFI_REG(0x4024) = (W_RANDOM ^ (W_RANDOM<<7) ^ (W_RANDOM<<15))&0xFFFF;
				WIFI_REG(0x4026) = ((W_RANDOM ^ (W_RANDOM>>7))&0xFF) | (WifiData->wepkeyid7<<14);
			}
			//	WIFI_REG(0xB8)=0x0001;
			WIFI_REG(0xA8)=0x8000;
         WIFI_REG(0xAE)=0x000D;
		}
	}
}

void Wifi_Intr_TBTT(void) {
	if(WIFI_REG(0xA8)&0x8000) {
		WIFI_REG(0xAE)=0x000D;
	}
}

void Wifi_Intr_DoNothing(void) {
}



void Wifi_Interrupt(void) {
	int wIF;
	if(!WifiData) return;
	if(!(WifiData->flags7&WFLAG_ARM7_RUNNING)) return;
	do {
	REG_IF=0x01000000; // now that we've cleared the wireless IF, kill the bit in regular IF.
	wIF= W_IE & W_IF;
	if(wIF& 0x0001) { W_IF=0x0001;  Wifi_Intr_RxEnd();  } // 0) Rx End
	if(wIF& 0x0002) { W_IF=0x0002;  Wifi_Intr_TxEnd();  } // 1) Tx End
	if(wIF& 0x0004) { W_IF=0x0004;  Wifi_Intr_DoNothing();  } // 2) Rx Cntup
	if(wIF& 0x0008) { W_IF=0x0008;  Wifi_Intr_DoNothing();  } // 3) Tx Err
	if(wIF& 0x0010) { W_IF=0x0010;  Wifi_Intr_CntOverflow();  } // 4) Count Overflow
	if(wIF& 0x0020) { W_IF=0x0020;  Wifi_Intr_CntOverflow();  } // 5) AckCount Overflow
	if(wIF& 0x0040) { W_IF=0x0040;  Wifi_Intr_DoNothing();  } // 6) Start Rx
	if(wIF& 0x0080) { W_IF=0x0080;  Wifi_Intr_StartTx();  } // 7) Start Tx
	if(wIF& 0x0100) { W_IF=0x0100;  Wifi_Intr_DoNothing();  } // 8)
	if(wIF& 0x0200) { W_IF=0x0200;  Wifi_Intr_DoNothing();  } // 9)
	if(wIF& 0x0400) { W_IF=0x0400;  Wifi_Intr_DoNothing();  } //10)
	if(wIF& 0x0800) { W_IF=0x0800;  Wifi_Intr_DoNothing();  } //11) RF Wakeup
	if(wIF& 0x1000) { W_IF=0x1000;  Wifi_Intr_DoNothing();  } //12) MP End
	if(wIF& 0x2000) { W_IF=0x2000;  Wifi_Intr_DoNothing();  } //13) ACT End
	if(wIF& 0x4000) { W_IF=0x4000;  Wifi_Intr_TBTT();  } //14) TBTT
	if(wIF& 0x8000) { W_IF=0x8000;  Wifi_Intr_DoNothing();  } //15) PreTBTT
	wIF= W_IE & W_IF;
	} while(wIF);

}






void Wifi_Update(void) {
	int i;
	if(!WifiData) return;
   WifiData->random ^=(W_RANDOM ^ (W_RANDOM<<11) ^ (W_RANDOM<<22));
   WifiData->stats[WSTAT_ARM7_UPDATES]++;
   WifiData->stats[WSTAT_DEBUG]=WIFI_REG(0xA8);
   if(WIFI_REG(0xA8)&0x8000) {
      WIFI_REG(0xAE)=0x000D;
   }
	// check flags, to see if we need to change anything
	switch(WifiData->curMode) {
	case WIFIMODE_DISABLED:
		ProxySetLedState(LED_ON);
		if(WifiData->reqMode!=WIFIMODE_DISABLED) {
			Wifi_Start();
			WifiData->curMode=WIFIMODE_NORMAL;
		}
		break;
	case WIFIMODE_NORMAL: // main switcher function
		ProxySetLedState(LED_LONGBLINK);
		if(WifiData->reqMode==WIFIMODE_DISABLED) {
			Wifi_Stop();
			WifiData->curMode=WIFIMODE_DISABLED;
			break;
		}
		if(WifiData->reqMode==WIFIMODE_SCAN) {
			WifiData->counter7=WIFI_REG(0xFA); // timer hword 2 (each tick is 65.5ms)
			WifiData->curMode=WIFIMODE_SCAN;
			break;
		}
		if((WifiData->reqReqFlags ^ WifiData->curReqFlags) & WFLAG_REQ_APCONNECT) {
			if(WifiData->curReqFlags& WFLAG_REQ_APCONNECT) { // already connected; disconnect
				WifiData->curReqFlags &=~WFLAG_REQ_APCONNECT;
			} else { // not connected - connect!
				if(WifiData->reqReqFlags&WFLAG_REQ_APCOPYVALUES) {
					WifiData->wepkeyid7=WifiData->wepkeyid9;
					WifiData->wepmode7=WifiData->wepmode9;
					WifiData->apchannel7=WifiData->apchannel9;
					Wifi_CopyMacAddr(WifiData->bssid7,WifiData->bssid9);
					Wifi_CopyMacAddr(WifiData->apmac7,WifiData->apmac9);
					for(i=0;i<20;i++) WifiData->wepkey7[i]=WifiData->wepkey9[i];
					for(i=0;i<34;i++) WifiData->ssid7[i]=WifiData->ssid9[i];
					for(i=0;i<16;i++) WifiData->baserates7[i]=WifiData->baserates9[i];
					if(WifiData->reqReqFlags&WFLAG_REQ_APADHOC) WifiData->curReqFlags |= WFLAG_REQ_APADHOC; else WifiData->curReqFlags &= ~WFLAG_REQ_APADHOC;
				}
            Wifi_SetWepKey((void *)WifiData->wepkey7);
				Wifi_SetWepMode(WifiData->wepmode7);
				// latch BSSID
				W_BSSID[0]= ((u16 *)WifiData->bssid7)[0];
				W_BSSID[1]= ((u16 *)WifiData->bssid7)[1];
				W_BSSID[2]= ((u16 *)WifiData->bssid7)[2];
				//WIFI_REG(0xD0) &= ~0x0400;
				WIFI_REG(0xD0) |= 0x0400;
				WifiData->reqChannel=WifiData->apchannel7;
				Wifi_SetChannel(WifiData->apchannel7);
				if(WifiData->curReqFlags&WFLAG_REQ_APADHOC) {
					WifiData->authlevel=WIFI_AUTHLEVEL_ASSOCIATED;
				} else {
					Wifi_SendOpenSystemAuthPacket();
					WifiData->authlevel=WIFI_AUTHLEVEL_DISCONNECTED;
				}
            WifiData->txbufIn=WifiData->txbufOut; // empty tx buffer.
				WifiData->curReqFlags |= WFLAG_REQ_APCONNECT;
                WifiData->counter7=WIFI_REG(0xFA); // timer hword 2 (each tick is 65.5ms)
				WifiData->curMode=WIFIMODE_ASSOCIATE;
				WifiData->authctr=0;
			}
		}
		break;
	case WIFIMODE_SCAN:
		ProxySetLedState(LED_LONGBLINK);
		if(WifiData->reqMode!=WIFIMODE_SCAN) {
			WifiData->curMode=WIFIMODE_NORMAL;
			break;
		}
		if(((u16)(WIFI_REG(0xFA)-WifiData->counter7))>6) { // jump ship!
			WifiData->counter7=WIFI_REG(0xFA);
			WifiData->reqChannel++;
         {
            int i;
            for(i=0;i<WIFI_MAX_AP;i++) {
               if(WifiData->aplist[i].flags & WFLAG_APDATA_ACTIVE) {
                  WifiData->aplist[i].timectr++;
                  if(WifiData->aplist[i].timectr>WIFI_AP_TIMEOUT) {
                     WifiData->aplist[i].rssi=0;
                     WifiData->aplist[i].rssi_past[0]=WifiData->aplist[i].rssi_past[1]=
                        WifiData->aplist[i].rssi_past[2]=WifiData->aplist[i].rssi_past[3]=
                        WifiData->aplist[i].rssi_past[4]=WifiData->aplist[i].rssi_past[5]=
                        WifiData->aplist[i].rssi_past[6]=WifiData->aplist[i].rssi_past[7]=0; // update rssi later.
                  }
               }
            }
         }
			if(WifiData->reqChannel==14) WifiData->reqChannel=1;
		}
		break;
	case WIFIMODE_ASSOCIATE:
		ProxySetLedState(LED_LONGBLINK);
		if(WifiData->authlevel==WIFI_AUTHLEVEL_ASSOCIATED) {
			WifiData->curMode=WIFIMODE_ASSOCIATED;
			break;
		}
		if(((u16)(WIFI_REG(0xFA)-WifiData->counter7))>20) { // ~1 second, reattempt connect stage
			WifiData->counter7=WIFI_REG(0xFA);
			WifiData->authctr++;
			if(WifiData->authctr>WIFI_MAX_ASSOC_RETRY) {
				WifiData->curMode=WIFIMODE_CANNOTASSOCIATE;
				break;
			}
			switch(WifiData->authlevel) {
			case WIFI_AUTHLEVEL_DISCONNECTED: // send auth packet
				if(!(WifiData->curReqFlags&WFLAG_REQ_APADHOC)) {
					Wifi_SendOpenSystemAuthPacket();
					break;
				}
				WifiData->authlevel=WIFI_AUTHLEVEL_ASSOCIATED;
				break;
			case WIFI_AUTHLEVEL_DEASSOCIATED:
			case WIFI_AUTHLEVEL_AUTHENTICATED: // send assoc packet
				Wifi_SendAssocPacket();
				break;
			case WIFI_AUTHLEVEL_ASSOCIATED:
				WifiData->curMode=WIFIMODE_ASSOCIATED;
				break;
			}
		}
		if(!(WifiData->reqReqFlags & WFLAG_REQ_APCONNECT)) {
			WifiData->curReqFlags &=~WFLAG_REQ_APCONNECT;
			WifiData->curMode=WIFIMODE_NORMAL;
			break;
		}
		break;
	case WIFIMODE_ASSOCIATED:
		ProxySetLedState(LED_SHORTBLINK);
		keepalive_time++;
		if(keepalive_time>WIFI_KEEPALIVE_COUNT) {
			keepalive_time=0;
			Wifi_SendNullFrame();
		}
		if((u16)(WIFI_REG(0xFA)-WifiData->pspoll_period)>WIFI_PS_POLL_CONST) {
			WifiData->pspoll_period=WIFI_REG(0xFA);
		//	Wifi_SendPSPollFrame();
		}
		if(!(WifiData->reqReqFlags & WFLAG_REQ_APCONNECT)) {
         W_BSSID[0]= ((u16 *)WifiData->bssid7)[0];
         W_BSSID[1]= ((u16 *)WifiData->bssid7)[1];
         W_BSSID[2]= ((u16 *)WifiData->bssid7)[2];
         //WIFI_REG(0xD0) &= ~0x0400;
         WifiData->curMode=WIFIMODE_NORMAL;
         WifiData->authlevel=WIFI_AUTHLEVEL_DISCONNECTED;
			if(WifiData->curReqFlags &WFLAG_REQ_APCONNECT) { // deassociate, then return
				WifiData->curReqFlags &=~WFLAG_REQ_APCONNECT;
			} else { // not connected for some reason, return.
				WifiData->curReqFlags &=~WFLAG_REQ_APCONNECT;
				WifiData->curMode=WIFIMODE_ASSOCIATE;
			}
         break;
		}
		if(WifiData->authlevel!=WIFI_AUTHLEVEL_ASSOCIATED) {
			WifiData->curMode=WIFIMODE_ASSOCIATE;
			break;
		}
		break;
	case WIFIMODE_CANNOTASSOCIATE:
		ProxySetLedState(LED_LONGBLINK);
		if(!(WifiData->reqReqFlags & WFLAG_REQ_APCONNECT)) {
			WifiData->curReqFlags &=~WFLAG_REQ_APCONNECT;
			WifiData->curMode=WIFIMODE_NORMAL;
			break;
		}
		break;


	}
	if(WifiData->curChannel!=WifiData->reqChannel) {
		Wifi_SetChannel(WifiData->reqChannel);
	}
	// check Rx
	Wifi_Intr_RxEnd();
	// check if we need to tx anything
	if(Wifi_TxCheck()) Wifi_Intr_TxEnd(); // easiest way to do so at the moment.
}




//////////////////////////////////////////////////////////////////////////
//
//  Wifi User-called Functions
//
void erasemem(void * mem, int length) {
	int i;
	char * m = (char *)mem;
	for(i=0;i<length;i++)
		m[i]=0;
}


void Wifi_Init(u32 wifidata) {
	WifiData = (Wifi_MainStruct *)wifidata;

	POWERCNT7 |= 2; // enable power for the wifi
	*((volatile u16 *)0x04000206) = 0x30; // ???

    InitFlashData();

	// reset/shutdown wifi:
	WIFI_REG(0x4)=0xffff;
	Wifi_Stop();
	Wifi_Shutdown(); // power off wifi

	WifiData->curChannel=1;
	WifiData->reqChannel=1;
	WifiData->curMode=WIFIMODE_DISABLED;
	WifiData->reqMode=WIFIMODE_DISABLED;
	WifiData->reqPacketFlags=WFLAG_PACKET_ALL & (~WFLAG_PACKET_BEACON);
	WifiData->curReqFlags=0;
	WifiData->reqReqFlags=0;
	WifiData->maxrate7=0x0A;

	int i;
	for(i=0x4000;i<0x6000;i+=2) WIFI_REG(i)=0;


   // load in the WFC data.
   GetWfcSettings();

	for(i=0;i<6;i++)  WifiData->MacAddr[i]=ReadFlashByte(0x36+i);

	W_IE=0;
	Wifi_WakeUp();

	Wifi_MacInit();
	Wifi_RFInit();
	Wifi_BBInit();

	// Set Default Settings
	W_MACADDR[0]=((u16 *)WifiData->MacAddr)[0];
	W_MACADDR[1]=((u16 *)WifiData->MacAddr)[1];
	W_MACADDR[2]=((u16 *)WifiData->MacAddr)[2];

	W_RETRLIMIT=7;
	Wifi_SetMode(2);
	Wifi_SetWepMode(WEPMODE_NONE);


	Wifi_SetChannel(1);

	Wifi_BBWrite(0x13, 0x00);
	Wifi_BBWrite(0x35, 0x1F);

//	Wifi_Shutdown();
   WifiData->random ^=(W_RANDOM ^ (W_RANDOM<<11) ^ (W_RANDOM<<22));

	WifiData->flags7 |= WFLAG_ARM7_ACTIVE;
}

void Wifi_Deinit() {
	Wifi_Stop();
	POWERCNT7 &= ~2;
}

void Wifi_Start() {
	int i, tIME;
	tIME=REG_IME;
	REG_IME=0;
	Wifi_Stop();

//	Wifi_WakeUp();

	WIFI_REG(0x8032) = 0x8000;
	WIFI_REG(0x8134) = 0xFFFF;
	WIFI_REG(0x802A) = 0;
	W_AIDS           = 0;
	WIFI_REG(0x80E8) = 1;
	WIFI_REG(0x8038) = 0x0000;
	WIFI_REG(0x20) = 0x0000;
	WIFI_REG(0x22) = 0x0000;
	WIFI_REG(0x24) = 0x0000;

	Wifi_TxSetup();
	Wifi_RxSetup();

	WIFI_REG(0x8030) = 0x8000;
/*
	switch(WIFI_REG(0x8006)&7) {
	case 0: // infrastructure mode?
		W_IF=0xFFFF;
		W_IE=0x003F;
		WIFI_REG(0x81AE)=0x1fff;
		//WIFI_REG(0x81AA)=0x0400;
		WIFI_REG(0x80D0)=0xffff;
		WIFI_REG(0x80E0)=0x0008;
		WIFI_REG(0x8008)=0;
		WIFI_REG(0x800A)=0;
		WIFI_REG(0x80E8)=0;
		WIFI_REG(0x8004)=1;
		//SetStaState(0x40);
		break;
	case 1: // ad-hoc mode? -- beacons are required to be created!
		W_IF=0xFFF;
		W_IE=0x703F;
		WIFI_REG(0x81AE)=0x1fff;
		WIFI_REG(0x81AA)=0; // 0x400
		WIFI_REG(0x80D0)=0x0301;
		WIFI_REG(0x80E0)=0x000D;
		WIFI_REG(0x8008)=0xE000;
		WIFI_REG(0x800A)=0;
		WIFI_REG(0x8004)=1;
		//??
		WIFI_REG(0x80EA)=1;
		WIFI_REG(0x80AE)=2;
		break;
	case 2: // DS comms mode?
	*/
		W_IF=0xFFFF;
		//W_IE=0xE03F;
		W_IE=0x40B3;
		WIFI_REG(0x81AE)=0x1fff;
		WIFI_REG(0x81AA)=0; //0x68
		W_BSSID[0]=0xFFFF;
		W_BSSID[1]=0xFFFF;
		W_BSSID[2]=0xFFFF;
		WIFI_REG(0x80D0)=0x0181; // 0x181
		WIFI_REG(0x80E0)=0x000B;
		WIFI_REG(0x8008)=0;
		WIFI_REG(0x800A)=0;
		WIFI_REG(0x8004)=1;
		WIFI_REG(0x80E8)=1;
		WIFI_REG(0x80EA)=1;
		//SetStaState(0x20);
	/*
		break;
	case 3:
	case 4:
		break;
	}
	*/

	WIFI_REG(0x8048)=0x0000;
	Wifi_DisableTempPowerSave();
	//WIFI_REG(0x80AE)=0x0002;
	W_POWERSTATE |= 2;
	WIFI_REG(0xAC) = 0xFFFF;
	i=0xFA0;
	while(i!=0 && !(WIFI_REG(0x819C)&0x80)) i--;
	WifiData->flags7 |=WFLAG_ARM7_RUNNING;
	REG_IME=tIME;
}

void Wifi_Stop() {
	int tIME;
	tIME=REG_IME;
	WifiData->flags7 &= ~WFLAG_ARM7_RUNNING;
	W_IE=0;
	WIFI_REG(0x8004) = 0;
	WIFI_REG(0x80EA) = 0;
	WIFI_REG(0x80E8) = 0;
	WIFI_REG(0x8008) = 0;
	WIFI_REG(0x800A) = 0;

	WIFI_REG(0x80AC) = 0xFFFF;
	WIFI_REG(0x80B4) = 0xFFFF;
//	Wifi_Shutdown();
	REG_IME=tIME;
}

void Wifi_SetChannel(int channel) {
	int i,n,l;
	if(channel<1 || channel>13) return;
    channel-=1;

    switch(ReadFlashByte(0x40)) {
    case 2:
    case 5:
        Wifi_RFWrite(ReadFlashBytes(0xf2+channel*6,3));
        Wifi_RFWrite(ReadFlashBytes(0xf5+channel*6,3));

		swiDelay( 12583 ); // 1500 us delay

		if(chdata_save5 & 0x10000)
		{
			if(chdata_save5 & 0x8000) break;
			n = ReadFlashByte(0x154+channel);
			Wifi_RFWrite( chdata_save5 | ((n&0x1F)<<10) );
		} else {
			Wifi_BBWrite(0x1E, ReadFlashByte(0x146+channel));
		}

        break;
    case 3:
        n=ReadFlashByte(0x42);
        n+=0xCF;
		l=ReadFlashByte(n-1);
        for(i=0;i<l;i++) {
            Wifi_BBWrite(ReadFlashByte(n),ReadFlashByte(n+channel+1));
            n+=15;
        }
        for(i=0;i<ReadFlashByte(0x43);i++) {
            Wifi_RFWrite( (ReadFlashByte(n)<<8) | ReadFlashByte(n+channel+1) | 0x050000 );
            n+=15;
        }

		swiDelay( 12583 ); // 1500 us delay

		break;
    default:
        break;
    }
	WifiData->curChannel=channel+1;
}
void Wifi_SetWepKey(void * wepkey) {
	int i;
	for(i=0;i<16;i++) {
		W_WEPKEY0[i]=((u16 *)wepkey)[i];
		W_WEPKEY1[i]=((u16 *)wepkey)[i];
		W_WEPKEY2[i]=((u16 *)wepkey)[i];
		W_WEPKEY3[i]=((u16 *)wepkey)[i];
	}
}

void Wifi_SetWepMode(int wepmode) {
	if(wepmode<0 || wepmode>7) return;
   if(wepmode==0) {
      WIFI_REG(0x32)=0x0000;
   } else {
      WIFI_REG(0x32)=0x8000;
   }
   if(wepmode==0) wepmode=1;
	W_MODE_WEP = (W_MODE_WEP & 0xFFC7) | (wepmode<<3);
}

void Wifi_SetBeaconPeriod(int beacon_period) {
	if(beacon_period<0x10 || beacon_period>0x3E7) return;
	WIFI_REG(0x8C)=beacon_period;
}

void Wifi_SetMode(int wifimode) {
	if(wifimode>3 || wifimode<0) return;
	W_MODE_WEP = (W_MODE_WEP& 0xfff8) | wifimode;
}
void Wifi_SetPreambleType(int preamble_type) {
	if(preamble_type>1 || preamble_type<0) return;
	WIFI_REG(0x80BC) = (WIFI_REG(0x80BC) & 0xFFBF) | (preamble_type<<6);
}
void Wifi_DisableTempPowerSave() {
	WIFI_REG(0x8038) &= ~2;
	WIFI_REG(0x8048) = 0;
}





//////////////////////////////////////////////////////////////////////////
//
//  802.11b system, tied in a bit with the :


int Wifi_TxQueue(u16 * data, int datalen) {
	int i,j;
	if(arm7qlen) {
		if(Wifi_TxCheck()) {
			Wifi_TxRaw(arm7q,arm7qlen);
			arm7qlen=0;
			j=(datalen+1)>>1;
			if(j>1024) return 0;
			for(i=0;i<j;i++) arm7q[i]=data[i];
			arm7qlen=datalen;
			return 1;
		}
		return 0;
	}
	if(Wifi_TxCheck()) {
		Wifi_TxRaw(data,datalen);
		return 1;
	}
	arm7qlen=0;
	j=(datalen+1)>>1;
	if(j>1024) return 0;
	for(i=0;i<j;i++) arm7q[i]=data[i];
	arm7qlen=datalen;
	return 1;
}

int Wifi_GenMgtHeader(u8 * data, u16 headerflags) {
	// tx header
	((u16 *)data)[0]=0;
	((u16 *)data)[1]=0;
	((u16 *)data)[2]=0;
	((u16 *)data)[3]=0;
	((u16 *)data)[4]=0;
	((u16 *)data)[5]=0;
	// fill in most header fields
	((u16 *)data)[7]=0x0000;
	Wifi_CopyMacAddr(data+16,WifiData->apmac7);
	Wifi_CopyMacAddr(data+22,WifiData->MacAddr);
	Wifi_CopyMacAddr(data+28,WifiData->bssid7);
	((u16 *)data)[17]=0;

	// fill in wep-specific stuff
	if(headerflags&0x4000) {
		((u32 *)data)[9]=((W_RANDOM ^ (W_RANDOM<<7) ^ (W_RANDOM<<15))&0x0FFF) | (WifiData->wepkeyid7<<30); // I'm lazy and certainly haven't done this to spec.
		((u16 *)data)[6]=headerflags;
		return 28+12;
	} else {
		((u16 *)data)[6]=headerflags;
		return 24+12;
	}
}

int Wifi_SendOpenSystemAuthPacket() {
	// max size is 12+24+4+6 = 46
	u8 data[64];
	int i;
	i=Wifi_GenMgtHeader(data,0x00B0);

	((u16 *)(data+i))[0]=0; // Authentication algorithm number (0=open system)
	((u16 *)(data+i))[1]=1; // Authentication sequence number
	((u16 *)(data+i))[2]=0; // Authentication status code (reserved for this message, =0)

	((u16 *)data)[4]=0x000A;
	((u16 *)data)[5]=i+6-12+4;

	return Wifi_TxQueue((u16 *)data, i+6);
}

int Wifi_SendSharedKeyAuthPacket(void) {
	// max size is 12+24+4+6 = 46
	u8 data[64];
	int i;
	i=Wifi_GenMgtHeader(data,0x00B0);

	((u16 *)(data+i))[0]=1; // Authentication algorithm number (1=shared key)
	((u16 *)(data+i))[1]=1; // Authentication sequence number
	((u16 *)(data+i))[2]=0; // Authentication status code (reserved for this message, =0)

	((u16 *)data)[4]=0x000A;
	((u16 *)data)[5]=i+6-12+4;

	return Wifi_TxQueue((u16 *)data, i+6);
}

int Wifi_SendSharedKeyAuthPacket2(int challenge_length, u8 * challenge_Text) {
	// max size is 12+24+4+6 = 46
	u8 data[320];
	int i,j;
	i=Wifi_GenMgtHeader(data,0x40B0);

	((u16 *)(data+i))[0]=1; // Authentication algorithm number (1=shared key)
	((u16 *)(data+i))[1]=3; // Authentication sequence number
	((u16 *)(data+i))[2]=0; // Authentication status code (reserved for this message, =0)

    data[i+6]=0x10; // 16=challenge text block
    data[i+7]=challenge_length;

	for(j=0;j<challenge_length;j++) {
		data[i+j+8]=challenge_Text[j];
	}

	((u16 *)data)[4]=0x000A;
	((u16 *)data)[5]=i+8+challenge_length-12+4 +4;

	return Wifi_TxQueue((u16 *)data, i+8+challenge_length);
}


int Wifi_SendAssocPacket() { // uses arm7 data in our struct
	// max size is 12+24+4+34+4 = 66
	u8 data[96];
	int i,j,numrates;

	i=Wifi_GenMgtHeader(data,0x0000);

	if(WifiData->wepmode7) {
		((u16 *)(data+i))[0]=0x0031; // CAPS info
	} else {
		((u16 *)(data+i))[0]=0x0021; // CAPS info
	}

	((u16 *)(data+i))[1]=WIFI_REG(0x8E); // Listen interval
	i+=4;
	data[i++]=0; // SSID element
	data[i++]=WifiData->ssid7[0];
	for(j=0;j<WifiData->ssid7[0];j++) data[i++]=WifiData->ssid7[1+j];

	if((WifiData->baserates7[0]&0x7f)!=2) {
		for(j=1;j<16;j++) WifiData->baserates7[i]=WifiData->baserates7[j-1];
	}
	WifiData->baserates7[0]=0x82;
	if((WifiData->baserates7[1]&0x7f)!=4) {
		for(j=2;j<16;j++) WifiData->baserates7[j]=WifiData->baserates7[j-1];
	}
	WifiData->baserates7[1]=0x04;

	WifiData->baserates7[15]=0;
	for(j=0;j<16;j++) if(WifiData->baserates7[j]==0) break;
	numrates=j;
	for(j=2;j<numrates;j++) WifiData->baserates7[j] &= 0x7F;

	data[i++]=1; // rate set
	data[i++]=numrates;
	for(j=0;j<numrates;j++) data[i++]=WifiData->baserates7[j];

	// reset header fields with needed data
	((u16 *)data)[4]=0x000A;
	((u16 *)data)[5]=i-12+4;

	return Wifi_TxQueue((u16 *)data, i);
}

int Wifi_SendNullFrame() {
	// max size is 12+16 = 28
	u8 data[32];
	// tx header
	((u16 *)data)[0]=0;
	((u16 *)data)[1]=0;
	((u16 *)data)[2]=0;
	((u16 *)data)[3]=0;
	((u16 *)data)[4]=WifiData->maxrate7;
	((u16 *)data)[5]=18+4;
	// fill in packet header fields
	((u16 *)data)[6]=0x0148;
	((u16 *)data)[7]=0;
	Wifi_CopyMacAddr(data+16,WifiData->apmac7);
	Wifi_CopyMacAddr(data+22,WifiData->MacAddr);

	return Wifi_TxQueue((u16 *)data, 30);
}

int Wifi_SendPSPollFrame() {
	// max size is 12+16 = 28
	u8 data[32];
	// tx header
	((u16 *)data)[0]=0;
	((u16 *)data)[1]=0;
	((u16 *)data)[2]=0;
	((u16 *)data)[3]=0;
	((u16 *)data)[4]=WifiData->maxrate7;
	((u16 *)data)[5]=16+4;
	// fill in packet header fields
	((u16 *)data)[6]=0x01A4;
	((u16 *)data)[7]=0xC000 | W_AIDS;
	Wifi_CopyMacAddr(data+16,WifiData->apmac7);
	Wifi_CopyMacAddr(data+22,WifiData->MacAddr);

	return Wifi_TxQueue((u16 *)data, 28);
}

int Wifi_ProcessReceivedFrame(int macbase, int framelen) {
	Wifi_RxHeader packetheader;
	u16 control_802;
	Wifi_MACCopy((u16 *)&packetheader,macbase,0,12);
	control_802=Wifi_MACRead(macbase,12);
	switch((control_802>>2)&0x3F) {
		// Management Frames
		case 0x20: // 1000 00 Beacon
        case 0x14: // 0101 00 Probe Response // process probe responses too.
			// mine data from the beacon...
			{
				u8 data[512];
				u8 wepmode,fromsta;
				u8 segtype,seglen;
				u8 channel;
				u8 wpamode;
				u8 rateset[16];
				u16 ptr_ssid;
				u16 maxrate;
				u16 curloc;
				u32 datalen;
				u16 i,j,compatible;
				datalen=packetheader.byteLength;
				if(datalen>512) datalen=512;
				Wifi_MACCopy((u16 *)data,macbase,12,(datalen+1)&~1);
				wepmode=0;
				maxrate=0;
				if(((u16 *)data)[5+12]&0x0010) { // capability info, WEP bit
					wepmode=1;
				}
				fromsta=Wifi_CmpMacAddr(data+10, data+16);
				curloc=12+24; // 12 fixed bytes, 24 802.11 header
				compatible=1;
				ptr_ssid=0;
				channel=WifiData->curChannel;
				wpamode=0;
				do {
					if(curloc>=datalen) break;
					segtype=data[curloc++];
					seglen=data[curloc++];
					switch(segtype) {
					case 0: // SSID element
						ptr_ssid=curloc-2;
						break;
					case 1: // rate set (make sure we're compatible)
						compatible=0;
						maxrate=0;
						j=0;
						for(i=0;i<seglen;i++) {
							if((data[curloc+i]&0x7F)>maxrate) maxrate=data[curloc+i]&0x7F;
							if(j<15 && data[curloc+i]&0x80) rateset[j++]=data[curloc+i];
						}
						for(i=0;i<seglen;i++) {
							if(data[curloc+i]==0x82 || data[curloc+i]==0x84) compatible=1;  // 1-2mbit, fully compatible
							else if(data[curloc+i]==0x8B || data[curloc+i]==0x96) compatible=2; // 5.5,11mbit, have to fake our way in.
							else if(data[curloc+i]&0x80) { compatible=0; break; }
						}
						rateset[j]=0;
						break;
					case 3: // DS set (current channel)
						channel=data[curloc];
						break;
					case 48: // RSN(A) field- WPA enabled.
						wpamode=1;
						break;

					} // don't care about the others.
					curloc+=seglen;
				} while(curloc<datalen);
				if(wpamode==1) compatible=0;
				if(1) {
					seglen=0;
					segtype=255;
					for(i=0;i<WIFI_MAX_AP;i++) {
						if(	Wifi_CmpMacAddr(WifiData->aplist[i].bssid,data+16)) {
							seglen++;
							if(Spinlock_Acquire(WifiData->aplist[i])==SPINLOCK_OK) {
								WifiData->aplist[i].timectr=0;
								WifiData->aplist[i].flags=WFLAG_APDATA_ACTIVE | (wepmode?WFLAG_APDATA_WEP:0) | (fromsta?0:WFLAG_APDATA_ADHOC);
								if(compatible==1) WifiData->aplist[i].flags |= WFLAG_APDATA_COMPATIBLE;
								if(compatible==2) WifiData->aplist[i].flags |= WFLAG_APDATA_EXTCOMPATIBLE;
								if(wpamode==1) WifiData->aplist[i].flags |= WFLAG_APDATA_WPA;
								WifiData->aplist[i].maxrate=maxrate;

								Wifi_CopyMacAddr(WifiData->aplist[i].macaddr,data+10); // src: +10
								if(ptr_ssid) {
									WifiData->aplist[i].ssid_len=data[ptr_ssid+1];
									if(WifiData->aplist[i].ssid_len>32) WifiData->aplist[i].ssid_len=32;
									for(j=0;j<WifiData->aplist[i].ssid_len;j++) {
										WifiData->aplist[i].ssid[j]=data[ptr_ssid+2+j];
									}
									WifiData->aplist[i].ssid[j]=0;
								}
								if(WifiData->curChannel==channel) { // only use RSSI when we're on the right channel
									if(WifiData->aplist[i].rssi_past[0]==0) { // min rssi is 2, heh.
										WifiData->aplist[i].rssi_past[0]=WifiData->aplist[i].rssi_past[1]=
											WifiData->aplist[i].rssi_past[2]=WifiData->aplist[i].rssi_past[3]=
											WifiData->aplist[i].rssi_past[4]=WifiData->aplist[i].rssi_past[5]=
											WifiData->aplist[i].rssi_past[6]=WifiData->aplist[i].rssi_past[7]=packetheader.rssi_&255;
									} else {
										for(j=0;j<7;j++) {
											WifiData->aplist[i].rssi_past[j]=WifiData->aplist[i].rssi_past[j+1];
										}
										WifiData->aplist[i].rssi_past[7]=packetheader.rssi_&255;
									}
								}
								WifiData->aplist[i].channel=channel;
								for(j=0;j<16;j++) WifiData->aplist[i].base_rates[j]=rateset[j];
								Spinlock_Release(WifiData->aplist[i]);
							} else {
								// couldn't update beacon - oh well :\ there'll be other beacons.
							}
						} else {
							if(WifiData->aplist[i].flags & WFLAG_APDATA_ACTIVE) {
							//	WifiData->aplist[i].timectr++;
							} else {
								if(segtype==255) segtype=i;
							}
						}
					}
					if(seglen==0) { // we couldn't find an existing record
						if(segtype==255) {
							j=0;
							for(i=0;i<WIFI_MAX_AP;i++) {
								if(WifiData->aplist[i].timectr>j) {
									j=WifiData->aplist[i].timectr;
									segtype=i;
								}
							}
						}
						// stuff new data in
						i=segtype;
						if(Spinlock_Acquire(WifiData->aplist[i])==SPINLOCK_OK) {
							Wifi_CopyMacAddr(WifiData->aplist[i].bssid,data+16); // bssid: +16
							Wifi_CopyMacAddr(WifiData->aplist[i].macaddr,data+10); // src: +10
							WifiData->aplist[i].timectr=0;
							WifiData->aplist[i].flags=WFLAG_APDATA_ACTIVE | (wepmode?WFLAG_APDATA_WEP:0) | (fromsta?0:WFLAG_APDATA_ADHOC);
							if(compatible==1) WifiData->aplist[i].flags |= WFLAG_APDATA_COMPATIBLE;
							if(compatible==2) WifiData->aplist[i].flags |= WFLAG_APDATA_EXTCOMPATIBLE;
							if(wpamode==1) WifiData->aplist[i].flags |= WFLAG_APDATA_WPA;
							WifiData->aplist[i].maxrate=maxrate;

							if(ptr_ssid) {
								WifiData->aplist[i].ssid_len=data[ptr_ssid+1];
								if(WifiData->aplist[i].ssid_len>32) WifiData->aplist[i].ssid_len=32;
								for(j=0;j<WifiData->aplist[i].ssid_len;j++) {
									WifiData->aplist[i].ssid[j]=data[ptr_ssid+2+j];
								}
								WifiData->aplist[i].ssid[j]=0;
							}
							if(WifiData->curChannel==channel) { // only use RSSI when we're on the right channel
								WifiData->aplist[i].rssi_past[0]=WifiData->aplist[i].rssi_past[1]=
									WifiData->aplist[i].rssi_past[2]=WifiData->aplist[i].rssi_past[3]=
									WifiData->aplist[i].rssi_past[4]=WifiData->aplist[i].rssi_past[5]=
									WifiData->aplist[i].rssi_past[6]=WifiData->aplist[i].rssi_past[7]=packetheader.rssi_&255;
							} else {
								WifiData->aplist[i].rssi_past[0]=WifiData->aplist[i].rssi_past[1]=
									WifiData->aplist[i].rssi_past[2]=WifiData->aplist[i].rssi_past[3]=
									WifiData->aplist[i].rssi_past[4]=WifiData->aplist[i].rssi_past[5]=
									WifiData->aplist[i].rssi_past[6]=WifiData->aplist[i].rssi_past[7]=0; // update rssi later.
							}
							WifiData->aplist[i].channel=channel;
							for(j=0;j<16;j++) WifiData->aplist[i].base_rates[j]=rateset[j];

							Spinlock_Release(WifiData->aplist[i]);
						} else {
							// couldn't update beacon - oh well :\ there'll be other beacons.
						}
					}
				}
			}
            if(((control_802>>2)&0x3F)==0x14) return WFLAG_PACKET_MGT;
			return WFLAG_PACKET_BEACON;
		case 0x04: // 0001 00 Assoc Response
		case 0x0C: // 0011 00 Reassoc Response
			// we might have been associated, let's check.
			{
				int datalen,i;
				u8 data[64];
				datalen=packetheader.byteLength;
				if(datalen>64) datalen=64;
				Wifi_MACCopy((u16 *)data,macbase,12,(datalen+1)&~1);

				if(Wifi_CmpMacAddr(data+4,WifiData->MacAddr)) { // packet is indeed sent to us.
					if(Wifi_CmpMacAddr(data+16,WifiData->bssid7)) { // packet is indeed from the base station we're trying to associate to.
						if(((u16 *)(data+24))[1]==0) { // status code, 0==success
							W_AIDS=((u16 *)(data+24))[2];
							WIFI_REG(0x2A)=((u16 *)(data+24))[2];
							// set max rate
							WifiData->maxrate7=0xA;
							for(i=0;i<((u8 *)(data+24))[7];i++) {
								if(((u8 *)(data+24))[8+i]==0x84 || ((u8 *)(data+24))[8+i]==0x04) {
									WifiData->maxrate7=0x14;
								}
							}
							if(WifiData->authlevel==WIFI_AUTHLEVEL_AUTHENTICATED || WifiData->authlevel==WIFI_AUTHLEVEL_DEASSOCIATED) {
								WifiData->authlevel=WIFI_AUTHLEVEL_ASSOCIATED;
								WifiData->authctr=0;



							}
						} else { // status code = failure!
							WifiData->curMode=WIFIMODE_CANNOTASSOCIATE;
						}
					}
				}
			}


			return WFLAG_PACKET_MGT;
		case 0x00: // 0000 00 Assoc Request
		case 0x08: // 0010 00 Reassoc Request
		case 0x10: // 0100 00 Probe Request
		case 0x24: // 1001 00 ATIM
		case 0x28: // 1010 00 Disassociation
			return WFLAG_PACKET_MGT;
		case 0x2C: // 1011 00 Authentication
			// check auth response to ensure we're in
			{
				int datalen;
				u8 data[384];
				datalen=packetheader.byteLength;
				if(datalen>384) datalen=384;
				Wifi_MACCopy((u16 *)data,macbase,12,(datalen+1)&~1);

				if(Wifi_CmpMacAddr(data+4,WifiData->MacAddr)) { // packet is indeed sent to us.
					if(Wifi_CmpMacAddr(data+16,WifiData->bssid7)) { // packet is indeed from the base station we're trying to associate to.
                        if(((u16 *)(data+24))[0]==0) { // open system auth
                            if(((u16 *)(data+24))[1]==2) { // seq 2, should be final sequence
                                if(((u16 *)(data+24))[2]==0) { // status code: successful
							        if(WifiData->authlevel==WIFI_AUTHLEVEL_DISCONNECTED) {
								        WifiData->authlevel=WIFI_AUTHLEVEL_AUTHENTICATED;
								        WifiData->authctr=0;
								        Wifi_SendAssocPacket();
							        }
                                } else { // status code: rejected, try something else
                                    Wifi_SendSharedKeyAuthPacket();
                                }
                            }
                        } else if(((u16 *)(data+24))[0]==1) { // shared key auth
                            if(((u16 *)(data+24))[1]==2) { // seq 2, challenge text
                                if(((u16 *)(data+24))[2]==0) { // status code: successful
                                    // scrape challenge text and send challenge reply
                                    if(data[24+6]==0x10) { // 16 = challenge text - this value must be 0x10 or else!
                                        Wifi_SendSharedKeyAuthPacket2(data[24+7],data+24+8);
                                    }
                                } else { // rejected, just give up.
                                    WifiData->curMode=WIFIMODE_CANNOTASSOCIATE;
                                }
                            } else if(((u16 *)(data+24))[1]==4) { // seq 4, accept/deny
                                if(((u16 *)(data+24))[2]==0) { // status code: successful
                                    if(WifiData->authlevel==WIFI_AUTHLEVEL_DISCONNECTED) {
                                        WifiData->authlevel=WIFI_AUTHLEVEL_AUTHENTICATED;
                                        WifiData->authctr=0;
                                        Wifi_SendAssocPacket();
                                    }
                                } else { // status code: rejected. Cry in the corner.
                                    WifiData->curMode=WIFIMODE_CANNOTASSOCIATE;
                                }
                            }
                        }
					}
				}
			}
			return WFLAG_PACKET_MGT;
		case 0x30: // 1100 00 Deauthentication
			{
				int datalen;
				u8 data[64];
				datalen=packetheader.byteLength;
				if(datalen>64) datalen=64;
				Wifi_MACCopy((u16 *)data,macbase,12,(datalen+1)&~1);

				if(Wifi_CmpMacAddr(data+4,WifiData->MacAddr)) { // packet is indeed sent to us.
					if(Wifi_CmpMacAddr(data+16,WifiData->bssid7)) { // packet is indeed from the base station we're trying to associate to.
						// bad things! they booted us!.
						// back to square 1.
						if(WifiData->curReqFlags&WFLAG_REQ_APADHOC){
							WifiData->authlevel=WIFI_AUTHLEVEL_AUTHENTICATED;
							Wifi_SendAssocPacket();
						} else {
							WifiData->authlevel=WIFI_AUTHLEVEL_DISCONNECTED;
							Wifi_SendOpenSystemAuthPacket();
						}
					}
				}
			}
			return WFLAG_PACKET_MGT;
		// Control Frames
		case 0x29: // 1010 01 PowerSave Poll
		case 0x2D: // 1011 01 RTS
		case 0x31: // 1100 01 CTS
		case 0x35: // 1101 01 ACK
		case 0x39: // 1110 01 CF-End
		case 0x3D: // 1111 01 CF-End+CF-Ack
			return WFLAG_PACKET_CTRL;
		// Data Frames
		case 0x02: // 0000 10 Data
		case 0x06: // 0001 10 Data + CF-Ack
		case 0x0A: // 0010 10 Data + CF-Poll
		case 0x0E: // 0011 10 Data + CF-Ack + CF-Poll
			// We like data!

			return WFLAG_PACKET_DATA;
		case 0x12: // 0100 10 Null Function
		case 0x16: // 0101 10 CF-Ack
		case 0x1A: // 0110 10 CF-Poll
		case 0x1E: // 0111 10 CF-Ack + CF-Poll
			return WFLAG_PACKET_DATA;
		default: // ignore!
		return 0;
	}
}
int Wifi_CreateTxFrame(int frametype, void * dest, int destlen) {
	switch(frametype) {

	default:
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// sync functions

void Wifi_Sync() {
   Wifi_Update();
}
void Wifi_SetSyncHandler(WifiSyncHandler sh) {
   synchandler=sh;
}


