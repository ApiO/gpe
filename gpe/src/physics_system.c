#include "physics_system.h"
#include <stdlib.h>
  
void physics_system_init (physics_system * system)
{
  system->physics_count = 0;

  // cpVect is a 2D vector and cpv() is a shortcut for initializing them.
  cpVect gravity = cpv(0, -100);
  
  // Create an empty space.
  cpSpace *space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  system->space = space;
}

HASHID physics_system_load (physics_system * system, char * data)
{
  cpSpace *space = system->space;

  cpFloat radius = 5;
  cpFloat mass = 1;
  
  // The moment of inertia is like mass for rotation
  // Use the cpMomentFor*() functions to help you approximate it.
  cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  
  // The cpSpaceAdd*() functions return the thing that you are adding.
  // It's convenient to create and add an object in one line.
  cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPos(ballBody, cpv(0, 15));
  
  // Now we create the collision shape for the ball.
  // You can create multiple collision shapes that point to the same body.
  // They will all be attached to the body and move around to follow it.
  cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
  cpShapeSetFriction(ballShape, 0.7);
  
  return gpPhysic_init(system, ballBody, ballShape);
}

HASHID gpPhysic_init (physics_system * system, cpBody * body, cpShape * shapes)
{
  //@system pas forcement utile, sauf si pour aider à determiner le HASHID de retour

  gpPhysic item = {0x0001, *body, shapes, 1};

  physics_add(system, item);

  return 0x0001;
}

void gpPhysic_add_shape(gpPhysic * physic, cpShape shape)
{
  //ralloc du tableau de shape
  //ajout du nouvel élément
  physic->shapes_count++;
}

void physics_add(physics_system * system, gpPhysic *item)
{
  gpPhysic * physics = system->physics;
  int count = system->physics_count;

  if (count == 0)
  {
    physics = (gpPhysic *)malloc (sizeof(gpPhysic));
  }
  else
  {
    physics = (gpPhysic *)realloc (physics, (count + 1) * sizeof(gpPhysic) );
  }

  if( physics == NULL )
  {
    exit(EXIT_FAILURE);
  }

  physics[count] = *item;

  system->physics_count = count++;
}