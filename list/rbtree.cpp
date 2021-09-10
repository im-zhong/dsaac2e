#include <iostream>
#include <chrono>
#include <random>
#include <cassert>
#include <map>
#include <fstream>
#include <stack>
#include <map>
#include <set>
#include <cstring>

// TODO: 完成一个交互式的树结构CLI，支持插入删除等操作，并且需要实时更新绘制出来的图片来展示树的结构
// TODO: 二叉树的深度优先搜索和广度优先搜索
// 深度优先搜索应该用栈来实现，而广度优先搜索，应该用队列来实现
// 不对啊，二叉树的深度优先搜索遍历就是先序遍历啊
// 只有广度优先搜索需要用队列去实现

enum Color
{
	kBlack,
	kRed
};

struct rbtree_node
{
	int element;
	rbtree_node *left;
	rbtree_node *right;
	Color color;
};

// rotate
// 就这么简单??
// X P GP GGP
// 旋转发生在X P GP之间,实际上传入的是GP节点
// 同时我们也需要GGP节点,因为旋转完成之后，产生了new_root，需要更新GGP的指针
// GP P X
//     GP  	|     P
//   P     	|  X    GP
// X		|
rbtree_node *left_single_rotate(rbtree_node *parent)
{
	rbtree_node *new_root = parent->left;
	parent->left = new_root->right;
	new_root->right = parent;
	return new_root;
}

// GP P X
// GP		|      P
//    P		|	GP	 X
//      X	|
rbtree_node *right_single_rotate(rbtree_node *parent)
{
	rbtree_node *new_root = parent->right;
	parent->right = new_root->left;
	new_root->left = parent;
	return new_root;
}

// parent = k3
// 就TM这么简单
// GP P X
//     GP	|	  X
//  P		|	P	GP
//      X	|
rbtree_node *left_double_rotate(rbtree_node *parent)
{
	parent->left = right_single_rotate(parent->left);
	return left_single_rotate(parent);
}

// GP P X
// GP		|		X
// 	  P		|	GP		P
//  X		|
rbtree_node *right_double_rotate(rbtree_node *parent)
{
	parent->right = left_single_rotate(parent->right);
	return right_single_rotate(parent);
}

using rbtree = rbtree_node *;

// head & nullnode
rbtree nullnode = nullptr;

// X 需要插入的节点
// P 需要插入的节点的parent
// GP grand parent
// GGP	grand grand parent
rbtree_node *X, *P, *GP, *GGP;

// 反转颜色
void flip_color(int value, rbtree_node *T)
{
	// 在反转颜色的时候，我们需要一些父节点
	X->color = kRed;
	X->left->color = kBlack;
	X->right->color = kBlack;

	// 我们现在需要判断是否需要进行旋转
	// 也就是判断X的父节点P是否是红色
	if (P->color == kRed)
	{
		// 在 X P GP之间发生旋转
		// 一共有四种情况，两种对称情况
		// 单旋转: 左左 右右
		// 双旋转: 左右 右左

		// 这一部分可以组织成一个函数，然后返回new_root
		rbtree_node *new_root = nullptr;
		// 如何判断是否是双旋转呢
		// 通过方向可以进行判断
		if ((value < P->element) != (value < GP->element))
		{
			// 只要两者的方向不相等，就一定是双旋转
			// 否则只需要单旋转
			// 在双旋转的时候，我们也需要判断是 左右 还是 右左

			// 双旋转相当于两次单旋转，我们可以在这里实施一次单旋转，然后在if后面和需要进行的单旋转进行合并
			// 实际编程的时候，我感觉不需要这个trick，这会应该代码的可读性和理解性
			if (P->element < GP->element && X->element > P->element)
			{
				// 左右双旋转
				new_root = left_double_rotate(GP);
			}
			else
			{
				new_root = right_double_rotate(GP);
			}
		}
		else
		{
			// 单旋转
			// 判断方向
			if (P->element < GP->element)
			{
				new_root = left_single_rotate(GP);
			}
			else
			{
				new_root = right_single_rotate(GP);
			}

			// 调整颜色
		}

		// 根据我的逻辑推理，颜色可以在最后统一完成处理
		// 在这里还需要调整颜色
		new_root->color = kBlack;
		GP->color = kRed;

		// 旋转完成之后，需要重置GGP的指针
		if (value < GGP->element)
		{
			GGP->left = new_root;
		}
		else
		{
			GGP->right = new_root;
		}
	}

	// 在flip_color的最后，书中同时设置的root->color = black
	// 这是有必要的吗??
	// 我感觉我这里的逻辑和书里面是不一样的
	// 我这里是不需要将root置为black的
	T->right->color = kBlack;
}

