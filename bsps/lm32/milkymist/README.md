milkymist
=========

Full RTEMS port to the Milkymist One. Supports Milkymist SoC 1.0.x.

Includes drivers for:
- Multi-standard video input (PAL/SECAM/NTSC)
- Two DMX512 (RS485) ports
- MIDI IN and MIDI OUT ports
- VGA output
- AC'97 audio
- NOR flash
- 10/100 Ethernet
- Memory card (experimental and incomplete)
- USB host connectors (input devices only, using the softusb-input firmware)
- RC5 infrared receiver
- RS232 debug port

For more information: http://www.milkymist.org/


UART
----
Initialization:

	set the CSR_UART_DIVISOR to the correct VALUE,
	depending on the internal frequency of the LatticeMico32 softcore.

	for the ML401 board, this value is calculated using this formula : clk_frequency/230400/16
	clk_frequency  = 100000000 Hz
	=> we must set CSR_UART_DIVISOR to 27

How to send a byte to uart : 

```c
void writechar(char c)
{
	        CSR_UART_RXTX = c;
		while(!(irq_pending() & IRQ_UARTTX));
		irq_ack(IRQ_UARTTX);
}
```

How to receive a byte from uart : 


```c
char readchar()
{
	char c;
	while(!(irq_pending() & IRQ_UARTRX));
	irq_ack(IRQ_UARTRX);
	c = CSR_UART_RXTX;
	return c;
}
```
