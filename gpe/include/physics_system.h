#ifndef physics_system_H_
#define physics_system_H_

#include "gpr_containers.h"
#include <chipmunk\chipmunk.h>

typedef struct
{
  cpBody  *body;
  cpShape *shapes;
  int      shapes_count;
} gpe_physics_entity;

GPR_IDLUT_INIT(gpe_physics_entity) 

typedef struct
{
  cpSpace                         *space;
  gpr_idlut_t(gpe_physics_entity)  idlut;  //table de correspondance entre HASID et index de l'élément dans la table dynamique physics
  U32                              physics_count;  //nombre d'entitées physique dans la table physics
  U32                             *toUpdate;       //liste des id d'élément à updater pour le tick courrant
  U32                              capacity;
} physics_system;

void  physics_system_init (physics_system * system, int object_count);
U32   physics_system_load (physics_system * system, char * data);
void  physics_system_remove (physics_system * system, U32 id);
void  physics_system_submitUpdate (physics_system * system, U32 id, char * data);
void  physics_system_update (physics_system * system, float dt);
void  physics_system_free (physics_system * system, U32 id);

#endif // physics_system_H_