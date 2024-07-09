#include "ordered_array.h"
#include "common.h"
#include "kHeap.h"
#include "monitor.h"

int8 StandardLessPredicate(type_t a, type_t b) { return (a < b) ? 1 : 0; }

void PrintArray(ordered_array_t *x){
  monitor_write("\nhole num: ");
  monitor_write_dec(x->size);
  for (uint32 i = 0; i < x->size; i++) {
    monitor_write_dec(((header_t*)x->array[i])->size);
    monitor_write(" ");
  }
}

ordered_array_t CreateOrderedArray(uint32 maxSize,
                                   lessthan_predicate_t lessFunc) {
  ordered_array_t ret;
  ret.array = (void *)kmalloc(maxSize * sizeof(type_t));
  memset(ret.array, 0, maxSize * sizeof(type_t));
  ret.size = 0;
  ret.maxSize = maxSize;
  ret.lessFunc = lessFunc;
  return ret;
}

ordered_array_t PlaceOrderedArray(void *addr, uint32 maxSize,
                                  lessthan_predicate_t lessFunc) {
  ordered_array_t ret;
  ret.array = addr;
  memset(ret.array, 0, maxSize * sizeof(type_t));
  ret.size = 0;
  ret.maxSize = maxSize;
  ret.lessFunc = lessFunc;
  return ret;
}

void DestroyOrderdArray(ordered_array_t *array) { kfree(array->array); }

void InsertOrderedArray(type_t *item, ordered_array_t *array) {
  ASSERT(array->lessFunc);
  uint32 iter = 0;
  while(iter < array->size && array->lessFunc(array->array[iter], item)){
    iter++;
  }

  if (iter == array->size){
    // expand array
    array->array[array->size++] = item;
  }else{
    // move backword
    type_t tmp = array->array[iter];
    array->array[iter] = item;
    while(iter < array->size){
      iter++;
      type_t tmp2 = array->array[iter];
      array->array[iter] = tmp;
      tmp = tmp2;
    }
    array->size++;
  }
}

type_t LookupOrderedArray(uint32 i, ordered_array_t *array) {
  ASSERT(i < array->size);
  return array->array[i];
}

void RemoveOrderedArray(uint32 i, ordered_array_t *array) {
  while (i < array->size) {
    array->array[i] = array->array[i + 1];
    i++;
  }
  array->size--;
}
