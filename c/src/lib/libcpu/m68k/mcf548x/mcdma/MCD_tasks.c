/*
 * File:        MCD_tasks.c
 * Purpose:     Contains task code and structures for Multi-channel DMA
 *
 * Notes:
 */

#include "MCD_dma.h"

u32 MCD_varTab0[];
u32 MCD_varTab1[];
u32 MCD_varTab2[];
u32 MCD_varTab3[];
u32 MCD_varTab4[];
u32 MCD_varTab5[];
u32 MCD_varTab6[];
u32 MCD_varTab7[];
u32 MCD_varTab8[];
u32 MCD_varTab9[];
u32 MCD_varTab10[];
u32 MCD_varTab11[];
u32 MCD_varTab12[];
u32 MCD_varTab13[];
u32 MCD_varTab14[];
u32 MCD_varTab15[];

u32 MCD_funcDescTab0[];
#ifdef MCD_INCLUDE_EU
u32 MCD_funcDescTab1[];
u32 MCD_funcDescTab2[];
u32 MCD_funcDescTab3[];
u32 MCD_funcDescTab4[];
u32 MCD_funcDescTab5[];
u32 MCD_funcDescTab6[];
u32 MCD_funcDescTab7[];
u32 MCD_funcDescTab8[];
u32 MCD_funcDescTab9[];
u32 MCD_funcDescTab10[];
u32 MCD_funcDescTab11[];
u32 MCD_funcDescTab12[];
u32 MCD_funcDescTab13[];
u32 MCD_funcDescTab14[];
u32 MCD_funcDescTab15[];
#endif

u32 MCD_contextSave0[];
u32 MCD_contextSave1[];
u32 MCD_contextSave2[];
u32 MCD_contextSave3[];
u32 MCD_contextSave4[];
u32 MCD_contextSave5[];
u32 MCD_contextSave6[];
u32 MCD_contextSave7[];
u32 MCD_contextSave8[];
u32 MCD_contextSave9[];
u32 MCD_contextSave10[];
u32 MCD_contextSave11[];
u32 MCD_contextSave12[];
u32 MCD_contextSave13[];
u32 MCD_contextSave14[];
u32 MCD_contextSave15[];

u32 MCD_realTaskTableSrc[] =
{
    0x00000000,
    0x00000000,
    (u32)MCD_varTab0,   /* Task 0 Variable Table */
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave0,  /* Task 0 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab1,   /* Task 1 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab1,  /* Task 1 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave1,  /* Task 1 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab2,   /* Task 2 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab2,  /* Task 2 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave2,  /* Task 2 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab3,   /* Task 3 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab3,  /* Task 3 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave3,  /* Task 3 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab4,   /* Task 4 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab4,  /* Task 4 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave4,  /* Task 4 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab5,   /* Task 5 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab5,  /* Task 5 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave5,  /* Task 5 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab6,   /* Task 6 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab6,  /* Task 6 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave6,  /* Task 6 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab7,   /* Task 7 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab7,  /* Task 7 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave7,  /* Task 7 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab8,   /* Task 8 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab8,  /* Task 8 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave8,  /* Task 8 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab9,   /* Task 9 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab9,  /* Task 9 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave9,  /* Task 9 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab10,  /* Task 10 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab10, /* Task 10 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave10, /* Task 10 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab11,  /* Task 11 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab11, /* Task 11 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave11, /* Task 11 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab12,  /* Task 12 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab12, /* Task 12 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave12, /* Task 12 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab13,  /* Task 13 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab13, /* Task 13 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave13, /* Task 13 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab14,  /* Task 14 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab14, /* Task 14 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave14, /* Task 14 context save space */
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_varTab15,  /* Task 15 Variable Table */
#ifdef MCD_INCLUDE_EU
    (u32)MCD_funcDescTab15, /* Task 15 Function Descriptor Table & Flags */
#else
    (u32)MCD_funcDescTab0,  /* Task 0 Function Descriptor Table & Flags */
#endif
    0x00000000,
    0x00000000,
    (u32)MCD_contextSave15, /* Task 15 context save space */
    0x00000000,
};


u32 MCD_varTab0[] =
{   /* Task 0 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};


u32 MCD_varTab1[] =
{   /* Task 1 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab2[]=
{   /* Task 2 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab3[]=
{   /* Task 3 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab4[]=
{   /* Task 4 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab5[]=
{   /* Task 5 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab6[]=
{   /* Task 6 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab7[]=
{   /* Task 7 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab8[]=
{   /* Task 8 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab9[]=
{   /* Task 9 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab10[]=
{   /* Task 10 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab11[]=
{   /* Task 11 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab12[]=
{   /* Task 12 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab13[]=
{   /* Task 13 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab14[]=
{   /* Task 14 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_varTab15[]=
{   /* Task 15 Variable Table */
    0x00000000, /* var[0] */
    0x00000000, /* var[1] */
    0x00000000, /* var[2] */
    0x00000000, /* var[3] */
    0x00000000, /* var[4] */
    0x00000000, /* var[5] */
    0x00000000, /* var[6] */
    0x00000000, /* var[7] */
    0x00000000, /* var[8] */
    0x00000000, /* var[9] */
    0x00000000, /* var[10] */
    0x00000000, /* var[11] */
    0x00000000, /* var[12] */
    0x00000000, /* var[13] */
    0x00000000, /* var[14] */
    0x00000000, /* var[15] */
    0x00000000, /* var[16] */
    0x00000000, /* var[17] */
    0x00000000, /* var[18] */
    0x00000000, /* var[19] */
    0x00000000, /* var[20] */
    0x00000000, /* var[21] */
    0x00000000, /* var[22] */
    0x00000000, /* var[23] */
    0xe0000000, /* inc[0] */
    0x20000000, /* inc[1] */
    0x2000ffff, /* inc[2] */
    0x00000000, /* inc[3] */
    0x00000000, /* inc[4] */
    0x00000000, /* inc[5] */
    0x00000000, /* inc[6] */
    0x00000000, /* inc[7] */
};

