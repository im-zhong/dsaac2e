// 2021/9/14
// zhangzhong
// 由于自顶向下的红黑树的功能过于弱小，所以决定实现一颗自底向上的红黑树
// 参考 算法导论

#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <chrono>
#include <cassert>
#include <fstream>

// 还真是没有办法提供提供合适的接口，只能像linux内核那样
// 让用户自己去写相应的实现了
// 插入，删除，查找
// 其实就是因为没有办法合适的传入一个compare函数
// 这样的好处就是可以避免函数指针的使用

// TODO: 2021/9/16
// 没办法，assign系列函数并没有提升很多的性能，相反，还会导致性能下降，而且让代码混乱
// 直接删掉。。。就可以了


enum rbtree_color
{
    rbtree_red,
    rbtree_black
};

struct rbtree_node
{
    struct rbtree_node *parent;
    struct rbtree_node *left;
    struct rbtree_node *right;
    rbtree_color color;

    int value;
};

using rbtree = rbtree_node *;

static inline bool is_leaf(rbtree_node *head, rbtree_node *node)
{
    return (node->left == head) && (node->right == head);
}

// 在子树中找到最小的值
static inline rbtree_node *rbtree_first(rbtree_node *head, rbtree_node *root)
{
    while (root->left != head)
        root = root->left;
    return root;
}

static inline rbtree_node *rbtree_last(rbtree_node *head, rbtree_node *root)
{
    while (root->right != head)
        root = root->right;
    return root;
}

