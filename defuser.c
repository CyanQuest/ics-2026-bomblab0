#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    if (count == 0 || cap == 0) return 1;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return 0;
}

static uint32_t rotate_left_one(uint32_t value) {
    return (value << 1) | (value >> 31);
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t middle = (uint32_t)(challenge >> 16);
    uint32_t first = (5U * low + high) ^ 0xf16fU;
    uint32_t second = 4U * high + (middle ^ 0x6ef3U);
    uint32_t value = ((second + 2U * low) ^ 0x5062U) ^ first;
    return write_answer(out, cap, &value, 1);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[6];
    values[0] = 2U * (uint32_t)((challenge >> 47) & 63U) + 15U;
    for (size_t i = 1; i < 6; ++i) {
        uint32_t digit = (uint32_t)(challenge >> (16U + 4U * i)) & 15U;
        values[i] = 2U * values[i - 1] + (digit ^ (uint32_t)i);
    }
    return write_answer(out, cap, values, 6);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[2];
    uint32_t value = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    values[0] = (uint32_t)challenge & 7U;
    switch (values[0]) {
        case 0: values[1] = (value ^ 0xf3a7U) + (value >> 2); break;
        case 1: values[1] = 5U * value + 0x1aaaeU; break;
        case 2: values[1] = 3U * value + 0xbaf8U; break;
        case 3: values[1] = (value ^ 0xa20cU) + (value >> 1); break;
        case 4: values[1] = (4U * value) ^ 0x5d92U; break;
        case 5: values[1] = 3U * value + 0xbe7cU; break;
        case 6: values[1] = 5U * value + 0x4de45U; break;
        default: values[1] = (4U * value) ^ 0x580U; break;
    }
    return write_answer(out, cap, values, 2);
}

static uint32_t walk(uint32_t target, uint32_t low, uint32_t high, uint32_t salt) {
    uint32_t middle = low + ((high - low) >> 1);
    if (target == middle) return (middle + 0x62U) ^ salt;
    if (target < middle) {
        return (3U * walk(target, low, middle - 1U, salt)) ^ (middle + salt) ^ 0x1aU;
    }
    return (3U * walk(target, middle + 1U, high, salt)) ^ (middle + salt) ^ 0x0bU;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t target = (uint32_t)challenge & 127U;
    uint32_t salt = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t value = walk(target, 0, 127, salt);
    return write_answer(out, cap, &value, 1);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    static const struct {
        uint32_t value;
        uint32_t salt;
        unsigned next;
    } nodes[12] = {
        {0x070fU, 0x4311U, 10},
        {0x094dU, 0xbb2eU, 9},
        {0x042fU, 0x492fU, 5},
        {0x0645U, 0xf9c0U, 8},
        {0x0369U, 0x33eeU, 1},
        {0x0067U, 0x00eaU, 11},
        {0x0b78U, 0x8485U, 4},
        {0x011aU, 0x0384U, 0},
        {0x023aU, 0x366dU, 2},
        {0x0876U, 0xe2c0U, 7},
        {0x0a12U, 0x425aU, 3},
        {0x0555U, 0xf4d1U, 12}
    };
    static const unsigned heads[8] = {6, 4, 1, 9, 7, 0, 10, 3};
    unsigned current = heads[challenge & 7U];
    unsigned steps = (unsigned)(challenge >> 3) & 3U;
    uint32_t values[2];
    values[0] = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32) ^ 0xc3cbU;
    for (unsigned i = 0; i <= steps; ++i) {
        values[0] = (values[0] ^ nodes[current].salt) + nodes[current].value;
        values[1] = nodes[current].value;
        current = nodes[current].next;
    }
    return write_answer(out, cap, values, 2);
}

static uint32_t operation_0(uint32_t value) {
    return (value + 0x9610U) ^ (value >> 7);
}

static uint32_t operation_1(uint32_t value) {
    return (value ^ 0x3fccU) + (value << 5);
}

static uint32_t operation_2(uint32_t value) {
    return (value >> 5) | (value << 27);
}

static uint32_t operation_3(uint32_t value) {
    return ((value << 5) + 0x5876U) ^ (7U * value);
}

static uint32_t operation_4(uint32_t value) {
    return rotate_left_one(value) ^ 0x37eaU;
}

static uint32_t operation_5(uint32_t value) {
    return (value ^ (value >> 3)) + 0xa1e7U;
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    static uint32_t (*const operations[8])(uint32_t) = {
        operation_0, operation_1, operation_5, operation_2,
        operation_1, operation_3, operation_4, operation_5
    };
    uint32_t value = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    value = operations[challenge & 7U](value) ^ 0x9223U;
    value = operations[(challenge >> 3) & 7U](value) + 0xc1d7U;
    return write_answer(out, cap, &value, 1);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t first = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t second = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t values[3];
    values[0] = (uint32_t)context & 3U;
    values[1] = ((rotate_left_one(first ^ second) + 0x7db3U) ^ 0x51b5U)
                ^ ((values[0] * 0x7db3U) ^ (uint32_t)(context >> 16));
    values[2] = values[1] ^ (second + values[0] + 0x51b5U + (first >> 1));
    int size = snprintf(out, cap, "%032uOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                        0U, values[0], values[1], values[2]);
    return size < 0 || (size_t)size >= cap;
}
