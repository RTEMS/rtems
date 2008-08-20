/*---------------------------------------------------------------------------------
	$Id$

	key input code -- provides slightly higher level input forming

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Christian Auby (DesktopMan)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

	$Log$
	Revision 1.1  2008/04/16 18:37:32  joel
	2008-04-16	Matthieu Bucchianeri <mbucchia@gmail.com>
	
		* ChangeLog, Makefile.am, README, bsp_specs, configure.ac,
		mk_libnds.sh, patch.libnds, preinstall.am, block/block.c,
		clock/clock.c, console/console.c, coproc/coproc.S, coproc/coproc.c,
		coproc/coproc.ld, dswifi/dswifi_license.txt, dswifi/makefile,
		dswifi/arm7/makefile, dswifi/arm7/source/wifi_arm7.c,
		dswifi/arm7/source/wifi_arm7.h, dswifi/arm9/makefile,
		dswifi/arm9/source/sgIP.c, dswifi/arm9/source/sgIP.h,
		dswifi/arm9/source/sgIP_ARP.c, dswifi/arm9/source/sgIP_ARP.h,
		dswifi/arm9/source/sgIP_Config.h, dswifi/arm9/source/sgIP_DHCP.c,
		dswifi/arm9/source/sgIP_DHCP.h, dswifi/arm9/source/sgIP_DNS.c,
		dswifi/arm9/source/sgIP_DNS.h, dswifi/arm9/source/sgIP_Hub.c,
		dswifi/arm9/source/sgIP_Hub.h, dswifi/arm9/source/sgIP_ICMP.c,
		dswifi/arm9/source/sgIP_ICMP.h, dswifi/arm9/source/sgIP_IP.c,
		dswifi/arm9/source/sgIP_IP.h, dswifi/arm9/source/sgIP_TCP.c,
		dswifi/arm9/source/sgIP_TCP.h, dswifi/arm9/source/sgIP_UDP.c,
		dswifi/arm9/source/sgIP_UDP.h, dswifi/arm9/source/sgIP_memblock.c,
		dswifi/arm9/source/sgIP_memblock.h,
		dswifi/arm9/source/sgIP_sockets.c, dswifi/arm9/source/sgIP_sockets.h,
		dswifi/arm9/source/wifi_arm9.c, dswifi/arm9/source/wifi_arm9.h,
		dswifi/common/source/dsregs.h, dswifi/common/source/spinlock.h,
		dswifi/common/source/spinlock.s, dswifi/common/source/wifi_shared.h,
		dswifi/include/dswifi7.h, dswifi/include/dswifi9.h,
		dswifi/include/dswifi_version.h, dswifi/include/netdb.h,
		dswifi/include/sgIP_errno.h, dswifi/include/netinet/in.h, fb/fb.c,
		fb/fb.h, include/bsp.h, include/my_ipc.h, include/tm27.h,
		include/types.h, include/sys/iosupport.h, irq/irq.c, irq/irq.h,
		libfat/gba/include/fat.h, libfat/include/fat.h,
		libfat/nds/include/fat.h, libfat/source/bit_ops.h,
		libfat/source/cache.c, libfat/source/cache.h, libfat/source/common.h,
		libfat/source/directory.c, libfat/source/directory.h,
		libfat/source/fatdir.c, libfat/source/fatdir.h,
		libfat/source/fatfile.c, libfat/source/fatfile.h,
		libfat/source/file_allocation_table.c,
		libfat/source/file_allocation_table.h, libfat/source/filetime.c,
		libfat/source/filetime.h, libfat/source/libfat.c,
		libfat/source/mem_allocate.h, libfat/source/partition.c,
		libfat/source/partition.h, libfat/source/disc_io/disc.c,
		libfat/source/disc_io/disc.h, libfat/source/disc_io/disc_io.h,
		libfat/source/disc_io/io_cf_common.c,
		libfat/source/disc_io/io_cf_common.h,
		libfat/source/disc_io/io_dldi.h, libfat/source/disc_io/io_dldi.s,
		libfat/source/disc_io/io_efa2.c, libfat/source/disc_io/io_efa2.h,
		libfat/source/disc_io/io_fcsr.c, libfat/source/disc_io/io_fcsr.h,
		libfat/source/disc_io/io_m3_common.c,
		libfat/source/disc_io/io_m3_common.h,
		libfat/source/disc_io/io_m3cf.c, libfat/source/disc_io/io_m3cf.h,
		libfat/source/disc_io/io_m3sd.c, libfat/source/disc_io/io_m3sd.h,
		libfat/source/disc_io/io_mpcf.c, libfat/source/disc_io/io_mpcf.h,
		libfat/source/disc_io/io_njsd.c, libfat/source/disc_io/io_njsd.h,
		libfat/source/disc_io/io_nmmc.c, libfat/source/disc_io/io_nmmc.h,
		libfat/source/disc_io/io_sc_common.c,
		libfat/source/disc_io/io_sc_common.h,
		libfat/source/disc_io/io_sccf.c, libfat/source/disc_io/io_sccf.h,
		libfat/source/disc_io/io_scsd.c, libfat/source/disc_io/io_scsd.h,
		libfat/source/disc_io/io_scsd_s.s,
		libfat/source/disc_io/io_sd_common.c,
		libfat/source/disc_io/io_sd_common.h, libnds/Makefile.arm7,
		libnds/Makefile.arm9, libnds/libnds_license.txt,
		libnds/basicARM7/source/defaultARM7.c,
		libnds/include/default_font_bin.h, libnds/include/gbfs.h,
		libnds/include/nds.h, libnds/include/nds/bios.h,
		libnds/include/nds/card.h, libnds/include/nds/dma.h,
		libnds/include/nds/interrupts.h, libnds/include/nds/ipc.h,
		libnds/include/nds/jtypes.h, libnds/include/nds/memory.h,
		libnds/include/nds/registers_alt.h, libnds/include/nds/reload.h,
		libnds/include/nds/system.h, libnds/include/nds/timers.h,
		libnds/include/nds/arm7/audio.h, libnds/include/nds/arm7/clock.h,
		libnds/include/nds/arm7/serial.h, libnds/include/nds/arm7/touch.h,
		libnds/include/nds/arm9/background.h,
		libnds/include/nds/arm9/boxtest.h, libnds/include/nds/arm9/cache.h,
		libnds/include/nds/arm9/console.h,
		libnds/include/nds/arm9/exceptions.h,
		libnds/include/nds/arm9/image.h, libnds/include/nds/arm9/input.h,
		libnds/include/nds/arm9/math.h, libnds/include/nds/arm9/ndsmotion.h,
		libnds/include/nds/arm9/pcx.h, libnds/include/nds/arm9/postest.h,
		libnds/include/nds/arm9/rumble.h, libnds/include/nds/arm9/sound.h,
		libnds/include/nds/arm9/sprite.h, libnds/include/nds/arm9/trig_lut.h,
		libnds/include/nds/arm9/video.h, libnds/include/nds/arm9/videoGL.h,
		libnds/source/arm7/audio.c, libnds/source/arm7/clock.c,
		libnds/source/arm7/microphone.c, libnds/source/arm7/spi.c,
		libnds/source/arm7/touch.c, libnds/source/arm7/userSettings.c,
		libnds/source/arm9/COS.bin, libnds/source/arm9/COS.s,
		libnds/source/arm9/SIN.bin, libnds/source/arm9/SIN.s,
		libnds/source/arm9/TAN.bin, libnds/source/arm9/TAN.s,
		libnds/source/arm9/boxtest.c, libnds/source/arm9/console.c,
		libnds/source/arm9/dcache.s, libnds/source/arm9/default_font.bin,
		libnds/source/arm9/default_font.s,
		libnds/source/arm9/exceptionHandler.S,
		libnds/source/arm9/exceptionHandler.s,
		libnds/source/arm9/exceptions.c, libnds/source/arm9/gurumeditation.c,
		libnds/source/arm9/icache.s, libnds/source/arm9/image.c,
		libnds/source/arm9/initSystem.c, libnds/source/arm9/keys.c,
		libnds/source/arm9/ndsmotion.c, libnds/source/arm9/pcx.c,
		libnds/source/arm9/rumble.c, libnds/source/arm9/sound.c,
		libnds/source/arm9/system.c, libnds/source/arm9/touch.c,
		libnds/source/arm9/video.c, libnds/source/arm9/videoGL.c,
		libnds/source/common/biosCalls.s, libnds/source/common/card.c,
		libnds/source/common/gbfs.c,
		libnds/source/common/interruptDispatcher.s,
		libnds/source/common/interrupts.c, rtc/rtc.c, sound/sound.c,
		sound/sound.h, start/start.S, startup/linkcmds, startup/start.c,
		timer/timer.c, tools/Makefile.am, tools/bin2s, tools/bin2s.c,
		tools/configure.ac, tools/runtest,
		tools/ndstool/include/arm7_sha1_homebrew.h,
		tools/ndstool/include/arm7_sha1_nintendo.h,
		tools/ndstool/include/banner.h, tools/ndstool/include/bigint.h,
		tools/ndstool/include/crc.h, tools/ndstool/include/default_icon.h,
		tools/ndstool/include/encryption.h, tools/ndstool/include/header.h,
		tools/ndstool/include/hook.h, tools/ndstool/include/little.h,
		tools/ndstool/include/loadme.h, tools/ndstool/include/logo.h,
		tools/ndstool/include/ndscreate.h,
		tools/ndstool/include/ndsextract.h, tools/ndstool/include/ndstool.h,
		tools/ndstool/include/ndstree.h, tools/ndstool/include/overlay.h,
		tools/ndstool/include/passme.h, tools/ndstool/include/passme_sram.h,
		tools/ndstool/include/passme_vhd1.h,
		tools/ndstool/include/passme_vhd2.h, tools/ndstool/include/raster.h,
		tools/ndstool/include/sha1.h, tools/ndstool/include/types.h,
		tools/ndstool/source/arm7_sha1_homebrew.c,
		tools/ndstool/source/arm7_sha1_nintendo.c,
		tools/ndstool/source/banner.cpp, tools/ndstool/source/bigint.cpp,
		tools/ndstool/source/compile_date.c, tools/ndstool/source/crc.cpp,
		tools/ndstool/source/default_icon.c,
		tools/ndstool/source/encryption.cpp, tools/ndstool/source/header.cpp,
		tools/ndstool/source/hook.cpp, tools/ndstool/source/loadme.c,
		tools/ndstool/source/logo.cpp, tools/ndstool/source/ndscodes.cpp,
		tools/ndstool/source/ndscreate.cpp,
		tools/ndstool/source/ndsextract.cpp,
		tools/ndstool/source/ndstool.cpp, tools/ndstool/source/ndstree.cpp,
		tools/ndstool/source/passme.cpp, tools/ndstool/source/passme_sram.c,
		tools/ndstool/source/raster.cpp, tools/ndstool/source/sha1.cpp,
		touchscreen/README.reco, touchscreen/parser.c, touchscreen/reco.c,
		touchscreen/reco.h, touchscreen/touchscreen.c,
		touchscreen/touchscreen.h, wifi/compat.c, wifi/compat.h, wifi/wifi.c:
		New files.
	
	Revision 1.14  2006/02/25 02:18:53  wntrmute
	doxygenation updates
	
	Revision 1.13  2006/01/12 09:10:47  wntrmute
	Added key repeat as suggested by pepsiman
	
	Revision 1.12  2006/01/10 05:50:24  dovoto
	uhmm...about that KEY_R vs KEY_L thing...lets pretend that never happened
	
	Revision 1.11  2006/01/10 05:45:24  dovoto
	KEY_L and KEY_R were swapped
	
	Revision 1.10  2005/11/27 07:55:14  joatski
	Fixed my mistake in the changelogs
	
	Revision 1.9  2005/11/27 07:48:45  joatski
	Changed function returns to uint32
	
	Revision 1.8  2005/11/14 11:59:49  wntrmute
	reformat for consistency
	
	Revision 1.7  2005/11/03 23:38:49  wntrmute
	don't use enum for key function returns
	
	Revision 1.6  2005/10/18 04:17:04  wntrmute
	doxygenation
	
	Revision 1.5  2005/10/13 16:30:11  dovoto
	Changed KEYPAD_BITS to a typedef enum, this resolved some issues with multiple redefinition of KEYPAD_BITS (although this error did not allways occur).
	
	Revision 1.4  2005/10/03 21:21:21  wntrmute
	doxygenation
	
	Revision 1.3  2005/09/07 18:03:36  wntrmute
	renamed key input registers
	moved key bit definitions to input.h
	
	Revision 1.2  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.1  2005/08/03 17:37:28  wntrmute
	moved to input.h

	Revision 1.5  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging

	Revision 1.4  2005/07/25 02:31:07  wntrmute
	made local variables static
	added proper header to keys.h

	Revision 1.3  2005/07/25 02:19:01  desktopman
	Added support for KEY_LID in keys.c.
	Moved KEYS_CUR from header to source file.
	Changed from the custom abs() to stdlib.h's abs().

	Revision 1.2  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib


---------------------------------------------------------------------------------*/
//!	NDS input support.
/*! \file input.h
*/

