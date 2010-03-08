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
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "Map.h"
#include "entities/Tileset.h"
#include "lowlevel/Surface.h"

/**
 * Constructor.
 * @param image_file_name the image from which the frames are extracted
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param frame_delay delay in millisecond between two frames for this sprite animation
 * (or 0 to make no animation, for example when you have only one frame)
 * @param loop_on_frame frame to loop on after the last frame (or -1 to make no loop)
 */
SpriteAnimation::SpriteAnimation(const std::string &image_file_name, 
    int nb_directions, SpriteAnimationDirection **directions, uint32_t frame_delay, int loop_on_frame):

  src_image(NULL), src_image_loaded(false), nb_directions(nb_directions), directions(directions),
  frame_delay(frame_delay), loop_on_frame(loop_on_frame) {

  if (image_file_name != "tileset") {
    src_image = new Surface(image_file_name);
    src_image_loaded = true;
  }
}

/**
 * Destructor.
 */
SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }

  delete[] directions;

  if (src_image_loaded) {
    delete src_image;
  }
}

/**
 * When the sprite is displayed on a map, sets the map.
 * This function must be called if this sprite image depends on the map's tileset.
 * @param map the map
 */
void SpriteAnimation::set_map(Map *map) {

  if (!src_image_loaded) {
    this->src_image = map->get_tileset()->get_entities_image();
  }
}

/**
 * Returns the number of directions of this animation.
 * @return the number of directions
 */
int SpriteAnimation::get_nb_directions(void) {
  return nb_directions;
}

/**
 * Returns a direction.
 * @param direction the direction
 * @return the sequence of images corresponding to this direction
 */
SpriteAnimationDirection * SpriteAnimation::get_direction(int direction) {
  return directions[direction];
}

/**
 * Returns the delay between two frames for this sprite animation.
 * @return the frame delay in milliseconds
 */
uint32_t SpriteAnimation::get_frame_delay(void) {
  return frame_delay;
}

/**
 * Returns whether this animation loops on a frame.
 * @return true if this animation loops
 */
bool SpriteAnimation::is_looping(void) {
  return loop_on_frame != -1;
}

/**
 * Returns the next frame of the current frame.
 * @param current_direction the current direction
 * @param current_frame the current frame
 * @return the next frame of the current frame in this direction
 * (or -1 if the animation is over)
 */
int SpriteAnimation::get_next_frame(int current_direction, int current_frame) {

  if (current_direction >= nb_directions) {
    DIE("Invalid sprite direction '" << current_direction << "': this sprite animation has only "
	<< nb_directions << " direction(s)");
  }

  int next_frame = current_frame + 1;

  // if we are on the last frame
  if (next_frame == directions[current_direction]->get_nb_frames()) {
    // we loop on the appropriate frame
    // or -1 if there is no loop
    next_frame = loop_on_frame;
  }

  return next_frame;
}

/**
 * Displays a specific frame of this animation on a surface.
 * @param destination the surface on which the sprite will be displayed
 * @param x x coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param current_direction the direction to show
 * @param current_frame the frame to show in this direction
 */
void SpriteAnimation::display(Surface *destination, int x, int y,
    int current_direction, int current_frame) {

  directions[current_direction]->display(destination, x, y, current_frame, src_image);
}

/**
 * Enables the pixel-perfect collision detection for this animation.
 */
void SpriteAnimation::enable_pixel_collisions(void) {

  for (int i = 0; i < nb_directions; i++) {
    directions[i]->enable_pixel_collisions(src_image);
  }
}

/**
 * Returns whether the pixel-perfect collisions are enabled for this animations.
 * @return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimation::are_pixel_collisions_enabled(void) {
  return directions[0]->are_pixel_collisions_enabled();
}

