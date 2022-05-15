/**
  ******************************************************************************
  * @file    system_stm32h7xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from 
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32h7xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include <stm32h7xx_hal.h>

#define DATA_IN_ExtSRAM
#define DATA_IN_ExtSDRAM

void  SystemInit_ExtMemCtl(void)
{

  #define  FMC_BMAP_Value    0x02000000    /* FMC Bank Mapping 2 (SDRAM Bank2 remapped) */

  __IO uint32_t  tmp = 0;


  /********** SDRAM + SRAM ***********************************************************************/

  #if defined (DATA_IN_ExtSDRAM) && defined (DATA_IN_ExtSRAM)

  register uint32_t       tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t  index;
  
  /*-- I/O Ports Configuration ------------------------------------------------------*/

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface clock */
  RCC->AHB4ENR |= 0x000001F8;
  
  /* Delay after an RCC peripheral clock enabling */
  tmp = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);
  
  /* Connect PDx pins to FMC Alternate function */     
  GPIOD->AFR[0]  = 0x00CC00CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAAFAFA;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0F0F;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* Configure PDx pins in Pull-up */
  GPIOD->PUPDR   = 0x55550505;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAABEBA;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* Configure PEx pins in Pull-up */
  GPIOE->PUPDR   = 0x55554145;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCCC000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAABFFAAA;
  /* Configure PFx pins speed to 100 MHz */ 
  GPIOF->OSPEEDR = 0xFFC00FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* Configure PFx pins in Pull-up */
  GPIOF->PUPDR   = 0x55400555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0xC0000C0C;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0xBFEEFAAA;
  /* Configure PGx pins speed to 100 MHz */ 
  GPIOG->OSPEEDR = 0xC0330FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* Configure PGx pins in Pull-up */ 
  GPIOG->PUPDR   = 0x40110555;
  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0xCCC00000;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAAABFF;
  /* Configure PHx pins speed to 100 MHz */ 
  GPIOH->OSPEEDR = 0xFFFFFC00;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* Configure PHx pins in Pull-up */
  GPIOH->PUPDR   = 0x55555400;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0xFFEBAAAA;
  /* Configure PIx pins speed to 100 MHz */ 
  GPIOI->OSPEEDR = 0x003CFFFF;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* Configure PIx pins in Pull-up */
  GPIOI->PUPDR   = 0x00145555;

  /*-- FMC Configuration ------------------------------------------------------*/

  /* Enable the FMC/FSMC interface clock */
  (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));
  
  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1_R->BTCR[4]  = 0x00001091;
  FMC_Bank1_R->BTCR[5]  = 0x00110212;
  FMC_Bank1E_R->BWTR[4] = 0x0FFFFFFF;
  
  /* SDRAM Timing and access interface configuration */

  /*SDBank               = FMC_SDRAM_BANK2

    ColumnBitsNumber     = FMC_SDRAM_COLUMN_BITS_NUM_9           CC
    RowBitsNumber        = FMC_SDRAM_ROW_BITS_NUM_12             RR
    MemoryDataWidth      = FMC_SDRAM_MEM_BUS_WIDTH_32            MM
    InternalBankNumber   = FMC_SDRAM_INTERN_BANKS_NUM_4          N
    CASLatency           = FMC_SDRAM_CAS_LATENCY_2               LL   // 2 oder 3, s.u.
    WriteProtection      = FMC_SDRAM_WRITE_PROTECTION_DISABLE    W
    SDClockPeriod        = FMC_SDRAM_CLOCK_PERIOD_2              KK
    ReadBurst            = FMC_SDRAM_RBURST_ENABLE               B
    ReadPipeDelay        = FMC_SDRAM_RPIPE_DELAY_0               PP
  
    LoadToActiveDelay    = 2  -> 1     LLLL     TMRD
    ExitSelfRefreshDelay = 6  -> 5     EEEE     TXSR
    SelfRefreshTime      = 4  -> 3     SSSS     TRAS
    RowCycleDelay        = 6  -> 5     RRRR     TRC
    WriteRecoveryTime    = 2  -> 1     WWWW     TWR
    RPDelay              = 2  -> 1     PPPP     TRP
    RCDDelay             = 2  -> 1     CCCC     TRCD              */
  #if 0
  FMC_Bank5_6_R->SDCR[0] = 0x00005965;  // 0000 0000 0000 0000 0101 1001 0110 0101  Bank 1
                                        //                      PPB KKWL LNMM RRCC
  FMC_Bank5_6_R->SDCR[1] = 0x00005965;  // 0000 0000 0000 0000 0101 1001 0110 0101  Bank 2  // CAS Latency = 2
                                        //                            WL LNMM RRCC

  FMC_Bank5_6_R->SDTR[0] = 0x00105000;  // 0000 0000 0001 0000 0101 0000 0000 0000  Bank 1  // Original,
                                        //      CCCC PPPP WWWW RRRR SSSS EEEE LLLL          // mit CAS Latency = 2 (s.o.)
  FMC_Bank5_6_R->SDTR[1] = 0x01010351;  // 0000 0001 0000 0001 0000 0011 0101 0001  Bank 2
                                        //      CCCC      WWWW      SSSS EEEE LLLL
  #endif
  #if 0
  FMC_Bank5_6_R->SDTR[0] = 0x00206000;  // 0000 0000 0010 0000 0110 0000 0000 0000  Bank 1  // Original + 1 bei allen Werten,
                                        //      CCCC PPPP WWWW RRRR SSSS EEEE LLLL          // mit CAS Latency = 3 (s.o.)
  FMC_Bank5_6_R->SDTR[1] = 0x02020462;  // 0000 0010 0000 0010 0000 0100 0110 0010  Bank 2
                                        //      CCCC      WWWW      SSSS EEEE LLLL
  #endif

  #if 0
  FMC_Bank5_6_R->SDTR[0] = 0x00209000;  // 0000 0000 0010 0000 1001 0000 0000 0000  Bank 1  // Versuch anhand ISSI-Datenblatt,
                                        //      CCCC PPPP WWWW RRRR SSSS EEEE LLLL          // mit CAS Latency = 3 (s.o.)
  FMC_Bank5_6_R->SDTR[1] = 0x020306B1;  // 0000 0010 0000 0011 0000 0110 1011 0001  Bank 2
                                        //      CCCC      WWWW      SSSS EEEE LLLL
  #endif
  FMC_Bank5_6_R->SDCR[0] = 0x00001800;
  FMC_Bank5_6_R->SDCR[1] = 0x00000165;
  FMC_Bank5_6_R->SDTR[0] = 0x00105000;
  FMC_Bank5_6_R->SDTR[1] = 0x01010351;
  
  /* SDRAM initialization sequence */

  /* Clock enable command */ 
  FMC_Bank5_6_R->SDCMR = 0x00000009; 
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }

  /* Delay */
  for (index=0; index<1000; index++);
  
  /* PALL command */ 
  FMC_Bank5_6_R->SDCMR = 0x0000000A; 	
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }
  
  FMC_Bank5_6_R->SDCMR = 0x000000EB;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }

  FMC_Bank5_6_R->SDCMR = 0x0004400C;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }

  /* Set refresh count */
  tmpreg = FMC_Bank5_6_R->SDRTR;
  FMC_Bank5_6_R->SDRTR = (tmpreg | (0x00000603 << 1));

  /* Disable write protection */
  tmpreg = FMC_Bank5_6_R->SDCR[1]; 
  FMC_Bank5_6_R->SDCR[1] = (tmpreg & 0xFFFFFDFF);

  /* Configure FMC Bank Mapping */
  FMC_Bank1_R->BTCR[0] |= FMC_BMAP_Value;

   /* FMC controller Enable */
  FMC_Bank1_R->BTCR[0] |= 0x80000000;
  

  /********** SDRAM only *************************************************************************/

  #elif defined (DATA_IN_ExtSDRAM)

  register uint32_t  tmpreg = 0, timeout = 0xFFFF;
  register __IO      uint32_t index;

  /*-- I/O Ports Configuration ------------------------------------------------------*/

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface clock */
  RCC->AHB4ENR |= 0x000001F8;
  
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x000000CC;
  GPIOD->AFR[1]  = 0xCC000CCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAFEAFFFA;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xF03F000F;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* Configure PDx pins in Pull-up */
  GPIOD->PUPDR   = 0x50150005;
   
  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00000CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAABFFA;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC00F;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* Configure PEx pins in Pull-up */
  GPIOE->PUPDR   = 0x55554005;
  
  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCCC000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAABFFAAA;
  /* Configure PFx pins speed to 100 MHz */ 
  GPIOF->OSPEEDR = 0xFFC00FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* Configure PFx pins in Pull-up */
  GPIOF->PUPDR   = 0x55400555;
  
  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0xC000000C;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0xBFFEFAAA;
 /* Configure PGx pins speed to 100 MHz */ 
  GPIOG->OSPEEDR = 0xC0030FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* Configure PGx pins in Pull-up */ 
  GPIOG->PUPDR   = 0x40010555;
  
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0xCCC00000;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */ 
  GPIOH->MODER   = 0xAAAAABFF;
  /* Configure PHx pins speed to 100 MHz */ 
  GPIOH->OSPEEDR = 0xFFFFFC00;
  /* Configure PHx pins Output type to push-pull */  
  GPIOH->OTYPER  = 0x00000000;
  /* Configure PHx pins in Pull-up */
  GPIOH->PUPDR   = 0x55555400;
  
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */ 
  GPIOI->MODER   = 0xFFEBAAAA;
  /* Configure PIx pins speed to 100 MHz */ 
  GPIOI->OSPEEDR = 0x003CFFFF;
  /* Configure PIx pins Output type to push-pull */  
  GPIOI->OTYPER  = 0x00000000;
  /* Configure PIx pins in Pull-up */
  GPIOI->PUPDR   = 0x00145555;
  
  /*-- FMC Configuration ------------------------------------------------------*/

  /* Enable the FMC interface clock */
  (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));

  /* SDRAM Timing and access interface configuration */

  /*LoadToActiveDelay    = 2
    ExitSelfRefreshDelay = 6
    SelfRefreshTime      = 4
    RowCycleDelay        = 6
    WriteRecoveryTime    = 2
    RPDelay              = 2
    RCDDelay             = 2
    SDBank               = FMC_SDRAM_BANK2
    ColumnBitsNumber     = FMC_SDRAM_COLUMN_BITS_NUM_9 
    RowBitsNumber        = FMC_SDRAM_ROW_BITS_NUM_12
    MemoryDataWidth      = FMC_SDRAM_MEM_BUS_WIDTH_32
    InternalBankNumber   = FMC_SDRAM_INTERN_BANKS_NUM_4
    CASLatency           = FMC_SDRAM_CAS_LATENCY_2
    WriteProtection      = FMC_SDRAM_WRITE_PROTECTION_DISABLE
    SDClockPeriod        = FMC_SDRAM_CLOCK_PERIOD_2
    ReadBurst            = FMC_SDRAM_RBURST_ENABLE
    ReadPipeDelay        = FMC_SDRAM_RPIPE_DELAY_0*/
  
  FMC_Bank5_6_R->SDCR[0] = 0x00001800;
  FMC_Bank5_6_R->SDCR[1] = 0x00000165;
  FMC_Bank5_6_R->SDTR[0] = 0x00105000;
  FMC_Bank5_6_R->SDTR[1] = 0x01010351;

  /* SDRAM initialization sequence */
  /* Clock enable command */ 
  FMC_Bank5_6_R->SDCMR = 0x00000009; 
  tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }

  /* Delay */
  for (index=0; index<1000; index++);
  
  /* PALL command */ 
    FMC_Bank5_6_R->SDCMR = 0x0000000A; 	
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }
  
  FMC_Bank5_6_R->SDCMR = 0x000000EB;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  }

  FMC_Bank5_6_R->SDCMR = 0x0004400C;
  timeout = 0xFFFF;
  while ((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020; 
  } 
  /* Set refresh count */
  tmpreg = FMC_Bank5_6_R->SDRTR;
  FMC_Bank5_6_R->SDRTR = (tmpreg | (0x00000603<<1));

  /* Disable write protection */
  tmpreg = FMC_Bank5_6_R->SDCR[1]; 
  FMC_Bank5_6_R->SDCR[1] = (tmpreg & 0xFFFFFDFF);

  /* FMC controller Enable */
  FMC_Bank1_R->BTCR[0]  |= 0x80000000;

  /********** SRAM only **************************************************************************/

  #elif defined(DATA_IN_ExtSRAM)

  /*-- I/O Ports Configuration -----------------------------------------------------*/

  /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHB4ENR |= 0x00000078;
  
  /* Connect PDx pins to FMC Alternate function */     
  GPIOD->AFR[0]  = 0x00CC00CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */  
  GPIOD->MODER   = 0xAAAAFABA;
  /* Configure PDx pins speed to 100 MHz */  
  GPIOD->OSPEEDR = 0xFFFF0F0F;
  /* Configure PDx pins Output type to push-pull */  
  GPIOD->OTYPER  = 0x00000000;
  /* Configure PDx pins in Pull-up */ 
  GPIOD->PUPDR   = 0x55550505;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */ 
  GPIOE->MODER   = 0xAAAABEBA;
  /* Configure PEx pins speed to 100 MHz */ 
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */  
  GPIOE->OTYPER  = 0x00000000;
  /* Configure PEx pins in Pull-up */
  GPIOE->PUPDR   = 0x55554145;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */   
  GPIOF->MODER   = 0xAAFFFAAA;
  /* Configure PFx pins speed to 100 MHz */ 
  GPIOF->OSPEEDR = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */  
  GPIOF->OTYPER  = 0x00000000;
  /* Configure PFx pins in Pull-up */ 
  GPIOF->PUPDR   = 0x55000555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0x00000C00;
  /* Configure PGx pins in Alternate function mode */ 
  GPIOG->MODER   = 0xFFEFFAAA;
  /* Configure PGx pins speed to 100 MHz */ 
  GPIOG->OSPEEDR = 0x00300FFF;
  /* Configure PGx pins Output type to push-pull */  
  GPIOG->OTYPER  = 0x00000000;
  /* Configure PGx pins in Pull-up */ 
  GPIOG->PUPDR   = 0x00100555;
  
  /*-- FMC/FSMC Configuration --------------------------------------------------*/

  /* Enable the FMC/FSMC interface clock */
  (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));

  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1_R->BTCR[4]  = 0x00001091;
  FMC_Bank1_R->BTCR[5]  = 0x00110212;
  FMC_Bank1E_R->BWTR[4] = 0x0FFFFFFF;  
  
  /* FMC controller Enable */
  FMC_Bank1_R->BTCR[0] |= 0x80000000;  

  #endif /* DATA_IN_ExtSRAM */
  
  (void)(tmp);

}
