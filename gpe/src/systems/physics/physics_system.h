#ifndef PHYSICS_SYSTEM_H_
#define PHYSICS_SYSTEM_H_


//#include <chipmunk.h>

typedef struct
{
  //cpSpace *space;
  /*
    list obj : liste des entité physiques
     type id;
     cpBody body;
     cpShape shapes[];
  */
  /*
   type toUpdate[]; //liste des id d'élément à updater pour le tick courrant
  */
  int asuppr;
} physics_system;

//void  load (id, data)
//OU
//type  load (data)

//void  remove (id)
//void  submitUpdate (id,data)
//void  update (dt)
//void  reset (void)
//void  free (void)

#endif // PHYSICS_SYSTEM_H_