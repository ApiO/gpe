#include <stdio.h>
#include "physics_system.h"

int main( int argc, const char* argv[] )
{
  physics_system physics_system;

  physics_system_init(&physics_system);

  physics_system_load(&physics_system, "les datas format�es du ou des objets physique � charger");
  
  return 0;
}