#ifndef	_LINUX_RBTREE_H
#define	_LINUX_RBTREE_H

#include <stddef.h>

struct rb_node {
	struct rb_node *rb_left;
	struct rb_node *rb_right;
	struct rb_node *rb_parent;
	int rb_color;
};

struct rb_root {
	struct rb_node *rb_node;
};
#define RB_ROOT ((struct rb_root){0})
#define rb_entry(p, container, field)		\
	((container *) ((char *)p - offsetof(container, field)))

#define RB_BLACK	0
#define RB_RED		1


extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

extern struct rb_node *rb_next(struct rb_node *);
extern struct rb_node *rb_prev(struct rb_node *);
extern struct rb_node *rb_first(struct rb_root *);
extern struct rb_node *rb_last(struct rb_root *);

extern void rb_replace_node(struct rb_node *victim, struct rb_node *new, 
			    struct rb_root *root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
				struct rb_node ** rb_link)
{
	node->rb_parent = parent;
	node->rb_color = RB_RED;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static inline struct rb_node *rb_parent(struct rb_node * node)
{
	return node->rb_parent;
}

#endif	/* _LINUX_RBTREE_H */
