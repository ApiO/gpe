/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <limits.h>
#include <string.h>

#include "GL\glfw3.h"
#include <chipmunk\chipmunk_private.h>
#include "ChipmunkDebugDraw.h"

/*
	IMPORTANT - READ ME!
	
	This file sets up a simple interface that the individual demos can use to get
	a Chipmunk space running and draw what's in it. In order to keep the Chipmunk
	examples clean and simple, they contain no graphics code. All drawing is done
	by accessing the Chipmunk structures at a very low level. It is NOT
	recommended to write a game or application this way as it does not scale
	beyond simple shape drawing and is very dependent on implementation details
	about Chipmunk which may change with little to no warning.
*/

const Color LINE_COLOR = {(float)(200.0/255.0), (float)(210.0/255.0), (float)(230.0/255.0), 1.0f};
const Color CONSTRAINT_COLOR = {0.0, 0.75, 0.0, 1.0};
const float SHAPE_ALPHA = 1.0;

float ChipmunkDebugDrawPointLineScale = 1.0f;

void ChipmunkDebugDraw_SetPointLineScale(float scale)
{
	ChipmunkDebugDrawPointLineScale = scale;
}

static Color
ColorFromHash(cpHashValue hash, float alpha)
{
	unsigned long val = (unsigned long)hash;
	GLfloat r, g, b,
          max, min;
	GLfloat intensity = 0.75;
	
	// scramble the bits up using Robert Jenkins' 32 bit integer hash function
	val = (val+0x7ed55d16) + (val<<12);
	val = (val^0xc761c23c) ^ (val>>19);
	val = (val+0x165667b1) + (val<<5);
	val = (val+0xd3a2646c) ^ (val<<9);
	val = (val+0xfd7046c5) + (val<<3);
	val = (val^0xb55a4f09) ^ (val>>16);
	
	r = (GLfloat)((val>>0) & 0xFF);
	g = (GLfloat)((val>>8) & 0xFF);
	b = (GLfloat)((val>>16) & 0xFF);
	
	max = (GLfloat)cpfmax(cpfmax(r, g), b);
	min = (GLfloat)cpfmin(cpfmin(r, g), b);
	
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

static void
glColor_from_color(Color color){
	glColor4fv((GLfloat *)&color);
}

static Color
ColorForShape(cpShape *shape)
{
	if(cpShapeGetSensor(shape)){
		return LAColor(1.0f, 0.0f);
	} else {
		cpBody *body = shape->body;
		
		if(cpBodyIsSleeping(body)){
			return LAColor(0.2f, 1.0f);
		} else if(body->node.idleTime > shape->space->sleepTimeThreshold) {
			return LAColor(0.66f, 1.0f);
		} else {
			return ColorFromHash(shape->hashid, SHAPE_ALPHA);
		}
	}
}

static const GLfloat circleVAR[] = {
	 0.0000f,  1.0000f,
	 0.2588f,  0.9659f,
	 0.5000f,  0.8660f,
	 0.7071f,  0.7071f,
	 0.8660f,  0.5000f,
	 0.9659f,  0.2588f,
	 1.0000f,  0.0000f,
	 0.9659f, -0.2588f,
	 0.8660f, -0.5000f,
	 0.7071f, -0.7071f,
	 0.5000f, -0.8660f,
	 0.2588f, -0.9659f,
	 0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	 0.0000f,  1.0000f,
	 0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLfloat)/2;

void ChipmunkDebugDrawCircle(cpVect center, cpFloat angle, cpFloat radius, Color lineColor, Color fillColor)
{
	glVertexPointer(2, GL_FLOAT, 0, circleVAR);

	glPushMatrix(); {
		glTranslatef((GLfloat)center.x, (GLfloat)center.y, 0.0f);
		glRotatef((GLfloat)(angle*180.0f/M_PI), 0.0f, 0.0f, 1.0f);
		glScalef((GLfloat)radius, (GLfloat)radius, 1.0f);
		
		if(fillColor.a > 0){
			glColor_from_color(fillColor);
			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
		}
		
		if(lineColor.a > 0){
			glColor_from_color(lineColor);
			glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
		}
	} glPopMatrix();
}

static const GLfloat pillVAR[] = {
	 0.0000f,  1.0000f, 1.0f,
	 0.2588f,  0.9659f, 1.0f,
	 0.5000f,  0.8660f, 1.0f,
	 0.7071f,  0.7071f, 1.0f,
	 0.8660f,  0.5000f, 1.0f,
	 0.9659f,  0.2588f, 1.0f,
	 1.0000f,  0.0000f, 1.0f,
	 0.9659f, -0.2588f, 1.0f,
	 0.8660f, -0.5000f, 1.0f,
	 0.7071f, -0.7071f, 1.0f,
	 0.5000f, -0.8660f, 1.0f,
	 0.2588f, -0.9659f, 1.0f,
	 0.0000f, -1.0000f, 1.0f,

	 0.0000f, -1.0000f, 0.0f,
	-0.2588f, -0.9659f, 0.0f,
	-0.5000f, -0.8660f, 0.0f,
	-0.7071f, -0.7071f, 0.0f,
	-0.8660f, -0.5000f, 0.0f,
	-0.9659f, -0.2588f, 0.0f,
	-1.0000f, -0.0000f, 0.0f,
	-0.9659f,  0.2588f, 0.0f,
	-0.8660f,  0.5000f, 0.0f,
	-0.7071f,  0.7071f, 0.0f,
	-0.5000f,  0.8660f, 0.0f,
	-0.2588f,  0.9659f, 0.0f,
	 0.0000f,  1.0000f, 0.0f,
};
static const int pillVAR_count = sizeof(pillVAR)/sizeof(GLfloat)/3;

void ChipmunkDebugDrawSegment(cpVect a, cpVect b, Color color)
{
	GLfloat verts[] = {
		(GLfloat)a.x, (GLfloat)a.y,
		(GLfloat)b.x, (GLfloat)b.y,
	};
	
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColor_from_color(color);
	glDrawArrays(GL_LINES, 0, 2);
}

void ChipmunkDebugDrawFatSegment(cpVect a, cpVect b, cpFloat radius, Color lineColor, Color fillColor)
{
	if(radius){
		glVertexPointer(3, GL_FLOAT, 0, pillVAR);
		glPushMatrix(); {
			cpVect d = cpvsub(b, a);
			cpVect r = cpvmult(d, radius/cpvlength(d));

			const GLfloat matrix[] = {
				 (GLfloat)r.x, (GLfloat)r.y, 0.0f, 0.0f,
				(GLfloat)-r.y, (GLfloat)r.x, 0.0f, 0.0f,
				 (GLfloat)d.x, (GLfloat)d.y, 0.0f, 0.0f,
				 (GLfloat)a.x, (GLfloat)a.y, 0.0f, 1.0f,
			};
			glMultMatrixf(matrix);
			
			if(fillColor.a > 0){
				glColor_from_color(fillColor);
				glDrawArrays(GL_TRIANGLE_FAN, 0, pillVAR_count);
			}
			
			if(lineColor.a > 0){
				glColor_from_color(lineColor);
				glDrawArrays(GL_LINE_LOOP, 0, pillVAR_count);
			}
		} glPopMatrix();
	} else {
		ChipmunkDebugDrawSegment(a, b, lineColor);
	}
}

void ChipmunkDebugDrawPolygon(int count, cpVect *verts, Color lineColor, Color fillColor)
{
#if CP_USE_DOUBLES
	glVertexPointer(2, GL_DOUBLE, 0, verts);
#else
	glVertexPointer(2, GL_FLOAT, 0, verts);
#endif
	
	if(fillColor.a > 0){
		glColor_from_color(fillColor);
		glDrawArrays(GL_TRIANGLE_FAN, 0, count);
	}
	
	if(lineColor.a > 0){
		glColor_from_color(lineColor);
		glDrawArrays(GL_LINE_LOOP, 0, count);
	}
}

void ChipmunkDebugDrawPoints(cpFloat size, int count, cpVect *verts, Color color)
{
#if CP_USE_DOUBLES
	glVertexPointer(2, GL_DOUBLE, 0, verts);
#else
	glVertexPointer(2, GL_FLOAT, 0, verts);
#endif
	
	glPointSize((GLfloat)size*ChipmunkDebugDrawPointLineScale);
	glColor_from_color(color);
	glDrawArrays(GL_POINTS, 0, count);
}

void ChipmunkDebugDrawBB(cpBB bb, Color color)
{
	cpVect verts[4];
	verts[0] = cpv(bb.l, bb.b);
	verts[1] = cpv(bb.l, bb.t);
	verts[2] = cpv(bb.r, bb.t);
	verts[3] = cpv(bb.r, bb.b);
	
	ChipmunkDebugDrawPolygon(4, verts, color, LAColor(0, 0));
}

static void
drawShape(cpShape *shape, void *unused)
{
	cpBody *body = shape->body;
	Color color = ColorForShape(shape);
	
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE: {
			cpCircleShape *circle = (cpCircleShape *)shape;
			ChipmunkDebugDrawCircle(circle->tc, body->a, circle->r, LINE_COLOR, color);
			break;
		}
		case CP_SEGMENT_SHAPE: {
			cpSegmentShape *seg = (cpSegmentShape *)shape;
			ChipmunkDebugDrawFatSegment(seg->ta, seg->tb, seg->r, LINE_COLOR, color);
			break;
		}
		case CP_POLY_SHAPE: {
			cpPolyShape *poly = (cpPolyShape *)shape;
			ChipmunkDebugDrawPolygon(poly->numVerts, poly->tVerts, LINE_COLOR, color);
			break;
		}
		default: break;
	}
}

