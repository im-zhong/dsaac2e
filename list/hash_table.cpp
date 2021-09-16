// 2021/9/16
// zhangzhong
// hash_table.h

// 首先提供两个哈希函数

/*
 * This hash multiplies the input by a large odd number and takes the
 * high bits.  Since multiplication propagates changes to the most
 * significant end only, it is essential that the high bits of the
 * product be used for the hash value.
 *
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * Although a random odd number will do, it turns out that the golden
 * ratio phi = (sqrt(5)-1)/2, or its negative, has particularly nice
 * properties.  (See Knuth vol 3, section 6.4, exercise 9.)
 *
 * These are the negative, (1 - phi) = phi**2 = (3 - sqrt(5))/2,
 * which is very slightly easier to multiply by and makes no
 * difference to the hash distribution.
 */
#define GOLDEN_RATIO_32 0x61C88647
#define GOLDEN_RATIO_64 0x61C8864680B583EBull

// 这两个数取自linux内核，其原理见 算法导论

// 假设关键字为32bit或者64bit

// 我们的数组长度一般选取 2^bits 次方，这样rehash的时候非常方便

// 而我们的内部需要一个链表，显然，我们还是需要一个双向循环链表
// 否则在我们找到一个node之后，我们无法在O(1)的时间内将其删除

#include "list.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// 数字哈希函数
// 内核将其划分为两种
// 32bit 64bit

#define load_factor_threshold (1.25)

// bits指的是数组的长度
// 比如 bits = 8
// 则数组长度为 1 << 8 = 2^8
// 00000001 -> 1 00000000 = 256 = 2^8
// 也就是数组的下标可以在 00000000 - 11111111 这8个bit之间取任何值
static inline size_t hash32(int key, int bits)
{
    // use result's high bits
    return (key * GOLDEN_RATIO_32) >> (32 - bits);
}

static inline size_t hash64(long key, int bits)
{
    return (key * GOLDEN_RATIO_64) >> (64 - bits);
}

#define hash_integer(key, bits) \
    sizeof(key) <= 4 ? hash32(key, bits) : hash64(key, bits)

// 字符串哈希函数
size_t hash_string(unsigned char *string, int bits)
{
    size_t v = 5381;
    size_t c = 0;

    while (c = *str++)
        v = ((v << 5) + v) + c; /* hash * 33 + c */

    return hash_integer(v, bits);
}

struct hash_table
{
    list_node_t *slots;

    // 我们在这里要记录bits 也就是数组的大小
    // 和链表的长度，并且计算 load_factor
    // 并且预定义一个阈值 load_factor_threshold = 1.0
    // 1 - 64, 用int就行了
    int bits;
    size_t capacity;
    size_t size;
    size_t slot_size;
};

// 用户必须定义自己的list_node类型的list_entry
struct list_entry
{
    // any type of key
    int key;
    // any type of value
    int value;

    list_node_t node;
};

static inline list_node_t *hash_table_init_slots(int bits)
{
    // 有那种分配内存的同时执行初始化的函数吗
    // 一个数 * 2^n == << n, 应该是对的
    list_node_t *slots = (list_node_t *)malloc(sizeof(list_node_t) << bits));
    size_t size = 1 << bits;
    for (size_t i = 0; i < size; ++i)
        list_init_head(&slots[i]);
}

void hash_table_init(hash_table *table, size_t bits)
{
    table->bits = bits;
    size_t table_size = 1 << bits;
    table->slots = hash_table_init_slots(bits);
    // table->size = (1 << bits);
}


