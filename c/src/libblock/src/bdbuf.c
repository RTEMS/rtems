/*
 * Disk I/O buffering
 * Buffer managment
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Peterburg, Russia
 * Author: Andrey G. Ivanov <Andrey.Ivanov@oktet.ru>
 *         Victor V. Vengerov <vvv@oktet.ru>
 *
 * @(#) $Id$
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include "rtems/bdbuf.h"

/* Fatal errors: */
#define BLKDEV_FATAL_ERROR(n) (('B' << 24) | ((n) & 0x00FFFFFF))
#define BLKDEV_FATAL_BDBUF_CONSISTENCY BLKDEV_FATAL_ERROR(1)
#define BLKDEV_FATAL_BDBUF_SWAPOUT     BLKDEV_FATAL_ERROR(2)

enum balance_factor {BF_LEFT = -1, 
                     BF_NONE = 0, 
                     BF_RIGHT = 1}; 


#define SWAPOUT_PRIORITY 15
#define SWAPOUT_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE * 2)

static rtems_task bdbuf_swapout_task(rtems_task_argument unused);

/*
 * The groups of the blocks with the same size are collected in the
 * bd_pool. Note that a several of the buffer's groups with the
 * same size can exists.
 */
typedef struct bdbuf_pool
{
    bdbuf_buffer *tree;         /* Buffer descriptor lookup AVL tree root */

    Chain_Control free;         /* Free buffers list */
    Chain_Control lru;          /* Last recently used list */

    int           blksize;      /* The size of the blocks (in bytes) */
    int           nblks;        /* Number of blocks in this pool */
    rtems_id      bufget_sema;  /* Buffer obtain counting semaphore */
    void         *mallocd_bufs; /* Pointer to the malloc'd buffer memory,
                                   or NULL, if buffer memory provided in
                                   buffer configuration */
    bdbuf_buffer *bdbufs;       /* Pointer to table of buffer descriptors
                                   allocated for this buffer pool. */
} bdbuf_pool;

/* Buffering layer context definition */
struct bdbuf_context {
    bdbuf_pool    *pool;         /* Table of buffer pools */
    int            npools;       /* Number of entries in pool table */
    
    Chain_Control  mod;          /* Modified buffers list */
    rtems_id       flush_sema;   /* Buffer flush semaphore; counting 
                                    semaphore; incremented when buffer
                                    flushed to the disk; decremented when 
                                    buffer modified */
    rtems_id       swapout_task; /* Swapout task ID */
};

/* Block device request with a single buffer provided */
typedef struct blkdev_request1 {
    blkdev_request   req;
    blkdev_sg_buffer sg[1];
} blkdev_request1;

/* The static context of buffering layer */
static struct bdbuf_context bd_ctx;

#define SAFE
#ifdef SAFE
typedef rtems_mode preemption_key;

#define DISABLE_PREEMPTION(key) \
    do {                                                               \
        rtems_task_mode(RTEMS_PREEMPT_MASK, RTEMS_NO_PREEMPT, &(key)); \
    } while (0)

#define ENABLE_PREEMPTION(key) \
    do {                                                        \
        rtems_mode temp;                                        \
        rtems_task_mode(RTEMS_PREEMPT_MASK, (key), &temp);      \
    } while (0)

#else
 
typedef boolean preemption_key;

#define DISABLE_PREEMPTION(key) \
    do {                                             \
        (key) = _Thread_Executing->is_preemptible;   \
        _Thread_Executing->is_preemptible = 0;       \
    } while (0)
    
#define ENABLE_PREEMPTION(key) \
    do {                                             \
        _Thread_Executing->is_preemptible = (key);   \
        if (_Thread_Evaluate_mode())                 \
            _Thread_Dispatch();                      \
    } while (0)

#endif


#ifdef BINARY_TREE
static bdbuf_buffer *
avl_search(bdbuf_buffer **root, dev_t dev, blkdev_bnum block)
{
    bdbuf_buffer *p = *root;
    while ((p != NULL) && ((p->dev != dev) || (p->block != block)))
    {
        if ((p->dev < dev) || ((p->dev == dev) && (p->block < block)))
            p = p->avl.right;
        else
            p = p->avl.left;
    }
    return p;
}

static bdbuf_buffer *
avl_search_for_sync(bdbuf_buffer **root, disk_device *dd)
{
    bdbuf_buffer *p = *root;
    bdbuf_buffer *s[32];
    bdbuf_buffer **sp = s;
    dev_t dev = dd->phys_dev->dev;
    blkdev_bnum b = dd->start;
    blkdev_bnum e = b + dd->size - 1;
    
    while (p != NULL)
    {
        if ((p->dev < dev) || ((p->dev == dev) && (p->block < b)))
            p = p->avl.right;
        else if ((p->dev > dev) || ((p->dev == dev) && (p->block > e)))
            p = p->avl.left;
        else if (p->modified)
            return p;
        else
        {
            if (p->avl.right != NULL)
                *sp++ = p->avl.right;
            p = p->avl.left;
        }
        if ((p == NULL) && (sp > s))
            p = *--sp;
    }
    return p;
}

static int
avl_insert(bdbuf_buffer **root, bdbuf_buffer *node)
{
    bdbuf_buffer **r = root;
    node->avl.left = node->avl.right = NULL;
    while (TRUE)
    {
        bdbuf_buffer *rr = *r;
        if (rr == NULL)
        {
            *r = node;
            return 0;
        }
        else if ((rr->dev < node->dev) || ((rr->dev == node->dev) &&
                                           (rr->block < node->block)))
        {
            r = &rr->avl.right;
        }
        else if ((rr->dev == node->dev) && (rr->block == node->block))
        {
            return -1;
        }
        else
        {
            r = &rr->avl.left;
        }
    }
}

static int
avl_remove(bdbuf_buffer **root, bdbuf_buffer *node)
{
    bdbuf_buffer **p = root;
    dev_t dev = node->dev;
    blkdev_bnum block = node->block;
    
    while ((*p != NULL) && (*p != node))
    {
        if (((*p)->dev < dev) || (((*p)->dev == dev) && ((*p)->block < block)))
            p = &(*p)->avl.right;
        else
            p = &(*p)->avl.left;
    }
    if (*p == NULL)
        return -1;
    
    *p = node->avl.left;
    while (*p != NULL)
        p = &(*p)->avl.right;
    *p = node->avl.right;
    node->avl.left = node->avl.right = NULL;
    return 0;
}

#else

/* The default maximum height of 32 allows for AVL trees having
   between 5,704,880 and 4,294,967,295 nodes, depending on order of
   insertion.  You may change this compile-time constant as you
   wish. */
#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT  32
#endif



