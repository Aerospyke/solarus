/**
 * This module defines the class Link.
 */

#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>
#include "Map.h"
#include "Moving8ByPlayer.h"
#include "AnimatedSprite.h"

/**
 * Link's entity.
 * It is animated and can be controlled with an 8 directions system.
 */
class Link: public Moving8ByPlayer {

 protected:

  /**
   * Link's sprite.
   */
  AnimatedSprite *sprite;

  /**
   * Redefinition of Moving8ByPlayer::update_movement
   * to take care of link's animation.
   * This function is called when the user presses or
   * releases a keyboard arrow.
   */
  void update_movement(void);

 public:

  /**
   * Constructor.
   */
  Link(void);

  /**
   * Destructor.
   */
  ~Link(void);

  /**
   * Returns Link's sprite.
   * @return Link's sprite
   */
  inline AnimatedSprite *get_sprite(void) {
    return sprite;
  }

  /**
   * Sets Link's current map.
   * @param map the map
   */
  void set_map(Map *map);

  /**
   * Displays Link on the map with its current animation and
   * at its current position.
   * @param map the map
   */
  void display_on_map(Map *map);
};

#endif
