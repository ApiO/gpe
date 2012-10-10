/*
 * 2D vector using the SoA approach (Struct of Array)
 * to favorise memory access & SIMD computing
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <assert.h>
#include <xmmintrin.h>
#include "short_types.h"

#define VEC_INDEX_MASK 0xFFFF
#define VEC_NEW_ID_ADD 0x10000

// external identifier for vector values
typedef U32 vec2_id;

// internal index used to manage the packed array of values
typedef struct
{
   U32   id;
   U16   index;
   U16   next;
} vec_index;

// 2D SoA vector struct
typedef struct
{
   F32 *x;
   F32 *y;

   U32       *ids;
   vec_index *indices;

   U16 size;
   U16 num_vecs;
   U16 freelist_enqueue;
   U16 freelist_dequeue;
} vec2s;

// must be called after vec2s vec;
void    vec2s_init   (vec2s *vec, U16 size);

// release the memory allocated for the vector
void    vec2s_free   (vec2s *vec);

// create, set or remove a vector value
vec2_id vec2s_create (vec2s *vec, F32 x, F32 y);
void    vec2s_set    (vec2s *vec, vec2_id id, F32 x, F32 y);
void    vec2s_remove (vec2s *vec, vec2_id id);

// vector operations
void    vec2s_add   (vec2s *result, vec2s *vec0, vec2s *vec1);
void    vec2s_sub   (vec2s *result, vec2s *vec0, vec2s *vec1);
void    vec2s_neg   (vec2s *result, vec2s *vec);
void    vec2s_mult  (vec2s *result, vec2s *vec0, vec2s *vec1);
void    vec2s_dot   (F32   *result, vec2s *vec0, vec2s *vec1);
void    vec2s_cross (F32   *result, vec2s *vec0, vec2s *vec1);
void    vec2s_perp  (vec2s *result, vec2s *vec);
void    vec2s_proj  (F32   *result, vec2s *vec0, vec2s *vec1);


#endif // VECTOR_H_