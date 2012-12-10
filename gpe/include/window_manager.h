/*

  Bouzin infernal qui regroupe la gestion de la fen�tre, les input et output graphique via glfw
  TODO
    separation du conteneur (la fen�tre) de la gestion du rendu : wrapper graphique
    virrer la ref � chipmunk : ici � cause de types, rempla�ables (cpVect, cpFloat), dans les signature des draw
*/

#ifndef _window_manager_h_
#define _window_manager_h_

typedef struct window_manager
{
  int   running;
  int   restart;
} window_manager;

void    window_manager_init (window_manager * manager, char * title, int height, int width);
void    window_manager_clear (void);
void    window_manager_swapBuffers (window_manager * manager);
void    window_manager_free (window_manager * manager);
double  window_manager_getFps(void);

#endif //_window_manager_h_