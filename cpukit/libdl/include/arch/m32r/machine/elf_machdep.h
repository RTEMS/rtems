#define ELF32_MACHDEP_ENDIANNESS		ELFDATA2MSB

#define ELF32_MACHDEP_ID_CASES \
	case EM_M32R: \
		break;

#define ELF32_MACHDEP_ID	EM_M32R

#define ARCH_ELFSIZE	32

#define R_M32R_NONE 0
/*-----------OLD TYPE-------------*/
#define R_M32R_16            1
#define R_M32R_32            2
#define R_M32R_24            3
#define R_M32R_10_PCREL      4
#define R_M32R_18_PCREL      5
#define R_M32R_26_PCREL      6
#define R_M32R_HI16_ULO      7
#define R_M32R_HI16_SLO      8
#define R_M32R_LO16          9
#define R_M32R_SDA16         10
#define R_M32R_GNU_VTINHERIT 11
#define R_M32R_GNU_VTENTRY   12
/*--------------------------------*/

#define R_M32R_16_RELA       33
#define R_M32R_32_RELA       34
#define R_M32R_24_RELA       35
#define R_M32R_18_PCREL_RELA 37
#define R_M32R_26_PCREL_RELA 38
#define R_M32R_HI16_ULO_RELA 39
#define R_M32R_HI16_SLO_RELA 40
#define R_M32R_LO16_RELA     41
#define R_M32R_SDA16_RELA    42
#define R_M32R_RELA_GNU_VTINHERIT 43
#define R_M32R_RELA_GNU_VTENTRY   44

#define R_TYPE(name)		__CONCAT(R_M32R_,name)
