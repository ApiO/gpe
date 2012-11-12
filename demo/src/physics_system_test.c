#include <stdio.h>
#include "physics_system_test.h"
#include "window_manager.h"
#include "physics_system.h"
#include "physics_debug.h"

void physics_system_test_foo0(void)
{
  const I32 ENTITY_COUNT = 1;

  window_manager manager;
  physics_system system;
  physics_debug debug;

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -100, ENTITY_COUNT);
  physics_debug_init(&debug, system.space);

  physics_system_loadFoo0(&system);
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();
    physics_debug_draw(&debug);
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);
}

void physics_system_test_foo1(void)
{
  const I32 ENTITY_COUNT = 3;
  U32 ids[ENTITY_COUNT];
  I32 i;
  I32 currentId = 0;

  window_manager manager;
  physics_system system;
  physics_debug debug;
  gpe_physics_segment data;

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -500, ENTITY_COUNT);
  physics_debug_init(&debug, system.space);
  
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
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();
    physics_debug_draw(&debug);
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);

  for (i = 0; i < ENTITY_COUNT; i++)
  {
    physics_system_remove(&system, ids[i]);
  }
}

void physics_system_test_foo2(void)
{
  const I32 ENTITY_COUNT = 1;

  window_manager manager;
  physics_system system;
  physics_debug debug;

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -100, ENTITY_COUNT);
  physics_debug_init(&debug, system.space);

  physics_system_loadFoo1(&system);
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();
    physics_debug_draw(&debug);
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);
}