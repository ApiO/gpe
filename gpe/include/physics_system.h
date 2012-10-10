#ifndef physics_system_H_
#define physics_system_H_

#include "gpr_containers.h"
#include <chipmunk\chipmunk.h>


typedef int HASHUID; //ID lookup table value

typedef struct
{
  cpBody  *body;
  cpShape *shapes;
  int      shapes_count;
} gp_physics_entity;

GPR_IDLUT_INIT(gp_physics_entity) 

typedef struct
{
  cpSpace      *space;
  gpr_idlut_t(gp_physics_entity)  *idLookupTable;  //table de correspondance entre HASID et index de l'élément dans la table dynamique physics
  U32                              physics_count;  //nombre d'entitées physique dans la table physics
  HASHUID                         *toUpdate;       //liste des id d'élément à updater pour le tick courrant
  U32                              capacity;
} physics_system;

void    physics_system_init (physics_system * system);
HASHUID physics_system_load (physics_system * system, char * data);
void    physics_system_remove (physics_system * system, HASHUID uid);
void    physics_system_submitUpdate (physics_system * system, HASHUID uid, char * data);
void    physics_system_update (physics_system * system, float dt);
void    physics_system_free (physics_system * system, HASHUID uid);

#endif // physics_system_H_