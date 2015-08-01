void
__sync_synchronize (void)
{
#ifdef ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
  asm volatile("dmb" : : : "memory");
#else
  asm volatile("" : : : "memory");
#endif
}