// 初始化，需要为nullnode和head分配空间
rbtree init_rbtree()
{
	// 这个nullnode只需要一个就行了
	// 但是这个是用来干嘛的呢??
	if (nullnode == nullptr)
	{
		nullnode = new rbtree_node;
		nullnode->color = kBlack;
		nullnode->element = std::numeric_limits<int>::max();
		nullnode->left = nullnode->right = nullnode;
	}

	rbtree head = nullptr;
	head = new rbtree_node;
	head->color = kBlack;
	head->element = std::numeric_limits<int>::min();
	head->left = head->right = nullnode;
	return head;
}

// 书中的例子中的插入不是递归实现的
rbtree_node *insert(int value, rbtree_node *T)
{
	X = P = GP = T;

	nullnode->element = value;

	// descend down the tree ???
	while (X->element != value)
	{
		// 在这里记录节点信息，就像栈一样
		GGP = GP;
		GP = P;
		P = X;

		if (value < X->element)
		{
			X = X->left;
		}
		else
		{
			X = X->right;
		}

		if (X->left->color == kRed && X->right->color == kRed)
		{
			// flip color
			flip_color(value, T);

			// 还有一个非常重要的问题，反转颜色并且旋转之后，指针的位置就变了
			// 或者说 X P GP GGP的位置都不再正确
			// 这个需要如何处理呢?
		}
	}

	if (X != nullnode)
	{
		// duplicate 重复？？
		// 如果不是nullnode的意思是，我们找到了一个 X->element == value的节点
		// 当然就是重复
		// 所以实际上所有的节点的空指针都是指向nullnode的，而不是一个nullptr
		return nullnode;
	}

	// 我们找到了一个nullnode
	// 这个地方就是我们需要插入的地方
	// 我们为X分配一个新的节点
	X = new rbtree_node;
	X->element = value;
	// 分配的新的节点都让它的left和right指向nullnode
	X->left = X->right = nullnode;

	if (value < P->element)
		P->left = X;
	else
		P->right = X;

	// 插入节点之后，我们让新的节点是红色
	// 同时我们也需要判断其父亲节点是否为红色
	// 这同样可能触发一次flip_color过程，直接复用
	flip_color(value, T);

	return T;
}

// 可以写一个普通的二叉排序树的insert进行参考
// 这个T实际上是一个头吧，只不过头可以作为真正的节点来考虑
void binary_insert(int value, rbtree_node *T)
{
	// 首先要找到需要插入的地方
	// 这个T一定是一个head
	// 而head->element = INT_MIN, 也就是负无穷
	// 如果你没有一个head来编程的话，一开始是没有parent的，这样就造成了编程上的困难
	rbtree_node *work = T;
	rbtree_node *parent = work;

	// 这句话是为什么??
	// 所以这里需要设置,为的是在work指向nullnode的时候，可以跳出循环
	nullnode->element = value;
	while (work->element != value)
	{
		// 在循环logn次之后，如果是一个新的值，那么work将指向nullnode
		// 一旦指向nullnode，我们需要跳出循环，所以我们要让 work->element == value
		// 也就是 nullnode->element == value
		parent = work;
		if (value < work->element)
			work = work->left;
		else
			work = work->right;
		// 我觉得有必要解释以上上面的代码
		// 在一开始的时候，T是head
		// 而work = parent = T = head
		// 循环开始判断 head->element == value, 因为head->element是负无穷，自然不相等
		// 进入循环体，parent = work = head
		// 而work一定等于root, 因为value一定不小于负无穷, work = head->right = root

		// 然后我们判断root的两个孩子是不是都是红色的
		// 这就是用head和nullnode的好处，省了很多逻辑判断，让代码更简洁
		// 我们需要做颜色反转,根据反转的规则，我们需要查看我们的两个孩子是不是都是红色的
		if (work->left->color == kRed && work->right->color == kRed)
			flip_color(value, T);
	}

	if (work != nullnode)
	{
		// 重复的值
	}

	// make new node
	work = new rbtree_node;
	work->element = value;
	work->left = work->right = nullnode;
	// 因为目前不考虑颜色，所以让所有的节点的颜色都是黑色
	// get its parent
	if (work->element < parent->element)
		parent->left = work;
	else
		parent->right = work;
}

