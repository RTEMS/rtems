/*
 *  File: symbols.c
 *
 *  Description:
 *    Symbol table manager for the RTEMS monitor.
 *    These routines may be used by other system resources also.
 *
 *
 *  TODO:
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <rtems/monitor.h>
#include "symbols.h"


rtems_symbol_table_t *
rtems_symbol_table_create(void)
{
    rtems_symbol_table_t *table;

    table = (rtems_symbol_table_t *) malloc(sizeof(rtems_symbol_table_t));
    memset((void *) table, 0, sizeof(*table));

    table->growth_factor = 30;          /* 30 percent */

    return table;
}

void
rtems_symbol_table_destroy(rtems_symbol_table_t *table)
{
    rtems_symbol_string_block_t *p, *pnext;

    if (table)
    {
        if (table->addresses)
            (void) free(table->addresses);
        table->addresses = 0;
        p = table->string_buffer_head;
        while (p)
        {
            pnext = p->next;
            free(p);
            p = pnext;
        }
        table->string_buffer_head = 0;
        table->string_buffer_current = 0;

        free(table);
    }
}

rtems_symbol_t *
rtems_symbol_create(
    rtems_symbol_table_t *table,
    const char           *name,
    uint32_t              value
    )
{
    size_t symbol_length;
    size_t newsize;
    rtems_symbol_t *sp;

    symbol_length = strlen(name) + 1;   /* include '\000' in length */

    /* need to grow the table? */
    if (table->next >= table->size)
    {
        if (table->size == 0)
            newsize = 100;
        else
            newsize = table->size + (table->size / (100 / table->growth_factor));

        table->addresses = (rtems_symbol_t *) realloc((void *) table->addresses, newsize * sizeof(rtems_symbol_t));
        if (table->addresses == 0)        /* blew it; lost orig */
            goto failed;
        table->size = newsize;
    }

    sp = &table->addresses[table->next];
    sp->value = value;

    /* Have to add it to string pool */
    /* need to grow pool? */

    if ((table->string_buffer_head == 0) ||
        (table->strings_next + symbol_length) >= SYMBOL_STRING_BLOCK_SIZE)
    {
        rtems_symbol_string_block_t *p;

        p = (rtems_symbol_string_block_t *) malloc(sizeof(rtems_symbol_string_block_t));
        if (p == 0)
            goto failed;
        p->next = 0;
        if (table->string_buffer_head == 0)
            table->string_buffer_head = p;
        else
            table->string_buffer_current->next = p;
        table->string_buffer_current = p;

        table->strings_next = 0;
    }

    sp->name = table->string_buffer_current->buffer + table->strings_next;
    (void) strcpy(sp->name, name);

    table->strings_next += symbol_length;
    table->sorted = 0;
    table->next++;

    return sp;

/* XXX Not sure what to do here.  We've possibly destroyed the initial
   symbol table due to realloc failure */
failed:
    return 0;
}

/*
 * Qsort entry point for compare by address
 */

static int
rtems_symbol_compare(const void *e1,
                     const void *e2)
{
    rtems_symbol_t *s1, *s2;
    s1 = (rtems_symbol_t *) e1;
    s2 = (rtems_symbol_t *) e2;

    if (s1->value < s2->value)
        return -1;
    if (s1->value > s2->value)
        return 1;
    return 0;
}


/*
 * Sort the symbol table using qsort
 */

static void
rtems_symbol_sort(rtems_symbol_table_t *table)
{
    qsort((void *) table->addresses, (size_t) table->next,
          sizeof(rtems_symbol_t), rtems_symbol_compare);
    table->sorted = 1;
}


/*
 * Search the symbol table by address
 * This code based on CYGNUS newlib bsearch, but changed
 * to allow for finding closest symbol <= key
 */

rtems_symbol_t *
rtems_symbol_value_lookup(
    rtems_symbol_table_t *table,
    uint32_t              value
  )
{
    rtems_symbol_t *sp;
    rtems_symbol_t *base;
    rtems_symbol_t *best = 0;
    uint32_t   distance;
    uint32_t   best_distance = ~0;
    uint32_t   elements;

    if (table == 0)
        table = rtems_monitor_symbols;

    if ((table == 0) || (table->size == 0))
        return 0;

    if (table->sorted == 0)
        rtems_symbol_sort(table);

    base = table->addresses;
    elements = table->next;

    while (elements)
    {
        sp = base + (elements / 2);
        if (value < sp->value)
            elements /= 2;
        else if (value > sp->value)
        {
            distance = value - sp->value;
            if (distance < best_distance)
            {
                best_distance = distance;
                best = sp;
            }
            base = sp + 1;
            elements = (elements / 2) - (elements % 2 ? 0 : 1);
        }
        else
            return sp;
    }

    if (value == base->value)
        return base;

    return best;
}

/*
 * Search the symbol table for the exact matching address.
 * If the symbol table has already been sorted, then
 * call the regular symbol value lookup, however, it it
 * has not yet been sorted, search it sequentially.
 * This routine is primarily used for low level symbol
 * lookups (eg. from exception handler and interrupt routines)
 * where the penality of sorted is not wanted and where
 * an exact match is needed such that symbol table order
 * is not important.
 */
const rtems_symbol_t *
rtems_symbol_value_lookup_exact(
    rtems_symbol_table_t *table,
    uint32_t              value
  )
{
    uint32_t   s;
    rtems_symbol_t *sp;

    if (table == 0)
    {
        table = rtems_monitor_symbols;
        if (table == 0)
            return NULL;
    }

    if (table->sorted)
    {
        sp = rtems_symbol_value_lookup(table, value);
        if ( rtems_symbol_value(sp) == value )
            return sp;
        else
            return NULL;  /* not an exact match */
    }

    for (s = 0, sp = table->addresses; s < table->next; s++, sp++)
    {
        if ( sp->value == value )
            return sp;
    }

    return NULL;

}


