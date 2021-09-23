// 2021/9/23
// zhangzhong
// spares graph, 稀疏图，边的条数|E|远远小于|V|^2
// 使用邻接链表
// G = (V, E)
//
//   1 2 3 4 5
// 1   1     1
// 2 1   1 1 1
// 3   1   1
// 4   1 1   1
// 5 1 1   1
// 
// 1 -> 2 -> 5
// 2 -> 1 -> 5 -> 3 -> 4
// 3 -> 2 -> 4
// 4 -> 2 -> 5 -> 3
// 5 -> 4 -> 1 -> 2
//
// 上面第一个是邻接矩阵，12345表示的是某一个节点
// 矩阵的值1表示有一条边存在。
// 第二个表示是邻接链表，每个节点都有一条链表，
// 这个链表含有的值就是与1邻接的所有节点
// 这两种方式都可以表示无向图和有向图
//
// 因为图是一个非常抽象的东西，所以我们可以直接对图的节点进行编号
// 就像在disjoint_set里面那样，直接用long表示节点即可
// 因为这些数据结构往往不作为容器来使用，所以不需要考虑存放各种类型的数据
// 它只是描述一个问题，抽象！
//

// 权重图，weight
// 每条边带有一个相关的权重 weight
// E(u,v)的weight可以保存在u->v的链表节点上

// 邻接链表的存储空间需求为O(E + V)
// 邻接矩阵的存储空间需求为O(E^2)

// 那么在这个文件里面，我将实现一个邻接矩阵表示的稀疏图

#include "list.h"
#include "queue.h"
#include <stddef.h>
#include <stdlib.h>

// 但是，我们的entry是什么呢？？

typedef struct spares_graph
{
    // 稀疏图实际上就是一个链表数组

    // 每一个节点应该有一个独一无二的标识，
    // 这个标识实际上也应该由应用程序来决定，而不是库来决定
    // 但是这里使用序号代替 也就是long或者int
    // 这个可以使用一个typedef来定义, 就叫...叫啥好
    // 因为这样可以直接根数组下标进行一个交互

    // 图的所有节点
    list_node_t *vertex_array;
    size_t size;
    size_t capacity;

} spares_graph;
// TODO
// 或许可以利用realloc函数来实现一个可变长数组？？？
// 这种数组仅为了方便的实现多种数据结构??
typedef long vertex_t;
// edge
typedef double weight_t;

// 广度优先搜索
// G = (V, E) 和一个源点 s
// 按照广度进行搜索，也就是说，搜索完所有与s直接邻接的节点之后，才会搜索其他节点
//

typedef struct
{
    vertex_t vertex;
    weight_t weight;

    // 如果这里不需要写这个 继承队列 就好了
    // 就更加灵活
    // 看来还是没有必要多此一举啊，越灵活越好
    INHERIT_QUEUE;
} vertex_entry;

static inline vertex_entry *make_vertex_entry(vertex_t vertex, weight_t weight)
{
    vertex_entry *entry = malloc(sizeof(vertex_entry));
    entry->vertex = vertex;
    entry->weight = weight;
    return entry;
}

enum graph_color
{
    white,
    black,
    gray
};

typedef struct
{
    // 这里的color可以变成一个bool，其实就是有没有访问的区别
    int color;
    vertex_t prev;
    size_t distance;
} graph_bfs;