// 找到head的下一个节点
// 因为我们没有parent指针，所以无法正确实现这个函数
// 原因可以见 算法导论 p164 讨论
// 我们只能基于一个假设
// 这个后继节点一定在head的右子树中
// 幸运的是，这个假设在特定的情境下是成立的,只要head的右子树一定存在即可
std::pair<rbtree_node *, rbtree_node *> binary_next(rbtree_node *head)
{
	// if (head->right == nullnode)
	// {
	// 	return nullptr;
	// }

	// 如果存在右子树, 那么肯定是这个右子树最小的值
	// 也就是一直往左
	rbtree_node *parent = head;
	head = head->right;
	while (head->left != nullnode)
	{
		parent = head;
		head = head->left;
	}
	return {parent, head};
}

// 找到node的前一个节点
// @binary_next
rbtree_node *binary_prev(rbtree_node *head)
{
	if (head->left == nullnode)
	{
		return nullptr;
	}

	head = head->left;
	while (head->right != nullnode)
		head = head->left;
	return head;
}

rbtree_node *binary_delete(int value, rbtree_node *T)
{
	// 在删除之前，我们需要找到这个节点
	// head
	rbtree_node *work = T;
	rbtree_node *parent = work;

	// 这里的逻辑应该和insert的逻辑是一样的
	// 只有处理的逻辑不同
	nullnode->element = value;
	while (work->element != value)
	{
		parent = work;
		if (value < work->element)
		{
			work = work->left;
		}
		else
		{
			work = work->right;
		}
	}

	rbtree_node *to_delete = nullptr;

	// 我们检查我们是否找到了这个节点
	if (work != nullnode)
	{
		// we found the node
		// so we need to delete it

		// case 1, work is leaf
		if (work->left == nullnode && work->right == nullnode)
		{
			// 直接删除，然后重置parent的指针
			to_delete = work;
			if (work->element < parent->element)
			{
				parent->left = nullnode;
			}
			else
			{
				parent->right = nullnode;
			}
		}
		// case 2, work only have one child
		else if (work->left != nullnode && work->right == nullnode)
		{
			// only have left child, just let the child be work itself
			to_delete = work;
			if (work->element < parent->element)
				parent->left = work->left;
			else
				parent->right = work->left;
		}
		else if (work->right != nullnode && work->left == nullnode)
		{
			// only have right child
			to_delete = work;
			if (work->element < parent->element)
				parent->left = work->right;
			else
				parent->right = work->right;
		}
		else if (work->left != nullnode && work->right != nullnode)
		{
			// 不对，这里有一个问题，如果右子树只有一个值的时候！！！

			// work->left != nullnode && work->right != nullnode

			// 找到work的后继，其实就是work的右子树中的最小值

			// 我们需要一个函数，这个函数根据给定的节点，找到它的前驱prev和后继next
			// 因为work的右子树一定存在，所以这个调用是正确的
			rbtree_node *successor_parent = nullptr;
			rbtree_node *successor = nullptr;
			auto node_pair = binary_next(work);
			successor_parent = node_pair.first;
			successor = node_pair.second;

			// successor一定没有左孩子，但是它可能有右孩子
			// 判断successor一定是successor_parent的左孩子
			// 我们需要用successor的右孩子代替successor_parent的左孩子

			// 如果work的右子树只有一个节点，那么返回的successor_parent就是work
			// 在这种情况下
			// 好像是必须区分这种情况的，没办法避免， 在算法导论 p166-167中讨论了删除的四种情况
			// 其中第三种和第四种分别对应这里的两个分支
			if (successor_parent == work)
			{
				// 在这种情况下，直接用successor替换work
				if (successor->element < parent->element)
					parent->left = successor;
				else
					parent->right = successor;

				successor->left = successor_parent->left;
			}
			else
			{
				successor_parent->left = successor->right;

				// 首先在work的右子树中删掉successor
				// 如果有parent指针的话，这个过程相当好实现
				// 如果没有的话，这个binary_next必须返回success的parent指针才行.

				// 让successor代替work的位置
				// 我发现每次需要修改parent的child指针的时候，都需要重新判断是left还是right
				// 这个可以优化一下吗?
				if (successor->element < parent->element)
					parent->left = successor;
				else
					parent->right = successor;
				successor->left = work->left;
				successor->right = work->right;
			}
		}
		else
		{
			// some error happends
		}
	}

	return to_delete;
}

