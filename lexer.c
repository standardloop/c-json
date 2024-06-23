#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"
#include "./util.h"

static void advanceChar(Lexer *);
static void backtrackChar(Lexer *);
static void skipWhitespace(Lexer *);
static char *makeStringLiteral(Lexer *, char);
static char *makeNumberLiteral(Lexer *);
static char *makeNULLLiteral(Lexer *);
static char *makeBoolLiteral(Lexer *);

extern Lexer *LexerInit(char *input)
{
    Lexer *lexer = malloc(sizeof(Lexer));
    if (lexer == NULL)
    {
        return NULL;
    }
    lexer->input = input;
    lexer->input_len = strlen(input) + 1;
    lexer->current_char = NULL_CHAR;
    lexer->position = -2;
    lexer->read_position = -1;
    lexer->line = 1;

    advanceChar(lexer);

    return lexer;
}

static void advanceChar(Lexer *lexer)
{
    if (lexer->read_position >= lexer->input_len)
    {
        lexer->current_char = NULL_CHAR;
    }
    else
    {
        lexer->current_char = lexer->input[lexer->read_position];
    }
    lexer->position = lexer->read_position;
    lexer->read_position++;
}

static void backtrackChar(Lexer *lexer)
{
    lexer->position -= 2;
    lexer->read_position--;
    lexer->current_char = lexer->input[lexer->read_position];
}

static void skipWhitespace(Lexer *lexer)
{
    while (lexer->current_char == SPACE_CHAR || lexer->current_char == TAB_CHAR || lexer->current_char == NEWLINE_CHAR || lexer->current_char == CARRIAGE_CHAR)
    {
        if (lexer->current_char == NEWLINE_CHAR)
        {
            lexer->line++;
        }
        advanceChar(lexer);
    }
}

extern Token *NewToken(enum TokenType type, u_int32_t start, u_int32_t end, u_int32_t line_num, char *literal)
{
    Token *token = malloc(sizeof(Token));
    if (token == NULL)
    {
        return NULL;
    }
    token->type = type;
    token->start = start;
    token->end = end;
    token->literal = literal;
    token->line = line_num;
    return token;
}

