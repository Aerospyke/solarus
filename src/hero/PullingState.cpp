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
#include "hero/PullingState.h"
#include "hero/GrabbingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::PullingState::PullingState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::PullingState::~PullingState(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::PullingState::start(State *previous_state) {

  State::start(previous_state);

  pulled_entity = NULL;
  sprites->set_animation_pulling();
}

/**
 * @brief Stops this state.
 */
void Hero::PullingState::stop(State *next_state) {

  State::stop(next_state);

  if (is_moving_grabbed_entity()) {
    hero->clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::PullingState::update(void) {

  State::update();

  if (is_moving_grabbed_entity()) { // the hero is pulling an entity and is currently moving it (typically a block)

    // detect when the hero movement is finished
    // because the hero has covered 16 pixels, has reached an obstacle or has aligned the entity on the grid

    PathMovement *movement = (PathMovement*) hero->get_movement();

    bool horizontal = sprites->get_animation_direction() % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && hero->is_x_aligned_to_grid()) || (!horizontal && hero->is_y_aligned_to_grid()));

    if (movement->is_finished() || has_reached_grid) {
      stop_moving_pulled_entity();
    }
  }
  else { // he is not moving an entity

    GameControls *controls = game->get_controls();

    int wanted_direction8 = controls->get_wanted_direction8();
    int opposite_direction8 = (sprites->get_animation_direction8() + 4) % 8;

    // stop pulling if the action key is released or if there is no more obstacle
    if (!controls->is_key_pressed(GameControls::ACTION)
	|| !hero->is_facing_obstacle()) {
      hero->set_state(new FreeState(hero));
    }

    // stop pulling the obstacle if the player changes his direction
    else if (wanted_direction8 != opposite_direction8) {
      hero->set_state(new GrabbingState(hero));
    }

    // see if the obstacle is an entity that the hero can pull
    else {
 
      Detector *facing_entity = hero->get_facing_entity();
      if (facing_entity != NULL) {

	if (facing_entity->get_type() == BLOCK) { // TODO use dynamic linking
	  hero->try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  std::string path = "  ";
	  path[0] = path[1] = '0' + opposite_direction8;

	  hero->set_movement(new PathMovement(path, 8, false, false, false));
	  pulled_entity = facing_entity;
	}
      }
    }
  }
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::PullingState::is_grabbing_or_pulling(void) {
  return true;
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::PullingState::is_moving_grabbed_entity(void) {
  return pulled_entity != NULL;
}

/**
 * @brief Notifies the hero that the entity he is pulling cannot move any more because of a collision.
 */
void Hero::PullingState::notify_grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  static const Rectangle straight_dxy[] = {
    Rectangle( 1, 0),
    Rectangle( 0,-1),
    Rectangle(-1, 0),
    Rectangle( 0, 1)
  };

  // go back one pixel into the direction
  int pulling_direction4 = sprites->get_animation_direction();
  Rectangle bounding_box = hero->get_bounding_box();
  bounding_box.add_xy(straight_dxy[pulling_direction4]);
  hero->set_bounding_box(bounding_box);

  stop_moving_pulled_entity();
}

/**
 * @brief Makes the hero stop pulling the entity he is facing.
 *
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::PullingState::stop_moving_pulled_entity(void) {

  pulled_entity = NULL;
  hero->clear_movement();
  hero->set_state(new GrabbingState(hero));
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::PullingState::can_be_hurt(void) {
  return !is_moving_grabbed_entity();
}