void ChipmunkDebugDrawShape(cpShape *shape)
{
	drawShape(shape, NULL);
}

void ChipmunkDebugDrawShapes(cpSpace *space)
{
	cpSpaceEachShape(space, drawShape, NULL);
}

static const GLfloat springVAR[] = {
	0.00f, 0.0f,
	0.20f, 0.0f,
	0.25f, 3.0f,
	0.30f,-6.0f,
	0.35f, 6.0f,
	0.40f,-6.0f,
	0.45f, 6.0f,
	0.50f,-6.0f,
	0.55f, 6.0f,
	0.60f,-6.0f,
	0.65f, 6.0f,
	0.70f,-3.0f,
	0.75f, 6.0f,
	0.80f, 0.0f,
	1.00f, 0.0f,
};
static const int springVAR_count = sizeof(springVAR)/sizeof(GLfloat)/2;

static void
drawSpring(cpDampedSpring *spring, cpBody *body_a, cpBody *body_b)
{
	cpVect points[2];
	cpVect delta;
	cpVect a = cpvadd(body_a->p, cpvrotate(spring->anchr1, body_a->rot));
	cpVect b = cpvadd(body_b->p, cpvrotate(spring->anchr2, body_b->rot));
	
	points[0] = a;
	points[1] = b;
	ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);

	delta = cpvsub(b, a);

	glVertexPointer(2, GL_FLOAT, 0, springVAR);
	glPushMatrix(); {
		GLfloat x = (GLfloat)a.x;
		GLfloat y = (GLfloat)a.y;
		GLfloat cos = (GLfloat)delta.x;
		GLfloat sin = (GLfloat)delta.y;
		GLfloat s = (GLfloat)(1.0f/cpvlength(delta));

		const GLfloat matrix[] = {
				 cos,    sin, 0.0f, 0.0f,
			-sin*s,  cos*s, 0.0f, 0.0f,
				0.0f,   0.0f, 1.0f, 0.0f,
					 x,      y, 0.0f, 1.0f,
		};
		
		glMultMatrixf(matrix);
		glDrawArrays(GL_LINE_STRIP, 0, springVAR_count);
	} glPopMatrix();
}

