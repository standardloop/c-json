#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./json.h"
#include "./util.h"

static inline bool isDynamicArrayFull(DynamicArray *);
static inline bool isDynamicArrayEmpty(DynamicArray *);
static void freeDynamicArrayList(JSONValue **, u_int32_t, bool);
static void dynamicArrayResize(DynamicArray *);

static JSONValue *json_elementReplicate(JSONValue *);

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
            FreeJSONValue(list[i], true);
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

static JSONValue *json_elementReplicate(JSONValue *json_element)
{
    if (json_element == NULL)
    {
        return NULL;
    }
    void *value = NULL;
    switch (json_element->value_type)
    {
    case NUMBER_DOUBLE_t:
        value = (double *)malloc(sizeof(double) * json_element->value_len);
        memcpy(value, json_element->value, sizeof(double) * json_element->value_len);
        break;
    case NUMBER_INT_t:
        value = (int *)malloc(sizeof(int) * json_element->value_len);
        memcpy(value, json_element->value, sizeof(int) * json_element->value_len);
        break;
    case STRING_t:
        value = (char *)malloc(sizeof(char) * json_element->value_len);
        memcpy(value, json_element->value, sizeof(char) * json_element->value_len);
        break;
    case LIST_t:
        value = (DynamicArray *)DynamicArrayReplicate(json_element->value);
    default:
        break;
    }
    return JSONValueInit(json_element->value_type, value, json_element->value_len);
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
        deep_clone->list[i] = json_elementReplicate(dynamic_array->list[i]);
        deep_clone->size++;
    }
    return deep_clone;
}

extern char *DynamicArrayToString(DynamicArray *dynamic_array)
{
    if (dynamic_array == NULL)
    {
        return NULL;
    }
    return NULL;
}

extern JSONValue *DynamicArrayGetAtIndex(DynamicArray *dynamic_array, u_int32_t index)
{
    if (dynamic_array == NULL || dynamic_array->size < index)
    {
        return NULL;
    }
    return dynamic_array->list[index];
}
