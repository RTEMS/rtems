/**
 * @file
 *
 * @ingroup kinetis_sd_card
 *
 * @brief sd card driver(spi interface).
 */

/*
 * Copyright (c) 2023 Yu Zhennan
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <bsp/spi_sd.h>
#include <bsp/pinout.h>
#include <bsp/spi.h>

//--------------------------------------------------------------
unsigned int SD1_Addr_Mode=0; // 1 - accessed by block 0 - accessed by byte
unsigned int SD1_Ver=0; //SD Card version
//---------------------------------------------------------------

#define SD1_SPI_SPEED_HIGH() spi_baudrate_config(SPI1, 800000)

#define SD1_SPI_SPEED_LOW()  spi_baudrate_config(SPI1, 240000)

#define SD1_SPI_WByte(x) spi_send_receive(SPI1, x)

#define SD1_SPI_RByte()  spi_send_receive(SPI1, 0XFF)


#define SD1_SPI_CS_ENABLE()  gpio_clear_bit(PTE, IO_4)
#define SD1_SPI_CS_DISABLE() gpio_set_bit(PTE, IO_4)

static int sd_spi_init(void)
{
	  spi_clock_enable( SPI1 );
	  spi_init( SPI1, 240000 );
    return 0;
}

static int sd_write_command(unsigned char *pcmd) 
{
 unsigned char r=0,time=0;
 
 SD1_SPI_CS_DISABLE();

 SD1_SPI_WByte(0xFF); // Send 8 cycles for compatibility,or else some sd cards may dont't work
	
 SD1_SPI_CS_ENABLE();
 while(0XFF!=SD1_SPI_RByte()); // Waiting for sd card get ready

 SD1_SPI_WByte(pcmd[0]);
 SD1_SPI_WByte(pcmd[1]);
 SD1_SPI_WByte(pcmd[2]);
 SD1_SPI_WByte(pcmd[3]);
 SD1_SPI_WByte(pcmd[4]);
 SD1_SPI_WByte(pcmd[5]);
	
 if(pcmd[0]==0X1C) SD1_SPI_RByte(); // if got stop command, skip remained bytes

 do 
 {  
  r=SD1_SPI_RByte();
  time++;
 }while((r&0X80)&&(time<TRY_TIME)); // if tried more than TRY_TIME , then return error

 return r;
}

int sd_init(void)
{
 unsigned char time=0,r=0,i=0;
	
 unsigned char rbuf[4]={0};
	
 unsigned char pCMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0: Switched work mode from sd to spi ,then sd card will be idle
 unsigned char pCMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1: Initialize mmc card
 unsigned char pCMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8: Get version of sd card to know its voltage
 unsigned char pCMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16: Set sector size to be 512 bytes(Test the card by the way)
                                                          //This should work if card initialization is ok
 unsigned char pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55: Tell the card that ACMD41 will come
                                                          //MMC is initialized by CMD1, sd card use CMD55+ACMD41
 unsigned char pACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41: This is prepared for sd 2.0 (not for mmc)
 unsigned char pACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41: This is prepared for sd 1.0 (not for mmc)

 unsigned char pCMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58: Check if the card is sdhc or normal.Both are sd2.0 but their sector addressing methods are different.
 unsigned char pCMD59[6]={0x7B,0x00,0x00,0x00,0x00,0x01};
 sd_spi_init();

 SD1_SPI_SPEED_LOW(); // First slow down the card
	
 SD1_SPI_CS_DISABLE(); 
	
 for(i=0;i<0x0f;i++) // Here should send at list 74 cycles to activate the card
 {
  SD1_SPI_WByte(0xff); //120 cycles
 }

 time=0;
 do
 { 
  r=sd_write_command(pCMD0); // Write CMD0
  time++;
  if(time>=TRY_TIME) 
  { 
   return(INIT_CMD0_ERROR);
  }
 }while(r!=0x01);
 
 if(1==sd_write_command(pCMD8))// Write CMD8, it should be sd2.0 if it return 1
 {
	rbuf[0]=SD1_SPI_RByte(); rbuf[1]=SD1_SPI_RByte(); // Read 4 bytes to know if the card can work at 2.7~3.6V
	rbuf[2]=SD1_SPI_RByte(); rbuf[3]=SD1_SPI_RByte();
	 
	if(rbuf[2]==0X01 && rbuf[3]==0XAA)//SD can work at 2.7~3.6V
	{		
	 time=0;
	 do
	 {
		sd_write_command(pCMD55);// Write CMD55
		r=sd_write_command(pACMD41H);// Write ACMD41 (dedicated for SD2.0)
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV2_ACMD41_ERROR);
    }
   }while(r!=0);	

   if(0==sd_write_command(pCMD58)) // Write CMD58 to check SD2.0
   {
	  rbuf[0]=SD1_SPI_RByte(); rbuf[1]=SD1_SPI_RByte(); // Read 4 bytes as OCR where the CCS indicate sdhc
	  rbuf[2]=SD1_SPI_RByte(); rbuf[3]=SD1_SPI_RByte();

    if(rbuf[0]&0x40) 
		{
		 SD1_Ver=SD_VER_V2HC; // Confirmed SDHC
		 SD1_Addr_Mode=1; //SDHC use sector number as address
		}	
    else SD1_Ver=SD_VER_V2; // Confirmed normal card
   }
  }
 }
 else // The card may be SD1.0 or MMC
 {
	// ACMD41 for SD card, and CMD1 for MMC
	sd_write_command(pCMD55);//Write CMD55
	r=sd_write_command(pACMD41S);//Write ACMD41 (dedicated for SD1.0)
    
  if(r<=1) // If true then we get SD card
  {
	 SD1_Ver=SD_VER_V1; //Generally SD1.0 is no more than 2G
			
	 time=0;
	 do
	 {
		sd_write_command(pCMD55);//Write CMD55
		r=sd_write_command(pACMD41S);//Write ACMD41 (dedicated for SD1.0)
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV1_ACMD41_ERROR);
    }
   }while(r!=0);			 
  }
  else // Or else we get MMC
	{
	 SD1_Ver=SD_VER_MMC;
			
	 time=0;
   do
   { 
    r=sd_write_command(pCMD1);//Write CMD1
    time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_CMD1_ERROR);
    }
   }while(r!=0);			
  }
 }
 if(0!=sd_write_command(pCMD59)) {return INIT_ERROR;};
 if(0!=sd_write_command(pCMD16)) //SD card block size is 512 bytes
 {
	SD1_Ver=SD_VER_ERR; // Unrecognized card
	return INIT_ERROR;
 }	
 
 SD1_SPI_CS_DISABLE();
 SD1_SPI_WByte(0xFF); // Here give 8 cycles as timing complement
 
 SD1_SPI_SPEED_HIGH(); // Now speed up the card
 
 return 0;
}

int sd_erase_nsector(unsigned int addr_sta,unsigned int addr_end)
{
 unsigned char r,time;
 unsigned char pCMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //Set start sector to be erased
 unsigned char pCMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //Set end sector to be erased
 unsigned char pCMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //Erase sectors

 if(!SD1_Addr_Mode) {addr_sta<<=9;addr_end<<=9;} //addr = addr * 512 transform sector to bytes

 pCMD32[1]=addr_sta>>24; //Assemble CMD32 sequence with start address
 pCMD32[2]=addr_sta>>16;
 pCMD32[3]=addr_sta>>8;
 pCMD32[4]=addr_sta;	 

 pCMD33[1]=addr_end>>24; //Assemble CMD33 sequence with start address
 pCMD33[2]=addr_end>>16;
 pCMD33[3]=addr_end>>8;
 pCMD33[4]=addr_end;	

 time=0;
 do
 {  
  r=sd_write_command(pCMD32);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r);
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=sd_write_command(pCMD33);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r);
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=sd_write_command(pCMD38);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r);
  }
 }while(r!=0);

 return 0; 

}

int sd_write_sector(unsigned int addr,unsigned char *buffer)	//Write 512 bytes to specified sector(using CMD24)
{  
 unsigned char r,time;
 unsigned char i=0;
 unsigned char pCMD24[]={0x58,0x00,0x00,0x00,0x00,0xff};

 if(!SD1_Addr_Mode) addr<<=9; // Transform sector to bytes

 pCMD24[1]=addr>>24; //Assemble CMD24 sequence with byte address
 pCMD24[2]=addr>>16;
 pCMD24[3]=addr>>8;
 pCMD24[4]=addr;	

 time=0;
 do
 {  
  r=sd_write_command(pCMD24);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r);
  }
 }while(r!=0); 

 while(0XFF!=SD1_SPI_RByte()); //Waiting for SD card get ready
 SD1_SPI_WByte(0xFE);//Write 0xfe as head
	
 for(i=0;i<4;i++) // Do more spi transfers in one loop to make less loops
 {
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
 }
  
 SD1_SPI_WByte(0xFF); 
 SD1_SPI_WByte(0xFF); //two byte CRC , don't worry
       
 r=SD1_SPI_RByte();
 if((r & 0x1F)!=0x05) //If get XXX00101 , it means that data was stored into sd card
 {
  return(WRITE_BLOCK_ERROR);
 }
 
 while(0xFF!=SD1_SPI_RByte());//Waiting for SD card get free
						                  //0x00 - busy  0xff - free

 SD1_SPI_CS_DISABLE();
 SD1_SPI_WByte(0xFF);
 
 return(0);
} 

int sd_read_sector(unsigned int addr,unsigned char *buffer)// Read 512 bytes from specified sector (using CMD17)
{
 unsigned char i;
 unsigned char time,r;
	
 unsigned char pCMD17[]={0x51,0x00,0x00,0x00,0x00,0x01};
   
 if(!SD1_Addr_Mode) addr<<=9;

 pCMD17[1]=addr>>24; //Assemble CMD17 sequence with byte address
 pCMD17[2]=addr>>16;
 pCMD17[3]=addr>>8;
 pCMD17[4]=addr;	

 time=0;
 do
 {  
  r=sd_write_command(pCMD17); //Write CMD17
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_BLOCK_ERROR);
  }
 }while(r!=0); 
   			
 while(SD1_SPI_RByte()!= 0xFE); // When got 0xfe, another 512 bytes will come

 for(i=0;i<4;i++)
 {	
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
 }

 SD1_SPI_RByte();
 SD1_SPI_RByte();//Read tow bytes as CRC, don't worry

 SD1_SPI_CS_DISABLE();
 SD1_SPI_WByte(0xFF); //Here give 8 cycles as complement 

 return 0;
}

int sd_write_nsector(unsigned int nsec,unsigned int addr,unsigned char *buffer)	
{  
 unsigned char r,time;
 unsigned int i=0,j=0;
	
 unsigned char pCMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25 for writing on continous blocks
 unsigned char pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55 for indicating that the later is ACMD(CMD55+ACMD23)
 unsigned char pACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23 for early erasing continous blocks

 if(!SD1_Addr_Mode) addr<<=9; 

 pCMD25[1]=addr>>24;
 pCMD25[2]=addr>>16;
 pCMD25[3]=addr>>8;
 pCMD25[4]=addr;

 pACMD23[1]=nsec>>24;
 pACMD23[2]=nsec>>16;
 pACMD23[3]=nsec>>8;
 pACMD23[4]=nsec; 

 if(SD1_Ver!=SD_VER_MMC) //If not MMC, then writing CMD55+ACMD23 at first, as which the later continous writing blocks will be more fast
 {
	sd_write_command(pCMD55);
	sd_write_command(pACMD23);
 }

 time=0;
 do
 {  
  r=sd_write_command(pCMD25);
  time++;
  if(time==TRY_TIME) 
  { 
   return(WRITE_CMD25_ERROR);
  }
 }while(r!=0); 

 while(0XFF!=SD1_SPI_RByte()); //Waiting for SD card get ready

 for(j=0;j<nsec;j++)
 {
  SD1_SPI_WByte(0xFC);//Write 0xfc which will be followd by 512 bytes
	
  for(i=0;i<4;i++)
  {
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  }
  
  SD1_SPI_WByte(0xFF); 
  SD1_SPI_WByte(0xFF); //Write two bytes as CRC, dont't worry
       
  r=SD1_SPI_RByte();
  if((r & 0x1F)!=0x05) //Check if data were into SD card
  {
   return(WRITE_NBLOCK_ERROR);
  }
 
  while(0xFF!=SD1_SPI_RByte());
 }

 SD1_SPI_WByte(0xFD);

 while(0xFF!=SD1_SPI_RByte());

 SD1_SPI_CS_DISABLE();

 SD1_SPI_WByte(0xFF);

 return(0);
} 

int sd_read_nsector(unsigned int nsec,unsigned int addr,unsigned char *buffer)
{
 unsigned char r,time;
 unsigned int i=0,j=0;
	
 unsigned char pCMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; // CMD18 
 unsigned char pCMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; // CMD12 for forcing card to stop
   
 if(!SD1_Addr_Mode) addr<<=9;

 pCMD18[1]=addr>>24; //Assemble CMD18 sequence with byte address
 pCMD18[2]=addr>>16;
 pCMD18[3]=addr>>8;
 pCMD18[4]=addr;	

 time=0;
 do
 {  
  r=sd_write_command(pCMD18); //Write CMD18
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_CMD18_ERROR);
  }
 }while(r!=0); 
 
 for(j=0;j<nsec;j++)
 {  
  while(SD1_SPI_RByte()!= 0xFE);
 
  for(i=0;i<4;i++)
  {	
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  }
 
  SD1_SPI_RByte();
  SD1_SPI_RByte();
 }

 sd_write_command(pCMD12); //Write CMD12 

 SD1_SPI_CS_DISABLE();
 SD1_SPI_WByte(0xFF);

 return 0;
}

int sd_get_total_sectors_num(void)
{
 unsigned char pCSD[16];
 unsigned int Capacity;  
 unsigned char n,i;
 unsigned short csize; 

 unsigned char pCMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	

 if(sd_write_command(pCMD9)!=0) //Write CMD9
 {
	return GET_CSD_ERROR;
 }

 while(SD1_SPI_RByte()!= 0xFE); //When got 0xfe, it will be followd by 16 bytes CSD

 for(i=0;i<16;i++) pCSD[i]=SD1_SPI_RByte(); //Reading CSD

 SD1_SPI_RByte();
 SD1_SPI_RByte(); //Read two bytes as CRC

 SD1_SPI_CS_DISABLE();
 SD1_SPI_WByte(0xFF); 
	
 //As for SDHC, its capacity can be as below
 if((pCSD[0]&0xC0)==0x40)	 //SD2.0
 {	
	csize=pCSD[9]+(((unsigned short)(pCSD[8]))<<8)+1;
  Capacity=((unsigned int)csize)<<10;//Get sector number	   
 }
 else //SD1.0
 {	
	n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
	csize=(pCSD[8]>>6)+((unsigned short)pCSD[7]<<2)+((unsigned short)(pCSD[6]&0x03)<<0x0A)+1;
	Capacity=(unsigned int)csize<<(n-9);//Get sector number
 }
 return Capacity;
}
