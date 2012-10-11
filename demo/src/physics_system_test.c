#include <stdio.h>
#include "physics_system_test.h"
#include "physics_system.h"

void physics_system_test()
{
  const int LOOP_COUNT = 50;
  int i;

  physics_system system;

  physics_system_init(&system, 1);

  U32 elementID = physics_system_load(&system, "les datas formatées du ou des objets physique à charger");

  physics_system_remove(&system, elementID);


  //-------------

  /*
  physics_system_init(&physics_system, LOOP_COUNT);


  for (i = 1; i < LOOP_COUNT; i++)
  {
    physics_system_load(&physics_system, "les datas formatées d'un objet");
  }

  physics_system_remove(&physics_system, LOOP_COUNT);

  for (i = 0; i < physics_system.physics_count; i++)
  {
    //printf("idLookupTable[%d] = { uid=%d, index=%d }\n", i, physics_system.idLookupTable[i].id, physics_system.idLookupTable[i].index);
    physics_system_remove(&physics_system, elementID);
  }
  */
}
