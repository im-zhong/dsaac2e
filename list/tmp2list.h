// 2021/8/31
// zhangzhong

// header & dummy node
//

#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>
#include <stdbool.h>

// 获得结构体类型type中member成员的偏移量offset
// #define offset_of(type, member) ((size_t) &((type *)0)->member)

// 用结构体成员指针 减去 结构体成员的偏移量 就是整个结构体开始的位置
// 也就是结构体对象的指针
// 我感觉这里是不需要这个 (type *) 强制类型转换的
// #define container_of(ptr, type, member) \
//     (type *) \
//     ((void *)ptr - offsetof(type, member))

#define container_of(ptr, type, member) \
    ((void *)ptr - offsetof(type, member))

struct list_t
{
    struct list_t *next, *prev;
};

// 所有私有的成员或者函数都需要使用一个 _
static inline void init_list_(struct list_t *head)
{
    head->next = head;
    head->prev = head;
}

static inline void list_insert_between_(
    struct list_t *prev, struct list_t* next,
    struct list_t *node)
{
    prev->next = node;
    node->prev = prev;
    next->prev = node;
    node->next = next;
}

static inline void list_push_front_(
    struct list_t *head, struct list_t *node)
{
    list_insert_between_(head, head->next, node);
}

static inline void list_push_back_(
    struct list_t *head, struct list_t *node
)
{
    list_insert_between_(head->prev, head, node);
}

static inline void list_erase_between_(struct list_t *prev, struct list_t *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_erase_(struct list_t* node)
{
    list_erase_between_(node->prev, node->next);
}


// 算了
// 追求都是四个字符的意义是什么？？有病吧
static inline void list_replace_(struct list_t *oldn, struct list_t *newn)
{
    newn->prev = oldn->prev;
    newn->next = oldn->next;
    newn->prev->next = newn;
    newn->next->prev = newn;
}

// list_replace + init
// 这些结合的函数就不用写了

// list del init

// move: delete from one list, and add it to another list
// move head, move tail

// 没有必要定义那么多所谓的帮助方法，除了增加记忆量和复杂度之外没有任何意义

// 插入动作，我们有可能插入一系列值 bulk
static inline void list_insert_bulk_between_(struct list_t *prev, struct list_t *next,
    struct list_t *first, struct list_t *last)
{
    // prev [first, last] next
}

// 保证所有函数的第一个参数都是代表链表对象，这是用来模拟C++的this指针

// 一般我们都会用head代替整个链表
static inline bool list_is_first_(const struct list_t *head,
    const struct list_t *node)
{
    return node->prev == head;
}

static inline bool list_is_last_(const struct list_t *head,
    const struct list_t *node)
{
    return node->next == head;
}

static inline bool list_is_empty_(const struct list_t *head)
{
    return head->next == head;
}

static inline struct list_t * list_get_first_(struct list_t *head)
{
    return head->next;
}

static inline struct list_t* list_get_last_(struct list_t *head)
{
    return head->prev;
}


// @ptr:    the &struct list_t pointer
// list_entry
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

// 这些玩意必须得有注释
// 不然写着写着就忘了
// @head: list head ptr
// @type: list entry type
// @member: list_node_t's name in list entry type

// 我有一个问题，如果是一个空链表
// 我直接一个list_first_entry，这里会不会触发内存错误？
#define list_first_entry(head, type, member) \
    list_entry((head)->next, type, member)

#define list_last_entry(head, type, member) \
    list_entry((head)->prev, type, member)

// first or null
// last or null

// #define list_next_entry(node, type, member) \
//     list_entry((node)->next, type, member)

// #define list_prev_entry(node, type, member) \
//     list_entry((node)->prev, type, member)

// @entry: type*
#define list_next_entry(entry, type, member) \
    list_entry((entry)->member.next, type, member)

// list_prev_entry



// @node: struct list_node_t*, use for the worker ptr
// @headL struct list_node_t*, list head ptr
#define list_for_each(node, head) \
    for (node = (head)->next; node != (head); node = node->next)

// iterator reverse

// iterator at 


// 迭代的时候我们想删除某个node
// list_for_each_safe

// type
// @entry: type*
// @head: list head ptr
// @member: list node name 
#define list_is_head_entry(entry, head, member) \
    (&entry->member == (head))

// @type: struct entry_t
// @entry: type*, use for the worker ptr or loop cursor
// @head: struct node_t*, the list head ptr
// @member: as usual

#define list_for_each_entry(entry, head, type, member) \
    for (entry = list_first_entry(head, type, member); \
        !list_is_head_entry(entry, head, member);      \
        entry = list_next_entry(entry, type, member))




#endif // _LIST_H_
