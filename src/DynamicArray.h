#ifndef DynamicArray_H
#define DynamicArray_H

#include <stdlib.h>

typedef struct DynamicArray
{
    int size;
    int capacity;
    int element_size;
    void *data;
} DynamicArray;

DynamicArray *array_create(int init_size, int element_size);
void array_free(DynamicArray *arr);
void array_clear(DynamicArray *arr);
void array_add(DynamicArray *arr, void *element);
void *array_add_uninitialized(DynamicArray *arr);
void array_set(DynamicArray *arr, int index, void *element);
void *array_get(DynamicArray *arr, int index);
DynamicArray *array_copy(DynamicArray *arr);
void array_remove_last(DynamicArray *arr);
void array_remove_at(DynamicArray *arr, int index);

#endif