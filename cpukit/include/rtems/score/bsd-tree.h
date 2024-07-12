/*	$NetBSD: tree.h,v 1.8 2004/03/28 19:38:30 provos Exp $	*/
/*	$OpenBSD: tree.h,v 1.7 2002/10/17 21:51:54 art Exp $	*/
/* $FreeBSD: head/sys/sys/tree.h 347360 2019-05-08 18:47:00Z trasz $ */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This header file provides a red-black tree implementation.
 *
 * The contents of this file come from the above version of FreeBSD with local
 * changes applied in Newlib and then migrated to RTEMS. These changes lock
 * RTEMS to a specific version of this file. As a result, the macros defined
 * in this file have been scoped with an RTEMS_ namespace. This allows other
 * versions of tree.h to be used by RTEMS libraries and applications without
 * conflicts.
 */

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	_RTEMS_SCORE_BSD_TREE_H_
#define	_RTEMS_SCORE_BSD_TREE_H_

#include <sys/cdefs.h>

/*
 * This file defines data structures for different types of trees:
 * splay trees and red-black trees.
 *
 * A splay tree is a self-organizing data structure.  Every operation
 * on the tree causes a splay to happen.  The splay moves the requested
 * node to the root of the tree and partly rebalances it.
 *
 * This has the benefit that request locality causes faster lookups as
 * the requested nodes move to the top of the tree.  On the other hand,
 * every lookup causes memory writes.
 *
 * The Balance Theorem bounds the total access time for m operations
 * and n inserts on an initially empty tree as O((m + n)lg n).  The
 * amortized cost for a sequence of m accesses to a splay tree is O(lg n);
 *
 * A red-black tree is a binary search tree with the node color as an
 * extra attribute.  It fulfills a set of conditions:
 *	- every search path from the root to a leaf consists of the
 *	  same number of black nodes,
 *	- each red node (except for the root) has a black parent,
 *	- each leaf node is black.
 *
 * Every operation on a red-black tree is bounded as O(lg n).
 * The maximum height of a red-black tree is 2lg (n+1).
 */

#define RTEMS_SPLAY_HEAD(name, type)						\
struct name {								\
	struct type *sph_root; /* root of the tree */			\
}

#define RTEMS_SPLAY_INITIALIZER(root)						\
	{ NULL }

#define RTEMS_SPLAY_INIT(root) do {						\
	(root)->sph_root = NULL;					\
} while (/*CONSTCOND*/ 0)

#define RTEMS_SPLAY_ENTRY(type)						\
struct {								\
	struct type *spe_left; /* left element */			\
	struct type *spe_right; /* right element */			\
}

#define RTEMS_SPLAY_LEFT(elm, field)		(elm)->field.spe_left
#define RTEMS_SPLAY_RIGHT(elm, field)		(elm)->field.spe_right
#define RTEMS_SPLAY_ROOT(head)		(head)->sph_root
#define RTEMS_SPLAY_EMPTY(head)		(RTEMS_SPLAY_ROOT(head) == NULL)

/* RTEMS_SPLAY_ROTATE_{LEFT,RIGHT} expect that tmp hold RTEMS_SPLAY_{RIGHT,LEFT} */
#define RTEMS_SPLAY_ROTATE_RIGHT(head, tmp, field) do {			\
	RTEMS_SPLAY_LEFT((head)->sph_root, field) = RTEMS_SPLAY_RIGHT(tmp, field);	\
	RTEMS_SPLAY_RIGHT(tmp, field) = (head)->sph_root;			\
	(head)->sph_root = tmp;						\
} while (/*CONSTCOND*/ 0)
	
#define RTEMS_SPLAY_ROTATE_LEFT(head, tmp, field) do {			\
	RTEMS_SPLAY_RIGHT((head)->sph_root, field) = RTEMS_SPLAY_LEFT(tmp, field);	\
	RTEMS_SPLAY_LEFT(tmp, field) = (head)->sph_root;			\
	(head)->sph_root = tmp;						\
} while (/*CONSTCOND*/ 0)

#define RTEMS_SPLAY_LINKLEFT(head, tmp, field) do {				\
	RTEMS_SPLAY_LEFT(tmp, field) = (head)->sph_root;			\
	tmp = (head)->sph_root;						\
	(head)->sph_root = RTEMS_SPLAY_LEFT((head)->sph_root, field);		\
} while (/*CONSTCOND*/ 0)

#define RTEMS_SPLAY_LINKRIGHT(head, tmp, field) do {				\
	RTEMS_SPLAY_RIGHT(tmp, field) = (head)->sph_root;			\
	tmp = (head)->sph_root;						\
	(head)->sph_root = RTEMS_SPLAY_RIGHT((head)->sph_root, field);	\
} while (/*CONSTCOND*/ 0)

#define RTEMS_SPLAY_ASSEMBLE(head, node, left, right, field) do {		\
	RTEMS_SPLAY_RIGHT(left, field) = RTEMS_SPLAY_LEFT((head)->sph_root, field);	\
	RTEMS_SPLAY_LEFT(right, field) = RTEMS_SPLAY_RIGHT((head)->sph_root, field);\
	RTEMS_SPLAY_LEFT((head)->sph_root, field) = RTEMS_SPLAY_RIGHT(node, field);	\
	RTEMS_SPLAY_RIGHT((head)->sph_root, field) = RTEMS_SPLAY_LEFT(node, field);	\
} while (/*CONSTCOND*/ 0)

/* Generates prototypes and inline functions */

