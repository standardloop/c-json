# Dynamic Library (macOS)

## Disclaimer

Header file name and subject to change

## setup
```sh
$ make release
```

## Example Program

### Source

```c
#include <json.h>
int main(void)
{
    JSON *test = StringToJSON("[]");
    PrintJSON(test);
    FreeJSON(test);
    return 0;
}
```

### Setup and Run

```sh
$ export DYLD_LIBRARY_PATH="/usr/local/lib"
$ gcc main.c -lstandardloop-json
$ ./a.out
```