/*
 * Search the symbol table by string name (case independent)
 */

rtems_symbol_t *
rtems_symbol_name_lookup(
    rtems_symbol_table_t *table,
    const char           *name
  )
{
    uint32_t   s;
    rtems_symbol_t *sp;

    if (table == 0)
    {
        table = rtems_monitor_symbols;
        if (table == 0)
            return NULL;
    }

    for (s = 0, sp = table->addresses; s < table->next; s++, sp++)
    {
        if ( strcasecmp(sp->name, name) == 0 )
            return sp;
    }

    return NULL;
}

void *
rtems_monitor_symbol_next(
    void                   *object_info,
    rtems_monitor_symbol_t *canonical __attribute__((unused)),
    rtems_id               *next_id
)
{
    rtems_symbol_table_t *table;
    uint32_t   n = rtems_object_id_get_index(*next_id);

    table = *(rtems_symbol_table_t **) object_info;
    if (table == 0)
        goto failed;

    if (n >= table->next)
        goto failed;

    /* NOTE: symbols do not have id and name fields */

    if (table->sorted == 0)
        rtems_symbol_sort(table);

    _Thread_Disable_dispatch();

    *next_id += 1;
    return (void *) (table->addresses + n);

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}

void
rtems_monitor_symbol_canonical(
    rtems_monitor_symbol_t *canonical_symbol,
    rtems_symbol_t *sp
)
{
    canonical_symbol->value = sp->value;
    canonical_symbol->offset = 0;
    strncpy(canonical_symbol->name, sp->name, sizeof(canonical_symbol->name)-1);
}


void
rtems_monitor_symbol_canonical_by_name(
    rtems_monitor_symbol_t *canonical_symbol,
    const char             *name
)
{
    rtems_symbol_t *sp;

    sp = rtems_symbol_name_lookup(0, name);

    canonical_symbol->value = sp ? sp->value : 0;

    strncpy(canonical_symbol->name, name, sizeof(canonical_symbol->name) - 1);
    canonical_symbol->offset = 0;
}

void
rtems_monitor_symbol_canonical_by_value(
    rtems_monitor_symbol_t *canonical_symbol,
    void                   *value_void_p
)
{
    uintptr_t   value = (uintptr_t) value_void_p;
    rtems_symbol_t *sp;

    sp = rtems_symbol_value_lookup(0, value);
    if (sp)
    {
        canonical_symbol->value = sp->value;
        canonical_symbol->offset = value - sp->value;
        strncpy(canonical_symbol->name, sp->name, sizeof(canonical_symbol->name)-1);
    }
    else
    {
        canonical_symbol->value = value;
        canonical_symbol->offset = 0;
        canonical_symbol->name[0] = '\0';
    }
}


uint32_t
rtems_monitor_symbol_dump(
    rtems_monitor_symbol_t *canonical_symbol,
    bool                    verbose
)
{
    uint32_t   length = 0;

    /*
     * print the name if it exists AND if value is non-zero
     * Ie: don't print some garbage symbol for address 0
     */

    if (canonical_symbol->name[0] && (canonical_symbol->value != 0))
    {
        if (canonical_symbol->offset == 0)
            length += fprintf(stdout,"%.*s",
                             (int) sizeof(canonical_symbol->name),
                             canonical_symbol->name);
        else
            length += fprintf(stdout,"<%.*s+0x%" PRIx32 ">",
                             (int) sizeof(canonical_symbol->name),
                             canonical_symbol->name,
                             canonical_symbol->offset);
        if (verbose)
            length += fprintf(stdout,
                        " [0x%" PRIx32 "]", canonical_symbol->value);
    }
    else
        length += fprintf(stdout,"[0x%" PRIx32 "]", canonical_symbol->value);

    return length;
}


static void
rtems_monitor_symbol_dump_all(
    rtems_symbol_table_t *table,
    bool                  verbose __attribute__((unused))
)
{
    uint32_t   s;
    rtems_symbol_t *sp;

    if (table == 0)
    {
        table = rtems_monitor_symbols;
        if (table == 0)
            return;
    }

    if (table->sorted == 0)
        rtems_symbol_sort(table);

    for (s = 0, sp = table->addresses; s < table->next; s++, sp++)
    {
        rtems_monitor_symbol_t canonical_symbol;

        rtems_monitor_symbol_canonical(&canonical_symbol, sp);
        rtems_monitor_symbol_dump(&canonical_symbol, true);
        fprintf(stdout,"\n");
    }
}


/*
 * 'symbol' command
 */

void rtems_monitor_symbol_cmd(
  int                                argc,
  char                             **argv,
  const rtems_monitor_command_arg_t *command_arg,
  bool                               verbose
)
{
    int arg;
    rtems_symbol_table_t *table;

    table = *command_arg->symbol_table;
    if (table == 0)
    {
        table = rtems_monitor_symbols;
        if (table == 0)
            return;
    }

    /*
     * Use object command to dump out whole symbol table
     */
    if (argc == 1)
        rtems_monitor_symbol_dump_all(table, verbose);
    else
    {
        rtems_monitor_symbol_t canonical_symbol;

        for (arg=1; argv[arg]; arg++)
        {
            rtems_monitor_symbol_canonical_by_name(&canonical_symbol, argv[arg]);
            rtems_monitor_symbol_dump(&canonical_symbol, verbose);
            fprintf(stdout,"\n");
        }
    }
}
