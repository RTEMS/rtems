/*
	io_nmmc.c

	Hardware Routines for reading an SD or MMC card using
	a Neoflash MK2 or MK3.

	Written by www.neoflash.com

	Submit bug reports for this device to the NeoFlash forums

	See license.txt for license details.

	2006-02-09 - www.neoflash.com:
	 * First stable release

	2006-02-13 - Chishm
	 * Added ReadMK2Config function
	 * Added read config test to init function so no unnecessary card commands are sent
	 * Changed data read and write functions to use multiple block commands
*/

#include "io_nmmc.h"

#ifdef NDS

#include <nds/card.h>

int _NMMC_spi_freq = 3;

#define MK2_CONFIG_ZIP_RAM_CLOSE		(1 << 5)
#define MK2_CONFIG_GAME_FLASH_CLOSE		((1 << 4) | (1 << 0))
//#define MK2_CONFIG_ZIP_RAM_CLOSE		((1 << 5) | (1 << 1))
//#define MK2_CONFIG_GAME_FLASH_CLOSE		(1 << 4)

#define MMC_READ_MULTIPLE_BLOCK		18
#define MMC_READ_BLOCK		17
#define MMC_WRITE_MULTIPLE_BLOCK	25
#define MMC_WRITE_BLOCK		24
#define MMC_STOP_TRANSMISSION	12
#define MMC_SET_BLOCKLEN	16
#define MMC_SET_BLOCK_COUNT	23
#define MMC_SEND_CSD	9

// SPI functions

static inline void _Neo_OpenSPI( u8 frequency )
{
	CARD_CR1 = 0x0000A040 | frequency;
}

static inline u8 _Neo_SPI( u8 dataByte )
{
	CARD_EEPDATA = dataByte;
	while (CARD_CR1 & 0x80);		// card busy
	return CARD_EEPDATA;
}

static inline void _Neo_CloseSPI ( void )
{
	CARD_CR1 = 0;
}

static inline void _Neo_MK2GameMode(void)	{
	_Neo_OpenSPI(_NMMC_spi_freq);				// Enable DS Card's SPI port
	_Neo_SPI(0xF1);				// Switch to game mode
	_Neo_CloseSPI();				// Disable DS Card's SPI port
}

static inline void _Neo_EnableEEPROM( bool enable )	{
	_Neo_OpenSPI(_NMMC_spi_freq);
	if(enable)	_Neo_SPI(0x06);
	else 		_Neo_SPI(0x0E);
	_Neo_CloseSPI();
}

static void _Neo_WriteMK2Config(u8 config) {
	_Neo_EnableEEPROM(true);
	_Neo_OpenSPI(_NMMC_spi_freq);
	_Neo_SPI(0xFA);					// Send mem conf write command
	_Neo_SPI(0x01);					// Send high byte (0x01)
	_Neo_SPI(config);				// Send low byte
	_Neo_CloseSPI();
	_Neo_EnableEEPROM(false);
}

static u8 _Neo_ReadMK2Config(void)
{
	u8 config;
	_Neo_EnableEEPROM(true);
	_Neo_OpenSPI(_NMMC_spi_freq);
	_Neo_SPI(0xf8);					// Send mem conf read command
	_Neo_SPI(0x01);					// Send high byte
	config = _Neo_SPI(0x00);			// Get low byte
	_Neo_CloseSPI();
	_Neo_EnableEEPROM(false);
	return config;
}

// Low level functions

u8 selectMMC_command [8] = {0xFF, 0x00, 0x6A, 0xDF, 0x37, 0x59, 0x33, 0xA3};

static void _Neo_SelectMMC (u8 dataByte)
{
	selectMMC_command[1] = dataByte;	// Set enable / disable byte
	cardWriteCommand (selectMMC_command);	// Send "5. Use the EEPROM CS to access the MK2 MMC/SD card"
	CARD_CR2 = CARD_ACTIVATE | CARD_nRESET;
	while (CARD_CR2 & CARD_BUSY);
	return;
}