//---------------------------------------------------------------------------------
#ifndef	INPUT_HEADER_INCLUDE
#define	INPUT_HEADER_INCLUDE
//---------------------------------------------------------------------------------

#include <nds/jtypes.h>

// Keyboard

//!	Bit values for the keypad buttons.
typedef enum KEYPAD_BITS {
  KEY_A      = BIT(0),  //!< Keypad A button.
  KEY_B      = BIT(1),  //!< Keypad B button.
  KEY_SELECT = BIT(2),  //!< Keypad SELECT button.
  KEY_START  = BIT(3),  //!< Keypad START button.
  KEY_RIGHT  = BIT(4),  //!< Keypad RIGHT button.
  KEY_LEFT   = BIT(5),  //!< Keypad LEFT button.
  KEY_UP     = BIT(6),  //!< Keypad UP button.
  KEY_DOWN   = BIT(7),  //!< Keypad DOWN button.
  KEY_R      = BIT(8),  //!< Right shoulder button.
  KEY_L      = BIT(9),  //!< Left shoulder button.
  KEY_X      = BIT(10), //!< Keypad X button.
  KEY_Y      = BIT(11), //!< Keypad Y button.
  KEY_TOUCH  = BIT(12), //!< Touchscreen pendown.
  KEY_LID    = BIT(13)  //!< Lid state.
} KEYPAD_BITS;

#ifdef __cplusplus
extern "C" {
#endif

//!	Obtains the current keypad state.
/*!	Call this function once per main loop in order to use the keypad functions.
*/
void scanKeys(void);

//!	Obtains the current keypad held state.
uint32 keysHeld(void);

//!	Obtains the current keypad pressed state.
uint32 keysDown(void);

//!	Obtains the current keypad pressed or repeating state.
uint32 keysDownRepeat(void);

//!	Sets the key repeat parameters.
/*!	\param setDelay Number of %scanKeys calls before keys start to repeat.
	\param setRepeat Number of %scanKeys calls before keys repeat.
*/
void keysSetRepeat( u8 setDelay, u8 setRepeat );

//! Obtains the current keypad released state.
uint32 keysUp(void);

//!	Obtains the current touchscreen co-ordinates.
touchPosition touchReadXY(void);

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif // INPUT_HEADER_INCLUDE
//---------------------------------------------------------------------------------
