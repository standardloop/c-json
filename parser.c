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
    parser->current_token = NULL;
    parser->peek_token = NULL;

    nextToken(parser);
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
    FreeParser(parser);
    return json;
}