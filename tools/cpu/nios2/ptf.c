/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ptf.h"

#define PTFPARSER_MAXDEPTH 20
#define PTFPARSER_NAMEBUFSIZE 1024
#define PTFPARSER_VALUEBUFSIZE 4096

#define DEBUG_EXPECTATIONS 1
#define DEBUG_READVALUES 2
#define DEBUG_FINDER 4
#define DEBUG 0

struct ptf_parser_state
{
  struct ptf *tree;
  struct ptf *current_in[PTFPARSER_MAXDEPTH];
  struct ptf *current_item;

  long line;  /* starts with 1, increments whenever a LF (ASCII 10) passes */
  int section_level; /* starts at 0, incremented at {, decremented at } */
  char *filename;
  char name_buffer[PTFPARSER_NAMEBUFSIZE];
  int name_length;
  char value_buffer[PTFPARSER_VALUEBUFSIZE];
  int value_length;
  struct
  {
    unsigned error:1;
    unsigned escaped:1;
    unsigned single_quoted:1;
    unsigned double_quoted:1;
  } flag;

  enum
  {
    section_or_item_specification,
    more_section_type_or_item_name_chars,
    whitespace_after_section_specification,
    whitespace_after_section_or_item_name,
    whitespace_before_item_value,
    more_section_name_chars,
    more_item_value_chars,
  } expectation;
};

/***************************************************************************/
struct ptf *ptf_alloc_item(ptf_item_type t, char *name, char *value)
{
  struct ptf *new_item;
  new_item = (struct ptf *)malloc(sizeof(struct ptf));
  if(!new_item) return NULL;

  new_item->type = t;
  new_item->sub = NULL;
  new_item->next = NULL;
  new_item->name = NULL;
  new_item->value = NULL;

  if(name != NULL)
  {
    int n = strlen(name);
    if(n > 0)
    {
      new_item->name = (char *)malloc(n + 1);
      if(new_item->name == NULL)
      {
        free(new_item);
        return NULL;
      };
      strcpy(new_item->name, name);
    }
  };

  if(value != NULL)
  {
    int n = strlen(value);
    if(n > 0)
    {
      new_item->value = (char *)malloc(n + 1);
      if(new_item->value == NULL)
      {
        if(name != NULL) free(new_item->name);
        free(new_item);
        return NULL;
      };
      strcpy(new_item->value, value);
    };
  };

  return new_item;
}

/***************************************************************************/
void add_ptf_item(struct ptf_parser_state *state, struct ptf *item)
{
   if(state->current_item == NULL)
   {
     if(state->section_level > 0)
       state->current_in[state->section_level-1]->sub = item;
     else
       state->tree = item;
   }
   else
     state->current_item->next = item;
}


/***************************************************************************/
void parse_error(struct ptf_parser_state *state, char *errmsg)
{
  fprintf(stderr, "Error while parsing %s (line %lu): %s\n",
    state->filename, state->line, errmsg);

  state->flag.error = 1;
}

/***************************************************************************/
void init_parser(struct ptf_parser_state *state, char *filename)
{
  int i;

  state->line = 1;
  state->flag.error = 0;
  state->flag.escaped = 0;
  state->flag.double_quoted = 0;
  state->flag.single_quoted = 0;
  state->section_level = 0;

  state->filename = (char *)malloc(strlen(filename)+1);
  if(state->filename != NULL) strcpy(state->filename, filename);

  state->expectation = section_or_item_specification;

  state->tree = NULL;
  state->current_item = NULL;
  for(i=1; i<PTFPARSER_MAXDEPTH; i++) state->current_in[i] = NULL;
}

/***************************************************************************/
void ptf_free(struct ptf *ptf)
{
  struct ptf *this, *next;
  for(this = ptf; this != NULL; this = next)
  {
    next = this->next;
    if(this->value != NULL) free(this->value);
    if(this->name != NULL) free(this->name);
    if(this->type == section) ptf_free(this->sub);
    free(this);
  };
}

/***************************************************************************/
void abort_parsing(struct ptf_parser_state *state)
{
  if(state->filename != NULL) free(state->filename);
  ptf_free(state->tree);
}

/***************************************************************************/
int add_char_to_buffer(int *len, char *buf, int maxlen, char c)
{
  if(*len >= maxlen) return 0;

  buf[(*len)++] = c;

  return 1;
}

