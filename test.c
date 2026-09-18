#include <_string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static void testJSONV2()
{
    JSONValueV2 *json_v = JSONValueV2Init(JSONSTRING_t, strdup("testing"));
    assert(json_v != NULL);
    assert(json_v->value_type == JSONSTRING_t);
    assert(json_v->str != NULL);
    assert(strcmp(json_v->str, "testing") == 0);

    JSONValueV2Free(json_v);
}

int main(void)
{
    printf("starting test....\n");
    testJSONV2();
    printf("done!\n");
    return EXIT_SUCCESS;
    TestJSON();
}
