#include <stdio.h>
#include "physics_system.h"

int main( int argc, const char* argv[] )
{
  int CONSOLE_BREAK;
  
  physics_system physics_system;

  physics_system_init(&physics_system);

  HASHUID elementID = physics_system_load(&physics_system, "les datas formatées du ou des objets physique à charger");

printf("%d", physics_system.idLookupTable[0].uid);

  physics_system_remove(&physics_system, elementID);
  

  scanf_s("%d", &CONSOLE_BREAK);

  return 0;
}