/*
 * avl_cmp_node_node --
 *     Compares two avl nodes. Function compares dev/block pairs of
 *     the node1 and node2.
 *
 * PARAMETERS:
 *     node1 - Pointer to the first node to compare
 *     node2 - Pointer to the second node to compare
 *
 * RETURNS:
 *     0 - dev/block of the nodes are equal
 *     1 - dev/block of the second node are less
 *    -1 - dev/block of the first node are less
 */
static inline int 
avl_cmp_node_node(const bdbuf_buffer *const node1,
                  const bdbuf_buffer *const node2)
{
    if (node1->dev < node2->dev)
        return -1;
    else if (node1->dev > node2->dev)
        return +1;
    else if (node1->block < node2->block)
        return -1;
    else if (node1->block > node2->block)
        return +1;
    else
        return 0;
}

/*
 * avl_cmp_node_pattern -
 *     compares the dev/block of the node with specified two.
 *
 * PARAMETERS:
 *     node1     - Pointer to the node to compare
 *     dev/block - The pattern to compare with
 *
 * RETURNS:
 *     0 - dev/block of the node and specified are equal
 *     1 - dev/block specified are less
 *    -1 - dev/block of the first node are less
 *
 */
static inline int 
avl_cmp_node_pattern(const bdbuf_buffer *const node1,
                     dev_t dev,
                     blkdev_bnum block)
{
    if (node1->dev < dev)
        return -1;
    else if (node1->dev > dev)
        return +1;
    else if (node1->block < block)
        return -1;
    else if (node1->block > block)
        return +1;
    else
        return 0;
}


/*
 * avl_search --
 *     Searches for the node with specified dev/block.
 *
 * PARAMETERS:
 *     root - pointer to the root node of the AVL-Tree.
 *     dev, block - search key
 *
 * RETURNS:
 *     NULL if node with specified dev/block not found
 *     non-NULL - pointer to the node with specified dev/block
 */
static bdbuf_buffer *
avl_search(bdbuf_buffer **root, dev_t dev, blkdev_bnum block)
{

    bdbuf_buffer *p = *root;
    while ((p != NULL) && ((p->dev != dev) || (p->block != block)))
    {
        if ((p->dev < dev) || ((p->dev == dev) && (p->block < block)))
            p = p->avl.link[1];
        else
            p = p->avl.link[0];
    }
    return p;
}

/* avl_search_for_sync --
 *     Search in AVL tree for first modified buffer belongs to specified
 *     disk device.
 *
 * PARAMETERS:
 *     root - pointer to tree root
 *     dd - disk device descriptor
 *
 * RETURNS:
 *     Block buffer, or NULL if no modified blocks on specified device
 *     exists.
 */
static bdbuf_buffer *
avl_search_for_sync(bdbuf_buffer **root, disk_device *dd)
{
    bdbuf_buffer *p = *root;
    bdbuf_buffer *s[AVL_MAX_HEIGHT];
    bdbuf_buffer **sp = s;
    dev_t dev = dd->phys_dev->dev;
    blkdev_bnum b = dd->start;
    blkdev_bnum e = b + dd->size - 1;
    
    while (p != NULL)
    {
        if ((p->dev < dev) || ((p->dev == dev) && (p->block < b)))
            p = p->avl.link[1];
        else if ((p->dev > dev) || ((p->dev == dev) && (p->block > e)))
            p = p->avl.link[0];
        else if (p->modified)
            return p;
        else
        {
            if (p->avl.link[1] != NULL)
                *sp++ = p->avl.link[1];
            p = p->avl.link[0];
        }
        if ((p == NULL) && (sp > s))
            p = *--sp;
    }
    return p;
}


/*
 * avl_insert --
 *     Inserts the specified node to the AVl-Tree.
 *
 * PARAMETERS:
 *     root_addr - Pointer to pointer to the root node
 *     node - Pointer to the node to add.
 *
 * RETURNS:
 *     0 - The node added successfully
 *    -1 - An error occured
 */
static int
avl_insert (bdbuf_buffer **root_addr, bdbuf_buffer *node)
{
    /* Uses Knuth's Algorithm 6.2.3A (balanced tree search and
       insertion), but caches results of comparisons.  In empirical
       tests this eliminates about 25% of the comparisons seen under
       random insertions.  */

  /* A1. */
    int t_modified = 0;
    bdbuf_buffer *t;
    bdbuf_buffer *s, *p, *q, *r;

    bdbuf_buffer *root_link = *root_addr;;

    t = root_link;
    s = p = t;

    if (s == NULL)
    {
        q = t = node;
        q->avl.link[0] = q->avl.link[1] = NULL;
        q->avl.bal = 0;
        *root_addr = t;
        return 0;
    }

    for (;;)
    {
        /* A2. */
        int diff = avl_cmp_node_node(node, p);

      /* A3. */
        if (diff < 0)
        {
            p->avl.cache = 0;
            q = p->avl.link[0];
            if (q == NULL)
            {
                p->avl.link[0] = q = node;
                break;
            }
        }
        /* A4. */
        else 
            if (diff > 0)
            {
                p->avl.cache = 1;
                q = p->avl.link[1];
                if (q == NULL)
                {
                    p->avl.link[1] = q = node;
                    break;
                }
            }
            else
                /* A2. */
            {
                /*
                 * The item found. Nothing changed. Have not to update 
                 * root_adr*/

                return -1;
            }

        /* A3, A4. */
        if (q->avl.bal != 0)
        {
            t = p, s = q;
            t_modified = 1;
        }
        p = q;
    }
  
    /* A5. */
    q->avl.link[0] = q->avl.link[1] = NULL;
    q->avl.bal = 0;

    /* A6. */
    r = p = s->avl.link[(int) s->avl.cache];
    while (p != q)
    {
        p->avl.bal = p->avl.cache * 2 - 1;
        p = p->avl.link[(int) p->avl.cache];
    }

    /* A7. */
    if (s->avl.cache == 0)
    {
        /* a = -1. */
        if (s->avl.bal == 0)
        {
            s->avl.bal = -1;
            *root_addr = root_link;
            return 0;
        }
        else if (s->avl.bal == +1)
        {
            s->avl.bal = 0;
            *root_addr = root_link;
            return 0;
        }
      
        assert (s->avl.bal == -1);
        if (r->avl.bal == -1)
        {
            /* A8. */
            p = r;
            s->avl.link[0] = r->avl.link[1];
            r->avl.link[1] = s;
            s->avl.bal = r->avl.bal = 0;
        }
        else
        {
            /* A9. */
            assert(r->avl.bal == +1);
            p = r->avl.link[1];
            r->avl.link[1] = p->avl.link[0];
            p->avl.link[0] = r;
            s->avl.link[0] = p->avl.link[1];
            p->avl.link[1] = s;
            if (p->avl.bal == -1)
                s->avl.bal = 1, r->avl.bal = 0;
            else 
            {
                if (p->avl.bal == 0)
                {
                    s->avl.bal = r->avl.bal = 0;
                }
                else 
                {
                    assert (p->avl.bal == +1);
                    s->avl.bal = 0, r->avl.bal = -1;
                }
            }
            p->avl.bal = 0;
        }
    }
    else
    {
        /* a == +1. */
        if (s->avl.bal == 0)
        {
            s->avl.bal = 1;
            *root_addr = root_link;
            return 0;
        } 
        else if (s->avl.bal == -1)
        {
            s->avl.bal = 0;
            *root_addr = root_link;
            return 0;
        }

        assert(s->avl.bal == +1);
        if (r->avl.bal == +1)
        {
            /* A8. */
            p = r;
            s->avl.link[1] = r->avl.link[0];
            r->avl.link[0] = s;
            s->avl.bal = r->avl.bal = 0;
        }
        else
        {
            /* A9. */
            assert(r->avl.bal == -1);
            p = r->avl.link[0];
            r->avl.link[0] = p->avl.link[1];
            p->avl.link[1] = r;
            s->avl.link[1] = p->avl.link[0];
            p->avl.link[0] = s;
            if (p->avl.bal == +1)
            {
                s->avl.bal = -1, r->avl.bal = 0;
            }
            else 
            {
                if (p->avl.bal == 0)
                {
                    s->avl.bal = r->avl.bal = 0;
                }
                else 
                {
                    assert(p->avl.bal == -1);
                    s->avl.bal = 0, r->avl.bal = 1;
                }
            }
            p->avl.bal = 0;
        }
    }
                
    /* A10. */
    if (t_modified)
    {
        if (s == t->avl.link[1])
            t->avl.link[1] = p;
        else
            t->avl.link[0] = p;
    }
    else
    {
        root_link = p;
    }

    *root_addr = root_link;
    return 0;
}


