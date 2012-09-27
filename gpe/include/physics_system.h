#ifndef PHYSICS_SYSTEM_H_
#define PHYSICS_SYSTEM_H_

#include <chipmunk\chipmunk.h>

#define HASHID int //ID lookup table value

typedef struct
{
  int     id;
  cpBody  body;
  cpShape shapes[];
} gpPhysic;


typedef struct
{
  cpSpace   *space;
  //gpPhysic  *physics;  //liste des entité physiques
  HASHID    toUpdate[]; //liste des id d'élément à updater pour le tick courrant
} physics_system;


void    physics_system_init (physics_system * system);

HASHID  physics_system_load (physics_system * system, char * data);

HASHID  gpPhysic_init (physics_system * system, cpBody * body, cpShape * shapes);

void    physics_add(physics_system * system, gpPhysic item);

//void  remove (id)
//void  submitUpdate (id,data)
//void  update (dt)
//void  reset (void)
//void  free (void)

#endif // PHYSICS_SYSTEM_H_