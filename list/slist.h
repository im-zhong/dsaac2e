// 2021/9/5
// zhangzhong
// slist.h

#ifndef SLIST_H
#define SLIST_H

#include "container_of.h"
#include <stdbool.h>

typedef struct slist_node_t
{
    struct slist_node_t *next;
} slist_node_t;

#define SLIST_HEAD(head) \
    slist_node_t head = {&head};

#define SLIST_INIT(head) \
    slist_node_t head = { &head }

static inline void slist_init_head(slist_node_t *head)
{
    head->next = head;
}

// slist上是没有prev这个概念的

static inline void slist_insert_after(slist_node_t *head, slist_node_t *node)
{
    node->next = head->next;
    head->next = node;
}

// slist_erase_first = slist_erase_after(head)
static inline void slist_erase_after(slist_node_t *head)
{
    head->next = head->next->next;
}

static inline slist_node_t *slist_first(const slist_node_t *head)
{
    return head->next;
}

// replace
// swap

static inline bool slist_is_first(const slist_node_t *head, const slist_node_t *node)
{
    return node == slist_first(head);
}

static inline bool slist_is_empty(const slist_node_t *head)
{
    return head->next == head;
}

// rotate 好像是没有办法办到的
// 能做到的事情好少啊
// 算了，反正平常也是几乎不会用的，只是用来实现一个stack而已


// 然后就是遍历了 真没啥了
#define slist_for_each(node, head) \
    for (node = slist_first(head); node != (head); node = node->next)

#define slist_entry(node, type, member) \
    container_of(node, type, member)

#define slist_first_entry(head, type, member) \
    slist_entry(slist_first(head), type, member)

#define slist_next_entry(entry, type, member) \
    slist_entry((entry)->member.next, type, member)

#define slist_is_head_entry(entry, head, member) \
    (&entry->member == (head))

#define slist_for_each_entry(entry, head, type, member) \
    for (entry = slist_first_entry(head, type, member); \
         !slist_is_head_entry(entry, head, member);     \
         entry = slist_next_entry(entry, type, member))


#endif // SLIST_H