#define RTEMS_SPLAY_PROTOTYPE(name, type, field, cmp)				\
void name##_SPLAY(struct name *, struct type *);			\
void name##_RTEMS_SPLAY_MINMAX(struct name *, int);				\
struct type *name##_RTEMS_SPLAY_INSERT(struct name *, struct type *);		\
struct type *name##_RTEMS_SPLAY_REMOVE(struct name *, struct type *);		\
									\
/* Finds the node with the same key as elm */				\
static __unused __inline struct type *					\
name##_RTEMS_SPLAY_FIND(struct name *head, struct type *elm)			\
{									\
	if (RTEMS_SPLAY_EMPTY(head))						\
		return(NULL);						\
	name##_SPLAY(head, elm);					\
	if ((cmp)(elm, (head)->sph_root) == 0)				\
		return (head->sph_root);				\
	return (NULL);							\
}									\
									\
static __unused __inline struct type *					\
name##_RTEMS_SPLAY_NEXT(struct name *head, struct type *elm)			\
{									\
	name##_SPLAY(head, elm);					\
	if (RTEMS_SPLAY_RIGHT(elm, field) != NULL) {				\
		elm = RTEMS_SPLAY_RIGHT(elm, field);				\
		while (RTEMS_SPLAY_LEFT(elm, field) != NULL) {		\
			elm = RTEMS_SPLAY_LEFT(elm, field);			\
		}							\
	} else								\
		elm = NULL;						\
	return (elm);							\
}									\
									\
static __unused __inline struct type *					\
name##_RTEMS_SPLAY_MIN_MAX(struct name *head, int val)			\
{									\
	name##_RTEMS_SPLAY_MINMAX(head, val);					\
        return (RTEMS_SPLAY_ROOT(head));					\
}

/* Main splay operation.
 * Moves node close to the key of elm to top
 */
#define RTEMS_SPLAY_GENERATE(name, type, field, cmp)				\
struct type *								\
name##_RTEMS_SPLAY_INSERT(struct name *head, struct type *elm)		\
{									\
    if (RTEMS_SPLAY_EMPTY(head)) {						\
	    RTEMS_SPLAY_LEFT(elm, field) = RTEMS_SPLAY_RIGHT(elm, field) = NULL;	\
    } else {								\
	    int __comp;							\
	    name##_SPLAY(head, elm);					\
	    __comp = (cmp)(elm, (head)->sph_root);			\
	    if(__comp < 0) {						\
		    RTEMS_SPLAY_LEFT(elm, field) = RTEMS_SPLAY_LEFT((head)->sph_root, field);\
		    RTEMS_SPLAY_RIGHT(elm, field) = (head)->sph_root;		\
		    RTEMS_SPLAY_LEFT((head)->sph_root, field) = NULL;		\
	    } else if (__comp > 0) {					\
		    RTEMS_SPLAY_RIGHT(elm, field) = RTEMS_SPLAY_RIGHT((head)->sph_root, field);\
		    RTEMS_SPLAY_LEFT(elm, field) = (head)->sph_root;		\
		    RTEMS_SPLAY_RIGHT((head)->sph_root, field) = NULL;	\
	    } else							\
		    return ((head)->sph_root);				\
    }									\
    (head)->sph_root = (elm);						\
    return (NULL);							\
}									\
									\
struct type *								\
name##_RTEMS_SPLAY_REMOVE(struct name *head, struct type *elm)		\
{									\
	struct type *__tmp;						\
	if (RTEMS_SPLAY_EMPTY(head))						\
		return (NULL);						\
	name##_SPLAY(head, elm);					\
	if ((cmp)(elm, (head)->sph_root) == 0) {			\
		if (RTEMS_SPLAY_LEFT((head)->sph_root, field) == NULL) {	\
			(head)->sph_root = RTEMS_SPLAY_RIGHT((head)->sph_root, field);\
		} else {						\
			__tmp = RTEMS_SPLAY_RIGHT((head)->sph_root, field);	\
			(head)->sph_root = RTEMS_SPLAY_LEFT((head)->sph_root, field);\
			name##_SPLAY(head, elm);			\
			RTEMS_SPLAY_RIGHT((head)->sph_root, field) = __tmp;	\
		}							\
		return (elm);						\
	}								\
	return (NULL);							\
}									\
									\
void									\
name##_SPLAY(struct name *head, struct type *elm)			\
{									\
	struct type __node, *__left, *__right, *__tmp;			\
	int __comp;							\
\
	RTEMS_SPLAY_LEFT(&__node, field) = RTEMS_SPLAY_RIGHT(&__node, field) = NULL;\
	__left = __right = &__node;					\
\
	while ((__comp = (cmp)(elm, (head)->sph_root)) != 0) {		\
		if (__comp < 0) {					\
			__tmp = RTEMS_SPLAY_LEFT((head)->sph_root, field);	\
			if (__tmp == NULL)				\
				break;					\
			if ((cmp)(elm, __tmp) < 0){			\
				RTEMS_SPLAY_ROTATE_RIGHT(head, __tmp, field);	\
				if (RTEMS_SPLAY_LEFT((head)->sph_root, field) == NULL)\
					break;				\
			}						\
			RTEMS_SPLAY_LINKLEFT(head, __right, field);		\
		} else if (__comp > 0) {				\
			__tmp = RTEMS_SPLAY_RIGHT((head)->sph_root, field);	\
			if (__tmp == NULL)				\
				break;					\
			if ((cmp)(elm, __tmp) > 0){			\
				RTEMS_SPLAY_ROTATE_LEFT(head, __tmp, field);	\
				if (RTEMS_SPLAY_RIGHT((head)->sph_root, field) == NULL)\
					break;				\
			}						\
			RTEMS_SPLAY_LINKRIGHT(head, __left, field);		\
		}							\
	}								\
	RTEMS_SPLAY_ASSEMBLE(head, &__node, __left, __right, field);		\
}									\
									\