/* avl_remove --
 *     removes the node from the tree.
 *
 * PARAMETERS:
 *     root_addr - Pointer to pointer to the root node
 *     node      - Pointer to the node to remove
 *
 * RETURNS:
 *     0 - Item removed
 *    -1 - No such item found
 */
static int
avl_remove(bdbuf_buffer **root_addr, const bdbuf_buffer *node)
{
    /* Uses my Algorithm D, which can be found at
       http://www.msu.edu/user/pfaffben/avl.  Algorithm D is based on
       Knuth's Algorithm 6.2.2D (Tree deletion) and 6.2.3A (Balanced
       tree search and insertion), as well as the notes on pages 465-466
       of Vol. 3. */

  /* D1. */
    bdbuf_buffer *pa[AVL_MAX_HEIGHT];           /* Stack P: Nodes. */
    char a[AVL_MAX_HEIGHT];             /* Stack P: Bits. */
    int k = 1;                          /* Stack P: Pointer. */
  
    bdbuf_buffer **q;
    bdbuf_buffer *p;


    /*
     * To avoid using unnessary instance of the 'bdbuf_buffer' (as pa[0])
     * we will catch all access to pa[0] and use &root_avl instead
     */
    struct bdbuf_avl_node root_avl;

    root_avl.link[0] = *root_addr;
    root_avl.link[1] = NULL;
    root_avl.bal = 0;
    root_avl.cache = 0;

    a[0] = 0;

    p = root_avl.link[0];


    k = 1;

    for (;;)
    {
        /* D2. */
        int diff;

        if (p == NULL)
            return -1;

        diff = avl_cmp_node_node(node, p);

        if (diff == 0)
            break;

      /* D3, D4. */
        pa[k] = p;
        if (diff < 0)
        {
            p = p->avl.link[0];
            a[k] = 0;
        }
        else if (diff > 0)
        {
            p = p->avl.link[1];
            a[k] = 1;
        }
        k++;
    }

    /* D5. */
    if (k == 1)
    {
        /* Andron */
        q = &root_avl.link[(int) a[k - 1]];
    }
    else
    {
        q = &pa[k - 1]->avl.link[(int) a[k - 1]];
    }
    if (p->avl.link[1] == NULL)
    {
        *q = p->avl.link[0];
        if (*q)
            (*q)->avl.bal = 0;
    }
    else
    {
        /* D6. */
        bdbuf_buffer *r = p->avl.link[1];
        if (r->avl.link[0] == NULL)
        {
            r->avl.link[0] = p->avl.link[0];
            *q = r;
            r->avl.bal = p->avl.bal;
            a[k] = 1;
            pa[k++] = r;
        }
        else
        {
            /* D7. */
            bdbuf_buffer *s = r->avl.link[0];
            int l = k++;

            a[k] = 0;
            pa[k++] = r;
            
            /* D8. */
            while (s->avl.link[0] != NULL)
            {
                r = s;
                s = r->avl.link[0];
                a[k] = 0;
                pa[k++] = r;
            }

            /* D9. */
            a[l] = 1;
            pa[l] = s;
            s->avl.link[0] = p->avl.link[0];
            r->avl.link[0] = s->avl.link[1];
            s->avl.link[1] = p->avl.link[1];
            s->avl.bal = p->avl.bal;
            *q = s;
        }
    }

    assert(k > 0);
    /* D10. */
    while (--k)
    {
        bdbuf_buffer *s = pa[k], *r;

        if (a[k] == 0)
        {
            /* D10. */
            if (s->avl.bal == -1)
            {
                s->avl.bal = 0;
                continue;
            }
            else if (s->avl.bal == 0)
            { 
                s->avl.bal = 1;
                break;
            }

            assert(s->avl.bal == +1);
            r = s->avl.link[1];

            assert(r != NULL);
            if (r->avl.bal == 0)
            {
                /* D11. */
                s->avl.link[1] = r->avl.link[0];
                r->avl.link[0] = s;
                r->avl.bal = -1;
                if (k == 1)
                {
                    /* Andron */
                    root_avl.link[(int) a[k - 1]] = r;
                }
                else
                {
                    pa[k - 1]->avl.link[(int) a[k - 1]] = r;
                }
                break;
            }
            else 
                if (r->avl.bal == +1)
                {
                    /* D12. */
                    s->avl.link[1] = r->avl.link[0];
                    r->avl.link[0] = s;
                    s->avl.bal = r->avl.bal = 0;
                    if (k == 1)
                    {
                        /* Andron */
                        root_avl.link[(int) a[k - 1]] = r;
                    }
                    else
                    {
                        pa[k - 1]->avl.link[(int) a[k - 1]] = r;
                    }
                }
                else 
                {
                    /* D13. */
                    assert(r->avl.bal == -1);
                    p = r->avl.link[0];
                    r->avl.link[0] = p->avl.link[1];
                    p->avl.link[1] = r;
                    s->avl.link[1] = p->avl.link[0];
                    p->avl.link[0] = s;
                    if (p->avl.bal == +1)
                        s->avl.bal = -1, r->avl.bal = 0;
                    else if (p->avl.bal == 0)
                    {
                        s->avl.bal = r->avl.bal = 0;
                    }
                    else
                    {
                        assert(p->avl.bal == -1);
                        s->avl.bal = 0, r->avl.bal = +1;
                    }
                    p->avl.bal = 0;
                    if (k == 1)
                    {
                        /* Andron */
                        root_avl.link[(int) a[k - 1]] = p;
                    }
                    else
                    {
                        pa[k - 1]->avl.link[(int) a[k - 1]] = p;
                    }
                }
        }
        else
        {
            assert(a[k] == 1);

            /* D10. */
            if (s->avl.bal == +1)
            {
                s->avl.bal = 0;
                continue;
            }
            else 
                if (s->avl.bal == 0)
                {
                    s->avl.bal = -1;
                    break;
                }

            assert(s->avl.bal == -1);
            r = s->avl.link[0];

            if (r == NULL || r->avl.bal == 0)
            {
                /* D11. */
                s->avl.link[0] = r->avl.link[1];
                r->avl.link[1] = s;
                r->avl.bal = 1;
                if (k == 1)
                {
                    /* Andron */
                    root_avl.link[(int) a[k - 1]] = r;
                }
                else
                {
                    pa[k - 1]->avl.link[(int) a[k - 1]] = r;
                }

                break;
            }
            else 
                if (r->avl.bal == -1)
                {
                    /* D12. */
                    s->avl.link[0] = r->avl.link[1];
                    r->avl.link[1] = s;
                    s->avl.bal = r->avl.bal = 0;
                    if (k == 1)
                    {
                        root_avl.link[(int) a[k - 1]] = r;
                    }
                    else
                    {
                        pa[k - 1]->avl.link[(int) a[k - 1]] = r;
                    }

                }
                else 
                    if (r->avl.bal == +1)
                    {
                        /* D13. */
                        p = r->avl.link[1];
                        r->avl.link[1] = p->avl.link[0];
                        p->avl.link[0] = r;
                        s->avl.link[0] = p->avl.link[1];
                        p->avl.link[1] = s;
                        if (p->avl.bal == -1)
                            s->avl.bal = 1, r->avl.bal = 0;
                        else 
                            if (p->avl.bal == 0)
                                s->avl.bal = r->avl.bal = 0;
                            else
                            {
                                assert(p->avl.bal == 1);
                                s->avl.bal = 0, r->avl.bal = -1;
                            }
                        p->avl.bal = 0;
                        if (k == 1)
                        {
                            /* Andron */
                            root_avl.link[(int) a[k - 1]] = p;
                        }
                        else
                        {
                            pa[k - 1]->avl.link[(int) a[k - 1]] = p;
                        }
                    }
        }
    }

    *root_addr = root_avl.link[0];
    return 0;
}

