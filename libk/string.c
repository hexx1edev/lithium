#include "string.h"
#include <math.h>
#include <memory.h>

#define	ULONG_MAX	((unsigned long)(~0L))

size_t strlen(const char* s) {
    size_t len = 0;

    while (s[len] != '\0')
        len++;

    return len;
}

char* strchr(const char* s, int c) {
    while (*s != (char)c) {
        if (*s == '\0')
            return NULL;
        s++;
    }

    return (char*)s;
}

size_t strnlen(const char* s, size_t maxlen) {
    size_t i;

    for (i = 0; i < maxlen && s[i]; i++);

    return i;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;

    while (*s++) {
        if (*s == (char)c)
            last = s;
    }

    return (char*)last;
}

int strcmp(const char* s1, const char* s2) {
    return memcmp(s1, s2, MIN(strlen(s1),strlen(s2))+1);
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while ( n && *s1 && ( *s1 == *s2 ) ) {
        ++s1;
        ++s2;
        --n;
    }

    if (n == 0)
        return 0;
    else
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

unsigned long strtoul(const char* nptr, char** endptr, int base) {
	const char* s;
	unsigned long acc, cutoff;
	int c;
	int neg, any, cutlim;

	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = ULONG_MAX / (unsigned long)base;
	cutlim = ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
			acc = ULONG_MAX;
		} else {
			any = 1;
			acc *= (unsigned long)base;
			acc += c;
		}
	}
	if (neg && any > 0)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char*) (any ? s - 1 : nptr);
	return (acc);
}

bool isalpha(char c) {
    return strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", c) != NULL;
}

bool isdigit(char c) {
    return strchr("0123456789", c) != NULL;
    
    return false;
}

bool isspace(char c) {
    return c == ' ';
}

bool isupper(char c) {
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", c) != NULL;
}