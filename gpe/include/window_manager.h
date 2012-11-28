/*

  Bouzin infernal qui regroupe la gestion de la fenêtre, les input et output graphique via glfw
  TODO
    separation du conteneur (la fenêtre) de la gestion du rendu : wrapper graphique
    virrer la ref à chipmunk : ici à cause de types, remplaçables (cpVect, cpFloat), dans les signature des draw
*/

#ifndef _window_manager_h_
#define _window_manager_h_

#include <GL\glfw.h>
//#include <chipmunk\chipmunk.h>
/*
typedef struct {
	float r, g, b, a;
} Color;
*/
typedef struct
{
  int   running;
  int   restart;
} window_manager;

void window_manager_init (window_manager * manager, char * title, int height, int width);
void window_manager_clear (void);
void window_manager_swapBuffers (window_manager * manager);
void window_manager_free (window_manager * manager);
/*
void drawCircle(cpVect center, cpFloat angle, cpFloat radius, Color lineColor, Color fillColor);
void drawFatSegment(cpVect a, cpVect b, cpFloat radius, Color lineColor, Color fillColor);
void drawSegment(cpVect a, cpVect b, Color color);
void drawPolygon(int count, cpVect *verts, Color lineColor, Color fillColor);
*/
#endif //_window_manager_h_