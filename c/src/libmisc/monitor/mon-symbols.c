/*
 *      @(#)symbols.c	1.3 - 95/04/24
 *      
 */

/* #define qsort _quicksort */

/*
 *  File:	symbols.c
 *
 *  Description:
 *    Symbol table manager for the RTEMS monitor.
 *    These routines may be used by other system resources also.
 *
 *
 *  TODO:
 */

#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbols.h"

extern rtems_symbol_table_t *rtems_monitor_symbols;

#ifdef RTEMS_DEBUG
#define CHK_ADR_PTR(p)  \
do { \
    if (((p) < rtems_monitor_symbols->addresses) || \
        ((p) >= (rtems_monitor_symbols->addresses + rtems_monitor_symbols->next))) \
    { \
        printf("bad address pointer %p\n", (p)); \
        rtems_fatal_error_occurred(RTEMS_INVALID_ADDRESS); \
    } \
} while (0)

#define CHK_NAME_PTR(p) \
do { \
    if (((p) < rtems_monitor_symbols->symbols) || \
        ((p) >= (rtems_monitor_symbols->symbols + rtems_monitor_symbols->next))) \
    { \
        printf("bad symbol pointer %p\n", (p)); \
        rtems_fatal_error_occurred(RTEMS_INVALID_ADDRESS); \
    } \
} while (0)
#else
#define CHK_ADR_PTR(p)
#define CHK_NAME_PTR(p)
#endif

rtems_symbol_table_t *
rtems_symbol_table_create()
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

        if (table->symbols)
            (void) free(table->symbols);
        table->symbols = 0;

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
    char                 *name,
    rtems_unsigned32     value
    )
{
    int symbol_length;
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

        table->symbols = (rtems_symbol_t *) realloc((void *) table->symbols, newsize * sizeof(rtems_symbol_t));
        if (table->symbols == 0)        /* blew it; lost orig */
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

    table->symbols[table->next] = *sp;

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

int
rtems_symbol_compare(const void *e1,
                     const void *e2)
{
    rtems_symbol_t *s1, *s2;
    s1 = (rtems_symbol_t *) e1;
    s2 = (rtems_symbol_t *) e2;

    CHK_ADR_PTR(s1);
    CHK_ADR_PTR(s2);

    if (s1->value < s2->value)
        return -1;
    if (s1->value > s2->value)
        return 1;
    return 0;
}

/*
 * Qsort entry point for compare by string name (case independent)
 */

int
rtems_symbol_string_compare(const void *e1,
                            const void *e2)
{
    rtems_symbol_t *s1, *s2;
    s1 = (rtems_symbol_t *) e1;
    s2 = (rtems_symbol_t *) e2;

    CHK_NAME_PTR(s1);
    CHK_NAME_PTR(s2);

    return strcasecmp(s1->name, s2->name);
}


/*
 * Sort the symbol table using qsort
 */

void
rtems_symbol_sort(rtems_symbol_table_t *table)
{
#ifdef simhppa
    printf("Sorting symbols ... ");         /* so slow we need a msg */
    fflush(stdout);
#endif

    qsort((void *) table->addresses, (size_t) table->next,
          sizeof(rtems_symbol_t), rtems_symbol_compare);

    qsort((void *) table->symbols, (size_t) table->next,
          sizeof(rtems_symbol_t), rtems_symbol_string_compare);

#ifdef simhppa
    /* so slow we need a msg */
    printf("done\n");
#endif

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
    rtems_unsigned32      value
  )
{
    rtems_symbol_t *sp;
    rtems_symbol_t *base;
    rtems_symbol_t *best = 0;
    rtems_unsigned32 distance;
    rtems_unsigned32 best_distance = ~0;
    rtems_unsigned32 elements;

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
 * Search the symbol table by string name (case independent)
 */

rtems_symbol_t *
rtems_symbol_name_lookup(
    rtems_symbol_table_t *table,
    char                 *name
  )
{
    rtems_symbol_t *sp = 0;
    rtems_symbol_t  key;

    if ((table == 0) || (name == 0))
        goto done;

    if (table->sorted == 0)
    {
        rtems_symbol_sort(table);
    }

    /*
     * dummy up one for bsearch()
     */

    key.name = name;
    key.value = 0;

    sp = (rtems_symbol_t *) bsearch((const void *) &key,
                                    (const void *) table->symbols,
                                    (size_t) table->next,
                                    sizeof(rtems_symbol_t),
                                    rtems_symbol_string_compare);

done:
    return sp;
}

