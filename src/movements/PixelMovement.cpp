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
#include "movements/PixelMovement.h"
#include "lowlevel/System.h"

/**
 * Creates a pixel movement object, not specifying the trajectory for now.
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PixelMovement::PixelMovement(int nb_vectors, uint32_t delay,
			     bool loop, bool with_collisions):
  CollisionMovement(with_collisions),
  nb_vectors(nb_vectors), delay(delay), loop(loop),
  vector_index(0), finished(false) {

}

/**
 * Creates a pixel movement object.
 * @param translation_vectors the succession of translations
 * composing this movement (each element of the array represents
 * a translation vector in pixels; only the fields x and y of the
 * Rectangle are used.
 * @param nb_vectors number of translation vectors in the array
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PixelMovement::PixelMovement(const Rectangle *translation_vectors,
			     int nb_vectors, uint32_t delay, bool loop, bool with_collisions):
  CollisionMovement(with_collisions),
  translation_vectors(translation_vectors), nb_vectors(nb_vectors),
  delay(delay), loop(loop), vector_index(0), finished(false) {

}

/**
 * Destructor.
 */
PixelMovement::~PixelMovement(void) {

}

/**
 * Sets the translation vectors of the trajectory.
 * @param translation_vectors the trajectory
 */
void PixelMovement::set_translation_vectors(const Rectangle *translation_vectors) {
  this->translation_vectors = translation_vectors;
}

/**
 * Changes the delay between two moves.
 * @param delay the new delay
 */
void PixelMovement::set_delay(uint32_t delay) {
  this->delay = delay;
}

/**
 * Updates the position.
 */
void PixelMovement::update(void) {

  if (is_suspended()) {
    return;
  }

  uint32_t now = System::now();

  while (now >= get_next_move_date_x() && !finished) {
    make_next_move();
  }
}

/**
 * Makes a move in the path.
 */
void PixelMovement::make_next_move(void) {

  int dx = translation_vectors[vector_index].x;
  int dy = translation_vectors[vector_index].y;

  if (!test_collision_with_map(dx, dy)) {
    translate(dx, dy);
  }

  set_next_move_date_x(get_next_move_date_x() + delay);

  vector_index++;
  if (vector_index >= nb_vectors) {
    if (loop) {
      vector_index = 0;
    }
    else {
      finished = true;
    }
  }
}

/**
 * Returns the total number of moves in this trajectory.
 * @return the total number of moves in this trajectory
 */
int PixelMovement::get_length(void) {
  return nb_vectors;
}

/**
 * Returns the current iteration number.
 * @return the current iteration number of the movement
 */
int PixelMovement::get_vector_index(void) {
  return vector_index;
}

/**
 * Returns whether the movement is finished, i.e.
 * whether the end of the trajectory was reached.
 */
bool PixelMovement::is_finished(void) {
  return finished;
}
