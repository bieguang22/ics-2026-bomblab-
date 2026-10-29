#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);
    uint32_t low = (uint32_t)challenge;
    uint32_t step1 = UINT32_C(3) * (high - middle) + UINT32_C(0x7439);
    uint32_t step0 = (high + UINT32_C(0xdc16)) ^ low;

    values[0] = UINT32_C(2) * step0 + (step1 ^ UINT32_C(0x27cf));
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    values[0] = (uint32_t)((challenge >> 46) & UINT64_C(0x3f));
    for (uint32_t index = 1; index < count; ++index) {
        uint32_t part = (uint32_t)((challenge >> (index + 23)) & UINT64_C(0xf));
        values[index] = UINT32_C(3) * ((part + index) ^ values[index - 1]);
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[2];
    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    values[0] = (uint32_t)challenge & 7U;
    switch (values[0]) {
        case 0: values[1] = (4U * x) ^ 0x2d8dU; break;
        case 1: values[1] = (x ^ 0x90d2U) + (x >> 1); break;
        case 2: values[1] = 5U * x + 0x4c54aU; break;
        case 3: values[1] = 3U * x + 0xe0d3U; break;
        case 4: values[1] = 5U * x + 0xbf09U; break;
        case 5: values[1] = (x ^ 0xad1cU) + (x >> 1); break;
        case 6: values[1] = 3U * x + 0x7808U; break;
        default: values[1] = (4U * x) ^ 0xaa22U; break;
    }
    return write_answer(out, cap, values, 2);
}

static uint32_t phase4_walk(uint32_t target, uint32_t lo, uint32_t hi, uint32_t key) {
    uint32_t mid = lo + ((hi - lo) >> 1);
    if (target == mid) return (mid + 0x56U) ^ key;
    if (target < mid)
        return (3U * phase4_walk(target, lo, mid - 1U, key)) ^ (mid + key) ^ 0x1bU;
    return (3U * phase4_walk(target, mid + 1U, hi, key)) ^ (mid + key) ^ 6U;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t value = phase4_walk((uint32_t)challenge & 127U, 0, 127, key);
    return write_answer(out, cap, &value, 1);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    /* Each row is {next node index, value, tag}, recovered from p5_nodes. */
    static const uint32_t nodes[12][3] = {
        {0, 0xb1d, 0xa101}, {7, 0xa70, 0xbece},
        {6, 0x658, 0x70ce}, {8, 0x058, 0x77ff},
        {10, 0x120, 0x718e}, {2, 0x21a, 0x2e4a},
        {11, 0x768, 0x06d0}, {5, 0x86a, 0x4cfa},
        {0, 0x930, 0x78e5}, {4, 0x43b, 0x5ad7},
        {3, 0x53c, 0x8933}, {9, 0x32c, 0x2149}
    };
    static const uint32_t heads[8] = {1, 7, 5, 2, 6, 11, 9, 4};
    uint32_t node = heads[challenge & 7U];
    uint32_t steps = (uint32_t)(challenge >> 3) & 3U;
    uint32_t values[2] = {
        (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32) ^ 0x8a88U, 0
    };
    for (uint32_t i = 0; i <= steps; ++i) {
        values[0] = (values[0] ^ nodes[node][2]) + nodes[node][1];
        values[1] = nodes[node][1];
        node = nodes[node][0];
    }
    return write_answer(out, cap, values, 2);
}

static uint32_t phase6_op(uint32_t slot, uint32_t x) {
    /* This switch follows the pointer table, not the function name suffix. */
    switch (slot) {
        case 0: return (x + 0xd9a7U) ^ (x >> 3);
        case 1: return (x ^ (x >> 2)) + 0xa553U;
        case 2:
        case 5: return (4U * x + 0x2c06U) ^ (7U * x);
        case 3:
        case 7: return (x ^ 0xf304U) + 8U * x;
        case 4: return ((x << 5) | (x >> 27)) ^ 0xd157U;
        default: return (x >> 5) | (x << 27);
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    x = phase6_op((uint32_t)challenge & 7U, x) ^ 0x60e4U;
    x = phase6_op((uint32_t)(challenge >> 3) & 7U, x) + 0xe780U;
    return write_answer(out, cap, &x, 1);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t selector = (uint32_t)context & 3U;
    uint32_t folded = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t history = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t x = folded ^ history;
    uint32_t first = ((x >> 1) | (x << 31)) ^ 0x7b38U
                   ^ (selector * 0x7a47U) ^ (uint32_t)(context >> 16);
    uint32_t second = first ^ (history + selector + 0x17fU + (folded >> 1));
    /* secret_gate checks OPEN at byte 32 and parses numbers from byte 36. */
    int size = snprintf(out, cap, "%032uOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                        0U, selector, first, second);
    return size < 0 || (size_t)size >= cap;
}
