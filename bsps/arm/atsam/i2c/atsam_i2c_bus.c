/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/atsam-clock-config.h>
#include <bsp/atsam-i2c.h>

#include <rtems/irq-extension.h>

#define ATSAMV_I2C_IRQ_ERROR \
	(TWIHS_IDR_ARBLST \
	| TWIHS_IDR_TOUT \
	| TWIHS_IDR_OVRE \
	| TWIHS_IDR_UNRE \
	| TWIHS_IDR_NACK)

#define TEN_BIT_MASK 0xFC00
#define SEVEN_BIT_MASK 0xFF80
#define TEN_BIT_START_ADDR_MASK 0x78
#define LAST_TWO_BITS_MASK 0x03
#define LAST_BYTE_MASK 0x00FF

static void
atsam_i2c_disable_interrupts(Twihs *regs)
{
	regs->TWIHS_IDR = 0xFFFFFFFF;
}

static void
atsam_i2c_set_transfer_status(transfer_desc *trans_desc, transfer_state state)
{
	trans_desc->trans_state = state;
}

static void
atsam_i2c_continue_read(Twihs *regs, transfer_desc *trans_desc)
{
	trans_desc->data[trans_desc->already_transferred] = TWI_ReadByte(regs);
	trans_desc->already_transferred++;

	/* check for transfer finish */
	if (trans_desc->already_transferred == trans_desc->data_size) {
		if (trans_desc->stop_request){
			TWI_DisableIt(regs, TWIHS_IDR_RXRDY);
			TWI_EnableIt(regs, TWIHS_IER_TXCOMP);
			atsam_i2c_set_transfer_status(trans_desc, TX_RX_STOP_SENT);
		} else {
			atsam_i2c_set_transfer_status(trans_desc, RX_CONT_MESSAGE_NEEDED);
		}
	}
	/* Last byte? */
	else if ((trans_desc->already_transferred == (trans_desc->data_size - 1))
	    && (trans_desc->stop_request)){
		TWI_Stop(regs);
	}
}

static bool
atsam_i2c_is_state(transfer_desc *trans_desc, transfer_state state)
{
	return (trans_desc->trans_state == state);
}

static void
atsam_i2c_continue_write(Twihs *regs, transfer_desc *trans_desc)
{
	/* Transfer finished ? */
	if (trans_desc->already_transferred == trans_desc->data_size) {
		TWI_DisableIt(regs, TWIHS_IDR_TXRDY);
		if (trans_desc->stop_request){
			TWI_EnableIt(regs, TWIHS_IER_TXCOMP);
			TWI_SendSTOPCondition(regs);
			atsam_i2c_set_transfer_status(trans_desc, TX_RX_STOP_SENT);
		} else {
			atsam_i2c_set_transfer_status(trans_desc, TX_CONT_MESSAGE_NEEDED);
		}
	}
	/* Bytes remaining */
	else {
		TWI_WriteByte(regs,
		    trans_desc->data[trans_desc->already_transferred]);
		trans_desc->already_transferred++;
	}
}

static void
atsam_i2c_finish_write_transfer(Twihs *regs, transfer_desc *trans_desc)
{
		TWI_ReadByte(regs);
		TWI_DisableIt(regs, TWIHS_IDR_TXCOMP);
		trans_desc->status = 0;
}

static void
atsam_i2c_next_packet(atsam_i2c_bus *bus)
{
	i2c_msg *msg;

	++bus->msgs;
	--bus->msg_todo;

	msg = &bus->msgs[0];

	bus->current_msg_todo = msg->len;
	bus->current_msg_byte = msg->buf;
}

static void
atsam_i2c_set_td(atsam_i2c_bus *bus, uint32_t already_transferred,
    bool stop_needed)
{
	transfer_desc *trans_desc = &bus->trans_desc;

	trans_desc->status = ASYNC_STATUS_PENDING;
	trans_desc->data = bus->current_msg_byte;
	trans_desc->data_size = bus->current_msg_todo;
	trans_desc->already_transferred = already_transferred;
	trans_desc->stop_request = stop_needed;
}

static bool
atsam_i2c_set_address_size(const i2c_msg *msg)
{
	bool rv = ((msg->flags & I2C_M_TEN) == 0) ? false : true;
	return rv;
}

