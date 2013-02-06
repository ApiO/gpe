#ifndef physics_debug_system_h
#define physics_debug_system_h

#include <chipmunk\chipmunk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

typedef struct 
{
	cpSpace * space;
} physics_debug_system;

void  physics_debug_system_init (physics_debug_system * system, cpSpace * space);
void  physics_debug_system_draw (physics_debug_system * system);

#ifdef __cplusplus
}
#endif

#endif //physics_debug_system_h