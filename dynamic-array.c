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
    // PrintSpaces(depth)
    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        PrintJSONValue(dynamic_array->list[i]->value_type, dynamic_array->list[i]->value);
        if (i != dynamic_array->size - 1)
        {

            printf(", ");
        }
    }
    printf("]");
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
    case NUMBER_DOUBLE_t:
        value = (double *)malloc(sizeof(double) * dynamicArrayElement->len);
        memcpy(value, dynamicArrayElement->value, sizeof(double) * dynamicArrayElement->len);
        break;
    case NUMBER_INT_t:
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
