/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

/*
 * \file
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include <string.h>

#include <rtems/irq-extension.h>
#include <rtems/sysinit.h>
#include <bsp/fatal.h>

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/* Maximum number of interrupt sources that can be defined. This
 * constant can be increased, but the current value is the smallest possible
 * that will be compatible with all existing projects. */
#define MAX_INTERRUPT_SOURCES       7

/*----------------------------------------------------------------------------
 *        Local types
 *----------------------------------------------------------------------------*/

/**
 * Describes a PIO interrupt source, including the PIO instance triggering the
 * interrupt and the associated interrupt handler.
 */
typedef struct _InterruptSource {
	/* Pointer to the source pin instance. */
	const Pin *pPin;

	/* Interrupt handler. */
	void (*handler)(const Pin *, void *arg);

	void *arg;
} InterruptSource;

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/* List of interrupt sources. */
static InterruptSource _aIntSources[MAX_INTERRUPT_SOURCES];

/* Number of currently defined interrupt sources. */
static uint32_t _dwNumSources = 0;

/*----------------------------------------------------------------------------
 *        Local Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Handles all interrupts on the given PIO controller.
 * \param id  PIO controller ID.
 * \param pPio  PIO controller base address.
 */
static void PIO_Interrupt(Pio *pPio, uint32_t id)
{
	uint32_t status;
	uint32_t status_save;
	size_t i;

	do {
		status = pPio->PIO_ISR;
		status &= pPio->PIO_IMR;
		status_save = status;

		for (i = 0; status != 0 && i < MAX_INTERRUPT_SOURCES; ++i) {
			const InterruptSource *is = &_aIntSources[i];
			const Pin *pin = is->pPin;;

			if (pin->id == id) {
				uint32_t mask = pin->mask;

				if ((status & mask) != 0) {
					status &= ~mask;
					(*is->handler)(pin, is->arg);
				}
			}
		}
	} while (status_save != 0);
}

/*----------------------------------------------------------------------------
 *        Global Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Parallel IO Controller A interrupt handler
 * \Redefined PIOA interrupt handler for NVIC interrupt table.
 */
static void PIOA_Interrupt(void *arg)
{
	PIO_Interrupt(arg, ID_PIOA);
}

/**
 * \brief Parallel IO Controller B interrupt handler
 * \Redefined PIOB interrupt handler for NVIC interrupt table.
 */
static void PIOB_Interrupt(void *arg)
{
	PIO_Interrupt(arg, ID_PIOB);
}

/**
 * \brief Parallel IO Controller C interrupt handler
 * \Redefined PIOC interrupt handler for NVIC interrupt table.
 */
static void PIOC_Interrupt(void *arg)
{
	PIO_Interrupt(arg, ID_PIOC);
}


/**
 * \brief Parallel IO Controller D interrupt handler
 * \Redefined PIOD interrupt handler for NVIC interrupt table.
 */
static void PIOD_Interrupt(void *arg)
{
	PIO_Interrupt(arg, ID_PIOD);
}

/**
 * \brief Parallel IO Controller E interrupt handler
 * \Redefined PIOE interrupt handler for NVIC interrupt table.
 */
static void PIOE_Interrupt(void *arg)
{
	PIO_Interrupt(arg, ID_PIOE);
}