static void
atsam_i2c_set_address_regs(Twihs *regs, uint16_t address, bool ten_bit_addr,
    bool read_transfer)
{
	/* No internal addresses used */
	uint32_t mmr_temp = 0;
	uint32_t iadr_temp = 0;

	assert(regs != NULL);
	if (ten_bit_addr){
		uint8_t addr_temp = TEN_BIT_START_ADDR_MASK;
		assert(address & TEN_BIT_MASK);
		mmr_temp = (1u << 8) | ((addr_temp & LAST_TWO_BITS_MASK) << 16);
		iadr_temp = (addr_temp & LAST_BYTE_MASK);
	} else {
		assert((address & SEVEN_BIT_MASK) == 0);
		mmr_temp = (address << 16);
	}

	if (read_transfer){
		mmr_temp |= TWIHS_MMR_MREAD;
	}

	/* Set slave and number of internal address bytes */
	regs->TWIHS_MMR = 0;
	regs->TWIHS_MMR = mmr_temp;

	/* Set internal address bytes */
	regs->TWIHS_IADR = 0;
	regs->TWIHS_IADR = iadr_temp;
}

static void
atsam_i2c_setup_read_transfer(Twihs *regs, bool ctrl, uint16_t slave_addr,
    bool send_stop)
{
	TWI_EnableIt(regs, TWIHS_IER_RXRDY);
	atsam_i2c_set_address_regs(regs, slave_addr, ctrl, true);
	if (send_stop){
		regs->TWIHS_CR = TWIHS_CR_START | TWIHS_CR_STOP;
	} else {
		regs->TWIHS_CR = TWIHS_CR_START;
	}
}

static void
atsam_i2c_setup_write_transfer(atsam_i2c_bus *bus, Twihs *regs, bool ctrl,
    uint16_t slave_addr)
{
	atsam_i2c_set_address_regs(regs, slave_addr, ctrl, false);
	TWI_WriteByte(regs, *bus->current_msg_byte);
	TWI_EnableIt(regs, TWIHS_IER_TXRDY);
}

static void
atsam_i2c_setup_transfer(atsam_i2c_bus *bus, Twihs *regs)
{
	const i2c_msg *msgs = bus->msgs;
	bool send_stop = false;
	uint32_t msg_todo = bus->msg_todo;
	uint16_t slave_addr;
	bool ten_bit_addr = false;
	uint32_t already_transferred;
	bool stop_needed = true;

	ten_bit_addr = atsam_i2c_set_address_size(msgs);

	if ((msg_todo > 1) && ((msgs[1].flags & I2C_M_NOSTART) != 0)){
		stop_needed = false;
	}

	bus->read = (msgs->flags & I2C_M_RD) != 0;
	slave_addr = msgs->addr;
	already_transferred = (bus->read == true) ? 0 : 1;
	atsam_i2c_set_td(bus, already_transferred, stop_needed);

	transfer_desc *trans_desc = &bus->trans_desc;

	if (bus->read){
		if (bus->current_msg_todo == 1){
			send_stop = true;
		}
		atsam_i2c_set_transfer_status(trans_desc, RX_SEND_DATA);
		atsam_i2c_setup_read_transfer(regs, ten_bit_addr,
		    slave_addr, send_stop);
	} else {
		atsam_i2c_set_transfer_status(trans_desc, TX_SEND_DATA);
		atsam_i2c_setup_write_transfer(bus, regs, ten_bit_addr,
		    slave_addr);
	}
}

static void
atsam_i2c_interrupt(void *arg)
{
	atsam_i2c_bus *bus = arg;
	uint32_t irqstatus;
	bool done = false;
	transfer_desc *trans_desc;

	Twihs *regs = bus->regs;

	/* read interrupts */
	irqstatus = regs->TWIHS_SR;

	if((irqstatus & (TWIHS_SR_ARBLST | TWIHS_SR_NACK)) != 0) {
		done = true;
	}

	trans_desc = &bus->trans_desc;

	if (((irqstatus & TWIHS_SR_RXRDY) != 0) &&
	    (atsam_i2c_is_state(trans_desc, RX_SEND_DATA))){
		/* carry on read transfer */
		atsam_i2c_continue_read(regs, trans_desc);
	} else if (((irqstatus & TWIHS_SR_TXCOMP) != 0) &&
	    (atsam_i2c_is_state(trans_desc, TX_RX_STOP_SENT))){
		atsam_i2c_finish_write_transfer(regs, trans_desc);
		done = true;
	} else if (((irqstatus & TWIHS_SR_TXRDY) != 0) &&
	    (atsam_i2c_is_state(trans_desc, TX_SEND_DATA))){
		atsam_i2c_continue_write(regs, trans_desc);
		if (trans_desc->trans_state == TX_CONT_MESSAGE_NEEDED){
			done = true;
		}
	}

	if(done){
		uint32_t err = irqstatus & ATSAMV_I2C_IRQ_ERROR;

		atsam_i2c_next_packet(bus);
		if (bus->msg_todo == 0 || err != 0) {
			atsam_i2c_disable_interrupts(regs);
			rtems_binary_semaphore_post(&bus->sem);
		} else {
			atsam_i2c_setup_transfer(bus, regs);
		}
	}
}

