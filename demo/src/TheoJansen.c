#include <stdio.h>
#include "TheoJansen.h"
#include "window_manager.h"
#include "physics_system.h"
#include "physics_debug_system.h"
#include "glbmfont.h"
#include <GL/glfw.h>

int TheoJansen_app (void);
void TheoJansen_init (physics_system * system);
void TheoJansen_update (physics_system * system, int ticks);
void TheoJansen_free (physics_system * system);

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

static cpConstraint *tj_motor;
static cpFloat seg_radius = 3.0f;
cpVect tj_Keyboard = {};

void TheoJansen_foo(void)
{
  int restart = 0;
  do
  {
    restart = TheoJansen_app();
    if(restart == 1)
    {
      printf("- TheoJansen RESTART -\n");
    }
  } while(restart == 1);
}

int TheoJansen_app(void)
{
  const I32 ENTITY_COUNT = 1;

  window_manager        manager;
  physics_system        physics_system;
  physics_debug_system  physics_debug_system;
  char buffer[255];
  double fps;
  int ticks = 0;

  window_manager_init(&manager, "Physics debug", SCREEN_HEIGHT, SCREEN_WIDTH);
  physics_system_init(&physics_system, 0, -500, ENTITY_COUNT);
  physics_debug_system_init(&physics_debug_system, physics_system.space);

  TheoJansen_init(&physics_system); //ici on charge les entités dans le space ss passer par le physics_system
  
  glbmfont_load();

  tj_Keyboard.x = 0.0f;
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();

    if (glfwGetKey( GLFW_KEY_RIGHT ) && glfwGetWindowParam( GLFW_OPENED ))
    {
      tj_Keyboard.x = 1.0f;
    }
	
    if(glfwGetKey( GLFW_KEY_LEFT ) && glfwGetWindowParam( GLFW_OPENED ))
    {
      tj_Keyboard.x = -1.0f;
    } 

    if(glfwGetKey( GLFW_KEY_DOWN ) && glfwGetWindowParam( GLFW_OPENED ))
    {
      tj_Keyboard.x = 0.0f;
    } 

    fps = window_manager_getFps();
    sprintf_s(buffer, "FPS: %f", fps);
    glbmfont_print(buffer, 0, 0, dock_center);

    TheoJansen_update(&physics_system, ticks);

    physics_debug_system_draw(&physics_debug_system);

    window_manager_swapBuffers(&manager);
    ticks += 1;
  }
  
  glbmfont_free();
  TheoJansen_free(&physics_system);
  window_manager_free(&manager);

  return manager.restart;
}

static void make_leg(physics_system * system, cpFloat side, cpFloat offset, cpBody *chassis, cpBody *crank, cpVect anchor)
{
	cpVect a, b;
	cpFloat leg_mass = 1.0f;
	cpFloat diag;
	cpShape *shape;
  cpBody  *lower_leg;
  cpBody  *upper_leg;
	cpConstraint *constraint;

	// make leg
	a = cpvzero, b = cpv(0.0f, side);
	upper_leg = cpSpaceAddBody(system->space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(upper_leg, cpv(offset, 0.0f));
	
	cpSpaceAddShape(system->space, cpSegmentShapeNew(upper_leg, a, b, seg_radius));
	cpSpaceAddConstraint(system->space, cpPivotJointNew2(chassis, upper_leg, cpv(offset, 0.0f), cpvzero));
	
	// lower leg
	a = cpvzero, b = cpv(0.0f, -1.0f*side);
	lower_leg = cpSpaceAddBody(system->space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(lower_leg, cpv(offset, -side));
	
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(lower_leg, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	shape = cpSpaceAddShape(system->space, cpCircleShapeNew(lower_leg, seg_radius*2.0f, b));
	cpShapeSetGroup(shape, 1);
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 1.0f);
	
	cpSpaceAddConstraint(system->space, cpPinJointNew(chassis, lower_leg, cpv(offset, 0.0f), cpvzero));
	
	cpSpaceAddConstraint(system->space, cpGearJointNew(upper_leg, lower_leg, 0.0f, 1.0f));
	
	diag = cpfsqrt(side*side + offset*offset);
	
	constraint = cpSpaceAddConstraint(system->space, cpPinJointNew(crank, upper_leg, anchor, cpv(0.0f, side)));
	cpPinJointSetDist(constraint, diag);
	
	constraint = cpSpaceAddConstraint(system->space, cpPinJointNew(crank, lower_leg, anchor, cpvzero));
	cpPinJointSetDist(constraint, diag);
}

void TheoJansen_init(physics_system * physics_system)
{
  int i;
	int num_legs = 2;
	cpVect a, b;
	cpFloat chassis_mass = 2.0f;
	cpFloat crank_mass = 1.0f;
	cpFloat crank_radius = 13.0f;
	cpFloat offset = 30.0f;
	cpFloat side = 30.0f;
	cpShape *shape;
  cpBody  *chassis;
	cpBody  *crank;
	cpBody  *staticBody = cpSpaceGetStaticBody(physics_system->space);

	cpSpaceSetIterations(physics_system->space, 20);
	
	// Create segments around the edge of the screen.
	shape = cpSpaceAddShape(physics_system->space, cpSegmentShapeNew(staticBody, cpv(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(-SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
  
	shape = cpSpaceAddShape(physics_system->space, cpSegmentShapeNew(staticBody, cpv(SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
  
	shape = cpSpaceAddShape(physics_system->space, cpSegmentShapeNew(staticBody, cpv(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);

	// make chassis
	a = cpv(-offset, 0.0f), b = cpv(offset, 0.0f);
	chassis = cpSpaceAddBody(physics_system->space, cpBodyNew(chassis_mass, cpMomentForSegment(chassis_mass, a, b)));
	
	shape = cpSpaceAddShape(physics_system->space, cpSegmentShapeNew(chassis, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	// make crank
	crank = cpSpaceAddBody(physics_system->space, cpBodyNew(crank_mass, cpMomentForCircle(crank_mass, crank_radius, 0.0f, cpvzero)));
	
	shape = cpSpaceAddShape(physics_system->space, cpCircleShapeNew(crank, crank_radius, cpvzero));
	cpShapeSetGroup(shape, 1);
	
	cpSpaceAddConstraint(physics_system->space, cpPivotJointNew2(chassis, crank, cpvzero, cpvzero));
  
	for(i=0; i<num_legs; i++){
		make_leg(physics_system, side,  offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+0)/(cpFloat)num_legs*M_PI), crank_radius));
		make_leg(physics_system, side, -offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+1)/(cpFloat)num_legs*M_PI), crank_radius));
	}
  
	tj_motor = cpSpaceAddConstraint(physics_system->space, cpSimpleMotorNew(chassis, crank, 6.0f));
}

static void TheoJansen_update(physics_system * system, int ticks)
{
  int i;
	int steps = 3;
	cpFloat coef = (2.0f + tj_Keyboard.y)/3.0f;
	cpFloat rate = tj_Keyboard.x*10.0f*coef;
	cpFloat dt = 1.0f/60.0f/(cpFloat)steps;

	cpSimpleMotorSetRate(tj_motor, rate);
	cpConstraintSetMaxForce(tj_motor, (rate) ? 100000.0f : 0.0f);
	
	for(i=0; i<steps; i++){
		cpSpaceStep(system->space, dt);
	}
}

void TheoJansen_free(physics_system * system)
{
  //free children
	//cpSpaceFree(system->space);
}