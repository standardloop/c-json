#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"
#include "./util.h"

static void nextToken(Parser *);
static JSONValue *parse(Parser *);
static JSONValue *parseList(Parser *);
static JSONValue *parseObj(Parser *);
static JSONValue *parseNumber(Parser *);

static JSONValue *initQuickJSONValue(enum JSONValueType, void *);

extern Parser *ParserInit(Lexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }

    Parser *parser = malloc(sizeof(Parser));
    if (parser == NULL)
    {
        FreeLexer(lexer);
        return NULL;
    }
    parser->error = NULL;
    parser->lexer = lexer;
    parser->current_token = NewToken(TokenEOF, 0, 0, 0, NULL_CHAR_STRING);
    parser->peek_token = NewToken(TokenEOF, 0, 0, 0, NULL_CHAR_STRING);

    nextToken(parser);

    return parser;
}

extern void FreeParser(Parser *parser)
{
    if (parser != NULL)
    {
        if (parser->lexer != NULL)
        {
            FreeLexer(parser->lexer);
        }
        if (parser->current_token != NULL)
        {
            FreeToken(parser->current_token);
        }
        if (parser->peek_token != NULL)
        {
            FreeToken(parser->peek_token);
        }
        free(parser);
    }
}

extern void PrintParserError(Parser *parser)
{
    if (parser->error != NULL)
    {
        printf("%s\n", parser->error);
    }
}

static void nextToken(Parser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    FreeToken(parser->current_token);
    parser->current_token = parser->peek_token;
    parser->peek_token = Lex(parser->lexer);
}

static JSONValue *parseList(Parser *parser)
{
    // printf("parseList\n");
    // PrintToken(parser->current_token);
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    if (parser->current_token->type == TokenOpenBracket)
    {
        // nextToken(parser);
    }

    DynamicArray *list = DefaultDynamicArrayInit();
    // while (parser->current_token->type == TokenEOF)
    while (ALWAYS)
    {
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenComma)
        {
            nextToken(parser);
            break;
        }
        if (parser->current_token->type == TokenCloseBracket && parser->peek_token->type == TokenEOF)
        {
            break;
        }
        if (parser->peek_token->type == TokenCloseCurlyBrace)
        {
            break;
        }
        if (parser->current_token->type == TokenIllegal)
        {
            FreeDynamicArray(list);
            FreeJSONValue(json_value, false);
            PrintToken(parser->current_token);
            parser->error = "[ERROR]: Invalid token";
            return NULL;
        }
        JSONValue *list_value = parse(parser);
        if (list_value == NULL)
        {
            // FIXME
            // if ((parser->current_token->type == TokenCloseBracket && parser->peek_token->type != TokenEOF && parser->peek_token->type != TokenComma && parser->peek_token->type != TokenCloseBracket) || (parser->current_token->type == COLON_CHAR && !IsTokenValueType(parser->current_token, true)))
            // {
            //     FreeDynamicArray(list);
            //     FreeJSONValue(json_value, false);
            //     FreeJSONValue(list_value, false);
            //     PrintToken(parser->current_token);
            //     parser->error = "[ERROR]: Expected value";
            //     return NULL;
            // }
            // else
            // {
            //     continue;
            // }
        }
        else
        {
            DynamicArrayAddLast(list, list_value);
        }
    }
    json_value->value_type = LIST_t;
    json_value->value = list;
    return json_value;
}

extern void FreeJSONValue(JSONValue *json_value, bool deep)
{
    if (json_value != NULL)
    {
        if (deep)
        {
            if (json_value->value_type == LIST_t)
            {
                FreeDynamicArray(json_value->value);
            }
            else if (json_value->value_type == OBJ_t)
            {
                FreeHashMap(json_value->value);
            }
            else if (json_value->value_type != NULL_t)
            {
                free(json_value->value);
            }
        }
        free(json_value);
    }
}

static JSONValue *parseObj(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }

    HashMap *map = DefaultHashMapInit();
    while (ALWAYS)
    {
        // PrintToken(parser->current_token);
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenComma)
        {
            nextToken(parser);
            break;
        }
        if (parser->current_token->type == TokenCloseCurlyBrace && parser->peek_token->type == TokenEOF)
        {
            break;
        }
        if (parser->peek_token->type == TokenCloseBracket)
        {
            break;
        }
        if (parser->current_token->type == TokenIllegal)
        {
            FreeHashMap(map);
            return NULL;
        }
        // nextToken(parser);
        JSONValue *obj_key = parse(parser);
        // if (obj_key != NULL && obj_key->value_type != STRING_t)
        // {
        //     printf("FIXME\n");
        // }
        if (obj_key != NULL)
        {
            if (parser->peek_token->type == TokenColon)
            {
                nextToken(parser); // skip over colon
                if (parser->current_token->type == TokenColon)
                {
                    JSONValue *obj_value = parse(parser);
                    obj_value->key = obj_key->value;
                    if (obj_value != NULL)
                    {
                        HashMapInsert(map, obj_value);
                    }
                }
            }
            else
            {
                pass;
            }
            FreeJSONValue(obj_key, false);
        }

        // FreeJSONValue();
    }
    json_value->value_type = OBJ_t;
    json_value->value = map;
    return json_value;
}