// FIXME position needs changing!!!!
extern Token *Lex(Lexer *lexer)
{
    Token *token = NULL;
    advanceChar(lexer);
    skipWhitespace(lexer);

    u_int32_t curr_pos = lexer->position;
    if (lexer->current_char == NULL_CHAR)
    {
        token = NewToken(TokenEOF, curr_pos, lexer->position + 1, lexer->line, NULL_CHAR_STRING);
    }
    else if (lexer->current_char == CURLY_OPEN_CHAR)
    {
        token = NewToken(TokenOpenCurlyBrace, curr_pos, lexer->position + 1, lexer->line, TOKEN_OPEN_CURLY_BRACE_STRING);
    }
    else if (lexer->current_char == CURLY_CLOSE_CHAR)
    {
        token = NewToken(TokenCloseCurlyBrace, curr_pos, lexer->position + 1, lexer->line, TOKEN_CLOSE_CURLY_BRACE_STRING);
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
        token = NewToken(TokenOpenBracket, curr_pos, lexer->position + 1, lexer->line, TOKEN_OPEN_BRACKET_STRING);
    }
    else if (lexer->current_char == BRACKET_CLOSE_CHAR)
    {
        token = NewToken(TokenCloseBracket, curr_pos, lexer->position + 1, lexer->line, TOKEN_CLOSE_BRACKET_STRING);
    }
    else if (lexer->current_char == COMMA_CHAR)
    {
        token = NewToken(TokenComma, curr_pos, lexer->position + 1, lexer->line, TOKEN_COMMA_STRING);
    }
    else if (lexer->current_char == COLON_CHAR)
    {
        token = NewToken(TokenColon, curr_pos, lexer->position + 1, lexer->line, TOKEN_COLON_STRING);
    }
    else if (lexer->current_char == DOUBLE_QUOTES_CHAR)
    {
        char *string_literal = makeStringLiteral(lexer, DOUBLE_QUOTES_CHAR);
        if (string_literal == NULL)
        {
            token = NewToken(TokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewToken(TokenString, curr_pos, lexer->position + 1, lexer->line, string_literal);
        }
    }
    else if (IsDigitOrMinusSign(lexer->current_char))
    {
        char *number_literal = makeNumberLiteral(lexer);
        if (number_literal == NULL)
        {
            token = NewToken(TokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewToken(TokenNumber, curr_pos, lexer->position + 1, lexer->line, number_literal);
            backtrackChar(lexer);
        }
    }
    else if (lexer->current_char == 't' || lexer->current_char == 'f')
    {
        char *bool_literal = makeBoolLiteral(lexer);
        if (bool_literal == NULL)
        {
            token = NewToken(TokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewToken(TokenBool, curr_pos, lexer->position + 1, lexer->line, bool_literal);
        }
    }
    else if (lexer->current_char == 'n')
    {
        char *null_literal = makeNULLLiteral(lexer);
        if (null_literal == NULL)
        {
            token = NewToken(TokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewToken(TokenNULL, curr_pos, lexer->position + 1, lexer->line, null_literal);
        }
    }
    else
    {
        token = NewToken(TokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
    }

    return token;
}

static char *makeBoolLiteral(Lexer *lexer)
{
    char first_char = lexer->current_char;
    u_int32_t start_position = lexer->position;
    if (first_char != 't' && first_char != 'f')
    {
        return NULL;
    }

    while (ALWAYS)
    {
        // true and false both end with e :)
        if (lexer->current_char == NULL_CHAR || lexer->current_char == 'e')
        {
            break;
        }
        advanceChar(lexer);
    }
    u_int32_t bool_literal_size = (lexer->position - start_position) + 1;
    if (lexer->current_char == 'e')
    {
        bool_literal_size++;
    }
    char *bool_literal = malloc(sizeof(char) * bool_literal_size);
    if (bool_literal == NULL)
    {
        return NULL;
    }
    CopyString(lexer->input, bool_literal, bool_literal_size, start_position);
    bool_literal[bool_literal_size - 1] = NULL_CHAR;

    if (strcmp(bool_literal, JSON_BOOL_TRUE) != 0 && strcmp(bool_literal, JSON_BOOL_FALSE) != 0)
    {
        free(bool_literal);
        return NULL;
    }
    return bool_literal;
}

static char *makeNULLLiteral(Lexer *lexer)
{
    if (lexer == NULL)
    {
        return NULL;
    }
    u_int32_t start_position = lexer->position;
    u_int8_t l_count = 0;
    while (ALWAYS)
    {
        if (lexer->current_char == 'l')
        {
            l_count++;
        }
        if (lexer->current_char == NULL_CHAR || l_count >= 2)
        {
            break;
        }
        advanceChar(lexer);
    }
    u_int32_t null_literal_size = (lexer->position - start_position) + 1;
    if (lexer->current_char == 'l')
    {
        null_literal_size++;
    }
    char *null_literal = malloc(sizeof(char) * null_literal_size);
    if (null_literal == NULL)
    {
        return NULL;
    }
    CopyString(lexer->input, null_literal, null_literal_size, start_position);
    null_literal[null_literal_size - 1] = NULL_CHAR;

    if (strcmp(null_literal, JSON_NULL) != 0)
    {
        return NULL;
    }
    return null_literal;
}

static char *makeNumberLiteral(Lexer *lexer)
{
    u_int32_t start_position = lexer->position;
    if (lexer->current_char == DASH_MINUS_CHAR)
    {
        advanceChar(lexer);
    }

    u_int8_t decimal_count = 0;
    while (ALWAYS)
    {
        if (lexer->current_char == DOT_CHAR)
        {
            decimal_count++;
        }
        else if (lexer->current_char == NULL_CHAR || !isdigit(lexer->current_char))
        {
            break;
        }
        advanceChar(lexer);
    }
    if (decimal_count > 1)
    {
        return NULL;
    }
    u_int32_t number_literal_size = (lexer->position - start_position) + 1;
    char *number_literal = malloc(sizeof(char) * number_literal_size);
    if (number_literal == NULL)
    {
        return NULL;
    }
    CopyString(lexer->input, number_literal, number_literal_size, start_position);
    number_literal[number_literal_size - 1] = NULL_CHAR;
    return number_literal;
}

static char *makeStringLiteral(Lexer *lexer, char delimiter)
{
    u_int32_t start_position = lexer->position + 1;
    while (ALWAYS)
    {
        advanceChar(lexer);

        if (lexer->current_char == delimiter || lexer->current_char == NULL_CHAR)
        {
            break;
        }
    }
    u_int32_t string_literal_size = (lexer->position - start_position) + 1;
    char *string_literal = malloc(sizeof(char) * string_literal_size);
    if (string_literal == NULL)
    {
        return NULL;
    }
    CopyString(lexer->input, string_literal, string_literal_size, start_position);
    string_literal[string_literal_size - 1] = NULL_CHAR;
    return string_literal;
}

extern void PrintToken(Token *token)
{
    if (token == NULL)
    {
        return;
    }
    switch (token->type)
    {
    case TokenEOF:
        printf("Token: Line: %u Place: %u %u Kind: TokenEOF\n", token->line, token->start, token->end);
        break;
    case TokenColon:
        printf("Token: Line: %u Place: %u %u Kind: TokenColon, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenOpenCurlyBrace:
        printf("Token: Line: %u Place: %u %u Kind: TokenOpenCurlyBrace, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenCloseCurlyBrace:
        printf("Token: Line: %u Place: %u %u Kind: TokenCloseCurlyBrace, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenOpenBracket:
        printf("Token: Line: %u Place: %u %u Kind: TokenOpenBracket, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenCloseBracket:
        printf("Token: Line: %u Place: %u %u Kind: TokenCloseBracket, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenComma:
        printf("Token: Line: %u Place: %u %u Kind: TokenComma, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenString:
        printf("Token: Line: %u Place: %u %u Kind: TokenString, Literal: \"%s\"\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenNumber:
        printf("Token: Line: %u Place: %u %u Kind: TokenNumber, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenBool:
        printf("Token: Line: %u Place: %u %u Kind: TokenBool, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenNULL:
        printf("Token: Line: %u Place: %u %u Kind: TokenNULL, Literal: %s\n", token->line, token->start, token->end, token->literal);
        break;
    case TokenIllegal:
    default:
        printf("Token: Line: %u Place: %u %u Kind: TokenIllegal\n", token->line, token->start, token->end);
        break;
    }
}

extern void FreeToken(Token *token)
{
    if (token != NULL)
    {
        // if (token->type == TokenString || token->type == TokenNumber ||
        //     token->type == TokenBool || token->type == TokenNULL)
        // {
        //     if (token->literal != NULL)
        //     {
        //         free(token->literal);
        //     }
        // }
        free(token);
    }
}

extern void FreeLexer(Lexer *lexer)
{
    if (lexer != NULL)
    {
        free(lexer);
    }
}

extern void LexerDebugTest(char *input_str)
{
    Lexer *lexer = LexerInit(input_str);

    while (ALWAYS)
    {
        Token *token = Lex(lexer);
        PrintToken(token);
        if (token->type == TokenEOF)
        {
            FreeToken(token);
            break;
        }
        // else if (token->type == TokenIllegal)
        // {
        //     FreeToken(token);
        //     FreeLexer(lexer);
        //     printf("JSON is invalid!\n");
        //     return NULL;
        // }

        FreeToken(token);
    }
    FreeLexer(lexer);
}
