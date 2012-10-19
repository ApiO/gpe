#include "physics_debug.h"
#include "window_manager.h"

const Color LINE_COLOR = {200.0/255.0, 210.0/255.0, 230.0/255.0, 1.0};
const Color CONSTRAINT_COLOR = {0.0, 0.75, 0.0, 1.0};
const float SHAPE_ALPHA = 1.0;

typedef struct {
	cpSpaceShapeIteratorFunc 
        func;
	void *data;
} spaceShapeContext;

void cpSpaceLock(cpSpace *space);
void drawShapes(cpSpace * space, cpSpaceShapeIteratorFunc func, void *data);
void drawConstraints(cpSpace * space);
void drawCollisionPoints(cpSpace * space);
void drawShape(cpShape *shape, void *unused);
static void spaceEachShapeIterator(cpShape *shape, spaceShapeContext *context);
static Color ColorForShape(cpShape *shape);
static Color ColorFromHash(cpHashValue hash, float alpha);
static inline Color LAColor(float l, float a);
static inline Color RGBAColor(float r, float g, float b, float a);
static inline void glColor_from_color(Color color);
//void cpSpaceLock(cpSpace *space);
//void cpSpaceUnlock(cpSpace *space, cpBool runPostStep);


void physics_debug_init (physics_debug * debug, cpSpace * space)
{
  debug->space = space;
}

void physics_debug_draw (physics_debug * debug)
{
  drawShapes(debug->space, drawShape, NULL);
	drawConstraints(debug->space);
	drawCollisionPoints(debug->space);
}

void drawShapes(cpSpace * space, cpSpaceShapeIteratorFunc func, void *data)
{
  //TODO
	//cpSpaceLock(space); {
		spaceShapeContext context = { func, data };
		cpSpatialIndexEach(space->activeShapes_private, (cpSpatialIndexIteratorFunc)spaceEachShapeIterator, &context);
		cpSpatialIndexEach(space->staticShapes_private, (cpSpatialIndexIteratorFunc)spaceEachShapeIterator, &context);
	//} cpSpaceUnlock(space, cpTrue);
}

static void spaceEachShapeIterator(cpShape *shape, spaceShapeContext *context)
{
	context->func(shape, context->data);
}


static Color ColorFromHash(cpHashValue hash, float alpha)
{
	unsigned long val = (unsigned long)hash;
	
	// scramble the bits up using Robert Jenkins' 32 bit integer hash function
	val = (val+0x7ed55d16) + (val<<12);
	val = (val^0xc761c23c) ^ (val>>19);
	val = (val+0x165667b1) + (val<<5);
	val = (val+0xd3a2646c) ^ (val<<9);
	val = (val+0xfd7046c5) + (val<<3);
	val = (val^0xb55a4f09) ^ (val>>16);
	
	GLfloat r = (val>>0) & 0xFF;
	GLfloat g = (val>>8) & 0xFF;
	GLfloat b = (val>>16) & 0xFF;
	
	GLfloat max = cpfmax(cpfmax(r, g), b);
	GLfloat min = cpfmin(cpfmin(r, g), b);
	GLfloat intensity = 0.75;
	
	// Saturate and scale the color
	if(min == max){
		return RGBAColor(intensity, 0.0, 0.0, alpha);
	} else {
		GLfloat coef = alpha*intensity/(max - min);
		return RGBAColor(
			(r - min)*coef,
			(g - min)*coef,
			(b - min)*coef,
			alpha
		);
	}
}

static inline Color LAColor(float l, float a){
	Color color = {l, l, l, a};
	return color;
}

static inline Color RGBAColor(float r, float g, float b, float a){
	Color color = {r, g, b, a};
	return color;
}

static Color ColorForShape(cpShape *shape)
{
	if(cpShapeGetSensor(shape)){
		return LAColor(1, 0);
	} else {
		cpBody *body = shape->body;
		
		if(cpBodyIsSleeping(body)){
			return LAColor(0.2, 1);
		} else if(body->node_private.idleTime > shape->space_private->sleepTimeThreshold) {
			return LAColor(0.66, 1);
		} else {
			return ColorFromHash(shape->hashid_private, SHAPE_ALPHA);
		}
	}
}

static void drawShape(cpShape *shape, void *unused)
{
	cpBody *body = shape->body;
	Color color = ColorForShape(shape);
	
	switch(shape->klass_private->type){
		case CP_CIRCLE_SHAPE: {
			cpCircleShape *circle = (cpCircleShape *)shape;
			drawCircle(circle->tc, body->a, circle->r, LINE_COLOR, color);
			break;
		}
		case CP_SEGMENT_SHAPE: {
			cpSegmentShape *seg = (cpSegmentShape *)shape;
			drawFatSegment(seg->ta, seg->tb, seg->r, LINE_COLOR, color);
			break;
		}
		case CP_POLY_SHAPE: {
			cpPolyShape *poly = (cpPolyShape *)shape;
			drawPolygon(poly->numVerts, poly->tVerts, LINE_COLOR, color);
			break;
		}
		default: break;
	}
}

void drawConstraints(cpSpace * space)
{
  //TODO
}

void drawCollisionPoints(cpSpace * space)
{
  //TODO
}

/*
void cpSpaceLock(cpSpace *space)
{
  space->locked_private += 1;
}

void cpSpaceUnlock(cpSpace *space, cpBool runPostStep)
{
	space->locked_private -= 1;
	
	if(space->locked_private == 0 && runPostStep && !space->skipPostStep_private){
		space->skipPostStep_private = cpTrue;
		
		cpArray *waking = space->rousedBodies_private;
		for(int i=0, count=waking->num; i<count; i++){
			cpSpaceActivateBody(space, (cpBody *)waking->arr[i]);
			waking->arr[i] = NULL;
		}
		
		cpArray *arr = space->postStepCallbacks_private;
		for(int i=0; i<arr->num; i++){
			cpPostStepCallback *callback = (cpPostStepCallback *)arr->arr[i];
			cpPostStepFunc func = callback->func;
			
			// Mark the func as NULL in case calling it calls cpSpaceRunPostStepCallbacks() again.
			callback->func = NULL;
			if(func) func(space, callback->key, callback->data);
			
			arr->arr[i] = NULL;
			cpfree(callback);
		}
		
		waking->num = 0;
		arr->num = 0;
		space->skipPostStep_private = cpFalse;
	}
}
*/