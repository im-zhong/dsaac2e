// 2021/9/3
// zhangzhong
// list.h

#ifndef LIST_H
#define LIST_H

#include "container_of.h"
#include <stdbool.h>

// 选择一手尊重历史
// 现有的单链表，再有的双链表
// 先有的next,再有的prev
// 按照字母顺序也n ... p
// 然后linux内核也是这个顺序
typedef struct list_node_t
{
    // 感觉还是先 prev 再 next要好一点
    // 因为很多函数的参数都是这样的
    // 我们思考的时候也可以遵循一样的顺序
    // 这样更好记忆一点
    // 一致的东西愈多，越舒服
    struct list_node_t *next, *prev;
} list_node_t;

// 原来的文件里面，list_head的定义和他的实现是分开的
// 因为我们想要声明一个链表结构的时候，我们一般而言需要包含list.h
// 头文件，但是这样我们会把一堆没有用的函数也包含进来，这是不好的
// 你看这个实现头文件一堆static inline
// 分明就是给.c文件包含用的
// TODO: 把list_node_t的声明与相关实现函数分开放到两个头文件里面

// TODO: 排序链表的merge操作
// TODO: 以链表表示集合的相关集合操作，交集 并集 差集
// TODO: reverse操作反转链表
// TODO: 排序链表的unique操作，这些好像都是stl里面的算法

/**
 * @brief create and init a list head
 * @head:   list head variable name
 */
#define LIST_HEAD(head) \
    list_node_t head = {&head, &head}

static inline void list_init_head(list_node_t *head)
{
    head->next = head;
    head->prev = head;
}

// 但是这里的顺序又是prev next
static inline void list_insert_between(list_node_t *prev, list_node_t *next,
                                       list_node_t *node)
{
    prev->next = node;
    node->prev = prev;
    next->prev = node;
    node->next = next;
}

// 名字从 push_front 改成了 insert_first
// 所有的插入都叫 insert
// 统一第一个元素叫做 first
// 统一最后一个元素叫做 last
// 越统一越好
static inline void list_insert_after(list_node_t *head, list_node_t *node)
{
    list_insert_between(head, head->next, node);
}

static inline void list_insert_before(list_node_t *head, list_node_t *node)
{
    list_insert_between(head->prev, head, node);
}

// 如果prev == next , prev绝对不能等于next,否则这个链表会从这个点断开
// 如果 prev == next == head
static inline void list_erase_between(list_node_t *prev, list_node_t *next)
{
    prev->next = next;
    next->prev = prev;
}

// 调用这个函数，node一定不能是head
// 否则这个链表就没有head了
static inline void list_erase(list_node_t *node)
{
    list_erase_between(node->prev, node->next);
}

static inline list_node_t *list_first(const list_node_t *head)
{
    return head->next;
}

static inline list_node_t *list_last(const list_node_t *head)
{
    return head->prev;
}

static inline void list_erase_first(list_node_t *head)
{
    list_erase(list_first(head));
}

static inline void list_erase_last(list_node_t *head)
{
    list_erase(list_last(head));
}

static inline void list_replace(list_node_t *old, list_node_t *new)
{
    new->prev = old->prev;
    new->next = old->next;
    new->prev->next = new;
    new->next->prev = new;
}

static inline void list_swap(list_node_t *left, list_node_t *right)
{
    list_node_t *right_prev = right->prev;
    list_erase(right);
    list_replace(left, right);
    list_insert_after(left == right_prev ? right : right_prev, left);
}

static inline bool list_is_first(const list_node_t *head, const list_node_t *node)
{
    return node == list_first(head);
}

static inline bool list_is_last(const list_node_t *head, const list_node_t *node)
{
    return node == list_last(head);
}

static inline bool list_is_empty(const list_node_t *head)
{
    return head->next == head;
}

// 这四个rotate确实是可以把 list_erase + list_insert_
// 这两个动作加起来的，那就叫做mova吧

// 把node删掉，insert_before head
static inline void list_move_before(list_node_t *head, list_node_t *node)
{
    list_erase(node);
    list_insert_before(head, node);
}

static inline void list_move_after(list_node_t *head, list_node_t *node)
{
    list_erase(node);
    list_insert_after(head, node);
}

static inline void list_rotate_left(list_node_t *head)
{
    // list_node_t *first = list_first(head);
    list_move_before(head, list_first(head));

    // list_erase(first);
    // list_insert_before(head, first);
}

