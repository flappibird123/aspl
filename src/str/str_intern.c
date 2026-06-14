#include "str/str_intern.h"

#include <string.h>
#include <stdlib.h>

struct InternNode {
    const char *str;
    struct InternNode *next;
};

#define TABLE_SIZE 4096

struct InternNode *table[TABLE_SIZE];

static char *str_copy(const char *s) {
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    memcpy(p, s, len + 1);
    return p;
}

static unsigned long hash_str(const char *s) {
    unsigned long h = 1469598103934665603ULL;

    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 1099511628211ULL;
    }

    return h;
}

const char *str_intern(const char *s) {
    unsigned long h = hash_str(s);
    unsigned long idx = h % TABLE_SIZE;

    struct InternNode *node = table[idx];

    // 1. check if already exists
    while (node) {
        if (strcmp(node->str, s) == 0) {
            return node->str;
        }
        node = node->next;
    }

    // 2. not found → create new entry
    struct InternNode *new_node = malloc(sizeof(struct InternNode));
    new_node->str = str_copy(s);
    new_node->next = table[idx];

    table[idx] = new_node;

    return new_node->str;
}
