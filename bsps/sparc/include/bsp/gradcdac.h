/*  ADC / DAC (GRADCDAC) interface
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRADCDAC_H__
#define __GRADCDAC_H__

#ifdef __cplusplus
extern "C" {
#endif

struct gradcdac_regs {
	volatile unsigned int config;      /* 0x00 Configuration Register */
	volatile unsigned int status;      /* 0x04 Status Register */
	int unused0[2];
	volatile unsigned int adc_din;     /* 0x10 ADC Data Input Register */
	volatile unsigned int dac_dout;    /* 0x14 DAC Data Output Register */
	int unused1[2];
	volatile unsigned int adrin;       /* 0x20 Address Input Register */
	volatile unsigned int adrout;      /* 0x24 Address Output Register */
	volatile unsigned int adrdir;      /* 0x28 Address Direction Register */
	int unused2[1];
	volatile unsigned int data_in;     /* 0x30 Data Input Register  */
	volatile unsigned int data_out;    /* 0x34 Data Output Register */
	volatile unsigned int data_dir;    /* 0x38 Data Direction Register */
};

#define GRADCDAC_CFG_DACWS    0x00f80000
#define GRADCDAC_CFG_WRPOL    0x00040000
#define GRADCDAC_CFG_DACDW    0x00030000
#define GRADCDAC_CFG_ADCWS    0x0000f800
#define GRADCDAC_CFG_RCPOL    0x00000400
#define GRADCDAC_CFG_CSMODE   0x00000300
#define GRADCDAC_CFG_CSPOL    0x00000080
#define GRADCDAC_CFG_RDYMODE  0x00000040
#define GRADCDAC_CFG_RDYPOL   0x00000020
#define GRADCDAC_CFG_TRIGPOL  0x00000010
#define GRADCDAC_CFG_TRIGMODE 0x0000000c
#define GRADCDAC_CFG_ADCDW    0x00000003

#define GRADCDAC_CFG_DACWS_BIT    19
#define GRADCDAC_CFG_WRPOL_BIT    18
#define GRADCDAC_CFG_DACDW_BIT    16
#define GRADCDAC_CFG_ADCWS_BIT    11
#define GRADCDAC_CFG_RCPOL_BIT    10
#define GRADCDAC_CFG_CSMODE_BIT    8
#define GRADCDAC_CFG_CSPOL_BIT     7
#define GRADCDAC_CFG_RDYMODE_BIT   6
#define GRADCDAC_CFG_RDYPOL_BIT    5
#define GRADCDAC_CFG_TRIGPOL_BIT   4
#define GRADCDAC_CFG_TRIGMODE_BIT  2
#define GRADCDAC_CFG_ADCDW_BIT     0

#define GRADCDAC_STATUS_DACNO   0x40
#define GRADCDAC_STATUS_DACRDY  0x20
#define GRADCDAC_STATUS_DACON   0x10
#define GRADCDAC_STATUS_ADCTO   0x08
#define GRADCDAC_STATUS_ADCNO   0x04
#define GRADCDAC_STATUS_ADCRDY  0x02
#define GRADCDAC_STATUS_ADCON   0x01

#define GRADCDAC_STATUS_DACNO_BIT  6
#define GRADCDAC_STATUS_DACRDY_BIT 5
#define GRADCDAC_STATUS_DACON_BIT  4
#define GRADCDAC_STATUS_ADCTO_BIT  3
#define GRADCDAC_STATUS_ADCNO_BIT  2
#define GRADCDAC_STATUS_ADCRDY_BIT 1
#define GRADCDAC_STATUS_ADCON_BIT  0

#define GRADCDAC_IRQ_DAC 1
#define GRADCDAC_IRQ_ADC 0

struct gradcdac_config {
	unsigned char dac_ws;
	char wr_pol;
	unsigned char dac_dw;
	unsigned char adc_ws;
	char rc_pol;
	unsigned char cs_mode;
	char cs_pol;
	char ready_mode;
	char ready_pol;
	char trigg_pol;
	unsigned char trigg_mode;
	unsigned char adc_dw;
};

extern void *gradcdac_open(char *devname);

extern void gradcdac_set_config(void *cookie, struct gradcdac_config *cfg);

extern void gradcdac_get_config(void *cookie, struct gradcdac_config *cfg);

extern void gradcdac_set_cfg(void *cookie, unsigned int config);

extern unsigned int gradcdac_get_cfg(void *cookie);

extern unsigned int gradcdac_get_status(void *cookie);

