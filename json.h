#ifndef JSON_H
#define JSON_H

#define BRACKET_OPEN_CHAR '['
#define BRACKET_CLOSE_CHAR ']'

#define CURLY_OPEN_CHAR '{'
#define CURLY_CLOSE_CHAR '}'

#define NULL_CHAR '\0'

typedef struct JSON
{
    struct JSON *next;
    struct JSON *prev;

    char *name;
    void *value;
} JSON;

JSON *StringToJSON(const char *);
char *JSONToString(JSON *);

void FreeJSON(JSON *);
void PrintJSON(JSON *);

#endif
