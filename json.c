#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./json.h"
#include "./util.h"

static void printJSONStringValue(char *);
static void printJSONNumberIntValue(int *value);
static void printJSONNumberDoubleValue(double *value);

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
    if ((input_str[0] != CURLY_OPEN_CHAR && input_str[0] != BRACKET_OPEN_CHAR) ||
        (input_str[0] == CURLY_OPEN_CHAR && input_str[input_str_len - 1] != CURLY_CLOSE_CHAR) ||
        (input_str[0] == BRACKET_OPEN_CHAR && input_str[input_str_len - 1] != BRACKET_CLOSE_CHAR))
    {
        printf("[ERROR]: input StringToJSON is not valid JSON\n");
        return NULL;
    }

    // LexerDebugTest(input_str);
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

extern JSON *JSONFromFile(char *filename)
{

    FILE *file_ptr = fopen(filename, "rb");

    if (file_ptr == NULL)
    {
        return NULL;
    }
    char *buffer = NULL;
    fseek(file_ptr, 0, SEEK_END);
    long length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, file_ptr);
    }
    fclose(file_ptr);

    if (buffer == NULL)
    {
        return NULL;
    }
    return StringToJSON(buffer);
}

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
    if (json->root_value->value_type == LIST_t)
    {
        FreeDynamicArray(json->root_value->value);
    }
    else if (json->root_value->value_type == OBJ_t)
    {
        FreeHashMap(json->root_value->value);
    }
    FreeJSONValue(json->root_value, false);
    free(json);
}

// Pretty Print can be handled by piping into jq
extern void PrintJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
    if (json->root_value->value_type == LIST_t)
    {
        PrintDynamicArray(json->root_value->value);
    }
    else if (json->root_value->value_type == OBJ_t)
    {
        PrintHashMap(json->root_value->value);
    }
}

extern void PrintJSONValue(enum JSONValueType value_type, void *value)
{
    switch (value_type)
    {
    case OBJ_t:
        printJSONObjValue(value);
        break;
    case NUMBER_INT_t:
        printJSONNumberIntValue(value);
        break;
    case NUMBER_DOUBLE_t:
        printJSONNumberDoubleValue(value);
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

static void printJSONNumberIntValue(int *value)
{
    printf("%d", *value);
}

static void printJSONNumberDoubleValue(double *value)
{
    printf("%f", *value);
}

static void printJSONBoolValue(bool *value)
{
    if (*value == true)
    {
        printf("%s", JSON_BOOL_TRUE);
    }
    else if (*value == false)
    {
        printf("%s", JSON_BOOL_FALSE);
    }
}

static void printJSONNULLValue(void)
{
    printf("%s", JSON_NULL);
}

static void printJSONListValue(DynamicArray *value)
{
    PrintDynamicArray(value);
}

static void printJSONObjValue(HashMap *value)
{
    PrintHashMap(value);
}