bool is_leaf(rbtree_node *head)
{
	if (head->left == nullnode && head->right == nullnode)
	{
		return true;
	}
	return false;
}

bool has_red_child(rbtree_node *head)
{
	return head->left->color == kRed || head->right->color == kRed;
}

rbtree_node *get_sibling(rbtree_node *parent, rbtree_node *child)
{
	if (child == parent->left)
		return parent->right;
	else if (child == parent->right)
		return parent->left;
	else
	{
		std::cout << "parent: " << parent->element << "\n"
				  << "child: " << child->element << "\n";

		assert(false);
		return nullptr;
	}
}

enum DeleteFlag
{
	NORMAL,
	FIND_LEFT_MAX,
	FIND_RIGHT_MIN
};

/*
rbtree_delete(v, T)

T->color = red
GGP = GP = P = X = S = T;

while true

	在循环的一开始，我们应该处于一种可以继续向下搜索的状态

	我们首先应该判断，我们是否找到了我们想要的节点

	if we found, the X is what we want
		deal with it

	if (flag == NORMAL)
		if (X == nullnode)
			return
		else if (X->element == value)
			found in normal mode
	else if (flag == FIND_RIGHT_MIN && X->left == nullnode)
		found right min node
	else if (flag == FIND_LEFT_MAX && X->right == nullnode)
		found left max node

	if we found, then check if replace or delete
		if (is_leaf(X))
			delete it
			return
		else
			if (to_replace == nullptr)
				we should be replaced
			else
				repalce it, and go on

	else if we not found, keep down searching
		update GGP GP P
		down search X
		update S

		adjust X

*/

