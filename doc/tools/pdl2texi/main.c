/*
 *  main.c
 *  
 *  This program error checks the OAR PDL and converts it into 
 *  It works by reading the input file into a linked list of lines
 *  and then performing sweeps on that list until all formatting is
 *  complete.
 *
 *  FEATURES:
 *    + rudimentary statistics
 *    + forgiveness features 
 * 
 *  CHECKS PERFORMED:
 *    + unable to open file
 *    + unexpected end of file
 *    + line should have a colon
 *    + basic text (to right or below) associated with keyword improperly placed
 *    + an "incomplete marker" is still in place
 *    + missing keywords within a subsection
 *    + duplicated keywords withing a subsection
 *    + subsections in correct order
 *    + section header indicates no subsections and there are subsections
 *    + section header indicates subsections and there are no subsections
 *    + inconsistent spacing in RAW text.  This tends to be 1st line with
 *      text is indented further than a subsequent line.
 *    + missing components on line (incomplete)
 *    + invalid repitition of a subsection
 *    + type keyword validated for class, type, and spacing between
 *    + both members and range are present
 *    + neither members and range are present
 *    + enumerated types and attributes have members
 *    + non-enumerated types and attributes have ranges.
 *    + Object name and end object have the same name
 *    + booleans in attribute section list both true and false
 *    + task synchronization keyword is checked.  There must be a valid 
 *      type of synchronization primitive and a description when expected.
 *    + sections in correct order
 *
 *  INTERNAL ERRORS:
 *    + attempting to reformat an already formatted line
 *    + end of processing reached and no formatting assigned to line
 *
 *  CHECKS NOT PERFORMED:
 *
 *  TODO:
 *
 *  IDEAS NOT IMPLEMENTED:
 *    + smarter reporting of sections not in the right order
 *    + task which uses multiple synchronization types
 *    + improved error messages
 *    + no introductions inserted to sections
 *    + recognize special "symbols" like infinity, +-, etc.
 *
 *  QUESTIONS:
 *    + "what do I know" is actually a table and should not be reformatted.
 *
 *  COPYRIGHT (c) 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 *
 *  $Id$
 */

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* XXX -- just for testing -- these should be set by options */
char DocsNextNode[256]     = "";
char DocsPreviousNode[256] = "";
char DocsUpNode[256]       = "";

extern int   optind;     /* Why is this not in <stdlib.h>? */
extern char *optarg;     /* Why is this not in <stdlib.h>? */

#ifndef NAME_MAX
#define NAME_MAX      14 /* Why is the one in limits.h not showing up? */
#endif
#define INIT_DATA
#define EXTERN

#include "base.h"

FILE           *OutFile = stdout;

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
  "usage: cmd [-vti?] [-p previous_node] [-u up_node] files ...\n", 
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

#define TEXT         0
#define SECTION      1
#define SUBSECTION   2
#define HEADING      3
  
typedef enum {
  UNUSED,                            /* dummy 0 slot */
  OBJECT,                            /* sections */
  ATTRIBUTE_DESCRIPTIONS,
  ASSOCIATION_DESCRIPTIONS,
  ABSTRACT_TYPE_DESCRIPTIONS,
  DATA_ITEM_DESCRIPTIONS,
  METHOD_DESCRIPTIONS,
  TASK_DESCRIPTIONS,
  END_OBJECT,
 
  ATTRIBUTE,                         /* subsections */
  ASSOCIATION,
  ABSTRACT_TYPE,
  DATA_ITEM,
  METHOD,
  TASK,
 
  DESCRIPTION,                       /* headings */
  COPYRIGHT,
  PORTING,
  THEORY_OF_OPERATION,
  DERIVATION,
  DEPENDENCIES,
  NOTES,
  TYPE,
  RANGE,
  MEMBERS,
  UNITS,
  SCALE_FACTOR,
  DEFAULT,
  TOLERANCE,
  REQUIREMENTS,
  REFERENCES,
  VISIBILITY,
  ASSOCIATED_WITH,
  MULTIPLICITY,
  INPUTS,
  OUTPUTS,
  PDL,
  SYNCHRONIZATION,
  TIMING
}  Keyword_indices_t;

#define KEYWORD_FIRST OBJECT
#define KEYWORD_LAST  TIMING

/*
 *  Line Management Structure
 */

typedef enum {
  NO_EXTRA_FORMATTING_INFO,
  RAW_OUTPUT,
  PARAGRAPH_OUTPUT,
  BULLET_OUTPUT,
}  ExtraFormat_info_t;

typedef struct {
  Chain_Node         Node;
  Keyword_indices_t  keyword;   /* unused is unknown/undecided */
  ExtraFormat_info_t format;
  int                number;
  char               Contents[ PARAGRAPH_SIZE ];
} Line_Control;

typedef enum {
  RT_FORBIDDEN,     /* no text to right allowed */
  RT_OPTIONAL,      /* text to right optional -- none below */
  RT_NONE,          /* text to right is "none" or nothing -- none below */
  RT_REQUIRED,      /* text to right required -- none below */
  RT_BELOW,         /* text to right forbidden -- text below required */
  RT_NONE_OR_BELOW, /* text to right is "none" OR there is text below  */
  RT_MAYBE_BELOW,   /* text to right required -- text below optional */
  RT_EITHER,        /* text to right OR below */
  RT_BOTH           /* text to right AND below */
}  Keywords_text_mode_t;

typedef enum {
  BL_FORBIDDEN,     /* text below forbidden */
  BL_FORMATTED,     /* text below is to be formatted as paragraphs */
  BL_RAW,           /* text below is to be unprocessed by this program */
}  Keywords_text_below_t;

typedef int (*Keyword_validater_t)( Line_Control * );

typedef struct {
  char                      Name[ MAXIMUM_KEYWORD_LENGTH ];
  int                       level;
  Keywords_text_mode_t      text_mode;
  Keywords_text_below_t     text_below_mode;
  Keyword_validater_t       keyword_validation_routine;
}  Keyword_info_t;

/*
 *  Keyword Validation Routines
 */

int Validate_visibility(
  Line_Control   *line
);

int Validate_synchronization(
  Line_Control   *line
);

