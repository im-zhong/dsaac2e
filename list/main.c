// 2021/9/12
// zhangzhong
// 那我就用这个链表实现一个多项式加法吧
//
// to test git restore
// i add some thing i do not want to commit

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "queue.h"

typedef struct
{
    int coefficient;
    int power;
    struct list_node_t node;
} polynomial_t;

typedef struct
{
    // data
    int data;
    struct list_node_t node;
} entry_t;

void print_polynomial(const polynomial_t *polynomial)
{
    printf("%d, %d\n", polynomial->coefficient, polynomial->power);
}



typedef struct
{
    char *symbol;

    INHERIT_STACK;
} symbol_stack_t;

typedef struct
{
    int i;
    INHERIT_STACK;
} int_stack_t;

typedef struct
{
    int i;
    INHERIT_QUEUE;
} int_queue_t;

int main(int argc, char* argv[])
{
    stack_init(stack);
    
    symbol_stack_t *symbol = malloc(sizeof(symbol_stack_t));

    // 就仅仅提供四个成员函数即可
    stack_push(&stack, symbol);

    stack_pop(&stack);

    symbol = stack_top(&stack, symbol_stack_t);

    stack_is_empty(&stack);

    stack_init(int_stack);
    int_stack_t *pi;
    for (int i = 0; i < 10; ++i)
    {
        pi = malloc(sizeof(int_stack_t));
        pi->i = i;
        stack_push(&int_stack, pi);
    }

    while (!stack_is_empty(&int_stack))
    {
        pi = stack_top(&int_stack, int_stack_t);
        printf("%d\n", pi->i);
        stack_pop(&int_stack);
        free(pi);
    }

    printf("queue\n");
    queue_init(queue);
    int_queue_t *pq;
    for (int i = 0; i< 10; ++i)
    {
        pq = malloc(sizeof(int_queue_t));
        pq->i = i;
        queue_push(&queue, pq);
    }

    while (!queue_is_empty(&queue))
    {
        pq = queue_top(&queue, int_queue_t);
        printf("%d\n", pq->i);
        queue_pop(&queue);
        free(pq);
    }
}