rbtree_node *rbtree_delete(int value, rbtree_node *T)
{
	// 现在我们仅实现 删除一个叶子节点的情况
	// 只不过这个叶子节点可以是红的，也可以是黑的

	// 首先，我们要将T涂成红色
	// 或者说，我们可以直接跳过这个步骤，因为这个步骤是一定的
	// 只要你有根
	if (T->right == nullnode)
	{
		return nullnode;
	}

	// 我们直接将head涂成红色
	T->color = kRed;
	// GGP GP P X = head
	GGP = GP = P = X = T;

	// 令X = head, 并且令head->color = red
	// X = T->right;

	rbtree_node *sibling = nullptr;

	nullnode->element = value;

	DeleteFlag flag = NORMAL;

	// rbtree_node *to_replace_parent = nullptr;
	rbtree_node *to_replace = nullptr;
	rbtree_node *new_root = nullptr;

	bool is_found = false;

	while (true)
	{
		is_found = false;
		if (flag == NORMAL && X->element == value)
		{
			// we found the value
			is_found = true;
		}
		else if (flag == FIND_RIGHT_MIN && X->left == nullnode)
		{
			// we found the right min
			is_found = true;
		}
		else if (flag == FIND_LEFT_MAX && X->right == nullnode)
		{
			// we found the left max
			is_found = true;
		}

		if (is_found)
		{
			// 目前的replace先使用替换值的方式实现
			// 之后再改成修改指针的模式
			if (to_replace)
			{
				to_replace->element = X->element;
			}
			to_replace = X;

			if (is_leaf(X))
			{
				// 检查是否是红色叶子
				if (X->color == kRed)
				{
					// 直接删除本节点
					// 然后更新父节点

					// 之类需要处理一种特殊的情况
					// 就是X是P的孩子
					// 不对啊，这里好像只有一种情况
					// 就是X是P的孩子
					if (X == P->left)
					{
						P->left = nullnode;
					}
					else if (X == P->right)
					{
						P->right = nullnode;
					}
					else
					{
						assert(false);
						// if (X->element < P->element)
						// 	P->left = nullnode;
						// else
						// 	P->right = nullnode;
					}
				}
				else
				{
					// 如果X不是红色叶子，说明我的代码有问题
					std::cout << "there is errors in my code\n";
				}

				break;
			}
			else
			{
				// 如果不是叶子，我们需要记住这个节点
				// to_replace = X;
				// to_replace_parent = P;
				GGP = GP;
				GP = P;
				P = X;

				if (X->right != nullnode)
				{
					// 在这里就是可以向下的
					flag = FIND_RIGHT_MIN;
					X = X->right;
				}
				else
				{
					flag = FIND_LEFT_MAX;
					X = X->left;
				}

				// 这里没有更新sibling
				sibling = get_sibling(P, X);
			}
		}
		else
		{
			// 如果我们没有找到
			// 根据不同的flag，有不同的指针移动方法
			GGP = GP;
			GP = P;
			P = X;

			if (flag == NORMAL)
			{
				if (value < X->element)
					X = X->left;
				else
					X = X->right;

				if (X == nullnode)
				{
					break;
				}
			}
			else if (flag == FIND_RIGHT_MIN)
			{
				X = X->left;
			}
			else if (flag == FIND_LEFT_MAX)
			{
				X = X->right;
			}

			sibling = get_sibling(P, X);
		}

		// while (X->element != value)
		// {
		// // 当我们想要向下的时候， 我们需要根据一些条件来判断是否可以向下
		// // 1. 如果X有红色儿子
		// // 2. 如果X本身是hongse

		// // 在一般情况下，X在这里是红的，所以P就是红的
		// GGP = GP;
		// GP = P;
		// P = X;

		// // 在一开始的时候，X指向head，
		// // 然后我们让X指向root
		// // 此时一定符合
		// // X，sib是黑，P是红，这是每次迭代的初始状态
		// if (value < X->element)
		// 	X = X->left;
		// else
		// 	X = X->right;
		// sibling = get_sibling(P, X);
		// // 在一般情况下，X和sib一定是黑的，因为之前X是红的
		// // 而新的X和sib是之前的X的孩子
		// // 在这里，X会变成每次迭代的初始状态

		// rbtree_node *new_root = nullptr;

		// 简单情况： X本身就是红的
		if (X->color == kRed)
		{
			// 这种情况直接不需要处理
			continue;
		}

		if (P->color == kBlack)
		{
			// 特殊情况
			// sibling->color is red
			// 特殊情况，这个时候X P是黑的，sib是红的
			// 我们需要在P sib 之间执行一次单旋转
			//    P				P
			// X     sib	sib    X

			if (sibling->element < P->element)
				new_root = left_single_rotate(P);
			else
				new_root = right_single_rotate(P);

			// 调整new_root的父节点指针
			if (new_root->element < GP->element)
				GP->left = new_root;
			else
				GP->right = new_root;

			// 调整节点yanse
			X->color = kBlack; // X本来就是黑的吧
			P->color = kRed;
			sibling->color = kBlack;

			// 然后这会回到最基础情况

			// 我觉得有必要更新指针
			GGP = GP;
			GP = sibling;
			sibling = get_sibling(P, X);
		}

		// 基本情况
		// P is red, X, sib is black

		// case 1: 都没有红儿子
		if (!has_red_child(X) && !has_red_child(sibling))
		{
			// 直接反转颜色
			P->color = kBlack;
			sibling->color = (sibling != nullnode ? kRed : kBlack);
			X->color = (X != nullnode ? kRed : kBlack);
		}
		else if (has_red_child(X))
		{
			// X 有红儿子
			// 不用处理，直接继续向下搜索
			// continue;

			// 只有当我们处理完了X节点，发现我们不是要删除这个节点的时候
			// X 才有可能是黑的
			// 当我们想要删除节点的时候，X一定是红的
		}
		else
		{
			// X 没有红儿子， sib有红儿子
			// 否则，X的兄弟sibling有红孩子，我们需要判断这个红孩子是左红孩子还是右红孩子

			// 这里的旋转有四种情况
			// 左左 右右
			// 左右 右左

			// 我们需要知道sibling是P的左孩子还是右孩子
			// 如果兄弟节点的左右孩子都是红的，任选一种旋转即可
			// 因为结果仍然是正确的
			if (sibling->element < P->element)
			{
				// sibling 左孩子
				if (sibling->left->color == kRed)
				{
					//        P
					//	  sib
					// 	L
					// 如上结构，我们需要做一次左单旋转
					rbtree_node *sibling_left = sibling->left;
					new_root = left_single_rotate(P);
					// 更新new_root的父节点
					if (new_root->element < GP->element)
						GP->left = new_root;
					else
						GP->right = new_root;

					// 调整yanse
					// 调整颜色的时候，需要仔细分析，节点是否可能是nullnode
					X->color = kRed;
					P->color = kBlack;
					sibling->color = kRed;
					sibling_left->color = kBlack;
				}
				else
				{
					//		P
					//	sib
					//		R
					// 如上结构，我么需要做一次左双旋转
					rbtree_node *sibling_right = sibling->right;
					new_root = left_double_rotate(P);
					// 修改new_root父节点
					if (new_root->element < GP->element)
						GP->left = new_root;
					else
						GP->right = new_root;
					// 修正yanse
					// 仍然是X P sib sib_child
					X->color = kRed;
					P->color = kBlack;
					sibling->color = kBlack;
					sibling_right->color = kRed;
				}
			}
			else
			{
				// sibling是P的右孩子

				if (sibling->right->color == kRed)
				{
					// P
					//   sib
					// 		R
					rbtree_node *sibling_right = sibling->right;
					new_root = right_single_rotate(P);
					// 更新new_root的父节点
					if (new_root->element < GP->element)
						GP->left = new_root;
					else
						GP->right = new_root;

					// 更新yanse
					X->color = kRed;
					P->color = kBlack;
					sibling->color = kRed;
					sibling_right->color = kBlack;
				}
				else
				{
					// 	P
					//	  sib
					//	L
					rbtree_node *sibling_left = sibling->left;
					new_root = right_double_rotate(P);
					// 更新new_root的父节点
					if (new_root->element < GP->element)
						GP->left = new_root;
					else
						GP->right = new_root;
					// 更新颜色
					X->color = kRed;
					P->color = kBlack;
					sibling->color = kBlack;
					sibling_left->color = kRed;
				}
			}
		}

		// 在基本情况下，X在这里是红的
		// 如果是特殊情况，比如X有红儿子，那么在这里X就是黑的

		// 在我们现在假设下，仅删除叶子
		// 现在的逻辑应该就是正确的
		// 但是如果我们是删除中间节点，我们就需要在外面处理X是黑色的情况
		// }
	}

	// if (X != nullnode)
	// {
	// 	// 找到了相应的节点, 如果X仍然是黑色，我们还是需要进行一个颜色的转换
	// 	// 然后删掉X

	// 	// 检查是否是红色叶子
	// 	if (X->color == kRed && is_leaf(X))
	// 	{
	// 		// 直接删除本节点
	// 		// 然后更新父节点
	// 		if (X->element < P->element)
	// 			P->left = nullnode;
	// 		else
	// 			P->right = nullnode;
	// 	}
	// 	else
	// 	{
	// 		// 如果X不是红色叶子，说明我的代码有问题
	// 		std::cout << "there is errors in my code\n";
	// 	}
	// }
	// else
	// {
	// 	// 没找到节点，不用删除
	// }

	// 删除完成之后，恢复树跟的颜色
	T->color = kBlack;
	T->right->color = kBlack;

	return X;
}

