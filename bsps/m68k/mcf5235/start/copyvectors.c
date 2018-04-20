/*
 * Move the copy out of the Init5235 file because gcc is broken.
 */

#include <stdint.h>

void CopyVectors(const uint32_t* old, uint32_t* new);

void CopyVectors(const uint32_t* old, uint32_t* new)
{
    int v = 0;
    while (v < 256)
    {
        *new = *old;
        ++v;
        ++new;
        ++old;
    }
}