static int __inline__ gradcdac_DAC_ReqRej(unsigned int status)
{
	return (status & GRADCDAC_STATUS_DACNO);
}

static int __inline__ gradcdac_DAC_isCompleted(unsigned int status)
{
	return (status & GRADCDAC_STATUS_DACRDY);
}

static int __inline__ gradcdac_DAC_isOngoing(unsigned int status)
{
	return (status & GRADCDAC_STATUS_DACON);
}

static int __inline__ gradcdac_ADC_isTimeouted(unsigned int status)
{
	return (status & GRADCDAC_STATUS_ADCTO);
}

static int __inline__ gradcdac_ADC_ReqRej(unsigned int status)
{
	return (status & GRADCDAC_STATUS_ADCNO);
}

static int __inline__ gradcdac_ADC_isCompleted(unsigned int status)
{
	return (status & GRADCDAC_STATUS_ADCRDY);
}

static int __inline__ gradcdac_ADC_isOngoing(unsigned int status)
{
	return (status & GRADCDAC_STATUS_ADCON);
}

#define GRADCDAC_ISR_BOTH 3
#define GRADCDAC_ISR_DAC 2
#define GRADCDAC_ISR_ADC 1

/* Install IRQ handler for ADC and/or DAC interrupt.
 * The installed IRQ handler(ISR) must read the status 
 * register to clear the pending interrupt avoiding multiple
 * entries to the ISR caused by the same IRQ.
 *
 * \param adc  1=ADC interrupt, 2=ADC interrupt, 3=ADC and DAC interrupt
 * \param isr  Interrupt service routine called when IRQ is fired
 * \param arg  custom argument passed to ISR when called.
 */
extern int gradcdac_install_irq_handler
	(void *cookie, int adc, void (*isr)(void *cookie, void *arg), void *arg);

extern void gradcdac_uninstall_irq_handler(void *cookie, int adc);

/* Make the ADC circuitry initialize a analogue to digital
 * conversion. The result can be read out by gradcdac_adc_convert_try
 * or gradcdac_adc_convert.
 */
extern void gradcdac_adc_convert_start(void *cookie);

/* Tries to read the conversion result. If the circuitry is busy 
 * converting the function return a non-zero value, if the conversion 
 * has successfully finished the function return zero.
 *
 * \param digital_value the resulting converted value is placed here
 * \return zero     = ADC conversion complete, digital_value contain current conversion result
 *         positive = ADC busy, digital_value contain previous conversion result
 *         negative = Conversion request failed.
 */
extern int gradcdac_adc_convert_try(void *cookie, unsigned short *digital_value);

/* Waits until the ADC circuity has finished a digital to analogue 
 * conversion. The Waiting is implemented as a busy loop utilizing
 * 100% CPU load.
 *
 * \return zero     = Conversion ok
 *         negative = Conversion request failed.
 */
extern int gradcdac_adc_convert(void *cookie, unsigned short *digital_value);

/* Try to make the DAC circuitry initialize a digital to analogue 
 * conversion. If the circuitry is busy by a previous conversion
 * the function return a non-zero value, if the conversion is
 * successfully initialized the function return zero.
 */
extern int gradcdac_dac_convert_try(void *cookie, unsigned short digital_value);

/* Initializes a digital to analogue conversion by waiting until 
 * previous conversions is finished before procceding with the
 * conversion. The Waiting is implemented as a busy loop utilizing
 * 100% CPU load.
 */
extern void gradcdac_dac_convert(void *cookie, unsigned short digital_value);

extern unsigned int gradcdac_get_adrinput(void *cookie);
extern void gradcdac_set_adrinput(void *cookie, unsigned int input);

extern unsigned int gradcdac_get_adroutput(void *cookie);
extern void gradcdac_set_adroutput(void *cookie, unsigned int output);

extern unsigned int gradcdac_get_adrdir(void *cookie);
extern void gradcdac_set_adrdir(void *cookie, unsigned int dir);

extern unsigned int gradcdac_get_datainput(void *cookie);
extern void gradcdac_set_datainput(void *cookie, unsigned int input);

extern unsigned int gradcdac_get_dataoutput(void *cookie);
extern void gradcdac_set_dataoutput(void *cookie, unsigned int output);

extern unsigned int gradcdac_get_datadir(void *cookie);
extern void gradcdac_set_datadir(void *cookie, unsigned int dir);

/* Show one or all GRADCDAC cores. If cookie is NULL all GRADCDAC's are shown */
extern void grAdcDacShow(void *cookie);

/* Register Driver routine */
extern void gradcdac_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif
