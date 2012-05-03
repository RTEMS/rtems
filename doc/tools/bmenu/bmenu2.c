/*
 *  main.c
 *
 *  This program takes a texinfo file without node and menu commands,
 *  build those commands and inserts them.
 *
 *  It works by reading the input file into a linked list of lines
 *  and then performing sweeps on that list until all formatting is
 *  complete.  After the program is run, there is still a little
 *  clean up to be performed by hand.  The following have to be fixed
 *  by hand:
 *    + previous of the first node
 *    + next of the last node
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 */

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* XXX -- just for testing -- these should be set by options */
char TopString[]   = "Top";
char EmptyString[] = "";

char *DocsNextNode;
char *DocsPreviousNode;
char *DocsUpNode;
int NodeNameIncludesChapter = 1;

extern int   optind;     /* Why is this not in <stdlib.h>? */
extern char *optarg;     /* Why is this not in <stdlib.h>? */

#ifndef NAME_MAX
#define NAME_MAX      14 /* Why is the one in limits.h not showing up? */
#endif
#define INIT_DATA

#include "base.h"

boolean          Verbose;                 /* status/debug msgs */
Chain_Control    Lines;

FILE           *OutFile;

static void ProcessFile2(
  FILE *infile,
  FILE *outfile );
static void PrintFile2(
  FILE *outfile );
static void ReadFileIntoChain2(
  FILE *InFile );

/*************************************************************************
 *************************************************************************
 *****                 DATA TYPES AND CONSTANT TABLES                *****
 *************************************************************************
 *************************************************************************/
/*
 *  Usage Information
 */

char *Usage_Strings[] = {
  "\n",
  "usage: cmd [-cv] [-p prev] [-n next] [-u up] \n",
  "\n",
  "EOF"
};

/*
 *  The page separator is not really a keyword and will be purged before
 *  it is seen elsewhere.
 */

#define PAGE_SEPARATOR                            "#PAGE"

/*
 *  Section Delimiter Keywords
 */

#define MAXIMUM_KEYWORD_LENGTH   32

/*
 *  Level indicates where in the format the delimiter is allowed to occur.
 *    1 indicates a major section divider (e.g. "ATTRIBUTE DESCRIPTIONS:").
 *    2 indicates a subsection (e.g. "ATTRIBUTE:").
 *    3 indicates a heading (e.g. "DESCRIPTION:").
 */

#define TEXT            0
#define SECTION         1
#define SUBSECTION      2
#define SUBSUBSECTION   3
#define HEADING         4

typedef enum {
  UNUSED,                            /* dummy 0 slot */
  KEYWORD_CHAPTER,
  KEYWORD_APPENDIX,
  KEYWORD_PREFACE,
  KEYWORD_CHAPHEADING,
  KEYWORD_SECTION,
  KEYWORD_SUBSECTION,
  KEYWORD_SUBSUBSECTION,
  KEYWORD_RAISE,
  KEYWORD_LOWER,
  KEYWORD_OTHER,
  KEYWORD_END

}  Keyword_indices_t;

#define KEYWORD_FIRST KEYOWRD_CHAPTER
#define KEYWORD_LAST  KEYWORD_END

/*
 *  Line Management Structure
 */

typedef enum {
  NO_EXTRA_FORMATTING_INFO,
  RAW_OUTPUT,
  PARAGRAPH_OUTPUT
}  ExtraFormat_info_t;

typedef struct {
  Chain_Node         Node;
  Keyword_indices_t  keyword;   /* unused is unknown/undecided */
  ExtraFormat_info_t format;
  int                number;
  int                level;
  char               Contents[ PARAGRAPH_SIZE ];
} Line_Control;

typedef enum {
  RT_FORBIDDEN,     /* no text to right allowed */
  RT_OPTIONAL,      /* text to right optional -- none below */
  RT_NONE,          /* text to right is "none" or nothing -- none below */
  RT_REQUIRED,      /* text to right required -- none below */
  RT_BELOW,         /* text to right forbidden -- text below required */
  RT_NONE_OR_BELOW, /* text to right is "none" OR there is text below  */
  RT_EITHER,        /* text to right OR below */
  RT_BOTH           /* text to right AND below */
}  Keywords_text_mode_t;

typedef enum {
  BL_FORBIDDEN,     /* text below forbidden */
  BL_FORMATTED,     /* text below is to be formatted as paragraphs */
  BL_RAW,           /* text below is to be unprocessed by this program */
}  Keywords_text_below_t;

typedef (*Keyword_validater_t)( Line_Control * );

typedef struct {
  char                      Name[ MAXIMUM_KEYWORD_LENGTH ];
  int                       level;
  Keywords_text_mode_t      text_mode;
  Keywords_text_below_t     text_below_mode;
  Keyword_validater_t       keyword_validation_routine;
}  Keyword_info_t;

Keyword_info_t Keywords[] = {
  { "unused",         0, 0, 0, NULL }, /* so 0 can be invalid */
  { "@chapter",       SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@appendix",      SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@preface",       SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@chapheading",   SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@section",       SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@subsection",    SUBSECTION, RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@subsubsection", SUBSUBSECTION, RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@raise",         SUBSECTION, RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "@lower",         SUBSECTION, RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "",               HEADING,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL },
  { "END OF FILE",    SECTION,    RT_FORBIDDEN,      BL_FORBIDDEN, NULL }
};

#define NUMBER_OF_KEYWORDS \
  ( sizeof( Keywords ) / sizeof( Keyword_info_t ) ) - 2

/*
 *  exit_application
 */

void exit_application(
  int status
)
{
  fprintf( stderr, "*** Error encountered ***\n" );
/*
  fprintf( stderr, "*** Error encountered on line %d ***\n", CurrentLine );
*/
  fclose( OutFile );
  exit( status );
}

/*************************************************************************
 *************************************************************************
 *****                LINE MANIPULATION ROUTINES                     *****
 *************************************************************************
 *************************************************************************/

/*
 * PrintLine
 */

void PrintLine(
  Line_Control *line
)
{
  assert( line );

  if ( line->number == -1 )
    fprintf( stderr, "     " );
  else
    fprintf( stderr, "%5d", line->number );

#if 0
  fprintf( stderr, "%s\n", line->Contents );
#else
  /*
   *  Include some debugging information
   */
  fprintf(
    stderr,
    "<%d,%d,%d>:%s\n",
    line->keyword,
    line->format,
    line->level,
    line->Contents
  );
#endif
}

Chain_Control Line_Pool;

/*
 *  FillLinePool
 */

void FillLinePool( void )
{
  void *pool;

#define LINE_POOL_FILL_COUNT 100

  pool = malloc( sizeof( Line_Control ) * LINE_POOL_FILL_COUNT );
  assert( pool );

  _Chain_Initialize(
    &Line_Pool,
    pool,
    LINE_POOL_FILL_COUNT,
    sizeof( Line_Control )
  );
}

/*
 * AllocateLine
 */

Line_Control *AllocateLine( void )
{
  Line_Control  *new_line;

  new_line = (Line_Control *) _Chain_Get( &Line_Pool );
  if ( !new_line ) {
    FillLinePool();
    new_line = (Line_Control *) _Chain_Get( &Line_Pool );
    assert( new_line );
  }

/*
 *  This is commented out because although it is helpful during debug,
 *  it consumes a significant percentage of the program's execution time.

  memset( new_line->Contents, '\0', sizeof( new_line->Contents ) );
*/
  new_line->number = -1;
  new_line->level = -1;

  new_line->keyword = UNUSED;
  new_line->format = NO_EXTRA_FORMATTING_INFO;

  new_line->Node.next     = NULL;
  new_line->Node.previous = NULL;

  return new_line;
}

/*
 * FreeLine
 */

void FreeLine(
  Line_Control *line
)
{
  fflush( stdout );
  _Chain_Append( &Line_Pool, &line->Node );
}

/*
 * DeleteLine
 */

Line_Control *DeleteLine(
  Line_Control *line
)
{
  Line_Control *next;

  next = (Line_Control *)line->Node.next;
  _Chain_Extract( &line->Node );
  FreeLine( line );
  return next;
}

/*
 *  PrintSurroundingLines
 */

void PrintSurroundingLines(
  Line_Control *line,
  int           backward,
  int           forward
)
{
  int           i;
  int           real_backward;
  Line_Control *local;

  for ( local=line, real_backward=0, i=1 ;
        i<=backward ;
        i++, real_backward++ ) {
    if ( &local->Node == Lines.first )
      break;
    local = (Line_Control *) local->Node.previous;
  }

  for ( i=1 ; i<=real_backward ; i++ ) {
    PrintLine( local );
    local = (Line_Control *) local->Node.next;
  }

  PrintLine( local );

  for ( i=1 ; i<=forward ; i++ ) {
    local = (Line_Control *) local->Node.next;
    if ( _Chain_Is_last( &local->Node ) )
      break;
    PrintLine( local );
  }

}

/*
 *  SetLineFormat
 */

void SetLineFormat(
  Line_Control       *line,
  ExtraFormat_info_t  format
)
{
  if ( line->format != NO_EXTRA_FORMATTING_INFO ) {
    fprintf( stderr, "Line %d is already formatted\n", line->number );
    PrintLine( line );
    assert( FALSE );
  }

  line->format = format;
}

/*
 *  LineCopyFromRight
 */

void LineCopyFromRight(
  Line_Control *line,
  char         *dest
)
{
  char *p;

  for ( p=line->Contents ; *p != ' ' ; p++ )
    ;
  p++;  /* skip the ' ' */
  for ( ; isspace( *p ) ; p++ )
    ;

  strcpy( dest, p );

}

/*
 *  LineCopySectionName
 */

void LineCopySectionName(
  Line_Control *line,
  char         *dest
)
{
  char *p;
  char *d;

  p = line->Contents;
  d = dest;

  if ( *p == '@' ) {                /* skip texinfo command */
    while ( !isspace( *p++ ) )
      ;
  }

  for ( ; *p ; )
    *d++ = *p++;

  *d = '\0';
}

/*************************************************************************
 *************************************************************************
 *****              END OF LINE MANIPULATION ROUTINES                *****
 *************************************************************************
 *************************************************************************/

/*
 *  main
 */

int main(
  int    argc,
  char **argv
)
{
  int      c;
  int      index;
  boolean  single_file_mode;

  OutFile = stdout;
  Verbose = FALSE;
  DocsNextNode     = EmptyString;
  DocsPreviousNode = TopString;
  DocsUpNode       = TopString;

  while ((c = getopt(argc, argv, "vcp:n:u:")) != EOF) {
    switch (c) {
      case 'v':
        Verbose = TRUE;
        break;
      case 'c':
        NodeNameIncludesChapter = 0;
        break;
      case 'p':
        DocsPreviousNode = strdup(optarg);
        break;
      case 'n':
        DocsNextNode = strdup(optarg);
        break;
      case 'u':
        DocsUpNode = strdup(optarg);
        break;

      case '?':
        usage();
        return 0;
    }
  }

  if ( optind != argc )
  {
     usage();
     return 0 ;
  }

  if ( Verbose )
    fprintf( stderr, "Arguments successfully parsed\n" );

  FillLinePool();

  ProcessFile2( stdin, stdout );

  if ( Verbose )
    fprintf( stderr, "Exitting\n" );

  return 0;
}

/*
 *  ProcessFile
 */

void ProcessFile2(
  FILE	*infile,
  FILE	*outfile
)
{
   int             index;

   /*
    *  Read the file into our internal data structure
    */

   if ( Verbose )
     printf( "Processing (%s) -> (%s)\n", "stdin", "stdout" );

   ReadFileIntoChain2( infile );

   if ( Verbose )
     fprintf( stderr, "-------->FILE READ IN\n" );

   /*
    *  Remove any spaces before the keyword and mark each keyword line as
    *  such.  Also remove extra white space at the end of lines.
    */

   StripBlanks();

   if ( Verbose )
     fprintf( stderr, "-------->BLANKS BEFORE KEYWORDS STRIPPED\n" );


   FormatToTexinfo();

   if ( Verbose )
     fprintf( stderr, "-------->FILE FORMATTED TO TEXINFO\n" );

   /*
    *  Print the file
    */

   PrintFile2( outfile );

   if ( Verbose )
     fprintf( stderr, "-------->FILE PRINTED\n" );

   /*
    *  Clean Up
    */

   ReleaseFile();

   if ( Verbose )
     fprintf( stderr, "-------->FILE RELEASED\n" );
}

/*
 *  usage
 */

void usage( void )
{
  int index;

  for ( index=0 ; strcmp( Usage_Strings[ index ], "EOF" ) ; index++ )
    fprintf( stderr, Usage_Strings[ index ] );
}

/*
 *  ReadFileIntoChain
 */

void ReadFileIntoChain2(
  FILE *InFile
)
{
   int   line_count;
   int   max_length;
   char *line;
   char  Buffer[ BUFFER_SIZE ];
   Line_Control *new_line;

   if ( !InFile ) {
     fprintf( stderr, "Unable to open (%s)\n", "stdin" );
     exit( 1 );
   }
   assert( InFile );

   max_length = 0;
   line_count = 0;

   _Chain_Initialize_empty( &Lines );

   for ( ;; ) {
      line = fgets( Buffer, BUFFER_SIZE, InFile );
      if ( !line )
        break;

      Buffer[ strlen( Buffer ) - 1 ] = '\0';

      new_line = AllocateLine();

      strcpy( new_line->Contents, Buffer );

      new_line->number = ++line_count;

      _Chain_Append( &Lines, &new_line->Node );
   }

   fclose( InFile );
}

/*
 *  StripBlanks
 */

void StripBlanks( void )
{
  Line_Control      *line;
  Keyword_indices_t  index;
  int                indentation;
  int                length;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        line = (Line_Control *) line->Node.next
        ) {

    /*
     *  Strip white space from the end of each line
     */

    length = strlen( line->Contents );

    while ( isspace( line->Contents[ --length ] ) )
      line->Contents[ length ] = '\0';

    if ( strstr( line->Contents, "@chapter" ) )
      line->keyword = KEYWORD_CHAPTER;
    else if ( strstr( line->Contents, "@appendix" ) )
      line->keyword = KEYWORD_APPENDIX;
    else if ( strstr( line->Contents, "@preface" ) )
      line->keyword = KEYWORD_PREFACE;
    else if ( strstr( line->Contents, "@chapheading" ) )
      line->keyword = KEYWORD_CHAPHEADING;
    else if ( strstr( line->Contents, "@section" ) )
      line->keyword = KEYWORD_SECTION;
    else if ( strstr( line->Contents, "@subsection" ) )
      line->keyword = KEYWORD_SUBSECTION;
    else if ( strstr( line->Contents, "@subsubsection" ) )
      line->keyword = KEYWORD_SUBSUBSECTION;
    else if ( strstr( line->Contents, "@raise" ) )
      line->keyword = KEYWORD_RAISE;
    else if ( strstr( line->Contents, "@lower" ) )
      line->keyword = KEYWORD_LOWER;
    else
      line->keyword = KEYWORD_OTHER;

  }
  line = AllocateLine();
  line->keyword = KEYWORD_END;
  _Chain_Append( &Lines, &line->Node );
}

/*
 *  strIsAllSpace
 */

boolean strIsAllSpace(
  char *s
)
{
  char *p;

  for ( p = s ; *p ; p++ )
    if ( !isspace( *p ) )
      return FALSE;

  return TRUE;
}

/*
 *  BuildTexinfoNodes
 */

void BuildTexinfoNodes( void )
{
  char               Buffer[ BUFFER_SIZE ];
  Line_Control      *line;
  Line_Control      *next_node;
  Line_Control      *up_node;
  Line_Control      *new_line;
  Line_Control      *menu_insert_point;
  Line_Control      *node_line;
  int                next_found;
  int                menu_items;
  Keyword_indices_t  index;
  char               ChapterName[ BUFFER_SIZE ];
  char               NodeName[ BUFFER_SIZE ];
  char               UpNodeName[ BUFFER_SIZE ];
  char               NextNodeName[ BUFFER_SIZE ];
  char               PreviousNodeName[ BUFFER_SIZE ];

  /*
   *  Set Initial Previous Node Name
   */

  strcpy( PreviousNodeName, DocsPreviousNode );

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        line = (Line_Control *) line->Node.next
        ) {

    if ( line->level == -1 )
      continue;

    LineCopyFromRight( line, NodeName );

    if ( line->keyword == KEYWORD_CHAPTER ||
         line->keyword == KEYWORD_APPENDIX ||
         line->keyword == KEYWORD_PREFACE ||
         line->keyword == KEYWORD_CHAPHEADING ) {

      strcpy( ChapterName, NodeName );

    } else if ( NodeNameIncludesChapter ) {

      sprintf( Buffer, "%s %s", ChapterName, NodeName );
      strcpy( NodeName, Buffer );
    }

    /*
     *  Set Default Next Node Name
     */

    next_found = FALSE;
    strcpy( NextNodeName, DocsNextNode );

    /*
     *  Go ahead and put it on the chain in the right order (ahead of
     *  the menu) and we can fill it in later (after the menu is built).
     */

    new_line = AllocateLine();
    strcpy( new_line->Contents, "" ); /*"@ifinfo" ); */
    _Chain_Insert( line->Node.previous, &new_line->Node );

    node_line = AllocateLine();
    _Chain_Insert( line->Node.previous, &node_line->Node );

    new_line = AllocateLine();
    strcpy( new_line->Contents, "" ); /* "@end ifinfo" ); */
    _Chain_Insert( line->Node.previous, &new_line->Node );

    next_node = (Line_Control *) line->Node.next;
    menu_insert_point = next_node;
    menu_items = 0;

    for ( ; ; ) {
      if ( next_node->keyword == KEYWORD_END )
        break;

      if ( next_node->level == -1 )
        goto continue_menu_loop;

      LineCopySectionName( next_node, Buffer );
      if ( !next_found ) {
        next_found = TRUE;
        if (NodeNameIncludesChapter)
          sprintf( NextNodeName, "%s %s", ChapterName, Buffer );
        else
          sprintf( NextNodeName, "%s", Buffer );
      }

      if ( next_node->level <= line->level )
        break;

      if ( next_node->level != (line->level + 1) )
        goto continue_menu_loop;

      if ( menu_items == 0 ) {
        new_line = AllocateLine();
        strcpy( new_line->Contents, "" ); /* "@ifinfo" ); */
        _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@menu" );
        _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
      }

      menu_items++;

      new_line = AllocateLine();
      if (NodeNameIncludesChapter)
        sprintf( new_line->Contents, "* %s %s::", ChapterName, Buffer );
      else
        sprintf( new_line->Contents, "* %s::", Buffer );
      _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );

continue_menu_loop:
      next_node = (Line_Control *) next_node->Node.next;
    }

    /*
     *  If menu items were generated, then insert the end of menu stuff.
     */

    if ( menu_items ) {
      new_line = AllocateLine();
      strcpy( new_line->Contents, "@end menu" );
      _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );

      new_line = AllocateLine();
      strcpy( new_line->Contents, "" ); /* "@end ifinfo" ); */
      _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
    }

    /*
     *  Find the UpNodeName
     */

/* DumpList( &Lines ); */

    if ( line->level == 0 ) {
      strcpy( UpNodeName, DocsUpNode );
    } else {
      for ( up_node = line;
            up_node && !_Chain_Is_first((Chain_Node *)up_node) ;
            up_node = (Line_Control *) up_node->Node.previous
            ) {

        if ( (up_node->level == -1) )
          continue;

        if ( up_node->level == (line->level - 1) ) {
          LineCopySectionName( up_node, Buffer );
          if (NodeNameIncludesChapter) {
            if (!strcmp(ChapterName, Buffer))
              sprintf( UpNodeName, "%s", Buffer );
            else
              sprintf( UpNodeName, "%s %s", ChapterName, Buffer );
          } else
            sprintf( UpNodeName, "%s", Buffer );
          break;
        }
      }
    }

    /*
     *  Update the node information
     */

#if 0
    fprintf(
      stderr,
      "@node %s, %s, %s, %s\n",
      NodeName,
      NextNodeName,
      PreviousNodeName,
      UpNodeName
    );
#endif

    /* node_line was previously inserted */
    if (!NodeNameIncludesChapter) {
      sprintf(
        node_line->Contents,
        "@node %s, %s, %s, %s",
        NodeName,
        NextNodeName,
        PreviousNodeName,
        UpNodeName
      );
    } else {
      sprintf(
        node_line->Contents,
        "@node %s, %s, %s, %s",
        NodeName,
        NextNodeName,
        PreviousNodeName,
        UpNodeName
      );
    }

    strcpy( PreviousNodeName, NodeName );

    /* PrintLine( line ); */
  }
}

/*
 *  FormatToTexinfo
 */

void FormatToTexinfo( void )
{
  Line_Control *line;
  int           baselevel = 0;
  int           currentlevel;

  if ( Verbose )
    fprintf( stderr, "-------->INSERTING TEXINFO MENUS\n" );

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        line = (Line_Control *) line->Node.next ) {

    switch (line->keyword) {
      case UNUSED:
      case KEYWORD_OTHER:
      case KEYWORD_END:
        line->level = -1;
        break;
      case KEYWORD_CHAPTER:
      case KEYWORD_APPENDIX:
      case KEYWORD_PREFACE:
      case KEYWORD_CHAPHEADING:
        currentlevel = 0;
        line->level = baselevel + currentlevel;
        break;
      case KEYWORD_SECTION:
        currentlevel = 1;
        line->level = baselevel + currentlevel;
        break;
      case KEYWORD_SUBSECTION:
        currentlevel = 2;
        line->level = baselevel + currentlevel;
        break;
      case KEYWORD_SUBSUBSECTION:
        currentlevel = 3;
        line->level = baselevel + currentlevel;
        break;
      case KEYWORD_RAISE:
        assert( baselevel );
        baselevel--;
        line->level = -1;
        break;
      case KEYWORD_LOWER:
        baselevel++;
        line->level = -1;
        break;
    }
  }

  BuildTexinfoNodes();
}

