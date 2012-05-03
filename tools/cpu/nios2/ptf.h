/*
 *  Copyright (c) 2006 Kolja Waschk rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if !defined(__PTF_H)
#define __PTF_H 1

#include <stdio.h>

#define MAX_SECTION_NESTING 20

typedef enum
{
  item, section
}
ptf_item_type;

struct ptf
{
  ptf_item_type type;
  char *name;
  char *value;
  struct ptf *sub;
  struct ptf *next;
};

struct ptf_item
{
  int level;
  struct ptf *item[MAX_SECTION_NESTING];
};


typedef void (*ptf_match_action)(struct ptf_item *x, void *arg);

struct ptf *ptf_parse_file(char *filename);
struct ptf *ptf_concat(struct ptf *a, struct ptf *b);
struct ptf *ptf_alloc_item(ptf_item_type t, char *name, char *value);
void ptf_printf(FILE *s, struct ptf *tree, char *prefix);
void ptf_dump_ptf_item(FILE *s, struct ptf_item *pi);

struct ptf *ptf_find(
  struct ptf *tree,
  struct ptf_item *item,
  ptf_item_type ttype,
  char *name,
  char *value);

struct ptf *ptf_next(
  struct ptf_item *item,
  ptf_item_type ttype,
  char *name,
  char *value);

int ptf_match(
  struct ptf *const ptf,
  struct ptf_item *const match,
  const ptf_match_action action,
  void *arg);

char *ptf_defused_name(char *);


#endif /* !defined(__PTF_H) */