/***************************************************************************/
void parse_char(struct ptf_parser_state *state, int c)
{
  int is_not_quoted;
  int is_no_space;
  enum { item_parsed, section_opened, section_closed, none } parser_event;

  switch(c)
  {
    case '\\':
    {
      if(state->flag.escaped == 0)
      {
        state->flag.escaped = 1;
        return;
      };
      break;
    };
    case '"':
    {
      if(!state->flag.escaped && !state->flag.single_quoted)
      {
        state->flag.double_quoted = 1 - state->flag.double_quoted;
        return;
      }
      break;
    };
    case '\'':
    {
      if(!state->flag.escaped && !state->flag.double_quoted)
      {
        state->flag.single_quoted = 1 - state->flag.single_quoted;
        return;
      }
      break;
    };
    case '\n':
    {
      state->line++;
      break;
    };
    default:
      break;
  };

  parser_event = none;

  is_not_quoted = !(state->flag.escaped ||
    state->flag.single_quoted || state->flag.double_quoted);
  is_no_space = (!is_not_quoted || !isspace(c));
  state->flag.escaped = 0;

  switch(state->expectation)
  {
    case section_or_item_specification:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: section_or_item_specification\n");
#endif

      if(is_not_quoted && c == '}')
      {
        parser_event = section_closed;
      }
      else if(is_no_space)
      {
        state->name_length = 1;
        state->name_buffer[0] = c;
        state->expectation = more_section_type_or_item_name_chars;
      };
      break;
    };

    case more_section_type_or_item_name_chars:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: more_section_type_or_item_name_chars\n");
#endif

      /* Item name is stored in name_buffer */
      /* Section type is stored in name_buffer */
      if(is_no_space)
      {
        if(!add_char_to_buffer(&state->name_length, state->name_buffer, PTFPARSER_NAMEBUFSIZE, c))
          parse_error(state, "First word is too long; I expected a shorter section type or item name");
      }
      else
      {
        state->expectation = whitespace_after_section_or_item_name;
      }
      break;
    };

    case whitespace_after_section_specification:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: whitespace_after_section_specification\n");
#endif

      if(c == '{')
        parser_event = section_opened;
      else if(is_no_space)
        parse_error(state, "Expected section content within brackets {...}");
      break;
    };

    case whitespace_after_section_or_item_name:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: whitespace_after_section_or_item_name\n");
#endif

      if(c == '{')
      {
        state->value_length = 0;
        parser_event = section_opened;
      }
      else if(c == '=')
        state->expectation = whitespace_before_item_value;
      else if(is_no_space)
      {
        state->value_length = 1;
        state->value_buffer[0] = c;
        state->expectation = more_section_name_chars;
      };
      break;
    };

    case more_section_name_chars:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: more_section_name_chars\n");
#endif

      /* Section name is stored in value_buffer */
      if(is_no_space)
      {
        if(!add_char_to_buffer(&state->value_length, state->value_buffer, PTFPARSER_VALUEBUFSIZE, c))
          parse_error(state, "Section name is too long");
      }
      else
        state->expectation = whitespace_after_section_specification;
      break;
    }

    case whitespace_before_item_value:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: whitespace_before_item_value\n");
#endif

      if(is_not_quoted && c == ';')
      {
        state->value_length = 0;
        parser_event = item_parsed;
      }
      else if(is_no_space)
      {
        state->value_length = 1;
        state->value_buffer[0] = c;
        state->expectation = more_item_value_chars;
      };
      break;
    };

    case more_item_value_chars:
    {
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: more_item_value_chars\n");
#endif

      /* Item value is stored in value_buffer */
      if(is_not_quoted && c == ';')
        parser_event = item_parsed;
      else if(is_no_space)
      {
        if(!add_char_to_buffer(&state->value_length, state->value_buffer, PTFPARSER_VALUEBUFSIZE, c))
          parse_error(state, "Item value is too long");
      }
      else
        parser_event = item_parsed;
      break;
    }

    default:
#if DEBUG&DEBUG_EXPECTATIONS
      printf("Expectation: %d (?)\n", state->expectation);
#endif

      parse_error(state, "Internal error: Unhandled state of expectation");
  };

  switch(parser_event)
  {
    /* TODO: pointer tuff */

    case item_parsed:
    {
      struct ptf *new_item;
      state->name_buffer[state->name_length] = 0;
      state->value_buffer[state->value_length] = 0;
#if DEBUG&DEBUG_READVALUES
      printf("== Item %s is '%s' ==\n", state->name_buffer, state->value_buffer);
#endif

      new_item = ptf_alloc_item(item, state->name_buffer, state->value_buffer);
      if(new_item == NULL)
      {
        parse_error(state, "Internal error: "
            "Could not allocate memory for new item");
        return;
      };

      add_ptf_item(state, new_item);
      state->current_item = new_item;
      state->expectation = section_or_item_specification;

      break;
    };
    case section_opened:
    {
      struct ptf *new_section;
      state->name_buffer[state->name_length] = 0;
      state->value_buffer[state->value_length] = 0;
#if DEBUG&DEBUG_READVALUES
      printf("== New %s section '%s' opened ==\n", state->name_buffer, state->value_buffer);
#endif

      if(state->section_level >= PTFPARSER_MAXDEPTH-1)
      {
        parse_error(state, "Internal error: "
             "cannot handle sections nested as deep as here.");
        return;
      };

      new_section = ptf_alloc_item(section, state->name_buffer, state->value_buffer);
      if(new_section == NULL)
      {
        parse_error(state, "Internal error: "
            "Could not allocate memory for new section");
        return;
      };

      add_ptf_item(state, new_section);
      state->current_item = NULL;
      state->current_in[state->section_level] = new_section;
      state->section_level++;

      state->expectation = section_or_item_specification;
      break;
    };
    case section_closed:
    {
      if(state->section_level < 1)
      {
        parse_error(state, "Found closing '}' without opening '{' before");
        return;
      };

      state->section_level--;
      state->current_item = state->current_in[state->section_level];
      state->expectation = section_or_item_specification;
#if DEBUG&DEBUG_READVALUES
      printf("-- Closed section --\n");
#endif
      break;
    };
    default:
      break;
  };
}