/*
 *  PrintFile
 */

void PrintFile2(
  FILE	*OutFile
)
{
  Line_Control *line;

  if ( !OutFile ) {
    fprintf( stderr, "Unable to open (%s) for output\n", "stdout" );
    exit_application( 1 );
  }
  assert( OutFile );

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        line = (Line_Control *) line->Node.next ) {
    fprintf( OutFile, "%s\n", line->Contents );
/*
    fprintf(
      OutFile,
      "(%d,%d)%s\n",
      line->keyword,
      line->format,
      line->Contents
    );
*/
  }
}

/*
 *  DumpList
 */

void DumpList(
  Chain_Control  *the_list
)
{
  Line_Control  *line;

  fprintf( stderr, "---> Dumping list (%p)\n", the_list );

  for ( line = (Line_Control *) the_list->first ;
      !_Chain_Is_last( &line->Node ) ;
      line = (Line_Control *) line->Node.next ) {
    /* if (line->level != -1) */
      PrintLine( line );
      /* fprintf( stderr, "%s\n", line->Contents ); */
  }
}

/*
 * ReleaseFile
 */

void ReleaseFile()
{
   Line_Control *line;
   Line_Control *next;

   for ( line = (Line_Control *) Lines.first ;
         !_Chain_Is_last( &line->Node ) ;
       ) {
     next = (Line_Control *) line->Node.next;
     line = next;
   }
}

/*
 *  strtoInitialCaps
 */

void strtoInitialCaps(
  char *dest,
  char *src
)
{
  char *source = src;
  char *destination = dest;

  source = src;
  destination = (dest) ? dest : src;

  while ( *source ) {
    while ( isspace( *source ) )
      *destination++ = *source++;

    if ( !*source )
      break;

    *destination++ = toupper( *source++ );

    for ( ; *source && !isspace( *source ) ; source++ )
      *destination++ = tolower( *source );

    if ( !*source )
      break;
  }

  *destination = '\0';
}
