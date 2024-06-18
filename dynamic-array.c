#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./json.h"
#include "./util.h"

static inline bool isDynamicArrayFull(DynamicArray *);
static inline bool isDynamicArrayEmpty(DynamicArray *);
static void freeDynamicArrayList(DynamicArrayElement **, u_int32_t, bool);
static void freeDynamicArrayListElement(DynamicArrayElement *element);
static void dynamicArrayResize(DynamicArray *);

static DynamicArrayElement *dynamicArrayElementReplicate(DynamicArrayElement *);

extern DynamicArray *DefaultDynamicArrayInit(void)
{
    return DynamicArrayInit(DEFAULT_DYN_ARR_SIZE);
}

extern DynamicArray *DynamicArrayInit(u_int32_t initial_capacity)
{
    DynamicArray *dynamic_array = malloc(sizeof(DynamicArray));
    if (dynamic_array == NULL)
    {
        printf("[ERROR]: could not allocate memory for DynamicArray\n");
        return NULL;
    }
    dynamic_array->size = 0;
    dynamic_array->capacity = initial_capacity;
    dynamic_array->list = malloc(sizeof(DynamicArrayElement *) * dynamic_array->capacity);
    return dynamic_array;
}

extern void DynamicArrayAddFirst(DynamicArray *dynamic_array, DynamicArrayElement *element)
{
    DynamicArrayAdd(dynamic_array, element, 0);
}

extern void DynamicArrayAddLast(DynamicArray *dynamic_array, DynamicArrayElement *element)
{
    if (dynamic_array == NULL || element == NULL)
    {
        return;
    }
    DynamicArrayAdd(dynamic_array, element, dynamic_array->size);
}

extern void DynamicArrayAdd(DynamicArray *dynamic_array, DynamicArrayElement *element, u_int32_t index)
{
    if (dynamic_array == NULL || element == NULL)
    {
        return;
    }
    if (isDynamicArrayFull(dynamic_array))
    {
        dynamicArrayResize(dynamic_array);
    }
    for (u_int32_t i = dynamic_array->size; i > index; i--)
    {
        dynamic_array->list[i] = dynamic_array->list[i - 1];
    }

    dynamic_array->list[index] = element;
    dynamic_array->size++;
}

static void dynamicArrayResize(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }

    DynamicArrayElement **newList = malloc(sizeof(DynamicArrayElement *) * dynamic_array->capacity * DEFAULT_DYN_ARR_RESIZE_MULTIPLE);
    if (newList == NULL)
    {
        printf("not enough memory for newList\n");
        return; // FIXME
    }

    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        newList[i] = dynamic_array->list[i];
    }
    freeDynamicArrayList(dynamic_array->list, dynamic_array->size, false);
    dynamic_array->list = newList;
    dynamic_array->capacity *= DEFAULT_DYN_ARR_RESIZE_MULTIPLE;
}

static inline bool isDynamicArrayFull(DynamicArray *dynamic_array)
{
    return dynamic_array->capacity == dynamic_array->size;
}

static inline bool isDynamicArrayEmpty(DynamicArray *dynamic_array)
{
    return dynamic_array->size == 0;
}

static void freeDynamicArrayListElement(DynamicArrayElement *element)
{
    if (element == NULL)
    {
        return;
    }

    if (element->value != NULL)
    {
        if (element->value_type == LIST_t)
        {
            FreeDynamicArray(element->value);
        }
        else
        {
            free(element->value);
        }
    }
    free(element);
}

static void freeDynamicArrayList(DynamicArrayElement **list, u_int32_t size, bool deep)
{
    if (list == NULL)
    {
        return;
    }
    if (deep)
    {
        for (u_int32_t i = 0; i < size; i++)
        {
            freeDynamicArrayListElement(list[i]);
        }
    }
    free(list);
}

extern void FreeDynamicArray(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }
    if (dynamic_array->list != NULL)
    {
        freeDynamicArrayList(dynamic_array->list, dynamic_array->size, true);
    }
    free(dynamic_array);
}

extern void PrintDynamicArray(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }
    printf("[");
    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        PrintJSONValue(dynamic_array->list[i]->value_type, dynamic_array->list[i]->value);
        if (i != dynamic_array->size - 1)
        {

            printf(", ");
        }
    }
    printf("]\n");
}

extern void DynamicArrayRemove(DynamicArray *dynamic_array, u_int32_t index)
{
    if (dynamic_array == NULL || index >= dynamic_array->size || isDynamicArrayEmpty(dynamic_array))
    {
        return;
    }
    freeDynamicArrayListElement(dynamic_array->list[index]);

    for (u_int32_t i = index + 1; i < dynamic_array->size; i++)
    {
        dynamic_array->list[i - 1] = dynamic_array->list[i];
    }

    dynamic_array->size--;
}

extern void DynamicArrayRemoveFirst(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }
    DynamicArrayRemove(dynamic_array, 0);
}

extern void DynamicArrayRemoveLastElement(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return;
    }
    DynamicArrayRemove(dynamic_array, dynamic_array->size - 1);
}

