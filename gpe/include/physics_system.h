#ifndef physics_system_H_
#define physics_system_H_

#include <chipmunk\chipmunk.h>
#include "gpr_idlut.h"

typedef enum
{
  segment_shape
} gpe_physics_type;

typedef struct
{
  int x;
  int y;
} gpe_pair;

typedef struct
{
  gpe_pair vec_a;
  gpe_pair vec_b;
  F32      radius;
  F32      elasticity;
  F32      friction;
} gpe_physics_segment;

typedef struct
{
  cpBody  *body;
  cpShape *shapes;
  int      shapes_count;
} gpe_physics;
GPR_IDLUT_INIT(gpe_physics)

typedef struct
{
  cpSpace *space;
  gpr_idlut_t(gpe_physics)  
           table;
  U32      physics_count;   //nombre d'entitées physique dans la table physics
  U32     *toUpdate;        //liste des id d'élément à updater pour le tick courrant
  U32      capacity;
} physics_system;

void  physics_system_init (physics_system * system, int gravity_x, int gravity_y, int object_count);
U32   physics_system_load (physics_system * system, gpe_physics_type type, gpe_physics_segment data);
//U32   physics_system_load (physics_system * system, gp_physics_type type, char * data);
void  physics_system_remove (physics_system * system, U32 id);
void  physics_system_submitUpdate (physics_system * system, U32 id, char * data);
void  physics_system_update (physics_system * system, float dt);
void  physics_system_free (physics_system * system);

void  physics_system_loadFoo0 (physics_system * system);
void  physics_system_loadFoo1 (physics_system * system);

#endif // physics_system_H_