/* Splay with either the minimum or the maximum element			\
 * Used to find minimum or maximum element in tree.			\
 */									\
void name##_RTEMS_SPLAY_MINMAX(struct name *head, int __comp) \
{									\
	struct type __node, *__left, *__right, *__tmp;			\
\
	RTEMS_SPLAY_LEFT(&__node, field) = RTEMS_SPLAY_RIGHT(&__node, field) = NULL;\
	__left = __right = &__node;					\
\
	while (1) {							\
		if (__comp < 0) {					\
			__tmp = RTEMS_SPLAY_LEFT((head)->sph_root, field);	\
			if (__tmp == NULL)				\
				break;					\
			if (__comp < 0){				\
				RTEMS_SPLAY_ROTATE_RIGHT(head, __tmp, field);	\
				if (RTEMS_SPLAY_LEFT((head)->sph_root, field) == NULL)\
					break;				\
			}						\
			RTEMS_SPLAY_LINKLEFT(head, __right, field);		\
		} else if (__comp > 0) {				\
			__tmp = RTEMS_SPLAY_RIGHT((head)->sph_root, field);	\
			if (__tmp == NULL)				\
				break;					\
			if (__comp > 0) {				\
				RTEMS_SPLAY_ROTATE_LEFT(head, __tmp, field);	\
				if (RTEMS_SPLAY_RIGHT((head)->sph_root, field) == NULL)\
					break;				\
			}						\
			RTEMS_SPLAY_LINKRIGHT(head, __left, field);		\
		}							\
	}								\
	RTEMS_SPLAY_ASSEMBLE(head, &__node, __left, __right, field);		\
}

#define RTEMS_SPLAY_NEGINF	-1
#define RTEMS_SPLAY_INF	1

#define RTEMS_SPLAY_INSERT(name, x, y)	name##_RTEMS_SPLAY_INSERT(x, y)
#define RTEMS_SPLAY_REMOVE(name, x, y)	name##_RTEMS_SPLAY_REMOVE(x, y)
#define RTEMS_SPLAY_FIND(name, x, y)		name##_RTEMS_SPLAY_FIND(x, y)
#define RTEMS_SPLAY_NEXT(name, x, y)		name##_RTEMS_SPLAY_NEXT(x, y)
#define RTEMS_SPLAY_MIN(name, x)		(RTEMS_SPLAY_EMPTY(x) ? NULL	\
					: name##_RTEMS_SPLAY_MIN_MAX(x, RTEMS_SPLAY_NEGINF))
#define RTEMS_SPLAY_MAX(name, x)		(RTEMS_SPLAY_EMPTY(x) ? NULL	\
					: name##_RTEMS_SPLAY_MIN_MAX(x, RTEMS_SPLAY_INF))

#define RTEMS_SPLAY_FOREACH(x, name, head)					\
	for ((x) = RTEMS_SPLAY_MIN(name, head);				\
	     (x) != NULL;						\
	     (x) = RTEMS_SPLAY_NEXT(name, head, x))

/* Macros that define a red-black tree */
#define RTEMS_RB_HEAD(name, type)						\
struct name {								\
	struct type *rbh_root; /* root of the tree */			\
}

#define RTEMS_RB_INITIALIZER(root)						\
	{ NULL }

#define RTEMS_RB_INIT(root) do {						\
	(root)->rbh_root = NULL;					\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_BLACK	0
#define RTEMS_RB_RED		1
#define RTEMS_RB_ENTRY(type)							\
struct {								\
	struct type *rbe_left;		/* left element */		\
	struct type *rbe_right;		/* right element */		\
	struct type *rbe_parent;	/* parent element */		\
	int rbe_color;			/* node color */		\
}

#define RTEMS_RB_LEFT(elm, field)		(elm)->field.rbe_left
#define RTEMS_RB_RIGHT(elm, field)		(elm)->field.rbe_right
#define RTEMS_RB_PARENT(elm, field)		(elm)->field.rbe_parent
#define RTEMS_RB_COLOR(elm, field)		(elm)->field.rbe_color
#define RTEMS_RB_ISRED(elm, field)		((elm) != NULL && RTEMS_RB_COLOR(elm, field) == RTEMS_RB_RED)
#define RTEMS_RB_ROOT(head)			(head)->rbh_root
#define RTEMS_RB_EMPTY(head)			(RTEMS_RB_ROOT(head) == NULL)