void do_print(rbtree root)
{
	if (root != nullnode)
	{
		do_print(root->left);
		// std::cout << "{ " << root->element << ", " << root->color << "}, ";
		std::cout << root->element << ", ";
		do_print(root->right);
	}
}

// 先序遍历打印这颗树
void print(rbtree head)
{
	do_print(head->right);
	std::cout << "\n";
}

void do_gen_vec(rbtree root, std::vector<int> &vi)
{
	if (root != nullnode)
	{
		do_gen_vec(root->left, vi);
		vi.push_back(root->element);
		do_gen_vec(root->right, vi);
	}
}

std::vector<int> gen_vec(rbtree head)
{
	std::vector<int> vi;
	do_gen_vec(head->right, vi);
	return vi;
}

void do_gen_dot(rbtree root, std::string &dot, bool is_color)
{
	if (root != nullnode)
	{
		if (is_color)
		{
			if (root->color == kRed)
			{
				dot.append("  " + std::to_string(root->element) + " [fontcolor = red]\n");
			}
		}
		if (root->left != nullnode)
		{
			dot.append("  " + std::to_string(root->element) + " -> " + std::to_string(root->left->element) + "\n");
		}
		if (root->right != nullnode)
		{
			dot.append("  " + std::to_string(root->element) + " -> " + std::to_string(root->right->element) + "\n");
		}
		do_gen_dot(root->left, dot, is_color);
		do_gen_dot(root->right, dot, is_color);
	}
}