// breadth-first search
// bfs
void bfs(spares_graph *graph, vertex_t vertex)
{


    // 在遍历的过程中，我还需要一个数组来记录每个节点的各种属性
    // 颜色，树的指针等等

    // 在实际的环境中，我们需要根据vertex找到对应的节点
    // 这里假设我们可以直接使用下标，如果有特殊的需要
    // 直接替换这里即可
    // graph->vertex_array[vertex];
    // 我们可以像上面这样直接访问一个节点


    // 创建一个辅助实现bfs的数组，并初始化一下
    graph_bfs *vertex_property = malloc(graph->size * sizeof(graph_bfs));
    for (size_t i = 0; i < graph->size; ++i)
    {
        vertex_property[i].color = white;
        vertex_property[i].prev = -1;
        vertex_property[i].distance = -1;
    }
    vertex_property[vertex].color = gray;
    vertex_property[vertex].prev = -1;
    vertex_property[vertex].distance = 0;

    // 定义一个队列
    // 将源点放到队列中
    vertex_entry *entry = NULL;
    // 我需要定义一个简单的函数来定义一个entry
    entry = malloc(sizeof(vertex_entry));
    entry->vertex = vertex;

    queue_init(queue);
    queue_push(&queue, entry);


    // 这个队列的内存管理需要我自己来动手啊,,,烦内
    while (!queue_is_empty(&queue))
    {
        entry = queue_top(&queue, vertex_entry);
        queue_pop(&queue);
        // 我需要遍历entry->vertex对应的链表
        // 其实遍历的就是所有的邻接节点
        vertex_entry *adjacency_entry = NULL;
        list_for_each_entry(adjacency_entry, &(graph->vertex_array[vertex]), vertex_entry, queue_)
        {
            // 判断，当前节点是否被遍历过了
            if (vertex_property[adjacency_entry->vertex].color == white)
            {
                // 白色就表示没有被遍历过
                // 让他的颜色变成gray
                // 他的距离是当前entry.distance + 1
                vertex_property[adjacency_entry->vertex].color = gray;
                vertex_property[adjacency_entry->vertex].distance = vertex_property[entry->vertex].distance + 1;
                vertex_property[adjacency_entry->vertex].prev = entry->vertex;

                // 我们需要将这个节点入队，这样就可以继续遍历下去
                queue_push(&queue, make_vertex_entry(adjacency_entry->vertex, adjacency_entry->weight));
            }
        }

        // 遍历完entry的所有邻接节点之后，将entry的颜色置位黑色
        vertex_property[entry->vertex].color = black;
        free(entry);
    }

    // 在理解了算法导论 图22-3 之后，你就会发现，颜色是不需要的
    // 其实gray和black都可以看作一个状态，就是我们访问了这个节点
    // gray的意思是，我们将来要访问这个节点，他已经在队列里面了，但是我们还没有实际去访问
    // 而黑色就是，我们实际上访问了这个节点
    // 因此这个颜色可以使用一个bool来代替
}

// 广度优先搜索，可以给出从源节点s到所有可以到达的节点之间的距离
// 这个结果会记录在一个数组里面

// 广度优先树
// 如果生成了广度优先数，我们可以给出从s到v的最短路径
// 这非常简单，从v开始不断向前查找前驱即可，直到找到s
// 这个过程可以借助一个递归简单的完成
// shortest_path

// 我们的这个深度优先搜索，应该在遍历的时候，做一些事情吧
// 其实入队出队的节点顺序就是深度优先顺序，可以有两种方式
// 用户可以提供一个函数，在深度优先搜索的时候干
// 我们会针对每一个entry调用该函数
// 然后，我们可以返回一个广度优先树，其实就是那个vertex_property数组
// 不如就把他的名字改成 广度优先树 bftree， 就酱~~~


// -------------------------------------------------------

// 深度优先搜索
// 只要可能，就在图中尽量深入
// 递归，很容易实现，特别类似二叉树的根序遍历

// ?? 深度优先搜索有多个源节点？？？ how to
// 深度优先同样可以记录前驱节点
// 不过，广度优先是一颗 广度优先树
// 而，深度优先前驱子图，是一颗由多颗深度优先树构成的深度优先森林
// 可以使用颜色来防止重复访问
// 所有的深度优先树是不相交的

// 时间戳？？

typedef struct
{
    // 不对啊，我为什么要在这里面记录vertex
    // 数组的下标就是vertex啊
    // vertex_t vertex;
    vertex_t prev;
    // 这里是两个时间戳,一个是入队的时间戳
    // 我傻逼了，dfs哪有队列。。。
    // 第一个时间就是visit时间，也就是第一次访问到这个节点的时间
    size_t visit_time;
    // 因为是递归实现的，所以他需要回溯，第二个时间就会回溯回来的时间
    // 每次访问一个新的节点都会使得time++
    size_t back_time;


    int color;

} graph_dfs;


