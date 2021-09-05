// 2021/8/31
// zhangzhong

// header & dummy node
//

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>


// 获得结构体类型type中member成员的偏移量offset
#define offset_of(type, member) ((size_t) &((type *)0)->member)

// 用结构体成员指针 减去 结构体成员的偏移量 就是整个结构体开始的位置
// 也就是结构体对象的指针
#define container_of(ptr, type, member) \
    (type *)((void *)ptr - offset_of(type, member))

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

static inline void list_swap_(struct list_t *lnode, struct list_t* rnode)
{

}

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

static inline void list_rotate_left(struct list_t *head)
{

}

static inline void list_rotate_right_(struct list_t *head)
{

}

static inline void list_rotate_to_first_(struct list_t *head, struct list_t *node)
{

}

static inline void list_rotate_to_last_(struct list_t *head, struct list_t *node)
{

}

static inline bool list_is_singular_(const struct list_t *head)
{

}

// 如何保存链表的数量呢?
// 保存链表的数量会不会让链表的实现变得复杂？？会！
// 有什么用？几乎没有用
// 那当然就不保存啦！！！

// cut into two list
static inline bool list_cut_(struct list_t *head, struct list_t *node)
{

}

// join two list
static inline bool list_splice_(struct list_t *head, struct list_t *another_head)
{

}

static inline void list_pop_first_(struct list_t *head)
{

}

static inline void list_pop_last_(struct list_t *head)
{

}

// merge two sorted list
// splice
// remove, remove_if
// reverse
// unique
// sort


// 貌似就这些接口
// 我去研究一下C++的接口




// 先定义接口吧，先不实现了


// 然后是最终的部分
// container_of

// @ptr:    the &struct list_t pointer
// list_entry
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)


#define list_first_entry(head, type, member) \
    list_entry((head)->next, type, memeber)

#define list_last_entry(head, type, member) \
    list_entry((head)->prev, type, member)

// first or null
// last or null

#define list_next_entry(node, type, member) \
    list_entry((node)->next, type, member)

// list_prev_entry

// iterator
#define list_for_each(head, work) \
    for (work = (head)->next; work != (head); work = work->next)

// iterator reverse

// iterator at 


// 迭代的时候我们想删除某个node
// list_for_each_safe

#define list_for_each_entry(entry, head, type, member) \
    for (entry = list_first_entry(head, type, member); \
        !list_entry_is_head(entry, head, member); \
        entry = list_next_entry(entry, type, member))



// 迭代


// TODO: 把这些例程缩小一点，拿一部分出来，先试试看，怎么用，怎么写，对吧

#endif // _LIST_H_
