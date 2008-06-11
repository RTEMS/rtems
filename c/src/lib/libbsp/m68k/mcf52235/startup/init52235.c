/*
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 */

#include <rtems.h>
#include <bsp.h>

extern void _wr_vbr(uint32_t);
extern void init_main();
extern int boot_card(int, char **, char **);

/*
 * From linkcmds
 */

extern uint8_t _RamBase[];
extern uint8_t _INTERRUPT_VECTOR[];

extern uint8_t _clear_start[];
extern uint8_t _clear_end[];

extern uint8_t _data_src_start[];
extern uint8_t _data_dest_start[];
extern uint8_t _data_dest_end[];      


void Init52235 (void)
{
	register uint32_t i;
	register uint32_t *dp, *sp;
    register uint8_t *dbp, *sbp;

    /* 
     * Initialize the hardware
     */
    init_main();

	/* 
     * Copy the vector table to RAM 
     */

	if(_RamBase != _INTERRUPT_VECTOR)
	{
        sp = (uint32_t *) _INTERRUPT_VECTOR;
        dp = (uint32_t *) _RamBase;
        for(i = 0; i < 256; i++)
		{
			*dp++ = *sp++;
		}
	}

	_wr_vbr((uint32_t) _RamBase);

    /* 
	 * Move initialized data from ROM to RAM. 
	 */
	if (_data_src_start != _data_dest_start)
	{
		dbp = (uint8_t *) _data_dest_start;
		sbp = (uint8_t *) _data_src_start;
		i = _data_dest_end - _data_dest_start;
		while (i--)
			*dbp++ = *sbp++;
	}
    
	/* 
	 * Zero uninitialized data 
	 */

	if (_clear_start != _clear_end)
	{
        sbp = _clear_start;
        dbp = _clear_end;        
		i = dbp - sbp;
		while (i--)
			*sbp++ = 0;
	}
    
    /*
     * We have to call some kind of RTEMS function here!
     */
    
    boot_card(0, 0, 0);
    for(;;);
}

#if 0
/***********************************************************************
 *
 * This is the exception handler for all defined exceptions.  Most
 * exceptions do nothing, but some of the more important ones are
 * handled to some extent.
 *
 * Called by asm_exception_handler 
 *
 * The ColdFire family of processors has a simplified exception stack
 * frame that looks like the following:
 *
 *              3322222222221111 111111
 *              1098765432109876 5432109876543210
 *           8 +----------------+----------------+
 *             |         Program Counter         |
 *           4 +----------------+----------------+
 *             |FS/Fmt/Vector/FS|      SR        |
 *   SP -->  0 +----------------+----------------+
 *
 * The stack self-aligns to a 4-byte boundary at an exception, with
 * the FS/Fmt/Vector/FS field indicating the size of the adjustment
 * (SP += 0,1,2,3 bytes).
 */
#define MCF5XXX_RD_SF_FORMAT(PTR)	\
	((*((uint16_t *)(PTR)) >> 12) & 0x00FF)

#define MCF5XXX_RD_SF_VECTOR(PTR)	\
	((*((uint16_t *)(PTR)) >>  2) & 0x00FF)

#define MCF5XXX_RD_SF_FS(PTR)		\
	( ((*((uint16_t *)(PTR)) & 0x0C00) >> 8) | (*((uint16_t *)(PTR)) & 0x0003) )

#define MCF5XXX_SF_SR(PTR)	*((uint16_t *)(PTR)+1)
#define MCF5XXX_SF_PC(PTR)	*((uint32_t *)(PTR)+1)

void ecatch(const char* a_error, uint32_t a_pc)
{
    volatile const char* error = a_error;
    volatile uint32_t pc = a_pc;
    return;
}

void mcf5xxx_exception_handler(void *framep) 
{
    volatile uint16_t sr;
    volatile uint32_t pc;
    
	switch (MCF5XXX_RD_SF_FORMAT(framep))
	{
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		default:
            // Illegal stack type
			ecatch("Illegal stack type", MCF5XXX_SF_PC(framep));
			break;
	}

	switch (MCF5XXX_RD_SF_VECTOR(framep))
	{
		case 2:
			ecatch("Access Error", MCF5XXX_SF_PC(framep));
			switch (MCF5XXX_RD_SF_FS(framep))
			{
				case 4:
					ecatch("Error on instruction fetch\n", 0);
					break;
				case 8:
					ecatch("Error on operand write\n", 0);
					break;
				case 9:
					ecatch("Attempted write to write-protected space\n", 0);
					break;
				case 12:
					ecatch("Error on operand read\n", 0);
					break;
				default:
					ecatch("Reserved Fault Status Encoding\n", 0);
					break;
			}
			break;
		case 3:
			ecatch("Address Error", MCF5XXX_SF_PC(framep));
			switch (MCF5XXX_RD_SF_FS(framep))
			{
				case 4:
					ecatch("Error on instruction fetch\n", 0);
					break;
				case 8:
					ecatch("Error on operand write\n", 0);
					break;
				case 9:
					ecatch("Attempted write to write-protected space\n", 0);
					break;
				case 12:
					ecatch("Error on operand read\n", 0);
					break;
				default:
					ecatch("Reserved Fault Status Encoding\n", 0);
					break;
			}
			break;
		case 4:
			ecatch("Illegal instruction", MCF5XXX_SF_PC(framep));
			break;
		case 8:
			ecatch("Privilege violation", MCF5XXX_SF_PC(framep));
			break;
		case 9:
			ecatch("Trace Exception", MCF5XXX_SF_PC(framep));
			break;
		case 10:
			ecatch("Unimplemented A-Line Instruction", MCF5XXX_SF_PC(framep));
			break;
		case 11:
			ecatch("Unimplemented F-Line Instruction", MCF5XXX_SF_PC(framep));
			break;
		case 12:
			ecatch("Debug Interrupt", MCF5XXX_SF_PC(framep));
			break;
		case 14:
			ecatch("Format Error", MCF5XXX_SF_PC(framep));
			break;
		case 15:
			ecatch("Unitialized Interrupt", MCF5XXX_SF_PC(framep));
			break;
		case 24:
			ecatch("Spurious Interrupt", MCF5XXX_SF_PC(framep));
			break;
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
			ecatch("Autovector interrupt level %d\n", MCF5XXX_RD_SF_VECTOR(framep) - 24);
			break;
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			ecatch("TRAP #%d\n", MCF5XXX_RD_SF_VECTOR(framep) - 32);
			break;
		case 5:
		case 6:
		case 7:
		case 13:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
		case 63:
			ecatch("Reserved: #%d\n", MCF5XXX_RD_SF_VECTOR(framep));
			break;
		default:
            ecatch("derivitave handler", MCF5XXX_RD_SF_VECTOR(framep));
			break;
	}
}
#endif
