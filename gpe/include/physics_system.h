#ifndef PHYSICS_SYSTEM_H_
#define PHYSICS_SYSTEM_H_

#include <chipmunk\chipmunk.h>

typedef int HASHUID; //ID lookup table value

typedef struct
{
  HASHUID uid;
  int     index;
} gpIdLookup;

typedef struct
{
  cpBody  *body;
  cpShape *shapes;
  int     shapes_count;
} gpPhysic;

typedef struct
{
  cpSpace     *space;
  gpPhysic    *physics;       //liste des entité physiques
  gpIdLookup  *idLookupTable; //table de correspondance entre HASID et index de l'élément dans la table dynamique physics
  int         physics_count;  //nombre d'entitées physique dans la table physics
  HASHUID      *toUpdate;      //liste des id d'élément à updater pour le tick courrant
} physics_system;

void    physics_system_init (physics_system * system);

HASHUID physics_system_load (physics_system * system, char * data);
HASHUID physics_add(physics_system * system, gpPhysic physic);
HASHUID generate_hashuid (void);

void    physics_system_remove (physics_system * system, HASHUID uid);

void    physics_system_submitUpdate (physics_system * system, HASHUID uid, char * data);

void    physics_system_update(physics_system * system, float dt);

void    physics_system_free (physics_system * system, HASHUID uid);

#endif // PHYSICS_SYSTEM_H_