extern DynamicArrayElement *DynamicArrayElementInit(enum JSONValueType type, void *value, u_int32_t len)
{
    DynamicArrayElement *dynamicArrayElement = malloc(sizeof(DynamicArrayElement));
    if (dynamicArrayElement == NULL)
    {
        return NULL;
    }
    dynamicArrayElement->value_type = type;
    dynamicArrayElement->value = value;
    dynamicArrayElement->len = len;
    return dynamicArrayElement;
}

static DynamicArrayElement *dynamicArrayElementReplicate(DynamicArrayElement *dynamicArrayElement)
{
    if (dynamicArrayElement == NULL)
    {
        return NULL;
    }
    void *value = NULL;
    switch (dynamicArrayElement->value_type)
    {
    case NUMBER_t:
        value = (int *)malloc(sizeof(int) * dynamicArrayElement->len);
        memcpy(value, dynamicArrayElement->value, sizeof(int) * dynamicArrayElement->len);
        break;
    case STRING_t:
        value = (char *)malloc(sizeof(char) * dynamicArrayElement->len);
        memcpy(value, dynamicArrayElement->value, sizeof(char) * dynamicArrayElement->len);
        break;
    case LIST_t:
        value = (DynamicArray *)DynamicArrayReplicate(dynamicArrayElement->value);
    default:
        break;
    }
    return DynamicArrayElementInit(dynamicArrayElement->value_type, value, dynamicArrayElement->len);
}

extern DynamicArray *DynamicArrayReplicate(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return NULL;
    }
    DynamicArray *deep_clone = DynamicArrayInit(dynamic_array->capacity);

    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        deep_clone->list[i] = dynamicArrayElementReplicate(dynamic_array->list[i]);
        deep_clone->size++;
    }
    return deep_clone;
}

extern DynamicArray *DynamicArrayInitFromStr(char *input_str)
{
    if (input_str == NULL)
    {
        printf("invalid input to DynamicArrayInitFromStr\n");
        return NULL;
    }
    size_t input_str_len = strlen(input_str);
    if (input_str_len == 0 || input_str[0] != BRACKET_OPEN_CHAR || input_str[input_str_len - 1] != BRACKET_CLOSE_CHAR)
    {
        printf("invalid input to DynamicArrayInitFromStr\n");
        return NULL;
    }
    DynamicArray *dynamic_array = DefaultDynamicArrayInit();
    if (dynamic_array == NULL)
    {
        printf("out of memory for dynamic_array\n");
        return NULL;
    }

    input_str++;
    char *value_start = input_str;
    char *value_end = NULL;
    size_t char_count = 1;
    bool nested = false;
    if (*value_start == BRACKET_OPEN_CHAR)
    {
        nested = true;
    }

    while (*input_str != NULL_CHAR)
    {
        if (((*input_str == COMMA_CHAR) && !nested) || ((*input_str == BRACKET_CLOSE_CHAR) && nested) || char_count == input_str_len - 1)
        {
            enum JSONValueType element_type;
            bool is_element_arr = false;
            DynamicArrayElement *element = NULL;
            if (*input_str == COMMA_CHAR || char_count == input_str_len - 1)
            {
                value_end = input_str - 1;
            }
            else
            {
                value_end = input_str;
            }

            if (*value_start == DOUBLE_QUOTES_CHAR && *value_end == DOUBLE_QUOTES_CHAR)
            {
                value_start++;
                value_end--;
            }
            else if (*value_start == BRACKET_OPEN_CHAR && *value_end == BRACKET_CLOSE_CHAR)
            {
                is_element_arr = true;
            }
            // printf("%c %c\n", *value_start, *value_end);
            size_t value_size = (value_end - value_start) + 1;

            if (value_size > 0)
            {
                char *substring = malloc(sizeof(char) * (value_size + 1));
                CopyString(value_start, substring, value_size, 0);

                substring[value_size] = NULL_CHAR;

                if (is_element_arr)
                {
                    element_type = LIST_t;
                }
                else
                {
                    // FIXME, zero value
                    if (!atoi(substring))
                    {
                        element_type = STRING_t;
                    }
                    else
                    {
                        element_type = NUMBER_t;
                    }
                }

                if (element_type == STRING_t)
                {
                    element = DynamicArrayElementInit(element_type, substring, (value_size + 1));
                }
                else if (element_type == NUMBER_t)
                {
                    int value_temp = atoi(substring);
                    free(substring);
                    int *value = malloc(sizeof(int) * 1);
                    *value = value_temp;
                    element = DynamicArrayElementInit(element_type, value, 1);
                }
                else if (element_type == LIST_t)
                {
                    DynamicArray *sub_dyn_array = DynamicArrayInitFromStr(substring);
                    element = DynamicArrayElementInit(element_type, sub_dyn_array, sub_dyn_array->size);
                    free(substring);
                }
                else
                {
                    exit(1);
                }

                DynamicArrayAddLast(dynamic_array, element);

                input_str++;
                char_count++;
                if (*input_str == COMMA_CHAR)
                {
                    input_str++;
                    char_count++;
                }
                while (*input_str == SPACE_CHAR)
                {
                    input_str++;
                    char_count++;
                }
                value_start = input_str;
                nested = (*value_start == BRACKET_OPEN_CHAR) && (char_count != input_str_len - 1);
            }
        }
        input_str++;
        char_count++;
    }

    return dynamic_array;
}

char *DynamicArrayToString(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return NULL;
    }
    FreeDynamicArray(dynamic_array);
    return NULL;
}