#endif 

/* bdbuf_initialize_pool --
 *      Initialize single buffer pool.
 *
 * PARAMETERS:
 *     config - buffer pool configuration
 *     pool   - pool number
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL, if buffer pool initialized successfully, or error
 *     code if error occured.
 */
static rtems_status_code
bdbuf_initialize_pool(rtems_bdbuf_config *config, int pool)
{
    bdbuf_pool *p = bd_ctx.pool + pool;
    unsigned char *bufs;
    bdbuf_buffer *b;
    rtems_status_code rc;
    int i;
    
    p->blksize = config->size;
    p->nblks = config->num;
    p->tree = NULL;
    
    Chain_Initialize_empty(&p->free);
    Chain_Initialize_empty(&p->lru);
    
    /* Allocate memory for buffer descriptors */
    p->bdbufs = calloc(config->num, sizeof(bdbuf_buffer));
    if (p->bdbufs == NULL)
    {
        return RTEMS_NO_MEMORY;
    }
    
    /* Allocate memory for buffers if required */
    if (config->mem_area == NULL)
    {
        bufs = p->mallocd_bufs = malloc(config->num * config->size);
        if (bufs == NULL)
        {
            free(p->bdbufs);
            return RTEMS_NO_MEMORY;
        }
    }
    else
    {
        bufs = config->mem_area;
        p->mallocd_bufs = NULL;
    }
    
    for (i = 0, b = p->bdbufs; i < p->nblks; i++, b++, bufs += p->blksize)
    {
        b->dev = -1; b->block = 0;
        b->buffer = bufs;
        b->actual = b->modified = b->in_progress = FALSE;
        b->use_count = 0;
        b->pool = pool;
        _Chain_Append(&p->free, &b->link);
    }
    
    rc = rtems_semaphore_create(
        rtems_build_name('B', 'U', 'F', 'G'),
        p->nblks,
        RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY |
        RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL,
        0,
        &p->bufget_sema);
    
    if (rc != RTEMS_SUCCESSFUL)
    {
        free(p->bdbufs);
        free(p->mallocd_bufs);
        return rc;
    }
    
    return RTEMS_SUCCESSFUL;
}

