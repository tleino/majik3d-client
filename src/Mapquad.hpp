#ifndef __MAPQUAD_HPP__
#define __MAPQUAD_HPP__

#include <ssg.h>

/// Size of one map chunk.
#define   MAP_SIZE    16

/** A map chunk. */

class Mapquad
{
 public:
   ///
   Mapquad(Mapquad *parent, int level, int top_x, int top_y);
   ~Mapquad();

   ///
   static Mapquad *root_map;
  
   ///
   Mapquad *parent;
   ///
   Mapquad *child1;
   ///
   Mapquad *child2;
   ///
   Mapquad *child3;
   ///
   Mapquad *child4;

   ///
   char *submap1;
   ///
   char *submap2;
   ///
   char *submap3;
   ///
   char *submap4;
   
   ///
   ssgSelector  *lod_switch;
   
   ///
   int *lod_indices;
   
   ///
   ssgTransform *trans;
   //   Object *inventory;
   //   Object *observers;

   ///
   int level;
   ///
   int top_x;
   ///
   int top_y;
   ///
   int mid_x;
   ///
   int mid_y;
   ///
   int reference_count;
   ///
   int map_requested;
   ///
   char *terrain_map;
//   char zmap[45646];
   ///
   int  incRef  ();
   ///
   int  decRef  ();
   /// Clean up the map from the memory when it is no longer needed.
   void cleanUp ();
   
   ///
   Mapquad *getChild1();
   ///
   Mapquad *getChild2();
   ///
   Mapquad *getChild3();
   ///
   Mapquad *getChild4();
   ///
   Mapquad *getMapquad(int, int, int);
   ///
   Mapquad *tryMapquad(int, int, int);
   ///
   char *getMap ();
   ///
   void setMap (char *);
   ///
   void setSubMap (int, char *);
   ///
   void selectLOD(int);
   ///
   void divideMap(int, char *);
   ///
   void show();
   ///
   void hide();
   
   /*
   getMaps ( ... );
   getInventory ( ... );
   getObservers ( ... );
  */ 

//   static int stats[13];

  // static void printStats();
};


#endif // __MAPQUAD_HPP__
