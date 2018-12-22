/*
 *  GRPWM PWM Driver interface.
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRPWM_H__
#define __GRPWM_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void grpwm_register_drv (void);

#define GRPWM_IOCTL_GET_CAP	1	/* Get Capabilities */
#define GRPWM_IOCTL_SET_CONFIG	2	/* Configure one PWM Channel */
#define GRPWM_IOCTL_SET_SCALER	3	/* Set one scaler */
#define GRPWM_IOCTL_UPDATE	4	/* Set current period and compare value */
#define GRPWM_IOCTL_IRQ		5	/* Set up IRQ handling */

/*** Argument for GRPWM_IOCTL_GET_CAP ***/

/* The Capability of the PWM core */
struct grpwm_ioctl_cap {
	int		channel_cnt;	/* Number of channels */
	unsigned int	pwm;		/* Capability1 register */
	unsigned int	wave;		/* Capability2 register, Wave form capabilities of last PWM channel, otherwise 0 */
};

/*** Argument for GRPWM_IOCTL_GET_CONFIG and GRPWM_IOCTL_SET_CONFIG ***/

/* Config One PWM */
struct grpwm_ioctl_config {
	unsigned int	channel;	/* Select channel to configure */

	/* Specific for one PWM channel */
	unsigned int	options;	/* PWM options */
	unsigned char	dbscaler;	/* value greater than 15 disable Dead band */
	unsigned char	scaler_index;	/* Select scaler used by PWM channel */

	/* IRQ Setup */
	unsigned char	irqscaler;	/* IRQ scaler */
	void		*isr_arg;	/* Argument of IRQ handler */
	void		(*isr)(int channel, void *arg);	/* Interrupt service routine for this PWM Channel */

	/* Waveform set up */
	int		wave_activate;		/* Enables Waveform functionality */
	unsigned int	wave_synccfg;		/* Bits [29,30,31] is written into Wave-Config register */
	unsigned int	wave_sync;		/* Sets sync compare register */
	unsigned int	*wave_data;		/* If not NULL, the Wave RAM is filled with data */
	unsigned int	wave_data_length;	/* Length of Wave RAM Data, Also used for wstopaddr */
};

#define GRPWM_CONFIG_OPTION_FLIP		0x04000000	/* Set this to Flip PWM output pair */
#define GRPWM_CONFIG_OPTION_DEAD_BAND		0x00200000	/* Dead Band enable */
#define GRPWM_CONFIG_OPTION_SYMMETRIC		0x00000040	/* If not defined, asymmetric */
#define GRPWM_CONFIG_OPTION_ASYMMERTIC		0
#define GRPWM_CONFIG_OPTION_DUAL		0x00000020	/* Dual Compare Enable */
#define GRPWM_CONFIG_OPTION_PAIR		0x00000004	/* PWM Pair Enable */
#define GRPWM_CONFIG_OPTION_SINGLE		0x00000000	/* PWM Pair Disable */
#define GRPWM_CONFIG_OPTION_POLARITY_HIGH	0x00000002	/* PWM Polarity HIGH */
#define GRPWM_CONFIG_OPTION_POLARITY_LOW	0x00000000	/* PWM Polarity LOW */

#define GRPWM_CONFIG_OPTION_MASK ( \
	GRPWM_CONFIG_OPTION_DEAD_BAND | GRPWM_CONFIG_OPTION_SYMMETRIC | \
	GRPWM_CONFIG_OPTION_DUAL | GRPWM_CONFIG_OPTION_PAIR | \
	GRPWM_CONFIG_OPTION_POLARITY_HIGH \
	)

/*** Argument for GPPWM_IOCTL_SET_SCALER ***/

struct grpwm_ioctl_scaler {
	unsigned int index_mask;/* Scaler update index mask, bit 0 = Scaler 0, bit 1 = Scaler 1 */
	unsigned int values[8];	/* Scaler update values, values[N] is stored into scaler N, if mask & 1<<N is set */
};

/*** Argument for GRPWM_IOCTL_UPDATE ***/

#define GRPWM_UPDATE_OPTION_ENABLE	0x01	/* Enable the PWM core */
#define GRPWM_UPDATE_OPTION_DISABLE	0x02	/* Disable the PWM core */
#define GRPWM_UPDATE_OPTION_PERIOD	0x04	/* Update period register */
#define GRPWM_UPDATE_OPTION_COMP	0x08	/* Update Compare register */
#define GRPWM_UPDATE_OPTION_DBCOMP	0x10	/* Update Dead band register */
#define GRPWM_UPDATE_OPTION_FIX		0x20	/* Update fix output pins (bypass PWM) */

/* FIX PIN bit-mask */
#define GRPWM_UPDATE_FIX_ENABLE		1	/* Enable force ouput */
#define GRPWM_UPDATE_FIX_DISABLE	0	/* Disable force ouput */
#define GRPWM_UPDATE_FIX_0_LOW		0	/* PIN 0 OUPUT: LOW */
#define GRPWM_UPDATE_FIX_0_HIGH		2	/* PIN 0 OUPUT: HIGH */
#define GRPWM_UPDATE_FIX_1_LOW		0	/* PIN 1 OUPUT: LOW */
#define GRPWM_UPDATE_FIX_1_HIGH		4	/* PIN 1 OUPUT: HIGH */

struct grpwm_ioctl_update_chan {
	unsigned int	options;	/* Select what is updated */
	unsigned int	period;		/* Period register content */
	unsigned int	compare;	/* Compare register content */
	unsigned int	dbcomp;		/* Dead band register content */
	unsigned char	fix;		/* Bit-mask that select output on one or two PWM
					 * output pins. Depends on PAIR config value.
					 */
};
struct grpwm_ioctl_update {
	unsigned char			chanmask; /* Bit Mask select channels */
	struct grpwm_ioctl_update_chan	channels[8]; /*  */
};

/*** Argument for GPPWM_IOCTL_IRQ ***/

#define GRPWM_IRQ_DISABLE	0	/* Disable IRQ */
#define GRPWM_IRQ_PERIOD	1	/* Enable IRQ on period match */
#define GRPWM_IRQ_COMPARE	3	/* Enable IRQ on Compare Match */
#define GRPWM_IRQ_CLEAR		0x10	/* Clear any pending IRQ on GRPWM and IRQ controller */

#define GRPWM_IRQ_CHAN		0x100	/* Channel N is selected, by adding 0x100*N */

#ifdef __cplusplus
}
#endif

#endif