static void _Neo_EnableMMC( bool enable )
{
	if ( enable == false) {
		_Neo_CloseSPI ();
		_Neo_SelectMMC (0);
		_Neo_SelectMMC (0);
	} else {
		_Neo_SelectMMC (1);
		_Neo_SelectMMC (1);
		_Neo_OpenSPI (_NMMC_spi_freq);
	}
	return;
}

static void _Neo_SendMMCCommand( u8 command, u32 argument )
{
	_Neo_SPI (0xFF);
	_Neo_SPI (command | 0x40);
	_Neo_SPI ((argument >> 24) & 0xff);
	_Neo_SPI ((argument >> 16) & 0xff);
	_Neo_SPI ((argument >> 8) & 0xff) ;
	_Neo_SPI (argument & 0xff);
	_Neo_SPI (0x95);
	_Neo_SPI (0xFF);
	return;
}

static bool _Neo_CheckMMCResponse( u8 response, u8 mask )	{
	u32 i;
	for(i=0;i<256;i++)	{
		if( ( _Neo_SPI( 0xFF ) & mask ) == response )
			return true;
	}
	return false;
}

// Neo MMC functions

static bool _Neo_InitMMC(void)	{
	_Neo_MK2GameMode();
	_Neo_WriteMK2Config( MK2_CONFIG_ZIP_RAM_CLOSE | MK2_CONFIG_GAME_FLASH_CLOSE);

	// Make sure the configuration was accepted
	if (_Neo_ReadMK2Config() != (MK2_CONFIG_ZIP_RAM_CLOSE | MK2_CONFIG_GAME_FLASH_CLOSE)) {
		return false;	// If not, then it wasn't initialised properly
	}

	return true;
}

// Neo MMC driver functions

bool _NMMC_isInserted(void)	{
	int i;

	_Neo_EnableMMC( true );		// Open SPI port to MMC card
	_Neo_SendMMCCommand(MMC_SEND_CSD, 0);
	if( _Neo_CheckMMCResponse( 0x00, 0xFF ) == false )	{				// Make sure no errors occured
		_Neo_EnableMMC( false );
		return false;
	}
	if( _Neo_CheckMMCResponse( 0xFE, 0xFF ) == false )	{			// Check for Start Block token
		_Neo_EnableMMC( false );
		return false;
	}

	// consume data from card, and send clocks.
	for (i = 0; i < 28; i++) {
		_Neo_SPI(0xff);
	}

	return true;
}

bool _NMMC_clearStatus (void) {
	u32 i;

	_Neo_EnableMMC( true );		// Open SPI port to MMC card
	for (i = 0; i < 10; i++) {
		_Neo_SPI(0xFF);			// Send 10 0xFF bytes to MMC card
	}
	_Neo_SendMMCCommand(0, 0);	// Send GO_IDLE_STATE command
	if( _Neo_CheckMMCResponse( 0x01, 0xFF ) == false )	{		// Check that it replied with 0x01 (not idle, no other error)
		_Neo_EnableMMC( false );
		return false;
	}
	for(i=0;i<256;i++)	{
		_Neo_SendMMCCommand(1, 0);	// Poll with SEND_OP_COND
		if( _Neo_CheckMMCResponse( 0x00, 0x01 ) == true ) {	// Check for idle state
			_Neo_EnableMMC( false );				// Close SPI port to MMC card
			return true;						// Card is now idle
		}
	}
	_Neo_EnableMMC( false );
	return false;
}

bool _NMMC_shutdown(void) {
	return _NMMC_clearStatus();
}

bool _NMMC_startUp(void) {
	int i;
	int transSpeed;
	if (_Neo_InitMMC() == false) {
		return false;
	}
	if (_NMMC_clearStatus() == false) {
		return false;
	}
	_Neo_EnableMMC( true );		// Open SPI port to MMC card

	// Set block length
	_Neo_SendMMCCommand(MMC_SET_BLOCKLEN, BYTES_PER_READ );
	if( _Neo_CheckMMCResponse( 0x00, 0xFF ) == false )	{				// Make sure no errors occured
		_Neo_EnableMMC( false );
		return false;
	}

	// Check if we can use a higher SPI frequency
	_Neo_SendMMCCommand(MMC_SEND_CSD, 0);
	if( _Neo_CheckMMCResponse( 0x00, 0xFF ) == false )	{				// Make sure no errors occured
		_Neo_EnableMMC( false );
		return false;
	}
	if( _Neo_CheckMMCResponse( 0xFE, 0xFF ) == false )	{			// Check for Start Block token
		_Neo_EnableMMC( false );
		return false;
	}
	for (i = 0; i < 3; i++) {
		_Neo_SPI(0xFF);
	}
	transSpeed = _Neo_SPI (0xFF);
	for (i = 0; i < 24; i++) {
		_Neo_SPI(0xFF);
	}
	if ((transSpeed & 0xf0) >= 0x30) {
		_NMMC_spi_freq = 0;
	}

	_Neo_EnableMMC( false );

	return true;
}


