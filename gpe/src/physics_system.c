#include <stdlib.h>
#include "physics_system.h"

void physics_system_init (physics_system * system, int object_count)
{
  system->capacity = object_count;
  gpr_idlut_init(gpe_physics_entity, &system->idlut, object_count);

  cpVect gravity = cpv(0, -100);
  cpSpace *space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  system->space = space;
}

U32 physics_system_load (physics_system * system, char * data)
{
  //pars de data, si ce n'est pas déjà une data intermediaire, et création de body et shape(s)
  cpFloat radius = 5;
  cpFloat mass = 1;
  cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  cpBody *ballBody = cpSpaceAddBody(system->space, cpBodyNew(mass, moment));
  cpBodySetPos(ballBody, cpv(0, 15));
  cpShape *ballShape = cpSpaceAddShape(system->space, cpCircleShapeNew(ballBody, radius, cpvzero));
  cpShapeSetFriction(ballShape, 0.7);
  //--------------------------------------------------
  
  gpe_physics_entity physics = { ballBody, ballShape, 1 };
  
  return gpr_gpe_physics_entity_idlut_add(&system->idlut, &physics);
}

void physics_system_remove (physics_system * system, U32 id)
{
  gpr_gpe_physics_entity_idlut_remove(&system->idlut, id);
}

void  physics_system_submitUpdate (physics_system * system, U32 id, char * data)
{
}

void  physics_system_update (physics_system * system, float dt)
{
}

void  physics_system_free (physics_system * system, U32 id)
{
}