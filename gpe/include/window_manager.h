/*

  Bouzin infernal qui regroupe la gestion de la fen�tre, les input et output graphique via glfw
  TODO
    separation du conteneur (la fen�tre) de la gestion du rendu : wrapper graphique
    virrer la ref � chipmunk : ici � cause de types, rempla�ables (cpVect, cpFloat), dans les signature des draw
*/

#ifndef window_manager_h
#define window_manager_h

typedef struct window_manager
{
  int     running;
  int     restart;
  double  fps;
  bool    display_fps;
} window_manager;

void    window_manager_init         (window_manager * manager, char * title, int height, int width);
void    window_manager_clear        (window_manager * manager);
void    window_manager_swapBuffers  (window_manager * manager);
void    window_manager_free         (window_manager * manager);

#endif //window_manager_h