static int
atsam_i2c_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t msg_count)
{
	atsam_i2c_bus *bus = (atsam_i2c_bus *)base;
	Twihs *regs;
	uint32_t i;
	int eno;

	if (msg_count < 1){
		return 0;
	}

	for (i = 0; i < msg_count; ++i) {
		if ((msgs[i].flags & I2C_M_RECV_LEN) != 0) {
			return -EINVAL;
		}
	}

	bus->msgs = &msgs[0];
	bus->msg_todo = msg_count;
	bus->current_msg_todo = msgs[0].len;
	bus->current_msg_byte = msgs[0].buf;

	regs = bus->regs;

	atsam_i2c_setup_transfer(bus, regs);

	regs->TWIHS_IER = ATSAMV_I2C_IRQ_ERROR;

	eno = rtems_binary_semaphore_wait_timed_ticks(
		&bus->sem,
		bus->base.timeout
	);
	if (eno != 0) {
		TWI_ConfigureMaster(bus->regs, bus->output_clock, BOARD_MCK);
		rtems_binary_semaphore_try_wait(&bus->sem);
		return -ETIMEDOUT;
	}

	return 0;
}

static int
atsam_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
	atsam_i2c_bus *bus = (atsam_i2c_bus *)base;
	bus->output_clock = clock;
	TWI_ConfigureMaster(bus->regs, clock, BOARD_MCK);
	return 0;
}

static void
atsam_i2c_destroy(i2c_bus *base)
{
	atsam_i2c_bus *bus = (atsam_i2c_bus *)base;
	rtems_status_code sc;

	sc = rtems_interrupt_handler_remove(bus->irq, atsam_i2c_interrupt, bus);
	assert(sc == RTEMS_SUCCESSFUL);

	i2c_bus_destroy_and_free(&bus->base);
}

static void
atsam_i2c_init(atsam_i2c_bus *bus, uint32_t board_id, const Pin *pins)
{

	/* Initialize the TWI */
	PIO_Configure(pins, TWI_AMOUNT_PINS);

	/* Enable the TWI clock */
	ENABLE_PERIPHERAL(board_id);

	TWI_ConfigureMaster(bus->regs, bus->output_clock, BOARD_MCK);
}

int
i2c_bus_register_atsam(
    const char *bus_path,
    Twihs *register_base,
    rtems_vector_number irq,
    const Pin pins[TWI_AMOUNT_PINS]
)
{
	atsam_i2c_bus *bus;
	rtems_status_code sc;

	bus = (atsam_i2c_bus *) i2c_bus_alloc_and_init(sizeof(*bus));
	if (bus == NULL){
		return -1;
	}

	rtems_binary_semaphore_init(&bus->sem, "ATSAM I2C");
	bus->regs = register_base;
	bus->irq = irq;
	bus->output_clock = I2C_BUS_CLOCK_DEFAULT;

	atsam_i2c_init(bus, irq, pins);

	sc = rtems_interrupt_handler_install(
	    irq,
	    "ATSAM I2C",
	    RTEMS_INTERRUPT_UNIQUE,
	    atsam_i2c_interrupt,
	    bus
	);
	if(sc != RTEMS_SUCCESSFUL){
		(*bus->base.destroy)(&bus->base);

		rtems_set_errno_and_return_minus_one(EIO);
	}

	bus->base.transfer = atsam_i2c_transfer;
	bus->base.set_clock = atsam_i2c_set_clock;
	bus->base.destroy = atsam_i2c_destroy;

	return i2c_bus_register(&bus->base, bus_path);
}
