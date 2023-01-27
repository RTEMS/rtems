/* SPDX-License-Identifier: BSD-2-Clause */

/*  GPIO Library interface
 *
 *  COPYRIGHT (c) 2009.
 *  Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GPIOLIB_H__
#define __GPIOLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO Config of one GPIO port */
struct gpiolib_config {
	char		mask;		/* 0=Masked/1=Unmasked IRQ */
	char		irq_level;	/* Edge or Level triggered IRQ */
	char		irq_polarity;	/* Polarity of IRQ */
};

#define GPIOLIB_IRQ_EDGE 0
#define GPIOLIB_IRQ_LEVEL 1

#define GPIOLIB_IRQ_POL_LOW 0
#define GPIOLIB_IRQ_POL_HIGH 1

/* Libarary initialize function must be called befor any other */
extern int gpiolib_initialize(void);

/*** User Interface ***/

extern void *gpiolib_open(int port);
extern void *gpiolib_open_by_name(char *devName);
extern void gpiolib_close(void *handle);

/* Show the current status one or all GPIO ports in the system. 
 * Int port is port nunber, if port = -1 selects all ports.
 * 
 * If port != -1, handle is used to get port.
 * If port != -1, handle == NULL, then port is used as port number
 */
extern void gpiolib_show(int port, void *handle);

extern int gpiolib_set_config(void *handle, struct gpiolib_config *cfg);
extern int gpiolib_set(void *handle, int dir, int val);
extern int gpiolib_get(void *handle, int *inval);
extern int gpiolib_irq_clear(void *handle);
extern int gpiolib_irq_enable(void *handle);
extern int gpiolib_irq_disable(void *handle);
extern int gpiolib_irq_mask(void *handle);
extern int gpiolib_irq_unmask(void *handle);
extern int gpiolib_irq_force(void *handle);
extern int gpiolib_irq_register(void *handle, void *func, void *arg);

/*** Driver Interface ***/

struct gpiolib_info {
	char		devName[80];
};

struct gpiolib_drv_ops {
	int		(*config)(void *handle, struct gpiolib_config *cfg);
	int		(*get)(void *handle, int *val);
	int		(*irq_opts)(void *handle, unsigned int options);
	int		(*irq_register)(void *handle, void *func, void *arg);
	int		(*open)(void *handle);
	int		(*set)(void *handle, int dir, int outval);
	int		(*show)(void *handle);
	int		(*get_info)(void *handle, struct gpiolib_info *pinfo);
};

#define GPIOLIB_IRQ_ENABLE  0x01
#define GPIOLIB_IRQ_DISABLE 0x02
#define GPIOLIB_IRQ_CLEAR   0x04
#define GPIOLIB_IRQ_FORCE   0x08
#define GPIOLIB_IRQ_MASK    0x10
#define GPIOLIB_IRQ_UNMASK  0x20

struct gpiolib_drv {
	struct gpiolib_drv_ops	*ops;
};

/* Register a GPIO port */
extern int gpiolib_drv_register(struct gpiolib_drv *drv, void *handle);

#ifdef __cplusplus
}
#endif

#endif