/* bdbuf_release_pool --
 *     Free resources allocated for buffer pool with specified number.
 *
 * PARAMETERS:
 *     pool - buffer pool number
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
static rtems_status_code
bdbuf_release_pool(rtems_bdpool_id pool)
{
    bdbuf_pool *p = bd_ctx.pool + pool;
    rtems_semaphore_delete(p->bufget_sema);
    free(p->bdbufs);
    free(p->mallocd_bufs);
    return RTEMS_SUCCESSFUL;
}

/* rtems_bdbuf_init --
 *     Prepare buffering layer to work - initialize buffer descritors 
 *     and (if it is neccessary)buffers. Buffers will be allocated accoriding
 *     to the configuration table, each entry describes kind of block and 
 *     amount requested. After initialization all blocks is placed into
 *     free elements lists.
 *
 * PARAMETERS:
 *     conf_table - pointer to the buffers configuration table
 *     size       - number of entries in configuration table
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_init(rtems_bdbuf_config *conf_table, int size)
{
    rtems_bdpool_id i;
    rtems_status_code rc;

    if (size <= 0)
        return RTEMS_INVALID_SIZE;
    
    bd_ctx.npools = size;

    /*
     * Allocate memory for buffer pool descriptors
     */
    bd_ctx.pool = calloc(size, sizeof(bdbuf_pool));
    if (bd_ctx.pool == NULL)
    {
        return RTEMS_NO_MEMORY;
    }

    Chain_Initialize_empty(&bd_ctx.mod);
    
    /* Initialize buffer pools and roll out if something failed */
    for (i = 0; i < size; i++)
    {
        rc = bdbuf_initialize_pool(conf_table + i, i);
        if (rc != RTEMS_SUCCESSFUL)
        {
             rtems_bdpool_id j;
             for (j = 0; j < i - 1; j++)
             {
                 bdbuf_release_pool(j);
             }
             return rc;
        }
    }

    /* Create buffer flush semaphore */
    rc = rtems_semaphore_create(
        rtems_build_name('B', 'F', 'L', 'U'), 0,
        RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY |
        RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL, 0,
        &bd_ctx.flush_sema);
    if (rc != RTEMS_SUCCESSFUL)
    {
        for (i = 0; i < size; i++)
            bdbuf_release_pool(i);
        free(bd_ctx.pool);
        return rc;
    }
    
    /* Create and start swapout task */
    rc = rtems_task_create(
        rtems_build_name('B', 'S', 'W', 'P'),
        SWAPOUT_PRIORITY,
        SWAPOUT_STACK_SIZE, 
        RTEMS_DEFAULT_MODES | RTEMS_NO_PREEMPT,
        RTEMS_DEFAULT_ATTRIBUTES,
        &bd_ctx.swapout_task);
    if (rc != RTEMS_SUCCESSFUL)
    {
        rtems_semaphore_delete(bd_ctx.flush_sema);
        for (i = 0; i < size; i++)
            bdbuf_release_pool(i);
        free(bd_ctx.pool);
        return rc;
    }

    rc = rtems_task_start(bd_ctx.swapout_task, bdbuf_swapout_task, 0);
    if (rc != RTEMS_SUCCESSFUL)
    {
        rtems_task_delete(bd_ctx.swapout_task);
        rtems_semaphore_delete(bd_ctx.flush_sema);
        for (i = 0; i < size; i++)
            bdbuf_release_pool(i);
        free(bd_ctx.pool);
        return rc;
    }

    return RTEMS_SUCCESSFUL;
}

/* find_or_assign_buffer --
 *     Looks for buffer already assigned for this dev/block. If one is found
 *     obtain block buffer. If specified block already cached (i.e. there's
 *     block in the _modified_, or _recently_used_), return address
 *     of appropriate buffer descriptor and increment reference counter to 1. 
 *     If block is not cached, allocate new buffer and return it. Data 
 *     shouldn't be read to the buffer from media; buffer contains arbitrary 
 *     data. This primitive may be blocked if there are no free buffer 
 *     descriptors available and there are no unused non-modified (or 
 *     synchronized with media) buffers available.
 *
 * PARAMETERS:
 *     device - device number (constructed of major and minor device number
 *     block  - linear media block number
 *     ret_buf - address of the variable to store address of found descriptor
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFEECTS:
 *     bufget_sema may be obtained by this primitive
 *
 * NOTE:
 *     It is assumed that primitive invoked when thread preemption is disabled.
 */
static rtems_status_code
find_or_assign_buffer(disk_device *dd,
                      blkdev_bnum block, 
                      bdbuf_buffer **ret_buf)
{
    bdbuf_buffer *bd_buf;
    bdbuf_pool   *bd_pool;
    rtems_status_code rc;
    dev_t         device;
    ISR_Level     level;

    int blksize;

    device = dd->dev;
    bd_pool = bd_ctx.pool + dd->pool;
    blksize = dd->block_size;

again:
    /* Looking for buffer descriptor used for this dev/block. */
    bd_buf = avl_search(&bd_pool->tree, device, block);

    if (bd_buf == NULL)
    {
        /* Try to obtain semaphore without waiting first. It is the most
           frequent case when reasonable number of buffers configured. If 
           it is failed, obtain semaphore blocking on it. In this case 
           it should be checked that appropriate buffer hasn't been loaded 
           by another thread, because this thread is preempted */
        rc = rtems_semaphore_obtain(bd_pool->bufget_sema, RTEMS_NO_WAIT, 0);
        if (rc == RTEMS_UNSATISFIED)
        {
            rc = rtems_semaphore_obtain(bd_pool->bufget_sema,
                                        RTEMS_WAIT, RTEMS_NO_TIMEOUT);
            bd_buf = avl_search(&bd_pool->tree, device, block);
            if (bd_buf != NULL)
                rtems_semaphore_release(bd_pool->bufget_sema);
        }
    }
    
    if (bd_buf == NULL)
    {
        /* Assign new buffer descriptor */
        if (_Chain_Is_empty(&bd_pool->free))
        {
            bd_buf = (bdbuf_buffer *)Chain_Get(&bd_pool->lru);
            if (bd_buf != NULL)
            {
                int avl_result; 
                avl_result = avl_remove(&bd_pool->tree, bd_buf);
                if (avl_result != 0)
                {
                    rtems_fatal_error_occurred(BLKDEV_FATAL_BDBUF_CONSISTENCY);
                    return RTEMS_INTERNAL_ERROR;
                }
            }
        }
        else
        {
            bd_buf = (bdbuf_buffer *)Chain_Get(&(bd_pool->free));
        }

        if (bd_buf == NULL)
        {
            goto again;
        }
        else
        {
            bd_buf->dev = device;
            bd_buf->block = block;
#ifdef BINARY_TREE
            bd_buf->avl.left = NULL;
            bd_buf->avl.right = NULL;
#else
            bd_buf->avl.link[0] = NULL;
            bd_buf->avl.link[1] = NULL;
#endif
            bd_buf->use_count = 1;
            bd_buf->modified = bd_buf->actual = bd_buf->in_progress = FALSE;
            bd_buf->status = RTEMS_SUCCESSFUL;

            if (avl_insert(&bd_pool->tree, bd_buf) != 0)
            {
                rtems_fatal_error_occurred(BLKDEV_FATAL_BDBUF_CONSISTENCY);
                return RTEMS_INTERNAL_ERROR;
            }

            *ret_buf = bd_buf;

            return RTEMS_SUCCESSFUL;
        }
    }
    else
    {
        /* Buffer descriptor already assigned for this dev/block */
        if (bd_buf->use_count == 0)
        {
            /* If we are removing from lru list, obtain the bufget_sema
             * first. If we are removing from mod list, obtain flush sema.
             * It should be obtained without blocking because we know
             * that our buffer descriptor is in the list. */
            if (bd_buf->modified)
            {
                rc = rtems_semaphore_obtain(bd_ctx.flush_sema, 
                                            RTEMS_NO_WAIT, 0);
            }
            else
            {
                rc = rtems_semaphore_obtain(bd_pool->bufget_sema, 
                                            RTEMS_NO_WAIT, 0);
            }
            /* It is possible that we couldn't obtain flush or bufget sema 
             * although buffer in the appropriate chain is available:
             * semaphore may be released to swapout task, but this task 
             * actually did not start to process it. */
            if (rc == RTEMS_UNSATISFIED)
                rc = RTEMS_SUCCESSFUL;
            if (rc != RTEMS_SUCCESSFUL)
            {
                rtems_fatal_error_occurred(BLKDEV_FATAL_BDBUF_CONSISTENCY);
                return RTEMS_INTERNAL_ERROR;
            }

            /* Buffer descriptor is linked to the lru or mod chain. Remove
               it from there. */
            Chain_Extract(&bd_buf->link);
        }
        bd_buf->use_count++;
        while (bd_buf->in_progress != 0)
        {
            _CORE_mutex_Seize(&bd_buf->transfer_sema, 0, TRUE,
                              WATCHDOG_NO_TIMEOUT, level);
        }
        
        *ret_buf = bd_buf;
        return RTEMS_SUCCESSFUL;
    }
}

