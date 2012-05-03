#ifndef __DEBUG_DEFS_H__
#define __DEBUG_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef DEBUG

 #ifndef DEBUG_FLAGS
  #define DEBUG_FLAGS 0
 #endif

 #define DBG(fmt, args...)    do { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); } while(0)
 #define DBG2(fmt)            do { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); } while(0)
 #define DBGC(c,fmt, args...) do { if (DEBUG_FLAGS & c) { printk(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args);  }} while(0)

#else

 #define DBG(fmt, args...)
 #define DBG2(fmt, args...)
 #define DBGC(c, fmt, args...)

#endif

#ifdef DEBUGFUNCS
  #define FUNCDBG()         do { printk("%s\n\r",__FUNCTION__); } while(0)
#else
  #define FUNCDBG()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_DEFS_H__ */