#define RTEMS_RB_SET_PARENT(dst, src, field) do {				\
	RTEMS_RB_PARENT(dst, field) = src;					\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_SET(elm, parent, field) do {					\
	RTEMS_RB_SET_PARENT(elm, parent, field);				\
	RTEMS_RB_LEFT(elm, field) = RTEMS_RB_RIGHT(elm, field) = NULL;		\
	RTEMS_RB_COLOR(elm, field) = RTEMS_RB_RED;					\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_SET_BLACKRED(black, red, field) do {				\
	RTEMS_RB_COLOR(black, field) = RTEMS_RB_BLACK;				\
	RTEMS_RB_COLOR(red, field) = RTEMS_RB_RED;					\
} while (/*CONSTCOND*/ 0)

/*
 * Something to be invoked in a loop at the root of every modified subtree,
 * from the bottom up to the root, to update augmented node data.
 */
#ifndef RTEMS_RB_AUGMENT
#define RTEMS_RB_AUGMENT(x)	break
#endif

#define RTEMS_RB_SWAP_CHILD(head, out, in, field) do {			\
	if (RTEMS_RB_PARENT(out, field) == NULL)				\
		RTEMS_RB_ROOT(head) = (in);					\
	else if ((out) == RTEMS_RB_LEFT(RTEMS_RB_PARENT(out, field), field))	\
		RTEMS_RB_LEFT(RTEMS_RB_PARENT(out, field), field) = (in);		\
	else								\
		RTEMS_RB_RIGHT(RTEMS_RB_PARENT(out, field), field) = (in);		\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_ROTATE_LEFT(head, elm, tmp, field) do {			\
	(tmp) = RTEMS_RB_RIGHT(elm, field);					\
	if ((RTEMS_RB_RIGHT(elm, field) = RTEMS_RB_LEFT(tmp, field)) != NULL) {	\
		RTEMS_RB_SET_PARENT(RTEMS_RB_RIGHT(elm, field), elm, field);	\
	}								\
	RTEMS_RB_SET_PARENT(tmp, RTEMS_RB_PARENT(elm, field), field);		\
	RTEMS_RB_SWAP_CHILD(head, elm, tmp, field);				\
	RTEMS_RB_LEFT(tmp, field) = (elm);					\
	RTEMS_RB_SET_PARENT(elm, tmp, field);					\
	RTEMS_RB_AUGMENT(elm);						\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_ROTATE_RIGHT(head, elm, tmp, field) do {			\
	(tmp) = RTEMS_RB_LEFT(elm, field);					\
	if ((RTEMS_RB_LEFT(elm, field) = RTEMS_RB_RIGHT(tmp, field)) != NULL) {	\
		RTEMS_RB_SET_PARENT(RTEMS_RB_LEFT(elm, field), elm, field);		\
	}								\
	RTEMS_RB_SET_PARENT(tmp, RTEMS_RB_PARENT(elm, field), field);		\
	RTEMS_RB_SWAP_CHILD(head, elm, tmp, field);				\
	RTEMS_RB_RIGHT(tmp, field) = (elm);					\
	RTEMS_RB_SET_PARENT(elm, tmp, field);					\
	RTEMS_RB_AUGMENT(elm);						\
} while (/*CONSTCOND*/ 0)

/*
 * The RTEMS_RB_PARENT_ROTATE_LEFT() and RTEMS_RB_PARENT_ROTATE_RIGHT() rotations are
 * specialized versions of RTEMS_RB_ROTATE_LEFT() and RTEMS_RB_ROTATE_RIGHT() which may be
 * used if the parent node exists and the direction of the child element is
 * known.
 */

#define RTEMS_RB_PARENT_ROTATE_LEFT(parent, left, tmp, field) do {		\
	(tmp) = RTEMS_RB_RIGHT(left, field);					\
	if ((RTEMS_RB_RIGHT(left, field) = RTEMS_RB_LEFT(tmp, field)) != NULL) {	\
		RTEMS_RB_SET_PARENT(RTEMS_RB_RIGHT(left, field), left, field);	\
	}								\
	RTEMS_RB_SET_PARENT(tmp, parent, field);				\
	RTEMS_RB_LEFT(parent, field) = (tmp);					\
	RTEMS_RB_LEFT(tmp, field) = (left);					\
	RTEMS_RB_SET_PARENT(left, tmp, field);				\
	RTEMS_RB_AUGMENT(left);						\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_PARENT_ROTATE_RIGHT(parent, right, tmp, field) do {		\
	(tmp) = RTEMS_RB_LEFT(right, field);					\
	if ((RTEMS_RB_LEFT(right, field) = RTEMS_RB_RIGHT(tmp, field)) != NULL) {	\
		RTEMS_RB_SET_PARENT(RTEMS_RB_LEFT(right, field), right, field);	\
	}								\
	RTEMS_RB_SET_PARENT(tmp, parent, field);				\
	RTEMS_RB_RIGHT(parent, field) = (tmp);				\
	RTEMS_RB_RIGHT(tmp, field) = (right);					\
	RTEMS_RB_SET_PARENT(right, tmp, field);				\
	RTEMS_RB_AUGMENT(right);						\
} while (/*CONSTCOND*/ 0)

/*
 * The RTEMS_RB_RED_ROTATE_LEFT() and RTEMS_RB_RED_ROTATE_RIGHT() rotations are specialized
 * versions of RTEMS_RB_ROTATE_LEFT() and RTEMS_RB_ROTATE_RIGHT() which may be used if we
 * rotate an element with a red child which has a black sibling.  Such a red
 * node must have at least two child nodes so that the following red-black tree
 * invariant is fulfilled:
 *
 *  Every path from a given node to any of its descendant NULL nodes goes
 *  through the same number of black nodes.
 *
 *  elm (could be the root)
 *    /      \
 * BLACK   RED (left or right child)
 *          /   \
 *       BLACK  BLACK
 */

#define RTEMS_RB_RED_ROTATE_LEFT(head, elm, tmp, field) do {			\
	(tmp) = RTEMS_RB_RIGHT(elm, field);					\
	RTEMS_RB_RIGHT(elm, field) = RTEMS_RB_LEFT(tmp, field);			\
	RTEMS_RB_SET_PARENT(RTEMS_RB_RIGHT(elm, field), elm, field);		\
	RTEMS_RB_SET_PARENT(tmp, RTEMS_RB_PARENT(elm, field), field);		\
	RTEMS_RB_SWAP_CHILD(head, elm, tmp, field);				\
	RTEMS_RB_LEFT(tmp, field) = (elm);					\
	RTEMS_RB_SET_PARENT(elm, tmp, field);					\
	RTEMS_RB_AUGMENT(elm);						\
} while (/*CONSTCOND*/ 0)

#define RTEMS_RB_RED_ROTATE_RIGHT(head, elm, tmp, field) do {			\
	(tmp) = RTEMS_RB_LEFT(elm, field);					\
	RTEMS_RB_LEFT(elm, field) = RTEMS_RB_RIGHT(tmp, field);			\
	RTEMS_RB_SET_PARENT(RTEMS_RB_LEFT(elm, field), elm, field);			\
	RTEMS_RB_SET_PARENT(tmp, RTEMS_RB_PARENT(elm, field), field);		\
	RTEMS_RB_SWAP_CHILD(head, elm, tmp, field);				\
	RTEMS_RB_RIGHT(tmp, field) = (elm);					\
	RTEMS_RB_SET_PARENT(elm, tmp, field);					\
	RTEMS_RB_AUGMENT(elm);						\
} while (/*CONSTCOND*/ 0)

