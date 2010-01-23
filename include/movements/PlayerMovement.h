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
#ifndef SOLARUS_PLAYER_MOVEMENT
#define SOLARUS_PLAYER_MOVEMENT

#include "Common.h"
#include "movements/SmoothCollisionMovement.h"

/**
 * Abstract class for an 8-direction movement controlled by the player.
 */
class PlayerMovement: public SmoothCollisionMovement {

 protected:

  // movement

  /**
   * True if the entity is moving.
   */
  bool started;

  /**
   * Speed of the entity when it is moving.
   */
  int moving_speed;

  /**
   * True if the player can move the entity, i.e. if the game is not interrupted
   * and the entity is in a state such that the player has the control.
   */
  bool moving_enabled;
  bool moving_enabled_before_suspended;

  /**
   * True if the direction arrows pressed are taken into account.
   */
  bool direction_enabled;
  bool direction_enabled_before_suspended;

  // keyboard

  /**
   * Indicates which ones of the 4 arrows are currently pressed by the player.
   * This bit field is an OR combination of right_mask, up_mask, left_mask and down_mask.
   */
  uint16_t direction_mask;

  static const uint16_t direction_masks[4];

  void add_direction_mask(uint16_t direction_mask);
  void remove_direction_mask(uint16_t direction_mask);
  void set_direction_mask(uint16_t direction_mask);

 public:

  // creation and destruction
  PlayerMovement(int speed);
  ~PlayerMovement(void);

  // direction
  int get_direction(void);
  uint16_t get_direction_mask(void);

  void add_direction(int direction);
  void remove_direction(int direction);

  // movement
  bool is_moving_enabled(void);
  void set_moving_enabled(bool moving_enabled, bool direction_enabled);
  int get_moving_speed(void);
  void set_moving_speed(int moving_speed);
  bool is_direction_enabled(void);

  bool is_started(void);
  void set_suspended(bool suspended);

  void compute_movement(void);
};

#endif

