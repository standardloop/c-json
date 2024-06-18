#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./util.h"

extern void CopyString(const char *src, char *des, size_t len, size_t des_offset)
{
    if (src == NULL || des == NULL || len <= 0)
    {
        return;
    }

    char *dest_it = des + des_offset;
    size_t size = 0;
    while (size < len)
    {
        *dest_it = *src;
        dest_it++;
        src++;
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
