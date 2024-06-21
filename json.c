#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./json.h"
#include "./util.h"

static void printJSONStringValue(char *);
static void printJSONNumberValue(int *);
static void printJSONBoolValue(bool *);
static void printJSONNULLValue(void);
static void printJSONListValue(DynamicArray *);
static void printJSONObjValue(HashMap *);

extern JSON *StringToJSON(char *input_str)
{
    if (input_str == NULL)
    {
        printf("[ERROR]: input StringToJSON is NULL\n");
        return NULL;
    }
    size_t input_str_len = strlen(input_str);
    // printf("[STRLEN]: %d\n", (int)input_str_len);
    if ((input_str[0] != CURLY_OPEN_CHAR && input_str[0] != BRACKET_OPEN_CHAR) ||
        (input_str[0] == CURLY_OPEN_CHAR && input_str[input_str_len - 1] != CURLY_CLOSE_CHAR) ||
        (input_str[0] == BRACKET_OPEN_CHAR && input_str[input_str_len - 1] != BRACKET_CLOSE_CHAR))
    {
        printf("[ERROR]: input StringToJSON is not valid JSON\n");
        return NULL;
    }

    // if (input_str[0] == BRACKET_OPEN_CHAR)
    // {
    //     json->map = NULL;
    //     json->array = DynamicArrayInitFromStr(input_str);
    //     if (json->array == NULL)
    //     {
    //         FreeJSON(json);
    //         return NULL;
    //     }
    // }
    // else if (input_str[0] == CURLY_OPEN_CHAR)
    // {
    //     json->array = NULL;
    //     json->map = HashMapInitFromStr(input_str);
    //     if (json->map == NULL)
    //     {
    //         FreeJSON(json);
    //         return NULL;
    //     }
    // }

    // LexerRunTest(input_str);
    Lexer *lexer = LexerInit(input_str);
    if (lexer == NULL)
    {
        return NULL;
    }
    Parser *parser = ParserInit(lexer);
    if (parser == NULL)
    {
        return NULL;
    }

    return ParseJSON(parser);
}

// static void *stringToJSON(char *input_str, enum JSONValueType type)
// {
//     if (input_str == NULL || (type != LIST_t && type != OBJ_t))
//     {
//         return NULL;
//     }
//     return NULL;
// }

extern char *JSONToString(JSON *json)
{
    if (json == NULL)
    {
        return NULL;
    }
    FreeJSON(json);
    return NULL;
}

extern void FreeJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
    if (json->array != NULL)
    {
        FreeDynamicArray(json->array);
    }
    if (json->map != NULL)
    {
        FreeHashMap(json->map);
    }
    free(json);
}

extern void PrintJSON(JSON *json)
{
    // TODO: pretty print with map and array
    if (json == NULL)
    {
        return;
    }
    if (json->array != NULL)
    {
        PrintDynamicArray(json->array);
    }
    else if (json->map != NULL)
    {
        PrintHashMap(json->map);
    }
}

extern void PrintJSONValue(enum JSONValueType value_type, void *value)
{
    switch (value_type)
    {
    case OBJ_t:
        printJSONObjValue(value);
        break;
    case NUMBER_t:
        printJSONNumberValue(value);
        break;
    case STRING_t:
        printJSONStringValue(value);
        break;
    case BOOL_t:
        printJSONBoolValue(value);
        break;
    case NULL_t:
        printJSONNULLValue();
        break;
    case LIST_t:
        printJSONListValue(value);
        break;
    default:
        break;
    }
}

static void printJSONStringValue(char *value)
{
    printf("\"%s\"", value);
}

static void printJSONNumberValue(int *value)
{
    printf("%d", *value);
}

static void printJSONBoolValue(bool *value)
{
    if (*value == true)
    {
        printf("true");
    }
    else if (*value == false)
    {
        printf("false");
    }
}

static void printJSONNULLValue(void)
{
    printf("null");
}

static void printJSONListValue(DynamicArray *value)
{
    PrintDynamicArray(value);
}

static void printJSONObjValue(HashMap *value)
{
    PrintHashMap(value);
}