void dfs_visit(spares_graph *graph,  graph_dfs *df_forest, vertex_t vertex, size_t time)
{
    ++time;
    df_forest[vertex].visit_time = time;
    df_forest[vertex].color = gray;

    // 访问所有的邻接节点
    vertex_entry *adjacency_entry = NULL;
    list_for_each_entry(adjacency_entry, &graph->vertex_array[vertex], vertex_entry, queue_)
    {
        if (df_forest[adjacency_entry->vertex].color == white)
        {
            df_forest[adjacency_entry->vertex].prev = vertex;
            dfs_visit(graph, df_forest, adjacency_entry->vertex, time);
        }
    }

    // 所有的链接节点都访问完成之后，递归回溯到此处
    df_forest[vertex].color = black;
    ++time;
    df_forest[vertex].back_time = time;
}

// depth-first search
// O(E + V)
void dfs(spares_graph *graph)
{
    // 还是准备一个数组，类似广度优先那样就可以了
    graph_dfs *df_forest = malloc(graph->size * sizeof(graph_dfs));
    for (size_t vertex = 0; vertex < graph->size; ++vertex)
    {
        df_forest[vertex].color = white;
        df_forest[vertex].prev = -1;
        df_forest[vertex].visit_time = -1;
        df_forest[vertex].back_time = -1;
    }

    // 初始化时间戳
    size_t time = 0;

    // 遍历所有的节点
    for (size_t vertex = 0; vertex < graph->size; ++vertex)
    {
        if (df_forest[vertex].color == white)
            dfs_visit(graph, df_forest, vertex, time);
    }
}

// 这个深度优先实现起来要简单的多
// 因为不涉及队列，仅仅是一个简单的递归

// 深度优先搜索提供了树的结构价值很高的信息
// 算法导论 图22-4, 一个典型的深度优先搜索过程, 注意时间戳
// 每个节点的发现时间与完成时间




// -------------------------------------------
// 这些实现应该放在一个单独的.c文件里面
// 最小生成树 mst
// minimal spanning tree
// 如果采取kruskal实现的话，我可以使用disjoint_set
// 而且最终的最小生成树应该就是一个disjoint_set
//
#include "disjoint_set.c"

typedef struct 
{
    vertex_t v1;
    vertex_t v2;
    weight_t weight;
} edge_t;

void mst_kruskal(spares_graph *graph)
{
    // 声明一个空的MST集合
    // 这个东西应该是一个edge的集合

    // 首先制造一个disjoint_set, 
    // 此时，每个节点都是独立的
    disjoint_set djs;
    disjoint_set_init(&djs, graph->size);

    // sort the edges of G.E by weight in decreasing order
    // 我要按照weight进行排序
    // 然后根据排序结果, 按照顺序遍历
    // 这要怎么实现呢？？
    // 我要维护一个边的数组？？
    // 这一部分实现起来应该也是比较简单, 准备一个数组，
    // 存储所有的边，然后进行一个快速排序，即可
    // 假设我这里已经获得了一个这样的数组
    edge_t *edges = NULL;
    size_t edge_size = 0;
    for (size_t i = 0; i < edge_size; ++i)
    {
        if (disjoint_set_find(&djs, edges[i].v1) != disjoint_set_find(&djs, edges[i].v2))
        {
            // 最小生成树是边的集合
            // MST = MST union {edge(v1, v2)}
            disjoint_set_union(&djs, edges[i].v1, edges[i].v2);
        }
    }

    // 最终可以返回MST
}

// 这个实现是真的简单，就是感觉不太适合邻接链表



// prim算法依赖一个最小堆
#include "heap.c"

void mst_prim(spares_graph *graph)
{
    bheap heap;
    // heap_init

    // 算法的一开始，任选一个节点，我们选择0
    vertex_t vertex = 0;

    vertex_t minimal_vertex = 0;
    // Q = G.V
    // Q是全体顶点形成的一个最小堆
    // 这个堆是怎么构建的？？
    // 每个节点有一个key属性，一开始vertex是0，其余是-1
    // 最小堆根据这个key进行构建

    while (!bheap_is_empty(&heap))
    {
        minimal_vertex = bheap_top(&bheap);
        // 遍历minimal_vertex的所有邻接节点

    }

}

// 这个算法理解起来就比kruskal要稍微复杂一些
// 然后也需要很多额外的数组来保存一些中间信息
// 还是算了，不实现了
// 只要领会了思想就可以了

// 还有一个问题，就是验证是否是最小生成树的问题
// 在一篇论文中有所描述


