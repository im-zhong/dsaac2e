// 2021/9/16
// zhangzhong
// 二叉堆

// 因为stl和linux内核都是使用的最简单的数组实现的二叉堆
// 那我也就这么实现了
// 只不过这次linux内核没有采用像红黑树那样，需要用户自己实现关键的函数
// 反而是使用了函数指针，这两种选择其实就是一个trade-off
// 只不过在这里我需要自行实现一个可变数组
// 我可以看看linux内核有没有实现可变数组这个功能,,,并没有，，，那就真的只是一个定长的数组
// 而我肯定是要实现一个变长的数组的，就直接乘以2
// 用户在创建一个数据结构的时候，是可以不去指定一个初始长度的，那么我就来一个默认的长度2^n
// 然后用户插入的时候，就可以自行按照*2的倍率取分配新的空间就可以了

// 就仅仅实现一个二叉堆就可以了, 最基础的功能就可以

// 用户仍然希望在堆中存在一种类型 T
// 同时用户也必须提供一个可以比较T类型的比较函数

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

typedef struct
{
    void *data;
    size_t size;
    size_t capacity;

    // 用户在初始化一个堆的时候，同时传入一个该堆的比较函数
    // 因为我们的概念是一个堆，而不是最大堆或者最小堆
    // 我们的规则是 predicate(parent, child) is true
    // 这样我们就可以使用heap指针来调用这个函数了，而不用用户每次调用函数都需要传入一个指针，很拉跨
    size_t type_size;
    bool (*predicate)(const void *lhs, const void *rhs);
    void (*swap)(void *lhs, void *rhs);
    void (*move)(void *dest, void* src);
} bheap;

size_t ceiling(size_t size)
{
    int bits = 0;
    while (size != 0)
    {
        size >>= 1;
        ++bits;
    }
    return 1 << bits;
}

void bheap_init(bheap *heap, size_t size, size_t type_size,
                bool (*predicate)(const void *lhs, const void *rhs),
                void (*swap)(void *lhs, void *rhs),
                void (*move)(void *dest, void *src))
{
    heap->size = 0;
    // 我需要找到距离size最近的一个 2^n
    heap->capacity = ceiling(size);
    heap->type_size = type_size;
    heap->predicate = predicate;
    heap->swap = swap;
    heap->move = move;
    // 我需要分配一个数组
    heap->data = malloc(heap->capacity * heap->type_size);
}

void bheap_clear(bheap *heap)
{
    heap->size = 0;
}

void bheap_free(bheap *heap)
{
    free(heap->data);
}

// 二叉堆实际上是一个数组
// 用来表示一颗近似的完全二叉树
// 什么是完全二叉树呢？？
// 就是一颗被填满二叉树，如果把所有节点按照层先的顺序放到一个数组里面，每隔节点的孩子和父亲的下标是存在一个奇妙的关系的
//          A
//      B        C
//   D    E    F    G
//  H I  J K  L M  N O
//
//   A B C D E F G H I  J  K  L  M  N  O
// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
//
// 我们随便挑一对父子节点 C F G
// C 3, F 6, G 7
// C * 2 = F:               parent * 2 = left_child
// C * 2 + 1 = G:           parent * 2 + 1 = right_child
// F / 2 = 6 / 2 = 3 :      left_child / 2 = parent
// G / x = 7 / 2 = 3:       right_child / 2 = parent, 上面两条可以总结成 child / 2 = parent
// 当然也有一些特例，比如root的parent还是零；而叶子节点没有对应的孩子，直接计算其下标将溢出
// 不过这些情况都可以很简单的判断出来
//

static inline size_t bheap_root()
{
    return 1;
}

static inline size_t bheap_parent(size_t child)
{
    return child >> 1;
}

static inline size_t bheap_left(size_t parent)
{
    return parent << 1;
}

static inline size_t bheap_right(size_t parent)
{
    return bheap_left(parent) + 1;
}



// 但是一个完全二叉树想要成为堆，还需要满族一些堆的性质
// 堆根据其需要满足的性质的不同，分成最大堆和最小堆
// 这两种堆实际是对称的，只需要实现其中一种即可
// 这里以最小堆为例
//
// 最小堆 min_heap
// 1. 除了根(index = 0)以外的所有节点i,均满足 heap[parent(i)] <= heap[i]
// 简单的说，就是除了根节点，所有节点的值都比其子节点要小; 同时根就是最小的节点
// 一句话就是，任意的节点小于他的后裔

