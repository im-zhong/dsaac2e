#include <iostream>
#include <chrono>
#include <random>
#include <cassert>
#include <map>
#include <fstream>

// TODO: 完成一个交互式的树结构CLI，支持插入删除等操作，并且需要实时更新绘制出来的图片来展示树的结构

enum Color
{
	kBlack,
	kRed
};

struct rbtree_node
{
	int element;
	rbtree_node* left;
	rbtree_node* right;
	Color color;
};

// rotate
// 就这么简单??
// X P GP GGP
// 旋转发生在X P GP之间,实际上传入的是GP节点
// 同时我们也需要GGP节点,因为旋转完成之后，产生了new_root，需要更新GGP的指针
rbtree_node* left_single_rotate(rbtree_node* parent)
{
	rbtree_node* new_root = parent->left;
	parent->left = new_root->right;
	new_root->right = parent;
	return new_root;
}

rbtree_node* right_single_rotate(rbtree_node* parent)
{
	rbtree_node* new_root = parent->right;
	parent->right = new_root->left;
	new_root->left = parent;
	return new_root;
}

// parent = k3
// 就TM这么简单
rbtree_node* left_double_rotate(rbtree_node* parent)
{
	parent->left = right_single_rotate(parent->left);
	return left_single_rotate(parent);
}

rbtree_node* right_double_rotate(rbtree_node* parent)
{
	parent->right = left_single_rotate(parent->right);
	return right_single_rotate(parent);
}

using rbtree = rbtree_node*;

// head & nullnode
rbtree nullnode = nullptr;

// X 需要插入的节点
// P 需要插入的节点的parent
// GP grand parent
// GGP	grand grand parent
rbtree_node* X, * P, * GP, * GGP;

// 反转颜色
void flip_color(int value, rbtree_node* T)
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
		rbtree_node* new_root = nullptr;
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
rbtree_node* insert(int value, rbtree_node* T)
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
void binary_insert(int value, rbtree_node* T)
{
	// 首先要找到需要插入的地方
	// 这个T一定是一个head
	// 而head->element = INT_MIN, 也就是负无穷
	// 如果你没有一个head来编程的话，一开始是没有parent的，这样就造成了编程上的困难
	rbtree_node* work = T;
	rbtree_node* parent = work;

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
std::pair<rbtree_node*, rbtree_node*> binary_next(rbtree_node* head)
{
	// if (head->right == nullnode)
	// {
	// 	return nullptr;
	// }

	// 如果存在右子树, 那么肯定是这个右子树最小的值
	// 也就是一直往左
	rbtree_node* parent = head;
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
rbtree_node* binary_prev(rbtree_node* head)
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

rbtree_node* binary_delete(int value, rbtree_node* T)
{
	// 在删除之前，我们需要找到这个节点
	// head
	rbtree_node* work = T;
	rbtree_node* parent = work;

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

	rbtree_node* to_delete = nullptr;

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
			rbtree_node* successor_parent = nullptr;
			rbtree_node* successor = nullptr;
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


bool is_leaf(rbtree_node* head)
{
	if (head->left == nullnode && head->right == nullnode)
	{
		return true;
	}
	return false;
}


rbtree_node* delete(int value, rbtree_node* T)
{
	// 首先仅实现 删除红色叶子节点

	// 首先是一个用了很多遍的find例程，我感觉这个可以抽象出来，直接找到

	rbtree_node* parent = T;
	rbtree_node* work = T;
	nullnode->element = value;
	while (work->element != nullnode->element)
	{
		parent = work;
		if (value < work->element)
			work = work->left;
		else
			work = work->right;
	}

	if (work != nullnode)
	{
		// 找到了相应的节点

		// 检查是否是红色叶子
		if (work->color == kRed && is_leaf(work))
		{
			// 直接删除本节点
			// 然后更新父节点
			if (work->element < parent->element)
				parent->left = nullnode;
			else
				parent->right = nullnode;
		}
	}
	else
	{
		// 没找到节点，不用删除
	}
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

void do_gen_vec(rbtree root, std::vector<int>& vi)
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

void do_gen_dot(rbtree root, std::string& dot, bool is_color)
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
bool check_vec_order(const std::vector<int>& vi)
{
	for (size_t i = 1; i < vi.size() ; ++i)
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

// 实现一个简单命令行
// 支持插入和删除
// insert value
// delete value
void tree_cmd(rbtree_node* head)
{
	std::string op;
	int value = 0;
	while (true)
	{
		std::cout << "\n>>";
		std::cin >> op >> value;
		
		if (op == "insert")
		{
			binary_insert(value, head);
		}
		else if (op == "delete")
		{
			binary_delete(value, head);
		}
		else
		{
			std::cout << "invlaid op\n";
		}

		// 每次修改结构之后都检查是否有序
		std::vector<int> vi = gen_vec(head);
		assert(check_vec_order(vi));

		// 重新生成dot文件
		// 然后用户就可以重新绘制查看了

		std::ofstream fout("tree.dot", std::ios::trunc);
		std::string dot = gen_dot(head, false);
		fout << dot << std::endl;
		fout.close();
	}
}

int main(int argc, char* argv[])
{
	int i = 10;
	rbtree head = init_rbtree();
	// before insert anything
	// std::cout << "before insert\n";
	// print(head);

	std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> ui(0, 100);
	// std::cout << "insert something\n";
	for (int i = 0; i < 20; ++i)
	{
		binary_insert(ui(e), head);
		// insert(ui(e), head);
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


	// 好的，现在最起码最基本的插入是正确的
	// 我们还需要做的一件事情，就是在插入的时候反转颜色
	// flip color
	// 我们去看看这个函数的实现

	// std::string dot = gen_dot(head);
	std::string dot = gen_dot(head, false);
	std::cout << dot << std::endl;


	tree_cmd(head);


	return 0;
}