static void PIO_SysInitializeInterrupts(void)
{
	rtems_status_code sc;

	TRACE_DEBUG("PIO_Initialize()\n\r");

	/* Configure PIO interrupt sources */
	TRACE_DEBUG("PIO_Initialize: Configuring PIOA\n\r");
	PMC_EnablePeripheral(ID_PIOA);
	PIOA->PIO_ISR;
	PIOA->PIO_IDR = 0xFFFFFFFF;
	sc = rtems_interrupt_handler_install(
		PIOA_IRQn,
		"PIO A",
		RTEMS_INTERRUPT_UNIQUE,
		PIOA_Interrupt,
		PIOA
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_PIO_IRQ_A);
	}

	TRACE_DEBUG("PIO_Initialize: Configuring PIOB\n\r");
	PMC_EnablePeripheral(ID_PIOB);
	PIOB->PIO_ISR;
	PIOB->PIO_IDR = 0xFFFFFFFF;
	sc = rtems_interrupt_handler_install(
		PIOB_IRQn,
		"PIO B",
		RTEMS_INTERRUPT_UNIQUE,
		PIOB_Interrupt,
		PIOB
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_PIO_IRQ_B);
	}

	TRACE_DEBUG("PIO_Initialize: Configuring PIOC\n\r");
	PMC_EnablePeripheral(ID_PIOC);
	PIOC->PIO_ISR;
	PIOC->PIO_IDR = 0xFFFFFFFF;
	sc = rtems_interrupt_handler_install(
		PIOC_IRQn,
		"PIO C",
		RTEMS_INTERRUPT_UNIQUE,
		PIOC_Interrupt,
		PIOC
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_PIO_IRQ_C);
	}

	TRACE_DEBUG("PIO_Initialize: Configuring PIOD\n\r");
	PMC_EnablePeripheral(ID_PIOD);
	PIOD->PIO_ISR;
	PIOD->PIO_IDR = 0xFFFFFFFF;
	sc = rtems_interrupt_handler_install(
		PIOD_IRQn,
		"PIO D",
		RTEMS_INTERRUPT_UNIQUE,
		PIOD_Interrupt,
		PIOD
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_PIO_IRQ_D);
	}

	TRACE_DEBUG("PIO_Initialize: Configuring PIOE\n\r");
	PMC_EnablePeripheral(ID_PIOE);
	PIOE->PIO_ISR;
	PIOE->PIO_IDR = 0xFFFFFFFF;
	sc = rtems_interrupt_handler_install(
		PIOE_IRQn,
		"PIO E",
		RTEMS_INTERRUPT_UNIQUE,
		PIOE_Interrupt,
		PIOE
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_PIO_IRQ_E);
	}
}

RTEMS_SYSINIT_ITEM(PIO_SysInitializeInterrupts, RTEMS_SYSINIT_BSP_START,
    RTEMS_SYSINIT_ORDER_LAST);

/**
 * Configures a PIO or a group of PIO to generate an interrupt on status
 * change. The provided interrupt handler will be called with the triggering
 * pin as its parameter (enabling different pin instances to share the same
 * handler).
 * \param pPin  Pointer to a Pin instance.
 * \param handler  Interrupt handler function pointer.
 * \param arg Pointer to interrupt handler argument
 */
void PIO_ConfigureIt(const Pin *pPin, void (*handler)(const Pin *, void *arg),
    void *arg)
{
	InterruptSource *pSource;
	rtems_interrupt_level level;

	TRACE_DEBUG("PIO_ConfigureIt()\n\r");

	rtems_interrupt_disable(level);

	if (_dwNumSources == MAX_INTERRUPT_SOURCES) {
		bsp_fatal(ATSAM_FATAL_PIO_CONFIGURE_IT);
	}

	pSource = &(_aIntSources[_dwNumSources]);
	pSource->pPin = pPin;
	pSource->handler = handler;
	pSource->arg = arg;

	_dwNumSources++;

	rtems_interrupt_enable(level);

	/* Define new source */
	TRACE_DEBUG("PIO_ConfigureIt: Defining new source #%d.\n\r", _dwNumSources);
}

/**
 * Search for a PIO interrupt and remove it if it is there.
 * \param pPin  Pointer to a Pin instance.
 * \param handler  Interrupt handler function pointer.
 * \param arg Pointer to interrupt handler argument
 * \return RTEMS_SUCCESSFUL if removed.
 * \return RTEMS_UNSATISFIED if the handler couldn't be found
 */
rtems_status_code PIO_RemoveIt(const Pin *pPin,
    void (*handler)(const Pin *, void *arg), void *arg)
{
	InterruptSource *pSource;
	rtems_interrupt_level level;
	rtems_status_code sc = RTEMS_UNSATISFIED;
	uint32_t i;
	uint32_t j;

	TRACE_DEBUG("PIO_RemoveIt()\n\r");

	rtems_interrupt_disable(level);

	for(i = 0; i < _dwNumSources; ++i) {
		pSource = &(_aIntSources[_dwNumSources]);
		if(pSource->pPin == pPin &&
		    pSource->handler == handler &&
		    pSource->arg == arg) {
			if(i + 1 < _dwNumSources) {
				TRACE_DEBUG("PIO_RemoveIt: Remove #%d.\n\r", i);
				/* Move remaining sources */
				memcpy(pSource,
				    &(_aIntSources[i+1]),
				    sizeof(_aIntSources[0])*(_dwNumSources-i-1)
				    );
			}
			_dwNumSources--;
			sc = RTEMS_SUCCESSFUL;
		}
	}

	rtems_interrupt_enable(level);

	return sc;
}