// 需要支持的几个操作
// push pop top
// heapify: 在一次修改之后重新维护一个堆的性质？？
// make_heap: 构建一个堆

// void *heap_top(heap *h)
// {
//     return h->data + 1;
// }

static inline void *bheap_top_(bheap *heap)
{
    return heap->data + 1;
}

#define bheap_top(heap, type) \
    (type *)((heap)->data + (heap)->type_size)

// 我们需要定义一个简单的操作，可以用来代替数组的读取
static inline void *bheap_index(bheap *heap, size_t index)
{
    return heap->data + (index * heap->type_size);
}

// 接下来实现最为重要的函数，在插入一个新的元素时，维护堆的性质
// void heapify(heap *h, void *data)
// {
//     // 为了插入一个元素，我们在下一个空的位置制造一个空穴
//     // 刚好heap.size指向这个位置
//     size_t index = h->size;
//     h->data[h->size] = data;
//     h->size++;
//     // 我们检查堆序是否仍然保持,也就是检查我们的parent的值是否比我们的小
//     size_t parent = heap_parent(index);
//     // if (parent != 0)
//     // {
//     //     // compare
//     //     bool result;
//     //     // int result = compare(h->data[parent], h->data[h->size]);
//     //     // 如果result为false，说明我们新插入的节点破坏了堆的性质
//     //     if (result)
//     //     {
//     //         // 仍然维持堆的性质，不需要修改
//     //     }
//     //     else
//     //     {
//     //         // 堆失序，需要调整
//     //         // 我们将此节点和父节点里面的元素调换
//     //         // 实现一个简单的swap函数, 这里仅仅调换void*指针而已
//     //         heap_swap(parent, index);
//     //         // 我们这个heap换上去之后，仍然有可能还是失序的，所以仍然需要上滤，知道 parent == 0 || data[parent] <= data[index]
//     //     }
//     // }

//     // data[0]可以存放一个dummy元素，
//     while (parent != 0 && data[parent] <= data[index])
//     {
//         heap_swap(h, parent, index);
//         index = parent;
//         parent = heap_parent(index);
//     }
// }

// 插入和调整都是比较简单
// 找出最小的单元也是非常简单的
// // 困难的是删除最小的节点
// void heap_pop(heap *h)
// {
//     // 如果删掉根，那么整棵树分成两颗子树，
//     // 如果我们还要维护堆序，我们就需要把剩下的最小的元素给拿出来，放到根的位置上
//     // 最小的元素一定要么是左子树的根，要么是右子树的根
//     // size_t parent = 1;
//     // size_t left = heap_left_child(1);
//     // size_t right = heap_left_child(2);
//     // 其实也可以直接指定 2 3
//     // 我们还要判断这些子树是否存在，这里先假设他们都存在
//     // if (data[left] < data[right])
//     // {
//     //     // 左子树的根是新的最小元
//     //     // 我们交换两个节点
//     //     heap_swap(h, parent, left);
//     //     // 这样交换完成之后，左子树上的根又被删除了
//     //     // 我们向下迭代这个过程
//     //     // 那么结束条件是什么呢？？
//     //     // 我们无法找到下一个孩子，
//     // }
//     // else
//     // {
//     //     // 右子树的根是新的最小元
//     //     heap_swap(h, parent, right);
//     // }

//     // 我们删除root之后，会形成一个空洞
//     size_t hole = 1;
//     size_t left = heap_left_child(hole);
//     size_t right = left + 1;
//     // 其实left一定是小于right，我们可以削减判断的次数

//     while (left < h->size)
//     {
//         // 只要left在索引内即可，如果left都不在，那么right一定不在
//         // 因为right = left + 1

//         // 如果left存在，我们需要判断right是否存在
//         right = left + 1;
//         if (right < h->size)
//         {
//             // left和right都在，我们需要swap(parent, min(left, right))
//             // 然后更新left，然后继续
//             // hole = data[left] < data[right] ? left : right;
//             size_t index = data[left] < data[right] ? left : right;
//             swap(h, hole, index);
//             hole = index;
//             left = heap_left_child(hole);
//         }
//         else
//         {
//             // 如果left存在，但是right不在，这说明我们已经到底了
//             // swap(h, hole, left);
//             // 这说明left就是最后一个元素，这句话可以放到外面取运行，
//             break;
//         }
//     }

