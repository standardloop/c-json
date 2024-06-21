#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./util.h"

extern void CopyString(char *src, char *des, size_t len, size_t src_offset)
{
    if (src == NULL || des == NULL || len <= 0)
    {
        return;
    }

    char *src_it = src + src_offset;
    size_t size = 0;
    while (size < len)
    {
        *des = *src_it;
        des++;
        src_it++;
        size++;
    }
}

extern bool IsCharInString(const char *input_str, char checker)
{
    while (*input_str != NULL_CHAR)
    {
        if (*input_str == checker)
        {
            return true;
        }
        input_str++;
    }
    return false;
}

extern size_t NumCharInString(const char *input_str, char checker)
{
    size_t count = 0;
    while (*input_str != NULL_CHAR)
    {
        if (*input_str == checker)
        {
            count++;
        }
        input_str++;
    }
    return count;
}

extern char *QuickAllocatedString(char *copy)
{
    size_t true_len = strlen(copy) + 1;
    char *allocated_string = malloc(sizeof(char) * true_len);
    strcpy(allocated_string, copy);
    return allocated_string;
}

extern inline void PrintSpaces(int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf(" ");
    }
}
