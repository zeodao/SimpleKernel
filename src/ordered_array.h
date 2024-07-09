#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "common.h"

typedef void *type_t;

typedef int8 (*lessthan_predicate_t)(type_t, type_t);

typedef struct {
  type_t *array;
  uint32 size;
  uint32 maxSize;
  lessthan_predicate_t lessFunc;
} ordered_array_t;


void PrintArray(ordered_array_t* x);

/**
 * @Synopsis sort func for array (less first)
 *
 * @Param a
 * @Param b
 *
 * @Returns    
 */
int8 StandardLessPredicate(type_t a, type_t b);


/**
 * @Synopsis create hole size array 
 *
 * @Param maxSize hole Size
 * @Param lessFunc using to sort array info
 *
 * @Returns   
 */
ordered_array_t CreateOrderedArray(uint32 maxSize, lessthan_predicate_t lessFunc);


/**
 * @Synopsis inplace version of create hole size array
 *
 * @Param addr place addr
 * @Param maxSize hole Size
 * @Param lessFunc using to sort array info
 *
 * @Returns   
 */
ordered_array_t PlaceOrderedArray(void* addr, uint32 maxSize, lessthan_predicate_t lessFunc);


/**
 * @Synopsis delete Ordered Array 
 *
 * @Param array
 */
void DestroyOrderdArray(ordered_array_t * array);


/**
 * @Synopsis insert a hole/block in to array
 *
 * @Param item inserted item 
 * @Param array array to be insert
 */
void InsertOrderedArray(type_t* item, ordered_array_t *array);


/**
 * @Synopsis get item from array by index 
 *
 * @Param i index 
 * @Param array ordered array
 *
 * @Returns   
 */
type_t LookupOrderedArray(uint32 i, ordered_array_t *array);


/**
 * @Synopsis RemoveOrderedArray 
 *
 * @Param i
 * @Param array
 */
void RemoveOrderedArray(uint32 i, ordered_array_t *array);

#endif // ORDERED_ARRAY_H
