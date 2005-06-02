#include <rtems.h>
#include <bsp.h>
#include <s3c2400.h>

void gp32_setFramebuffer(void *add) {
   uint32_t addr = (uint32_t) add;
   uint32_t LCDBANK  =  addr >> 22;
   uint32_t LCDBASEU = (addr & 0x3FFFFF) >> 1;
   uint32_t LCDBASEL;
   uint16_t OFFSIZE = 0;
   uint16_t PAGEWIDTH;
   uint16_t vidmode = ((rLCDCON1>>1) & 15)-8;
   const uint8_t faktor[]= {15,30,60,120,240};

   LCDBASEL  = LCDBASEU + 320*faktor[vidmode];
   PAGEWIDTH = faktor[vidmode];

   rLCDSADDR1 = (LCDBANK<<21) | (LCDBASEU<<0) ;
   rLCDSADDR2 = (LCDBASEL<<0) ;
   rLCDSADDR3 = (OFFSIZE<<11) | (PAGEWIDTH<<0) ;
}

short gp32_initFramebufferN(void *add,uint32_t bitmode,uint32_t refreshrate) {
   uint32_t addr = (uint32_t) add;
   uint32_t GPHCLK = get_HCLK();
   {  uint16_t BPPMODE = 12;     
      uint16_t CLKVAL = 3;       
      uint16_t ENVID = 1;
      uint16_t MMODE = 0;
      uint16_t PNRMODE = 3;
      switch (bitmode) {
         case 16 : BPPMODE=12; break;
         case  8 : BPPMODE=11; break;
         case  4 : BPPMODE=10; break;
         case  2 : BPPMODE= 9; break;
         case  1 : BPPMODE= 8; break;
	 default : BPPMODE= 12; break;
      }
      /* Get the correct CLKVAL for refreshrate
         works in all bitmodes now :)
      */
      if (refreshrate <  50) refreshrate = 50;
      if (refreshrate > 120) refreshrate =120;
      CLKVAL = (GPHCLK/(83385*2*refreshrate))-1;
      if (CLKVAL == 0) CLKVAL=1;
      refreshrate = GPHCLK / (83385*2*(CLKVAL+1));
      rLCDCON1 = (CLKVAL<<8) | (MMODE<<7) | (PNRMODE<<5) | (BPPMODE<<1) | (ENVID<<0) ;
   }
   {  uint16_t LINEVAL = 320-1;
      uint16_t VBPD = 1;
      uint16_t VFPD = 2;
      uint16_t VSPW = 1;
      rLCDCON2 = 0;
      rLCDCON2 = (VBPD<<24) | (LINEVAL<<14) | (VFPD<<6) | (VSPW<<0) ;
   }
   {  uint16_t HBPD = 6;
      uint16_t HFPD = 2;
      uint16_t HOZVAL = 240-1;
      rLCDCON3 = 0;
      rLCDCON3 = (HBPD<<19) | (HOZVAL<<8) | (HFPD<<0) ;
   }
   {  uint16_t ADDVAL = 0;
      uint16_t HSPW = 4;
      uint16_t MVAL = 0;
      uint16_t PALADDEN = 0;
      rLCDCON4 = 0;
      rLCDCON4 = (PALADDEN<<24) | (ADDVAL<<16) | (MVAL<<8) | (HSPW<<0) ;
   }
   {  uint16_t BSWP = 0;
      uint16_t ENLEND = 0;
      uint16_t HWSWP = 1;
      uint16_t INVENDLINE = 0;
      uint16_t INVVCLK = 1;
      uint16_t INVVD = 0;
      uint16_t INVVDEN = 0;
      uint16_t INVVFRAME = 1;
      uint16_t INVVLINE = 1;
      if (bitmode<16) { BSWP=1;HWSWP=0; }
      rLCDCON5 = 0;
      rLCDCON5 = (INVVCLK<<10) | (INVVLINE<<9) | (INVVFRAME<<8) | (INVVD<<7) | (INVVDEN<<6)
               | (INVENDLINE<<4) | (ENLEND<<2) | (BSWP<<1) | (HWSWP<<0) ;
   }
   gp32_setFramebuffer((uint32_t*)addr);

return refreshrate;
}

short gp32_initFramebufferBP(void *add,uint32_t bitmode,uint32_t refreshrate) {
  uint32_t addr = (uint32_t) add;
  uint32_t GPHCLK = get_HCLK();
  {  uint16_t BPPMODE = 12;     
     uint16_t CLKVAL = 3;       
     uint16_t ENVID = 1;
     uint16_t MMODE = 0;
     uint16_t PNRMODE = 3;
     switch (bitmode) {
        case 16 : BPPMODE=12; break;
        case  8 : BPPMODE=11; break;
        case  4 : BPPMODE=10; break;
        case  2 : BPPMODE= 9; break;
        case  1 : BPPMODE= 8; break;
     }
     /* Get the correct CLKVAL for refreshrate
        works in all bitmodes now :)
     */
     if (refreshrate <  50) refreshrate = 50;
     if (refreshrate > 120) refreshrate =120;
     CLKVAL = (GPHCLK/(109850*2*refreshrate))-1;
     if (CLKVAL == 0) CLKVAL=1;
     refreshrate = GPHCLK / (109850*2*(CLKVAL+1));
     rLCDCON1 = (CLKVAL<<8) | (MMODE<<7) | (PNRMODE<<5) | (BPPMODE<<1) | (ENVID<<0);
  }
  {  uint16_t LINEVAL = 320-1;
     uint16_t VBPD = 8;
     uint16_t VFPD = 2;
     uint16_t VSPW = 5;
     rLCDCON2 = 0;
     rLCDCON2 = (VBPD<<24) | (LINEVAL<<14) | (VFPD<<6) | (VSPW<<0);
  }
  {  uint16_t HBPD = 50;
     uint16_t HFPD = 2;
     uint16_t HOZVAL = 240-1;
     rLCDCON3 = 0;
     rLCDCON3 = (HBPD<<19) | (HOZVAL<<8) | (HFPD<<0);
  }
  {  uint16_t ADDVAL = 0;
     uint16_t HSPW = 30;
     uint16_t MVAL = 0;
     uint16_t PALADDEN = 0;
     rLCDCON4 = 0;
     rLCDCON4 = (PALADDEN<<24) | (ADDVAL<<16) | (MVAL<<8) | (HSPW<<0);
  }
  {  uint16_t BSWP = 0;
     uint16_t ENLEND = 0;
     uint16_t HWSWP = 1;
     uint16_t INVENDLINE = 0;
     uint16_t INVVCLK = 1;
     uint16_t INVVD = 0;
     uint16_t INVVDEN = 0;
     uint16_t INVVFRAME = 1;
     uint16_t INVVLINE = 1;
     if (bitmode<16) { BSWP=1;HWSWP=0; }
     rLCDCON5 = 0;
     rLCDCON5 = (INVVCLK<<10) | (INVVLINE<<9) | (INVVFRAME<<8) | (INVVD<<7) | (INVVDEN<<6)
              | (INVENDLINE<<4) | (ENLEND<<2) | (BSWP<<1) | (HWSWP<<0);
  }
  gp32_setFramebuffer((uint32_t*)addr);

return refreshrate;
}

void gp32_setPalette( unsigned char pos, uint16_t color) {
	unsigned long *palette=(unsigned long *)0x14A00400;
	while ((rLCDCON5>>19) == 2);
	palette[pos]=color;
}