/* rtems_bdbuf_get --
 *     Obtain block buffer. If specified block already cached (i.e. there's
 *     block in the _modified_, or _recently_used_), return address
 *     of appropriate buffer descriptor and increment reference counter to 1. 
 *     If block is not cached, allocate new buffer and return it. Data 
 *     shouldn't be read to the buffer from media; buffer may contains 
 *     arbitrary data. This primitive may be blocked if there are no free 
 *     buffer descriptors available and there are no unused non-modified 
 *     (or synchronized with media) buffers available.
 *
 * PARAMETERS:
 *     device - device number (constructed of major and minor device number)
 *     block  - linear media block number
 *     bd     - address of variable to store pointer to the buffer descriptor
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     bufget_sema semaphore obtained by this primitive.
 */
rtems_status_code
rtems_bdbuf_get(dev_t device, blkdev_bnum block, bdbuf_buffer **bd)
{
    rtems_status_code rc;
    disk_device *dd;
    disk_device *pdd;
    preemption_key key;

    /*
     * Convert logical dev/block to physical one
     */
    dd = rtems_disk_lookup(device);
    if (dd == NULL)
        return RTEMS_INVALID_ID;
    
    if (block >= dd->size)
    {
        rtems_disk_release(dd);
        return RTEMS_INVALID_NUMBER;
    }
    
    pdd = dd->phys_dev;
    block += dd->start;
    rtems_disk_release(dd);

    DISABLE_PREEMPTION(key);
    rc = find_or_assign_buffer(pdd, block, bd);
    ENABLE_PREEMPTION(key);

    if (rc != RTEMS_SUCCESSFUL)
        return rc;

    return RTEMS_SUCCESSFUL;
}

/* bdbuf_initialize_transfer_sema --
 *     Initialize transfer_sema mutex semaphore associated with buffer
 *     descriptor.
 */
static inline void
bdbuf_initialize_transfer_sema(bdbuf_buffer *bd_buf)
{
    CORE_mutex_Attributes mutex_attr;
    mutex_attr.lock_nesting_behavior = CORE_MUTEX_NESTING_BLOCKS;
    mutex_attr.only_owner_release = FALSE;
    mutex_attr.discipline = CORE_MUTEX_DISCIPLINES_FIFO;
    mutex_attr.priority_ceiling = 0;

    _CORE_mutex_Initialize(&bd_buf->transfer_sema, OBJECTS_NO_CLASS,
                           &mutex_attr, CORE_MUTEX_LOCKED, NULL);
}

/* bdbuf_write_transfer_done --
 *     Callout function. Invoked by block device driver when data transfer
 *     to device (write) is completed. This function may be invoked from
 *     interrupt handler.
 *
 * PARAMETERS:
 *     arg    - arbitrary argument specified in block device request
 *              structure (in this case - pointer to the appropriate
 *              bdbuf_buffer buffer descriptor structure).
 *     status - I/O completion status
 *     error  - errno error code if status != RTEMS_SUCCESSFUL
 *
 * RETURNS:
 *     none
 */
static void
bdbuf_write_transfer_done(void *arg, rtems_status_code status, int error)
{
    bdbuf_buffer *bd_buf = arg;
    bd_buf->status = status;
    bd_buf->error = error;
    bd_buf->in_progress = bd_buf->modified = FALSE;
    _CORE_mutex_Surrender(&bd_buf->transfer_sema, 0, NULL);
    _CORE_mutex_Flush(&bd_buf->transfer_sema, NULL, 
                      CORE_MUTEX_STATUS_SUCCESSFUL);
}

/* bdbuf_read_transfer_done --
 *     Callout function. Invoked by block device driver when data transfer
 *     from device (read) is completed. This function may be invoked from
 *     interrupt handler.
 *
 * PARAMETERS:
 *     arg    - arbitrary argument specified in block device request
 *              structure (in this case - pointer to the appropriate
 *              bdbuf_buffer buffer descriptor structure).
 *     status - I/O completion status
 *     error  - errno error code if status != RTEMS_SUCCESSFUL
 *
 * RETURNS:
 *     none
 */
static void
bdbuf_read_transfer_done(void *arg, rtems_status_code status, int error)
{
    bdbuf_buffer *bd_buf = arg;
    bd_buf->status = status;
    bd_buf->error = error;
    _CORE_mutex_Surrender(&bd_buf->transfer_sema, 0, NULL);
    _CORE_mutex_Flush(&bd_buf->transfer_sema, NULL, 
                      CORE_MUTEX_STATUS_SUCCESSFUL);
}

/* rtems_bdbuf_read --
 *     (Similar to the rtems_bdbuf_get, except reading data from media)
 *     Obtain block buffer. If specified block already cached, return address
 *     of appropriate buffer and increment reference counter to 1. If block is
 *     not cached, allocate new buffer and read data to it from the media.
 *     This primitive may be blocked on waiting until data to be read from
 *     media, if there are no free buffer descriptors available and there are
 *     no unused non-modified (or synchronized with media) buffers available.
 *
 * PARAMETERS:
 *     device - device number (consists of major and minor device number)
 *     block  - linear media block number
 *     bd     - address of variable to store pointer to the buffer descriptor
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     bufget_sema and transfer_sema semaphores obtained by this primitive.
 */
