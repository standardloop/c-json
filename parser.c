#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"
#include "./util.h"

static void nextToken(Parser *);

extern Parser *ParserInit(Lexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }

    Parser *parser = malloc(sizeof(Parser));
    if (parser == NULL)
    {
        return NULL;
    }
    parser->lexer = lexer;
    parser->current_token = NewToken(TokenEOF, 0, 0, NULL_CHAR_STRING);
    parser->peek_token = NewToken(TokenEOF, 0, 0, NULL_CHAR_STRING);

    // nextToken(parser);
    // nextToken(parser);

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

static void nextToken(Parser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    // FreeToken(parser->current_token);
    parser->current_token = parser->peek_token;
    parser->peek_token = Lex(parser->lexer);
}

DynamicArray *parserCreateDynamicArray(Parser *parser)
{
    DynamicArray *arr = DefaultDynamicArrayInit();
    nextToken(parser);
    while (ALWAYS)
    {
        if (parser->current_token->type == TokenCloseBracket || parser->current_token->type == TokenEOF)
        {
            break;
        }
        JSONValue *value = parse(parser);
        DynamicArrayAddFirst(arr, NULL);
    }
}

static JSONValue *parse(Parser *parser)
{
    if (parser == NULL)
    {
        return NULL;
    }

    nextToken(parser);
    if (parser->current_token->type == TokenOpenCurlyBrace)
    {
    }
    else if (parser->current_token->type == TokenOpenBracket)
    {
    }
    else if (parser->current_token->type == TokenString)
    {
    }
    else if (parser->current_token->type == TokenNumber)
    {
    }
    else if (parser->current_token->type == TokenNumber)
    {
    }
    else if (parser->current_token->type == TokenBool)
    {
    }
    else if (parser->current_token->type == TokenNULL)
    {
    }
    else
    {
        printf("big error in static JSONValue *parse\n");
        return NULL;
    }
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

    nextToken(parser);
    enum JSONValueType root_type;
    if (parser->current_token->type == TokenOpenBracket)
    {
        json->map = NULL;
        json->array = DefaultDynamicArrayInit();
        root_type = LIST_t;
    }
    else if (parser->current_token->type == TokenOpenCurlyBrace)
    {
        json->array = NULL;
        json->map = DefaultHashMapInit();
        root_type = OBJ_t;
    }
    else
    {
        FreeParser(parser);
        printf("invalid JSON from Parser");
        return NULL;
    }
    nextToken(parser);
    while (ALWAYS)
    {
        if (parser->current_token->type != TokenEOF)
        {
            break;
        }
        JSONValue *json_value = parse(parser);
        if (root_type == LIST_t)
        {
            DynamicArrayElement *array_ele = NULL;
            DynamicArrayAddFirst(json->array, array_ele);
        }
        else if (root_type == OBJ_t)
        {
            HashMapEntry *entry = NULL;
            HashMapInsert(json->map, entry);
        }
    }

    FreeParser(parser);
    return json;
}