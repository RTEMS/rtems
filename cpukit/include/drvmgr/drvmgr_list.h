/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Linked list help functions used by driver manager.
 */

/*
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

/*
 * Help functions for the Driver Manager. Implements a singly linked list
 * with head and tail pointers for fast insertions/deletions to head and
 * tail in list.
 */

#ifndef _DRVIVER_MANAGER_LIST_H_
#define _DRVIVER_MANAGER_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! List description, Singly link list with head and tail pointers. */
struct drvmgr_list {
	void	*head;	/*!< First entry in queue */
	void	*tail;	/*!< Last entry in queue */
	int	ofs;	/*!< Offset into head and tail to find next field */
};

/* Static initialization of list */
#define LIST_INITIALIZER(type, field) {NULL, NULL, offsetof(type, field)}

/* Return the first element in list */
#define LIST_HEAD(list, type) ((type *)(list)->head)

/* Return the last element in list */
#define LIST_TAIL(list, type) ((type *)(list)->tail)

/* Get the next pointer of an entry */
#define LIST_FIELD(list, entry) (*(void **)((char *)(entry) + (list)->ofs))

/* Return the next emlement in list */
#define LIST_NEXT(list, entry, type) ((type *)(LIST_FIELD(list, entry)))

/* Iterate through all entries in list */
#define LIST_FOR_EACH(list, entry, type) \
	for (entry = LIST_HEAD(list, type); \
	     entry; \
	     entry = LIST_NEXT(list, entry, type))

/*! Initialize a list during runtime
 *
 * \param list    The list to initialize
 * \param offset  The number of bytes into the entry structure the next pointer
 *                is found
 */
extern void drvmgr_list_init(struct drvmgr_list *list, int offset);

/*! Clear list */
extern void drvmgr_list_empty(struct drvmgr_list *list);

/*! Add entry to front of list */
extern void drvmgr_list_add_head(struct drvmgr_list *list, void *entry);

/*! Add entry to end of list */
extern void drvmgr_list_add_tail(struct drvmgr_list *list, void *entry);

/*! Remove entry from front of list */
extern void drvmgr_list_remove_head(struct drvmgr_list *list);

/*! Remove entry from anywhere in list */
extern void drvmgr_list_remove(struct drvmgr_list *list, void *entry);

#ifdef __cplusplus
}
#endif

#endif