Keyword_info_t Keywords[] = {
  { "unused",
        0,          0,                0, NULL }, /* so 0 can be invalid */
  { "OBJECT:",
        SECTION,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "ATTRIBUTE DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "ASSOCIATION DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "ABSTRACT TYPE DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "DATA ITEM DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "METHOD DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "TASK DESCRIPTIONS:",
        SECTION,    RT_NONE,          BL_FORBIDDEN, NULL },
  { "ENDOBJECT:",
        SECTION,    RT_REQUIRED,      BL_FORBIDDEN, NULL },

  { "ATTRIBUTE:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "ASSOCIATION:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "ABSTRACT TYPE:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "DATA ITEM:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "METHOD:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "TASK:",
        SUBSECTION, RT_REQUIRED,      BL_FORBIDDEN, NULL },

  { "DESCRIPTION:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "COPYRIGHT:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "PORTING:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "THEORY OF OPERATION:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "DERIVATION:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "DEPENDENCIES:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "NOTES:",
        HEADING,    RT_BELOW,         BL_FORMATTED, NULL },
  { "TYPE:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "RANGE:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "MEMBERS:",
        HEADING,    RT_BELOW,         BL_RAW,       NULL },
  { "UNITS:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "SCALE FACTOR:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "DEFAULT:",
        HEADING,    RT_EITHER,        BL_RAW, NULL },
  { "TOLERANCE:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "REQUIREMENTS:",
        HEADING,    RT_BELOW,         BL_RAW,       NULL },
  { "REFERENCES:",
        HEADING,    RT_BELOW,         BL_RAW,       NULL },
  { "VISIBILITY:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, Validate_visibility },
  { "ASSOCIATED WITH:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "MULTIPLICITY:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL },
  { "INPUTS:",
        HEADING,    RT_NONE_OR_BELOW, BL_RAW,       NULL },
  { "OUTPUTS:",
        HEADING,    RT_NONE_OR_BELOW, BL_RAW,       NULL },
  { "PDL:",
        HEADING,    RT_BELOW,         BL_RAW,       NULL },
  { "SYNCHRONIZATION:",
        HEADING,    RT_MAYBE_BELOW,   BL_RAW,    Validate_synchronization },
  { "TIMING:",
        HEADING,    RT_REQUIRED,      BL_FORBIDDEN, NULL }
};

#define NUMBER_OF_KEYWORDS \
  ( sizeof( Keywords ) / sizeof( Keyword_info_t ) ) - 2

/*
 *  Section Descriptions
 */

#define MAXIMUM_ELEMENTS 16

typedef struct {
  Keyword_indices_t  keyword;
  boolean            is_required;
}  Element_info_t;

typedef struct Section_info_struct_t Section_info_t;

typedef (*Section_validater_t)(
  Section_info_t *,
  Line_Control *,  /* start */
  Line_Control *   /* next_section */
);

struct Section_info_struct_t {
  boolean              repeats;
  Keyword_indices_t    This_section;
  Keyword_indices_t    Next_section;
  Section_validater_t  section_validation_routine;
  Element_info_t       Description[MAXIMUM_ELEMENTS];
};

int Validate_object(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
);
  
int Validate_attribute(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
);
  
int Validate_abstract_type(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
);
  

Section_info_t ObjectSection = {
  FALSE,                        /* subsections repeat */
  OBJECT,                       /* this section */
  ATTRIBUTE_DESCRIPTIONS,       /* next section */
  Validate_object,              /* validation routine */
  {
    { OBJECT,              TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { COPYRIGHT,           FALSE },    /*  2 */
    { PORTING,             FALSE },    /*  3 */
    { THEORY_OF_OPERATION, TRUE },     /*  4 */
    { DERIVATION,          FALSE },    /*  5 */
    { DEPENDENCIES,        FALSE },    /*  6 */
    { REQUIREMENTS,        FALSE },    /*  7 */
    { REFERENCES,          FALSE },    /*  8 */
    { NOTES,               FALSE },    /*  9 */
    { UNUSED,              FALSE },    /* 10 */
    { UNUSED,              FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t AttributeSection = {
  TRUE,                         /* subsections repeat */
  ATTRIBUTE_DESCRIPTIONS,       /* this section */
  ASSOCIATION_DESCRIPTIONS,     /* next section */
  Validate_attribute,           /* validation routine */
  {
    { ATTRIBUTE,           TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { TYPE,                TRUE },     /*  2 */
    { MEMBERS,             FALSE },    /*  3 */
    { RANGE,               FALSE },    /*  4 */
    { UNITS,               FALSE },    /*  5 */
    { SCALE_FACTOR,        FALSE },    /*  6 */
    { DEFAULT,             FALSE },    /*  7 */
    { TOLERANCE,           FALSE },    /*  8 */
    { REQUIREMENTS,        FALSE },    /*  9 */
    { REFERENCES,          FALSE },    /* 10 */
    { NOTES,               FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t AssociationSection = {
  TRUE,                         /* subsections repeat */
  ASSOCIATION_DESCRIPTIONS,     /* this section */
  ABSTRACT_TYPE_DESCRIPTIONS,   /* next section */
  NULL,                         /* validation routine */
  {
    { ASSOCIATION,         TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { VISIBILITY,          TRUE },     /*  2 */
    { ASSOCIATED_WITH,     TRUE },     /*  3 */
    { MULTIPLICITY,        TRUE },     /*  4 */
    { REQUIREMENTS,        FALSE },    /*  5 */
    { REFERENCES,          FALSE },    /*  6 */
    { NOTES,               FALSE },    /*  7 */
    { UNUSED,              FALSE },    /*  8 */
    { UNUSED,              FALSE },    /*  9 */
    { UNUSED,              FALSE },    /* 10 */
    { UNUSED,              FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t AbstractTypeSection = {
  TRUE,                         /* subsections repeat */
  ABSTRACT_TYPE_DESCRIPTIONS,   /* this section */
  DATA_ITEM_DESCRIPTIONS,       /* next section */
  Validate_abstract_type,       /* validation routine */
  {
    { ABSTRACT_TYPE,       TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { VISIBILITY,          TRUE },     /*  2 */
    { DERIVATION,          TRUE },     /*  3 */
    { MEMBERS,             FALSE },    /*  4 */
    { RANGE,               FALSE },    /*  5 */
    { UNITS,               FALSE },    /*  6 */
    { SCALE_FACTOR,        FALSE },    /*  7 */
    { DEFAULT,             FALSE },    /*  8 */
    { TOLERANCE,           FALSE },    /*  9 */
    { REQUIREMENTS,        FALSE },    /* 10 */
    { REFERENCES,          FALSE },    /* 11 */
    { NOTES,               FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t DataItemSection = {
  TRUE,                         /* subsections repeat */
  DATA_ITEM_DESCRIPTIONS,       /* this section */
  METHOD_DESCRIPTIONS,          /* next section */
  NULL,                         /* validation routine */
  {
    { DATA_ITEM,           TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { TYPE,                TRUE },     /*  2 */
    { UNITS,               FALSE },    /*  3 */
    { SCALE_FACTOR,        FALSE },    /*  4 */
    { DEFAULT,             FALSE },    /*  5 */
    { TOLERANCE,           FALSE },    /*  6 */
    { NOTES,               FALSE },    /*  7 */
    { UNUSED,              FALSE },    /*  8 */
    { UNUSED,              FALSE },    /*  9 */
    { UNUSED,              FALSE },    /* 10 */
    { UNUSED,              FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t MethodSection = {
  TRUE,                         /* subsections repeat */
  METHOD_DESCRIPTIONS,          /* this section */
  TASK_DESCRIPTIONS,            /* next section */
  NULL,                         /* validation routine */
  {
    { METHOD,              TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { VISIBILITY,          TRUE },     /*  2 */
    { INPUTS,              TRUE },     /*  3 */
    { OUTPUTS,             TRUE },     /*  4 */
    { REQUIREMENTS,        FALSE },    /*  5 */
    { REFERENCES,          FALSE },    /*  6 */
    { NOTES,               FALSE },    /*  7 */
    { PDL,                 TRUE },     /*  8 */
    { UNUSED,              FALSE },    /*  9 */
    { UNUSED,              FALSE },    /* 10 */
    { UNUSED,              FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};

Section_info_t TaskSection = {
  TRUE,                         /* subsections repeat */
  TASK_DESCRIPTIONS,            /* this section */
  END_OBJECT,                   /* next section */
  NULL,                         /* validation routine */
  {
    { METHOD,              TRUE },     /*  0 */
    { DESCRIPTION,         TRUE },     /*  1 */
    { VISIBILITY,          TRUE },     /*  2 */
    { INPUTS,              TRUE },     /*  3 */
    { SYNCHRONIZATION,     TRUE },     /*  4 */
    { TIMING,              TRUE },     /*  5 */
    { REQUIREMENTS,        FALSE },    /*  6 */
    { REFERENCES,          FALSE },    /*  7 */
    { NOTES,               FALSE },    /*  8 */
    { PDL,                 TRUE },     /*  9 */
    { UNUSED,              FALSE },    /* 10 */
    { UNUSED,              FALSE },    /* 11 */
    { UNUSED,              FALSE },    /* 12 */
    { UNUSED,              FALSE },    /* 13 */
    { UNUSED,              FALSE },    /* 14 */
    { UNUSED,              FALSE },    /* 15 */
  }
};


Section_info_t *Sections[] = {
  &ObjectSection,
  &AttributeSection,
  &AssociationSection,
  &AbstractTypeSection,
  &DataItemSection,
  &MethodSection,
  &TaskSection
};

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
    "<%d,%d>:%s\n", 
    line->keyword,
    line->format,
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
 */

  memset( new_line->Contents, '\0', sizeof( new_line->Contents ) );
  new_line->number = -1;

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

  for ( p=line->Contents ; *p != ':' ; p++ ) 
    ;
  p++;  /* skip the ':' */
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

  for ( ; *p != ':' ; ) 
    *d++ = *p++;

  *d = '\0';
}

/*
 *  FormatParagraph
 */

Line_Control *FormatParagraph(
  Line_Control *starting
)
{
  Chain_Node   *insert_after;
  Line_Control *to_add;
  Line_Control *new_line;
  char          Paragraph[ PARAGRAPH_SIZE ];
  int           starting_line;
  char         *src;
  char         *dest;
  boolean       do_loop;
  boolean       is_bullet;
  int           length;

  length = 0;

  starting_line = starting->number;
  insert_after = starting->Node.previous;
  to_add = starting;

  dest = Paragraph;

  is_bullet = FALSE;

  for ( ; ; ) {
    src = to_add->Contents;

    for ( ; *src && isspace( *src ) ; src++ ); /* skip leading spaces */

    if ( *src == '+' ) {
      if ( is_bullet == TRUE )
        break;
      is_bullet = TRUE;
      for ( src++ ; isspace(*src) ; src++ ) ;
    }

    do_loop = TRUE;
    while ( *src && do_loop == TRUE ) {
                    
      if ( isspace( *src ) ) {         /* convert multiple spaces to 1 */

        if ( *(dest-1) == '.' ) {      /* two spaces after period */
          *dest++ = ' ';
          length++;
        }

        assert( length < PARAGRAPH_SIZE );

        *dest++ = ' ';
        length++;

        assert( length < PARAGRAPH_SIZE );
 
        for (  ; isspace( *src ) ; src++ )
          if ( !*src ) {
            do_loop = FALSE;
            break;
          }
 
        continue;
 
      } else {
        *dest++ = *src;
        length++;
        assert( length < PARAGRAPH_SIZE );
        src++;
      }
    }

    to_add = DeleteLine( to_add );

    if ( _Chain_Is_last( &to_add->Node ) )
      break;

    if ( !strlen( to_add->Contents ) )
      break;

    if ( to_add->keyword )
      break;

    if ( !isspace( *(dest-1) ) ) {
      if ( *(dest-1) == '.' ) {
        *dest++ = ' ';
        length++;
        assert( length < PARAGRAPH_SIZE );
      }
      *dest++ = ' ';
      length++;
      assert( length < PARAGRAPH_SIZE );
    }
  }

  new_line = AllocateLine();
  
  SetLineFormat( new_line, (is_bullet) ? BULLET_OUTPUT : PARAGRAPH_OUTPUT );

  *dest = '\0';

  strcpy( new_line->Contents, Paragraph );

  new_line->number = starting_line;
 
  _Chain_Insert( insert_after, &new_line->Node );

  return new_line;
}

/*
 *  FormatMultipleParagraphs
 */

Line_Control *FormatMultipleParagraphs(
  Line_Control *starting
)
{
  Line_Control *line = starting;

  if ( _Chain_Is_last( &line->Node ) ) {
    fprintf( stderr, "end of file reached when expecting text\n" ); 
    exit_application( 0 );
  }

  if ( line->keyword ) {
    line = (Line_Control *) line->Node.next;

    if ( _Chain_Is_last( &line->Node ) ) {
      fprintf( stderr, "end of file reached when expecting text\n" ); 
      exit_application( 0 );
    }
  }
    
  for ( ;; ) {
    assert( line );

    if ( _Chain_Is_last( &line->Node ) )
      break;

    if ( line->keyword )
      break;
    
    if ( !strlen( line->Contents ) ) {
      line = DeleteLine( line );
      continue;
    }

    line = FormatParagraph( line );
    line = (Line_Control *) line->Node.next;
  }
}

/*************************************************************************
 *************************************************************************
 *****              END OF LINE MANIPULATION ROUTINES                *****
 *************************************************************************
 *************************************************************************/

/*
 *  CountKeywordOccurrences
 */

int CountKeywordOccurrences(
  Keyword_indices_t  keyword
)
{
  int           count = 0;
  Line_Control *line;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next ) {
    if ( line->keyword == keyword )
      count += 1;
  }

  return count;
}

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

  Verbose = FALSE;
  Statistics = FALSE;
  OutputWord = FALSE;
  IncompletesAreErrors = TRUE;
  InsertTBDs = FALSE;

  while ((c = getopt(argc, argv, "istvw?n:p:u:")) != EOF) {
    switch (c) {
      case 'i':
        IncompletesAreErrors = FALSE;
        break;
      case 't':
        InsertTBDs = TRUE;
        break;
      case 'v':
        Verbose = TRUE;
        break;
      case 'n':
         strcpy( DocsNextNode, optarg );
         break;
      case 'p':
         strcpy( DocsPreviousNode, optarg );
         break;
      case 'u':
         strcpy( DocsUpNode, optarg );
         break;
      case 's':
         Statistics = TRUE;
         break;
      case 'w':
         OutputWord = TRUE;
         break;
      case '?':
        usage();
        return 0;
    }
  }

  if ( Verbose ) {
    fprintf( stderr, "Arguments successfully parsed\n" );
    fprintf( stderr, "Next Node = (%s)\n", DocsNextNode );
    fprintf( stderr, "Previous Node = (%s)\n", DocsPreviousNode );
    fprintf( stderr, "Up Node = (%s)\n", DocsUpNode );
    fprintf( stderr, "Output Format = (%s)\n", (OutputWord) ? "Word" : "Texi" );
  }

  FillLinePool();

  for ( index=optind ; index < argc ; index++ ) {
    ProcessFile( argv[ index ], NULL );
  }
    
  if ( Verbose )
    fprintf( stderr, "Exitting\n" );

  return 0;
}

/*
 *  ProcessFile
 */

void ProcessFile(
  char   *inname,
  char   *outname
)
{
   char            out[ 256 ];
   int             index;
   int             out_index;

   /*
    *  Automatically generate the output file name.
    */

   if ( outname == NULL ) {
     out_index = 0;
     for( index=0 ; inname[index] && inname[index] != '.' ; index++ ) {
/*
       if ( inname[ index ] != '_' )
*/
         out[ out_index++ ] = inname[ index ];
     }

     if ( OutputWord ) {
       out[ out_index++ ] = '.';
       out[ out_index++ ] = 't';
       out[ out_index++ ] = 'x';
       out[ out_index++ ] = 't';
     } else {
       out[ out_index++ ] = '.';
       out[ out_index++ ] = 't';
       out[ out_index++ ] = 'e';
       out[ out_index++ ] = 'x';
       out[ out_index++ ] = 'i';
     }
     out[ out_index ] = '\0';

   }

   /*
    *  Read the file into our internal data structure
    */

   if ( Verbose )
     printf( "Processing (%s) -> (%s)\n", inname, out );

   ReadFileIntoChain( inname );

   if ( Verbose )
     fprintf( stderr, "-------->FILE READ IN\n" );

   /*
    *  Remove any spaces before the keyword and mark each keyword line as
    *  such.  Also remove extra white space at the end of lines.
    */

   StripBlanks();

   if ( Verbose )
     fprintf( stderr, "-------->BLANKS BEFORE KEYWORDS STRIPPED\n" );

   /*
    *  Count the number of each type of thing
    */

   NumberOfAttributes    = CountKeywordOccurrences( ATTRIBUTE );
   NumberOfAssociations  = CountKeywordOccurrences( ASSOCIATION );
   NumberOfAbstractTypes = CountKeywordOccurrences( ABSTRACT_TYPE );
   NumberOfDataItems     = CountKeywordOccurrences( DATA_ITEM );
   NumberOfMethods       = CountKeywordOccurrences( METHOD );
   NumberOfTasks         = CountKeywordOccurrences( TASK );

   if ( Verbose || Statistics ) {
     fprintf( stderr, "NUMBER OF ATTRIBUTES = %d\n", NumberOfAttributes );
     fprintf( stderr, "NUMBER OF ASSOCIATIONS = %d\n", NumberOfAssociations );
     fprintf( stderr, "NUMBER OF ABSTRACT TYPES = %d\n", NumberOfAbstractTypes);
     fprintf( stderr, "NUMBER OF DATA ITEMS = %d\n", NumberOfDataItems );
     fprintf( stderr, "NUMBER OF METHODS = %d\n", NumberOfMethods );
     fprintf( stderr, "NUMBER OF TASKS = %d\n", NumberOfTasks );
   }

   /*
    *  1.  Merge paragraphs entered as multiple lines into a single node 
    *      on the chain.
    *  2.  Clean up "colon lines".
    */

   if ( MergeText() == -1 )
     exit_application( 1 );

   if ( Verbose )
     fprintf( stderr, "-------->PARAGRAPHS MERGED\n" );

   RemovePagebreaks();
   
   /*
    *  Remove extraneous white space
    */

   if ( Verbose )
     fprintf( stderr, "-------->PAGEBREAKS REMOVED\n" );

   /*
    *  At this point, the only unmarked lines should be empty or errors.
    *  This deletes empty unmarked lines and flags the others as errors.
    */

   if ( RemoveExtraBlankLines() == -1 )
     exit_application( 1 );
 
   if ( Verbose )
     fprintf( stderr, "-------->EXTRA BLANK LINES REMOVED\n" );

   /*
    *  Now report which lines do not appear to be complete
    */

   if ( CheckForIncomplete() == -1 )
     exit_application( 1 );

   if ( Verbose )
     fprintf( stderr, "-------->CHECKED FOR INCOMPLETE KEYWORDS\n" );

   /*
    *  Check that all the required sections are present.
    */

   if ( CheckOutline() == -1 )
     exit_application( 1 );
  
   if ( Verbose )
     fprintf( stderr, "-------->CHECKED FOR HIGH LEVEL SECTION PROBLEMS\n" );

   /*
    *  Check for problems in each section.
    */

   if ( CheckSections() == -1 )
     exit_application( 1 );

   if ( Verbose )
     fprintf( stderr, "-------->CHECKED FOR INTERNAL SECTION PROBLEMS\n" );

   /*
    *  Look for Internal Consistencies
    */

   if ( Verbose ) 
     fprintf( stderr, "-------->LOOKING FOR ERRORS\n" );

   LookForInternalInconsistencies();

   /*
    *  Formatting the file 
    */

   if ( OutputWord ) {
     FormatToWord();
     if ( Verbose ) 
       fprintf( stderr, "-------->FILE FORMATTED TO WORD\n" );
   } else {
     FormatToTexinfo();
     if ( Verbose ) 
       fprintf( stderr, "-------->FILE FORMATTED TO TEXINFO\n" );
   }

   /*
    *  Print the file 
    */

   PrintFile( out );

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
 *  LookForInternalInconsistencies
 *
 *  This routine looks for lines with no formatting information and
 *  lines with too much formatting information.
 */

void LookForInternalInconsistencies( void )
{
  Line_Control  *line;
  int            i;
  int            errors = 0;
  char          *src;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        line = (Line_Control *)line->Node.next
        ) {
    if ( !line->keyword && line->format == NO_EXTRA_FORMATTING_INFO ) {
      fprintf(
        stderr,
        "\nLine %d has no keyword or formatting:\n", 
        line->number
      );
      PrintSurroundingLines( line, 3,  3 );
      errors++;
    } 
  }
 
  if ( errors ) 
    exit_application( 1 );
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

void ReadFileIntoChain( 
  char *inname
)
{
   FILE *InFile;
   int   line_count;
   int   max_length;
   char *line;
   char  Buffer[ BUFFER_SIZE ];
   Line_Control *new_line;

   InFile = fopen( inname, "r" );

   if ( !InFile ) {
     fprintf( stderr, "Unable to open (%s)\n", inname );
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
  Keyword_info_t    *key;
 
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

    /*
     *  Strip white space from the front of each keyword line.
     */

    for ( index=KEYWORD_FIRST ; index <= KEYWORD_LAST ; index++ ) {
      key = &Keywords[ index ];

      switch ( key->level ) {
        case SECTION:    indentation = 0; break; 
        case SUBSECTION: indentation = 0; break; 
        case HEADING:    indentation = 2; break; 
        default:         assert( FALSE );
      }

      if ( !strncmp( &line->Contents[ indentation ], key->Name, 
                    strlen( key->Name ) ) ) {
        if ( indentation )
          strcpy( line->Contents,  &line->Contents[ indentation ] );
        line->keyword = index;
        break;
      }
    }
  }
}

/*
 *  CrunchRight
 */

boolean CrunchRight(
  Line_Control *line
)
{
  char  Buffer[ PARAGRAPH_SIZE ];
  char *src;
  char *dest;
  boolean is_text_to_right = FALSE;

  src  = line->Contents;
  dest = Buffer;

  while( *src != ':' ) {
    if ( !*src ) {
      fprintf(
        stderr,
        "Line %d should have had a colon\n",
        line->number
      );
      exit_application( 1 );
    }
    *dest++ = *src++;
  }
 
  *dest++ = *src++;    /* this is the ':' */
 
  if ( *src ) {
    *dest++ = ' ';
    while ( *src && isspace( *src ) )
      src++;
 
    if ( *src )
      is_text_to_right = TRUE;
 
    while ( *src )
      *dest++ = *src++;
  }
 
  *dest = '\0';

  strcpy( line->Contents, Buffer );

  return is_text_to_right;
}

/*
 *  CrunchBelow
 */

void CrunchBelow(
  Line_Control          *start_line,
  Keywords_text_below_t  text_below_mode
)
{
  Line_Control      *line;
  int                i;
  int                leading_white;
  int                length;
 
  switch ( text_below_mode ) {
    case BL_FORBIDDEN:
      assert( FALSE );
    case BL_FORMATTED:
      (void) FormatMultipleParagraphs( start_line );
      break;
    case BL_RAW:
      for ( line = (Line_Control *) start_line->Node.next;
            !_Chain_Is_last( &line->Node ) ;
          ) {
         if ( strlen( line->Contents ) )
           break;
         line = DeleteLine( line );
      }  

      for ( i=0 ; isspace( line->Contents[ i ] ) ; i++ )
        ;
      
      leading_white = i;

      for ( ;
            !_Chain_Is_last( &line->Node ) ;
            ) {
 
         assert( line );

         if ( line->keyword )
           break;

         assert( !_Chain_Is_last( &line->Node ) );

         length = strlen( line->Contents );

         if ( length ) {
           if ( length < leading_white ) {
             fprintf(
               stderr,
               "\nLine %d %s:\n",
               line->number,
               "has inconsistent spacing -- is too short -- see 1st line of text"
             );
             PrintSurroundingLines( line, 0,  0 );
             fprintf( stderr, "\n" );

           } 
           for ( i=0 ; i < leading_white ; i++ ) {
             if ( !isspace( line->Contents[ i ] ) ) {
               fprintf(
                 stderr,
                 "\nLine %d %s:\n",
                 line->number,
                 "has inconsistent spacing -- see 1st line of text"
               );
               PrintSurroundingLines( line, 0,  0 );
               fprintf( stderr, "\n" );
               break;
             }
           } 

           strcpy( line->Contents, &line->Contents[ leading_white ] );

           SetLineFormat( line, RAW_OUTPUT );
           line = (Line_Control *) line->Node.next;
         } else {
           SetLineFormat( line, RAW_OUTPUT );
           /* line = DeleteLine( line ); */
           line = (Line_Control *) line->Node.next;
         }
      }
      break;
  }
}

/*
 *  MergeText
 */
 
/* XXX expand this and address the error numbers */
char *Format_Errors[] = {
  /* unused */             "no formatting error",
  /* RT_FORBIDDEN     */   "no text allowed to right or below",
  /* RT_OPTIONAL      */   "text to right optional -- none below",
  /* RT_NONE          */   "text to right is \"none\" or nothing -- none below",
  /* RT_REQUIRED      */   "text to right required -- none below",
  /* RT_BELOW         */   "text to right forbidden -- text below required",
  /* RT_NONE_OR_BELOW */   "text to right is \"none\" OR there is text below",
  /* RT_MAYBE_BELOW   */   "text to right required -- text below optional",
  /* RT_EITHER        */   "text to right OR below",
  /* RT_BOTH          */   "text to right AND below"
};


int MergeText( void )
{
  Line_Control      *line;
  Line_Control      *next;
  Line_Control      *new_line;
  Keyword_info_t    *key;
  boolean            is_text_to_right;
  boolean            is_text_below;
  boolean            add_text_to_right;
  boolean            add_text_below;
  int                errors = 0;
  int                error_code = 0;
 
  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ;
        ) {
 
    if ( error_code ) {

      fprintf(
        stderr,
        "\nLine %d %s:\n",
        line->number,
        Format_Errors[ error_code ]
      );
      PrintSurroundingLines( line, 0,  3 );
      fprintf( stderr, "\n" );
      error_code = 0;
      line = (Line_Control *) line->Node.next;
    }

    key = &Keywords[ line->keyword ];

    if ( !line->keyword ) {
      line = (Line_Control *) line->Node.next;
      continue;
    }

    /*
     *  Figure out where the text is for this keyword.  It is a pretty
     *  ugly thing to look ahead for text below because of intermediate
     *  blank lines which we are not allowed to remove.
     */

    add_text_to_right = FALSE;
    add_text_below = FALSE;

    is_text_to_right = CrunchRight( line );

    is_text_below = FALSE;

    for ( next = (Line_Control *) line->Node.next ; 
          !_Chain_Is_last( &next->Node ) ;
          next = (Line_Control *) next->Node.next ) {

      if ( next->keyword )
        break;

      if ( strlen( next->Contents ) ) {
        is_text_below = TRUE;
        break;
      }
    }

    switch ( key->text_mode ) {
      case RT_FORBIDDEN: /* no text to right or below allowed */
        if ( is_text_to_right || is_text_below ) {
          error_code = 1;
          errors++;
          continue;
        }
        break;

      case RT_OPTIONAL:  /* text to right optional -- none below */
        if ( is_text_below ) {
          error_code = 2;
          errors++;
          continue;
        }
        break;

      case RT_NONE:      /* text to right is "none" or nothing -- none below */
        if ( (is_text_to_right && !strstr( line->Contents, "none" )) ||
             is_text_below ) {
          error_code = 3;
          errors++;
          continue;
        }
        break;

      case RT_REQUIRED:  /* text to right required -- none below */
        if ( is_text_below ) {
          error_code = 4;
          errors++;
          continue;
        }

        if ( !is_text_to_right ) {
          if ( !InsertTBDs ) {
            error_code = 4;
            errors++;
            continue;
          } else
            add_text_to_right = TRUE;
        }
        break;

      case RT_BELOW:     /* text to right forbidden -- text below required */
        if ( is_text_to_right ) {
          error_code = 5;
          errors++;
          continue;
        }

        if ( !is_text_below ) {
          if ( !InsertTBDs ) {
            error_code = 5;
            errors++;
            continue;
          } else
            add_text_below = TRUE;
        }
        break;

      case RT_NONE_OR_BELOW: /* text to right is "none" OR text below */

        if ( is_text_to_right ) {
          if ( strstr( line->Contents, "none" ) )
            break;
          error_code = 6;
          errors++;
          continue;
        }

        if ( !is_text_below ) {
          if ( !InsertTBDs ) {
            error_code = 6;
            errors++;
            continue;
          } else
            add_text_to_right = TRUE;
        }
        break;
                          /* text to right required -- text below optional */
      case RT_MAYBE_BELOW:
        if ( !is_text_to_right ) {
          error_code = 7;
          errors++;
          continue;
        }
        break;

      case RT_EITHER:    /* text to right OR below */
        if ( !(is_text_to_right || is_text_below) ) {
          if ( !InsertTBDs ) {
            error_code = 8;
            errors++;
            continue;
          } else
            add_text_to_right = TRUE;
        }
        break;

      case RT_BOTH:      /* text to right AND below */
        if ( !(is_text_to_right && is_text_below) ) {
          if ( !InsertTBDs ) {
            error_code = 9;
            errors++;
            continue;
          } else {
            add_text_to_right = TRUE;
            add_text_below = TRUE;
          }
        }
        break;
    }

    if ( add_text_to_right )
      strcat( line->Contents, " TBD" );

    if ( add_text_below ) {
      new_line = AllocateLine();
      strcpy( new_line->Contents, "TBD" );
      _Chain_Insert( &line->Node, &new_line->Node );
      is_text_below = TRUE;
    }

    if ( is_text_below )
      CrunchBelow( line, key->text_below_mode );

    line = (Line_Control *) line->Node.next;
  }
  return (errors) ? -1 : 0;
}

/*
 *  CheckForIncomplete
 */

char *IncompleteMarkers[] = {
  "??",
  "xxx",
  "XXX",
  "xyz",
  "XYZ"
};

int CheckForIncomplete()
{
  Line_Control *line;
  int           i;
  int           errors = 0;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next
        ) {
    for ( i=0 ; i < NUMBER_ELEMENTS( IncompleteMarkers ) ; i++ ) {
      if ( !strstr( line->Contents, IncompleteMarkers[ i ] ) ) 
        continue;

      fprintf(
        stderr,
        "\nLine %d %s:\n",
        line->number,
        "appears to be incomplete"
      );
      PrintSurroundingLines( line, 0,  0 );
      fprintf( stderr, "\n" );
      if ( IncompletesAreErrors )
        errors++;
      break;
    }
  }
  return ( errors ) ? -1 : 0;
}

/*
 *  CheckOutline
 *
 *  Insure all section headers are present.
 *  Check sections which say "none", really have subsections.
 *  Check sections which have subsections, don't say none.
 */

int CheckOutline( void )
{
  Line_Control      *line;
  int                count;
  int                errors = 0;
  void              *none_present;
  boolean            KeywordsPresent[ KEYWORD_LAST + 1 ];
  Keyword_indices_t  keyword;

  for ( keyword=0 ; keyword <= KEYWORD_LAST ; keyword++ )
    KeywordsPresent[ keyword ] = FALSE;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next
        ) {

    KeywordsPresent[ line->keyword ] = TRUE;

    switch ( line->keyword ) {
      case ATTRIBUTE_DESCRIPTIONS:
        count = NumberOfAttributes;
        break;
      case ASSOCIATION_DESCRIPTIONS:
        count = NumberOfAssociations;
        break;
      case ABSTRACT_TYPE_DESCRIPTIONS:
        count = NumberOfAbstractTypes;
        break;
      case DATA_ITEM_DESCRIPTIONS:
        count = NumberOfDataItems;
        break;
      case METHOD_DESCRIPTIONS:
        count = NumberOfMethods;
        break;
      case TASK_DESCRIPTIONS:
        count = NumberOfTasks;
        break;
      default:
        count = -1;
        break;
    }
    if ( count == -1 )
      continue;

    none_present = strstr( line->Contents, "none" );

    /* valid cases are (none_present && !count) AND (!none_present && count) */
    if ( none_present && count ) {
      fprintf(
        stderr,
        "\nLine %d : %s\n",
        line->number,
        "section header says \"none\" and there are subsections"
      );
      PrintSurroundingLines( line, 0,  0 );
      fprintf( stderr, "\n" );
      errors++;
    }
    if ( !none_present && !count ) {
      fprintf(
        stderr,
        "\nLine %d : %s\n",
        line->number,
        "section header does not say \"none\" and there are no subsections"
      );
      PrintSurroundingLines( line, 0,  0 );
      fprintf( stderr, "\n" );
      errors++;
    }
  }

  for ( keyword=0 ; keyword <= KEYWORD_LAST ; keyword++ ) {
    if ( Keywords[ keyword ].level != SECTION )
      continue;

    if ( !KeywordsPresent[ keyword ] ) {
      fprintf(
        stderr,
        "Section (%s) is missing\n",
        Keywords[ keyword ].Name
      );
      errors++;
    }
  }

  return (errors) ? -1 : 0;
}

/*
 *  ReportMissingKeywords
 */

int ReportMissingKeywords(
  Section_info_t  *section,
  Line_Control    *line,
  int              elements_present[ MAXIMUM_ELEMENTS ]
)
{
  int       i;
  int       errors = 0;
  int       last_found;

#ifdef SECTION_DEBUG
  PrintLine( line );
  for ( i=0 ; i < MAXIMUM_ELEMENTS ; i++ )
    fprintf( stderr, "%d ", elements_present[ i ] );
  fprintf( stderr, "\n" );
#endif
  
  /*
   *  Check for missing sections
   */

  for ( i=0 ; i < MAXIMUM_ELEMENTS ; i++ ) {
    if ( section->Description[ i ].keyword == UNUSED )
      break;

#ifdef SECTION_DEBUG
    fprintf(
      stderr, 
      "%d %d %s\n",
      section->Description[ i ].is_required,
      elements_present[ i ],
      Keywords[ section->Description[ i ].keyword ].Name
    );
#endif

    if ( section->Description[ i ].is_required && elements_present[i] == -1 ) {
      fprintf( 
        stderr,
        "Section starting at line %d is missing the %s keyword\n",
        line->number,
        Keywords[ section->Description[ i ].keyword ].Name
      );
      errors++;
    }
  }

  last_found = -1;

  for ( i=0 ; i < MAXIMUM_ELEMENTS ; i++ ) {
    if ( section->Description[ i ].keyword == UNUSED )
      break;

    if ( elements_present[i] == -1 )
      continue;

    if ( elements_present[i] > last_found ) {
      last_found = elements_present[i];
      continue;
    }

    fprintf(
      stderr,
      "Keywords in the section starting at line %d are in the wrong order\n",
      line->number
    );
    errors++;
    break;

  }

  return errors;
}

/*
 *  ScanForNextSection
 */

Line_Control *ScanForNextSection(
  Line_Control   *start_line,
  Section_info_t *section
)
{
  Line_Control   *line;
  Element_info_t *Elements;

  line = start_line;
  Elements = section->Description;

  for ( ;; ) {
    line = (Line_Control *) line->Node.next;

    if ( line->keyword == END_OBJECT ) 
      break;

    assert( line );
    assert( !_Chain_Is_last( &line->Node ) );

    if ( Keywords[ line->keyword ].level == SECTION && 
         section->Next_section != line->keyword ) {

        fprintf( 
          stderr,
          "Sections appear to be out of order at line %d\n",
          start_line->number
        );
        return NULL;
    }

    if ( section->Next_section == line->keyword )   /* next section */
      break;

    if ( Elements[ 0 ].keyword == line->keyword )   /* repeating subsection */
      break;
  }

  return line;
}

/*
 *  CheckSections
 */

int CheckSections( void )
{
  Line_Control   *line;
  Line_Control   *scan_line;
  Section_info_t *section;
  Keyword_info_t *key;
  Element_info_t *Elements;
  int             elements_present[ MAXIMUM_ELEMENTS ];
  boolean         stay_in_subsection;
  int             i;
  int             errors;
  int             subsection;
  int             element;
  int             keyword_count;
  Line_Control   *next_section;

  errors = 0;
  line = (Line_Control *) Lines.first;

  for ( i=0 ; i < NUMBER_ELEMENTS( Sections ) ; i++ ) {
    section = Sections[ i ];
    Elements = section->Description;
    subsection = 0;

    if ( strstr( line->Contents, "none" ) ) {
      next_section = ScanForNextSection( line, section );
      if ( !next_section ) {
        errors++;
        goto end_object;
      }
      line = next_section;
      if ( line->keyword == END_OBJECT )
        goto end_object;
      continue;
    }

    while ( Elements[ 0 ].keyword != line->keyword ) {
      if ( line->keyword == END_OBJECT )
        goto end_object;

      if( !line || _Chain_Is_last( &line->Node ) ) {
        fprintf( stderr, "out of sync -- aborting\n" );
        errors++;
        goto end_object;
      }
      line = (Line_Control *) line->Node.next;
    }

repeating_subsection:

#ifdef SECTION_DEBUG
    fprintf( stderr, "Checking Section %d Subsection %d\n", i, subsection );
#endif
    assert( line );
    assert( !_Chain_Is_last( &line->Node ) );
  
    /*
     *  Per Subsection Initialization
     */

    subsection++;

    keyword_count = 0;

    next_section = ScanForNextSection( line, section );
    if ( !next_section ) {
      errors++;
      goto end_object;
    }

    for ( element=0 ; element < MAXIMUM_ELEMENTS ; element++ )
      elements_present[ element ] = -1;

    /*  
     *  Do a subsection
     */

    /*  validate each keyword */

    for ( scan_line = line ; ; ) {

      if ( !scan_line->keyword ) {
        scan_line = (Line_Control *) scan_line->Node.next;
        continue;
      }

      if ( scan_line == next_section )
        break;

      if ( *Keywords[ scan_line->keyword ].keyword_validation_routine ) {
        if ( (*Keywords[ scan_line->keyword ].keyword_validation_routine )(
               scan_line
              ) ) errors++;
      }

      scan_line = (Line_Control *) scan_line->Node.next;
    }

    /* scan subsection for keywords */

    for ( scan_line = line ; ; ) {

      if ( !scan_line->keyword ) {
        scan_line = (Line_Control *) scan_line->Node.next;
        continue;
      }

      if ( scan_line == next_section )
        break;

      for ( element=0 ; element < MAXIMUM_ELEMENTS ; element++ ) {
        if ( scan_line->keyword == Elements[ element ].keyword ) {
          if ( elements_present[ element ] != -1 ) {
            fprintf( 
              stderr,
              "Section starting at line %d has the %s keyword more than once\n",
              line->number,
              Keywords[ Elements[ i ].keyword ].Name
            );
            errors++;
          }
          
#ifdef SECTION_DEBUG
          fprintf( stderr, "FOUND %s %d %d\n", 
            Keywords[ Elements[ element ].keyword ].Name, 
            element, keyword_count ); 
#endif
          elements_present[ element ] = keyword_count++;
          break;
        }
        if ( Elements[ element ].keyword == UNUSED )
          break;
      }

      scan_line = (Line_Control *) scan_line->Node.next;
    }
    errors += ReportMissingKeywords( section, line, elements_present );

    /*
     *  Validate the section as a whole
     */
     
    if ( section->section_validation_routine ) {
      if ( (*section->section_validation_routine)(section, line, next_section) )
        errors++;
    }

scan_for_next_section:
    /*
     *  Scan forward until next subsection or next section
     */

#ifdef SECTION_DEBUG
    fprintf( stderr, "End of Subsection\n" );
#endif
    line = next_section;
    if ( line->keyword == END_OBJECT ) {
      goto end_object;
    }

    if ( Elements[ 0 ].keyword == line->keyword ) { /* repeating subsection */
      if ( !section->repeats ) {
        fprintf(
          stderr,
          "Invalid repeating subsection starting at line %d\n",
          line->number
        );
        errors++;
        goto end_object;
      }
      goto repeating_subsection;
    }
 
  }

end_object:
  return (errors) ? -1 : 0;
}
/*
 *  RemovePagebreaks
 */

void RemovePagebreaks()
{
  Line_Control *line;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        ) {
    if ( !strcmp( line->Contents, PAGE_SEPARATOR ) ) 
        line = DeleteLine( line );
    else 
      line = (Line_Control *) line->Node.next;
  }
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
 *  RemoveExtraBlankLines
 *
 *  NOTE:  Be careful not remove to remove white space in raw text.
 */

int RemoveExtraBlankLines()
{
  Line_Control *line;
  Line_Control *previous;
  int errors;

  errors = 0;

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next
        ) {
    if ( line->keyword || line->format )
      continue;

    if ( !strlen( line->Contents ) ) {
      line = DeleteLine( line );
      line = (Line_Control *) line->Node.previous;
      continue;
    }

    fprintf(
      stderr,
      "\nLine %d is not associated with a keyword:\n", 
      line->number
    );
    PrintSurroundingLines( line, 0,  0 );
    fprintf( stderr, "\n" );
    errors++;
  }
  return ( errors ) ? -1 : 0;
}

/*
 *  strCopyToColon
 */

void strCopyToColon(
  char *src,
  char *dest
)
{
  char *s = src;
  char *d = dest;

  for ( ; *s && *s != ':' ; ) 
    *d++ = *s++;
  *d ='\0';
}

/*
 *  BuildTexinfoNodes
 */

void BuildTexinfoNodes( void ) 
{
  Line_Control *line;
  Line_Control *new_line;
  Line_Control *next_node;
  char          Buffer[ BUFFER_SIZE ];
  char          ObjectName[ BUFFER_SIZE ];
  char          NodeName[ BUFFER_SIZE ];
  char          NextNode[ BUFFER_SIZE ];
  char          NextNodeName[ BUFFER_SIZE ];
  char          PreviousNodeName[ BUFFER_SIZE ];
  char          UpNodeName[ BUFFER_SIZE ];
  char          SectionName[ BUFFER_SIZE ];
  char          MenuBuffer[ BUFFER_SIZE ];
  Line_Control *menu_insert_point;
  Line_Control *node_line;
  boolean       next_found;
  int           menu_items;

  strcpy( PreviousNodeName, DocsPreviousNode );

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next
      ) {
    menu_insert_point = (Line_Control *) line->Node.next;

    switch ( Keywords[ line->keyword ].level ) {
      case TEXT:
      case HEADING:
        break;
      case SECTION:
        if ( line->keyword == END_OBJECT )
          goto bottom; 

        if ( line->keyword == OBJECT ) {
          LineCopyFromRight( line, Buffer );
          sprintf( ObjectName, "%s Object", Buffer );
          strcpy( NodeName, ObjectName );
          strcpy( UpNodeName, DocsUpNode );
        } else {
          LineCopySectionName( line, Buffer );
          sprintf( NodeName, "%s %s", ObjectName, Buffer );
          strcpy( UpNodeName, ObjectName );
        }
        strtoInitialCaps( NULL, NodeName );
        strtoInitialCaps( NULL, UpNodeName );
        strcpy( SectionName, NodeName );

        /*
         *  Go ahead and put it on the chain in the right order (ahead of
         *  the menu) and we can fill it in later (after the menu is built).
         */

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@ifinfo" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        node_line = AllocateLine();
        _Chain_Insert( line->Node.previous, &node_line->Node );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@end ifinfo" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        menu_items = 0;
       
        if ( line->keyword == OBJECT ) {
          next_node = (Line_Control *) line->Node.next;
          next_found = FALSE;
          for ( ; ; ) {
            if ( next_node->keyword == END_OBJECT )
              break;
            if ( Keywords[ next_node->keyword ].level == SECTION ) {
              LineCopySectionName( next_node, Buffer );
              strtoInitialCaps( NULL, Buffer );     

              if ( !next_found ) {
                next_found = TRUE;
                sprintf( NextNodeName, "%s %s", ObjectName, Buffer );
              }
              if ( menu_items == 0 ) {
                new_line = AllocateLine();
                strcpy( new_line->Contents, "@ifinfo" );
                _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
 
                new_line = AllocateLine();
                strcpy( new_line->Contents, "@menu" );
                _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
              }

              menu_items++;

              new_line = AllocateLine();
              sprintf( new_line->Contents, "* %s %s::", ObjectName, Buffer );
              _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node);
            }
            next_node = (Line_Control *) next_node->Node.next;
          }
        } else {
          next_node = (Line_Control *) line->Node.next;
        
          next_found = FALSE;
          for ( ; ; ) {
            if ( Keywords[ next_node->keyword ].level == SECTION ) {
              if ( !next_found ) {
                if ( next_node->keyword == END_OBJECT ) {
                  strcpy( NextNodeName, DocsNextNode );
                } else {
                  LineCopySectionName( next_node, Buffer );
                  sprintf( NextNodeName, "%s %s", ObjectName, Buffer );
                  strtoInitialCaps( NULL, NextNodeName );
                }
                next_found = TRUE;
              }
              break;
            } else if ( Keywords[ next_node->keyword ].level == SUBSECTION ) {
              LineCopySectionName( next_node, Buffer );
              strtoInitialCaps( NULL, Buffer );
              sprintf( MenuBuffer, "%s %s - ", ObjectName, Buffer );
              LineCopyFromRight( next_node, Buffer );
              strcat( MenuBuffer, Buffer );

              if ( menu_items == 0 ) {
                new_line = AllocateLine();
                strcpy( new_line->Contents, "@ifinfo" );
                _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
 
                new_line = AllocateLine();
                strcpy( new_line->Contents, "@menu" );
                _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
              }

              menu_items++;

              new_line = AllocateLine();
              sprintf( new_line->Contents, "* %s::", MenuBuffer );
              _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );

              if ( !next_found ) {
                next_found = TRUE;
                strcpy( NextNodeName, MenuBuffer );
              }
            }
            next_node = (Line_Control *) next_node->Node.next;
          }
        }

        if ( menu_items ) {
          new_line = AllocateLine();
          strcpy( new_line->Contents, "@end menu" );
          _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
 
          new_line = AllocateLine();
          strcpy( new_line->Contents, "@end ifinfo" );
          _Chain_Insert( menu_insert_point->Node.previous, &new_line->Node );
        }

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
        sprintf(
          node_line->Contents,
          "@node %s, %s, %s, %s",
          NodeName,
          NextNodeName,
          PreviousNodeName,
          UpNodeName
        );

        strcpy( PreviousNodeName, NodeName );
        break;

      case SUBSECTION:
        strcpy( UpNodeName, SectionName );

        LineCopySectionName( line, Buffer );
        strtoInitialCaps( NULL, Buffer );
        sprintf( NodeName, "%s %s - ", ObjectName, Buffer );
        LineCopyFromRight( line, Buffer );
        strcat( NodeName, Buffer );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@ifinfo" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        next_node = (Line_Control *) line->Node.next;
        for ( ; ; ) {
          if ( Keywords[ next_node->keyword ].level == SECTION ) {
            if ( next_node->keyword == END_OBJECT ) {
              strcpy( NextNodeName, DocsNextNode );
            } else {
              LineCopySectionName( next_node, Buffer );
              sprintf( NextNodeName, "%s %s", ObjectName, Buffer );
              strtoInitialCaps( NULL, NextNodeName );
            }
            break;
          } else if ( Keywords[ next_node->keyword ].level == SUBSECTION ) {
            LineCopySectionName( next_node, Buffer );
            strtoInitialCaps( NULL, Buffer );
            sprintf( NextNodeName, "%s %s - ", ObjectName, Buffer );
            LineCopyFromRight( next_node, Buffer );
            strcat( NextNodeName, Buffer );
            break;
          }
          next_node = (Line_Control *) next_node->Node.next;
        }

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
        new_line = AllocateLine();
        sprintf(
          new_line->Contents,
          "@node %s, %s, %s, %s",
          NodeName,
          NextNodeName,
          PreviousNodeName,
          UpNodeName
        );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@end ifinfo" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        strcpy( PreviousNodeName, NodeName );
        break;
    }
  }
bottom:
}

/*
 *  FormatToTexinfo
 */

char *Texinfo_Headers[] = {
  "\\input texinfo   @c -*-texinfo-*-",
  "@c %**start of header",
  "@setfilename ",
  "@settitle ",
  "@paragraphindent 0",
  "@c %**end of header",
  "",
  "@c",
  "@c  COPYRIGHT (c) 1996.",
  "@c  On-Line Applications Research Corporation (OAR).",
  "@c  All rights reserved.",
  "@c",
  "@c  This file is automatically generated.  DO NOT EDIT!!",
  "@c",
  "",
  "@c  This prevents a black box from being printed on overfull lines.",
  "@c  The alternative is to rework a sentence to avoid this problem.",
  "@finalout",
  "",
  "@tex",
  "\\global\\parindent 0in",
  "\\global\\chapheadingskip = 15pt plus 4pt minus 2pt",
  "\\global\\secheadingskip = 12pt plus 4pt minus 2pt",
  "\\global\\subsecheadingskip = 9pt plus 4pt minus 2pt",
  "",
  "@ifclear smallbook",
  "\\global\\parskip 6pt plus 1pt",
  "@end ifclear",
  "@end tex",
  "@setchapternewpage odd",
  "@ifinfo",
  "@top ",
  "@node Top, (dir), (dir), (dir)",
  "@end ifinfo",
  "@c ",
  "@c This is the end of the header block",
  "@c "
};

void FormatToTexinfo( void )
{
  Line_Control *line;
  Line_Control *new_line;
  char          Buffer[ PARAGRAPH_SIZE ];
  int           i;
  char          ChapterTitle[ PARAGRAPH_SIZE ];
  char          InfoFile[ PARAGRAPH_SIZE ];
  char          *p;
  boolean        new_section; 
  boolean        in_bullets; 

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
      ) {

    switch ( line->keyword ) {
      case UNUSED:
        line = (Line_Control *) line->Node.next;
        break;

      case OBJECT:
        LineCopyFromRight( line, ChapterTitle );
        strcpy( InfoFile, ChapterTitle );

        for ( p=InfoFile ; *p ; *p++ )   /* turn this into a file name */
          if ( isspace( *p ) )
            *p = '_';             

        sprintf( Buffer, "@chapter %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case ATTRIBUTE_DESCRIPTIONS:
      case ASSOCIATION_DESCRIPTIONS:
      case ABSTRACT_TYPE_DESCRIPTIONS:
      case DATA_ITEM_DESCRIPTIONS:
      case METHOD_DESCRIPTIONS:
      case TASK_DESCRIPTIONS:
        sprintf( Buffer, "@section %s", line->Contents );
        strcpy( line->Contents, Buffer );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@page" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        line = (Line_Control *) line->Node.next;
        new_section = TRUE;
        break;

      case END_OBJECT:
        line->Contents[ 0 ] = '\0';
        goto bottom;

      case ATTRIBUTE:
      case ASSOCIATION:
      case ABSTRACT_TYPE:
      case DATA_ITEM:
        if ( !new_section ) {
          new_line = AllocateLine();
          strcpy( new_line->Contents, "@need 4000" );
          _Chain_Insert( line->Node.previous, &new_line->Node );
        }
        new_section = FALSE;
        sprintf( Buffer, "@subsection %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
         break;

      case METHOD:
      case TASK:
        if ( !new_section ) {
          new_line = AllocateLine();
          strcpy( new_line->Contents, "@page" );
          _Chain_Insert( line->Node.previous, &new_line->Node );
        }
        new_section = FALSE;
        sprintf( Buffer, "@subsection %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case DESCRIPTION:
      case COPYRIGHT:
      case PORTING:
      case THEORY_OF_OPERATION:
      case DEPENDENCIES:
      case NOTES:
        sprintf( Buffer, "@subheading %s\n", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;

        /* now take care of the paragraphs which are here */

        in_bullets = FALSE;
        do {
          line = (Line_Control *) line->Node.next;
          if ( line->format == BULLET_OUTPUT ) {
            if ( !in_bullets ) {
              in_bullets = TRUE;

              new_line = AllocateLine();
              _Chain_Insert( line->Node.previous, &new_line->Node );

              new_line = AllocateLine();
              strcpy( new_line->Contents, "@itemize @bullet" );
              _Chain_Insert( line->Node.previous, &new_line->Node );
            }
            sprintf( Buffer, "@item %s\n", line->Contents );
            strcpy( line->Contents, Buffer );
          } else if ( in_bullets ) {
            in_bullets = FALSE;

            new_line = AllocateLine();
            strcpy( new_line->Contents, "@end itemize" );
            _Chain_Insert( line->Node.previous, &new_line->Node );

            new_line = AllocateLine();
            _Chain_Insert( line->Node.previous, &new_line->Node );
          } else {
            new_line = AllocateLine();
            _Chain_Insert( line->Node.previous, &new_line->Node );
          }
        } while ( !line->keyword );

        break;

      case DERIVATION:
      case TYPE:
      case RANGE:
      case UNITS:
      case SCALE_FACTOR:
      case TOLERANCE:
      case VISIBILITY:
      case ASSOCIATED_WITH:
      case MULTIPLICITY:
      case TIMING:
        sprintf( Buffer, "@subheading %s\n", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case MEMBERS:
      case DEFAULT:
      case REQUIREMENTS:
      case REFERENCES:
      case INPUTS:
      case OUTPUTS:
      case PDL:
      case SYNCHRONIZATION:
        sprintf( Buffer, "@subheading %s\n", line->Contents );
        strcpy( line->Contents, Buffer );

        /* now take care of the raw text which is here */

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@example" );
        _Chain_Insert( &line->Node, &new_line->Node );

        do {
          line = (Line_Control *) line->Node.next;
          if ( !strlen( line->Contents ) ) {
            new_line = AllocateLine();
            strcpy( new_line->Contents, "@end example" );
            _Chain_Insert( line->Node.previous, &new_line->Node );

            new_line = AllocateLine();
            strcpy( new_line->Contents, "@example" );
            _Chain_Insert( &line->Node, &new_line->Node );

          } 
        } while ( !line->keyword );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@end example" );
        _Chain_Insert( line->Node.previous, &new_line->Node );
        /* at this point line points to the next keyword */
        break;
    }   
  }

bottom:
#if 0
  for ( i=NUMBER_ELEMENTS( Texinfo_Headers ) - 1 ; i >= 0 ; i-- ) {
    new_line = AllocateLine();
    strcpy( new_line->Contents, Texinfo_Headers[ i ] );
    if ( !strcmp( "@setfilename ", new_line->Contents ) )
      strcat( new_line->Contents, ChapterTitle );
    else if ( !strcmp( "@settitle ", new_line->Contents ) )
      strcat( new_line->Contents, InfoFile );
    else if ( !strcmp( "@top ", new_line->Contents ) )
      strcat( new_line->Contents, InfoFile );
    _Chain_Insert( _Chain_Head( &Lines ), &new_line->Node );
  }

  /*
   *  Remove the special end of object string.  No one wants to see 
   *  it in the printed output and the node is already marked "END_OBJECT".
   */

  ((Line_Control *)Lines.last)->Contents[ 0 ] = '\0';

  new_line = AllocateLine();
  strcpy( new_line->Contents, "@bye" );
  _Chain_Append( &Lines, &new_line->Node );

#endif
  if ( Verbose )
    fprintf( stderr, "-------->INSERTING TEXINFO MENUS\n" );

  BuildTexinfoNodes();
}

/*
 *  FormatToWord
 */

void FormatToWord( void )
{
  Line_Control *line;
  Line_Control *new_line;
  char          Buffer[ PARAGRAPH_SIZE ];
  int           i;
  char          ChapterTitle[ PARAGRAPH_SIZE ];
  char          InfoFile[ PARAGRAPH_SIZE ];
  char         *p;
  boolean       new_section; 
  boolean       in_bullets; 

  for ( line = (Line_Control *) Lines.first ;
        !_Chain_Is_last( &line->Node ) ; 
      ) {

    switch ( line->keyword ) {
      case UNUSED:
        line = (Line_Control *) line->Node.next;
        break;

      case OBJECT:
        LineCopyFromRight( line, ChapterTitle );
        strcpy( InfoFile, ChapterTitle );

        for ( p=InfoFile ; *p ; *p++ )   /* turn this into a file name */
          if ( isspace( *p ) )
            *p = '_';             

        sprintf( Buffer, "@Chapter = %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case ATTRIBUTE_DESCRIPTIONS:
      case ASSOCIATION_DESCRIPTIONS:
      case ABSTRACT_TYPE_DESCRIPTIONS:
      case DATA_ITEM_DESCRIPTIONS:
      case METHOD_DESCRIPTIONS:
      case TASK_DESCRIPTIONS:
        sprintf( Buffer, "@Section = %s", line->Contents );
        strcpy( line->Contents, Buffer );

        new_line = AllocateLine();
        strcpy( new_line->Contents, "@Page" );
        _Chain_Insert( line->Node.previous, &new_line->Node );

        line = (Line_Control *) line->Node.next;
        new_section = TRUE;
        break;

      case END_OBJECT:
        line->Contents[ 0 ] = '\0';
        goto bottom;

      case ATTRIBUTE:
      case ASSOCIATION:
      case ABSTRACT_TYPE:
      case DATA_ITEM:
        if ( !new_section ) {
          /*
           *  Do something with the style to keep subsection
           *  contents together
           */
          ;
        }
        new_section = FALSE;
        sprintf( Buffer, "@Subsection = %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case METHOD:
      case TASK:
        if ( !new_section ) {
          new_line = AllocateLine();
          strcpy( new_line->Contents, "@Page" );
          _Chain_Insert( line->Node.previous, &new_line->Node );
        }
        new_section = FALSE;
        sprintf( Buffer, "@Subsection = %s", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case DESCRIPTION:
      case COPYRIGHT:
      case PORTING:
      case THEORY_OF_OPERATION:
      case DEPENDENCIES:
      case NOTES:
        sprintf( Buffer, "@Subheading = %s\n", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;

        /* now take care of the paragraphs which are here */

        in_bullets = FALSE;
        do {
          line = (Line_Control *) line->Node.next;
          if ( line->format == BULLET_OUTPUT ) {
            if ( !in_bullets ) {
              in_bullets = TRUE;

              new_line = AllocateLine();
              _Chain_Insert( line->Node.previous, &new_line->Node );
            }
            sprintf( Buffer, "@Bullet = %s\n", line->Contents );
            strcpy( line->Contents, Buffer );
          } else if ( in_bullets ) {
            in_bullets = FALSE;

            new_line = AllocateLine();
            _Chain_Insert( line->Node.previous, &new_line->Node );
          } else {
            new_line = AllocateLine();
            _Chain_Insert( line->Node.previous, &new_line->Node );
          }
        } while ( !line->keyword );

        break;

      case DERIVATION:
      case TYPE:
      case RANGE:
      case UNITS:
      case SCALE_FACTOR:
      case TOLERANCE:
      case VISIBILITY:
      case ASSOCIATED_WITH:
      case MULTIPLICITY:
      case TIMING:
        sprintf( Buffer, "@Subheading = %s\n", line->Contents );
        strcpy( line->Contents, Buffer );
        line = (Line_Control *) line->Node.next;
        break;

      case MEMBERS:
      case DEFAULT:
      case REQUIREMENTS:
      case REFERENCES:
      case INPUTS:
      case OUTPUTS:
      case PDL:
      case SYNCHRONIZATION:
        sprintf( Buffer, "@Subheading = %s\n", line->Contents );
        strcpy( line->Contents, Buffer );

        /* now take care of the raw text which is here */

         line = (Line_Control *) line->Node.next;
        while ( !line->keyword ) {
          sprintf( Buffer, "@Example = %s\n", line->Contents );
          strcpy( line->Contents, Buffer );
          line = (Line_Control *) line->Node.next;
        }

        /* at this point line points to the next keyword */
        break;
    }   
  }

bottom:
}

/*
 *  PrintFile
 */

void PrintFile(
  char *out
)
{
  Line_Control *line;

  OutFile = fopen( out, "w+" );

  if ( !OutFile ) {
    fprintf( stderr, "Unable to open (%s) for output\n", out );
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
    fprintf( stderr, "%s\n", line->Contents );
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

  if ( !dest )
    return;
  strcpy( dest, src );
#if 0
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
#endif
}

/*
 *  Validate_visibility
 */
 
char *Valid_visibilities[] = {
  "public",
  "private"
};
 
int Validate_visibility(
  Line_Control   *line
)
{
  char   *s;
  char   *d;
  char    Buffer[ BUFFER_SIZE ];
  char    Visibility[ BUFFER_SIZE ];
  int     i;
  boolean found;
  int     errors = 0;
 
  LineCopyFromRight( line, Buffer );
 
  memset( Visibility, '\0', sizeof( Visibility ) );
  s = Buffer;
 
  for ( d=Visibility ; ; s++, d++ ) {
    *d = *s;
    if ( !*s || isspace(*s) )
      break;
  }
  *d = '\0';
 
  if ( isspace(*s) ) {
    fprintf(
       stderr,
       "Unexpected white space on line %d -- are there multiple words?\n",
       line->number
     );
     errors++;
  }
 
  /*
   *  Check out the type part of this keyword
   */
 
  for ( found=FALSE, i=0 ; i<NUMBER_ELEMENTS(Valid_visibilities) ; i++ ) {
    if ( !strcmp( Valid_visibilities[ i ], Visibility ) ) {
      found = TRUE;
      break;
    }
  }
 
  if ( !found ) {
    if ( !(InsertTBDs && !strcmp( Visibility, "TBD" )) ) {
      fprintf(
        stderr,
        "Invalid visibility type (%s) on line %d\n",
        Visibility,
        line->number
      );
      errors++;
    }
  }
 
  return (errors) ? -1 : 0;
}

/*
 *  Validate_synchronization
 */
 
char *Valid_synchronization[] = {
  "delay",
  "event",
  "mutex",
  "semaphore",
  "message",
  "signal",
  "period"
};
 
boolean Valid_synchronization_text_below[] = {
  FALSE,   /* delay */
  TRUE,    /* event */
  TRUE,    /* mutex */
  TRUE,    /* semaphore */
  TRUE,    /* message */
  TRUE,    /* signal */
  FALSE    /* period */
};

int Validate_synchronization(
  Line_Control   *line
)
{
  char   *s;
  char   *d;
  char    Buffer[ BUFFER_SIZE ];
  char    Synchronization[ BUFFER_SIZE ];
  int     i;
  boolean found;
  int     errors = 0;
 
  LineCopyFromRight( line, Buffer );
 
  memset( Synchronization, '\0', sizeof( Synchronization ) );
  s = Buffer;
 
  for ( d=Synchronization ; ; s++, d++ ) {
    *d = *s;
    if ( !*s || isspace(*s) )
      break;
  }
  *d = '\0';
 
  if ( isspace(*s) ) {
    fprintf(
       stderr,
       "Unexpected white space on line %d -- invalid use of multiple words\n",
       line->number
     );
     errors++;
  }
 
  /*
   *  Check out the type part of this keyword
   */
 
  for ( found=FALSE, i=0 ; i<NUMBER_ELEMENTS(Valid_synchronization) ; i++ ) {
    if ( !strcmp( Valid_synchronization[ i ], Synchronization ) ) {
      found = TRUE;
      break;
    }
  }
 
  if ( !found ) {
    fprintf(
       stderr,
       "Invalid synchronization type (%s) on line %d\n",
       Synchronization,
       line->number
     );
     errors++;
  }

  if ( line->keyword && !Valid_synchronization_text_below[ i ] ) {
    fprintf(
       stderr,
       "Expected text below synchronization type (%s) on line %d\n",
       Synchronization,
       line->number
     );
     errors++;
  }
 
  return (errors) ? -1 : 0;
}

/*
 *  Validate_abstract_type
 *
 * presence of range or members but not both
 */

int Validate_abstract_type(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
)
{
  boolean       range_found = FALSE;
  boolean       members_found = FALSE;
  boolean       enumerated_found = FALSE;
  boolean       true_found = FALSE;
  boolean       false_found = FALSE;
  Line_Control *line;
  int           errors = 0;

  for ( line = start; 
        line != next_section ; 
        line = (Line_Control *) line->Node.next ) {
    if ( line->keyword == RANGE ) 
      range_found = TRUE;
    else if ( line->keyword == MEMBERS ) {
      members_found = TRUE;
    } else if ( line->keyword == TYPE || line->keyword == DERIVATION ) {
      if ( strstr( line->Contents, "enumerated" ) || 
           strstr( line->Contents, "structure" ) )
        enumerated_found = TRUE;
    }
  }

  if ( !range_found && !members_found ) {
    fprintf(
       stderr,
       "Neither range nor members keyword present in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }

  if ( !InsertTBDs ) {
    if ( range_found && members_found ) {
      fprintf(
         stderr,
         "Both range and members keyword present in "
           "subsection starting at line %d\n",
         start->number
       );
       errors++;
    }
  }  

  if ( enumerated_found  && !members_found ) {
    fprintf(
       stderr,
       "Enumerated type without list of members in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }

  if ( !enumerated_found  && !range_found ) {
    fprintf(
       stderr,
       "Type does not have range specified in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }

  return (errors) ? -1 : 0;
}

/*
 *  Validate_attribute
 *
 * presence of range or members but not both
 */
 
int Validate_attribute(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
)
{
  boolean       range_found = FALSE;
  boolean       members_found = FALSE;
  boolean       enumerated_found = FALSE;
  boolean       boolean_found = FALSE;
  boolean       true_found = FALSE;
  boolean       false_found = FALSE;
  Line_Control *line;
  int           errors = 0;
 
  for ( line = start;
        line != next_section ;
        line = (Line_Control *) line->Node.next ) {
    if ( line->keyword == RANGE )
      range_found = TRUE;
    else if ( line->keyword == MEMBERS ) {
      members_found = TRUE;
      if ( boolean_found == TRUE ) {
        line = (Line_Control *) line->Node.next;
        while ( !_Chain_Is_last( &line->Node ) ) {
          if ( line->keyword )
            break;
          if ( strstr( line->Contents, "FALSE" ) )
            false_found = TRUE;
          else if ( strstr( line->Contents, "TRUE" ) )
            true_found = TRUE;
          line = (Line_Control *) line->Node.next;
        }
        line = (Line_Control *) line->Node.previous;
      }
    } else if ( line->keyword == TYPE || line->keyword == DERIVATION ) {
      if ( strstr( line->Contents, "enumerated" ) )
        enumerated_found = TRUE;
      else if ( strstr( line->Contents, "boolean" ) ) {
        enumerated_found = TRUE;
        boolean_found = TRUE;
      }
    }
  }
 
  if ( !range_found && !members_found ) {
    fprintf(
       stderr,
       "Neither range nor members keyword present in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }
 
  if ( !InsertTBDs ) {
    if ( range_found && members_found ) {
      fprintf(
         stderr,
         "Both range and members keyword present in "
           "subsection starting at line %d\n",
         start->number
       );
       errors++;
    }
  }
 
  if ( enumerated_found  && !members_found ) {
    fprintf(
       stderr,
       "Enumerated type without list of members in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }
 
  if ( !enumerated_found  && !range_found ) {
    fprintf(
       stderr,
       "Type does not have range specified in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }
 
  if ( boolean_found && !true_found ) {
    fprintf(
       stderr,
       "Boolean without a TRUE case specified in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }
 
  if ( boolean_found && !false_found ) {
    fprintf(
       stderr,
       "Boolean without a FALSE case specified in "
         "subsection starting at line %d\n",
       start->number
     );
     errors++;
  }
 
  return (errors) ? -1 : 0;
}

/*
 *  Validate_object
 *
 * presence of range or members but not both
 */
 
int Validate_object(
  Section_info_t *section,
  Line_Control   *start,
  Line_Control   *next_section
)
{
  char          ObjectName[ BUFFER_SIZE ];
  char          EndObjectName[ BUFFER_SIZE ];
  Line_Control *line;
  int           errors = 0;
 
  
  LineCopyFromRight( start, ObjectName );

  for ( line = start;
        !_Chain_Is_last( &line->Node ) ; 
        line = (Line_Control *) line->Node.next ) {
    if ( line->keyword == END_OBJECT ) {
      LineCopyFromRight( line, EndObjectName );
      break;
    }
  }
 
  if ( strcmp( ObjectName, EndObjectName ) ) {
    fprintf(
       stderr,
       "Object and End Object names do not match\n"
     );
     errors++;
  }

  return (errors) ? -1 : 0;
}

