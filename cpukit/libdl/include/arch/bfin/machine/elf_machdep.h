#define ELF32_MACHDEP_ENDIANNESS  ELFDATA2LSB

#define ELF32_MACHDEP_ID_CASES \
  case EM_BLACKFIN: \
    break;

#define ELF32_MACHDEP_ID  EM_BLACKFIN

#define ARCH_ELFSIZE          32

#define R_BFIN_UNUSED0        0

#define R_BFIN_RIMM16         5
#define R_BFIN_LUIMM16        6
#define R_BFIN_HUIMM16        7
#define R_BFIN_PCREL12_JUMP_S 8
#define R_BFIN_PCREL24_JUMP_X 9
#define R_BFIN_PCREL24        10
#define R_BFIN_PCREL24_JU     13
#define R_BFIN_PCREL24_CALL_X 14

#define R_BFIN_BYTE_DATA      16
#define R_BFIN_BYTE2_DATA     17
#define R_BFIN_BYTE4_DATA     18



#define R_TYPE(name)    __CONCAT(R_BFIN_,name)
