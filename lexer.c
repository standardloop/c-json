#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "./json.h"

static void advanceChar(JSONLexer *);
static void backtrackChar(JSONLexer *);
static void skipWhitespace(JSONLexer *);
static char *makeStringLiteral(JSONLexer *);
static char *makeNumberLiteral(JSONLexer *);
static char *makeNULLLiteral(JSONLexer *);
static char *makeBoolLiteral(JSONLexer *);

static bool isDigitOrMinusSign(char);

static void copyString(char *, char *, size_t, size_t);

static void copyString(char *src, char *des, size_t len, size_t src_offset)
{
    if (src == NULL || des == NULL || len <= 0)
    {
        return;
    }

    char *src_it = src + src_offset;
    size_t size = 0;
    while (size < len)
    {
        *des = *src_it;
        des++;
        src_it++;
        size++;
    }
}

extern bool IsJSONTokenValueType(JSONToken *token, bool check_starters)
{
    if (token->type == JSONTokenString || token->type == JSONTokenNumber || token->type == JSONTokenBool || token->type == JSONTokenNULL)
    {
        return true;
    }
    else if ((token->type == JSONTokenOpenCurlyBrace || token->type == JSONTokenOpenBracket) && check_starters)
    {
        return true;
    }
    return false;
}

extern JSONLexer *JSONLexerInit(char *input)
{
    JSONLexer *lexer = malloc(sizeof(JSONLexer));
    if (lexer == NULL)
    {
        return NULL;
    }
    lexer->input = input;
    lexer->input_len = strlen(input);
    lexer->current_char = NULL_CHAR;
    lexer->position = -2;
    lexer->read_position = -1;
    lexer->line = 1;

    advanceChar(lexer);

    return lexer;
}

static void advanceChar(JSONLexer *lexer)
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

static void backtrackChar(JSONLexer *lexer)
{
    lexer->position -= 2;
    lexer->read_position--;
    lexer->current_char = lexer->input[lexer->read_position];
}

static void skipWhitespace(JSONLexer *lexer)
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

