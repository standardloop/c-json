#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include <standardloop/util.h>

#include "./json.h"

static void printJSONStringValue(char *);
static void printJSONNumberIntValue(int64_t *value);
static void printJSONNumberDoubleValue(double *value);

static void printJSONBoolValue(bool *);
static void printJSONNULLValue(void);
static void printJSONListValue(DynamicArray *);
static void printJSONObjValue(HashMap *);

static char *doubleToString(double);

extern JSON *JSONInit()
{
    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        // print error
        return NULL;
    }
    json->root = NULL;
    return json;
}

extern JSON *StringToJSON(char *input_str)
{
    if (input_str == NULL)
    {
        printf("[ERROR]: input StringToJSON is NULL\n");
        return NULL;
    }

    // LexerDebugTest(input_str, true);
    Lexer *lexer = LexerInit(input_str);
    if (lexer == NULL)
    {
        return NULL;
    }

    Parser *parser = ParserInit(lexer);
    if (parser == NULL)
    {
        FreeLexer(lexer);
        return NULL;
    }
    JSON *json = ParseJSON(parser);
    if (json == NULL)
    {
        // need to differentitate between parsing error because JSON is invalid or
        // if we ran out of memory trying to parse it.
        printf("[ERROR]: couldn't parse JSON");
        return NULL;
    }

    return json;
}

extern JSON *JSONFromFile(char *filename)
{

    FILE *file_ptr = fopen(filename, "rb");

    if (file_ptr == NULL)
    {
        printf("[ERROR]: unable to open file \"%s\"\n", filename);
        return NULL;
    }
    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer != NULL)
    {
        fread(buffer, 1, length, file_ptr);
    }
    fclose(file_ptr);
    buffer[length] = NULL_CHAR;

    if (buffer == NULL)
    {
        return NULL;
    }
    JSON *json_from_string = StringToJSON(buffer);
    free(buffer);

    return json_from_string;
}

extern char *JSONToString(JSON *json)
{
    if (json == NULL)
    {
        return NULL;
    }
    char *json_as_string = JSONValueToString(json->root);
    if (json_as_string == NULL)
    {
        FreeJSON(json);
        return NULL;
    }
    FreeJSON(json);
    return json_as_string;
}

#define FLOAT_CHAR_MAX 10
static char *doubleToString(double num)
{
    // double integral;
    // double fractional = modf(num, &integral);
    // int64_t integral_as_int64 = (int64_t)integral;
    // printf("%lld %lf\n", integral_as_int64, fractional);
    char *double_as_string = malloc(sizeof(char) * FLOAT_CHAR_MAX);
    (void)gcvt(num, FLOAT_CHAR_MAX, double_as_string);

    return double_as_string;
}

extern char *JSONValueToString(JSONValue *json_value)
{
    char *json_value_string = NULL;
    switch (json_value->value_type)
    {
    case LIST_t:
        json_value_string = ListToString((DynamicArray *)json_value->value);
        break;
    case OBJ_t:
        json_value_string = ObjToString((HashMap *)json_value->value);
        break;
    case NUMBER_INT_t:
        json_value_string = Int64ToString(*(int64_t *)json_value->value);
        break;
    case NUMBER_DOUBLE_t:
        json_value_string = doubleToString(*(double *)json_value->value);
        break;
    case STRING_t:
        json_value_string = PutQuotesAroundString(json_value->value, false);
        break;
    case BOOL_t:
        if (*(bool *)json_value->value == true)
        {
            json_value_string = malloc(sizeof(char) * 5);
            strcpy(json_value_string, JSON_BOOL_TRUE);
        }
        else if (*(bool *)json_value->value == false)
        {
            json_value_string = malloc(sizeof(char) * 6);
            strcpy(json_value_string, JSON_BOOL_FALSE);
        }
        break;
    case NULL_t:
        json_value_string = malloc(sizeof(char) * 5);
        strcpy(json_value_string, JSON_NULL);
        break;
    default:
        break;
    }
    return json_value_string;
}

extern void FreeJSON(JSON *json)
{
    if (json == NULL)
    {
        return;
    }
    if (json->root != NULL)
    {
        if (json->root->value != NULL)
        {
            if (json->root->value_type == LIST_t)
            {
                FreeDynamicArray(json->root->value);
            }
            else if (json->root->value_type == OBJ_t)
            {
                FreeHashMap(json->root->value);
            }
        }
        FreeJSONValue(json->root, false);
    }
    free(json);
}

// Pretty Print can be handled by piping into jq
extern void PrintJSON(JSON *json)
{
    if (json == NULL || json->root == NULL || json->root->value == NULL)
    {
        return;
    }
    if (json->root->value_type == LIST_t)
    {
        PrintDynamicArray(json->root->value);
    }
    else if (json->root->value_type == OBJ_t)
    {
        PrintHashMap(json->root->value);
    }
}

extern void PrintJSONValue(JSONValue *json_value)
{
    if (json_value == NULL || json_value->value == NULL)
    {
        return;
    }
    switch (json_value->value_type)
    {
    case OBJ_t:
        printJSONObjValue((HashMap *)json_value->value);
        break;
    case NUMBER_INT_t:
        printJSONNumberIntValue((int64_t *)json_value->value);
        break;
    case NUMBER_DOUBLE_t:
        printJSONNumberDoubleValue((double *)json_value->value);
        break;
    case STRING_t:
        printJSONStringValue((char *)json_value->value);
        break;
    case BOOL_t:
        printJSONBoolValue((bool *)json_value->value);
        break;
    case NULL_t:
        printJSONNULLValue();
        break;
    case LIST_t:
        printJSONListValue((DynamicArray *)json_value->value);
        break;
    default:
        break;
    }
}

static void printJSONStringValue(char *value)
{
    printf("\"%s\"", value);
}

static void printJSONNumberIntValue(int64_t *value)
{
    printf("%lld", *value);
}

static void printJSONNumberDoubleValue(double *value)
{
    printf("%lf", *value);
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
