#include <stddef.h>
#include <stdlib.h>

#define STACK_DEFINE(Type)                                          \
    typedef struct {                                                \
        Type *data;                                                 \
        size_t size;                                                \
        size_t capacity;                                            \
    } Stack_##Type;                                                 \
                                                                    \
    void Stack_##Type##_init(Stack_##Type *stack, size_t capacity) { \
        stack->data = (Type *)malloc(capacity * sizeof(Type));      \
        stack->size = 0;                                            \
        stack->capacity = capacity;                                 \
    }                                                               \
                                                                    \
    void Stack_##Type##_free(Stack_##Type *stack) {                 \
        free(stack->data);                                          \
        stack->data = NULL;                                         \
        stack->size = 0;                                            \
        stack->capacity = 0;                                        \
    }                                                               \
                                                                    \
    int Stack_##Type##_push(Stack_##Type *stack, Type value) {      \
        if (stack->size >= stack->capacity) return 0;               \
        stack->data[stack->size++] = value;                         \
        return 1;                                                   \
    }                                                               \
                                                                    \
    int Stack_##Type##_pop(Stack_##Type *stack, Type *out) {        \
        if (stack->size == 0) return 0;                             \
        *out = stack->data[--stack->size];                          \
        return 1;                                                   \
    }                                                               \
                                                                    \
    int Stack_##Type##_is_empty(Stack_##Type *stack) {              \
        return stack->size == 0;                                    \
    }                                                               \
                                                                    \
    Type Stack_##Type##_top(Stack_##Type *stack) {                  \
        return stack->data[stack->size - 1];                        \
    }