static inline void list_rotate_right(list_node_t *head)
{
    // list_node_t *last = list_last(head);
    // list_erase(last);
    // list_insert_after(head, last);
    list_move_after(head, list_last(head));
}

// roate node to the first of the list
static inline void list_rotate_to_first(list_node_t *head, list_node_t *node)
{
    // list_erase(head);
    // list_insert_before(node, head);

    list_move_before(node, head);
}

static inline void list_rotate_to_last(list_node_t *head, list_node_t *node)
{
    // list_erase(head);
    // list_insert_after(node, head);

    list_move_after(node, head);
}

static inline bool list_is_singular(const list_node_t *head)
{
    return !list_is_empty(head) && (head->next == head->prev);
}

// 把链表从node处分开
// static inline void list_cut(list_node_t *head, list_node_t *node, list_node_t *list)
// {
//     list->next = node->next;
//     list->next->prev = list;
//     list->prev = head->prev;
//     list->prev->next = list;
//     head->prev = node;
//     node->next = head;
// }

// head: list head
// node: the cut position
// list: new list head
static inline void list_cut_after(list_node_t *head, list_node_t *node, list_node_t *list)
{
    list->next = node->next;
    list->next->prev = list;
    list->prev = head->prev;
    list->prev->next = list;
    head->prev = node;
    node->next = head;
}

static inline void list_cut_before(list_node_t *head, list_node_t *node, list_node_t *list)
{
    list->next = node;
    list->prev = head->prev;
    list->prev->next = list;
    head->prev = node->prev;
    head->prev->next = head;
    node->prev = list;
}

// tm人家没有这个函数...
// prev -- first -- last -- next
static inline void list_insert_bulk_between(list_node_t *prev, list_node_t *next,
                                            list_node_t *first, list_node_t *last)
{
    prev->next = first;
    first->prev = prev;
    next->prev = last;
    last->next = next;
}

// join two lists
static inline void list_splice_after(struct list_node_t *head,
                                     const struct list_node_t *list)
{
    list_insert_bulk_between(head, head->next,
                             list_first(list), list_last(list));
}

static inline void list_splice_before(struct list_node_t *head,
                                      const struct list_node_t *list)
{
    list_insert_bulk_between(head->prev, head,
                             list_first(list), list_last(list));
}

// 很多宏

// node: list_node_t *node, use for the work ptr
// head: list_node_t *head, list head ptr
#define list_for_each(node, head) \
    for (node = list_first(head); node != (head); node = node->next)

#define list_for_each_reverse(node, head) \
    for (node = list_last(head); node != (head); node = node->prev)

// 这两个有什么用呢
// #define list_for_each_continue(node, head) \
//     for (node = node->next; node != (head); node = node->next)

// #define list_for_each_continue_reverse(node, head) \
//     for (node = node->prev; node != (head); node = node->prev)

// node: list_node_t *, work ptr
// temp: list_node_t *, temp work ptr
// head: list_node_t*, list head ptr
// you could safely erase node when you traverse the list
#define list_for_each_safe(node, temp, head)         \
    for (node = list_first(head), temp = node->next; \
         node != (head);                             \
         node = temp, temp = node->next)

#define list_for_each_safe_reverse(node, temp, head) \
    for (node = list_last(head), temp = node->prev;  \
         node != (head);                             \
         node = temp, temp = node->prev)

// node: list_node_t*
#define list_entry(node, type, member) \
    container_of(node, type, member)

// head: list_node_t* head, list head ptr
#define list_first_entry(head, type, member) \
    list_entry(list_first(head), type, member)

#define list_last_entry(head, type, member) \
    list_entry(list_last(head), type, member)

// @entry: type*
#define list_next_entry(entry, type, member) \
    list_entry((entry)->member.next, type, member)

#define list_prev_entry(entry, type, member) \
    list_entry((entry)->member.prev, type, member)

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
         !list_is_head_entry(entry, head, member);     \
         entry = list_next_entry(entry, type, member))

#define list_for_each_entry_reverse(entry, head, type, member) \
    for (entry = list_last_entry(head, type, member);          \
         !list_is_head_entry(entry, head, member);             \
         entry = list_prev_entry(entry, type, member))

// 还有很多宏，等用到再去实现吧

#endif // LIST_H