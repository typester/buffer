#include "buffer.h"

/*
Copyright (c) 2004, Jan Kneschke, incremental
 All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

- Neither the name of the 'incremental' nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE. */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_PIECE_SIZE      64
#define BUFFER_MAX_REUSE_SIZE  (4 * 1024)

buffer* buffer_init(void) {
    buffer* b;

    b = malloc(sizeof(buffer));
    assert(b);

    b->ptr  = NULL;
    b->used = 0;
    b->size = 0;

    return b;
}

void buffer_free(buffer* b) {
    if (!b) return;

    free(b->ptr);
    free(b);
}

void buffer_reset(buffer* b) {
    if (!b) return;

    if (b->size > BUFFER_MAX_REUSE_SIZE) {
        free(b->ptr);
        b->ptr  = NULL;
        b->size = 0;
    }

    b->used = 0;
}

int buffer_prepare_append(buffer* b, size_t size) {
    if (!b) return -1;
    if (0 == size) return 0;

    /* for '\0' terminate char. its useful for debugging */
    size += 1;

    if (0 == b->size) {
        b->size = size;
        b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);

        b->ptr = malloc(b->size);
        assert(b->ptr);
    }
    else if (b->used + size > b->size) {
        b->size += size;
        b->size += BUFFER_PIECE_SIZE - (b->size % BUFFER_PIECE_SIZE);

        b->ptr = realloc(b->ptr, b->size);
        assert(b->ptr);
    }

    return 0;
}

int buffer_append_string(buffer* b, const char* s) {
    size_t len;

    if (!b || !s) return -1;

    len = strlen(s);
    return buffer_append_string_len(b, s, len);
}

int buffer_append_string_len(buffer* b, const char* s, size_t len) {
    if (!b || !s) return -1;
    if (0 == len) return 0;

    buffer_prepare_append(b, len);

    memcpy(b->ptr + b->used, s, len);
    b->used += len;

    b->ptr[b->used] = '\0';

    return 0;
}

int buffer_spin(buffer* b, size_t len) {
    size_t rest;

    if (!b) return -1;
    if (0 == b->used) return 0;

    rest = b->used - len;
    if (rest > 0) {
        memmove(b->ptr, b->ptr + len, rest);
        b->used -= len;
        b->ptr[b->used] = '\0';
    }
    else {
        buffer_reset(b);
    }

    return 0;
}