//     // 在最后，我们需要用目前最后一个元素，来填充空穴的位置
//     // 所以我们需要记录空穴的位置
//     swap(h, hole, h->size - 1);
//     --h->size;
// }

static inline bool bheap_is_empty(bheap *heap)
{
    return heap->size == 0;
}

static inline bool bheap_is_full(bheap *heap)
{
    return heap->size == heap->capacity;
}


void bheap_fixdown(bheap *heap, size_t index)
{
    // 我需要判断我是否符合条件

    // size_t left = 0;
    // size_t right = 0;
    size_t child = 0;

    // 首先我要找出left和right里面最小的那个
    // size_t child = (heap->predicate)(bheap_index(heap, left), bheap_index(heap, right)) ? 
    //     left : right;
    
    // // 而我们的index必须比其中最小的那个还要小
    // while(!(heap->predicate)(bheap_index(heap, index), bheap_index(heap, child)))
    // {
    //     // 那么我们就需要做一个替换
    //     (heap->swap)(bheap_index(heap, index), bheap_index(heap, child));
    //     index = child;

    //     // 同时，我们还需要判断left和right是否有效
    // }

    while ((child = bheap_left(index)) <= heap->size)
    {
        // 我首先要在left和right中找出最小的哪一个
        // 如果只有左孩子，那最小的肯定是左孩子啊
        child = left;
        right = left + 1;
        // 如果我们还有右孩子，我们就看看这两个谁最小
        if (right <= heap->size)
        {
            child = (heap->predicate)(bheap_index(heap, left), bheap_index(heap, right)) ? 
                left : right;
        }   

        // 然后我们看看
        if ((heap->predicate)(bheap_index(heap, index), bheap_index(heap, child)))
        {
            // 如果关系正确，直接返回
            return;
        }
        else
        {
            index = child;
            left = bheap_left(index);
        }
    }
}

// 现在还有一个关键的问题，就是size应该如何表示
// 我们的size最好等于实际使用的数量，这样有助于capacity相关的reallocate的实现
// 其实也无所谓啦，关键看哪个实现起来更优雅
// 那就选择用size表示实际的元素数量，然后我们实际占用的数组长度是size + 1, 因为0未使用
// 数组下标是从 [1, size]
// 所以第一个元素是data[1], 而最后一个元素是data[size]
void bheap_pop(bheap *heap)
{
    size_t hole = bheap_root();
    size_t left = bheap_left(hole);
    size_t right = 0;
    // 其实left一定是小于right，我们可以削减判断的次数

    // 这个函数最好可以和fixdown结合起来，思考一下
    // 我这个其实相当于
    // bheap_fixdown(heap, bheap_root())

    // 不对不对，我又理解错了
    // pop之后，我们需要将最后一个位置的元素放到一个正确的位置中
    // 那么什么是一个正确的位置呢
    // 其实就是把最后一个元素放到root的位置
    // 然后启动一个向下过滤的过程而已
    (heap->move)(heap_index(heap, heap->size), heap_index(heap, bheap_root()));
    --heap->size;
    // 然后启动一次下滤
    bheap_fixdown(heap, bheap_root());


    // 这个逻辑是不对的。。。tmd
    // 我得检测是不是
    while (left <= heap->size)
    {
        // 只要left在索引内即可，如果left都不在，那么right一定不在
        // 因为right = left + 1

        // 如果left存在，我们需要判断right是否存在
        right = left + 1;
        if (right <= heap->size)
        {
            left = (heap->predicate)(bheap_index(heap, left), bheap_index(heap, right)) ? left : right;
            (heap->swap)(bheap_index(heap, hole), bheap_index(heap, left));
            hole = left;
            left = bheap_left(hole);
        }
        else
        {
            break;
        }
    }

    // 在最后，我们需要用目前最后一个元素，来填充空穴的位置
    // 所以我们需要记录空穴的位置
    // (heap->swap)(bheap_index(heap, hole), bheap_index(heap, heap->size));
    (heap->move)(bheap_index(heap, hole), bheap_index(heap, heap->size));
    --heap->size;
}

