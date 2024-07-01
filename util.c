#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

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

extern bool IsDigitOrMinusSign(char test)
{
    if (isdigit(test) || test == DASH_MINUS_CHAR)
    {
        return true;
    }
    return false;
}

extern int32_t NumberOfDigitsInInt64(int64_t num)
{
    int32_t r = 1;
    if (num < 0)
        num = (num == LLONG_MIN) ? LLONG_MAX : -num;
    while (num > 9)
    {
        num /= 10;
        r++;
    }
    return r;
}

extern char *Int64ToString(int64_t num)
{
    int32_t num_digits = NumberOfDigitsInInt64(num);
    if (num < 0)
    {
        num_digits++;
    }
    char *int64_as_string = malloc(sizeof(char) * (num_digits + 1));

    char *ptr = int64_as_string;
    char *ptr1 = int64_as_string;
    char tmp_char = NULL_CHAR;

    int64_t tmp_value = 0;
    do
    {
        tmp_value = num;
        num /= 10;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - num * 10)];
    } while (num);

    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';

    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return int64_as_string;
}

#define FLOAT_CHAR_MAX 10

extern char *DoubleToString(double num)
{
    // double integral;
    // double fractional = modf(num, &integral);
    // int64_t integral_as_int64 = (int64_t)integral;
    // printf("%lld %lf\n", integral_as_int64, fractional);
    char *double_as_string = malloc(sizeof(char) * FLOAT_CHAR_MAX);
    (void)gcvt(num, FLOAT_CHAR_MAX, double_as_string);

    return double_as_string;
}

extern char *PutQuotesAroundString(char *input_str, bool free_input)
{
    if (input_str == NULL)
    {
        return NULL;
    }
    size_t input_str_real_len = strlen(input_str) + 1;
    size_t new_str_len = input_str_real_len + 2;
    char *new_string = malloc(sizeof(char) * new_str_len);
    new_string[0] = DOUBLE_QUOTES_CHAR;

    for (size_t i = 0; i < input_str_real_len - 1; i++)
    {
        new_string[i + 1] = input_str[i];
    }
    new_string[new_str_len - 2] = DOUBLE_QUOTES_CHAR;
    new_string[new_str_len - 1] = NULL_CHAR;
    if (free_input)
    {
        free(input_str);
    }

    return new_string;
}