std::string gen_dot(rbtree head, bool is_color = true)
{
	std::string dot;
	dot = "digraph {\n";
	do_gen_dot(head->right, dot, is_color);
	dot.append("}\n");
	return dot;
}

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

bool is_rbtree_impl(int &last_black_height, int current_black_height, rbtree_node *root)
{
	if (root == nullnode)
		return true;

	// 不能有两个连续的黑色节点
	if (root->color == kRed)
	{
		if (root->left->color == kRed || root->right->color == kRed)
		{
			std::cout << "have two continuous red node\n";
			return false;
		}
	}

	// 如果本节点是黑色节点，黑高++
	if (root->color == kBlack)
		current_black_height++;

	// 检测黑高

	// 更新current_black_height

	// 检查本节点是否是叶子节点
	// 如果是叶子节点，需要计算新的黑高，并与之前的黑高做一个比较
	if (is_leaf(root))
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
				std::cout << "different black height at " << root->element << "\n";
				std::cout << "current black height: " << current_black_height << std::endl;
				std::cout << "last black height: " << last_black_height << std::endl;
				return false;
			}
		}
	}
	else
	{
		// 如果不是叶子，就继续往下调用本函数就可以了
		if (!is_rbtree_impl(last_black_height, current_black_height, root->left))
			return false;
		if (!is_rbtree_impl(last_black_height, current_black_height, root->right))
			return false;
	}

	return true;
}

bool is_rbtree(rbtree_node *T, int &black_height)
{
	// 1. root is black
	if (T->right->color != kBlack)
	{
		return false;
	}

	// 2. 颜色要么是黑 要么是红

	// 3. 不能有两个连续的红色节点
	// 4. 任意一个节点的左右黑高是一样的
	// 这两个检测可以在递归的过程中实现

	black_height = 0;
	return is_rbtree_impl(black_height, 0, T->right);
}

