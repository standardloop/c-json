#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <standardloop/collections.h>
#include <standardloop/util.h>

#include "./json.h"

static void printJSONStringValue(char *);
static void printJSONNumberIntValue(int64_t *value);
static void printJSONNumberDoubleValue(double *value);

static void printJSONBoolValue(bool *);
static void printJSONNULLValue(void);
static void printJSONListValue(DynamicArray *);
static void printJSONObjValue(JSONHashMap *);

static char *doubleToString(double);

extern JSON *JSONInit()
{
    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    json->root = NULL;
    return json;
}

extern JSON *StringToJSON(char *input_str)
{
    if (input_str == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    // JSONLexerDebugTest(input_str, true);
    JSONLexer *lexer = JSONLexerInit(input_str);
    if (lexer == NULL)
    {
        return NULL;
    }

    JSONParser *parser = JSONParserInit(lexer);
    if (parser == NULL)
    {
        FreeJSONLexer(lexer);
        return NULL;
    }
    JSON *json = ParseJSON(parser);
    if (json == NULL)
    {
        // need to differentitate between parsing error because JSON is invalid
        // or if we ran out of memory trying to parse it.
        return NULL;
    }

    return json;
}

extern JSON *JSONFromFile(char *filename)
{
    FILE *file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        return NULL;
    }

    fseek(file_ptr, 0, SEEK_END);
    u_int64_t length = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        fclose(file_ptr);
        errno = ENOMEM;
        return NULL;
    }

    fread(buffer, 1, length, file_ptr);
    fclose(file_ptr);
    buffer[length] = NULL_CHAR;

    JSON *json_from_string = StringToJSON(buffer);
    free(buffer);

    return json_from_string;
}

