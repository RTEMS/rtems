/* Driver Manager List Interface Implementation.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <drvmgr/drvmgr_list.h>

/* LIST interface */

void drvmgr_list_init(struct drvmgr_list *list, int offset)
{
	list->head = list->tail = NULL;
	list->ofs = offset;
}

void drvmgr_list_empty(struct drvmgr_list *list)
{
	list->head = list->tail = NULL;
}

void drvmgr_list_add_head(struct drvmgr_list *list, void *entry)
{
	LIST_FIELD(list, entry) = list->head;
	if (list->head == NULL)
		list->tail = entry;
	list->head = entry;
}

void drvmgr_list_add_tail(struct drvmgr_list *list, void *entry)
{
	if (list->tail == NULL)
		list->head = entry;
	else
		LIST_FIELD(list, list->tail) = entry;
	LIST_FIELD(list, entry) = NULL;
	list->tail = entry;
}

void drvmgr_list_remove_head(struct drvmgr_list *list)
{
	list->head = LIST_FIELD(list, list->head);
	if (list->head == NULL)
		list->tail = NULL;
}

void drvmgr_list_remove(struct drvmgr_list *list, void *entry)
{
	void **prevptr = &list->head;
	void *curr, *prev;

	prev = NULL;
	curr = list->head;
	while (curr != entry) {
		prev = curr;
		prevptr = &LIST_FIELD(list, curr);
		curr = LIST_FIELD(list, curr);
	}
	*prevptr = LIST_FIELD(list, entry);
	if (list->tail == entry)
		list->tail = prev;
}
