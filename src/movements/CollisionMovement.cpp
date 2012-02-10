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
#include "movements/CollisionMovement.h"
#include "Game.h"
#include "Map.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"

/**
 * @brief Constructor specifying whether the collisions are enabled.
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
CollisionMovement::CollisionMovement(bool ignore_obstacles):
  last_collision_box_on_obstacle(-1, -1),
  initial_ignore_obstacles(ignore_obstacles), current_ignore_obstacles(ignore_obstacles) {

}

/**
 * @brief Destructor.
 */
CollisionMovement::~CollisionMovement(void) {

}

/**
 * @brief Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool CollisionMovement::test_collision_with_map(int dx, int dy) {

  if (current_ignore_obstacles) {
    return false;
  }

  Map *map = entity->get_map();

  // place the collision box where we want to check the collisions
  Rectangle collision_box = entity->get_bounding_box();
  collision_box.add_xy(dx, dy);

  bool collision = map->test_collision_with_obstacles(entity->get_layer(), collision_box, entity);

  if (collision) {
    last_collision_box_on_obstacle = collision_box;
    entity->notify_movement_changed();
  }

  return collision;
}

/**
 * @brief Updates the x position of the entity if it wants to move.
 *
 * This is a redefinition of Movement::update_x() to make the move
 * only if there is no collision with the map.
 */
void CollisionMovement::update_x(void) {

  uint32_t now = System::now();
  int x_move = get_x_move();
  if (x_move != 0 && now >= get_next_move_date_x()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(x_move, get_y_move())) {
      translate_x(x_move);
    }
    else {
      stop(); // also stop on y
    }
    set_next_move_date_x(get_next_move_date_x() + get_x_delay());
  }
}

/**
 * @brief Updates the y position of the entity if it wants to move
 *
 * This is a redefinition of Movement::update_y() to make the move
 * only if there is no collision with the map.
 */
void CollisionMovement::update_y(void) {

  uint32_t now = System::now();
  int y_move = get_y_move();
  if (y_move != 0 && now >= get_next_move_date_y()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(get_x_move(), y_move)) {
      translate_y(y_move);
    }
    else {
      stop(); // also stop on x
    }
    set_next_move_date_y(get_next_move_date_y() + get_y_delay());
  }
}

/**
 * @brief Returns the collision box of the last call
 * to test_collision_with_map() returning true.
 * @return the collision box of the last collision detected, or (-1, -1) if no obstacle was detected
 */
const Rectangle & CollisionMovement::get_last_collision_box_on_obstacle(void) {
  return last_collision_box_on_obstacle;
}

/**
 * @brief Returns whether this movement currently ignores obstacles.
 * @return true if the obstacles are ignored
 */
bool CollisionMovement::are_obstacles_ignored(void) {
  return current_ignore_obstacles;
}

/**
 * @brief Allows temporarily this movement to traverse obstacles.
 */
void CollisionMovement::set_ignore_obstacles(void) {
  this->current_ignore_obstacles = true;
}

/**
 * @brief Restores the initial value of ignore_obstacles.
 */
void CollisionMovement::restore_ignore_obstacles(void) {
  this->current_ignore_obstacles = initial_ignore_obstacles;
}