// 还有三个额外的操作
// 其中两个涉及到修改一个元素的优先级，当我们把优先级增大，我们需要将该节点向下移动
// 如果我们把优先级缩小，我们需要把该节点向上移动
// 其实非常简单
// 但是问题是，我们怎么找到一个特定的元素呢？？
// 算法导论的函数签名里头直接给出了要修改值的节点下标i，这是否有一些不妥，因为我们根本没有一个简单的方法可以知道一个值的索引，除非tm遍历？？！！

// 我们把上滤和下滤这两个过程抽象出来
void bheap_fixup(bheap *heap, size_t index)
{
    size_t parent = 0;
    while ((parent = bheap_parent(index)) != 0 && !(heap->predicate)(bheap_index(heap, parent), bheap_index(heap, index)))
    {
        (heap->swap)(bheap_index(heap, parent), bheap_index(heap, index));
        index = parent;
    }
}

// 我们把index的值变大了，所以我们需要下沉这个节点
// void hheap_fixdown(bheap *heap, size_t index)
// {
//     // size_t hole = index;
//     size_t left = bheap_left(index);
//     // 其实不需要right变量，但是加上根本也无所谓。。。为了可读性更高，还是加上吧
//     size_t right = 0;
//     // 其实left一定是小于right，我们可以削减判断的次数

//     // 判断两个孩子是否都在
//     // 首先判断左孩子是否存在
//     while (left < heap->size)
//     {
//         // 只要left在索引内即可，如果left都不在，那么right一定不在
//         // 因为right = left + 1

//         // 如果left存在，我们需要判断right是否存在
//         right = left + 1;
//         if (right < heap->size)
//         {
//             // left和right都在，我们需要swap(parent, min(left, right))
//             // 然后更新left，然后继续
//             // hole = data[left] < data[right] ? left : right;
//             // 这里需要一个临时变量，但是在这里left在求值之后是没有用的，我们就用这个left变量代替一下临时变量
//             left = data[left] < data[right] ? left : right;
//             swap(heap, index, left);
//             index = left;
//             left = bheap_left(index);
//         }
//         else
//         {
//             // 如果left存在，但是right不在，这说明我们已经到底了
//             // swap(h, hole, left);
//             // 这说明left就是最后一个元素，这句话可以放到外面取运行，
//             swap(heap, index, left);
//             return;
//         }
//     }
// }

// 实现了上面两个函数之后，修改key值的实现就非常简单了
// void bheap_modify(bheap *heap, size_t index, void *data)
// {
//     // 我们可以调用函数来判断是如何修改
//     if (compare(data, heap->data[index]) < 0)
//     {
//         bheap_fixup(heap, index);
//     }
//     else
//     {
//         bheap_fixdown(heap, index);
//     }
// }

// 我们如何根据一个现有的数组来构建堆
void make_heap(void *data, size_t size)
{
    // 调用一系列bheap_insert即可
}

void make_heap_in_place(void *data, size_t size)
{
    // 那么我如何在原地创建一个heap呢??
    // 你给我的任何一个数组，我都可以认为是一颗二叉树
    // 只不过现在这颗二叉树是没有堆序的
    // 我们可以从头开始，参考类似插入排序的思想
    // 我们首先只得到第一个节点，我们认为这是一个堆
    // 然后我们将size+1，我们认为我们插入了一个新的节点
    // 然后调用fixup例程使得这两个节点维持堆的性质
    // 然后size +1, 依次类推，直至所有节点

    // 虽然我上面的想法是对的，但是自顶向下的方法比较慢
    // 我们可以采取自底向上的方法
    // 我们可以知道，如果一个数组的大小是n
    // 那么 n / 2 就是所有非叶子节点的数量
    // 这个是非常容易理解和证明的
    // 一颗高度为h的完全二叉树，其总的节点数量为
    // n =  2^0 + 2^1 + 2^2 + ... + 2^(h-1)
    // 我们将此节点数量/2, 得到
    // n / 2 = [1/2] + 2^0 + ... + 2^(h-2)
    // 因为整数除法的性质，上面的[1/2]=0, 而一个h-1层的树刚好可以表示为这种形式
    // 所以一颗完全二叉树的一半节点是叶子节点，而另一半是非叶子节点
    //
    // 如果是一颗不是很满的二叉树呢？
    // 我们假设一颗叶子不满的二叉树的节点数量为m
    // 并且令k为缺少的数量，既 m + k = n, 则成为一颗完全二叉树
    // 我们让m = n - k
    // m / 2 = n / 2 - k / 2
    //       = {所有非叶子节点的数量} - 叶子节点数量/2
    // 同时 m = 非叶子节点数量+叶子节点数量
    // for (size_t i = size / 2; i > = 0; --i)
    // {
    //     bheap_fixdown(heap, data, i);
    // }
}