static inline rbtree_node *rbtree_prev(rbtree_node *head, rbtree_node *node)
{
    if (node->left != head)
    {
        return rbtree_last(head, node->left);
    }

    rbtree_node *parent = node->parent;
    while (parent != head && node == parent->left)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

static inline rbtree_node *rbtree_next(rbtree_node *head, rbtree_node *node)
{
    if (node->right != head)
    {
        return rbtree_first(head, node->right);
    }

    rbtree_node *parent = node->parent;
    while (parent != head && node == parent->right)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

// 这个函数名字应该改成 rbtree_init
static inline void init_rbtree(rbtree_node *head)
{
    head->parent = head;
    head->left = head;
    head->right = head;
    head->color = rbtree_black;
}

static inline void rbtree_left_rotate(rbtree_node *head, rbtree_node *parent)
{
    rbtree_node *right_child = parent->right;
    parent->right = right_child->left;
    if (right_child->left != head)
    {
        right_child->left->parent = parent;
    }

    right_child->parent = parent->parent;
    if (parent == parent->parent->left)
    {
        parent->parent->left = right_child;
    }
    else
    {
        parent->parent->right = right_child;
    }

    right_child->left = parent;
    parent->parent = right_child;
}

static inline void rbtree_left_rotate_assign(rbtree_node *head, rbtree_node *parent, rbtree_node **child)
{
    rbtree_node *right_child = parent->right;
    parent->right = right_child->left;
    if (right_child->left != head)
    {
        right_child->left->parent = parent;
    }

    right_child->parent = parent->parent;
    *child = right_child;

    right_child->left = parent;
    parent->parent = right_child;
}

static inline void rbtree_right_rotate(rbtree_node *head, rbtree_node *parent)
{
    rbtree_node *left_child = parent->left;
    parent->left = left_child->right;
    if (left_child->right != head)
    {
        left_child->right->parent = parent;
    }

    left_child->parent = parent->parent;
    if (parent == parent->parent->left)
    {
        parent->parent->left = left_child;
    }
    else
    {
        parent->parent->right = left_child;
    }

    left_child->right = parent;
    parent->parent = left_child;
}

static inline void rbtree_right_rotate_assign(rbtree_node *head, rbtree_node *parent, rbtree_node **child)
{
    rbtree_node *left_child = parent->left;
    parent->left = left_child->right;
    if (left_child->right != head)
    {
        left_child->right->parent = parent;
    }

    left_child->parent = parent->parent;
    *child = left_child;

    left_child->right = parent;
    parent->parent = left_child;
}

static inline void rbtree_color_flip(rbtree_node *parent)
{
    parent->color = rbtree_red;
    parent->left->color = rbtree_black;
    parent->right->color = rbtree_black;
}

static inline void rbtree_insert_fixup(rbtree_node *head, rbtree_node *node)
{
    rbtree_node *uncle = nullptr;
    while (node->parent->color == rbtree_red)
    {
        if (node->parent == node->parent->parent->left)
        {
            uncle = node->parent->parent->right;
            // case 1:
            //       pp         (PP)
            //   (p)    (u)    p     u
            // (n)           (n)
            if (uncle->color == rbtree_red)
            {
                rbtree_color_flip(node->parent->parent);
                node = node->parent->parent;
            }
            else
            {
                // case 2
                //      pp           pp
                //   (p)   u     (n)    u
                //     (n)     (p)
                if (node == node->parent->right)
                {
                    node = node->parent;
                    // rbtree_left_rotate_assign(head, node, &node->parent->left);
                    rbtree_left_rotate(head, node);
                }
                // case 3:
                //        pp
                //    (p)
                // (n)
                node->parent->color = rbtree_black;
                node->parent->parent->color = rbtree_red;
                rbtree_right_rotate(head, node->parent->parent);
            }
        }
        else // node->parent == node->parent->parent->right
        {
            uncle = node->parent->parent->left;
            if (uncle->color == rbtree_red)
            {
                rbtree_color_flip(node->parent->parent);
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    // rbtree_right_rotate_assign(head, node, &node->parent->right);
                    rbtree_right_rotate(head, node);
                }
                node->parent->color = rbtree_black;
                node->parent->parent->color = rbtree_red;
                rbtree_left_rotate(head, node->parent->parent);
            }
        }
    }

    head->right->color = rbtree_black;
}

static inline void rbtree_insert_fixup_1(rbtree_node *head, rbtree_node *node)
{
    rbtree_node *uncle = nullptr;
    while (node->parent->color == rbtree_red)
    {
        if (node->parent == node->parent->parent->left)
        {
            uncle = node->parent->parent->right;
            // case 1:
            //       pp         (PP)
            //   (p)    (u)    p     u
            // (n)           (n)
            if (uncle->color == rbtree_red)
            {
                rbtree_color_flip(node->parent->parent);
                node = node->parent->parent;
            }
            else
            {
                // case 2
                //      pp           pp
                //   (p)   u     (n)    u
                //     (n)     (p)
                if (node == node->parent->right)
                {
                    node = node->parent;
                    rbtree_left_rotate_assign(head, node, &node->parent->left);
                    // rbtree_left_rotate(head, node);
                }
                // case 3:
                //        pp
                //    (p)
                // (n)
                node->parent->color = rbtree_black;
                node->parent->parent->color = rbtree_red;
                rbtree_right_rotate(head, node->parent->parent);
            }
        }
        else // node->parent == node->parent->parent->right
        {
            uncle = node->parent->parent->left;
            if (uncle->color == rbtree_red)
            {
                rbtree_color_flip(node->parent->parent);
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    rbtree_right_rotate_assign(head, node, &node->parent->right);
                    // rbtree_right_rotate(head, node);
                }
                node->parent->color = rbtree_black;
                node->parent->parent->color = rbtree_red;
                rbtree_left_rotate(head, node->parent->parent);
            }
        }
    }

    head->right->color = rbtree_black;
}

static inline void rbtree_insert_impl(rbtree_node *head, rbtree_node *node)
{
    // empty tree, directly insert a black root
    if (head->right == head)
    {
        // empty tree, insert node and make it black
        head->right = node;
        node->parent = head;
        node->left = head;
        node->right = head;
        node->color = rbtree_black;
        return;
    }

    // not empty tree, find the positon to insert

    // get the root node
    // 我们保证root一定存在，就可以依赖下面的逻辑
    rbtree_node *parent = head;
    rbtree_node *work = head->right;
    while (true)
    {
        parent = work;
        // 用户自定义的比较函数可以放在这里
        int compare = node->value - work->value;
        if (compare < 0)
        {
            work = work->left;
            if (work == head)
            {
                parent->left = node;
                break;
            }
        }
        else if (compare > 0)
        {
            work = work->right;
            if (work == head)
            {
                parent->right = node;
                break;
            }
        }
        else
        {
            // we found the node, do not insert duplicate
            return;
        }
    }

    node->parent = parent;
    node->left = head;
    node->right = head;
    node->color = rbtree_red;
    rbtree_insert_fixup(head, node);
}