// 实现一个简单命令行
// 支持插入和删除
// insert value
// delete value
void tree_cmd(rbtree_node *head)
{
	std::string op;
	int value = 0;
	int black_height = 0;
	while (true)
	{
		std::cout << "\n>>";
		std::cin >> op >> value;

		if (op == "insert")
		{
			// binary_insert(value, head);
			insert(value, head);
		}
		else if (op == "delete")
		{
			// binary_delete(value, head);
			rbtree_delete(value, head);
		}
		else if (op == "draw")
		{
			std::ofstream fout("tree.dot", std::ios::trunc);
			std::string dot = gen_dot(head);
			fout << dot << std::endl;
			fout.close();
		}
		else
		{
			std::cout << "invlaid op\n";
		}

		// 每次修改结构之后都检查是否有序
		std::vector<int> vi = gen_vec(head);
		assert(check_vec_order(vi));
		// assert(is_rbtree(head, black_height));

		if (!is_rbtree(head, black_height))
		{
			std::cout << "do not be a rbtree, print its dot\n";
			std::string dot = gen_dot(head);
			assert(false);
		}

		// 我还需要一个函数用来检查目前是否还是一颗红黑树
		// 那么我们根据递归的定义，红黑树上任意一个节点的左右黑高是一样的
		// 最好还是先实现一个红黑树的检测函数

		// 重新生成dot文件
		// 然后用户就可以重新绘制查看了

		std::ofstream fout("tree.dot", std::ios::trunc);
		std::string dot = gen_dot(head);
		fout << dot << std::endl;
		fout.close();

		std::cout << "black height: " << black_height << std::endl;
	}
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
	rbtree head = init_rbtree();
	int number = 0;

	// 2. insert
	std::cout << "verifying insert...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.insert(number);
		insert(number, head);

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
	std::cout << "verifying repeated insert...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.insert(number);
		insert(number, head);

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
		rbtree_delete(number, head);

		// check is rbtree
		int black_height = 0;
		// assert(is_rbtree(head, black_height));
		if (!is_rbtree(head, black_height))
		{
			// 还原现场，方便调试
			print_error_env(unique_vec, i);
			assert(is_rbtree(head, black_height));
		}

		// check order
		std::vector<int> set_vec = to_vec(iset);
		assert(check_vec_order(set_vec));
		std::vector<int> rbtree_vec = gen_vec(head);
		//
		if (!check_vec_order(rbtree_vec))
		{
			print_error_env(unique_vec, i);
			assert(check_vec_order(rbtree_vec));
		}

		// check the two vec is same

		if (!is_vec_same(set_vec, rbtree_vec))
		{
			print_error_env(unique_vec, i);
			assert(is_vec_same(set_vec, rbtree_vec));
		}

		std::cout << "pass: " << i + 1 << "/" << unique_vec.size() << "\n";
	}

	// 3. delete
	std::cout << "verifying delete...\n";
	for (size_t i = 0; i < random_vec.size(); ++i)
	{
		number = random_vec[i];
		iset.erase(number);
		rbtree_delete(number, head);

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
	rbtree head = init_rbtree();
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
		insert(i, head);
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
		rbtree_delete(i, head);
	}
	rbtree_time = std::chrono::high_resolution_clock::now().time_since_epoch().count() - rbtree_time;
	std::cout << "rbtree delete: " << rbtree_time << "ns" << std::endl;
	std::cout << "rbtree is faster than set by " << static_cast<double>(set_time) / static_cast<double>(rbtree_time) << "\n";
}

int old_main(int argc, char *argv[])
{
	rbtree head = init_rbtree();
	// before insert anything
	// std::cout << "before insert\n";
	// print(head);

	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> ui(0, 100);
	// std::cout << "insert something\n";
	for (int i = 0; i < 100; ++i)
	{
		// binary_insert(ui(e), head);
		insert(ui(e), head);
	}
	// print(head);
	// 我们需要检查这个东西是不是有序的
	std::vector<int> vi = gen_vec(head);
	// for (const auto i : vi)
	// {
	// 	std::cout << i << "\n";
	// }
	// 卧槽 完全正确
	//std::cout << std::endl;
	assert(check_vec_order(vi));
	int black_height = 0;
	assert(is_rbtree(head, black_height));

	// 好的，现在最起码最基本的插入是正确的
	// 我们还需要做的一件事情，就是在插入的时候反转颜色
	// flip color
	// 我们去看看这个函数的实现

	// std::string dot = gen_dot(head);
	std::string dot = gen_dot(head);
	std::cout << dot << std::endl;

	tree_cmd(head);

	return 0;
}

void restore_from_file(const std::string &vec_file)
{
	std::vector<int> random_vec;
	std::ifstream fin(vec_file);
	int number = 0;
	while (fin)
	{
		fin >> number;
		random_vec.push_back(number);
	}

	rbtree head = init_rbtree();
	for (const auto number : random_vec)
	{
		insert(number, head);
	}

	tree_cmd(head);
}

int main(int argc, char *argv[])
{
	// if (argc > 1)
	// {
	// 	restore_from_file(argv[1]);
	// }
	// else
	// {
	// 	verify_rbtree(10000);
	// }

	if (argc > 1)
	{
		performance(std::atoi(argv[1]));
	}

	return 0;
}
