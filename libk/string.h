#pragma once

#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);

char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);

int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

unsigned long strtoul(const char* nptr, char** endptr, int base);

bool isalpha(char c);
bool isupper(char c);
bool isdigit(char c);
bool isspace(char c);