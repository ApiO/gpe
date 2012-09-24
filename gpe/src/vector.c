#include "vector.h"

// ----------------------------------------------------------------------------
// This following code about aligned allocation have to be moved elsewhere
// ----------------------------------------------------------------------------

/* Game Developer Magazine
 * December 2008
 * Volume 15, Number 11
 * The Inner Product
 * Data Alignment Part 1
 * By Noel Llopis
 */

//Listing 1. Function to check alignment
I32 is_aligned(void* data, int alignment)
{
   // check that the alignment is a power of two
   assert((alignment & (alignment-1)) == 0);
   return ((uintptr_t)data & (alignment-1)) == 0;
}

// Listing 2. Platform-independent macro to align data.
#if defined(VISUAL_STUDIO)
   #define DATA_ALIGN(declaration, alignment) __declspec(align(alignment)) declaration
#elif defined(GCC)
    #define DATA_ALIGN(declaration, alignment) declaration __attribute__ ((aligned (alignment)))
#else
   #define DATA_ALIGN(declaration, alignment)
#endif

// Listing 3. alligned_malloc and alligned_free
void* aligned_malloc(int size, int alignment)
{
   const int pointerSize = sizeof(void*);
   const int requestedSize = size + alignment - 1 + pointerSize;
   void* raw = malloc(requestedSize);
   void* start = (char*)raw + pointerSize;
   void* aligned = (void*)(((unsigned int)((char*)start+alignment-1)) & ~(alignment-1));
   *(void**)((char*)aligned-pointerSize) = raw;
   return aligned;
}

void aligned_free(void* aligned)
{
   void* raw = *(void**)((char*)aligned-sizeof(void*));
   free(raw);
}

// ----------------------------------------------------------------------------

/*
 * These first methods are about managing the vector values and keep them packed in the memory.
 * For most details, see http://bitsquid.blogspot.fr/2011/09/managing-decoupling-part-4-id-lookup.html
 */

void vec2s_init(vec2s *vec, U16 size)
{
   int i;
   vec->size = size;

   // The real size must be a multiple of 4 since we're gonna use
   // SIMD operations, so let's round it to the next multiple of 4.
   size = (size + 3) & ~0x03;
   assert(size % 4 == 0);

   vec->x = (F32*)aligned_malloc(size * sizeof(F32), 16);
   vec->y = (F32*)aligned_malloc(size * sizeof(F32), 16);

   vec->ids = (U32*)calloc(size, sizeof(U32));
   vec->indices = (vec_index*)calloc(size, sizeof(vec_index));

   vec->num_vecs = 0;

   for(i = 0; i < size; i++)
   {
      vec->x[i] = 0.0f;
      vec->y[i] = 0.0f;
      vec->indices[i].id   = i;
      vec->indices[i].next = i + 1;
   }

   vec->freelist_dequeue = 0;
   vec->freelist_enqueue = size - 1;
}

vec2_id vec2s_create(vec2s *vec, F32 x, F32 y)
{
   vec_index *in = &vec->indices[vec->freelist_dequeue];
   vec->freelist_dequeue = in->next;
   in->id += VEC_NEW_ID_ADD;
   in->index = vec->num_vecs++;
   vec->x[in->index] = x;
   vec->y[in->index] = y;
   return in->id;
}

void vec2s_set(vec2s *vec, vec2_id id, F32 x, F32 y)
{
   U16 index = vec->indices[id & VEC_INDEX_MASK].index;

   vec->x[index] = x;
   vec->y[index] = y;
}

void vec2s_remove(vec2s *vec, vec2_id id)
{
   vec_index *in = &vec->indices[id & VEC_INDEX_MASK];
   U16 new_num_vec = vec->num_vecs - 1;

   vec->x[vec->num_vecs] = vec->x[new_num_vec];
   vec->y[vec->num_vecs] = vec->y[new_num_vec];

   vec->indices[vec->ids[new_num_vec] & VEC_INDEX_MASK].index = in->index;

   vec->num_vecs = new_num_vec;
   in->index = USHRT_MAX;
   vec->indices[vec->freelist_enqueue].next = id & VEC_INDEX_MASK;
   vec->freelist_enqueue = id & VEC_INDEX_MASK;
}

void vec2s_free(vec2s *vec)
{
   aligned_free(vec->x);
   aligned_free(vec->y);

   free(vec->ids);
   free(vec->indices);
}

/*
 * SIMD operations
 */

void vec2s_add(vec2s *result, vec2s *vec0, vec2s *vec1)
{
   int i;

   for (i = 0; i < vec0->size; i += 4)
   {
      // load values from the floating-point arrays
      __m128 vf0_x = _mm_load_ps(&vec0->x[i]);
      __m128 vf0_y = _mm_load_ps(&vec0->y[i]);

      __m128 vf1_x = _mm_load_ps(&vec1->x[i]);
      __m128 vf1_y = _mm_load_ps(&vec1->y[i]);

      // add the vector components
      __m128 vfr_x = _mm_add_ps(vf0_x, vf1_x);
      __m128 vfr_y = _mm_add_ps(vf0_y, vf1_y);

      // store results
      _mm_store_ps(&result->x[i], vfr_x);
      _mm_store_ps(&result->x[i], vfr_y);
   }
}