extern char *JSONToString(JSON *json, bool free_json)
{
    if (json == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char *json_as_string = JSONValueToString(json->root);
    if (json_as_string == NULL)
    {
        FreeJSON(json);
        return NULL;
    }
    if (free_json)
    {
        FreeJSON(json);
    }
    return json_as_string;
}

#define FLOAT_CHAR_MAX 10
static char *doubleToString(double num)
{
    char *double_as_string =
        malloc((sizeof(char) * FLOAT_CHAR_MAX) + sizeof(char));
    if (double_as_string == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    (void)gcvt(num, FLOAT_CHAR_MAX, double_as_string);
    double_as_string[FLOAT_CHAR_MAX] = NULL_CHAR;
    return double_as_string;
}

extern char *JSONValueToString(JSONValue *json_value)
{
    if (json_value == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char *json_value_string = NULL;
    switch (json_value->value_type)
    {
    case JSONLIST_t:
        json_value_string =
            DynamicArrayToString((DynamicArray *)json_value->value);
        break;
    case JSONOBJ_t:
        json_value_string = ObjToString((JSONHashMap *)json_value->value);
        break;
    case JSONNUMBER_INT_t:
        json_value_string = Int64ToString(*(int64_t *)json_value->value);
        break;
    case JSONNUMBER_DOUBLE_t:
        json_value_string = doubleToString(*(double *)json_value->value);
        break;
    case JSONSTRING_t:
        json_value_string = PutQuotesAroundString(json_value->value, false);
        break;
    case JSONBOOL_t:
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
    case JSONNULL_t:
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
        errno = EINVAL;
        return;
    }
    if (json->root != NULL)
    {
        if (json->root->value != NULL)
        {
            if (json->root->value_type == JSONLIST_t)
            {
                FreeDynamicArray(json->root->value);
            }
            else if (json->root->value_type == JSONOBJ_t)
            {
                FreeJSONHashMap(json->root->value);
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
        errno = EINVAL;
        return;
    }
    if (json->root->value_type == JSONLIST_t)
    {
        PrintDynamicArray(json->root->value);
    }
    else if (json->root->value_type == JSONOBJ_t)
    {
        PrintJSONHashMap(json->root->value);
    }
}

extern void PrintJSONValue(JSONValue *json_value)
{
    if (json_value == NULL || json_value->value == NULL)
    {
        errno = EINVAL;
        return;
    }
    switch (json_value->value_type)
    {
    case JSONOBJ_t:
        printJSONObjValue((JSONHashMap *)json_value->value);
        break;
    case JSONNUMBER_INT_t:
        printJSONNumberIntValue((int64_t *)json_value->value);
        break;
    case JSONNUMBER_DOUBLE_t:
        printJSONNumberDoubleValue((double *)json_value->value);
        break;
    case JSONSTRING_t:
        printJSONStringValue((char *)json_value->value);
        break;
    case JSONBOOL_t:
        printJSONBoolValue((bool *)json_value->value);
        break;
    case JSONNULL_t:
        printJSONNULLValue();
        break;
    case JSONLIST_t:
        printJSONListValue((DynamicArray *)json_value->value);
        break;
    default:
        break;
    }
}

static void printJSONStringValue(char *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("\"%s\"", value);
}

static void printJSONNumberIntValue(int64_t *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("%lld", *value);
}

static void printJSONNumberDoubleValue(double *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    printf("%lf", *value);
}

static void printJSONBoolValue(bool *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }

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
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    PrintDynamicArray(value);
}

static void printJSONObjValue(JSONHashMap *value)
{
    if (value == NULL)
    {
        errno = EINVAL;
        return;
    }
    PrintJSONHashMap(value);
}

extern char *JSONValueV2ToString(void *value)
{
    if (value == NULL)
    {
        return NULL;
    }
    JSONValueV2 *json_v_ptr = (JSONValueV2 *)value;
    if (json_v_ptr->value_type == JSONOBJ_t)
    {
        return HashMapToString(json_v_ptr->obj);
    }
    else if (json_v_ptr->value_type == JSONLIST_t)
    {
        return ListToString(json_v_ptr->list);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_INT_t)
    {
        return IntToString(json_v_ptr->num_int);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_DOUBLE_t)
    {
        // FIXME
        return NULL;
    }
    else if (json_v_ptr->value_type == JSONSTRING_t)
    {
        return StringToString(json_v_ptr->str);
    }
    else if (json_v_ptr->value_type == JSONBOOL_t)
    {
        if (*json_v_ptr->boolean == true)
        {
            return strdup(JSON_BOOL_TRUE);
        }
        else if (*json_v_ptr->boolean == false)
        {
            return strdup(JSON_BOOL_FALSE);
        }
    }
    else if (json_v_ptr->value_type == JSONNULL_t)
    {
        return strdup(JSON_NULL);
    }
    return NULL;
}

extern void JSONValueV2Free(void *value)
{
    if (value == NULL)
    {
        return;
    }
    JSONValueV2 *json_v_ptr = (JSONValueV2 *)value;
    if (json_v_ptr->value_type == JSONOBJ_t)
    {
        HashMapFree(json_v_ptr->obj);
    }
    else if (json_v_ptr->value_type == JSONLIST_t)
    {
        ListFree(json_v_ptr->list);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_INT_t)
    {
        free(json_v_ptr->num_int);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_DOUBLE_t)
    {
        free(json_v_ptr->num_double);
    }
    else if (json_v_ptr->value_type == JSONSTRING_t)
    {
        free(json_v_ptr->str);
    }
    else if (json_v_ptr->value_type == JSONBOOL_t)
    {
        free(json_v_ptr->boolean);
    }
    else if (json_v_ptr->value_type == JSONNULL_t)
    {
        // nothing to free, we don't store null
    }
    free(json_v_ptr);
}

extern JSONValueV2 *JSONValueV2BlankInit()
{
    JSONValueV2 *self = malloc(sizeof(JSONValueV2));
    if (self == NULL)
    {
        return NULL;
    }

    return self;
}

extern JSONValueV2 *JSONValueV2Init(enum JSONValueType value_type, void *value)
{
    JSONValueV2 *self = JSONValueV2BlankInit();
    if (self == NULL)
    {
        return NULL;
    }
    self->value_type = value_type;
    if (value_type == JSONOBJ_t)
    {
        self->obj = value;
    }
    else if (value_type == JSONLIST_t)
    {
        self->list = value;
    }
    else if (value_type == JSONNUMBER_INT_t)
    {
        self->num_int = value;
    }
    else if (value_type == JSONNUMBER_DOUBLE_t)
    {
        self->num_double = value;
    }
    else if (value_type == JSONSTRING_t)
    {
        self->str = value;
    }
    else if (value_type == JSONBOOL_t)
    {
        self->boolean = value;
    }
    else if (value_type == JSONNULL_t)
    {
        // nothing to store here
    }
    return self;
}

extern void JSONValueV2Print(void *value)
{
    if (value == NULL)
    {
        return;
    }
    JSONValueV2 *json_v_ptr = (JSONValueV2 *)value;
    if (json_v_ptr->value_type == JSONOBJ_t)
    {
        HashMapPrint(json_v_ptr->obj);
    }
    else if (json_v_ptr->value_type == JSONLIST_t)
    {
        ListPrint(json_v_ptr->list);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_INT_t)
    {
        printf("%lld", *json_v_ptr->num_int);
    }
    else if (json_v_ptr->value_type == JSONNUMBER_DOUBLE_t)
    {
        printf("%lf", *json_v_ptr->num_double);
    }
    else if (json_v_ptr->value_type == JSONSTRING_t)
    {
        printf("\"%s\"", json_v_ptr->str);
    }
    else if (json_v_ptr->value_type == JSONBOOL_t)
    {
        if (*json_v_ptr->boolean == true)
        {
            printf("%s", JSON_BOOL_TRUE);
        }
        else if (*json_v_ptr->boolean == false)
        {
            printf("%s", JSON_BOOL_FALSE);
        }
    }
    else if (json_v_ptr->value_type == JSONNULL_t)
    {
        printf("%s", JSON_NULL);
    }
}

extern void *JSONValueV2Duplicate(void *value)
{
    // TODO
    (void)value;
    return NULL;
}

ItemValueOperations ItemValueLinkedListOperations = {
    .toStringFunction = JSONValueV2ToString,
    .freeFunction = JSONValueV2Free,
    .printFunction = JSONValueV2Print,
    .duplicateFunction = JSONValueV2Duplicate};
