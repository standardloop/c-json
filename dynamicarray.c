#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./json.h"

static inline bool isDynamicArrayFull(DynamicArray *);
static inline bool isDynamicArrayEmpty(DynamicArray *);
static void freeDynamicArrayList(JSONValue **, u_int32_t, bool);
static void dynamicArrayResize(DynamicArray *);

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
    dynamic_array->list = malloc(sizeof(JSONValue *) * dynamic_array->capacity);
    if (dynamic_array->list == NULL)
    {
        FreeDynamicArray(dynamic_array);
        printf("[ERROR]: could not allocate memory for DynamicArray list\n");
        return NULL;
    }
    return dynamic_array;
}

extern void DynamicArrayAddFirst(DynamicArray *dynamic_array, JSONValue *element)
{
    DynamicArrayAdd(dynamic_array, element, 0);
}

extern void DynamicArrayAddLast(DynamicArray *dynamic_array, JSONValue *element)
{
    if (dynamic_array == NULL || element == NULL)
    {
        return;
    }
    DynamicArrayAdd(dynamic_array, element, dynamic_array->size);
}

extern void DynamicArrayAdd(DynamicArray *dynamic_array, JSONValue *element, u_int32_t index)
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

    JSONValue **newList = malloc(sizeof(JSONValue *) * dynamic_array->capacity * DEFAULT_DYN_ARR_RESIZE_MULTIPLE);
    if (newList == NULL)
    {
        printf("Couldn't resize list, not enough memory!\n");
        return;
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

static void freeDynamicArrayList(JSONValue **list, u_int32_t size, bool deep)
{
    if (list == NULL)
    {
        return;
    }
    if (deep)
    {
        for (u_int32_t i = 0; i < size; i++)
        {
            FreeJSONValue(list[i], deep);
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
        PrintJSONValue(dynamic_array->list[i]);
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
    FreeJSONValue(dynamic_array->list[index], true);

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

extern DynamicArray *DynamicArrayReplicate(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return NULL;
    }
    DynamicArray *deep_clone = DynamicArrayInit(dynamic_array->capacity);

    for (u_int32_t i = 0; i < dynamic_array->size; i++)
    {
        deep_clone->list[i] = JSONValueReplicate(dynamic_array->list[i]);
        deep_clone->size++;
    }
    return deep_clone;
}

extern JSONValue *DynamicArrayGetAtIndex(DynamicArray *dynamic_array, u_int32_t index)
{
    if (dynamic_array == NULL || dynamic_array->size < index)
    {
        return NULL;
    }
    return dynamic_array->list[index];
}
