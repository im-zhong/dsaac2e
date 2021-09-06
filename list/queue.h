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

#endif // QUEUE_H