u32 MCD_funcDescTab0[]=
{   /* Task 0 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

#ifdef MCD_INCLUDE_EU
u32 MCD_funcDescTab1[]=
{   /* Task 1 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab2[]=
{   /* Task 2 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab3[]=
{   /* Task 3 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab4[]=
{   /* Task 4 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab5[]=
{   /* Task 5 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab6[]=
{   /* Task 6 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab7[]=
{   /* Task 7 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab8[]=
{   /* Task 8 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab9[]=
{   /* Task 9 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab10[]=
{   /* Task 10 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab11[]=
{   /* Task 11 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab12[]=
{   /* Task 12 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab13[]=
{   /* Task 13 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab14[]=
{   /* Task 14 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};

u32 MCD_funcDescTab15[]=
{   /* Task 15 Function Descriptor Table */
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0xa0045670, /* mainFunc(), EU# 3 */
    0xa0000000, /* rsduFunc(), EU# 3 */
    0xa0000000, /* crcAccumVal(), EU# 3 */
    0x20000000, /* setCrcAccum(), EU# 3 */
    0x21800000, /* and(), EU# 3 */
    0x21e00000, /* or(), EU# 3 */
    0x20400000, /* add(), EU# 3 */
    0x20500000, /* sub(), EU# 3 */
    0x205a0000, /* andNot(), EU# 3 */
    0x20a00000, /* shiftR(), EU# 3 */
    0x202fa000, /* andReadyBit(), EU# 3 */
    0x202f9000, /* andNotReadyBit(), EU# 3 */
    0x202ea000, /* andWrapBit(), EU# 3 */
    0x202da000, /* andLastBit(), EU# 3 */
    0x202e2000, /* andInterruptBit(), EU# 3 */
    0x202f2000, /* andCrcRestartBit(), EU# 3 */
};
#endif /*MCD_INCLUDE_EU*/

u32 MCD_contextSave0[128];  /* Task 0 context save space */
u32 MCD_contextSave1[128];  /* Task 1 context save space */
u32 MCD_contextSave2[128];  /* Task 2 context save space */
u32 MCD_contextSave3[128];  /* Task 3 context save space */
u32 MCD_contextSave4[128];  /* Task 4 context save space */
u32 MCD_contextSave5[128];  /* Task 5 context save space */
u32 MCD_contextSave6[128];  /* Task 6 context save space */
u32 MCD_contextSave7[128];  /* Task 7 context save space */
u32 MCD_contextSave8[128];  /* Task 8 context save space */
u32 MCD_contextSave9[128];  /* Task 9 context save space */
u32 MCD_contextSave10[128]; /* Task 10 context save space */
u32 MCD_contextSave11[128]; /* Task 11 context save space */
u32 MCD_contextSave12[128]; /* Task 12 context save space */
u32 MCD_contextSave13[128]; /* Task 13 context save space */
u32 MCD_contextSave14[128]; /* Task 14 context save space */
u32 MCD_contextSave15[128]; /* Task 15 context save space */


u32 MCD_ChainNoEu_TDT[];
u32 MCD_SingleNoEu_TDT[];
#ifdef MCD_INCLUDE_EU
u32 MCD_ChainEu_TDT[];
u32 MCD_SingleEu_TDT[];
#endif
u32 MCD_ENetRcv_TDT[];
u32 MCD_ENetXmit_TDT[];

