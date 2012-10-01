#include "physics_system.h"
#include <stdlib.h>
  
void physics_system_init (physics_system * system)
{
  system->physics_count = 0;

  cpVect gravity = cpv(0, -100);
  cpSpace *space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);

  system->space = space;
}

HASHUID physics_system_load (physics_system * system, char * data)
{
  cpSpace *space = system->space;

  //pars de data, si ce n'est pas d�j� une data intermediaire, et cr�ation de body et shape(s)
  cpFloat radius = 5;
  cpFloat mass = 1;
  cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
  cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
  cpBodySetPos(ballBody, cpv(0, 15));
  cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
  cpShapeSetFriction(ballShape, 0.7);
  //--------------------------------------------------
  
  gpPhysic physic = { ballBody, ballShape, 1 };

  return physics_add(system, physic);
}

HASHUID physics_add(physics_system * system, gpPhysic physic)
{
  int count = system->physics_count;
  gpIdLookup idLookup = { generate_hashuid(), count };

  if (count == 0)
  {
    system->physics = (gpPhysic *)malloc (sizeof(gpPhysic));
    system->idLookupTable = (gpIdLookup *)malloc (sizeof(gpIdLookup));
  }
  else
  {
    system->physics = (gpPhysic *)realloc (system->physics, (count + 1) * sizeof(gpPhysic) );
    system->idLookupTable = (gpIdLookup *)realloc (system->idLookupTable, (count + 1) * sizeof(gpIdLookup) );
  }

  if( system->physics == NULL || system->idLookupTable == NULL)
  {
    exit(EXIT_FAILURE);
  }
  
  system->physics[count] = physic;
  system->idLookupTable[count] = idLookup;
  system->physics_count = (count + 1);

  return idLookup.uid;
}

HASHUID generate_hashuid (void)
{
  return 42;
}

void physics_system_remove (physics_system * system, HASHUID uid)
{
}

void  physics_system_submitUpdate (physics_system * system, HASHUID uid, char * data)
{
}

void  physics_system_update(physics_system * system, float dt)
{
}

void  physics_system_free (physics_system * system, HASHUID uid)
{
}