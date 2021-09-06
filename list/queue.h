// 2021/9/6
// zhangzhong
// queue.h

#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

#define QUEUE_MEMBER queue_

#define INHERIT_QUEUE \
    list_node_t QUEUE_MEMBER

#define queue_init(queue) \
    LIST_HEAD(queue)

#define queue_push(head, entry) \
    list_insert_before(head, &(entry)->QUEUE_MEMBER)

#define queue_pop(head) \
    list_erase_first(head)

#define queue_top(head, type) \
    list_first_entry(head, type, QUEUE_MEMBER)

#define queue_is_empty(head) \
    list_is_empty(head)

// deque

#define DEQUE_MEMBER deque_

#define INHERIT_DEQUE \
    list_node_t DEQUE_MEMBER

#define deque_init(deque) \
    LIST_HEAD(deque)

#define deque_push_first(head, entry) \
    list_insert_after(head, &(entry)->DEQUE_MEMBER)

#define deque_push_last(head, entry) \
    list_insert_before(head, &(entry)->DEQUE_MEMBER)

#define deque_pop_first(head) \
    list_erase_first(head)

#define deque_pop_last(head) \
    list_erase_last(head)

#define deque_first(head, type) \
    list_first_entry(head, type, DEQUE_MEMBER)

#define deque_last(head, type) \
    list_last_entry(head, type, DEQUE_MEMBER)

#define deque_is_empty(head) \
    list_is_empty(head)

#endif // QUEUE_H