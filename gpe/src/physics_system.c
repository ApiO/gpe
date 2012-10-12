#include <stdlib.h>
#include "physics_system.h"

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)
static cpConstraint *motor;
static cpFloat seg_radius = 3.0f;

static void make_leg(cpSpace *space, cpFloat side, cpFloat offset, cpBody *chassis, cpBody *crank, cpVect anchor)
{
	cpVect a, b;
	cpShape *shape;
	
	cpFloat leg_mass = 1.0f;

	// make leg
	a = cpvzero, b = cpv(0.0f, side);
	cpBody *upper_leg = cpSpaceAddBody(space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(upper_leg, cpv(offset, 0.0f));
	
	cpSpaceAddShape(space, cpSegmentShapeNew(upper_leg, a, b, seg_radius));
	cpSpaceAddConstraint(space, cpPivotJointNew2(chassis, upper_leg, cpv(offset, 0.0f), cpvzero));
	
	// lower leg
	a = cpvzero, b = cpv(0.0f, -1.0f*side);
	cpBody *lower_leg = cpSpaceAddBody(space, cpBodyNew(leg_mass, cpMomentForSegment(leg_mass, a, b)));
	cpBodySetPos(lower_leg, cpv(offset, -side));
	
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(lower_leg, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	shape = cpSpaceAddShape(space, cpCircleShapeNew(lower_leg, seg_radius*2.0f, b));
	cpShapeSetGroup(shape, 1);
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 1.0f);
	
	cpSpaceAddConstraint(space, cpPinJointNew(chassis, lower_leg, cpv(offset, 0.0f), cpvzero));
	
	cpSpaceAddConstraint(space, cpGearJointNew(upper_leg, lower_leg, 0.0f, 1.0f));
	
	cpConstraint *constraint;
	cpFloat diag = cpfsqrt(side*side + offset*offset);
	
	constraint = cpSpaceAddConstraint(space, cpPinJointNew(crank, upper_leg, anchor, cpv(0.0f, side)));
	cpPinJointSetDist(constraint, diag);
	
	constraint = cpSpaceAddConstraint(space, cpPinJointNew(crank, lower_leg, anchor, cpvzero));
	cpPinJointSetDist(constraint, diag);
}

void physics_system_init (physics_system * system, U16 iterations, int gravity_x, int gravity_y, int object_count)
{
  system->capacity = object_count;
  gpr_idlut_init(gpe_physics_entity, &system->idlut, object_count);

  cpSpace *space = cpSpaceNew();
  system->space = space;
	cpSpaceSetIterations(space, iterations);
  cpSpaceSetGravity(space, cpv(gravity_x, gravity_y));
  
	//cpBody *staticBody = cpSpaceGetStaticBody(space);
	cpShape *shape;
	cpVect a, b;
	
	// Create segments around the edge of the screen.
  /*
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(-320,240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(320,-240), cpv(320,240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(-320,-240), cpv(320,-240), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 1.0f);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);
  */

	cpFloat offset = 30.0f;

	// make chassis
	cpFloat chassis_mass = 2.0f;
	a = cpv(-offset, 0.0f), b = cpv(offset, 0.0f);
	cpBody *chassis = cpSpaceAddBody(space, cpBodyNew(chassis_mass, cpMomentForSegment(chassis_mass, a, b)));
	
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(chassis, a, b, seg_radius));
	cpShapeSetGroup(shape, 1);
	
	// make crank
	cpFloat crank_mass = 1.0f;
	cpFloat crank_radius = 13.0f;
	cpBody *crank = cpSpaceAddBody(space, cpBodyNew(crank_mass, cpMomentForCircle(crank_mass, crank_radius, 0.0f, cpvzero)));
	
	shape = cpSpaceAddShape(space, cpCircleShapeNew(crank, crank_radius, cpvzero));
	cpShapeSetGroup(shape, 1);
	
	cpSpaceAddConstraint(space, cpPivotJointNew2(chassis, crank, cpvzero, cpvzero));
	
	cpFloat side = 30.0f;
	
	int num_legs = 2;
	for(int i=0; i<num_legs; i++){
    make_leg(system->space, side,  offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+0)/(cpFloat)num_legs*M_PI), crank_radius));
		make_leg(system->space, side, -offset, chassis, crank, cpvmult(cpvforangle((cpFloat)(2*i+1)/(cpFloat)num_legs*M_PI), crank_radius));
	}
	
	motor = cpSpaceAddConstraint(space, cpSimpleMotorNew(chassis, crank, 6.0f));
}

//gpe_physics_entity load_segment_shape(physics_system * system, char * data)
gpe_physics_entity load_segment_shape(physics_system * system, gpe_physics_segment data)
{
  //gpe_physics_segment seg_shape = (gpe_physics_segment) data + sizeof(gpe_physics_segment);
  
	cpBody *staticBody = cpSpaceGetStaticBody(system->space);
	cpShape *shape;
	shape = cpSpaceAddShape(system->space, cpSegmentShapeNew(staticBody, cpv(data.vec_a.x, data.vec_a.y), cpv(data.vec_b.x, data.vec_b.y), data.radius));
  cpShapeSetElasticity(shape, data.elasticity);
  cpShapeSetFriction(shape, data.friction);
	cpShapeSetLayers(shape, NOT_GRABABLE_MASK);

  gpe_physics_entity physics = { staticBody, shape, 1 };

  return physics;
}

//U32 physics_system_load (physics_system * system, gpe_physics_type type, char * data)
U32 physics_system_load (physics_system * system, gpe_physics_type type, gpe_physics_segment data)
{
  gpe_physics_entity physics;

  switch (type)
  {
    case segment_shape:      
      physics = load_segment_shape(system, data);
      break;
    default:
      break;
  }

  return gpr_gpe_physics_entity_idlut_add(&system->idlut, &physics);
}

void physics_system_remove (physics_system * system, U32 id)
{
  gpr_gpe_physics_entity_idlut_remove(&system->idlut, id);
}

void  physics_system_submitUpdate (physics_system * system, U32 id, char * data)
{
}

void  physics_system_update (physics_system * system, float dt)
{
}

void  physics_system_free (physics_system * system, U32 id)
{
}