/* Generates prototypes and inline functions */
#define	RTEMS_RB_PROTOTYPE(name, type, field, cmp)				\
	RTEMS_RB_PROTOTYPE_INTERNAL(name, type, field, cmp,)
#define	RTEMS_RB_PROTOTYPE_STATIC(name, type, field, cmp)			\
	RTEMS_RB_PROTOTYPE_INTERNAL(name, type, field, cmp, __unused static)
#define RTEMS_RB_PROTOTYPE_INTERNAL(name, type, field, cmp, attr)		\
	RTEMS_RB_PROTOTYPE_INSERT_COLOR(name, type, attr);			\
	RTEMS_RB_PROTOTYPE_REMOVE_COLOR(name, type, attr);			\
	RTEMS_RB_PROTOTYPE_INSERT(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_REMOVE(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_FIND(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_NFIND(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_NEXT(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_PREV(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_MINMAX(name, type, attr);				\
	RTEMS_RB_PROTOTYPE_REINSERT(name, type, attr);
#define RTEMS_RB_PROTOTYPE_INSERT_COLOR(name, type, attr)			\
	attr void name##_RTEMS_RB_INSERT_COLOR(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_REMOVE_COLOR(name, type, attr)			\
	attr void name##_RTEMS_RB_REMOVE_COLOR(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_REMOVE(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_REMOVE(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_INSERT(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_INSERT(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_FIND(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_FIND(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_NFIND(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_NFIND(struct name *, struct type *)
#define RTEMS_RB_PROTOTYPE_NEXT(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_NEXT(struct type *)
#define RTEMS_RB_PROTOTYPE_PREV(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_PREV(struct type *)
#define RTEMS_RB_PROTOTYPE_MINMAX(name, type, attr)				\
	attr struct type *name##_RTEMS_RB_MINMAX(struct name *, int)
#define RTEMS_RB_PROTOTYPE_REINSERT(name, type, attr)			\
	attr struct type *name##_RTEMS_RB_REINSERT(struct name *, struct type *)

/* Main rb operation.
 * Moves node close to the key of elm to top
 */
#define	RTEMS_RB_GENERATE(name, type, field, cmp)				\
	RTEMS_RB_GENERATE_INTERNAL(name, type, field, cmp,)
#define	RTEMS_RB_GENERATE_STATIC(name, type, field, cmp)			\
	RTEMS_RB_GENERATE_INTERNAL(name, type, field, cmp, __unused static)
#define RTEMS_RB_GENERATE_INTERNAL(name, type, field, cmp, attr)		\
	RTEMS_RB_GENERATE_INSERT_COLOR(name, type, field, attr)		\
	RTEMS_RB_GENERATE_REMOVE_COLOR(name, type, field, attr)		\
	RTEMS_RB_GENERATE_INSERT(name, type, field, cmp, attr)		\
	RTEMS_RB_GENERATE_REMOVE(name, type, field, attr)			\
	RTEMS_RB_GENERATE_FIND(name, type, field, cmp, attr)			\
	RTEMS_RB_GENERATE_NFIND(name, type, field, cmp, attr)			\
	RTEMS_RB_GENERATE_NEXT(name, type, field, attr)			\
	RTEMS_RB_GENERATE_PREV(name, type, field, attr)			\
	RTEMS_RB_GENERATE_MINMAX(name, type, field, attr)			\
	RTEMS_RB_GENERATE_REINSERT(name, type, field, cmp, attr)


#define RTEMS_RB_GENERATE_INSERT_COLOR(name, type, field, attr)		\
attr void								\
name##_RTEMS_RB_INSERT_COLOR(struct name *head, struct type *elm)		\
{									\
	struct type *parent, *gparent, *tmp;				\
	while (RTEMS_RB_ISRED((parent = RTEMS_RB_PARENT(elm, field)), field)) {	\
		gparent = RTEMS_RB_PARENT(parent, field);			\
		if (parent == RTEMS_RB_LEFT(gparent, field)) {		\
			tmp = RTEMS_RB_RIGHT(gparent, field);			\
			if (RTEMS_RB_ISRED(tmp, field)) {			\
				RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_BLACK;	\
				RTEMS_RB_SET_BLACKRED(parent, gparent, field);\
				elm = gparent;				\
				continue;				\
			}						\
			if (RTEMS_RB_RIGHT(parent, field) == elm) {		\
				RTEMS_RB_PARENT_ROTATE_LEFT(gparent, parent,	\
				    tmp, field);			\
				tmp = parent;				\
				parent = elm;				\
				elm = tmp;				\
			}						\
			RTEMS_RB_SET_BLACKRED(parent, gparent, field);	\
			RTEMS_RB_ROTATE_RIGHT(head, gparent, tmp, field);	\
		} else {						\
			tmp = RTEMS_RB_LEFT(gparent, field);			\
			if (RTEMS_RB_ISRED(tmp, field)) {			\
				RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_BLACK;	\
				RTEMS_RB_SET_BLACKRED(parent, gparent, field);\
				elm = gparent;				\
				continue;				\
			}						\
			if (RTEMS_RB_LEFT(parent, field) == elm) {		\
				RTEMS_RB_PARENT_ROTATE_RIGHT(gparent, parent,	\
				    tmp, field);			\
				tmp = parent;				\
				parent = elm;				\
				elm = tmp;				\
			}						\
			RTEMS_RB_SET_BLACKRED(parent, gparent, field);	\
			RTEMS_RB_ROTATE_LEFT(head, gparent, tmp, field);	\
		}							\
	}								\
	RTEMS_RB_COLOR(head->rbh_root, field) = RTEMS_RB_BLACK;			\
}

#define RTEMS_RB_GENERATE_REMOVE_COLOR(name, type, field, attr)		\
attr void								\
name##_RTEMS_RB_REMOVE_COLOR(struct name *head, struct type *parent)		\
{									\
	struct type *elm, *tmp;						\
	elm = NULL;							\
	do {								\
		if (RTEMS_RB_LEFT(parent, field) == elm) {			\
			tmp = RTEMS_RB_RIGHT(parent, field);			\
			if (RTEMS_RB_COLOR(tmp, field) == RTEMS_RB_RED) {		\
				RTEMS_RB_SET_BLACKRED(tmp, parent, field);	\
				RTEMS_RB_RED_ROTATE_LEFT(head, parent, tmp, field); \
				tmp = RTEMS_RB_RIGHT(parent, field);		\
			}						\
			if (RTEMS_RB_ISRED(RTEMS_RB_RIGHT(tmp, field), field))	\
				RTEMS_RB_COLOR(RTEMS_RB_RIGHT(tmp, field), field) = RTEMS_RB_BLACK; \
			else if (RTEMS_RB_ISRED(RTEMS_RB_LEFT(tmp, field), field)) { \
				struct type *oleft;			\
				RTEMS_RB_PARENT_ROTATE_RIGHT(parent, tmp,	\
				    oleft, field);			\
				RTEMS_RB_COLOR(oleft, field) = RTEMS_RB_BLACK;	\
				tmp = oleft;				\
			} else {					\
				RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_RED;		\
				elm = parent;				\
				parent = RTEMS_RB_PARENT(elm, field);		\
				continue;				\
			}						\
			RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_COLOR(parent, field);	\
			RTEMS_RB_COLOR(parent, field) = RTEMS_RB_BLACK;		\
			RTEMS_RB_ROTATE_LEFT(head, parent, tmp, field);	\
			elm = RTEMS_RB_ROOT(head);				\
			break;						\
		} else {						\
			tmp = RTEMS_RB_LEFT(parent, field);			\
			if (RTEMS_RB_COLOR(tmp, field) == RTEMS_RB_RED) {		\
				RTEMS_RB_SET_BLACKRED(tmp, parent, field);	\
				RTEMS_RB_RED_ROTATE_RIGHT(head, parent, tmp, field); \
				tmp = RTEMS_RB_LEFT(parent, field);		\
			}						\
			if (RTEMS_RB_ISRED(RTEMS_RB_LEFT(tmp, field), field))	\
				RTEMS_RB_COLOR(RTEMS_RB_LEFT(tmp, field), field) = RTEMS_RB_BLACK; \
			else if (RTEMS_RB_ISRED(RTEMS_RB_RIGHT(tmp, field), field)) { \
				struct type *oright;			\
				RTEMS_RB_PARENT_ROTATE_LEFT(parent, tmp,	\
				    oright, field);			\
				RTEMS_RB_COLOR(oright, field) = RTEMS_RB_BLACK;	\
				tmp = oright;				\
			} else {					\
				RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_RED;		\
				elm = parent;				\
				parent = RTEMS_RB_PARENT(elm, field);		\
				continue;				\
			}						\
			RTEMS_RB_COLOR(tmp, field) = RTEMS_RB_COLOR(parent, field);	\
			RTEMS_RB_COLOR(parent, field) = RTEMS_RB_BLACK;		\
			RTEMS_RB_ROTATE_RIGHT(head, parent, tmp, field);	\
			elm = RTEMS_RB_ROOT(head);				\
			break;						\
		}							\
	} while (RTEMS_RB_COLOR(elm, field) == RTEMS_RB_BLACK && parent != NULL);	\
	RTEMS_RB_COLOR(elm, field) = RTEMS_RB_BLACK;				\
}

#define RTEMS_RB_GENERATE_REMOVE(name, type, field, attr)			\
attr struct type *							\
name##_RTEMS_RB_REMOVE(struct name *head, struct type *elm)			\
{									\
	struct type *child, *old, *parent, *right;			\
	int color;							\
									\
	old = elm;							\
	parent = RTEMS_RB_PARENT(elm, field);					\
	right = RTEMS_RB_RIGHT(elm, field);					\
	color = RTEMS_RB_COLOR(elm, field);					\
	if (RTEMS_RB_LEFT(elm, field) == NULL)				\
		elm = child = right;					\
	else if (right == NULL)						\
		elm = child = RTEMS_RB_LEFT(elm, field);			\
	else {								\
		if ((child = RTEMS_RB_LEFT(right, field)) == NULL) {		\
			child = RTEMS_RB_RIGHT(right, field);			\
			RTEMS_RB_RIGHT(old, field) = child;			\
			parent = elm = right;				\
		} else {						\
			do						\
				elm = child;				\
			while ((child = RTEMS_RB_LEFT(elm, field)) != NULL);	\
			child = RTEMS_RB_RIGHT(elm, field);			\
			parent = RTEMS_RB_PARENT(elm, field);			\
			RTEMS_RB_LEFT(parent, field) = child;			\
			RTEMS_RB_SET_PARENT(RTEMS_RB_RIGHT(old, field), elm, field); \
		}							\
		RTEMS_RB_SET_PARENT(RTEMS_RB_LEFT(old, field), elm, field);		\
		color = RTEMS_RB_COLOR(elm, field);				\
		elm->field = old->field;				\
	}								\
	RTEMS_RB_SWAP_CHILD(head, old, elm, field);				\
	if (child != NULL) {						\
		RTEMS_RB_SET_PARENT(child, parent, field);			\
		RTEMS_RB_COLOR(child, field) = RTEMS_RB_BLACK;			\
	} else if (color != RTEMS_RB_RED && parent != NULL)			\
		name##_RTEMS_RB_REMOVE_COLOR(head, parent);			\
	while (parent != NULL) {					\
		RTEMS_RB_AUGMENT(parent);					\
		parent = RTEMS_RB_PARENT(parent, field);			\
	}								\
	return (old);							\
}

#define RTEMS_RB_GENERATE_INSERT(name, type, field, cmp, attr)		\
/* Inserts a node into the RB tree */					\
attr struct type *							\
name##_RTEMS_RB_INSERT(struct name *head, struct type *elm)			\
{									\
	struct type *tmp;						\
	struct type *parent = NULL;					\
	int comp = 0;							\
	tmp = RTEMS_RB_ROOT(head);						\
	while (tmp) {							\
		parent = tmp;						\
		comp = (cmp)(elm, parent);				\
		if (comp < 0)						\
			tmp = RTEMS_RB_LEFT(tmp, field);			\
		else if (comp > 0)					\
			tmp = RTEMS_RB_RIGHT(tmp, field);			\
		else							\
			return (tmp);					\
	}								\
	RTEMS_RB_SET(elm, parent, field);					\
	if (parent != NULL) {						\
		if (comp < 0)						\
			RTEMS_RB_LEFT(parent, field) = elm;			\
		else							\
			RTEMS_RB_RIGHT(parent, field) = elm;			\
	} else								\
		RTEMS_RB_ROOT(head) = elm;					\
	name##_RTEMS_RB_INSERT_COLOR(head, elm);				\
	while (elm != NULL) {						\
		RTEMS_RB_AUGMENT(elm);					\
		elm = RTEMS_RB_PARENT(elm, field);				\
	}								\
	return (NULL);							\
}

#define RTEMS_RB_GENERATE_FIND(name, type, field, cmp, attr)			\
/* Finds the node with the same key as elm */				\
attr struct type *							\
name##_RTEMS_RB_FIND(struct name *head, struct type *elm)			\
{									\
	struct type *tmp = RTEMS_RB_ROOT(head);				\
	int comp;							\
	while (tmp) {							\
		comp = cmp(elm, tmp);					\
		if (comp < 0)						\
			tmp = RTEMS_RB_LEFT(tmp, field);			\
		else if (comp > 0)					\
			tmp = RTEMS_RB_RIGHT(tmp, field);			\
		else							\
			return (tmp);					\
	}								\
	return (NULL);							\
}

#define RTEMS_RB_GENERATE_NFIND(name, type, field, cmp, attr)			\
/* Finds the first node greater than or equal to the search key */	\
attr struct type *							\
name##_RTEMS_RB_NFIND(struct name *head, struct type *elm)			\
{									\
	struct type *tmp = RTEMS_RB_ROOT(head);				\
	struct type *res = NULL;					\
	int comp;							\
	while (tmp) {							\
		comp = cmp(elm, tmp);					\
		if (comp < 0) {						\
			res = tmp;					\
			tmp = RTEMS_RB_LEFT(tmp, field);			\
		}							\
		else if (comp > 0)					\
			tmp = RTEMS_RB_RIGHT(tmp, field);			\
		else							\
			return (tmp);					\
	}								\
	return (res);							\
}

#define RTEMS_RB_GENERATE_NEXT(name, type, field, attr)			\
/* ARGSUSED */								\
attr struct type *							\
name##_RTEMS_RB_NEXT(struct type *elm)					\
{									\
	if (RTEMS_RB_RIGHT(elm, field)) {					\
		elm = RTEMS_RB_RIGHT(elm, field);				\
		while (RTEMS_RB_LEFT(elm, field))				\
			elm = RTEMS_RB_LEFT(elm, field);			\
	} else {							\
		if (RTEMS_RB_PARENT(elm, field) &&				\
		    (elm == RTEMS_RB_LEFT(RTEMS_RB_PARENT(elm, field), field)))	\
			elm = RTEMS_RB_PARENT(elm, field);			\
		else {							\
			while (RTEMS_RB_PARENT(elm, field) &&			\
			    (elm == RTEMS_RB_RIGHT(RTEMS_RB_PARENT(elm, field), field)))\
				elm = RTEMS_RB_PARENT(elm, field);		\
			elm = RTEMS_RB_PARENT(elm, field);			\
		}							\
	}								\
	return (elm);							\
}

#define RTEMS_RB_GENERATE_PREV(name, type, field, attr)			\
/* ARGSUSED */								\
attr struct type *							\
name##_RTEMS_RB_PREV(struct type *elm)					\
{									\
	if (RTEMS_RB_LEFT(elm, field)) {					\
		elm = RTEMS_RB_LEFT(elm, field);				\
		while (RTEMS_RB_RIGHT(elm, field))				\
			elm = RTEMS_RB_RIGHT(elm, field);			\
	} else {							\
		if (RTEMS_RB_PARENT(elm, field) &&				\
		    (elm == RTEMS_RB_RIGHT(RTEMS_RB_PARENT(elm, field), field)))	\
			elm = RTEMS_RB_PARENT(elm, field);			\
		else {							\
			while (RTEMS_RB_PARENT(elm, field) &&			\
			    (elm == RTEMS_RB_LEFT(RTEMS_RB_PARENT(elm, field), field)))\
				elm = RTEMS_RB_PARENT(elm, field);		\
			elm = RTEMS_RB_PARENT(elm, field);			\
		}							\
	}								\
	return (elm);							\
}

#define RTEMS_RB_GENERATE_MINMAX(name, type, field, attr)			\
attr struct type *							\
name##_RTEMS_RB_MINMAX(struct name *head, int val)				\
{									\
	struct type *tmp = RTEMS_RB_ROOT(head);				\
	struct type *parent = NULL;					\
	while (tmp) {							\
		parent = tmp;						\
		if (val < 0)						\
			tmp = RTEMS_RB_LEFT(tmp, field);			\
		else							\
			tmp = RTEMS_RB_RIGHT(tmp, field);			\
	}								\
	return (parent);						\
}

#define	RTEMS_RB_GENERATE_REINSERT(name, type, field, cmp, attr)		\
attr struct type *							\
name##_RTEMS_RB_REINSERT(struct name *head, struct type *elm)			\
{									\
	struct type *cmpelm;						\
	if (((cmpelm = RTEMS_RB_PREV(name, head, elm)) != NULL &&		\
	    cmp(cmpelm, elm) >= 0) ||					\
	    ((cmpelm = RTEMS_RB_NEXT(name, head, elm)) != NULL &&		\
	    cmp(elm, cmpelm) >= 0)) {					\
		/* XXXLAS: Remove/insert is heavy handed. */		\
		RTEMS_RB_REMOVE(name, head, elm);				\
		return (RTEMS_RB_INSERT(name, head, elm));			\
	}								\
	return (NULL);							\
}									\

#define RTEMS_RB_NEGINF	-1
#define RTEMS_RB_INF	1

#define RTEMS_RB_INSERT(name, x, y)	name##_RTEMS_RB_INSERT(x, y)
#define RTEMS_RB_REMOVE(name, x, y)	name##_RTEMS_RB_REMOVE(x, y)
#define RTEMS_RB_FIND(name, x, y)	name##_RTEMS_RB_FIND(x, y)
#define RTEMS_RB_NFIND(name, x, y)	name##_RTEMS_RB_NFIND(x, y)
#define RTEMS_RB_NEXT(name, x, y)	name##_RTEMS_RB_NEXT(y)
#define RTEMS_RB_PREV(name, x, y)	name##_RTEMS_RB_PREV(y)
#define RTEMS_RB_MIN(name, x)		name##_RTEMS_RB_MINMAX(x, RTEMS_RB_NEGINF)
#define RTEMS_RB_MAX(name, x)		name##_RTEMS_RB_MINMAX(x, RTEMS_RB_INF)
#define RTEMS_RB_REINSERT(name, x, y)	name##_RTEMS_RB_REINSERT(x, y)

#define RTEMS_RB_FOREACH(x, name, head)					\
	for ((x) = RTEMS_RB_MIN(name, head);					\
	     (x) != NULL;						\
	     (x) = name##_RTEMS_RB_NEXT(x))

#define RTEMS_RB_FOREACH_FROM(x, name, y)					\
	for ((x) = (y);							\
	    ((x) != NULL) && ((y) = name##_RTEMS_RB_NEXT(x), (x) != NULL);	\
	     (x) = (y))

#define RTEMS_RB_FOREACH_SAFE(x, name, head, y)				\
	for ((x) = RTEMS_RB_MIN(name, head);					\
	    ((x) != NULL) && ((y) = name##_RTEMS_RB_NEXT(x), (x) != NULL);	\
	     (x) = (y))

#define RTEMS_RB_FOREACH_REVERSE(x, name, head)				\
	for ((x) = RTEMS_RB_MAX(name, head);					\
	     (x) != NULL;						\
	     (x) = name##_RTEMS_RB_PREV(x))

#define RTEMS_RB_FOREACH_REVERSE_FROM(x, name, y)				\
	for ((x) = (y);							\
	    ((x) != NULL) && ((y) = name##_RTEMS_RB_PREV(x), (x) != NULL);	\
	     (x) = (y))

#define RTEMS_RB_FOREACH_REVERSE_SAFE(x, name, head, y)			\
	for ((x) = RTEMS_RB_MAX(name, head);					\
	    ((x) != NULL) && ((y) = name##_RTEMS_RB_PREV(x), (x) != NULL);	\
	     (x) = (y))

#endif	/* _RTEMS_SCORE_BSD_TREE_H_ */
