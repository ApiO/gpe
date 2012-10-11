#include <stdio.h>
#include "physics_system_test.h"

int main( int argc, const char* argv[] )
{
  int CONSOLE_BREAK;

  physics_system_test();
  
  scanf_s("%d", &CONSOLE_BREAK);
  return 0;
}