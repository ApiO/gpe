#include <stdio.h>
#include "physics_system_test.h"
#include "window_manager.h"
#include "physics_system.h"
#include "physics_debug_system.h"

void physics_system_test_foo0(void)
{
  const I32 ENTITY_COUNT = 1;

  window_manager manager;
  physics_system system;
  physics_debug_system debug;
  
  gpr_memory_init(4*1024*1024);

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -100, ENTITY_COUNT);
  physics_debug_system_init(&debug, system.space);

  physics_system_loadFoo0(&system);
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear(&manager);
    physics_debug_system_draw(800, 600, &debug);
    window_manager_swapBuffers(&manager);
  }
  
  physics_system_free (&system);
	cpSpaceFree(system.space);
  gpr_memory_shutdown();
  window_manager_free(&manager);
}

void physics_system_test_foo1(void)
{
  const I32 ENTITY_COUNT = 3;
  U64 ids[ENTITY_COUNT];
  I64 currentId = 0;

  window_manager manager;
  physics_system system;
  physics_debug_system debug;
  gpe_physics_segment data;

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -500, ENTITY_COUNT);
  physics_debug_system_init(&debug, system.space);
  
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
    window_manager_clear(&manager);
    physics_debug_system_draw(800, 600, &debug);
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);
  
  physics_system_free (&system);
	cpSpaceFree(system.space);
  gpr_memory_shutdown();
}

void physics_system_test_foo2(void)
{
  const I32 ENTITY_COUNT = 1;

  window_manager manager;
  physics_system system;
  physics_debug_system debug;

  window_manager_init(&manager, "Physics debug", 600, 800);
  physics_system_init(&system, 0, -100, ENTITY_COUNT);
  physics_debug_system_init(&debug, system.space);

  physics_system_loadFoo1(&system);
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear(&manager);
    physics_debug_system_draw(800, 600, &debug);
    window_manager_swapBuffers(&manager);
  }
  
  window_manager_free(&manager);
  physics_system_free (&system);
	cpSpaceFree(system.space);
  gpr_memory_shutdown();
}