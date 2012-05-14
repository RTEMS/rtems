/*
 *  PSIM addresses and constants based upon the configuration
 *  of devices used in the script provided by RTEMS.
 */

#ifndef __PSIM_h
#define __PSIM_h

/*
 * RAM Information
 */

extern char RamBase[];
extern char RamSize[];

/*
 * RamBase/RamSize is defined by the linker script;
 * CPP symbols are AFAIK unused and deprecated.
 */
#define PSIM_RAM_SIZE ((unsigned long)RamSize)
#define PSIM_RAM_BASE ((void*)RamBase)

/*
 * NVRAM/RTC Structure and Information
 */

#define PSIM_RTC_FREEZE 0xc0
#define PSIM_RTC_UPDATE 0x00

typedef struct {
  volatile uint8_t Control; /* 0x04 */
  volatile uint8_t Second;  /* 0x05 */
  volatile uint8_t Minute;  /* 0x06 */
  volatile uint8_t Hour;    /* 0x07 */
  volatile uint8_t Day;     /* 0x08 */
  volatile uint8_t Date;    /* 0x09 */
  volatile uint8_t Month;   /* 0x0a */
  volatile uint8_t Year;    /* 0x0b */
} psim_rtc_t;

/*
 *  System V IPC Semaphore -- Read Only
 */

typedef struct {
  volatile uint32_t  obtain_value;  /* 0x00 */
  volatile uint32_t  lock;          /* 0x04 */
  volatile uint32_t  unlock;        /* 0x08 */
} psim_sysv_sem_t;

typedef struct {
  /* 0x0c000000 - 0x0c007FFF - AMD 29F040 */
  volatile uint8_t Flash[ 512 * 1024 ];

  /* 0x0c080000 - 0x0c0FFFFF - NVRAM/NVRAM */
  volatile uint8_t nvram[ 512 * 1024 ];

  /* 0x0c100000 - 0x0c100007 - NVRAM/RTC */
  psim_rtc_t RTC;

  /* 0x0c100008 - 0x0c10000F - NVRAM/RTC */
  uint8_t gap1[8];

  /* 0x0c100010 - 0x0c10001b - System V IPC Semaphore */
  psim_sysv_sem_t Semaphore;

  /* 0x0c10001c - 0x0c10001f - NVRAM/RTC */
  uint8_t gap2[4];

  /* 0x0c100020 - 0x0c10005F - Ethernet */
  volatile uint8_t Ethtap[ 64 ];

  /* 0x0c100060 - 0x0c10FFFF - NVRAM/RTC */
  uint8_t gap3[65440];

  /* 0x0c110000 - 0x0c12FFFF - System V IPC Shared Memory */
  uint8_t SharedMemory[ 128 * 1024 ];

  /* 0x0c130000 - 0x0c170000 - OpenPIC IRQ Controller */
  volatile uint8_t OpenPIC[ 256 * 1024 ];

} psim_registers_t;

/*
 *  Data Structure Overlay for Registers.  See linkcmds for placement.
 */
extern psim_registers_t PSIM;

#endif
/* end of include file */
