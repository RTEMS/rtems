#define ELF32_MACHDEP_ENDIANNESS		ELFDATA2LSB

#define ELF32_MACHDEP_ID_CASES \
	case EM_V850: \
		break;

#define ELF32_MACHDEP_ID	EM_V850



#define EF_V850_ARCH		0xf0000000
#define E_V850_ARCH		0x00000000
#define E_V850E_ARCH		0x10000000
#define E_V850E1_ARCH		0x20000000
#define E_V850E2_ARCH		0x30000000
#define E_V850E2V3_ARCH		0x40000000

#define ARCH_ELFSIZE	32


#define		R_V850_NONE								0
#define		R_V850_9_PCREL						1
#define		R_V850_22_PCREL						2
#define		R_V850_HI16_S							3
#define		R_V850_HI16								4
#define		R_V850_LO16								5
#define		R_V850_ABS32							6
#define		R_V850_16									7
#define		R_V850_8									8
#define		R_V850_SDA_16_16_OFFSET		9
#define		R_V850_SDA_15_16_OFFSET		10
#define		R_V850_ZDA_16_16_OFFSET		11
#define		R_V850_ZDA_15_16_OFFSET		12
#define		R_V850_TDA_6_8_OFFSET			13
#define		R_V850_TDA_7_8_OFFSET			14
#define		R_V850_TDA_7_7_OFFSET			15
#define		R_V850_TDA_16_16_OFFSET		16
#define		R_V850_TDA_4_5_OFFSET			17
#define		R_V850_TDA_4_4_OFFSET			18
#define		R_V850_SDA_16_16_SPLIT_OFFSET		19
#define		R_V850_ZDA_16_16_SPLIT_OFFSET		20
#define		R_V850_CALLT_6_7_OFFSET			21
#define		R_V850_CALLT_16_16_OFFSET		22
#define		R_V850_GNU_VTINHERIT				23
#define		R_V850_GNU_VTENTRY					24
#define		R_V850_LONGCALL							25
#define		R_V850_LONGJUMP							26
#define		R_V850_ALIGN								27
#define		R_V850_REL32								28
#define		R_V850_LO16_SPLIT_OFFSET		29
#define		R_V850_16_PCREL							30
#define		R_V850_17_PCREL							31
#define		R_V850_23										32
#define		R_V850_32_PCREL							33
#define		R_V850_32_ABS								34
#define		R_V850_16_SPLIT_OFFSET			35
#define		R_V850_16_S1								36
#define		R_V850_LO16_S1							37
#define		R_V850_CALLT_15_16_OFFSET		38
#define		R_V850_32_GOTPCREL					39
#define		R_V850_16_GOT								40
#define		R_V850_32_GOT								41
#define		R_V850_22_PLT								42
#define		R_V850_32_PLT								43
#define		R_V850_COPY									44
#define		R_V850_GLOB_DAT							45
#define		R_V850_JMP_SLOT							46
#define		R_V850_RELATIVE							47
#define		R_V850_16_GOTOFF						48
#define		R_V850_32_GOTOFF						49
#define		R_V850_CODE									50
#define		R_V850_DATA									51

#define R_TYPE(name)		__CONCAT(R_V850_,name)
