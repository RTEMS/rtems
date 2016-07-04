#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"
/*-----------------------------------------------------------------------------------------------*/
/*
 * PWMSS Registers
 *
 * @Source : AM335x Technical Reference Manual ,page 1991
 *           Table 15-5. PWMSS REGISTERS
 *
*/

#define PWMSS0_MMAP_ADDR   0x48300000
#define PWMSS1_MMAP_ADDR   0x48302000
#define PWMSS2_MMAP_ADDR   0x48304000
#define PWMSS_MMAP_LEN     0x1000

#define PWMSS_IDVER    0x0
#define PWMSS_SYSCONFIG    0x4
#define PWMSS_CLKCONFIG    0x8
#define PWMSS_CLKSTATUS    0xC

/* EPWM Registers
 *
 * @Source : AM335x Technical Reference Manual ,page 2084
 *           Table 15-58. EPWM REGISTERS
 *
*/
#define EPWM_TBCTL 0x0
#define EPWM_TBSTS 0x2
#define EPWM_TBPHSHR   0x4
#define EPWM_TBPHS 0x6
#define EPWM_TBCNT 0x8
#define EPWM_TBPRD 0xA
#define EPWM_CMPCTL    0xE
#define EPWM_CMPAHR    0x10
#define EPWM_CMPA  0x12
#define EPWM_CMPB  0x14
#define EPWM_AQCTLA    0x16
#define EPWM_AQCTLB    0x18
#define EPWM_AQSFRC    0x1A
#define EPWM_AQCSFRC   0x1C
#define EPWM_DBCTL 0x1E
#define EPWM_DBRED 0x20
#define EPWM_DBFED 0x22
/*-----------------------------------------------------------------------------------------------*/
extern int memh;
extern volatile unsigned int *CM_ptr;  /*c ontrol module */
volatile unsigned int *cm_per_addr;


const unsigned int PWMSS_AddressOffset[]={PWMSS0_MMAP_ADDR,
                     PWMSS1_MMAP_ADDR,
                     PWMSS2_MMAP_ADDR};
volatile unsigned int *pwmss_ptr[3]     ={NULL, NULL, NULL} ;
volatile unsigned int *epwm_ptr[3]      ={NULL, NULL, NULL} ;
volatile unsigned int *ecap_ptr[3]      ={NULL, NULL, NULL} ;
volatile unsigned int *eqep_ptr[3]      ={NULL, NULL, NULL} ;

#define TBCTL_CTRMODE_UP        0x0
#define TBCTL_CTRMODE_DOWN      0x1
#define TBCTL_CTRMODE_UPDOWN    0x2
#define TBCTL_CTRMODE_FREEZE    0x3
/* ----------------------------------------------------------------------------------------------- */
/* PWMSS Timebase clock check
 *     check the timenase clock enable or not
 *
 * @param PWMSS_ID :  PWM sumsystem ID (BBBIO_PWMSS0 ,BBBIO_PWMSS1, BBBIO_PWMSS2)
 *
 * @return : 0 for disable timebase clock , 1 for enable for timebase clock
 */
static int PWMSS_TB_clock_check(unsigned int PWMSS_ID)
{
   volatile unsigned int* reg;
   unsigned int reg_value ;

   /* Control module check */
   reg =(void *)CM_ptr + BBBIO_PWMSS_CTRL;
   reg_value = *reg ;

   return (reg_value & (1 << PWMSS_ID)) ;
}

/* ----------------------------------------------------------------------------------------------- */
/* PWM subsystem system control
 *     enable or disable module clock
 *
 * @param PWMSS_ID :  PWM sumsystem ID (BBBIO_PWMSS0 ,BBBIO_PWMSS1, BBBIO_PWMSS2).
 * @param enable : 0 for disable , else for enable .
 *
 * @return : 1 for success ,  0 for error
 */
