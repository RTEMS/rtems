/*	$NetBSD: elf_machdep.h,v 1.7 2002/01/28 21:34:48 thorpej Exp $	*/

#define	ELF32_MACHDEP_ENDIANNESS	ELFDATA2MSB
#define	ELF32_MACHDEP_ID_CASES						\
		case EM_ALTERA_NIOS2:						\
			break;

#define	ELF64_MACHDEP_ENDIANNESS	XXX	/* break compilation */
#define	ELF64_MACHDEP_ID_CASES						\
		/* no 64-bit ELF machine types supported */

#define	ELF32_MACHDEP_ID	EM_ALTERA_NIOS2

/*
 * Machine-dependent ELF flags.  These are defined by the GNU tools.
 */
#define	EF_NIOS2	0x00810000

#define ARCH_ELFSIZE		32	/* MD native binary size */

/* NIOS2 relocation types */
#define	R_NIOS2_NONE	0
#define	R_NIOS2_32	1
#define	R_NIOS2_16	2
#define	R_NIOS2_8		3
#define	R_NIOS2_PC32	4
#define	R_NIOS2_PC16	5
#define	R_NIOS2_PC8	6
#define	R_NIOS2_GOT32	7
#define	R_NIOS2_GOT16	8
#define	R_NIOS2_GOT8	9
#define	R_NIOS2_GOT32O	10
#define	R_NIOS2_GOT16O	11
#define	R_NIOS2_GOT8O	12
#define	R_NIOS2_PLT32	13
#define	R_NIOS2_PLT16	14
#define	R_NIOS2_PLT8	15
#define	R_NIOS2_PLT32O	16
#define	R_NIOS2_PLT16O	17
#define	R_NIOS2_PLT8O	18
#define	R_NIOS2_COPY	19
#define	R_NIOS2_GLOB_DAT	20
#define	R_NIOS2_JMP_SLOT	21
#define	R_NIOS2_RELATIVE	22

#define	R_TYPE(name)	__CONCAT(R_NIOS2_,name)
