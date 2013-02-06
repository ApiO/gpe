#include <stdlib.h>
#include "physics_system.h"
#include "gpr_memory.h"

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

void physics_system_init (physics_system *system, int gravity_x, int gravity_y, int object_count)
{
  cpSpace *space;
  system->capacity = object_count;
  gpr_idlut_init(gpe_physics, &system->table, gpr_default_allocator);

  space = cpSpaceNew();
  system->space = space;
  cpSpaceSetGravity(space, cpv(gravity_x, gravity_y));
}

//gpe_physics_entity load_segment_shape(physics_system * system, char * data)
void load_segment_shape(gpe_physics *physics, physics_system * system, gpe_physics_segment data)
{
  //gpe_physics_segment seg_shape = (gpe_physics_segment) data + sizeof(gpe_physics_segment);
  
	cpBody *staticBody = cpBodyNewStatic();
	cpShape *shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(data.vec_a.x, data.vec_a.y), cpv(data.vec_b.x, data.vec_b.y), data.radius));
  cpShapeSetElasticity(shape, data.elasticity);
  cpShapeSetFriction(shape, data.friction);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);

  physics->body = staticBody;
  physics->shapes = shape;
  physics->shapes_count = 1;
}

//U32 physics_system_load (physics_system * system, gpe_physics_type type, char * data)
U64 physics_system_load (physics_system * system, gpe_physics_type type, gpe_physics_segment data)
{
  gpe_physics physics;

  switch (type)
  {
    case segment_shape:      
      load_segment_shape(&physics, system, data);
      break;
    default:
      break;
  }

  return gpr_idlut_add(gpe_physics, &system->table, &physics);
}

void physics_system_remove (physics_system * system, U32 id)
{
  gpe_physics *physics = gpr_idlut_lookup(gpe_physics, &system->table, id);
  cpShapeFree(physics->shapes);
  cpBodyFree(physics->body);
  gpr_idlut_remove(gpe_physics, &system->table, id);
}

void  physics_system_submitUpdate (physics_system * system, U32 id, char * data)
{
}

void  physics_system_update (physics_system * system, float dt)
{
}

void  physics_system_free (physics_system * system)
{
  gpr_idlut_destroy(gpe_physics, &system->table);
}

//DEV :: ajoute un shape directement au space : ne passe pas par idlut. doit être utilisé pour dev/test sur physics_debug
void  physics_system_loadFoo0 (physics_system * system)
{
  cpShape *ground;
  cpShape *ballShape;
  cpFloat radius = 5;
  cpFloat mass = 1;
  cpFloat moment;
  cpBody *ballBody;

  // Add a static line segment shape for the ground.
  // We'll make it slightly tilted so the ball will roll off.
  // We attach it to space->staticBody to tell Chipmunk it shouldn't be movable.
  ground = cpSegmentShapeNew(system->space->staticBody, cpv(-20, 5), cpv(20, -5), 0);
  cpShapeSetFriction(ground, 1);
  cpSpaceAddShape(system->space, ground);
  
  // Now let's make a ball that falls onto the line and rolls off.
  // First we need to make a cpBody to hold the physical properties of the object.
  // These include the mass, position, velocity, angle, etc. of the object.
  // Then we attach collision shapes to the cpBody to give it a size and shape.
    
  // The moment of inertia is like mass for rotation
  // Use the cpMomentFor*() functions to help you approximate it.
  moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  
  // The cpSpaceAdd*() functions return the thing that you are adding.
  // It's convenient to create and add an object in one line.
  ballBody = cpSpaceAddBody(system->space, cpBodyNew(mass, moment));
  cpBodySetPos(ballBody, cpv(0, 15));
  
  // Now we create the collision shape for the ball.
  // You can create multiple collision shapes that point to the same body.
  // They will all be attached to the body and move around to follow it.
  ballShape = cpSpaceAddShape(system->space, cpCircleShapeNew(ballBody, radius, cpvzero));
  cpShapeSetFriction(ballShape, 0.7);
}

void  physics_system_loadFoo1 (physics_system * system)
{
  cpFloat mass = 1.0f;
  cpFloat inertia = 1.0f;
	cpFloat radius = 10.0f;

  cpBody *body = cpSpaceAddBody(system->space, cpBodyNew(mass, inertia));
	cpShape *circle = cpSpaceAddShape(system->space,  cpCircleShapeNew(body, radius, cpvzero));
  
  cpFloat width = 5.0f;
  cpFloat height = 5.0f;
  cpShape *box = cpSpaceAddShape(system->space, cpBoxShapeNew(body, width, height));
}