static int PWMSS_module_ctrl(unsigned int PWMSS_ID, int enable)
{
   volatile unsigned int *reg = NULL;
   unsigned int module_set[] = {BBBIO_PWMSS0, BBBIO_PWMSS1, BBBIO_PWMSS2};
   unsigned int module_clk_set[] = {BBBIO_CM_PER_EPWMSS0_CLKCTRL, BBBIO_CM_PER_EPWMSS1_CLKCTRL, BBBIO_CM_PER_EPWMSS2_CLKCTRL};
   int ret = 1;

   reg = (void*)cm_per_addr + module_clk_set[PWMSS_ID];
   if(enable) {
       if(PWMSS_TB_clock_check(module_set[PWMSS_ID])) {
           /* Enable module clock */
           *reg = 0x2; /* Module enable and fully functional */
           return ret;
       }
#ifdef BBBIO_LIB_DBG
       else {
           printf("PWMSS_module_ctrl : PWMSS-%d timebase clock disable in Control Module\n", PWMSS_ID);
       }
#endif
       ret = 0 ;
   }
   *reg = 0x3 << 16;   /* Module is disabled and cannot be accessed */
   return ret;
}

/* ----------------------------------------------------------------------------------------------- */
/* PWM init
 * iolib_init will run this function automatically
 *
 *      @return         : 1 for success , 0 for failed
 */

int BBBIO_PWM_Init()
{
   int i = 0;

   if (memh == 0) {
#ifdef BBBIO_LIB_DBG
       printf("BBBIO_PWM_Init: memory not mapped?\n");
#endif
       return 0;
       }

   /* Create Memory map */
   for (i = 0 ; i < 3 ; i ++) {
       pwmss_ptr[i] = mmap(0, PWMSS_MMAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, PWMSS_AddressOffset[i]);
       if(pwmss_ptr[i] == MAP_FAILED) {
#ifdef BBBIO_LIB_DBG
           printf("BBBIO_PWM_Init: PWMSS %d mmap failure!\n", i);
#endif
           goto INIT_ERROR ;
       }
       ecap_ptr[i] = (void *)pwmss_ptr[i] + 0x100 ;
       eqep_ptr[i] = (void *)pwmss_ptr[i] + 0x180 ;
       epwm_ptr[i] = (void *)pwmss_ptr[i] + 0x200 ;

       if(!PWMSS_module_ctrl(i, 1)) {
#ifdef BBBIO_LIB_DBG
           printf("BBBIO_PWM_Init: PWMSS %d clock  failure!\n", i);
#endif
           goto INIT_ERROR ;
       }
       }
   return 1;

INIT_ERROR :
   BBBIO_PWM_Release();
   return 0;
}

/* ----------------------------------------------------------------------------------------------- */
void BBBIO_PWM_Release()
{
   int i = 0;
   for(i = 0 ; i < 3 ; i ++) {
       if(pwmss_ptr[i] != NULL) {
           munmap((void *)pwmss_ptr[i], PWMSS_MMAP_LEN);
           pwmss_ptr[i] = NULL;
           ecap_ptr[i] = NULL;
           eqep_ptr[i] = NULL;
           epwm_ptr[i] = NULL;
       }
   }
}

/* ----------------------------------------------------------------------------------------------- */
/* PWMSS status (no effect now)
 *     set pluse rgument of epwm module
 *
 *      @param PWMID    : EPWMSS number , 0~3
 *
 *      @return         : 1 for success , 0 for failed
 */
