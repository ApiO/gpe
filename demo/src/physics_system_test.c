#include <stdio.h>
#include "physics_system_test.h"
#include "physics_system.h"

void physics_system_test()
{
  const I32 ENTITY_COUNT = 3;
  U32 ids[ENTITY_COUNT];
  I32 i;
  I32 currentId = 0;

  physics_system system;
  gpe_physics_segment data;

  physics_system_init(&system, 20, 0, -500, ENTITY_COUNT);
  
  data.elasticity = 1.0f;
  data.friction = 1.0f;
  data.radius = 0.0f;
  
  data.vec_a.x = -320;
  data.vec_a.y = -240;
  data.vec_b.x = -320;
  data.vec_b.y =  240;
  ids[currentId] = physics_system_load(&system, segment_shape, data);
  currentId = ++currentId;

  data.vec_a.x =  320;
  data.vec_a.y = -240;
  data.vec_b.x =  320;
  data.vec_b.y =  240;
  ids[currentId] = physics_system_load(&system, segment_shape, data);
  currentId = ++currentId;
  
  data.vec_a.x = -320;
  data.vec_a.y = -240;
  data.vec_b.x =  320;
  data.vec_b.y = -240;
  ids[currentId] = physics_system_load(&system, segment_shape, data);
  currentId = ++currentId;
  
  for (i = 0; i < ENTITY_COUNT; i++)
  {
    physics_system_remove(&system, ids[i]);
  }
}