/***************************************************************************/
struct ptf *ptf_parse_file(char *filename)
{
  FILE *f;
  char buffer[1024];

  struct ptf *root;
  struct ptf_parser_state state;

  if(filename == NULL)
  {
    fprintf(stderr, "Internal error: "
                    "No filename was given to ptf_read()\n");
    return NULL;
  };

  f = fopen(filename, "r");
  if(f == NULL)
  {
    perror(filename);
    return NULL;
  };

  init_parser(&state, filename);

  while(!feof(f))
  {
    size_t r, n;

    if(ferror(f))
    {
      perror(filename);
      abort_parsing(&state);
      fclose(f);
      return NULL;
    };

    n = fread(buffer, 1, 1024, f);
    for(r=0; r<n && (state.flag.error==0); r++) parse_char(&state, buffer[r]);
  };

  fclose(f);

  if(state.section_level != 0)
  {
    parse_error(&state, "Input file seems to be incomplete, "
                        "one or more sections are not closed with '}'\n");
  };

  if(state.flag.error)
  {
    abort_parsing(&state);
    return NULL;
  };

  return state.tree;
}

/***************************************************************************/

struct ptf *ptf_concat(struct ptf *a, struct ptf *b)
{
  struct ptf *leaf = a;

  if(!a) return b;
  if(!b) return a;

  for(leaf = a; leaf->next != NULL; leaf = leaf->next);
  leaf->next = b;
  return a;
}

/***************************************************************************/

void ptf_dump_ptf_item(FILE *f, struct ptf_item *pi)
{
  int i;
  fprintf(f, "level=%d in %p\n", pi->level, pi);
  for(i=pi->level;i>=0;i--)
  {
    if(pi->item[i] != NULL)
    {
      fprintf(f, "  %d: %s name=%s value=%s\n",
        i,
        pi->item[i]->type == item ? "item":"section",
        pi->item[i]->name,
        pi->item[i]->value);
    }
    else
    {
        fprintf(f, "  %d: NULL\n");
    }
    fflush(f);
  }
}

/***************************************************************************/

void ptf_printf(FILE *s, struct ptf *tree, char *prefix)
{
  struct ptf *leaf;

  for(leaf = tree; leaf != NULL; leaf = leaf->next)
  {
    switch(leaf->type)
    {
      case section:
      {
        char *new_prefix;
        int new_prefix_len;
        new_prefix_len = strlen(prefix) + strlen(leaf->name) + 2;
        if(leaf->value != NULL && leaf->value[0] != 0)
        {
          new_prefix_len += strlen(leaf->value) + 1;
        };
        new_prefix = (char *)malloc(new_prefix_len);
        strcpy(new_prefix, prefix);
        strcat(new_prefix, leaf->name);
        if(leaf->value != NULL && leaf->value[0] != 0)
        {
          strcat(new_prefix, ":");
          strcat(new_prefix, leaf->value);
        };
        strcat(new_prefix, "/");
        fputs(new_prefix, s);
        fputs("\n", s);
        ptf_printf(s, leaf->sub, new_prefix);
        break;
      };

      case item:
      {
        char *c;
        fputs(prefix, s);
        fputs(leaf->name, s);
        fputs(" = \"", s);
        if(leaf->value == NULL)
        {
          fputs("(NULL)", s);
        }
        else
        {
          for(c=leaf->value; *c; c++)
          {
            if(*c=='\\' || *c=='"') putc('\\', s);
            putc(*c, s);
          };
        }
        fprintf(s, "\"\n");
        break;
      };

      default:
        break;
    };
  };
}

/***************************************************************************/