static inline void rbtree_insert_impl_1(rbtree_node *head, rbtree_node *node)
{
    // empty tree, directly insert a black root
    if (head->right == head)
    {
        // empty tree, insert node and make it black
        head->right = node;
        node->parent = head;
        node->left = head;
        node->right = head;
        node->color = rbtree_black;
        return;
    }

    // not empty tree, find the positon to insert

    // get the root node
    // 我们保证root一定存在，就可以依赖下面的逻辑
    rbtree_node *parent = head;
    rbtree_node *work = head->right;
    while (true)
    {
        parent = work;
        // 用户自定义的比较函数可以放在这里
        int compare = node->value - work->value;
        if (compare < 0)
        {
            work = work->left;
            if (work == head)
            {
                parent->left = node;
                break;
            }
        }
        else if (compare > 0)
        {
            work = work->right;
            if (work == head)
            {
                parent->right = node;
                break;
            }
        }
        else
        {
            // we found the node, do not insert duplicate
            return;
        }
    }

    node->parent = parent;
    node->left = head;
    node->right = head;
    node->color = rbtree_red;
    rbtree_insert_fixup_1(head, node);
}

static inline void rbtree_insert(rbtree_node *head, int value)
{
    // create a new rbtree node
    rbtree_node *node = (rbtree_node *)malloc(sizeof(rbtree_node));
    node->value = value;
    rbtree_insert_impl(head, node);
}

static inline void rbtree_insert_1(rbtree_node *head, int value)
{
    // create a new rbtree node
    rbtree_node *node = (rbtree_node *)malloc(sizeof(rbtree_node));
    node->value = value;
    rbtree_insert_impl_1(head, node);
}

// 这个动作仅仅修改head的孩子指针和new_node的父指针
static inline void rbtree_transplant(rbtree_node *head,
                                     rbtree_node *old_node, rbtree_node *new_node)
{
    old_node == old_node->parent->left ? old_node->parent->left = new_node : old_node->parent->right = new_node;
    new_node->parent = old_node->parent;
}

static inline void rbtree_transplant_assign(rbtree_node *head, rbtree_node *old_node, rbtree_node *new_node, rbtree_node **child)
{
    *child = new_node;
    new_node->parent = old_node->parent;
}