bool _NMMC_writeSectors (u32 sector, u32 totalSecs, const void* buffer)
{
	u32 i;
	u8 *p=(u8*)buffer;

	sector *= BYTES_PER_READ;

	_Neo_EnableMMC( true );												// Open SPI port to MMC card
	_Neo_SendMMCCommand( 25, sector );
	if( _Neo_CheckMMCResponse( 0x00, 0xFF ) == false )	{				// Make sure no errors occured
		_Neo_EnableMMC( false );
		return false;
	}

	while (totalSecs--) {
		_Neo_SPI( 0xFC );												// Send Start Block token
		for( i = 0; i < BYTES_PER_READ; i++ )							// Send a block of data
			_Neo_SPI( *p++ );
		_Neo_SPI( 0xFF );												// Send fake CRC16
		_Neo_SPI( 0xFF );												// Send fake CRC16

		if( ( _Neo_SPI( 0xFF ) & 0x0F ) != 0x05 )	{					// Make sure the block was accepted
			_Neo_EnableMMC( false );
			return false;
		}
		while( _Neo_SPI( 0xFF ) == 0x00 );								// Wait for the block to be written
	}

	// Stop transmission block
	_Neo_SPI( 0xFD );													// Send Stop Transmission Block token
	for( i = 0; i < BYTES_PER_READ; i++ )								// Send a block of fake data
		_Neo_SPI( 0xFF );
	_Neo_SPI( 0xFF );													// Send fake CRC16
	_Neo_SPI( 0xFF );													// Send fake CRC16

	_Neo_SPI (0xFF); 													// Send 8 clocks
	while( _Neo_SPI( 0xFF ) == 0x00 );									// Wait for the busy signal to clear


	for ( i = 0; i < 0x10; i++) {
		_Neo_SPI (0xFF);													// Send clocks for the MMC card to finish what it's doing
	}

	_Neo_EnableMMC( false );											// Close SPI port to MMC card
	return true;
}

bool _NMMC_readSectors (u32 sector, u32 totalSecs, void* buffer)
{
	u32 i;
	u8 *p=(u8*)buffer;

	sector *= BYTES_PER_READ;

	_Neo_EnableMMC( true );												// Open SPI port to MMC card

	while (totalSecs--) {
		_Neo_SendMMCCommand(MMC_READ_BLOCK, sector );
		if( _Neo_CheckMMCResponse( 0x00, 0xFF ) == false )	{			// Make sure no errors occured
			_Neo_EnableMMC( false );
			return false;
		}

		if( _Neo_CheckMMCResponse( 0xFE, 0xFF ) == false )	{			// Check for Start Block token
			_Neo_EnableMMC( false );
			return false;
		}
		for( i = 0; i < BYTES_PER_READ; i++ )							// Read in a block of data
			*p++ = _Neo_SPI( 0xFF );
		_Neo_SPI( 0xFF );												// Ignore CRC16
		_Neo_SPI( 0xFF );												// Ignore CRC16
		sector += BYTES_PER_READ;
	}

	_Neo_EnableMMC( false );											// Close SPI port to MMC card
	return true;
}


const IO_INTERFACE _io_nmmc = {
	DEVICE_TYPE_NMMC,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS,
	(FN_MEDIUM_STARTUP)&_NMMC_startUp,
	(FN_MEDIUM_ISINSERTED)&_NMMC_isInserted,
	(FN_MEDIUM_READSECTORS)&_NMMC_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_NMMC_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_NMMC_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_NMMC_shutdown
} ;

#endif // defined NDS
