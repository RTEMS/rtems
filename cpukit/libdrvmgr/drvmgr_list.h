/* Linked list help functions used by driver manager.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