// 从node开始往上修复node
void rbtree_erase_fixup(rbtree_node *head, rbtree_node *node)
{
    // 这里是不是不用每次都判断node是不是root
    // 是不是只需要一开始的时候判断??
    while (node != head->right && node->color == rbtree_black)
    {
        // x is black
        if (node == node->parent->left)
        {
            rbtree_node *sibling = node->parent->right;
            if (sibling->color == rbtree_red)
            {
                node->parent->color = rbtree_red;
                sibling->color = rbtree_black;
                rbtree_left_rotate(head, node->parent);
                sibling = node->parent->right;
            }

            // x is black, sibling is black
            // sibling->child may be red or black
            // there are 3 cases

            // case 2
            if (sibling->left->color == rbtree_black && sibling->right->color == rbtree_black)
            {
                sibling->color = rbtree_red;
                node = node->parent;
            }
            else
            {
                // case 3
                if (sibling->left->color == rbtree_red)
                {
                    sibling->left->color = rbtree_black;
                    sibling->color = rbtree_red;
                    // rbtree_right_rotate_assign(head, sibling, &node->parent->right);
                    rbtree_right_rotate(head, sibling);
                    sibling = node->parent->right;
                }

                // case 4
                // sibling->right->color == rbtree_red
                sibling->color = node->parent->color;
                node->parent->color = rbtree_black;
                sibling->right->color = rbtree_black;
                rbtree_left_rotate(head, node->parent);
                break;
            }
        }
        else // node == parent->right, same as above but symmetry
        {
            if (node == node->parent->right)
            {
                rbtree_node *sibling = node->parent->left;
                if (sibling->color == rbtree_red)
                {
                    node->parent->color = rbtree_red;
                    sibling->color = rbtree_black;
                    rbtree_right_rotate(head, node->parent);
                    sibling = node->parent->left;
                }
                if (sibling->left->color == rbtree_black && sibling->right->color == rbtree_black)
                {
                    sibling->color = rbtree_red;
                    node = node->parent;
                }
                else
                {
                    if (sibling->right->color == rbtree_red)
                    {
                        sibling->right->color = rbtree_black;
                        sibling->color = rbtree_red;
                        // rbtree_left_rotate_assign(head, sibling, &node->parent->left);
                        rbtree_left_rotate(head, sibling);
                        sibling = node->parent->left;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = rbtree_black;
                    sibling->left->color = rbtree_black;
                    rbtree_right_rotate(head, node->parent);
                    break;
                }
            }
        }
    }

    // 如果node的颜色是红色，直接让他变成黑色
    node->color = rbtree_black;
}

void rbtree_erase_fixup_1(rbtree_node *head, rbtree_node *node)
{
    // 这里是不是不用每次都判断node是不是root
    // 是不是只需要一开始的时候判断??
    while (node != head->right && node->color == rbtree_black)
    {
        // x is black
        if (node == node->parent->left)
        {
            rbtree_node *sibling = node->parent->right;
            if (sibling->color == rbtree_red)
            {
                node->parent->color = rbtree_red;
                sibling->color = rbtree_black;
                rbtree_left_rotate(head, node->parent);
                sibling = node->parent->right;
            }

            // x is black, sibling is black
            // sibling->child may be red or black
            // there are 3 cases

            // case 2
            if (sibling->left->color == rbtree_black && sibling->right->color == rbtree_black)
            {
                sibling->color = rbtree_red;
                node = node->parent;
            }
            else
            {
                // case 3
                if (sibling->left->color == rbtree_red)
                {
                    sibling->left->color = rbtree_black;
                    sibling->color = rbtree_red;
                    rbtree_right_rotate_assign(head, sibling, &node->parent->right);
                    // rbtree_right_rotate(head, sibling);
                    sibling = node->parent->right;
                }

                // case 4
                // sibling->right->color == rbtree_red
                sibling->color = node->parent->color;
                node->parent->color = rbtree_black;
                sibling->right->color = rbtree_black;
                rbtree_left_rotate(head, node->parent);
                break;
            }
        }
        else // node == parent->right, same as above but symmetry
        {
            if (node == node->parent->right)
            {
                rbtree_node *sibling = node->parent->left;
                if (sibling->color == rbtree_red)
                {
                    node->parent->color = rbtree_red;
                    sibling->color = rbtree_black;
                    rbtree_right_rotate(head, node->parent);
                    sibling = node->parent->left;
                }
                if (sibling->left->color == rbtree_black && sibling->right->color == rbtree_black)
                {
                    sibling->color = rbtree_red;
                    node = node->parent;
                }
                else
                {
                    if (sibling->right->color == rbtree_red)
                    {
                        sibling->right->color = rbtree_black;
                        sibling->color = rbtree_red;
                        rbtree_left_rotate_assign(head, sibling, &node->parent->left);
                        // rbtree_left_rotate(head, sibling);
                        sibling = node->parent->left;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = rbtree_black;
                    sibling->left->color = rbtree_black;
                    rbtree_right_rotate(head, node->parent);
                    break;
                }
            }
        }
    }

    // 如果node的颜色是红色，直接让他变成黑色
    node->color = rbtree_black;
}

void rbtree_erase_impl(rbtree_node *head, rbtree_node *node)
{
    // rbtree_node *y = node;

    // 实际上x 是一个fixup_node
    rbtree_color color = node->color;
    rbtree_node *fixup_node = nullptr;
    if (node->left == head)
    {
        // 此时node_right也可能是head啊
        // 注意fixup_node的父节点被正确的设置为node
        fixup_node = node->right;
        rbtree_transplant(head, node, node->right);
        // 但是我们不需要重置node->right的左右指针
    }
    else if (node->right == head)
    {
        fixup_node = node->left;
        rbtree_transplant(head, node, node->left);
    }
    else
    {
        // have two child
        // find right min
        rbtree_node *right_first = rbtree_first(head, node->right);
        color = right_first->color;
        fixup_node = right_first->right;
        // 这里还有两个情况要处理，
        // case 4.1 : right_first 是node->right
        // case 4.2: right_first 不是 node->right
        if (right_first->parent == node)
        {
            fixup_node->parent = right_first;
        }
        else
        {
            // rbtree_transplant_assign(head, right_first, right_first->right, &right_first->parent->left);
            rbtree_transplant(head, right_first, right_first->right);
            right_first->right = node->right;
            right_first->right->parent = right_first;
        }
        rbtree_transplant(head, node, right_first);
        right_first->left = node->left;
        right_first->left->parent = right_first;
        right_first->color = node->color;
    }

    if (color == rbtree_black)
        rbtree_erase_fixup(head, fixup_node);
}

void rbtree_erase_impl_1(rbtree_node *head, rbtree_node *node)
{
    // rbtree_node *y = node;

    // 实际上x 是一个fixup_node
    rbtree_color color = node->color;
    rbtree_node *fixup_node = nullptr;
    if (node->left == head)
    {
        // 此时node_right也可能是head啊
        // 注意fixup_node的父节点被正确的设置为node
        fixup_node = node->right;
        rbtree_transplant(head, node, node->right);
        // 但是我们不需要重置node->right的左右指针
    }
    else if (node->right == head)
    {
        fixup_node = node->left;
        rbtree_transplant(head, node, node->left);
    }
    else
    {
        // have two child
        // find right min
        rbtree_node *right_first = rbtree_first(head, node->right);
        color = right_first->color;
        fixup_node = right_first->right;
        // 这里还有两个情况要处理，
        // case 4.1 : right_first 是node->right
        // case 4.2: right_first 不是 node->right
        if (right_first->parent == node)
        {
            fixup_node->parent = right_first;
        }
        else
        {
            rbtree_transplant_assign(head, right_first, right_first->right, &right_first->parent->left);
            // rbtree_transplant(head, right_first, right_first->right);
            right_first->right = node->right;
            right_first->right->parent = right_first;
        }
        rbtree_transplant(head, node, right_first);
        right_first->left = node->left;
        right_first->left->parent = right_first;
        right_first->color = node->color;
    }

    if (color == rbtree_black)
        rbtree_erase_fixup_1(head, fixup_node);
}

static inline rbtree_node *rbtree_find(rbtree_node *head, int value)
{
    rbtree_node *work = head->right;
    int compare_result = 0;
    while (work != head)
    {
        compare_result = value - work->value;
        if (compare_result < 0)
            work = work->left;
        else if (compare_result > 0)
            work = work->right;
        else
            break;
    }
    return work;
}

void rbtree_erase(rbtree_node *head, int value)
{
    rbtree_node *node = rbtree_find(head, value);
    if (node != head)
    {
        rbtree_erase_impl(head, node);
        free(node);
    }
}

void rbtree_erase_1(rbtree_node *head, int value)
{
    rbtree_node *node = rbtree_find(head, value);
    if (node != head)
    {
        rbtree_erase_impl_1(head, node);
        free(node);
    }
}

// test
void do_print(rbtree head, rbtree root)
{
    if (root != head)
    {
        do_print(head, root->left);
        // std::cout << "{ " << root->element << ", " << root->color << "}, ";
        std::cout << root->value << ", ";
        do_print(head, root->right);
    }
}

void print(rbtree head)
{
    do_print(head, head->right);
    std::cout << "\n";
}

void do_gen_vec(rbtree head, rbtree root, std::vector<int> &vi)
{
    if (root != head)
    {
        do_gen_vec(head, root->left, vi);
        vi.push_back(root->value);
        do_gen_vec(head, root->right, vi);
    }
}

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

std::vector<int> gen_vec(rbtree head)
{
    std::vector<int> vi;
    do_gen_vec(head, head->right, vi);
    return vi;
}

bool is_rbtree_impl(int &last_black_height, int current_black_height, rbtree_node *head, rbtree_node *root)
{
    if (root == head)
        return true;

    // 不能有两个连续的黑色节点
    if (root->color == rbtree_red)
    {
        if (root->left->color == rbtree_red || root->right->color == rbtree_red)
        {
            std::cout << "have two continuous red node\n";
            return false;
        }
    }

    // 如果本节点是黑色节点，黑高++
    if (root->color == rbtree_black)
        current_black_height++;

    // 检测黑高

    // 更新current_black_height

    // 检查本节点是否是叶子节点
    // 如果是叶子节点，需要计算新的黑高，并与之前的黑高做一个比较
    if (is_leaf(head, root))
    {
        if (last_black_height == 0)
        {
            last_black_height = current_black_height;
        }
        else
        {
            if (last_black_height == current_black_height)
            {
                // std::cout << "current black height: " << current_black_height << std::endl;
                // std::cout << "last black height: " << last_black_height << std::endl;
                // 正常
            }
            else
            {
                std::cout << "different black height at " << root->value << "\n";
                std::cout << "current black height: " << current_black_height << std::endl;
                std::cout << "last black height: " << last_black_height << std::endl;
                return false;
            }
        }
    }
    else
    {
        // 如果不是叶子，就继续往下调用本函数就可以了
        if (!is_rbtree_impl(last_black_height, current_black_height, head, root->left))
            return false;
        if (!is_rbtree_impl(last_black_height, current_black_height, head, root->right))
            return false;
    }

    return true;
}

bool is_rbtree(rbtree_node *T, int &black_height)
{
    // 1. root is black
    if (T->right->color != rbtree_black)
    {
        return false;
    }

    // 2. 颜色要么是黑 要么是红

    // 3. 不能有两个连续的红色节点
    // 4. 任意一个节点的左右黑高是一样的
    // 这两个检测可以在递归的过程中实现

    black_height = 0;
    return is_rbtree_impl(black_height, 0, T, T->right);
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

bool is_vec_same(const std::vector<int> &left, const std::vector<int> &right)
{
    return left == right;
}

void print_error_env(const std::vector<int> &random_vec, size_t index)
{
    std::cout << "error index: " << index << std::endl;
    std::cout << "error number: " << random_vec[index] << std::endl;

    std::ofstream fout("random.vec", std::ios::trunc);
    for (const auto i : random_vec)
    {
        fout << i << std::endl;
    }
    fout.close();
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
    struct rbtree_node tree_head;
    init_rbtree(&tree_head);
    rbtree head = &tree_head;
    int number = 0;

    // 2. insert
    std::cout << "verifying insert...\n";
    for (size_t i = 0; i < random_vec.size(); ++i)
    {
        number = random_vec[i];
        iset.insert(number);
        rbtree_insert(head, number);

        // check is rbtree
        int black_height = 0;
        assert(is_rbtree(head, black_height));

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
    // std::cout << "verifying repeated insert...\n";
    // for (size_t i = 0; i < random_vec.size(); ++i)
    // {
    //     number = random_vec[i];
    //     iset.insert(number);
    //     rbtree_insert(head, number);

    //     // check is rbtree
    //     int black_height = 0;
    //     assert(is_rbtree(head, black_height));

    //     // check order
    //     std::vector<int> set_vec = to_vec(iset);
    //     assert(check_vec_order(set_vec));
    //     std::vector<int> rbtree_vec = gen_vec(head);
    //     assert(check_vec_order(rbtree_vec));

    //     // check the two vec is same
    //     assert(is_vec_same(set_vec, rbtree_vec));

    //     std::cout << "pass: " << i + 1 << "/" << random_vec.size() << "\n";
    // }

    // 4. delete un-exists value
    // std::vector<int> sorted_vec = to_vec(iset);
    // std::vector<int> unique_vec;
    // size_t index = 0;
    // for (int i = -count; i < count && index < sorted_vec.size(); ++i)
    // {
    //     if (i == sorted_vec[index])
    //     {
    //         ++index;
    //     }
    //     else
    //     {
    //         unique_vec.push_back(i);
    //     }
    // }
    // std::cout << "verifying delete non-exists...\n";
    // for (size_t i = 0; i < unique_vec.size(); ++i)
    // {
    //     number = unique_vec[i];
    //     iset.erase(number);
    //     rbtree_erase(head, number);

    //     // check is rbtree
    //     int black_height = 0;
    //     // 目前还不是红黑树，就不检测这个了
    //     // assert(is_rbtree(head, black_height));
    //     if (!is_rbtree(head, black_height))
    //     {
    //         // 还原现场，方便调试
    //         print_error_env(unique_vec, i);
    //         assert(is_rbtree(head, black_height));
    //     }

    //     // check order
    //     std::vector<int> set_vec = to_vec(iset);
    //     assert(check_vec_order(set_vec));
    //     std::vector<int> rbtree_vec = gen_vec(head);
    //     //
    //     if (!check_vec_order(rbtree_vec))
    //     {
    //         print_error_env(unique_vec, i);
    //         assert(check_vec_order(rbtree_vec));
    //     }

    //     // check the two vec is same

    //     if (!is_vec_same(set_vec, rbtree_vec))
    //     {
    //         print_error_env(unique_vec, i);
    //         assert(is_vec_same(set_vec, rbtree_vec));
    //     }

    //     std::cout << "pass: " << i + 1 << "/" << unique_vec.size() << "\n";
    // }

    // 5. delete
    std::cout << "verifying delete...\n";
    for (size_t i = 0; i < random_vec.size(); ++i)
    {
        number = random_vec[i];
        iset.erase(number);
        rbtree_erase(head, number);

        // check is rbtree
        int black_height = 0;
        // assert(is_rbtree(head, black_height));
        if (!is_rbtree(head, black_height))
        {
            // 还原现场，方便调试
            print_error_env(random_vec, i);
            assert(is_rbtree(head, black_height));
        }

        // check order
        std::vector<int> set_vec = to_vec(iset);
        assert(check_vec_order(set_vec));
        std::vector<int> rbtree_vec = gen_vec(head);
        //
        if (!check_vec_order(rbtree_vec))
        {
            print_error_env(random_vec, i);
            assert(check_vec_order(rbtree_vec));
        }

        // check the two vec is same

        if (!is_vec_same(set_vec, rbtree_vec))
        {
            print_error_env(random_vec, i);
            assert(is_vec_same(set_vec, rbtree_vec));
        }

        std::cout << "pass: " << i + 1 << "/" << random_vec.size() << "\n";
    }
}

// performance
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
    rbtree_node tree_head;
    init_rbtree(&tree_head);
    rbtree head = &tree_head;
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
        rbtree_insert(head, i);
    }
    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
    std::cout << "rbtree insert: " << rbtree_time << "ns" << std::endl;
    std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";

    rbtree_node tree_head_1;
    init_rbtree(&tree_head_1);
    rbtree head_1 = &tree_head_1;

    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    for (const auto i : random_vec)
    {
        // 得出结论，在数据量为100w量级时，显著的快，所以保留这个设计
        // 过了一个晚上，，，这个提升又没了。。。不知道为什么
        rbtree_insert_1(head_1, i);
    }
    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
    std::cout << "rbtree insert_1: " << rbtree_time << "ns" << std::endl;
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
        rbtree_erase(head, i);
    }
    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
    std::cout << "rbtree delete: " << rbtree_time << "ns" << std::endl;
    std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";


    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    for (const auto i : random_vec)
    {
        rbtree_erase_1(head_1, i);
    }
    rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
    std::cout << "rbtree delete_1: " << rbtree_time << "ns" << std::endl;
    std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";

}

int main(int argc, char *argv[])
{
    // verify_rbtree(100000);

    if (argc > 1)
    {
        // verify_rbtree(std::atoi(argv[1]));
        performance(std::atoi(argv[1]));
    }

    return 0;
}