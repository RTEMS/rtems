/* Structure map for NVRAM on PowerPC Reference Platform */
 
/* Revision 1 changes (8/25/94):
        - Power Management (RESTART_BLOCK struct)
        - Normal added to PM_MODE
        - OSIRQMask (HEADER struct) */
 
/* All fields are either character/byte strings which are valid either
endian or they are big-endian numbers.
 
There are a number of Date and Time fields which are in RTC format,
big-endian. These are stored in UT (GMT).
 
For enum's: if given in hex then they are bit significant, i.e. only
one bit is on for each enum.
*/
 
#ifndef _NVRAM_
#define _NVRAM_

#define VERSION  1
#define REVISION 0
 
#define OSAREASIZE 1024   /* size of OSArea space */
#define CONFSIZE 512      /* guess at size of Configuration space */
 
typedef struct _SECURITY {
  unsigned long BootErrCnt;         /* Count of boot password errors */
  unsigned long ConfigErrCnt;       /* Count of config password errors */
  unsigned long BootErrorDT[2];     /* Date&Time from RTC of last error in pw */
  unsigned long ConfigErrorDT[2];   /* Date&Time from RTC of last error in pw */
  unsigned long BootCorrectDT[2];   /* Date&Time from RTC of last correct pw */
  unsigned long ConfigCorrectDT[2]; /* Date&Time from RTC of last correct pw */
  unsigned long BootSetDT[2];       /* Date&Time from RTC of last set of pw */
  unsigned long ConfigSetDT[2];     /* Date&Time from RTC of last set of pw */
  unsigned char Serial[16];         /* Box serial number */
  } SECURITY;
 
typedef enum _OS_ID {
  Unknown = 0,
  Firmware = 1,
  AIX = 2,
  NT = 3,
  WPOS2 = 4,
  WPAIX = 5,
  Taligent = 6,
  Solaris = 7,
  Netware = 8,
  USL = 9,
  Low_End_Client = 10,
  SCO = 11
  } OS_ID;
 
typedef struct _ERROR_LOG {
  unsigned char ErrorLogEntry[40]; /* To be architected */
  } ERROR_LOG;
 
/*---Revision 1: Change the following struct:---*/
typedef struct _RESUME_BLOCK {
                                        /* Hibernation Resume Device will be an
                                           environment variable */
    unsigned long CheckSum;             /* Checksum of RESUME_BLOCK */
    volatile unsigned long BootStatus;
 
    void * ResumeAddr;                  /* For Suspend Resume */
    void * SaveAreaAddr;                /* For Suspend Resume */
    unsigned long SaveAreaLength;       /* For Suspend Resume */
 
    unsigned long HibResumeImageRBA;    /* RBA (512B blocks) of compressed OS
                                           memory image to be loaded by FW
                                           on Resume from hibernation */
    unsigned long HibResumeImageRBACount; /* Size of image in 512B blocks*/
    unsigned long Reserved;
  } RESUME_BLOCK;
 
typedef enum _OSAREA_USAGE {
  Empty = 0,
  Used = 1
  } OSAREA_USAGE;
 
typedef enum _PM_MODE {
  Suspend = 0x80,     /* Part of state is in memory */
  Hibernate = 0x40,   /* Nothing in memory - state saved elsewhere */
/* Revision 1: Normal added (actually was already here) */
  Normal = 0x00       /* No power management in effect */
  } PMMode;
 
typedef struct _HEADER {
  unsigned short Size;    /* NVRAM size in K(1024) */
  unsigned char Version;  /* Structure map different */
  unsigned char Revision; /* Structure map the same -
                             may be new values in old fields
                             in other words old code still works */
  unsigned short Crc1;    /* check sum from beginning of nvram to OSArea */
  unsigned short Crc2;    /* check sum of config */
  unsigned char LastOS;   /* OS_ID */
  unsigned char Endian;   /* B if big endian, L if little endian */
  unsigned char OSAreaUsage; /* OSAREA_USAGE */
  unsigned char PMMode;   /* Shutdown mode */
  RESUME_BLOCK ResumeBlock;
  SECURITY Security;
  ERROR_LOG ErrorLog[2];
 
/* Global Environment information */
  void * GEAddress;
  unsigned long GELength;
  /* Date&Time from RTC of last change to Global Environment */
  unsigned long GELastWriteDT[2];
 
/* Configuration information */
  void * ConfigAddress;
  unsigned long ConfigLength;
  /* Date&Time from RTC of last change to Configuration */
  unsigned long ConfigLastWriteDT[2];
  unsigned long ConfigCount; /* Count of entries in Configuration */
 
/* OS dependent temp area */
  void * OSAreaAddress;
  unsigned long OSAreaLength;
  /* Date&Time from RTC of last change to OSAreaArea */
  unsigned long OSAreaLastWriteDT[2];
 
/* Revision 1: add this mask - function tbd */
  /*unsigned short OSIRQMask;  OS to FW IRQ Mask - "I've used this one" */
  } HEADER, *PHEADER;
 
 #endif  /* ndef _NVRAM_ */