// ---------------------------------------
// 单源最短路径 Single-Source Shortest Paths
// dijkstra
// 带权重的有向图
// 路径 path = v0, v1, v2, ... , vn
// w(p) = w(v0, v1) + w(v1, v2) + ...
// 最短路径的子路径也是最短路径
// 我在完成所有基础算法之后，整理的时候
// 可以把这些重要的结论也写在头文件里面，最好可以达到
// 我忘了，我看一眼，我又想起来了。。这种程度
//

// 负数权重的边
// 如果图中存在一个环路，这个环路中存在一条负数权重的边
// 同时，从源点s可以走到这个环路
// 则此图不存在从s开始的单源最短路径
// 最短路径不能包含权重为负值的环路

// 环路
// 一条最短路径不可以包含环路，如果包含环路，去掉这个环路，会更短
// 所以可以合理的假设，所有的最短路径都是简单路径

// 所以可以实现一个斐波那契堆，因为很明显这两个算法都用到了这个结构
// 感觉挺值的
// 不过目前还是先使用二叉堆进行实现


typedef struct
{
    weight_t weight;
    vertex_t prev;
} sptree_t;


weight_t spares_graph_weight(spares_graph *graph, vertex_t lhs, vertex_t rhs)
{
    // 我们需要遍历lhs对应的链表，并得到其weight
    // 这个操作并不是O(1)
    // 这样好像不太好
    return 0.0;
}

// 1. 松弛 relaxation
// v.d: 最短路径权重 between s -> v
// 输入一条边<u, v>, 测试看看是否可以对从 s -> v的最短路径进行改善
// 通俗来讲，之前存在一条路径 s -> v
// 然后我们看看加入一个新的节点 u, 使得路径变成 s -> u -> v
// 看看新的路径是否权重更小，如果小的话，就更新相关的属性
static inline void relax(spares_graph *graph, sptree_t *sptree, vertex_t u, vertex_t v, weight_t weight)
{
    // 我们需要获取某条边的权重，这在稀疏图中并不是一个O(1)的操作
    // 我们需要遍历邻接链表
    // v.d <= u.d + w(u, v)
    if (sptree[v].weight > sptree[u].weight + weight)
    {
        // s -> v       : v.weight
        // s -> u -> v  : u.weight + weight(u, v)
        // 我们更新v的权重，同时v的前驱也变成了u
        sptree[v].weight = sptree[u].weight + weight;
        sptree[v].prev = u;
    }
}


// 24.2 在有向无环图中DAG中首先进行一次拓扑排序，然后遍历执行relax操作也可以得到sp

#include <limits.h>
// w(u, v) >= 0
void dijkstra(spares_graph *graph, vertex_t vertex)
{
    // 我感觉很多算法都会用到的一个结构就是 生成树
    // spanning tree : stree
    // 不过不同的算法要求的结构不太一样，还是每个都重新定义一个结构好了


    // 初始化所有节点
    sptree_t *sptree = malloc(graph->size * sizeof(sptree_t));
    for (size_t i = 0; i < graph->size; ++i)
    {
        // 我们让这个权重为赋值表示正无穷
        // 或者我们可以用MAX来表示正无穷
        // 因为待会堆的构建需要历来这个weight
        // 所以这个weight在实现上还真就必须是MAX
        sptree[i].weight = 0.0; // double_max
        // 在节点的表示上，我们也可以使用MAX来表示
        sptree[i].prev = -1;
    }
    sptree[vertex].weight = 0.0;

    // S. 从源节点s到该集合S中的每个节点之间的最短路径已经被找到
    // 从V - S中选择一个新的节点u，将u加入集合S，对所有从u发出的边进行松弛

    // 维护一个优先队列Q = G.V, 排序的依据是每个节点的key，也就是在这里的sptree[i].weight
    bheap heap;
    
    // Q = V - S
    while (!bheap_is_empty(&heap))
    {
        // top
        vertex_t u;

        // S = S U u
        // 这个可以使用一个数组来实现，其中每个元素都有一个标志位

        // for v in adj[u]
        //      relax(u, v, w(u, v))
    }
}

// 算法本身非常的简单，我要分析一下为什么可以找到最短路径
// 算法的实现非常简单，但是说实话，我并没有完全理解它
// 只能很模糊去的认同它


// 所有节点对之间的单元最短路径
// 网络最大流问题 流问题
// 这两个算法都好复杂，而且不知道有什么用，就是是做题，感觉也没啥用啊
// 就先不弄了