extern JSONToken *NewJSONToken(enum JSONTokenType type, u_int32_t start, u_int32_t end, u_int32_t line_num, char *literal)
{
    JSONToken *token = malloc(sizeof(JSONToken));
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

static bool isDigitOrMinusSign(char test)
{
    if (isdigit(test) || test == DASH_MINUS_CHAR)
    {
        return true;
    }
    return false;
}

extern JSONToken *Lex(JSONLexer *lexer)
{
    JSONToken *token = NULL;
    advanceChar(lexer);
    skipWhitespace(lexer);

    u_int32_t curr_pos = lexer->position;
    if (lexer->current_char == NULL_CHAR)
    {
        token = NewJSONToken(JSONTokenEOF, curr_pos, lexer->position + 1, lexer->line, NULL_CHAR_STRING);
    }
    else if (lexer->current_char == CURLY_OPEN_CHAR)
    {
        token = NewJSONToken(JSONTokenOpenCurlyBrace, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_OPEN_CURLY_BRACE_STRING);
    }
    else if (lexer->current_char == CURLY_CLOSE_CHAR)
    {
        token = NewJSONToken(JSONTokenCloseCurlyBrace, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_CLOSE_CURLY_BRACE_STRING);
    }
    else if (lexer->current_char == BRACKET_OPEN_CHAR)
    {
        token = NewJSONToken(JSONTokenOpenBracket, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_OPEN_BRACKET_STRING);
    }
    else if (lexer->current_char == BRACKET_CLOSE_CHAR)
    {
        token = NewJSONToken(JSONTokenCloseBracket, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_CLOSE_BRACKET_STRING);
    }
    else if (lexer->current_char == COMMA_CHAR)
    {
        token = NewJSONToken(JSONTokenComma, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_COMMA_STRING);
    }
    else if (lexer->current_char == COLON_CHAR)
    {
        token = NewJSONToken(JSONTokenColon, curr_pos, lexer->position + 1, lexer->line, JSON_TOKEN_COLON_STRING);
    }
    else if (lexer->current_char == DOUBLE_QUOTES_CHAR)
    {
        char *string_literal = makeStringLiteral(lexer);
        if (string_literal == NULL)
        {
            token = NewJSONToken(JSONTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewJSONToken(JSONTokenString, curr_pos, lexer->position + 1, lexer->line, string_literal);
        }
    }
    else if (isDigitOrMinusSign(lexer->current_char))
    {
        char *number_literal = makeNumberLiteral(lexer);
        if (number_literal == NULL)
        {
            token = NewJSONToken(JSONTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewJSONToken(JSONTokenNumber, curr_pos, lexer->position + 1, lexer->line, number_literal);
            backtrackChar(lexer);
        }
    }
    else if (lexer->current_char == 't' || lexer->current_char == 'f')
    {
        char *bool_literal = makeBoolLiteral(lexer);
        if (bool_literal == NULL)
        {
            token = NewJSONToken(JSONTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewJSONToken(JSONTokenBool, curr_pos, lexer->position + 1, lexer->line, bool_literal);
        }
    }
    else if (lexer->current_char == 'n')
    {
        char *null_literal = makeNULLLiteral(lexer);
        if (null_literal == NULL)
        {
            token = NewJSONToken(JSONTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
        }
        else
        {
            token = NewJSONToken(JSONTokenNULL, curr_pos, lexer->position + 1, lexer->line, null_literal);
        }
    }
    else
    {
        token = NewJSONToken(JSONTokenIllegal, curr_pos, lexer->position + 1, lexer->line, NULL);
    }

    return token;
}

static char *makeBoolLiteral(JSONLexer *lexer)
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
    copyString(lexer->input, bool_literal, bool_literal_size, start_position);
    bool_literal[bool_literal_size - 1] = NULL_CHAR;

    if (strcmp(bool_literal, JSON_BOOL_TRUE) != 0 && strcmp(bool_literal, JSON_BOOL_FALSE) != 0)
    {
        free(bool_literal);
        return NULL;
    }
    return bool_literal;
}

static char *makeNULLLiteral(JSONLexer *lexer)
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
    copyString(lexer->input, null_literal, null_literal_size, start_position);
    null_literal[null_literal_size - 1] = NULL_CHAR;

    if (strcmp(null_literal, JSON_NULL) != 0)
    {
        return NULL;
    }
    return null_literal;
}

static char *makeNumberLiteral(JSONLexer *lexer)
{
    u_int32_t start_position = lexer->position;
    u_int8_t minus_count = 0;
    if (lexer->current_char == DASH_MINUS_CHAR)
    {
        advanceChar(lexer);
        minus_count++;
        if (!isdigit(lexer->current_char))
        {
            return NULL;
        }
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
            if (decimal_count > 1 || (decimal_count == 1 && science_count >= 1))
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
            if (!isDigitOrMinusSign(lexer->current_char) && lexer->current_char != PLUS_CHAR)
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
    copyString(lexer->input, number_literal, number_literal_size, start_position);
    number_literal[number_literal_size - 1] = NULL_CHAR;
    // printf("number_literal: %s\n", number_literal);
    return number_literal;
}

static char *makeStringLiteral(JSONLexer *lexer)
{
    u_int32_t start_position = lexer->position + 1; // move pass quotes
    char prev_char = lexer->current_char;
    advanceChar(lexer);
    bool is_error = false;
    while (ALWAYS)
    {
        // printf("%c\n", lexer->current_char);
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
                for (u_int8_t i = 0; i < 4; i++)
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
    copyString(lexer->input, string_literal, string_literal_size, start_position);
    string_literal[string_literal_size - 1] = NULL_CHAR;
    // printf("[JOSH]: %s\n", string_literal);
    // printf("[JOSH]: %d\n", (int)strlen(string_literal));
    return string_literal;
}

extern void PrintJSONToken(JSONToken *token, bool print_literal)
{
    if (token == NULL)
    {
        return;
    }
    printf("Line: %u Place: %u - %u ", token->line, token->start, token->end);
    switch (token->type)
    {
    case JSONTokenEOF:
        printf("Kind: JSONTokenEOF");
        break;
    case JSONTokenColon:
        printf("Kind: JSONTokenColon");
        break;
    case JSONTokenOpenCurlyBrace:
        printf("Kind: JSONTokenOpenCurlyBrace");
        break;
    case JSONTokenCloseCurlyBrace:
        printf("Kind: JSONTokenCloseCurlyBrace");
        break;
    case JSONTokenOpenBracket:
        printf("Kind: JSONTokenOpenBracket");
        break;
    case JSONTokenCloseBracket:
        printf("Kind: JSONTokenCloseBracket");
        break;
    case JSONTokenComma:
        printf("Kind: JSONTokenComma");
        break;
    case JSONTokenString:
        printf("Kind: JSONTokenString");
        break;
    case JSONTokenNumber:
        printf("Kind: JSONTokenNumber");
        break;
    case JSONTokenBool:
        printf("Kind: JSONTokenBool");
        break;
    case JSONTokenNULL:
        printf("Kind: JSONTokenNULL");
        break;
    case JSONTokenIllegal:
    default:
        printf("Kind: JSONTokenIllegal");
        break;
    }
    if (print_literal && token->type != JSONTokenIllegal && token->type != JSONTokenEOF)
    {
        if (token->type == JSONTokenString)
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

extern void FreeJSONToken(JSONToken *token)
{
    if (token != NULL)
    {
        // if (token->type == JSONTokenString || token->type == JSONTokenNumber ||
        //     token->type == JSONTokenBool || token->type == JSONTokenNULL)
        // {
        //     if (token->literal != NULL)
        //     {
        //         free(token->literal);
        //     }
        // }
        free(token);
    }
}

extern void FreeJSONLexer(JSONLexer *lexer)
{
    if (lexer != NULL)
    {
        free(lexer);
    }
}

extern void JSONLexerDebugTest(char *input_str, bool exit_after)
{
    JSONLexer *lexer = JSONLexerInit(input_str);

    while (ALWAYS)
    {
        JSONToken *token = Lex(lexer);
        PrintJSONToken(token, true);
        if (token->type == JSONTokenEOF)
        {
            FreeJSONToken(token);
            break;
        }
        // else if (token->type == JSONTokenIllegal)
        // {
        //     FreeJSONToken(token);
        //     FreeJSONLexer(lexer);
        //     printf("JSON is invalid!\n");
        //     return NULL;
        // }

        FreeJSONToken(token);
    }
    FreeJSONLexer(lexer);
    if (exit_after)
    {
        exit(1);
    }
}
