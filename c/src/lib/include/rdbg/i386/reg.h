

#define NBREGS	19

#define SS      18
#define UESP    17
#define EFL     16
#define CS      15
#define EIP     14
#define ERR     13
#define TRAPNO  12
#define EAX     11
#define ECX     10
#define EDX     9
#define EBX     8
#define ESP     7
#define EBP     6
#define ESI     5
#define EDI     4
#define DS      3
#define ES      2
#define FS      1
#define GS      0

typedef unsigned int regs[NBREGS];

/* To be used in common code */
typedef regs REGS;




