#include <stdio.h>
#include "physics_system.h"

int main( int argc, const char* argv[] )
{
  physics_system physics_system;

  physics_system_init(&physics_system);

  physics_system_load(&physics_system, "les datas formatées du ou des objets physique à charger");
  
  return 0;
}