/************************************************
 * NAME     : s3c2400.h
 * Version  : 4.18.2008
 *
 * share code for different Samsung CPU
 ************************************************/

#ifndef S3C24XX_H_
#define S3C24XX_H_

#include <bspopts.h>

#ifdef CPU_S3C2400
#include<s3c2400.h>
#elif defined CPU_S3C2410
#include<s3c2410.h>
#else
#error "Undefined Samsung CPU model"
#endif

#endif /*S3C24XX_H_*/
