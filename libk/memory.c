#include "memory.h"

void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;

    while (n--)
        *p++ = (unsigned char)c;

    return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    while (n--)
        *d++ = *s++;

    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    if (d == s || n == 0)
        return dest;

    if (d > s && d < s + n) {
        d += n;
        s += n;
        while (n--)
            *(--d) = *(--s);
    } else {
        while (n--)
            *d++ = *s++;
    }
    
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;

    while (n--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = s;

    for (size_t i = 0; i < n; i++) {
        if (p[i] == (unsigned char)c)
            return (void*)(p + i);
    }

    return NULL;
}