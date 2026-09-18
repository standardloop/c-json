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

extern char *JSONToString(JSON *json)
{
    if (json == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char *json_as_string = JSONValueToString(json->root);
    if (json_as_string == NULL)
    {
        JSONFree(json);
        return NULL;
    }
    return json_as_string;
}

extern char *JSONValueToString(void *value)
{
    if (value == NULL)
    {
        return NULL;
    }
    JSONValue *json_v_ptr = (JSONValue *)value;
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

extern void JSONValueFree(void *value)
{
    if (value == NULL)
    {
        return;
    }
    JSONValue *json_v_ptr = (JSONValue *)value;
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

extern JSONValue *JSONValueBlankInit()
{
    JSONValue *self = malloc(sizeof(JSONValue));
    if (self == NULL)
    {
        return NULL;
    }

    return self;
}

extern JSONValue *JSONValueInit(enum JSONValueType value_type, void *value)
{
    JSONValue *self = JSONValueBlankInit();
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

extern void JSONValuePrint(void *value)
{
    if (value == NULL)
    {
        return;
    }
    JSONValue *json_v_ptr = (JSONValue *)value;
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

extern void *JSONValueDuplicate(void *value)
{
    // TODO
    (void)value;
    return NULL;
}

ItemValueOperations ItemValueJSONValueOperations = {
    .toStringFunction = JSONValueToString,
    .freeFunction = JSONValueFree,
    .printFunction = JSONValuePrint,
    .duplicateFunction = JSONValueDuplicate};

extern JSON *JSONInit()
{
    JSON *self = malloc(sizeof(JSON));
    if (self == NULL)
    {
        return NULL;
    }
    self->root = NULL;
    return self;
}

extern void JSONFree(JSON *json)
{
    if (json != NULL)
    {
        if (json->root != NULL)
        {
            ItemFree(json->root);
        }
        free(json);
    }
}

extern void JSONPrint(JSON *json)
{
    if (json != NULL)
    {
        ItemPrint(json->root);
    }
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

extern JSONValue *JSONGetRoot(JSON *json)
{
    if (json == NULL)
    {
        return NULL;
    }
    return (JSONValue *)json->root->value;
}
