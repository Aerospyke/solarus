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
#include "hero/StateSwordLoading.h"
#include "hero/StateSpinAttack.h"
#include "hero/StateSwordTapping.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateSwordLoading::StateSwordLoading(Hero *hero):
  StatePlayerMovement(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StateSwordLoading::~StateSwordLoading(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StateSwordLoading::start(State *previous_state) {

  StatePlayerMovement::start(previous_state);

  sword_loaded = false;
  sword_loaded_date = System::now() + 1000;
}

/**
 * @brief Updates this state.
 */
void Hero::StateSwordLoading::update(void) {

  StatePlayerMovement::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();

  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && now >= sword_loaded_date) {
    game->play_sound("sword_spin_attack_load");
    sword_loaded = true;
  }

  GameControls *controls = game->get_controls();

  if (!controls->is_key_pressed(GameControls::SWORD)) {
    // the player has just released the sword key

    // stop loading the sword, go to the normal state or make a spin attack
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      hero->start_free();
    }
    else {
      // the sword is loaded: release a spin attack
      hero->set_state(new StateSpinAttack(hero));
    }
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::StateSwordLoading::set_suspended(bool suspended) {

  StatePlayerMovement::set_suspended(suspended);

  if (!suspended) {
    sword_loaded_date += System::now() - when_suspended;
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::StateSwordLoading::notify_movement_tried(bool success) {

  StatePlayerMovement::notify_movement_tried(success);

  Detector *facing_entity = hero->get_facing_entity();

  if (!success					// the hero has just tried to move unsuccessfuly
      && hero->is_facing_point_on_obstacle()	// he is really facing an obstacle
      && get_wanted_movement_direction8() == sprites->get_animation_direction8()	// he is trying to move towards the obstacle
      && (facing_entity == NULL || !facing_entity->is_sword_ignored())) {		// the obstacle allows him to tap with his sword

    hero->set_state(new StateSwordTapping(hero));
  }
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::StateSwordLoading::set_animation_stopped(void) {
  sprites->set_animation_stopped_sword_loading();
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::StateSwordLoading::set_animation_walking(void) {
  sprites->set_animation_walking_sword_loading();
}

/**
 * @brief Returns whether the animation direction is locked.
 * @return true if the animation direction is locked
 */
bool Hero::StateSwordLoading::is_direction_locked(void) {
  return true;
}

