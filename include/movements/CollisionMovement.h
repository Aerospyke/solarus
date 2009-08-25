/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_COLLISION_MOVEMENT_H
#define ZSDX_COLLISION_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * Abstract movement class for all moving entities
 * that may be sensible to the obstacles of their map.
 */
class CollisionMovement: public Movement {

 private:
  SDL_Rect last_collision_box_on_obstacle; /**< copy of the collision box of the last call
					    * to test_collision_with_map() returning true */ 
  int last_direction;                      /**< direction of the movement before a collision */
  bool stop_on_obstacles;                  /**< indicates that this movement is currently sensible to collisions with obstacles */

 public:

  CollisionMovement(void);
  CollisionMovement(bool stop_on_obstacles);
  virtual ~CollisionMovement(void);

  bool test_collision_with_map(int dx, int dy);
  virtual void update_x(void);
  virtual void update_y(void);

  const SDL_Rect& get_last_collision_box_on_obstacle(void);
  int get_last_direction(void);

  void set_stop_on_obstacles(bool stop_on_obstacles);
};

#endif

