// 2021/9/2
// zhangzhong

// 如何在结构体中包含函数指针


static inline void fun_()
{

}

struct student_t {
    char *name;
    int age;
};

// 提供两个相关的函数
// 比如一个是modify_name
// 增加年龄

static inline void student_modify_name_(student_t* student, char* name)
{
    student->name = name;
}

static inline void student_increment_age_(student_t* student)
{
    ++student->age;
}


// 怎么保证这个东西整个程序只有一份呢？？
struct student_function {
    void(*modify_name)(student_t*, char*);
    void(*increment_age)(student_t*);
} student_fp = {
    &student_modify_name_,
    &student_increment_age_
};

// 可不可以提供一个宏？
// 比如我这个结构的初始化宏
// 然后在里面extern 我的这个函数变量
// 然后提供一种正确的合适的语法隐藏这些细节

// 然后呢，我要如何调用它？？

// student_fp.modify_name()

// 这tm和直接 student_modify_name 有个几把区别？？？
// 不行

// 你想要达到类似OO的语法
// 只能在结构体中添加一个指向这些函数指针的东西，最少也得加上一个类似 虚函数表指针 vptr 那样的东西
// 只想一块函数表结构体
// 这样做的问题就是你每复制一个结构体，都需要复制一个指针
// 得不偿失

// 放弃这种想法
// 还是把所有与某个struct相关的方法都以这个结构体的名字开头
// 这应该就是比较好的方法了








