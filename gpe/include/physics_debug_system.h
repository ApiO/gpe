#ifndef physics_debug_system_h
#define physics_debug_system_h

#include <chipmunk\chipmunk.h>

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

typedef struct physics_debug_system {
	cpSpace * space;
} physics_debug_system;

void  physics_debug_system_init (physics_debug_system * system, cpSpace * space);
void  physics_debug_system_draw (physics_debug_system * system);

#endif //physics_debug_system_h