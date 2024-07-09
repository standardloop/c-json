# Dynamic Library (macOS)

## Disclaimer

Header file name and subject to change

## Setup
```sh
$ make release
```

## Example Program

### Source

```C
#include <standardloop/json.h>
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
$ export DYLD_LIBRARY_PATH="/usr/local/lib/standardloop"
$ gcc main.c -L/usr/local/lib/standardloop -lstandardloop-json
$ ./a.out
[]%
```

## Note:
If using just `/usr/local/lib` instead of `/usr/local/lib/standardloop` then the addition arg, `-L`, is not needed.
