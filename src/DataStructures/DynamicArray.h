#ifndef DynamicArray_H
#define DynamicArray_H

#include <stdlib.h>

typedef struct DynamicArray
{
    int size;
    int capacity;
    void **data;
} DynamicArray;

DynamicArray *array_create(int init_size);
void array_free_with_elements(DynamicArray *arr);
void array_clear(DynamicArray *arr);
void array_add(DynamicArray *arr, void *element);
void *array_get(DynamicArray *arr, int index);
void array_remove_at(DynamicArray *arr, int index);
void array_print_stats(DynamicArray *arr, const char *label);

#endif