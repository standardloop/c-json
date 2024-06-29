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
static char *makeStringLiteral(Lexer *);
static char *makeNumberLiteral(Lexer *);
static char *makeNULLLiteral(Lexer *);
static char *makeBoolLiteral(Lexer *);

extern bool IsTokenValueType(Token *token, bool check_starters)
{
    if (token->type == TokenString || token->type == TokenNumber || token->type == TokenBool || token->type == TokenNULL)
    {
        return true;
    }
    else if ((token->type == TokenOpenCurlyBrace || token->type == TokenOpenBracket) && check_starters)
    {
        return true;
    }
    return false;
}

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
        char *string_literal = makeStringLiteral(lexer);
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
    u_int8_t minus_count = 0;
    if (lexer->current_char == DASH_MINUS_CHAR)
    {
        advanceChar(lexer);
        minus_count++;
    }

    u_int8_t decimal_count = 0;
    u_int8_t science_count = 0;
    u_int8_t plus_count = 0;
    bool error = false;
    while (ALWAYS)
    {
        if (lexer->current_char == DOT_CHAR)
        {
            decimal_count++;
            if (decimal_count > 1)
            {
                error = true;
            }
            advanceChar(lexer);
            if (!isdigit(lexer->current_char))
            {
                error = true;
            }
        }
        else if (lexer->current_char == 'e' || lexer->current_char == 'E')
        {
            science_count++;
            if (science_count > 1)
            {
                error = true;
            }
            advanceChar(lexer);
            if (!IsDigitOrMinusSign(lexer->current_char) && lexer->current_char != PLUS_CHAR)
            {
                error = true;
            }
        }
        else if (lexer->current_char == DASH_MINUS_CHAR)
        {
            minus_count++;
            if (minus_count > 2)
            {
                error = true;
            }
            advanceChar(lexer);
            if (!isdigit(lexer->current_char))
            {
                error = true;
            }
        }
        else if (lexer->current_char == PLUS_CHAR)
        {
            plus_count++;
            if (plus_count > 1)
            {
                error = true;
            }
            advanceChar(lexer);
            if (!isdigit(lexer->current_char))
            {
                error = true;
            }
        }
        else if (lexer->current_char == NULL_CHAR || !isdigit(lexer->current_char))
        {
            break;
        }
        advanceChar(lexer);
    }
    if (error)
    {
        // printf("number literal error");
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
    // printf("number_literal\n", number_literal);
    return number_literal;
}

static char *makeStringLiteral(Lexer *lexer)
{
    u_int32_t start_position = lexer->position + 1;
    char prev_char = lexer->current_char;
    advanceChar(lexer);
    bool is_error = false;
    while (ALWAYS)
    {
        // printf("%d\n", lexer->current_char);
        if (lexer->current_char == BACKSLASH_CHAR)
        {
            prev_char = lexer->current_char;
            advanceChar(lexer);
            if (lexer->current_char == DOUBLE_QUOTES_CHAR)
            {
                continue;
            }
            else if (lexer->current_char == 'n')
            {
                continue;
            }
            else if (lexer->current_char == BACKSLASH_CHAR)
            {
                continue;
            }
            else if (lexer->current_char == FORWARDLASH_CHAR)
            {
                continue;
            }
            else if (lexer->current_char == 'f')
            {
                continue;
            }
            else if (lexer->current_char == 'r')
            {
                continue;
            }
            else if (lexer->current_char == 't')
            {
                continue;
            }
            else if (lexer->current_char == 'u')
            {
                prev_char = lexer->current_char;
                advanceChar(lexer);
                for (u_int8_t i = 0; i < 3; i++)
                {
                    if (!isxdigit(lexer->current_char))
                    {
                        is_error = true;
                    }
                    prev_char = lexer->current_char;
                    advanceChar(lexer);
                }
            }
            else
            {
                is_error = true;
            }
        }
        if ((lexer->current_char == DOUBLE_QUOTES_CHAR && prev_char != BACKSLASH_CHAR) || lexer->current_char == NULL_CHAR)
        {
            break;
        }
        prev_char = lexer->current_char;
        advanceChar(lexer);
    }

    if (is_error)
    {
        return NULL;
    }

    u_int32_t string_literal_size = (lexer->position - start_position) + 1;
    char *string_literal = malloc(sizeof(char) * string_literal_size);
    if (string_literal == NULL)
    {
        return NULL;
    }
    CopyString(lexer->input, string_literal, string_literal_size, start_position);
    string_literal[string_literal_size - 1] = NULL_CHAR;
    // printf("[JOSH]\n", string_literal);
    return string_literal;
}

extern void PrintToken(Token *token, bool print_literal)
{
    if (token == NULL)
    {
        return;
    }
    printf("Line: %u Place: %u - %u ", token->line, token->start, token->end);
    switch (token->type)
    {
    case TokenEOF:
        printf("Kind: TokenEOF");
        break;
    case TokenColon:
        printf("Kind: TokenColon");
        break;
    case TokenOpenCurlyBrace:
        printf("Kind: TokenOpenCurlyBrace");
        break;
    case TokenCloseCurlyBrace:
        printf("Kind: TokenCloseCurlyBrace");
        break;
    case TokenOpenBracket:
        printf("Kind: TokenOpenBracket");
        break;
    case TokenCloseBracket:
        printf("Kind: TokenCloseBracket");
        break;
    case TokenComma:
        printf("Kind: TokenComma");
        break;
    case TokenString:
        printf("Kind: TokenString");
        break;
    case TokenNumber:
        printf("Kind: TokenNumber");
        break;
    case TokenBool:
        printf("Kind: TokenBool");
        break;
    case TokenNULL:
        printf("Kind: TokenNULL");
        break;
    case TokenIllegal:
    default:
        printf("Kind: TokenIllegal\n");
        break;
    }
    if (print_literal && token->type != TokenIllegal)
    {
        if (token->type == TokenString)
        {
            printf("Literal: \"%s\"\n", token->literal);
        }
        else
        {
            printf("Literal: %s\n", token->literal);
        }
    }
    else
    {
        printf("\n");
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

extern void LexerDebugTest(char *input_str, bool exit_after)
{
    Lexer *lexer = LexerInit(input_str);

    while (ALWAYS)
    {
        Token *token = Lex(lexer);
        PrintToken(token, true);
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
    if (exit_after)
    {
        exit(1);
    }
}