// 还有一个操作，删除某个位置，这tm不和删除最小元是一样的？？？
// 没什么好实现的

bool is_bheap(bheap *heap)
{
    size_t child = 0;
    // 判断是否是一个二叉堆特别简单
    // 只需要遍历非叶子节点，然后判断一下他们的孩子是不是都比他们大
    // 而叶子节点是没有孩子的，所以是无需判断的，因为他们都是一个堆
    for (size_t i = 1; i <= heap->size / 2; ++i)
    {
        child = bheap_left(i);
        if (child <= heap->size && !(heap->predicate)(bheap_index(heap, i), bheap_index(heap, child)))
            return false;
        child = bheap_right(i);
        if (child <= heap->size && !(heap->predicate)(bheap_index(heap, i), bheap_index(heap, child)))
            return false;
    }
    return true;
}

// 提供push操作
// 还要考虑reallocate
void bheap_push(bheap *heap, void *data)
{
    // 我们仍然可以使用一个, 我们怎么把一个 void* 指针赋值给他呢 ???

    // 判断是否full，如果满了，需要reallocate
    // 这里先不判断了

    // 将元素放到 size + 1 的位置
    ++(heap->size);
    // 用户还可以提供一个move函数，因为有的时候，我们需要的不是一个swap
    // 而是把一个值放到一个没有有意义的值的内存位置的时候
    (heap->move)(bheap_index(heap, heap->size), data);

    // 之后，我们执行一个上修动作
    bheap_fixup(heap, heap->size);
}


// 我想要一个int类型的heap
// 那么我需要定义三个函数
// 感觉这些比较普通的实现可以变成一个宏
// 用户给出函数名和类型可以直接生成一个函数定义
// 也能减轻部分负担
bool predicate_integer(const void* lhs, const void* rhs)
{
    return *(int*)(lhs) <= *(int*)(rhs);
}

void swap_integer(void *lhs, void *rhs)
{
    int temp = *(int*)(lhs);
    *(int*)lhs = *(int*)rhs;
    *(int*)rhs = temp;
}

void move_integer(void *dest, void *src)
{
    *(int*)dest = *(int*)src;
}

// 那么现在我可以写一些测试例程了
void test()
{
    // 随机生成一些数组，疯狂插入之后疯狂删除
    // 期间检查是否还是一个堆
    srandom(time(NULL));
    size_t size = 20;

    bheap iheap;
    bheap_init(&iheap, size, sizeof(int),
               &predicate_integer, &swap_integer, &move_integer);

    printf("testing bheap push...\n");
    int number = 0;
    for (size_t i = 0; i < size; ++i)
    {
        number = random() % 100;
        bheap_push(&iheap, &number);

        // check if a heap
        assert(is_bheap(&iheap));
    }

    printf("testing bheap top & pop...\n");
    int *pi = NULL;
    while (!bheap_is_empty(&iheap))
    {
        pi = bheap_top(&iheap, int);
        // 如果我们不断的把一个数取出来，那不就是一个排序吗？？
        printf("%d\n", *pi);
        bheap_pop(&iheap);

        assert(is_bheap(&iheap));
    }
}


int main(int argc, char *argv[])
{
    // 感觉swap可以用move生成??
    bheap iheap;
    bheap_init(&iheap, 100, sizeof(int),
     &predicate_integer, &swap_integer, &move_integer);

    int i = 0;
    bheap_push(&iheap, &i);

    int *pi = NULL;
    pi = bheap_top(&iheap, int);
    printf("%d\n", *pi);

    bheap_pop(&iheap);
    if (bheap_is_empty(&iheap))
    {
        printf("heap is empty\n");
    }

    bheap_free(&iheap);

    test();
}