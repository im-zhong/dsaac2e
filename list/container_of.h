// 2021/9/3
// zhangzhong
// container_of.h

#ifndef CONTAINER_OF_H
#define CONTAINER_OF_H

#include <stddef.h>

/**
 * @brief   get struct ptr from its member ptr
 * @ptr:    struct member's address
 * @type:   struct type
 * @member: struct member name
 */
#define container_of(ptr, type, member) \
    ((void *)ptr - offsetof(type, member))

#endif // CONTAINER_OF_H