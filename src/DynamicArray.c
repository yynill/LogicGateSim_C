#include "DynamicArray.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

DynamicArray *array_create(int init_capacity, int element_size)
{
    assert(init_capacity > 0);
    DynamicArray *new_array = (DynamicArray *)malloc(sizeof(DynamicArray)); // Allocate memory for struct
    if (new_array == NULL)
    {
        return NULL; // Check for allocation failure
    }

    new_array->size = 0;
    new_array->capacity = init_capacity;
    new_array->element_size = element_size;
    new_array->data = malloc(new_array->capacity * element_size); // Allocate memory for the array of elements

    if (new_array->data == NULL)
    {
        free(new_array); // Clean up struct memory if array allocation fails
        return NULL;
    }
    return new_array;
}

void array_free(DynamicArray *arr)
{
    assert(arr != NULL);
    free(arr->data);
    free(arr);
}

void array_add(DynamicArray *arr, void *element)
{
    assert(arr != NULL);
    assert(element != NULL);

    if (arr->size >= arr->capacity)
    {
        void *new_memory = realloc(arr->data, arr->capacity * 2 * arr->element_size);
        if (new_memory == NULL)
        {
            printf("ERROR - array_add: resize error\n");
            return;
        }
        arr->data = new_memory;
        arr->capacity *= 2;
    }

    void *destination = (char *)arr->data + (arr->size * arr->element_size);
    memcpy(destination, element, arr->element_size);
    arr->size++;
}

void array_set(DynamicArray *arr, int index, void *element)
{
    assert(arr != NULL);
    assert(index < arr->capacity);
    assert(element != NULL);

    if (index > arr->size)
    {
        printf("Error - array_set: index > arr->size\n");
        return;
    }

    void *destination = (char *)arr->data + (index * arr->element_size);
    memcpy(destination, element, arr->element_size);

    if (index == arr->size)
    {
        arr->size++;
    }
}

void *array_get(DynamicArray *arr, int index)
{
    assert(arr != NULL);
    assert(index < arr->size);

    return (void *)((char *)arr->data + index * arr->element_size);
}

DynamicArray *array_copy(DynamicArray *arr)
{
    assert(arr != NULL);
    DynamicArray *new_array = array_create(arr->size, arr->element_size);
    if (new_array == NULL)
    {
        free(new_array); // Clean up struct memory if array allocation fails
        return NULL;
    }

    memcpy(new_array->data, arr->data, arr->size * arr->element_size);

    new_array->size = arr->size;
    new_array->capacity = arr->capacity;

    return new_array;
}

void array_remove_last(DynamicArray *arr)
{
    assert(arr != NULL);
    if (arr->size == 0)
    {
        printf("Error - array_remove_last: array is already empty\n");
        return;
    }

    arr->size--;

    // resize array
    if (arr->size > 0 && arr->size <= arr->capacity / 4)
    {
        void *new_memory = realloc(arr->data, arr->capacity / 2 * arr->element_size);
        if (new_memory == NULL)
        {
            printf("ERROR - array_remove_last: resize error\n");
            return;
        }
        arr->data = new_memory;
        arr->capacity /= 2;
    }
}

void array_remove_at(DynamicArray *arr, int index)
{
    assert(arr != NULL);
    assert(index < arr->size);

    // Calculate how many bytes to move
    int bytes_to_move = (arr->size - index - 1) * arr->element_size;
    if (bytes_to_move > 0)
    {
        void *dest = (char *)arr->data + index * arr->element_size;
        void *src = (char *)arr->data + (index + 1) * arr->element_size;
        memmove(dest, src, bytes_to_move);
    }

    arr->size--;

    // Resize array if too empty
    if (arr->size > 0 && arr->size <= arr->capacity / 4)
    {
        void *new_memory = realloc(arr->data, (arr->capacity / 2) * arr->element_size);
        if (new_memory == NULL)
        {
            printf("ERROR - array_remove_at: resize error\n");
            return;
        }
        arr->data = new_memory;
        arr->capacity /= 2;
    }
}
