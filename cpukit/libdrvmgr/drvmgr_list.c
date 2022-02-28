/* SPDX-License-Identifier: BSD-2-Clause */

/* Driver Manager List Interface Implementation.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
