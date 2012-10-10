#include <stdlib.h>
#include "physics_system.h"

void physics_system_init (physics_system * system, int size)
{
  system->capacity = size;
  gpr_idlut_init(gp_physics_entity, system->idLookupTable, size);
}

HASHUID physics_system_load (physics_system * system, char * data)
{
  //travail sur gp_physics_t cf @ data

  return 42;//id_lookup_t_add(system->idLookupTable);
}

void physics_system_remove (physics_system * system, HASHUID uid)
{
  //if (id_lookup_t_has(system->idLookupTable, uid))
  //{
  //  //free(system->physics);
  //  //realloc : physics
  //  //free(system->idLookupTable[i]);
  //  //realloc : idLookupTable
  //    
  //  //cp free system->physics|i].body
  //  //cp free system->physics|i].shapes
  //  id_lookup_t_remove(system->idLookupTable, uid);
  //  system->physics_count = --system->physics_count;
  //  return;
}

void  physics_system_submitUpdate (physics_system * system, HASHUID uid, char * data)
{
}

void  physics_system_update (physics_system * system, float dt)
{
}

void  physics_system_free (physics_system * system, HASHUID uid)
{
}