static void
drawConstraint(cpConstraint *constraint, void *unused)
{
	cpVect points[2];
	cpBody *body_a = constraint->a;
	cpBody *body_b = constraint->b;

	const cpConstraintClass *klass = constraint->klass;
	if(klass == cpPinJointGetClass()){
		cpPinJoint *joint = (cpPinJoint *)constraint;
	
		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
		
		points[0] = a;
		points[1] = b;
		ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);
		ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
	} else if(klass == cpSlideJointGetClass()){
		cpSlideJoint *joint = (cpSlideJoint *)constraint;
	
		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
		
		points[0] = a;
		points[1] = b;
		ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);
		ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
	} else if(klass == cpPivotJointGetClass()){
		cpPivotJoint *joint = (cpPivotJoint *)constraint;
	
		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
    
		points[0] = a;
		points[1] = b;
		ChipmunkDebugDrawPoints(10, 2, points, CONSTRAINT_COLOR);
	} else if(klass == cpGrooveJointGetClass()){
		cpGrooveJoint *joint = (cpGrooveJoint *)constraint;
	
		cpVect a = cpvadd(body_a->p, cpvrotate(joint->grv_a, body_a->rot));
		cpVect b = cpvadd(body_a->p, cpvrotate(joint->grv_b, body_a->rot));
		cpVect c = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
		
		ChipmunkDebugDrawPoints(5, 1, &c, CONSTRAINT_COLOR);
		ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
	} else if(klass == cpDampedSpringGetClass()){
		drawSpring((cpDampedSpring *)constraint, body_a, body_b);
	}
}

void ChipmunkDebugDrawConstraint(cpConstraint *constraint)
{
	drawConstraint(constraint, NULL);
}

void ChipmunkDebugDrawConstraints(cpSpace *space)
{
	cpSpaceEachConstraint(space, drawConstraint, NULL);
}

void ChipmunkDebugDrawCollisionPoints(cpSpace *space)
{
  int i, j;
	cpArray *arbiters = space->arbiters;
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(4.0f*ChipmunkDebugDrawPointLineScale);
	
	glBegin(GL_POINTS); {
		for(i=0; i<arbiters->num; i++){
			cpArbiter *arb = (cpArbiter*)arbiters->arr[i];
			
			for(j=0; j<arb->numContacts; j++){
				cpVect v = arb->contacts[j].p;
				glVertex2f((GLfloat)v.x, (GLfloat)v.y);
			}
		}
	} glEnd();
}
