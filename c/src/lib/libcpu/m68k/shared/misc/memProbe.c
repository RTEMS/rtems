/*
 * Address Probing for M68k/ColdFire
 */

#include <bsp.h>
#include <string.h>

#if (M68K_COLDFIRE_ARCH == 1)
# define EXCEPTION_FRAME_PC_OFFSET "4"
#else
# define EXCEPTION_FRAME_PC_OFFSET "2"
#endif

typedef int (*MemProber)(void *from, void *to);
int memProbeByte(void *from, void *to);
int memProbeShort(void *from, void *to);
int memProbeLong(void *from, void *to);
int memProbeCatcher(void);

__asm__(
    ".text\n"
    "memProbeByte:        \n"
    "   move.l %sp@(4),%a0\n"
    "   move.b %a0@,%d0   \n"
    "   move.l %sp@(8),%a0\n"
    "   move.b %d0,%a0@   \n"
    "   bra.b 1f          \n"
    "memProbeShort:       \n"
    "   move.l %sp@(4),%a0\n"
    "   move.w %a0@,%d0   \n"
    "   move.l %sp@(8),%a0\n"
    "   move.w %d0,%a0@   \n"
    "   bra.b 1f          \n"
    "memProbeLong:        \n"
    "   move.l %sp@(4),%a0\n"
    "   move.l %a0@,%d0   \n"
    "   move.l %sp@(8),%a0\n"
    "   move.l %d0,%a0@   \n"
    "1: nop               \n"
    "   moveq.l #1,%d0    \n"
    "   rts               \n"
    "memProbeCatcher:     \n"
    "   move.l #1f,%d0    \n"
    "   move.l %d0,%sp@(" EXCEPTION_FRAME_PC_OFFSET ")\n"
    "   rte               \n"
    "1: clr.l  %d0        \n"
    "   rts               \n"
);

rtems_status_code
bspExtMemProbe(void *addr, int write, int size, void *pval)
{
    rtems_status_code rval=RTEMS_SUCCESSFUL;
    rtems_interrupt_level level;
    unsigned long buf;
    MemProber probe;
    void *saveVector;
    void **exceptionPointer;
    void *vbr;

    /*
     * Sanity check
     */
    switch (size) {
        case sizeof(char):  probe=memProbeByte; break;
        case sizeof(short): probe=memProbeShort; break;
        case sizeof(long):  probe=memProbeLong; break;
        default: return RTEMS_INVALID_SIZE;
    }

    /*
     * use a buffer to make sure we don't end up probing 'pval'.
     */
    if (write && pval)
        memcpy(&buf, pval, size);

    /*
     * Get location of access fault exception
     */
    m68k_get_vbr(vbr);
    exceptionPointer = (void **)((char *)vbr + (2 * 4));

    /*
     * Probe!
     */
    rtems_interrupt_disable(level);
    saveVector = *exceptionPointer;
    *exceptionPointer = memProbeCatcher;
    if (write) {
        if (probe(&buf, addr) == 0)
            rval = RTEMS_INVALID_ADDRESS;
    }
    else {
        if (probe(addr, &buf) == 0)
            rval = RTEMS_INVALID_ADDRESS;
    }
    *exceptionPointer = saveVector;
    rtems_interrupt_enable(level);

    if (!write && pval && (rval == RTEMS_SUCCESSFUL))
        memcpy(pval, &buf, size);
    return rval;
}