rtems_status_code
rtems_bdbuf_read(dev_t device, 
                 blkdev_bnum block,
                 bdbuf_buffer **bd)
{
    preemption_key key;
    ISR_Level level;
    
    bdbuf_buffer *bd_buf;
    rtems_status_code rc;
    int result;
    disk_device *dd;
    disk_device *pdd;
    blkdev_request1 req;

    dd = rtems_disk_lookup(device);
    if (dd == NULL)
        return RTEMS_INVALID_ID;
     
    if (block >= dd->size)
    {
        rtems_disk_release(dd);
        return RTEMS_INVALID_NUMBER;
    }
    
    pdd = dd->phys_dev;
    block += dd->start;

    DISABLE_PREEMPTION(key);
    rc = find_or_assign_buffer(pdd, block, &bd_buf);

    if (rc != RTEMS_SUCCESSFUL)
    {
        ENABLE_PREEMPTION(key);
        rtems_disk_release(dd);
        return rc;
    }

    if (!bd_buf->actual)
    {
        bd_buf->in_progress = 1;

        req.req.req = BLKDEV_REQ_READ;
        req.req.req_done = bdbuf_read_transfer_done;
        req.req.done_arg = bd_buf;
        req.req.start = block;
        req.req.count = 1;
        req.req.bufnum = 1;
        req.req.bufs[0].length = dd->block_size;
        req.req.bufs[0].buffer = bd_buf->buffer;
        
        bdbuf_initialize_transfer_sema(bd_buf);
        result = dd->ioctl(device, BLKIO_REQUEST, &req);
        if (result == -1)
        {
            bd_buf->status = RTEMS_IO_ERROR;
            bd_buf->error = errno;
            bd_buf->actual = FALSE;
        }
        else
        {
            _CORE_mutex_Seize(&bd_buf->transfer_sema, 0, TRUE,
                              WATCHDOG_NO_TIMEOUT, level);
            bd_buf->actual = TRUE;
        }
        bd_buf->in_progress = FALSE;
    }
    rtems_disk_release(dd);
    
    ENABLE_PREEMPTION(key);

    *bd = bd_buf;
           
    return RTEMS_SUCCESSFUL;
}


/* bdbuf_release --
 *     Release buffer. Decrease buffer usage counter. If it is zero, further
 *     processing depends on modified attribute. If buffer was modified, it
 *     is inserted into mod chain and swapout task waken up. If buffer was
 *     not modified, it is returned to the end of lru chain making it available
 *     for further use.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the released buffer descriptor.
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if buffer released successfully, or error code if
 *     error occured.
 *
 * NOTE:
 *     This is internal function. It is assumed that task made non-preemptive
 *     before its invocation.
 */
static rtems_status_code
bdbuf_release(bdbuf_buffer *bd_buf)
{
    bdbuf_pool *bd_pool;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    if (bd_buf->use_count <= 0)
        return RTEMS_INTERNAL_ERROR;
    
    bd_pool = bd_ctx.pool + bd_buf->pool;

    bd_buf->use_count--;

    if (bd_buf->use_count == 0)
    {
        if (bd_buf->modified)
        {
            
            /* Buffer was modified. Insert buffer to the modified buffers
             * list and initiate flushing. */
            Chain_Append(&bd_ctx.mod, &bd_buf->link);

            /* Release the flush_sema */
            rc = rtems_semaphore_release(bd_ctx.flush_sema);
        }
        else
        {
            /* Buffer was not modified. Add this descriptor to the 
             * end of lru chain and make it available for reuse. */
            Chain_Append(&bd_pool->lru, &bd_buf->link);
            rc = rtems_semaphore_release(bd_pool->bufget_sema);
        }
    }
    return rc;
}


/* rtems_bdbuf_release --
 *     Release buffer allocated before. This primitive decrease the
 *     usage counter. If it is zero, further destiny of buffer depends on
 *     'modified' status. If buffer was modified, it is placed to the end of
 *     mod list and flush task waken up. If buffer was not modified,
 *     it is placed to the end of lru list, and bufget_sema released, allowing
 *     to reuse this buffer.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive. 
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     flush_sema and bufget_sema semaphores may be released by this primitive.
 */
rtems_status_code
rtems_bdbuf_release(bdbuf_buffer *bd_buf)
{
    preemption_key key;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    if (bd_buf == NULL)
        return RTEMS_INVALID_ADDRESS;
    
    DISABLE_PREEMPTION(key);
    
    rc = bdbuf_release(bd_buf);
    
    ENABLE_PREEMPTION(key);
    
    return rc;
}

/* rtems_bdbuf_release_modified --
 *     Release buffer allocated before, assuming that it is _modified_ by
 *     it's owner. This primitive decrease usage counter for buffer, mark 
 *     buffer descriptor as modified. If usage counter is 0, insert it at
 *     end of mod chain and release flush_sema semaphore to activate the
 *     flush task.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     flush_sema semaphore may be released by this primitive.
 */
rtems_status_code
rtems_bdbuf_release_modified(bdbuf_buffer *bd_buf)
{
    preemption_key key;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    if (bd_buf == NULL)
        return RTEMS_INVALID_ADDRESS;
    
    DISABLE_PREEMPTION(key);

    if (!bd_buf->modified)
    {
        bdbuf_initialize_transfer_sema(bd_buf);
    }
    bd_buf->modified = TRUE;
    bd_buf->actual = TRUE;
    rc = bdbuf_release(bd_buf);    
    
    ENABLE_PREEMPTION(key);
    
    return rc;
}

/* rtems_bdbuf_sync --
 *     Wait until specified buffer synchronized with disk. Invoked on exchanges
 *     critical for data consistency on the media. This primitive mark owned
 *     block as modified, decrease usage counter. If usage counter is 0,
 *     block inserted to the mod chain and flush_sema semaphore released.
 *     Finally, primitives blocked on transfer_sema semaphore.
 *
 * PARAMETERS:
 *     bd_buf - pointer to the bdbuf_buffer structure previously obtained using
 *              get/read primitive.
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 *
 * SIDE EFFECTS:
 *     Primitive may be blocked on transfer_sema semaphore.
 */
rtems_status_code
rtems_bdbuf_sync(bdbuf_buffer *bd_buf)
{
    preemption_key key;
    ISR_Level level;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    if (bd_buf == NULL)
        return RTEMS_INVALID_ADDRESS;
    
    DISABLE_PREEMPTION(key);

    if (!bd_buf->modified)
    {
        bdbuf_initialize_transfer_sema(bd_buf);
    }
    bd_buf->modified = TRUE;
    bd_buf->actual = TRUE;

    rc = bdbuf_release(bd_buf);

    if (rc == RTEMS_SUCCESSFUL)
    {
        _CORE_mutex_Seize(&bd_buf->transfer_sema, 0, TRUE,
                          WATCHDOG_NO_TIMEOUT, level);
    }
    
    ENABLE_PREEMPTION(key);
    
    return rc;
}

/* rtems_bdbuf_syncdev --
 *     Synchronize with disk all buffers containing the blocks belonging to
 *     specified device.
 *
 * PARAMETERS:
 *     dev - block device number
 *
 * RETURNS:
 *     RTEMS status code (RTEMS_SUCCESSFUL if operation completed successfully
 *     or error code if error is occured)
 */