// Can this be combined with JSONValueInit?
static JSONValue *initQuickJSONValue(enum JSONValueType value_type, void *value)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    u_int32_t value_len;
    if (value_type == STRING_t)
    {
        value_len = strlen((char *)value) + 1;
        json_value->value = value;
    }
    else if (value_type == NULL_t)
    {
        value_len = 0;
        json_value->value = NULL;
        free(value);
    }
    else if (value_type == BOOL_t)
    {
        bool *new_bool = malloc(sizeof(bool));
        if (((char *)value)[0] == 't')
        {
            *new_bool = true;
        }
        else
        {
            *new_bool = false;
        }
        value_len = 1;
        json_value->value = new_bool;
        free(value);
    }
    else if (value_type == NUMBER_DOUBLE_t)
    {
        double *new_double = malloc(sizeof(double));
        double temp = atof((char *)value);
        *new_double = temp;
        value_len = 1;
        json_value->value = new_double;
        free(value);
    }
    else if (value_type == NUMBER_INT_t)
    {
        int *new_int = malloc(sizeof(int));
        int temp = atoi((char *)value);
        *new_int = temp;
        value_len = 1;
        json_value->value = new_int;
        free(value);
    }
    else
    {
        json_value->value = NULL;
        value_len = 0;
        // free(value); // FIXME
    }
    json_value->value_type = value_type;
    json_value->value_len = value_len;
    return json_value;
}

static JSONValue *parseNumber(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    if (IsCharInString(parser->current_token->literal, DOT_CHAR))
    {
        return initQuickJSONValue(NUMBER_DOUBLE_t, parser->current_token->literal);
    }
    else
    {
        return initQuickJSONValue(NUMBER_INT_t, parser->current_token->literal);
    }
}

static JSONValue *parse(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }

    nextToken(parser);
    // PrintToken(parser->current_token);
    JSONValue *return_value = NULL;

    if (parser->current_token->type == TokenOpenCurlyBrace)
    {
        return_value = parseObj(parser);
    }
    else if (parser->current_token->type == TokenOpenBracket)
    {
        // printf("Parsing List!\n");
        return_value = parseList(parser);
    }
    else if (parser->current_token->type == TokenString)
    {
        // printf("TokenString\n");
        return_value = initQuickJSONValue(STRING_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenNumber)
    {
        return_value = parseNumber(parser);
    }
    else if (parser->current_token->type == TokenBool)
    {
        return_value = initQuickJSONValue(BOOL_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenNULL)
    {
        // printf("test\n");
        return_value = initQuickJSONValue(NULL_t, parser->current_token->literal);
    }
    else if (parser->current_token->type == TokenIllegal)
    {
        // exit(100);
    }
    // else
    // {
    //     return NULL;
    // }
    // else if (parser->current_token->type == TokenEOF)
    // {
    //     // FIXME
    //     // printf("found token EOF!\n");
    //     return NULL;
    // }
    // else if (parser->current_token->type == TokenComma)
    // {
    //     // printf("found a comma!!\n");
    //     return NULL;
    // }
    // else if (parser->current_token->type == TokenCloseBracket)
    // {
    //     // printf("found a TokenCloseBracket\n");
    //     return NULL;
    // }
    // else
    // {
    //     printf("big error in JSONValue *parse\n");
    //     exit(1);
    //     return NULL;
    // }
    // FreeToken(parser->current_token);

    return return_value;
}

extern JSON *ParseJSON(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }
    JSON *json = malloc(sizeof(JSON));
    if (json == NULL)
    {
        FreeParser(parser);
        return NULL;
    }
    json->root = parse(parser);
    if (json->root == NULL)
    {
        PrintParserError(parser);
        FreeParser(parser);
        FreeJSON(json);
        return NULL;
    }
    FreeParser(parser);
    return json;
}

extern JSONValue *JSONValueInit(enum JSONValueType type, void *value, char *key, u_int32_t len)
{
    JSONValue *json_value = malloc(sizeof(JSONValue));
    if (json_value == NULL)
    {
        return NULL;
    }
    json_value->key = key;
    json_value->value_type = type;
    json_value->value = value;
    json_value->value_len = len;
    json_value->next = NULL;
    return json_value;
}

// FIXME, object support
extern JSONValue *JSONValueReplicate(JSONValue *json_value)
{
    if (json_value == NULL)
    {
        return NULL;
    }
    void *value = NULL;
    switch (json_value->value_type)
    {
    case NUMBER_DOUBLE_t:
        value = (double *)malloc(sizeof(double) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(double) * json_value->value_len);
        break;
    case NUMBER_INT_t:
        value = (int *)malloc(sizeof(int) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(int) * json_value->value_len);
        break;
    case STRING_t:
        value = (char *)malloc(sizeof(char) * json_value->value_len);
        memcpy(value, json_value->value, sizeof(char) * json_value->value_len);
        break;
    case LIST_t:
        value = (DynamicArray *)DynamicArrayReplicate(json_value->value);
    default:
        break;
    }
    return JSONValueInit(json_value->value_type, value, NULL, json_value->value_len);
}
