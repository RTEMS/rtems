/*---------------------------------------------------------------------------------

	default ARM7 core

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include "../include/my_ipc.h"

#ifdef ENABLE_WIFI
#include <dswifi7.h>
#endif

//---------------------------------------------------------------------------------
void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) {
//---------------------------------------------------------------------------------
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}

//---------------------------------------------------------------------------------
s32 getFreeSoundChannel() {
//---------------------------------------------------------------------------------
	int i;
	for (i=0; i<16; i++) {
		if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
	}
	return -1;
}


touchPosition first,tempPos;

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	static int lastbut = -1;

	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {

		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}

	} else {
		lastbut = but;
		but |= (1 <<6);
	}
	IPC->touchX			= x;
	IPC->touchY			= y;
	IPC->touchXpx		= xpx;
	IPC->touchYpx		= ypx;
	IPC->touchZ1		= z1;
	IPC->touchZ2		= z2;
	IPC->buttons		= but;

}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
  static u8 is_recording = 0;
	u32 i;

	//sound code  :)
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;

	if (0 != snd) {

		for (i=0; i<snd->count; i++) {
			s32 chan = getFreeSoundChannel();

			if (chan >= 0) {
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}

	// microphone code
	if (!is_recording && my_IPC->record)
	  {
	    StartRecording(my_IPC->record_buffer, my_IPC->record_length_max);
	    is_recording = 1;
	  }

	if (is_recording && !my_IPC->record)
	  {
	    my_IPC->recorded_length = 1 + StopRecording();
	    is_recording = 0;
	  }

#ifdef ENABLE_WIFI
	Wifi_Update(); // update wireless in vblank
#endif
}

#ifdef ENABLE_WIFI
// callback to allow wifi library to notify arm9
void arm7_synctoarm9() { // send fifo message
   REG_IPC_FIFO_TX = 0x87654321;
}
// interrupt handler to allow incoming notifications from arm9
void arm7_fifo() { // check incoming fifo messages
   u32 msg = REG_IPC_FIFO_RX;
   if(msg==0x87654321) Wifi_Sync();
}
#endif

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
#ifdef ENABLE_WIFI
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & prepare
							   // fifo asap
#endif

  // read User Settings from firmware
  readUserSettings();

  //enable sound
  powerON(POWER_SOUND);
  writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
  SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);

  irqInit();

  // Start the RTC tracking IRQ
  initClockIRQ();

  SetYtrigger(80);
  irqSet(IRQ_VCOUNT, VcountHandler);
  irqSet(IRQ_VBLANK, VblankHandler);
  irqSet(IRQ_TIMER0, ProcessMicrophoneTimerIRQ);

  irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_TIMER0);

#ifdef ENABLE_WIFI
  irqSet(IRQ_WIFI, Wifi_Interrupt); // set up wifi interrupt
  irqEnable(IRQ_WIFI);

  // sync with arm9 and init wifi
  u32 fifo_temp;

  while (1)
    { // wait for magic number
      while (REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)
	swiWaitForVBlank();
      fifo_temp = REG_IPC_FIFO_RX;
      if (fifo_temp == 0x12345678)
	break;
    }
  while (REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)
    swiWaitForVBlank();
  fifo_temp = REG_IPC_FIFO_RX;
  Wifi_Init(fifo_temp);

  irqSet(IRQ_FIFO_NOT_EMPTY,arm7_fifo); // set up fifo irq
  irqEnable(IRQ_FIFO_NOT_EMPTY);
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

  Wifi_SetSyncHandler(arm7_synctoarm9); // allow wifi lib to notify arm9
#endif
  // Keep the ARM7 mostly idle
  while (1) {
    swiWaitForVBlank();
  }
}


