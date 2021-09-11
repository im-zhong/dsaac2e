// 2021/9/11
// zhangzhong
// linux kernal rbtree

#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <chrono>
#include <iostream>
#include <set>
#include <cassert>

// 自己实现一个简单的红黑树

struct mytype
{
    struct rb_node node;
    int key;
};

struct mytype *my_search(struct rb_root *root, int key)
{
    struct rb_node *node = root->rb_node;

    while (node)
    {
        struct mytype *data = container_of(node, struct mytype, node);
        int result;

        result = key - data->key;

        if (result < 0)
            node = node->rb_left;
        else if (result > 0)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}

int my_insert(struct rb_root *root, int key)
// int my_insert(struct rb_root *root, struct mytype *data)
{
    struct rb_node **newnode = &(root->rb_node), *parent = NULL;

    /* Figure out where to put newnode node */
    while (*newnode)
    {
        struct mytype *thisnode = container_of(*newnode, struct mytype, node);
        // int result = strcmp(data->keystring, thisnode->keystring);

        int result = key - thisnode->key;

        parent = *newnode;
        if (result < 0)
            newnode = &((*newnode)->rb_left);
        else if (result > 0)
            newnode = &((*newnode)->rb_right);
        else
            return false;
    }

    // 分配一个node
    struct mytype *data = (struct mytype *)malloc(sizeof(struct mytype));
    data->key = key;

    /* Add newnode node and rebalance tree. */
    rb_link_node(&data->node, parent, newnode);
    rb_insert_color(&data->node, root);

    return true;
}

void my_erase(struct rb_root *root, int key)
{
    struct mytype *data = my_search(root, key);

    if (data)
    {
        rb_erase(&data->node, root);
        // myfree(data);
        free(data);
    }
}

// verify
// check vec order
bool check_vec_order(const std::vector<int> &vi)
{
	for (size_t i = 1; i < vi.size(); ++i)
	{
		if (vi[i - 1] < vi[i])
		{
		}
		else
		{
			return false;
		}
	}
	return true;
}

std::vector<int> to_vec(const std::set<int> &iset)
{
	std::vector<int> vi;
	for (const auto i : iset)
	{
		vi.push_back(i);
	}
	return vi;
}

std::vector<int> gen_vec(struct rb_root *head)
{
    // 我需要便利这颗树
    std::vector<int> vi;
    struct rb_node *node;
    for (node = rb_first(head); node; node = rb_next(node))
    {
        int key = ((struct mytype *)(rb_entry(node, struct mytype, node)))->key;
        vi.push_back(key);
    }
    return vi;
}

bool is_vec_same(const std::vector<int> &left, const std::vector<int> &right)
{
	return left == right;
}

void verify_rbtree(int count)
{
	// 1. 生成一个随机数数组

	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> ui(-count, count);

	std::vector<int> random_vec;
	for (int i = 0; i < count; ++i)
	{
		random_vec.push_back(ui(e));
	}

	std::set<int> iset;
	// rbtree head = init_rbtree();
    struct rb_root mytree = RB_ROOT;
    struct rb_root *head = &mytree;
	int number = 0;

	// 2. insert
	std::cout << "verifying insert...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.insert(number);
        my_insert(head, number);

		// check order
		std::vector<int> set_vec = to_vec(iset);
		assert(check_vec_order(set_vec));
		std::vector<int> rbtree_vec = gen_vec(head);
		assert(check_vec_order(rbtree_vec));

		// check the two vec is same
		assert(is_vec_same(set_vec, rbtree_vec));

		std::cout << "pass: " << i + 1 << "/" << random_vec.size() << "\n";
	}

	// 3. insert repeated value
	std::cout << "verifying repeated insert...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.insert(number);
		my_insert(head, number);

		// check order
		std::vector<int> set_vec = to_vec(iset);
		assert(check_vec_order(set_vec));
		std::vector<int> rbtree_vec = gen_vec(head);
		assert(check_vec_order(rbtree_vec));

		// check the two vec is same
		assert(is_vec_same(set_vec, rbtree_vec));

		std::cout << "pass: " << i + 1 << "/" << random_vec.size() << "\n";
	}

	// 4. delete un-exists value
	std::vector<int> sorted_vec = to_vec(iset);
	std::vector<int> unique_vec;
	size_t index = 0;
	for (int i = -count; i < count && index < sorted_vec.size(); ++i)
	{
		if (i == sorted_vec[index])
		{
			++index;
		}
		else
		{
			unique_vec.push_back(i);
		}
	}
	std::cout << "verifying delete non-exists...\n";
	for (size_t i = 0; i < unique_vec.size(); ++i)
	{
		number = unique_vec[i];
		iset.erase(number);
		my_erase(head, number);

		// check order
		std::vector<int> set_vec = to_vec(iset);
		assert(check_vec_order(set_vec));
		std::vector<int> rbtree_vec = gen_vec(head);
		assert(check_vec_order(rbtree_vec));

		// check the two vec is same
        assert(is_vec_same(set_vec, rbtree_vec));

		std::cout << "pass: " << i + 1 << "/" << unique_vec.size() << "\n";
	}

	// 3. delete
	std::cout << "verifying delete...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.erase(number);
		my_erase(head, number);

		// check order
		std::vector<int> set_vec = to_vec(iset);
		assert(check_vec_order(set_vec));
		std::vector<int> rbtree_vec = gen_vec(head);
		assert(check_vec_order(rbtree_vec));

		// check the two vec is same
        assert(is_vec_same(set_vec, rbtree_vec));

		std::cout << "pass: " << i + 1 << "/" << random_vec.size() << "\n";
	}
}

void performance(int count)
{
	// 1. 生成一个随机数数组
	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> ui(-count, count);
	std::vector<int> random_vec;
	for (int i = 0; i < count; ++i)
	{
		random_vec.push_back(ui(e));
	}

	std::set<int> iset;
	struct rb_root mytree = RB_ROOT;
    struct rb_root *head = &mytree;

	uint64_t set_time = 0;
	uint64_t rbtree_time = 0;

	// 2. insert
	set_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	for (const auto i : random_vec)
	{
		iset.insert(i);
	}
	set_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - set_time;
	std::cout << "set insert: " << set_time << "ns" << std::endl;

	rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	for (const auto i : random_vec)
	{
		my_insert(head, i);
	}
	rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
	std::cout << "rbtree insert: " << rbtree_time << "ns" << std::endl;
	std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";

	// 3. delete
	set_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	for (const auto i : random_vec)
	{
		iset.erase(i);
	}
	set_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - set_time;
	std::cout << "set delete: " << set_time << "ns" << std::endl;

	rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	for (const auto i : random_vec)
	{
		my_erase(head, i);
	}
	rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
	std::cout << "rbtree delete: " << rbtree_time << "ns" << std::endl;
	std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";
}


int main(int argc, char *argv[])
{
    // verify_rbtree(10000);

	if (argc > 1)
	{
		performance(std::atoi(argv[1]));
	}

    // 性能测试结果
    // set insert: 944671296ns
    // rbtree insert: 477803349ns
    // rbtree is faster than set by 1.97711
    // set delete: 881104895ns
    // rbtree delete: 445617127ns
    // rbtree is faster than set by 1.97727

    // struct rb_root mytree = RB_ROOT;

    // my_insert(&mytree, 0);

    // struct mytype *node = my_search(&mytree, 0);

    // if (node)
    // {
    //     printf("%d\n", node->key);
    // }

    // my_erase(&mytree, 0);

    // node = my_search(&mytree, 0);

    // if (node)
    // {
    //     printf("%d\n", node->key);
    // }

    return 0;
}