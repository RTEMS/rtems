/*
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 */

#ifndef __PDL2AMI_h
#define __PDL2AMI_h

#include "system.h"
#include "chain.h"

/*
 *  Paragraph size should be kept down because it is allocated for each
 *  Line_Control.  If this number is large, the memory requirements for
 *  the program increase significantly.
 */

#define BUFFER_SIZE    (2 * 1024)
#define PARAGRAPH_SIZE (2 * 1024)

#define NUMBER_ELEMENTS( _x ) (sizeof(_x) / sizeof _x[0])

void exit_application(
  int status
);

void ProcessFile(
  char   *inname,
  char   *outname
);

void strtolower(
  char *dest,
  char *src
);

void strtoInitialCaps(
  char *dest,
  char *src
);

void StripBlanks( void );

void MergeParagraphs( void );

int CheckForIncomplete( void );

int CheckOutline( void );

int CheckSections( void );

void GenerateLists( void );

void GenerateAList(
  char          *section,
  Chain_Control *the_list
);

void LookForInternalInconsistencies( void );

int Match_Argument(
  char  **array,
  int     entries,
  char   *users
);

void usage( void  );

void ReadFileIntoChain(
  char *inname
);

int MergeText( void );

int CheckForBadWhiteSpace();

void RemoveCopyright();

void RemovePagebreaks();

int RemoveExtraBlankLines();

void FormatToTexinfo( void );

void PrintFile(
   char *out
);

void DumpList(
  Chain_Control  *the_list
);

void ReleaseFile();

#endif