int BBBIO_PWMSS_Status(unsigned int PWMID)
{
   int param_error = 1;
   volatile unsigned int* reg;
   unsigned int reg_value ;

   if (memh == 0)
            param_error = 0;

       if (PWMID > 2)      /* if input is not EPWMSS 0~ WPEMSS 2 */
            param_error = 0;

       if (param_error == 0) {
#ifdef BBBIO_LIB_DBG
       printf("BBBIO_PWM_Status: parameter error!\n");
#endif
       return 0;
   }

   reg =(void *)CM_ptr + BBBIO_PWMSS_CTRL;

   reg_value = *reg >> PWMID & 0x01 ;
   if(reg_value == 0) {
       printf("PWMSS [%d] Timebase clock Disable , Control Module [pwmss_ctrl register]\n", PWMID);
   }
   else {
       reg=(void *)pwmss_ptr[PWMID] + PWMSS_CLKSTATUS;
       reg_value = *reg ;

       printf("PWMSS [%d] :\tCLKSTOP_ACK %d , CLK_EN_ACK %d , CLKSTOP_ACK %d , CLK_EN_ACK %d , CLKSTOP_ACK %d , CLK_EN_ACK %d\n",
           PWMID ,
           reg_value >>9 & 0x1 ,
           reg_value >>8 & 0x1 ,
           reg_value >>5 & 0x1 ,
           reg_value >>4 & 0x1 ,
           reg_value >>1 & 0x1 ,
           reg_value >>0 & 0x1 );
   }
   return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
/* PWMSS setting
 *     set pluse rgument of epwm module
 *
 *      @param PWMID    : EPWMSS number , 0~2
 *      @param HZ      : pluse HZ
 *      @param dutyA    : Duty Cycle in ePWM A
 *      @param dutyB    : Duty Cycle in ePWM B
 *
 *      @return         : 1 for success , 0 for failed
 *
 *      @example        :  BBBIO_PWMSS_Setting(0 , 50.0f , 50.0f , 25.0f);     // Generate 50HZ pwm in PWM0 ,
 *                                     // duty cycle is 50% for ePWM0A , 25% for ePWM0B
 *
 *     @Note :
 *         find an number nearst 65535 for TBPRD , to improve duty precision,
 *
 *     Using big TBPRD can increase the range of CMPA and CMPB ,
 *         and it means we can get better precision on duty cycle.
 *
 *     EX : 20.25% duty cycle
 *                  on TBPRD = 62500 , CMPA = 12656.25 ( .25 rejection) , real duty : 20.2496% (12656 /62500)
 *                  on TBPRD = 6250  , CMPA = 1265.625 ( .625 rejection), real duty : 20.24%   (1265 6250)
 *                  on TBPRD = 500   , CMPA = 101.25   ( .25 rejection) , real duty : 20.2%    (101/500)
 *
 *     Divisor = CLKDIV * HSPCLKDIV
 *                 1 TBPRD : 10 ns (default)
 *         65535 TBPRD : 655350 ns
 *         65535 TBPRD : 655350 * Divisor ns  = X TBPRD : Cyclens
 *
 *         accrooding to that , we must find a Divisor value , let X nearest 65535 .
 *         so , Divisor must  Nearest Cyclens/655350
*/

int BBBIO_PWMSS_Setting(unsigned int PWMID , float HZ ,float dutyA ,float dutyB)
{
   int param_error = 1;
   volatile unsigned short* reg16 ;
        if (memh == 0)
            param_error = 0;
        if (PWMID > 2)              // if input is not EPWMSS 0~ WPEMSS 2
            param_error = 0;
   if (HZ < 0 )
       param_error = 0;
   if(dutyA < 0.0f || dutyA > 100.0f || dutyB < 0.0f || dutyB > 100.0f)
       param_error = 0;

        if (param_error == 0) {
#ifdef BBBIO_LIB_DBG
       printf("BBBIO_PWMSS_Setting: parameter error!\n");
#endif
       return 0;
        }

   dutyA /= 100.0f ;
   dutyB /= 100.0f ;

   /* compute neccessary TBPRD */
   float Cyclens =0.0f ;
   float Divisor =0;
   int i , j ;
   const float CLKDIV_div[] = {1.0 ,2.0 ,4.0 ,8.0 ,16.0 ,32.0 , 64.0 , 128.0};
   const float HSPCLKDIV_div[] ={1.0 ,2.0 ,4.0 ,6.0 ,8.0 ,10.0 , 12.0 , 14.0};
   int NearCLKDIV =7;
   int NearHSPCLKDIV =7;
   int NearTBPRD =0;

   Cyclens = 1000000000.0f / HZ ; /* 10^9 / HZ , comput time per cycle (ns) */


   Divisor =  (Cyclens / 655350.0f) ;  /* am335x provide (128*14) divider , and per TBPRD means 10 ns when divider /1 ,
                        * and max TBPRD is 65535 , so , the max cycle is 128*14* 65535 *10ns
                        */
#ifdef BBBIO_LIB_DBG
   printf("Cyclens %f , Divisor %f\n", Cyclens, Divisor);
#endif

   if(Divisor > (128 * 14)) {
#ifdef BBBIO_LIB_DBG
       printf("BBBIO_PWMSS_Setting : Can't generate %f HZ \n", HZ);
#endif
       return 0;
   }
   else {
       /* using Exhaustive Attack metho */
       for(i = 0 ; i < 8 ; i ++) {
           for(j = 0 ; j < 8 ; j ++) {
               if((CLKDIV_div[i] * HSPCLKDIV_div[j]) < (CLKDIV_div[NearCLKDIV] * HSPCLKDIV_div[NearHSPCLKDIV]) &&
                 ((CLKDIV_div[i] * HSPCLKDIV_div[j]) > Divisor)) {
                   NearCLKDIV = i ;
                   NearHSPCLKDIV = j ;
               }
           }
       }
#ifdef BBBIO_LIB_DBG
       printf("nearest CLKDIV %f , HSPCLKDIV %f\n" ,CLKDIV_div[NearCLKDIV] ,HSPCLKDIV_div[NearHSPCLKDIV]);
#endif
       NearTBPRD = (Cyclens / (10.0 *CLKDIV_div[NearCLKDIV] *HSPCLKDIV_div[NearHSPCLKDIV])) ;

#ifdef BBBIO_LIB_DBG
       printf("nearest TBPRD %d, %f %f\n ",NearTBPRD,NearTBPRD * dutyA, NearTBPRD * dutyB);
#endif

       /* setting clock diver and freeze time base */
       reg16=(void*)epwm_ptr[PWMID] +EPWM_TBCTL;
       *reg16 = TBCTL_CTRMODE_FREEZE | (NearCLKDIV << 10) | (NearHSPCLKDIV << 7);

       /*  setting duty A and duty B */
       reg16=(void*)epwm_ptr[PWMID] +EPWM_CMPB;
       *reg16 =(unsigned short)((float)NearTBPRD * dutyB);

       reg16=(void*)epwm_ptr[PWMID] +EPWM_CMPA;
       *reg16 =(unsigned short)((float)NearTBPRD * dutyA);

       reg16=(void*)epwm_ptr[PWMID] +EPWM_TBPRD;
       *reg16 =(unsigned short)NearTBPRD;

       /* reset time base counter */
       reg16 = (void *)epwm_ptr[PWMID] + EPWM_TBCNT;
       *reg16 = 0;
   }
   return 1;
}
/* ----------------------------------------------------------------------------------------------- */
/* Enable/Disable ehrPWM module
 *      @param PWMID    : PWMSS number , 0~2
 *
 *      @return         : void
 *
 *      @example        : BBBIO_PWMSS_Enable(0) ;// Enable PWMSS 0
 */

void BBBIO_ehrPWM_Enable(unsigned int PWMSS_ID)
{
   volatile unsigned short *reg16 ;

   reg16=(void*)epwm_ptr[PWMSS_ID] +EPWM_AQCTLA;
   *reg16 = 0x2 | ( 0x3 << 4) ;
       
   reg16=(void*)epwm_ptr[PWMSS_ID] +EPWM_AQCTLB;
   *reg16 = 0x2 | ( 0x3 << 8) ;

   reg16 = (void *)epwm_ptr[PWMSS_ID] + EPWM_TBCNT;
   *reg16 = 0;

        reg16=(void *)epwm_ptr[PWMSS_ID] + EPWM_TBCTL;
   *reg16 &= ~0x3;
}

void BBBIO_ehrPWM_Disable(unsigned int PWMSS_ID)
{
   volatile unsigned short *reg16 ;
        reg16=(void *)epwm_ptr[PWMSS_ID] + EPWM_TBCTL;
        *reg16 |= 0x3;

   reg16=(void*)epwm_ptr[PWMSS_ID] +EPWM_AQCTLA;
   *reg16 = 0x1 | ( 0x3 << 4) ;
       
   reg16=(void*)epwm_ptr[PWMSS_ID] +EPWM_AQCTLB;
   *reg16 = 0x1 | ( 0x3 << 8) ;

   reg16 = (void *)epwm_ptr[PWMSS_ID] + EPWM_TBCNT;
   *reg16 = 0;
}
//--------------------------------------------------------












