#include <stdio.h>
#include "physics_system.h"

int main( int argc, const char* argv[] )
{
  int CONSOLE_BREAK, i;

  physics_system physics_system;

  physics_system_init(&physics_system);

  HASHUID elementID = physics_system_load(&physics_system, "les datas formatées du ou des objets physique à charger");
  
  physics_system_load(&physics_system, "les datas formatées d'un objet");

  for (i = 0; i < physics_system.physics_count; i++)
  {
    printf("idLookupTable[%d] = { uid=%d, index=%d }\n", i, physics_system.idLookupTable[i].uid, physics_system.idLookupTable[i].index);
  }

  physics_system_remove(&physics_system, elementID);
  

  scanf_s("%d", &CONSOLE_BREAK);

  return 0;
}