// 像这种函数，只能用户来写，程序是没有办法提供的
list_entry *hash_table_find(hash_table *table, int key)
{
    // 不是，为了能在遍历list_node的时候，找到正确的值，这些list_node是必须保存
    // key的，同时这些list_node也必须保存value
    // 不是，是这些list_entry 也必须保存一个key
    size_t index = hash_integer(key, table->bits);
    list_node_t *head = &table->buckets[index];

    // 如果涉及到entry，那么这个函数就必须由用户来写
    // 我们能做的，就是尽量的提供帮助。。。没啥了
    // list_for_each_entry()
    list_entry *entry = nullptr;
    list_for_each_entry(entry, head, list_entry, node)
    {
        if (key == entry->key)
        {
            return entry;
        }
    }
}

// 同时，用户也可以提供一个一起的函数，就是把上面这两个函数结合起来
// user define
static inline void hash_table_insert(hash_table *table, list_entry *entry)
{
    // 因为这个
    hash_table_insert_impl(table, hash_table_index_slot(table, entry->key), &entry->node);
    if (load_factor(table) > load_factor_threshold)
        hash_table_rehash(table);
}


// 还剩下一个删除，
// 这个函数还是只能用户来写，因为我不可能知道key是什么
// 我能做的，就是给你提供一个把node从hashtable里面删掉的简单函数
static inline void hash_table_erase(hash_table *table, int key)
{
    // 首先调用自己编写的find函数
    list_entry *entry = hash_table_find(table, key);
    if (entry)
    {
        // 我需要找到他在那条链表里面
        // 好像没有办法仅调用find, 我们的find还需要返回一个head
        // hash_table_erase_impl(table, entry->node);
        // 这里是不需要的，因为我们删除的一定不是head
        list_erase(entry->node);
    }
}

// 这个函数需要用户提供，因为我们不知道key是什么样子的
// 只不过这里的key都是以int为例
static inline list_node_t *hash_table_index_slot(hash_table *table, int key)
{
    return &table->slots[hash_integer(key, table->bits)];
}

static inline void hash_table_insert_impl(hash_table *table,
                                                  list_node_t *head, list_node_t *node)
{
    if (list_is_empty(head))
        ++(table->slot_size);
    ++(table->size);
    list_insert_after(head, node);
}

static inline double load_factor(hash_table *table)
{
    return (double)(table->size) / (double)(table->slot_size);
}



// 同时呢，我们需要支持一些操作
// 比如，插入，删除，查找等等

// 我们可以提供一些帮助函数，比如用户通过查找发现没有这个值
// 可以直接调用add

static inline void hash_table_move(hash_table *old_table, hash_table *new_table)
{
    // 好像有一个快速的move来着，忘了。。。
    // O, 想起来了，内存拷贝
    // memcpy()
    memcpy(new_table, old_table, sizeof(hash_table));
}



// hash表的遍历是没有顺序的
// @table: hash_table *, the hash table to iterate
// @entry: list_entry *
// @type: list_entry
// @member: the list_node_t member in list_entry
#define hash_table_for_each_entry(table, entry, type, member) \
    for (size_t index = 0; index < table->capacity; ++index) \
        list_for_each_entry(entry, &table->slots[index], type, member)

        // 在这里你可以使用你的entry对象，来获取所有的东西

// 什么是rehash？？
// 其实就是遍历一遍之前的hash表，然后重新插入到一个新的hash表
// 并且这个值是没有重复的，所以我们不需要进行值的检查
// 这个函数同样需要用户进行定义，但是模板是固定的，直接copy就可以了
void hash_table_rehash(hash_table *table)
{
    // 初始化一张hash_table,用来存放被rehash的数据
    hash_table rehash_table;
    hash_table_init(&rehash_table, table->bits + 1);

    // 遍历原来的表，进行一个hash的re
    list_entry *entry = nullptr;
    hash_table_for_each_entry(table, entry, list_entry, node)
    {
        // 用户实现一个不会检查重复和不会引发rehash的插入函数，放在这里
        hash_table_insert(rehash_table, entry);
    }

    // 我们可以释放掉整个数组了
    free(table->slots);
    // 用新的rehashed的table代替旧的
    hash_table_move(table, &rehash_table);
}