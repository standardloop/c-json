# c-json

JSON parsing library created from scratch for learning purposes

Planning to use this as a library in https://github.com/standardloop/cerver


## Example Program

```C
#include <stdio.h>
#include <stdlib.h>
#include "./json.h" // or <standardloop/json.h> if using dynamic library

int main(void)
{
    JSON *json = StringToJSON("[1, 2, {\"key\": \"value\"}, \"hello\"]");
    if (json == NULL)
    {
        return EXIT_FAILURE;
    }
    PrintJSON(json);
    FreeJSON(json);
    return EXIT_SUCCESS;
}
```
