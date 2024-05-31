#ifndef JSON_H
#define JSON_H

#define BRACKET_OPEN_CHAR '['
#define BRACKET_CLOSE_CHAR ']'

typedef struct JSON
{
    struct JSON *next;
    struct JSON *prev;

    char *name;
    void *value;
} JSON;

JSON *StringToJSON(char *);
char *JSONToString(JSON *);

void FreeJSON(JSON *);
void PrintJSON(JSON *);

#endif
