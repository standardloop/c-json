#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ALWAYS 1

#define SPACE_CHAR ' '
#define TAB_CHAR '\t'
#define NEWLINE_CHAR '\n'
#define CARRIAGE_CHAR '\r'
#define DASH_MINUS_CHAR '-'
#define PLUS_CHAR '+'

#define DOT_CHAR '.'
#define COMMA_CHAR ','
#define COLON_CHAR ':'

#define DOUBLE_QUOTES_CHAR '\"'

#define BACKSLASH_CHAR '\\'
#define FORWARDLASH_CHAR '/'

#define BRACKET_OPEN_CHAR '['
#define BRACKET_CLOSE_CHAR ']'

#define CURLY_OPEN_CHAR '{'
#define CURLY_CLOSE_CHAR '}'

#define NULL_CHAR '\0'

#define pass (void)0

extern void CopyString(char *, char *, size_t, size_t);
extern void CopyStringCanary(char *, char *, u_int64_t);

extern char *PutQuotesAroundString(char *, bool);
extern bool IsCharInString(const char *, char);
extern size_t NumCharInString(const char *, char);
extern char *QuickAllocatedString(char *);
extern inline void PrintSpaces(int);
extern bool IsDigitOrMinusSign(char);

extern int32_t NumberOfDigitsInInt64(int64_t);
extern char *Int64ToString(int64_t);
extern char *DoubleToString(double);

extern void PrintStringDebug(char *, size_t);

#endif
