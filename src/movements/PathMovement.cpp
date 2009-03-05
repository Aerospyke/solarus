#include "movements/PathMovement.h"

/**
 * Creates a path movement object.
 * @param map the map (can be NULL if with_collisions is false)
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 * @param speed speed of the movement
 * @param loop true to make the movement return to the beginning
 * once finished
 */
PathMovement::PathMovement(Map *map, string path, int speed, bool loop):
  MovementWithCollision(map),
  path(path), next_index(0), loop(loop),
  finished(false) {

  set_speed(speed);
  start_next_move();
}

/**
 * Destructor.
 */
PathMovement::~PathMovement(void) {

}

/**
 * Sets the x position of the entity.
 * This is a redefinition of MovementWithCollision::set_x() because we need
 * the number of pixels covered.
 * @param x the new x position
 */
void PathMovement::set_x(int x) {

  if (x != get_x()) {
    distance_covered_x++;
    if (distance_covered_x == 8) {
      start_next_move();
    }
  }
  MovementWithCollision::set_x(x);
}

/**
 * Sets the y position of the entity.
 * This is a redefinition of MovementWithCollision::set_y() because we need
 * the number of pixels covered.
 * @param y the new y position
 */
void PathMovement::set_y(int y) {

  if (y != get_y()) {
    distance_covered_y++;
    if (distance_covered_y == 8) {
      start_next_move();
    }
  }
  MovementWithCollision::set_y(y);
}

/**
 * Returns whether the movement is finished.
 * @return true if the end of the path was reached or the entity 
 * reached an obstacle
 */
bool PathMovement::is_finished(void) {
  return finished || is_stopped();
}

/**
 * Starts the next step of the movement.
 */
void PathMovement::start_next_move(void) {

  if (next_index == path.size()) {
    if (loop) {
      next_index = 0;
    }
    else {
      finished = true;
      stop();
    }
  }

  if (!finished) {
    current_direction = path[next_index] - '0';
    set_direction(current_direction * 45);
    next_index++;
    distance_covered_x = 0;
    distance_covered_y = 0;
  }
}

/**
 * Returns the direction of the current move, between 0 and 7.
 * @return the direction of the current move
 */
int PathMovement::get_current_direction(void) {
  return current_direction;
}
