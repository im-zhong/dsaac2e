// 2021/8/31
// zhangzhong
// container_of

#include <stdio.h>

struct st
{
    int a;
    int b;
};

// 获得结构体类型type中member成员的偏移量offset
#define offset_of(type, member) ((size_t) &((type *)0)->member)

// 用结构体成员指针 减去 结构体成员的偏移量 就是整个结构体开始的位置
// 也就是结构体对象的指针
#define container_of(ptr, type, member) \
    (type *)((void *)ptr - offset_of(type, member))

int main(int argc, char* argv[])
{
    // 这个只是把字面值0转换成了struct st*指针而已
    // 这样写是可以的，只不过你试图通过pst访问成员时会发生错误
    struct st* pst = (struct st*)0;
    // 0x4
    printf("%p\n", &(pst->b));

    struct st* apst = (struct st*)4;
    // 0x8 = 4 + 4
    printf("%p\n", &(apst->b));

    struct st nst;
    // 0x7fff6c877d14
    printf("%p\n", &(nst.b));

    size_t off = offset_of(struct st, b);
    printf("%ld\n", off);

    struct st* cpst = container_of(&nst.b, struct st, b);
    printf("%p\n", cpst);
    printf("%p\n", &nst);
}