int list_main(int argc, char *argv[])
{
    // 提供两种初始化head的方式
    // list_node_t head;
    // init_list_head(&head);

    // list_node_t head = { &head, &head };

    // 需不需要提供一个宏来做一个简化呢？
    // 我们想一下宏是为了什么，就是为了简化代码
    // 但是宏减弱了可读性，增加了维护的困难
    // 只有在明确减少了代码的前提下，我们才需要使用宏

    LIST_HEAD(head);

    // 好像还行哦，嘿嘿

    for (int i = 0; i < 10; ++i)
    {
        polynomial_t *polynomial = malloc(sizeof(polynomial_t));
        polynomial->coefficient = i * 2;
        polynomial->power = i;
        list_insert_after(&head, &polynomial->node);
    }

    for (int i = 0; i < 10; ++i)
    {
        polynomial_t *polynomial = malloc(sizeof(polynomial_t));
        polynomial->coefficient = i * 2;
        polynomial->power = i;
        list_insert_before(&head, &polynomial->node);
    }

    printf("init\n");
    list_node_t *work;
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("after erase\n");
    list_erase_first(&head);
    list_erase_last(&head);
    list_erase_first(&head);
    list_erase_last(&head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("erase first, replace last\n");
    list_erase_first(&head);
    list_node_t *first = list_first(&head);
    list_node_t *last = list_last(&head);
    list_erase_first(&head);
    list_replace(last, first);
    list_init_head(last);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    // 可以删除直到只有两个元素，再调用一次list_swap看看能不能正常工作

    printf("swap\n");
    list_swap(list_first(&head), list_last(&head));
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("%d\n", list_is_first(&head, list_first(&head)));
    printf("%d\n", list_is_first(&head, list_last(&head)));
    printf("%d\n", list_is_last(&head, list_first(&head)));
    printf("%d\n", list_is_last(&head, list_last(&head)));
    printf("%d\n", list_is_empty(&head));

    printf("rotate left\n");
    list_rotate_left(&head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("rotate right\n");
    list_rotate_right(&head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("rotate last to frist\n");
    list_rotate_to_first(&head, list_last(&head));
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("rotate first to last\n");
    list_rotate_to_last(&head, list_first(&head));
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("%d\n", list_is_singular(&head));

    polynomial_t *polynomial;
    // find the first power is 0
    list_for_each(work, &head)
    {
        polynomial = list_entry(work, polynomial_t, node);
        if (polynomial->power == 0)
        {
            break;
        }
    }
    // find power is 0
    printf("find power is 0\n");
    print_polynomial(polynomial);

    printf("cut after 0, 0\n");
    list_node_t new_head;
    list_cut_after(&head, work, &new_head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("-------\n");
    list_for_each(work, &new_head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("-------\n");
    list_splice_after(&head, &new_head);
    printf("insert after head\n");
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("-------\n");

    // find first 12
    list_for_each(work, &head)
    {
        polynomial = list_entry(work, polynomial_t, node);
        if (polynomial->coefficient == 12)
        {
            break;
        }
    }
    printf("find 12\n");
    print_polynomial(polynomial);
    printf("cut before 12\n");
    list_cut_before(&head, work, &new_head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("-------\n");
    list_for_each(work, &new_head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("-------\n");

    printf("list splice before head\n");
    list_splice_before(&head, &new_head);
    list_for_each(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }
    printf("erase first, for each reverse\n");
    list_erase_first(&head);
    list_for_each_reverse(work, &head)
    {
        print_polynomial(list_entry(work, polynomial_t, node));
    }

    printf("--------\n");

    polynomial = list_first_entry(&head, polynomial_t, node);
    print_polynomial(polynomial);

    polynomial = list_last_entry(&head, polynomial_t, node);
    print_polynomial(polynomial);


    printf("list for each entry\n");
    list_for_each_entry(polynomial, &head, polynomial_t, node)
    {
        print_polynomial(polynomial);
    }

    printf("list for each entry reverse\n");
    list_for_each_entry_reverse(polynomial, &head, polynomial_t, node)
    {
        print_polynomial(polynomial);
    }

    return 0;
}

int old_main(int argc, char *argv[])
{
    // polynomial_t* polynomial = malloc(sizeof(polynomial_t));

    // // 在外部代码使用list的时候，是不是不应该暴露这个list_t结构??
    // init_list_(&polynomial->node);

    // LIST_HEAD(head) 等于下面这个东西
    // ?? 还能这么写??
    // struct list_t head = { &head, &head };

    //

    // printf("head: %p\n", &head);
    // printf("prev: %p\n", head.prev);
    // printf("next: %p\n", head.next);
    // 雾草，真的可以
    // 其实也非常合理，因为地址在声明这个变量的时候就存在了，所以在其求值的时候当然可以正确的得到他的内存地址了
    //

    // 这个list是一个有“头”的list, 他的结构就像是这样
    //  ... - last - head - first - ...
    // 双向循环链表
    // 而head是不存放数据的
    // 每个entry都存放了一个链表的node

    // 所以我们要使用链表的时候，往往需要定义一个tou
    // 而我们传递一个链表的时候，也仅仅传递一个头就可以了
    // 这种设计是真的牛逼！！！

    // 我们这样写，就定义了一个万能的链表头
    // int i = 1;
    // struct list_t head = { &head, &head };
    // printf("head: %p\n", &head);
    // printf("prev: %p\n", head.prev);
    // printf("next: %p\n", head.next);

    // // 我尝试获得一个空链表的entry
    // entry_t *entry = list_first_entry(&head, entry_t, node);
    // // entry->data = 1;
    // // printf("%d\n", entry->data);
    // printf("%p\n", entry);

    // entry = container_of(&head, entry_t, node);
    // printf("%p\n", entry);
    // printf("%p\n", &entry->data);
    // // entry->data = 1;
    // // printf("%p\n", entry);
    // // printf("%p\n", &entry->data);

    // list_for_each_entry(entry, &head, entry_t, node)
    // {
    //     printf("have entry");
    // }

    // 到底为止，这个双向链表我基本上已经完全悟了
    // 一点一点的写出来吧，一定要边写实现，边写测试代码

    // 我们可以构建一个我们自己的entry
    // entry_t *entry = malloc(sizeof(entry_t));
    // // 为数据部分赋予新的值
    // // entry->data = ...
    // list_push_front_(&head, &entry->node);

    // // 我们可以继续push push push

    // // 最重要的，我们需要遍历
    // // 如果不使用宏，我们自己遍历这个链表，我们需要如何遍历呢
    // struct list_t *work;
    // 感觉这里的head更普遍的情况应该是作为一个指针来使用
    // head.next = first
    // for (work = head.next; work != &head; work = work->next)
    // {
    //     // 我们想要遍历链表，当然要处理的是 struct entry 对象
    //     // 所以我们自然需要一个能够通过指针获取entry对象的东西
    //     // 那就是container_of
    //     entry = container_of(work, entry_t, node);
    //     // 拿到entry结构的指针
    //     // func (entry)
    // }

    // 显然head是一个比list更好的名字
    // head清晰的表示了这是一个链表的头
    // 而list仅仅表示这是一个链表

    // list_head也比list_t更好
    // list_node也行
    // 感觉list_node_t好像更好？？
    // 因为head也是一个node，但是并不是所有的node都是head
    // 虽然他们也都可以成为head, 但是大部分时候他们都不是

    // 像上面这个循环，如果我们经常使用的话，写起来很烦
    // 所以可以提供一种简单的宏来减少代码
    // 其实 typeof 还是很有吸引力的，但是我需要知道这是不是GCC的扩展或者是C的标准
    // 如果不是标准，恐怕是不能用的
    // 并不是，这是GCC的扩展。。。
    // 虽然我也只用GCC
    // 但是其他的人可能不用，所以没办法
    // list_for_each(work, &head)
    // {

    // }

    // 但是如果我们像直接访问entry，像上面这个例子，我们还需要额外的container_of来访问每一个entry

    // for (entry_t *entry = list_first_entry(&head, entry_t, node);
    //     entry && entry != head; // 我需要判断结束条件，也就是next_entry不能是一个属于head的entry
    //     // 但是问题是，next_entry如果访问到head, 这个head是不存在一个对应的entry实例的
    //     // 这回触发内存错误
    //     // 所以在这种时候应该返回一个nullptr
    //     entry = list_next_entry(&head, entry_t, node))
    // {

    // }

    // 如何添加entry呢
    // 好像没有相关的宏暴露出来啊？？

    return 0;
}
