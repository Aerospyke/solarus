#ifndef ZSDX_BLOCK_H
#define ZSDX_BLOCK_H

#include "Common.h"
#include "Detector.h"

/**
 * A block or a statue that the hero can push.
 * If this a statue, it can also be pulled.
 */
class Block: public Detector {

 public:

  /**
   * The different kinds of blocks: normal block
   * or statue.
   */
  enum Subtype {
    NORMAL, /**< a usual block */
    STATUE, /**< a statue */
  };

 private:

  SDL_Rect initial_position; /**< position of the block when created */
  SDL_Rect last_position;    /**< last position of the block before moving */
  Subtype subtype;           /**< normal block or statue */
  int maximum_moves;         /**< indicates whether the block can be pushed
			      * (0: none, 1: once: 2: infinite) */
  bool sound_played;         /**< true if the block sound was played while pulling it */

 public:

  Block(string name, Layer layer, int x, int y,
	Subtype subtype, string skin, int maximum_push);
  ~Block();

  EntityType get_type(void);

  bool is_obstacle_for(MapEntity *other);
  bool collision_with_map(int dx, int dy);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  bool moved_by_hero(void);

  void update(void);
  void display_on_map_above_hero(void);
};

#endif
