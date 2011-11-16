#include <stdio.h>

#include "buffer.h"

static void tests(int num) {
    printf("1..%d\n", num);
}

static void ok(int ok, const char* msg) {
    static int testnum = 0;
    printf("%s %d - %s\n", ok ? "ok" : "not ok", ++testnum, msg);
}

int main(void) {
    buffer* b;
    int i;

    tests(1);

    b = buffer_init();
    ok(NULL == b->ptr, "ptr is NULL by default");
    ok(0 == b->used, "used 0");
    ok(0 == b->size, "size 0");

    buffer_append_string(b, "Hello");
    ok(NULL != b->ptr, "ptr is allocated");
    ok(5 == b->used, "used ok");
    ok(64 == b->size, "size ok");
    ok('\0' == b->ptr[b->used], "null terminated ok");
    ok(0 == strcmp(b->ptr, "Hello"), "data ok");

    buffer_append_string(b, " ");
    buffer_append_string(b, "World");

    ok(0 == strcmp(b->ptr, "Hello World"), "data ok 2");
    ok(11 == b->used, "used ok");

    buffer_spin(b, 6);
    ok(0 == strcmp(b->ptr, "World"), "spin ok");
    ok(5 == b->used, "used ok");

    buffer_spin(b, 5);
    ok(NULL != b->ptr, "reset buf reuse mem ok");
    ok(0 == b->used, "used ok");

    for (i = 0; i < 1024; ++i) {
        buffer_append_string(b, "Hello");
    }

    ok(b->used == 5 * 1024, "used ok");

    buffer_reset(b);
    ok(0 == b->used, "used ok");
    ok(0 == b->size, "size ok");
    ok(NULL == b->ptr, "reset mem when data > BUFFER_MAX_REUSE_SIZE ok");

    return 0;
}
