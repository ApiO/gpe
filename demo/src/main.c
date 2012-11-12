#include <stdio.h>
//#include "glfw_check.h"
//#include "window_test.h"
//#include "physics_system_test.h"
#include "TheoJansen.h"

int main( int argc, const char* argv[] )
{
  int CONSOLE_BREAK;

  //glfw_check_foo();
  //window_test_foo();
  //physics_system_test_foo0();
  TheoJansen_foo();

  printf("- APP END -\n");
  scanf_s("%d", &CONSOLE_BREAK);
  return 0;
}