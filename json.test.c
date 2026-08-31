#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./json.h"

static void testJSONInit()
{
    JSON *test = JSONInit();
    assert(test != NULL);
    assert(test->root == NULL);
    FreeJSON(test);
}

static void testStringToJSON() {}

extern void TestJSON()
{
    testJSONInit();
    testStringToJSON();
}
