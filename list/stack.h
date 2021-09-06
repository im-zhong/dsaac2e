// 2021/9/5
// zhangzhong
// stack.h

#ifndef STACK_H
#define STACK_H

#include "slist.h"

// 关键是你想怎么让代码使用stack

// typedef slist_node_t stack_node_t;

#define STACK_MEMBER stack_

#define INHERIT_STACK \
    slist_node_t STACK_MEMBER

#define stack_init(stack) \
    SLIST_INIT(stack)

//
#define stack_push(head, entry) \
    slist_insert_after(head, &(entry)->STACK_MEMBER)

#define stack_pop(head) \
    slist_erase_after(head)

// 这里返回的额是内部指针，这是不对的
#define stack_top(head, type) \
    slist_first_entry(head, type, STACK_MEMBER)

#define stack_is_empty(head) \
    slist_is_empty(head)

#endif // STACK_H