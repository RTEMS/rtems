/*-------------------------------------*/
/* systbl.c                            */
/* Last change : 21. 3.95              */
/*-------------------------------------*/
#include "prcb.h"
/*#include "uart_access.h" */
#include "fault.h"
#include "faultret.h"
#include "memchnl.h"
#include "main.h"
/*#include "string_impl.h"*/
#include "stdio.h"
/* #include "stdio_impl.h" */
#include "systbl.h"
#include "time.h"
/*-------------------------------------*/

void sprintf_impl(void)
{
} 

void printf_A_impl(void)
{
} 

void printf_B_impl(void)
{
} 

void sscanf_impl(void)
{
} 

void console_set_channel_impl(void)
{
} 

void kbhit_impl(void)
{
} 

void putc_impl(void)
{
} 

void getc_impl(void)
{
} 

void puts_impl(void)
{
} 

void gets_impl(void)
{
} 

void printf_impl(void)
{
} 

void scanf_impl(void)
{
} 

void strlen_impl(void)
{
} 

void strcpy_impl(void)
{
} 

void strcat_impl(void)
{
} 

void strcmp_impl(void)
{
} 

void strtok_impl(void)
{
} 

void strncmp_impl(void)
{
} 

void strncpy_impl(void)
{
} 

void console_register_callback_impl(void)
{
} 

void memcpy_impl(void)
{
} 

void memcmp_impl(void)
{
} 

void memset_impl(void)
{
} 

void memcpy16_impl(void)
{
} 

void faultRet(unsigned int * faultState)
{
} 

void memChnlIotFinished(IOTrCBlk *)
{
}
  /* A fault has just occured. Pass a signal over
   * memory channel.
   */
void memChnlRegisterHndl(IotHandler)
{
}
  /* Intermediate image buffer.
   * Defined in *.ld
   */

char * mainRevisionStr(void)
{
}

char * mainBuildStr(void)
{
}

BufLookupHndl * memBufLookupInit(BufLookupHndl *, IOTrCBlk *)
{
}

void * memBufLookupNext(BufLookupHndl *)
{
}

void memBufLookupInit(void)
{
} 

void memBufLookupNext(void)
{
} 

void gettime_impl(void)
{
} 

void gettime_us_impl(void)
{
} 

  /* System Procedures Table. 
   */
SystemTbl systemTbl = {
 {0, 0, 0},				/* Reserved */
  svrStackPtr,				/* Supervisor Stack Pointer Base */
 {0, 0, 0, 0, 0, 0, 0, 0},		/* Preserved */
 {0, 0, 0, 0, 0, 0, 0, 0,		                /* 0 - 7 */
  0, 0, 0, 0, 0, 0, 0, 0,                               /* 8 - 15 */
  0, 0,                                                 /* 16 - 17 */
  SP(sprintf_impl + SUPERVISOR_SP),                     /* 18 */
  SP(printf_A_impl + SUPERVISOR_SP),                    /* 19 */
  SP(printf_B_impl + SUPERVISOR_SP),                    /* 20 */
  SP(sscanf_impl + SUPERVISOR_SP),                      /* 21 */
  0, 0,                                                 /* 22 - 23 */
  SP(console_set_channel_impl + SUPERVISOR_SP),         /* 24 */  
  SP(kbhit_impl + SUPERVISOR_SP),                       /* 25 */
  SP(putc_impl + SUPERVISOR_SP),                        /* 26 */
  SP(getc_impl + SUPERVISOR_SP),                        /* 27 */
  SP(puts_impl + SUPERVISOR_SP),                        /* 28 */
  SP(gets_impl + SUPERVISOR_SP),                        /* 29 */
  SP(printf_impl + SUPERVISOR_SP),                      /* 30 */
  SP(scanf_impl + SUPERVISOR_SP),                       /* 31 */
  0, 0, 0, 0, 0, 0, 0, 0,                               /* 32 - 39 */
  SP(strlen_impl + SUPERVISOR_SP),                      /* 40 */
  SP(strcpy_impl + SUPERVISOR_SP),                      /* 41 */  
  SP(strcat_impl + SUPERVISOR_SP),                      /* 42 */  
  SP(strcmp_impl + SUPERVISOR_SP),                      /* 43 */
  SP(strtok_impl + SUPERVISOR_SP),                      /* 44 */
  SP(strncmp_impl + SUPERVISOR_SP),                     /* 45 */
  SP(strncpy_impl + SUPERVISOR_SP),                     /* 46 */
  0,                                                    /* 47 */
  0, 0, 0, 0, 0, 0, 0, 0,                               /* 48 - 55 */
  SP(faultRegister + SUPERVISOR_SP),                    /* 56 */
  SP(faultOk + SUPERVISOR_SP),                          /* 57 */
  SP(faultRet + SUPERVISOR_SP),                         /* 58 */
  0, 0, 0, 0, 0,                                        /* 59 - 63 */
  SP(memChnlIotFinished + SUPERVISOR_SP),               /* 64 */
  SP(memChnlRegisterHndl + SUPERVISOR_SP),              /* 65 */ 
  0, 0, 0, 0, 0, 0,                                     /* 66 - 71 */ 
  0,							/* 72 */ 
  0,							/* 73 */
  0, 							/* 74 */
  0,							/* 75 */
  0,							/* 76 */
  0,							/* 77 */
  0, 0,                                                 /* 78 - 79 */
  0,							/* 80 */
  0,							/* 81 */
  0, 0, 0, 0, 0, 0,                                     /* 82 - 87 */
  SP(console_register_callback_impl + SUPERVISOR_SP),   /* 88 */ 
  0, 0, 0, 0, 0, 0, 0,                                  /* 89 - 95 */
  SP(mainRevisionStr + SUPERVISOR_SP),                  /* 96 */ 
  SP(mainBuildStr + SUPERVISOR_SP),                     /* 97 */ 
  0, 0, 0, 0, 0, 0,                                     /* 98 - 103 */
  SP(memBufLookupInit + SUPERVISOR_SP),                 /* 104 */
  SP(memBufLookupNext + SUPERVISOR_SP),                 /* 105 */
  SP(memcpy_impl + SUPERVISOR_SP),                      /* 106 */
  SP(memcmp_impl + SUPERVISOR_SP),                      /* 107 */
  SP(memset_impl + SUPERVISOR_SP),                      /* 108 */
  SP(memcpy16_impl + SUPERVISOR_SP),                    /* 109 */
  0, 0,                                                 /* 110 - 111 */
  SP(gettime_impl + SUPERVISOR_SP),                     /* 112 */
  SP(gettime_us_impl + SUPERVISOR_SP),                  /* 113 */
  0}							/* 114 */
};
/*-------------*/
/* End of file */
/*-------------*/