rtems_status_code
rtems_bdbuf_syncdev(dev_t dev)
{
    preemption_key key;
    ISR_Level level;
            
    bdbuf_buffer *bd_buf;
    disk_device *dd;
    bdbuf_pool  *pool;
    
    dd = rtems_disk_lookup(dev);
    if (dd == NULL)
        return RTEMS_INVALID_ID;
    
    pool = bd_ctx.pool + dd->pool;
        
    DISABLE_PREEMPTION(key);
    do {
        bd_buf = avl_search_for_sync(&pool->tree, dd);
        if (bd_buf != NULL /* && bd_buf->modified */)
        {
            _CORE_mutex_Seize(&bd_buf->transfer_sema, 0, TRUE,
                              WATCHDOG_NO_TIMEOUT, level);
        }
    } while (bd_buf != NULL);
    ENABLE_PREEMPTION(key);
    return rtems_disk_release(dd);
}

/* bdbuf_swapout_task --
 *     Body of task which take care on flushing modified buffers to the
 *     disk.
 */
static rtems_task 
bdbuf_swapout_task(rtems_task_argument unused)
{
    rtems_status_code rc;
    int result;
    ISR_Level level;
    bdbuf_buffer *bd_buf;
    bdbuf_pool *bd_pool;
    disk_device *dd;
    blkdev_request1 req;

    while (1)
    {
        rc = rtems_semaphore_obtain(bd_ctx.flush_sema, RTEMS_WAIT, 0);
        if (rc != RTEMS_SUCCESSFUL)
        {
            rtems_fatal_error_occurred(BLKDEV_FATAL_BDBUF_SWAPOUT);
        }
            
        bd_buf = (bdbuf_buffer *)Chain_Get(&bd_ctx.mod);
        if (bd_buf == NULL)
        {
            /* It is possible that flush_sema semaphore will be released, but
             * buffer to be removed from mod chain before swapout task start
             * its processing. */
            continue;
        }

        bd_buf->in_progress = TRUE;
        bd_buf->use_count++;
        bd_pool = bd_ctx.pool + bd_buf->pool;
        dd = rtems_disk_lookup(bd_buf->dev);

        req.req.req = BLKDEV_REQ_WRITE;
        req.req.req_done = bdbuf_write_transfer_done;
        req.req.done_arg = bd_buf;
        req.req.start = bd_buf->block + dd->start;
        req.req.count = 1;
        req.req.bufnum = 1;
        req.req.bufs[0].length = dd->block_size;
        req.req.bufs[0].buffer = bd_buf->buffer;

        /* transfer_sema initialized when bd_buf inserted in the mod chain 
           first time */
        result = dd->ioctl(dd->phys_dev->dev, BLKIO_REQUEST, &req);
        
        rtems_disk_release(dd);
        
        if (result == -1)
        {
            bd_buf->status = RTEMS_IO_ERROR;
            bd_buf->error = errno;
            /* Release tasks waiting on syncing this buffer */
            _CORE_mutex_Flush(&bd_buf->transfer_sema, NULL,
                              CORE_MUTEX_STATUS_SUCCESSFUL);
        }
        else
        {
            if (bd_buf->in_progress)
            {
                _CORE_mutex_Seize(&bd_buf->transfer_sema, 0, TRUE, 0, level);
            }
        }
        bd_buf->use_count--;

        /* Another task have chance to use this buffer, or even
         * modify it. If buffer is not in use, insert it in appropriate chain
         * and release semaphore */
        if (bd_buf->use_count == 0)
        {
            if (bd_buf->modified)
            {
                Chain_Append(&bd_ctx.mod, &bd_buf->link);
                rc = rtems_semaphore_release(bd_ctx.flush_sema);
            }
            else
            {
                Chain_Append(&bd_pool->lru, &bd_buf->link);
                rc = rtems_semaphore_release(bd_pool->bufget_sema);
            }
        }
    }
}

/* rtems_bdbuf_find_pool --
 *     Find first appropriate buffer pool. This primitive returns the index
 *     of first buffer pool which block size is greater than or equal to
 *     specified size.
 *
 * PARAMETERS:
 *     block_size - requested block size
 *     pool       - placeholder for result
 *
 * RETURNS:
 *     RTEMS status code: RTEMS_SUCCESSFUL if operation completed successfully,
 *     RTEMS_INVALID_SIZE if specified block size is invalid (not a power 
 *     of 2), RTEMS_NOT_DEFINED if buffer pool for this or greater block size
 *     is not configured.
 */
rtems_status_code
rtems_bdbuf_find_pool(int block_size, rtems_bdpool_id *pool)
{
    rtems_bdpool_id i;
    bdbuf_pool *p;
    int cursize = INT_MAX;
    rtems_bdpool_id curid = -1;
    rtems_boolean found = FALSE;
    int j;
    
    for (j = block_size; (j != 0) && ((j & 1) == 0); j >>= 1);
    if (j != 1)
        return RTEMS_INVALID_SIZE;
    
    for (i = 0, p = bd_ctx.pool; i < bd_ctx.npools; i++, p++)
    {
        if ((p->blksize >= block_size) &&
            (p->blksize < cursize))
        {
            curid = i;
            cursize = p->blksize;
            found = TRUE;
        }
    }
    
    if (found)
    {
        if (pool != NULL)
            *pool = curid;
        return RTEMS_SUCCESSFUL;
    }
    else
    {
        return RTEMS_NOT_DEFINED;
    }
}

/* rtems_bdbuf_get_pool_info --
 *     Obtain characteristics of buffer pool with specified number.
 *
 * PARAMETERS:
 *     pool       - buffer pool number
 *     block_size - block size for which buffer pool is configured returned
 *                  there
 *     blocks     - number of buffers in buffer pool returned there 
 *
 * RETURNS:
 *     RTEMS status code: RTEMS_SUCCESSFUL if operation completed successfully,
 *     RTEMS_INVALID_NUMBER if appropriate buffer pool is not configured.
 *
 * NOTE:
 *     Buffer pools enumerated contiguously starting from 0.
 */
rtems_status_code
rtems_bdbuf_get_pool_info(rtems_bdpool_id pool, int *block_size,
                          int *blocks)
{
    if (pool >= bd_ctx.npools)
        return RTEMS_INVALID_NUMBER;
    
    if (block_size != NULL)
    {
        *block_size = bd_ctx.pool[pool].blksize;
    }
    
    if (blocks != NULL)
    {
        *blocks = bd_ctx.pool[pool].nblks;
    }
    
    return RTEMS_SUCCESSFUL;
}