u32 MCD_modelTaskTableSrc[]=
{
    (u32)MCD_ChainNoEu_TDT,
    (u32)&((u8*)MCD_ChainNoEu_TDT)[0x0000016c],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_SingleNoEu_TDT,
    (u32)&((u8*)MCD_SingleNoEu_TDT)[0x000000d4],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
#ifdef MCD_INCLUDE_EU
    (u32)MCD_ChainEu_TDT,
    (u32)&((u8*)MCD_ChainEu_TDT)[0x000001b4],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_SingleEu_TDT,
    (u32)&((u8*)MCD_SingleEu_TDT)[0x00000124],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
#endif
    (u32)MCD_ENetRcv_TDT,
    (u32)&((u8*)MCD_ENetRcv_TDT)[0x0000009c],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    (u32)MCD_ENetXmit_TDT,
    (u32)&((u8*)MCD_ENetXmit_TDT)[0x000000d0],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
u32 MCD_ChainNoEu_TDT[]=
{
    0x80004000, /* 0000(:370):  LCDEXT: idx0 = 0x00000000; ; */
    0x8118801b, /* 0004(:370):  LCD: idx1 = var2; idx1 once var0; idx1 += inc3 */
    0xb8c60018, /* 0008(:371):    LCD: idx2 = *(idx1 + var12); idx2 once var0; idx2 += inc3 */
    0x10002b10, /* 000C(:372):      DRD1A: var10 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x7000000d, /* 0010(:373):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x018cf89f, /* 0014(:373):      DRD2B1: var6 = EU3(); EU3(idx2)  */
    0x6000000a, /* 0018(:374):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf89f, /* 001C(:374):      DRD2B1: idx0 = EU3(); EU3(idx2)  */
    0x000001f8, /* 0020(:0):    NOP */
    0x98180364, /* 0024(:378):  LCD: idx0 = idx0; idx0 == var13; idx0 += inc4 */
    0x8118801b, /* 0028(:380):    LCD: idx1 = var2; idx1 once var0; idx1 += inc3 */
    0xf8c6001a, /* 002C(:381):      LCDEXT: idx2 = *(idx1 + var12 + 8); idx2 once var0; idx2 += inc3 */
    0xb8c6601b, /* 0030(:382):      LCD: idx3 = *(idx1 + var12 + 12); ; idx3 += inc3 */
    0x10002710, /* 0034(:384):        DRD1A: var9 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x00000f18, /* 0038(:385):        DRD1A: var3 = idx3; FN=0 init=0 WS=0 RS=0 */
    0xb8c6001d, /* 003C(:387):      LCD: idx2 = *(idx1 + var12 + 20); idx2 once var0; idx2 += inc3 */
    0x10001310, /* 0040(:388):        DRD1A: var4 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x60000007, /* 0044(:389):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=7 EXT init=0 WS=0 RS=0 */
    0x014cf88b, /* 0048(:389):        DRD2B1: var5 = EU3(); EU3(idx2,var11)  */
    0x98c6001c, /* 004C(:391):      LCD: idx2 = idx1 + var12 + 4; idx2 once var0; idx2 += inc3 */
    0x00000710, /* 0050(:392):        DRD1A: var1 = idx2; FN=0 init=0 WS=0 RS=0 */
    0x98c70018, /* 0054(:393):      LCD: idx2 = idx1 + var14; idx2 once var0; idx2 += inc3 */
    0x10001f10, /* 0058(:394):        DRD1A: var7 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c818, /* 005C(:395):        DRD1A: *idx2 = var3; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 0060(:0):      NOP */
    0xc1476018, /* 0064(:399):    LCDEXT: idx1 = var2 + var14; ; idx1 += inc3 */
    0xc003231d, /* 0068(:399):    LCDEXT: idx2 = var0, idx3 = var6; idx3 == var12; idx2 += inc3, idx3 += inc5 */
    0x811a601b, /* 006C(:400):    LCD: idx4 = var2; ; idx4 += inc3 */
    0xc1862102, /* 0070(:403):      LCDEXT: idx5 = var3, idx6 = var12; idx6 < var4; idx5 += inc0, idx6 += inc2 */
    0x849be009, /* 0074(:403):      LCD: idx7 = var9; ; idx7 += inc1 */
    0x03fed7b8, /* 0078(:406):        DRD1A: *idx7; FN=0 init=31 WS=3 RS=3 */
    0xda9b001b, /* 007C(:408):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 0080(:408):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x1000cb20, /* 0084(:409):        DRD1A: *idx2 = idx4; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0088(:410):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 008C(:410):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb28, /* 0090(:411):        DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0094(:412):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 0098(:412):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb30, /* 009C(:413):        DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00A0(:414):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 00A4(:414):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb38, /* 00A8(:415):        DRD1A: *idx2 = idx7; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c728, /* 00AC(:416):        DRD1A: *idx1 = idx5; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 00B0(:0):      NOP */
    0xc1476018, /* 00B4(:420):    LCDEXT: idx1 = var2 + var14; ; idx1 += inc3 */
    0xc003241d, /* 00B8(:420):    LCDEXT: idx2 = var0, idx3 = var6; idx3 == var16; idx2 += inc3, idx3 += inc5 */
    0x811a601b, /* 00BC(:421):    LCD: idx4 = var2; ; idx4 += inc3 */
    0xda9b001b, /* 00C0(:424):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00C4(:424):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x0000d3a0, /* 00C8(:425):        DRD1A: *idx4; FN=0 init=0 WS=0 RS=0 */
    0xc1862102, /* 00CC(:427):      LCDEXT: idx5 = var3, idx6 = var12; idx6 < var4; idx5 += inc0, idx6 += inc2 */
    0x849be009, /* 00D0(:427):      LCD: idx7 = var9; ; idx7 += inc1 */
    0x0bfed7b8, /* 00D4(:430):        DRD1A: *idx7; FN=0 TFD init=31 WS=3 RS=3 */
    0xda9b001b, /* 00D8(:432):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00DC(:432):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x1000cb20, /* 00E0(:433):        DRD1A: *idx2 = idx4; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00E4(:434):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 00E8(:434):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb28, /* 00EC(:435):        DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00F0(:436):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 00F4(:436):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb30, /* 00F8(:437):        DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00FC(:438):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88f, /* 0100(:438):        DRD2B1: idx2 = EU3(); EU3(idx2,var15)  */
    0x1000cb38, /* 0104(:439):        DRD1A: *idx2 = idx7; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c728, /* 0108(:440):        DRD1A: *idx1 = idx5; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 010C(:0):      NOP */
    0x8118801b, /* 0110(:444):    LCD: idx1 = var2; idx1 once var0; idx1 += inc3 */
    0xd8c60018, /* 0114(:446):      LCDEXT: idx2 = idx1 + var12; idx2 once var0; idx2 += inc3 */
    0x98c6601c, /* 0118(:446):      LCD: idx3 = idx1 + var12 + 4; ; idx3 += inc3 */
    0x6000000b, /* 011C(:447):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=11 EXT init=0 WS=0 RS=0 */
    0x0c8cfc9f, /* 0120(:447):        DRD2B1: *idx2 = EU3(); EU3(*idx2)  */
    0x000001f8, /* 0124(:0):      NOP */
    0xa146001e, /* 0128(:450):    LCD: idx1 = *(var2 + var12 + 24); idx1 once var0; idx1 += inc3 */
    0x10000b08, /* 012C(:451):      DRD1A: var2 = idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x10002050, /* 0130(:452):      DRD1A: var8 = var10; FN=0 MORE init=0 WS=0 RS=0 */
    0xb8c60018, /* 0134(:453):      LCD: idx2 = *(idx1 + var12); idx2 once var0; idx2 += inc3 */
    0x10002b10, /* 0138(:454):        DRD1A: var10 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x7000000a, /* 013C(:455):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT MORE init=0 WS=0 RS=0 */
    0x080cf89f, /* 0140(:455):        DRD2B1: idx0 = EU3(); EU3(idx2)  */
    0x6000000d, /* 0144(:456):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT init=0 WS=0 RS=0 */
    0x018cf89f, /* 0148(:456):        DRD2B1: var6 = EU3(); EU3(idx2)  */
    0x000001f8, /* 014C(:0):      NOP */
    0x8618801b, /* 0150(:462):    LCD: idx1 = var12; idx1 once var0; idx1 += inc3 */
    0x7000000e, /* 0154(:463):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT MORE init=0 WS=0 RS=0 */
    0x084cf21f, /* 0158(:463):      DRD2B1: idx1 = EU3(); EU3(var8)  */
    0xd8990336, /* 015C(:464):      LCDEXT: idx2 = idx1; idx2 > var12; idx2 += inc6 */
    0x8019801b, /* 0160(:464):      LCD: idx3 = var0; idx3 once var0; idx3 += inc3 */
    0x040001f8, /* 0164(:465):        DRD1A: FN=0 INT init=0 WS=0 RS=0 */
    0x000001f8, /* 0168(:0):      NOP */
    0x000001f8, /* 016C(:0):    NOP */
};
u32 MCD_SingleNoEu_TDT[]=
{
    0x8198001b, /* 0000(:657):  LCD: idx0 = var3; idx0 once var0; idx0 += inc3 */
    0x7000000d, /* 0004(:658):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x080cf81f, /* 0008(:658):    DRD2B1: idx0 = EU3(); EU3(idx0)  */
    0x8198801b, /* 000C(:659):    LCD: idx1 = var3; idx1 once var0; idx1 += inc3 */
    0x6000000e, /* 0010(:660):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT init=0 WS=0 RS=0 */
    0x084cf85f, /* 0014(:660):      DRD2B1: idx1 = EU3(); EU3(idx1)  */
    0x000001f8, /* 0018(:0):    NOP */
    0x8298001b, /* 001C(:664):  LCD: idx0 = var5; idx0 once var0; idx0 += inc3 */
    0x7000000d, /* 0020(:665):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x010cf81f, /* 0024(:665):    DRD2B1: var4 = EU3(); EU3(idx0)  */
    0x6000000e, /* 0028(:666):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT init=0 WS=0 RS=0 */
    0x018cf81f, /* 002C(:666):    DRD2B1: var6 = EU3(); EU3(idx0)  */
    0xc202601b, /* 0030(:669):  LCDEXT: idx0 = var4, idx1 = var4; ; idx0 += inc3, idx1 += inc3 */
    0xc002221c, /* 0034(:669):  LCDEXT: idx2 = var0, idx3 = var4; idx3 == var8; idx2 += inc3, idx3 += inc4 */
    0x809a601b, /* 0038(:670):  LCD: idx4 = var1; ; idx4 += inc3 */
    0xc10420c2, /* 003C(:673):    LCDEXT: idx5 = var2, idx6 = var8; idx6 < var3; idx5 += inc0, idx6 += inc2 */
    0x839be009, /* 0040(:673):    LCD: idx7 = var7; ; idx7 += inc1 */
    0x03fed7b8, /* 0044(:676):      DRD1A: *idx7; FN=0 init=31 WS=3 RS=3 */
    0xda9b001b, /* 0048(:678):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 004C(:678):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000006, /* 0050(:680):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 0054(:680):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x1000cb28, /* 0058(:681):      DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 005C(:682):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 0060(:682):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x1000cb30, /* 0064(:683):      DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0068(:684):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 006C(:684):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x0000cb38, /* 0070(:685):      DRD1A: *idx2 = idx7; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 0074(:0):    NOP */
    0xc202601b, /* 0078(:689):  LCDEXT: idx0 = var4, idx1 = var4; ; idx0 += inc3, idx1 += inc3 */
    0xc002229c, /* 007C(:689):  LCDEXT: idx2 = var0, idx3 = var4; idx3 == var10; idx2 += inc3, idx3 += inc4 */
    0x809a601b, /* 0080(:690):  LCD: idx4 = var1; ; idx4 += inc3 */
    0xda9b001b, /* 0084(:693):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 0088(:693):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x0000d3a0, /* 008C(:694):      DRD1A: *idx4; FN=0 init=0 WS=0 RS=0 */
    0xc10420c2, /* 0090(:696):    LCDEXT: idx5 = var2, idx6 = var8; idx6 < var3; idx5 += inc0, idx6 += inc2 */
    0x839be009, /* 0094(:696):    LCD: idx7 = var7; ; idx7 += inc1 */
    0x0bfed7b8, /* 0098(:699):      DRD1A: *idx7; FN=0 TFD init=31 WS=3 RS=3 */
    0xda9b001b, /* 009C(:701):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00A0(:701):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000006, /* 00A4(:703):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 00A8(:703):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x1000cb28, /* 00AC(:704):      DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00B0(:705):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 00B4(:705):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x1000cb30, /* 00B8(:706):      DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00BC(:707):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf889, /* 00C0(:707):      DRD2B1: idx2 = EU3(); EU3(idx2,var9)  */
    0x0000cb38, /* 00C4(:708):      DRD1A: *idx2 = idx7; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 00C8(:0):    NOP */
    0xc318022d, /* 00CC(:712):  LCDEXT: idx0 = var6; idx0 > var8; idx0 += inc5 */
    0x8018801b, /* 00D0(:712):  LCD: idx1 = var0; idx1 once var0; idx1 += inc3 */
    0x040001f8, /* 00D4(:713):    DRD1A: FN=0 INT init=0 WS=0 RS=0 */
};
#ifdef MCD_INCLUDE_EU
u32 MCD_ChainEu_TDT[]=
{
    0x80004000, /* 0000(:947):  LCDEXT: idx0 = 0x00000000; ; */
    0x8198801b, /* 0004(:947):  LCD: idx1 = var3; idx1 once var0; idx1 += inc3 */
    0xb8c68018, /* 0008(:948):    LCD: idx2 = *(idx1 + var13); idx2 once var0; idx2 += inc3 */
    0x10002f10, /* 000C(:949):      DRD1A: var11 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x7000000d, /* 0010(:950):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x01ccf89f, /* 0014(:950):      DRD2B1: var7 = EU3(); EU3(idx2)  */
    0x6000000a, /* 0018(:951):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf89f, /* 001C(:951):      DRD2B1: idx0 = EU3(); EU3(idx2)  */
    0x000001f8, /* 0020(:0):    NOP */
    0x981803a4, /* 0024(:955):  LCD: idx0 = idx0; idx0 == var14; idx0 += inc4 */
    0x8198801b, /* 0028(:957):    LCD: idx1 = var3; idx1 once var0; idx1 += inc3 */
    0xf8c6801a, /* 002C(:958):      LCDEXT: idx2 = *(idx1 + var13 + 8); idx2 once var0; idx2 += inc3 */
    0xb8c6e01b, /* 0030(:959):      LCD: idx3 = *(idx1 + var13 + 12); ; idx3 += inc3 */
    0x10002b10, /* 0034(:961):        DRD1A: var10 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x00001318, /* 0038(:962):        DRD1A: var4 = idx3; FN=0 init=0 WS=0 RS=0 */
    0xb8c6801d, /* 003C(:964):      LCD: idx2 = *(idx1 + var13 + 20); idx2 once var0; idx2 += inc3 */
    0x10001710, /* 0040(:965):        DRD1A: var5 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x60000007, /* 0044(:966):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=7 EXT init=0 WS=0 RS=0 */
    0x018cf88c, /* 0048(:966):        DRD2B1: var6 = EU3(); EU3(idx2,var12)  */
    0x98c6801c, /* 004C(:968):      LCD: idx2 = idx1 + var13 + 4; idx2 once var0; idx2 += inc3 */
    0x00000b10, /* 0050(:969):        DRD1A: var2 = idx2; FN=0 init=0 WS=0 RS=0 */
    0x98c78018, /* 0054(:970):      LCD: idx2 = idx1 + var15; idx2 once var0; idx2 += inc3 */
    0x10002310, /* 0058(:971):        DRD1A: var8 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c820, /* 005C(:972):        DRD1A: *idx2 = var4; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 0060(:0):      NOP */
    0x8698801b, /* 0064(:976):    LCD: idx1 = var13; idx1 once var0; idx1 += inc3 */
    0x7000000f, /* 0068(:977):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=15 EXT MORE init=0 WS=0 RS=0 */
    0x084cf2df, /* 006C(:977):      DRD2B1: idx1 = EU3(); EU3(var11)  */
    0xd899042d, /* 0070(:978):      LCDEXT: idx2 = idx1; idx2 >= var16; idx2 += inc5 */
    0x8019801b, /* 0074(:978):      LCD: idx3 = var0; idx3 once var0; idx3 += inc3 */
    0x60000003, /* 0078(:979):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=3 EXT init=0 WS=0 RS=0 */
    0x2cd7c7df, /* 007C(:979):        DRD2B2: EU3(var13)  */
    0xd8990364, /* 0080(:980):      LCDEXT: idx2 = idx1; idx2 == var13; idx2 += inc4 */
    0x8019801b, /* 0084(:980):      LCD: idx3 = var0; idx3 once var0; idx3 += inc3 */
    0x60000003, /* 0088(:981):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=3 EXT init=0 WS=0 RS=0 */
    0x2c17c7df, /* 008C(:981):        DRD2B2: EU3(var1)  */
    0x000001f8, /* 0090(:0):      NOP */
    0xc1c7e018, /* 0094(:984):    LCDEXT: idx1 = var3 + var15; ; idx1 += inc3 */
    0xc003a35e, /* 0098(:984):    LCDEXT: idx2 = var0, idx3 = var7; idx3 == var13; idx2 += inc3, idx3 += inc6 */
    0x819a601b, /* 009C(:985):    LCD: idx4 = var3; ; idx4 += inc3 */
    0xc206a142, /* 00A0(:988):      LCDEXT: idx5 = var4, idx6 = var13; idx6 < var5; idx5 += inc0, idx6 += inc2 */
    0x851be009, /* 00A4(:988):      LCD: idx7 = var10; ; idx7 += inc1 */
    0x63fe0000, /* 00A8(:991):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=0 EXT init=31 WS=3 RS=3 */
    0x0d4cfddf, /* 00AC(:991):        DRD2B1: *idx5 = EU3(); EU3(*idx7)  */
    0xda9b001b, /* 00B0(:993):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00B4(:993):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000002, /* 00B8(:994):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=2 EXT MORE init=0 WS=0 RS=0 */
    0x004cf81f, /* 00BC(:994):        DRD2B1: var1 = EU3(); EU3(idx0)  */
    0x1000cb20, /* 00C0(:995):        DRD1A: *idx2 = idx4; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00C4(:996):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 00C8(:996):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb28, /* 00CC(:997):        DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00D0(:998):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 00D4(:998):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb30, /* 00D8(:999):        DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00DC(:1000):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 00E0(:1000):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb38, /* 00E4(:1001):        DRD1A: *idx2 = idx7; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c728, /* 00E8(:1002):        DRD1A: *idx1 = idx5; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 00EC(:0):      NOP */
    0xc1c7e018, /* 00F0(:1006):    LCDEXT: idx1 = var3 + var15; ; idx1 += inc3 */
    0xc003a49e, /* 00F4(:1006):    LCDEXT: idx2 = var0, idx3 = var7; idx3 == var18; idx2 += inc3, idx3 += inc6 */
    0x819a601b, /* 00F8(:1007):    LCD: idx4 = var3; ; idx4 += inc3 */
    0xda9b001b, /* 00FC(:1010):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 0100(:1010):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x0000d3a0, /* 0104(:1011):        DRD1A: *idx4; FN=0 init=0 WS=0 RS=0 */
    0xc206a142, /* 0108(:1013):      LCDEXT: idx5 = var4, idx6 = var13; idx6 < var5; idx5 += inc0, idx6 += inc2 */
    0x851be009, /* 010C(:1013):      LCD: idx7 = var10; ; idx7 += inc1 */
    0x6bfe0000, /* 0110(:1016):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=0 TFD EXT init=31 WS=3 RS=3 */
    0x0d4cfddf, /* 0114(:1016):        DRD2B1: *idx5 = EU3(); EU3(*idx7)  */
    0xda9b001b, /* 0118(:1018):      LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 011C(:1018):      LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000002, /* 0120(:1019):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=2 EXT MORE init=0 WS=0 RS=0 */
    0x004cf81f, /* 0124(:1019):        DRD2B1: var1 = EU3(); EU3(idx0)  */
    0x1000cb20, /* 0128(:1020):        DRD1A: *idx2 = idx4; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 012C(:1021):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 0130(:1021):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb28, /* 0134(:1022):        DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0138(:1023):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 013C(:1023):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb30, /* 0140(:1024):        DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0144(:1025):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf891, /* 0148(:1025):        DRD2B1: idx2 = EU3(); EU3(idx2,var17)  */
    0x1000cb38, /* 014C(:1026):        DRD1A: *idx2 = idx7; FN=0 MORE init=0 WS=0 RS=0 */
    0x0000c728, /* 0150(:1027):        DRD1A: *idx1 = idx5; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 0154(:0):      NOP */
    0x8198801b, /* 0158(:1031):    LCD: idx1 = var3; idx1 once var0; idx1 += inc3 */
    0xd8c68018, /* 015C(:1033):      LCDEXT: idx2 = idx1 + var13; idx2 once var0; idx2 += inc3 */
    0x98c6e01c, /* 0160(:1033):      LCD: idx3 = idx1 + var13 + 4; ; idx3 += inc3 */
    0x6000000b, /* 0164(:1034):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=11 EXT init=0 WS=0 RS=0 */
    0x0c8cfc9f, /* 0168(:1034):        DRD2B1: *idx2 = EU3(); EU3(*idx2)  */
    0x0000cc08, /* 016C(:1035):      DRD1A: *idx3 = var1; FN=0 init=0 WS=0 RS=0 */
    0xa1c6801e, /* 0170(:1038):    LCD: idx1 = *(var3 + var13 + 24); idx1 once var0; idx1 += inc3 */
    0x10000f08, /* 0174(:1039):      DRD1A: var3 = idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x10002458, /* 0178(:1040):      DRD1A: var9 = var11; FN=0 MORE init=0 WS=0 RS=0 */
    0xb8c68018, /* 017C(:1041):      LCD: idx2 = *(idx1 + var13); idx2 once var0; idx2 += inc3 */
    0x10002f10, /* 0180(:1042):        DRD1A: var11 = idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x7000000a, /* 0184(:1043):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT MORE init=0 WS=0 RS=0 */
    0x080cf89f, /* 0188(:1043):        DRD2B1: idx0 = EU3(); EU3(idx2)  */
    0x6000000d, /* 018C(:1044):        DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT init=0 WS=0 RS=0 */
    0x01ccf89f, /* 0190(:1044):        DRD2B1: var7 = EU3(); EU3(idx2)  */
    0x000001f8, /* 0194(:0):      NOP */
    0x8698801b, /* 0198(:1050):    LCD: idx1 = var13; idx1 once var0; idx1 += inc3 */
    0x7000000e, /* 019C(:1051):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT MORE init=0 WS=0 RS=0 */
    0x084cf25f, /* 01A0(:1051):      DRD2B1: idx1 = EU3(); EU3(var9)  */
    0xd899037f, /* 01A4(:1052):      LCDEXT: idx2 = idx1; idx2 > var13; idx2 += inc7 */
    0x8019801b, /* 01A8(:1052):      LCD: idx3 = var0; idx3 once var0; idx3 += inc3 */
    0x040001f8, /* 01AC(:1053):        DRD1A: FN=0 INT init=0 WS=0 RS=0 */
    0x000001f8, /* 01B0(:0):      NOP */
    0x000001f8, /* 01B4(:0):    NOP */
};
u32 MCD_SingleEu_TDT[]=
{
    0x8218001b, /* 0000(:1248):  LCD: idx0 = var4; idx0 once var0; idx0 += inc3 */
    0x7000000d, /* 0004(:1249):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x080cf81f, /* 0008(:1249):    DRD2B1: idx0 = EU3(); EU3(idx0)  */
    0x8218801b, /* 000C(:1250):    LCD: idx1 = var4; idx1 once var0; idx1 += inc3 */
    0x6000000e, /* 0010(:1251):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT init=0 WS=0 RS=0 */
    0x084cf85f, /* 0014(:1251):      DRD2B1: idx1 = EU3(); EU3(idx1)  */
    0x000001f8, /* 0018(:0):    NOP */
    0x8318001b, /* 001C(:1255):  LCD: idx0 = var6; idx0 once var0; idx0 += inc3 */
    0x7000000d, /* 0020(:1256):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x014cf81f, /* 0024(:1256):    DRD2B1: var5 = EU3(); EU3(idx0)  */
    0x6000000e, /* 0028(:1257):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT init=0 WS=0 RS=0 */
    0x01ccf81f, /* 002C(:1257):    DRD2B1: var7 = EU3(); EU3(idx0)  */
    0x8498001b, /* 0030(:1260):  LCD: idx0 = var9; idx0 once var0; idx0 += inc3 */
    0x7000000f, /* 0034(:1261):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=15 EXT MORE init=0 WS=0 RS=0 */
    0x080cf19f, /* 0038(:1261):    DRD2B1: idx0 = EU3(); EU3(var6)  */
    0xd81882a4, /* 003C(:1262):    LCDEXT: idx1 = idx0; idx1 >= var10; idx1 += inc4 */
    0x8019001b, /* 0040(:1262):    LCD: idx2 = var0; idx2 once var0; idx2 += inc3 */
    0x60000003, /* 0044(:1263):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=3 EXT init=0 WS=0 RS=0 */
    0x2c97c7df, /* 0048(:1263):      DRD2B2: EU3(var9)  */
    0xd818826d, /* 004C(:1264):    LCDEXT: idx1 = idx0; idx1 == var9; idx1 += inc5 */
    0x8019001b, /* 0050(:1264):    LCD: idx2 = var0; idx2 once var0; idx2 += inc3 */
    0x60000003, /* 0054(:1265):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=3 EXT init=0 WS=0 RS=0 */
    0x2c17c7df, /* 0058(:1265):      DRD2B2: EU3(var1)  */
    0x000001f8, /* 005C(:0):    NOP */
    0xc282e01b, /* 0060(:1268):  LCDEXT: idx0 = var5, idx1 = var5; ; idx0 += inc3, idx1 += inc3 */
    0xc002a25e, /* 0064(:1268):  LCDEXT: idx2 = var0, idx3 = var5; idx3 == var9; idx2 += inc3, idx3 += inc6 */
    0x811a601b, /* 0068(:1269):  LCD: idx4 = var2; ; idx4 += inc3 */
    0xc184a102, /* 006C(:1272):    LCDEXT: idx5 = var3, idx6 = var9; idx6 < var4; idx5 += inc0, idx6 += inc2 */
    0x841be009, /* 0070(:1272):    LCD: idx7 = var8; ; idx7 += inc1 */
    0x63fe0000, /* 0074(:1275):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=0 EXT init=31 WS=3 RS=3 */
    0x0d4cfddf, /* 0078(:1275):      DRD2B1: *idx5 = EU3(); EU3(*idx7)  */
    0xda9b001b, /* 007C(:1277):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 0080(:1277):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000002, /* 0084(:1279):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=2 EXT MORE init=0 WS=0 RS=0 */
    0x004cf99f, /* 0088(:1279):      DRD2B1: var1 = EU3(); EU3(idx6)  */
    0x70000006, /* 008C(:1280):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 0090(:1280):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x1000cb28, /* 0094(:1281):      DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0098(:1282):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 009C(:1282):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x1000cb30, /* 00A0(:1283):      DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00A4(:1284):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 00A8(:1284):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x0000cb38, /* 00AC(:1285):      DRD1A: *idx2 = idx7; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 00B0(:0):    NOP */
    0xc282e01b, /* 00B4(:1289):  LCDEXT: idx0 = var5, idx1 = var5; ; idx0 += inc3, idx1 += inc3 */
    0xc002a31e, /* 00B8(:1289):  LCDEXT: idx2 = var0, idx3 = var5; idx3 == var12; idx2 += inc3, idx3 += inc6 */
    0x811a601b, /* 00BC(:1290):  LCD: idx4 = var2; ; idx4 += inc3 */
    0xda9b001b, /* 00C0(:1293):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00C4(:1293):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x0000d3a0, /* 00C8(:1294):      DRD1A: *idx4; FN=0 init=0 WS=0 RS=0 */
    0xc184a102, /* 00CC(:1296):    LCDEXT: idx5 = var3, idx6 = var9; idx6 < var4; idx5 += inc0, idx6 += inc2 */
    0x841be009, /* 00D0(:1296):    LCD: idx7 = var8; ; idx7 += inc1 */
    0x6bfe0000, /* 00D4(:1299):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=0 TFD EXT init=31 WS=3 RS=3 */
    0x0d4cfddf, /* 00D8(:1299):      DRD2B1: *idx5 = EU3(); EU3(*idx7)  */
    0xda9b001b, /* 00DC(:1301):    LCDEXT: idx5 = idx5, idx6 = idx6; idx5 once var0; idx5 += inc3, idx6 += inc3 */
    0x9b9be01b, /* 00E0(:1301):    LCD: idx7 = idx7; ; idx7 += inc3 */
    0x70000002, /* 00E4(:1303):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=2 EXT MORE init=0 WS=0 RS=0 */
    0x004cf99f, /* 00E8(:1303):      DRD2B1: var1 = EU3(); EU3(idx6)  */
    0x70000006, /* 00EC(:1304):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 00F0(:1304):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x1000cb28, /* 00F4(:1305):      DRD1A: *idx2 = idx5; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 00F8(:1306):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 00FC(:1306):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x1000cb30, /* 0100(:1307):      DRD1A: *idx2 = idx6; FN=0 MORE init=0 WS=0 RS=0 */
    0x70000006, /* 0104(:1308):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=6 EXT MORE init=0 WS=0 RS=0 */
    0x088cf88b, /* 0108(:1308):      DRD2B1: idx2 = EU3(); EU3(idx2,var11)  */
    0x0000cb38, /* 010C(:1309):      DRD1A: *idx2 = idx7; FN=0 init=0 WS=0 RS=0 */
    0x000001f8, /* 0110(:0):    NOP */
    0x8144801c, /* 0114(:1312):  LCD: idx0 = var2 + var9 + 4; idx0 once var0; idx0 += inc3 */
    0x0000c008, /* 0118(:1313):    DRD1A: *idx0 = var1; FN=0 init=0 WS=0 RS=0 */
    0xc398027f, /* 011C(:1315):  LCDEXT: idx0 = var7; idx0 > var9; idx0 += inc7 */
    0x8018801b, /* 0120(:1315):  LCD: idx1 = var0; idx1 once var0; idx1 += inc3 */
    0x040001f8, /* 0124(:1316):    DRD1A: FN=0 INT init=0 WS=0 RS=0 */
};
#endif
u32 MCD_ENetRcv_TDT[]=
{
    0x80004000, /* 0000(:1389):  LCDEXT: idx0 = 0x00000000; ; */
    0x81988000, /* 0004(:1389):  LCD: idx1 = var3; idx1 once var0; idx1 += inc0 */
    0x10000788, /* 0008(:1390):    DRD1A: var1 = *idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x6000000a, /* 000C(:1391):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf05f, /* 0010(:1391):    DRD2B1: idx0 = EU3(); EU3(var1)  */
    0x98180209, /* 0014(:1394):  LCD: idx0 = idx0; idx0 != var8; idx0 += inc1 */
    0x81c40004, /* 0018(:1396):    LCD: idx1 = var3 + var8 + 4; idx1 once var0; idx1 += inc0 */
    0x7000000e, /* 001C(:1397):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT MORE init=0 WS=0 RS=0 */
    0x010cf05f, /* 0020(:1397):      DRD2B1: var4 = EU3(); EU3(var1)  */
    0x7000000c, /* 0024(:1398):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=12 EXT MORE init=0 WS=0 RS=0 */
    0x01ccf05f, /* 0028(:1398):      DRD2B1: var7 = EU3(); EU3(var1)  */
    0x70000004, /* 002C(:1399):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=4 EXT MORE init=0 WS=0 RS=0 */
    0x014cf049, /* 0030(:1399):      DRD2B1: var5 = EU3(); EU3(var1,var9)  */
    0x70000004, /* 0034(:1400):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=4 EXT MORE init=0 WS=0 RS=0 */
    0x004cf04a, /* 0038(:1400):      DRD2B1: var1 = EU3(); EU3(var1,var10)  */
    0x00000b88, /* 003C(:1403):      DRD1A: var2 = *idx1; FN=0 init=0 WS=0 RS=0 */
    0xc4030150, /* 0040(:1406):    LCDEXT: idx1 = var8, idx2 = var6; idx1 < var5; idx1 += inc2, idx2 += inc0 */
    0x8119e012, /* 0044(:1406):    LCD: idx3 = var2; ; idx3 += inc2 */
    0x03e0cf90, /* 0048(:1409):      DRD1A: *idx3 = *idx2; FN=0 init=31 WS=0 RS=0 */
    0x81188000, /* 004C(:1412):    LCD: idx1 = var2; idx1 once var0; idx1 += inc0 */
    0x000ac788, /* 0050(:1413):      DRD1A: *idx1 = *idx1; FN=0 init=0 WS=1 RS=1 */
    0xc4030000, /* 0054(:1415):    LCDEXT: idx1 = var8, idx2 = var6; idx1 once var0; idx1 += inc0, idx2 += inc0 */
    0x8199e000, /* 0058(:1415):    LCD: idx3 = var3; ; idx3 += inc0 */
    0x70000004, /* 005C(:1421):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=4 EXT MORE init=0 WS=0 RS=0 */
    0x084cfc8b, /* 0060(:1421):      DRD2B1: idx1 = EU3(); EU3(*idx2,var11)  */
    0x60000005, /* 0064(:1422):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=5 EXT init=0 WS=0 RS=0 */
    0x0cccf841, /* 0068(:1422):      DRD2B1: *idx3 = EU3(); EU3(idx1,var1)  */
    0x81c60000, /* 006C(:1428):    LCD: idx1 = var3 + var12; idx1 once var0; idx1 += inc0 */
    0xc399021b, /* 0070(:1430):      LCDEXT: idx2 = var7; idx2 > var8; idx2 += inc3 */
    0x80198000, /* 0074(:1430):      LCD: idx3 = var0; idx3 once var0; idx3 += inc0 */
    0x00008400, /* 0078(:1431):        DRD1A: idx1 = var0; FN=0 init=0 WS=0 RS=0 */
    0x00000f08, /* 007C(:1432):      DRD1A: var3 = idx1; FN=0 init=0 WS=0 RS=0 */
    0x81988000, /* 0080(:1435):    LCD: idx1 = var3; idx1 once var0; idx1 += inc0 */
    0x10000788, /* 0084(:1436):      DRD1A: var1 = *idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x6000000a, /* 0088(:1437):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf05f, /* 008C(:1437):      DRD2B1: idx0 = EU3(); EU3(var1)  */
    0xc2188209, /* 0090(:1440):    LCDEXT: idx1 = var4; idx1 != var8; idx1 += inc1 */
    0x80190000, /* 0094(:1440):    LCD: idx2 = var0; idx2 once var0; idx2 += inc0 */
    0x040001f8, /* 0098(:1441):      DRD1A: FN=0 INT init=0 WS=0 RS=0 */
    0x000001f8, /* 009C(:0):    NOP */
};
u32 MCD_ENetXmit_TDT[]=
{
    0x80004000, /* 0000(:1516):  LCDEXT: idx0 = 0x00000000; ; */
    0x81988000, /* 0004(:1516):  LCD: idx1 = var3; idx1 once var0; idx1 += inc0 */
    0x10000788, /* 0008(:1517):    DRD1A: var1 = *idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x6000000a, /* 000C(:1518):    DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf05f, /* 0010(:1518):    DRD2B1: idx0 = EU3(); EU3(var1)  */
    0x98180309, /* 0014(:1521):  LCD: idx0 = idx0; idx0 != var12; idx0 += inc1 */
    0x80004003, /* 0018(:1523):    LCDEXT: idx1 = 0x00000003; ; */
    0x81c60004, /* 001C(:1523):    LCD: idx2 = var3 + var12 + 4; idx2 once var0; idx2 += inc0 */
    0x7000000e, /* 0020(:1524):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=14 EXT MORE init=0 WS=0 RS=0 */
    0x014cf05f, /* 0024(:1524):      DRD2B1: var5 = EU3(); EU3(var1)  */
    0x7000000c, /* 0028(:1525):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=12 EXT MORE init=0 WS=0 RS=0 */
    0x028cf05f, /* 002C(:1525):      DRD2B1: var10 = EU3(); EU3(var1)  */
    0x7000000d, /* 0030(:1526):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=13 EXT MORE init=0 WS=0 RS=0 */
    0x018cf05f, /* 0034(:1526):      DRD2B1: var6 = EU3(); EU3(var1)  */
    0x70000004, /* 0038(:1527):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=4 EXT MORE init=0 WS=0 RS=0 */
    0x01ccf04d, /* 003C(:1527):      DRD2B1: var7 = EU3(); EU3(var1,var13)  */
    0x10000b90, /* 0040(:1528):      DRD1A: var2 = *idx2; FN=0 MORE init=0 WS=0 RS=0 */
    0x60000004, /* 0044(:1529):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=4 EXT init=0 WS=0 RS=0 */
    0x020cf0a1, /* 0048(:1529):      DRD2B1: var8 = EU3(); EU3(var2,idx1)  */
    0xc3188312, /* 004C(:1532):    LCDEXT: idx1 = var6; idx1 > var12; idx1 += inc2 */
    0x83c70000, /* 0050(:1532):    LCD: idx2 = var7 + var14; idx2 once var0; idx2 += inc0 */
    0x00001f10, /* 0054(:1533):      DRD1A: var7 = idx2; FN=0 init=0 WS=0 RS=0 */
    0xc583a3c3, /* 0058(:1535):    LCDEXT: idx1 = var11, idx2 = var7; idx2 >= var15; idx1 += inc0, idx2 += inc3 */
    0x81042325, /* 005C(:1535):    LCD: idx3 = var2, idx4 = var8; idx4 == var12; idx3 += inc4, idx4 += inc5 */
    0x03e0c798, /* 0060(:1540):      DRD1A: *idx1 = *idx3; FN=0 init=31 WS=0 RS=0 */
    0xd8990000, /* 0064(:1543):    LCDEXT: idx1 = idx1, idx2 = idx2; idx1 once var0; idx1 += inc0, idx2 += inc0 */
    0x9999e000, /* 0068(:1543):    LCD: idx3 = idx3; ; idx3 += inc0 */
    0x000acf98, /* 006C(:1544):      DRD1A: *idx3 = *idx3; FN=0 init=0 WS=1 RS=1 */
    0xd8992306, /* 0070(:1546):    LCDEXT: idx1 = idx1, idx2 = idx2; idx2 > var12; idx1 += inc0, idx2 += inc6 */
    0x9999e03f, /* 0074(:1546):    LCD: idx3 = idx3; ; idx3 += inc7 */
    0x03eac798, /* 0078(:1549):      DRD1A: *idx1 = *idx3; FN=0 init=31 WS=1 RS=1 */
    0xd8990000, /* 007C(:1552):    LCDEXT: idx1 = idx1, idx2 = idx2; idx1 once var0; idx1 += inc0, idx2 += inc0 */
    0x9999e000, /* 0080(:1552):    LCD: idx3 = idx3; ; idx3 += inc0 */
    0x000acf98, /* 0084(:1553):      DRD1A: *idx3 = *idx3; FN=0 init=0 WS=1 RS=1 */
    0xd8990000, /* 0088(:1555):    LCDEXT: idx1 = idx1, idx2 = idx2; idx1 once var0; idx1 += inc0, idx2 += inc0 */
    0x99832302, /* 008C(:1555):    LCD: idx3 = idx3, idx4 = var6; idx4 > var12; idx3 += inc0, idx4 += inc2 */
    0x0beac798, /* 0090(:1558):      DRD1A: *idx1 = *idx3; FN=0 TFD init=31 WS=1 RS=1 */
    0x81988000, /* 0094(:1560):    LCD: idx1 = var3; idx1 once var0; idx1 += inc0 */
    0x6000000b, /* 0098(:1561):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=11 EXT init=0 WS=0 RS=0 */
    0x0c4cfc5f, /* 009C(:1561):      DRD2B1: *idx1 = EU3(); EU3(*idx1)  */
    0x81c80000, /* 00A0(:1563):    LCD: idx1 = var3 + var16; idx1 once var0; idx1 += inc0 */
    0xc5190312, /* 00A4(:1565):      LCDEXT: idx2 = var10; idx2 > var12; idx2 += inc2 */
    0x80198000, /* 00A8(:1565):      LCD: idx3 = var0; idx3 once var0; idx3 += inc0 */
    0x00008400, /* 00AC(:1566):        DRD1A: idx1 = var0; FN=0 init=0 WS=0 RS=0 */
    0x00000f08, /* 00B0(:1567):      DRD1A: var3 = idx1; FN=0 init=0 WS=0 RS=0 */
    0x81988000, /* 00B4(:1570):    LCD: idx1 = var3; idx1 once var0; idx1 += inc0 */
    0x10000788, /* 00B8(:1571):      DRD1A: var1 = *idx1; FN=0 MORE init=0 WS=0 RS=0 */
    0x6000000a, /* 00BC(:1572):      DRD2A: EU0=0 EU1=0 EU2=0 EU3=10 EXT init=0 WS=0 RS=0 */
    0x080cf05f, /* 00C0(:1572):      DRD2B1: idx0 = EU3(); EU3(var1)  */
    0xc2988309, /* 00C4(:1575):    LCDEXT: idx1 = var5; idx1 != var12; idx1 += inc1 */
    0x80190000, /* 00C8(:1575):    LCD: idx2 = var0; idx2 once var0; idx2 += inc0 */
    0x040001f8, /* 00CC(:1576):      DRD1A: FN=0 INT init=0 WS=0 RS=0 */
    0x000001f8, /* 00D0(:0):    NOP */
};

#ifdef MCD_INCLUDE_EU
MCD_bufDesc MCD_singleBufDescs[NCHANNELS];
#endif
