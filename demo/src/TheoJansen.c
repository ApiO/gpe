#include <stdio.h>
#include "TheoJansen.h"
#include "window_manager.h"
#include "physics_system.h"
#include "physics_debug.h"

int TheoJansen_app(void);
void TheoJansen_init(physics_system * system);
void TheoJansen_update(physics_system * system, int ticks);
void TheoJansen_free(physics_system * system);

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH = 640;

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

static cpConstraint *tj_motor;
//static cpFloat seg_radius = 3.0f;
static cpFloat seg_radius = 0.3f;
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

  window_manager manager;
  physics_system system;
  physics_debug debug;
  int ticks = 0;

  window_manager_init(&manager, "Physics debug", SCREEN_HEIGHT, SCREEN_WIDTH);
  physics_system_init(&system, 0, -500, ENTITY_COUNT);
  physics_debug_init(&debug, system.space);

  TheoJansen_init(&system); //ici on charge les entit�s dans le space ss passer par le physics_system
  
  //pseudo game loop
  while (manager.running)
  {
    window_manager_clear();

    if (glfwGetKey( GLFW_KEY_RIGHT ) && glfwGetWindowParam( GLFW_OPENED ))
    {
      tj_Keyboard.x = 10;
    }

    if(glfwGetKey( GLFW_KEY_LEFT ) && glfwGetWindowParam( GLFW_OPENED ))
    {
      tj_Keyboard.x = -10;
    } 

    TheoJansen_update(&system, ticks);

    physics_debug_draw(&debug);

    window_manager_swapBuffers(&manager);
    ticks += 1;
  }
  
  TheoJansen_free(&system);
  window_manager_free(&manager);

  return manager.restart;
}

static void make_leg(physics_system * system, cpFloat side, cpFloat offset, cpBody *chassis, cpBody *crank, cpVect anchor)
{
	cpVect a, b;
	cpShape *shape;
	
	cpFloat leg_mass = 1.0f;

	// make leg
	a = cpvzero, b = cpv(0.0f, side);
	cpBody *upper_leg = cpSpaceAddBody(system->space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(upper_leg, cpv(offset, 0.0f));
	
	cpSpaceAddShape(system->space, cpSegmentShapeNew(upper_leg, a, b, seg_radius));
	cpSpaceAddConstraint(system->space, cpPivotJointNew2(chassis, upper_leg, cpv(offset, 0.0f), cpvzero));
	
	// lower leg
	a = cpvzero, b = cpv(0.0f, -1.0f*side);
	cpBody *lower_leg = cpSpaceAddBody(system->space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(lower_leg, cpv(offset, -side));
	
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(lower_leg, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	shape = cpSpaceAddShape(system->space, cpCircleShapeNew(lower_leg, seg_radius*2.0f, b));
	cpShapeSetGroup(shape, 1);
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 1.0f);
	
	cpSpaceAddConstraint(system->space, cpPinJointNew(chassis, lower_leg, cpv(offset, 0.0f), cpvzero));
	
	cpSpaceAddConstraint(system->space, cpGearJointNew(upper_leg, lower_leg, 0.0f, 1.0f));
	
	cpConstraint *constraint;
	cpFloat diag = cpfsqrt(side*side + offset*offset);
	
	constraint = cpSpaceAddConstraint(system->space, cpPinJointNew(crank, upper_leg, anchor, cpv(0.0f, side)));
	cpPinJointSetDist(constraint, diag);
	
	constraint = cpSpaceAddConstraint(system->space, cpPinJointNew(crank, lower_leg, anchor, cpvzero));
	cpPinJointSetDist(constraint, diag);
}

void TheoJansen_init(physics_system * system)
{
	cpSpaceSetIterations(system->space, 20);

	cpBody *staticBody = cpSpaceGetStaticBody(system->space);
	cpShape *shape;
	cpVect a, b;
	
	// Create segments around the edge of the screen.
	//shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(-SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 0.0f));
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(-1,-1), cpv(-1,1), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
  
	//shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 0.0f));
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(1,-1), cpv(1,1), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
  
	//shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), cpv(SCREEN_WIDTH/2, -SCREEN_HEIGHT/2), 0.0f));
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(-1,-1), cpv(1,-1), 0.0f));
	//shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(-0.5,-0.5), cpv(0.5,-0.5), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
	
	//cpFloat offset = 30.0f;
	cpFloat offset = 0.003f;

	// make chassis
	cpFloat chassis_mass = 2.0f;
	a = cpv(-offset, 0.0f), b = cpv(offset, 0.0f);
	cpBody *chassis = cpSpaceAddBody(system->space, cpBodyNew(chassis_mass, cpMomentForSegment(chassis_mass, a, b)));
	
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(chassis, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	// make crank
	cpFloat crank_mass = 1.0f;
	//cpFloat crank_radius = 13.0f;
	cpFloat crank_radius = 0.13f;
	cpBody *crank = cpSpaceAddBody(system->space, cpBodyNew(crank_mass, cpMomentForCircle(crank_mass, crank_radius, 0.0f, cpvzero)));
	
	shape = cpSpaceAddShape(system->space, cpCircleShapeNew(crank, crank_radius, cpvzero));
	cpShapeSetGroup(shape, 1);
	
	cpSpaceAddConstraint(system->space, cpPivotJointNew2(chassis, crank, cpvzero, cpvzero));
  
	//cpFloat side = 30.0f;
	cpFloat side = 0.003f;
	
	int num_legs = 2;
	for(int i=0; i<num_legs; i++){
		make_leg(system, side,  offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+0)/(cpFloat)num_legs*M_PI), crank_radius));
		make_leg(system, side, -offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+1)/(cpFloat)num_legs*M_PI), crank_radius));
	}
  
	tj_motor = cpSpaceAddConstraint(system->space, cpSimpleMotorNew(chassis, crank, 6.0f));
}

static void TheoJansen_update(physics_system * system, int ticks)
{
	cpFloat coef = (2.0f + tj_Keyboard.y)/3.0f;
	cpFloat rate = tj_Keyboard.x*10.0f*coef;
	cpSimpleMotorSetRate(tj_motor, rate);
	cpConstraintSetMaxForce(tj_motor, (rate) ? 100000.0f : 0.0f);
	
	int steps = 3;
	cpFloat dt = 1.0f/60.0f/(cpFloat)steps;
	
	for(int i=0; i<steps; i++){
		cpSpaceStep(system->space, dt);
	}
}

void TheoJansen_free(physics_system * system)
{
  //free children
	//cpSpaceFree(system->space);
}