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
#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include "Common.h"

/**
 * An animation direction is a sequence of
 * frames representing a sprite with a particular
 * animation in a particular direction.
 */
class SpriteAnimationDirection {

 private:

  const int nb_frames;    /**< number of frames in this sequence */
  SDL_Rect *frames;       /**< position of each frame of the sequence on the image */
  SDL_Rect origin;        /**< coordinate of the sprite's origin from the
			   * upper-left corner of its image. */

  PixelBits **pixel_bits; /**< bit masks representing the non-transparent pixels of each frame,
			   * computed only if enable_pixel_collisions() is called */

 public:

  // creation and destruction
  SpriteAnimationDirection(int nb_frames, SDL_Rect *frames,
			   int x_origin, int y_origin);
  virtual ~SpriteAnimationDirection(void);

  // size and origin point
  SDL_Rect& get_size(void);
  SDL_Rect& get_origin(void);

  // frames
  int get_nb_frames(void);
  SDL_Rect& get_frame(int frame);
  void display(SDL_Surface *destination, int x, int y,
	       int current_frame, SDL_Surface *src_image);

  // pixel collisions
  void enable_pixel_collisions(SDL_Surface *src_image);
  bool are_pixel_collisions_enabled(void);
  PixelBits *get_pixel_bits(int frame);
};

#endif