int ptf_advance_one(struct ptf_item *item)
{
  int d;
  struct ptf *leaf;

  d = item->level;
  leaf = item->item[d];

  if(leaf != NULL)
  {
    if(leaf->type == section && leaf->sub != NULL)
    {
      if(item->level >= MAX_SECTION_NESTING-1)
      {
        /* raise an error? hm, for now we silently ignore the subtree */
      }
      else
      {
        d++;
        item->item[d] = leaf->sub;
        item->level = d;
        return 0;
      }
    }
    item->item[item->level] = leaf->next;
  };

  while(item->item[d] == NULL)
  {
    if(d == 0)
    {
      item->level = 0;
      item->item[0] = NULL;
      errno = ENOENT;
      return -1;
    }
    d --;
    leaf = item->item[d];
    if(leaf != NULL) item->item[d] = leaf->next;
  };

  item->level = d;
  return 0;
}

/***************************************************************************/

int ptf_advance_until(
  struct ptf_item *item,
  ptf_item_type ttype,
  char *name,
  char *value)
{
  int r;
  struct ptf *leaf;

  do
  {
    leaf = item->item[item->level];
#if DEBUG&DEBUG_FINDER
    printf(" Does %s/%s match %s/%s?\n", leaf->name, leaf->value, name, value);
#endif

    if(leaf->type == ttype)
    {
      if(name == NULL)
      {
        if(value == NULL)
        {
          return 0; /* got it (any value) */
        }
        else if (leaf->value != NULL)
        {
          if(strcmp(leaf->value, value) == 0) return 0; /* got it */
        }
      }
      else if(leaf->name != NULL)
      {
        if(strcmp(leaf->name, name) == 0)
        {
          if(value == NULL)
          {
            return 0; /* got it (any value) */
          }
          else if(leaf->value != NULL)
          {
            if(strcmp(leaf->value, value) == 0) return 0; /* got it */
          }
        }
      }
    };
    r = ptf_advance_one(item);

  } while(r == 0);

  return r;
}

/***************************************************************************/

struct ptf *ptf_find(
  struct ptf *tree,
  struct ptf_item *item,
  ptf_item_type ttype,
  char *name,
  char *value)
{
  int r;

  if(item == NULL) { errno = EINVAL; return NULL; };
  if(tree == NULL) { errno = ENOENT; return NULL; };

  item->level = 0;
  item->item[0] = tree;

  if(ptf_advance_until(item, ttype, name, value) != 0) return NULL;

  r = item->level;
  item->level++; /* To match ptf_match */
  return item->item[r];
}

/***************************************************************************/

struct ptf *ptf_next(
  struct ptf_item *item,
  ptf_item_type ttype,
  char *name,
  char *value)
{
  int r;
  struct ptf *leaf;

  if(item == NULL) { errno = EINVAL; return NULL; };

  if(item->level < 1) return NULL;
  item->level--; /* To match ptf_match */

  r = ptf_advance_one(item);

  if(r == 0) r = ptf_advance_until(item, ttype, name, value);

  if(r != 0) return NULL;

  r = item->level;
  item->level++; /* To match ptf_match */
  return item->item[r];
}

/***************************************************************************/

int ptf_match(
  struct ptf *const ptf,
  struct ptf_item *const match,
  const ptf_match_action action,
  void *arg)
{
    int count;
    struct ptf *p;
    struct ptf_item pi;

    p = ptf;
    count = 0;
    pi.level = 0;

    while(p != NULL)
    {
        ptf_item_type mtype = match->item[pi.level]->type;
        char *mname = match->item[pi.level]->name;
        char *mvalue = match->item[pi.level]->value;

#if DEBUG&DEBUG_FINDER
        printf("Looking for %s/%s, checking %s/%s\n",
          mname, mvalue, p->name, p->value);
#endif

        if(mtype == p->type &&
           (mname==NULL || p->name==NULL || strcmp(mname, p->name)==0) &&
           (mvalue==NULL || p->value==NULL || strcmp(mvalue, p->value)==0))
        {
            pi.item[pi.level] = p;

            if(pi.level == match->level - 1)
            {
                if(action != NULL) action(&pi, arg);
                p = p->next;
                count++;
            }
            else
            {
                if(p->sub != NULL && pi.level < MAX_SECTION_NESTING-1)
                {
                    pi.item[pi.level] = p;
                    pi.level++;
                    p = p->sub;
                }
                else
                {
                    p = p->next;
                };
            };
        }
        else
        {
            p = p->next;
        };

        while(p == NULL && pi.level > 0)
        {
            pi.level--;
            p = pi.item[pi.level]->next;
        };
    };
    return count;
}

/***************************************************************************/

char *ptf_defused_name(char *orig_name)
{
  int i,j;
  char *s = (char *)malloc(1+strlen(orig_name));

  if(!s) return NULL;

  for(i=j=0;orig_name[i];i++)
  {
    if(!isalnum(orig_name[i]))
    {
      if(j>0) if(s[j-1]!='_') s[j++]='_';
    }
    else
    {
      s[j++] = toupper(orig_name[i]);
    };
  };
  s[j] = 0;
  return s;
}

