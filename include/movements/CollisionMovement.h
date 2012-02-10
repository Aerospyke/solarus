/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_COLLISION_MOVEMENT_H
#define SOLARUS_COLLISION_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Abstract class for all movements sensible to obstacle.
 *
 * This class is inherited by all movements that may be sensible to the obstacles of the map.
 */
class CollisionMovement: public Movement {

  private:

    Rectangle last_collision_box_on_obstacle;	/**< copy of the collision box of the last call
						 * to test_collision_with_map() returning true */ 

    const bool initial_ignore_obstacles;	/**< indicates that this movement ignores collisions with obstacles when it is created */
    bool current_ignore_obstacles;		/**< indicates that this movement currently ignores obstacles */

  public:

    CollisionMovement(bool ignore_obstacles = false);
    virtual ~CollisionMovement();

    bool test_collision_with_map(int dx, int dy);
    virtual void update_x();
    virtual void update_y();

    const Rectangle & get_last_collision_box_on_obstacle();
    bool are_obstacles_ignored();
    void set_ignore_obstacles();
    void restore_ignore_obstacles();
};

#endif

