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
#include "hero/FreeState.h"
#include "hero/GrabbingState.h"
#include "hero/PushingState.h"
#include "hero/HeroSprites.h"
#include "movements/PlayerMovement.h"
#include "entities/Detector.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::FreeState::FreeState(Hero *hero):
  PlayerMovementState(hero) {

}

/**
 * @brief Destructor.
 */
Hero::FreeState::~FreeState(void) {

}

/**
 * @brief Starts this state.
 *
 * This function is called automatically when this state becomes the active state of the hero.
 *
 * @param previous_state the previous state
 */
void Hero::FreeState::start(State *previous_state) {

  PlayerMovementState::start(previous_state);
  pushing_direction4 = -1;
  start_pushing_date = 0;
}

/**
 * @brief Updates this state.
 */
void Hero::FreeState::update(void) {

  PlayerMovementState::update();

  if (!suspended
      && pushing_direction4 != -1					// the hero is trying to push
      && get_wanted_movement_direction8() != pushing_direction4 * 2) {	// but his movement direction has changed

    pushing_direction4 = -1; // stop trying to push
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::FreeState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (!suspended) {
    start_pushing_date += System::now() - when_suspended;
  }
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::FreeState::action_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();

  if (keys_effect->is_action_key_acting_on_facing_entity()) {

    // action on the facing entity
    hero->get_facing_entity()->action_key_pressed();
  }
  else if (hero->is_facing_point_on_obstacle()) {

    // grab an obstacle
    hero->set_state(new GrabbingState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::FreeState::notify_movement_tried(bool success) {

  PlayerMovementState::notify_movement_tried(success);

  if (!success // the hero has just tried to move unsuccessfuly
      &&  hero->is_facing_point_on_obstacle()) { // he is really facing an obstacle

    uint32_t now = System::now();
    if (pushing_direction4 == -1) { // we start counting to trigger animation "pushing"
      start_pushing_date = now + 800; // start animation "pushing" after 800 ms
      pushing_direction4 = hero->get_animation_direction();
    }
    else if (now >= start_pushing_date) {
      hero->set_state(new PushingState(hero));
    }
  }
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 * @return true
 */
bool Hero::FreeState::is_free(void) {
  return true;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::FreeState::can_start_sword(void) {
  return true;
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::FreeState::can_take_stairs(void) {
  return true;
}

/**
 * @brief Returns whether an item previously carried by the hero should be thrown when this state starts.
 * @return true if an item previously carried by the hero should be thrown when this state starts
 */
bool Hero::FreeState::can_throw_item(void) {
  return false;
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::FreeState::set_animation_stopped(void) {
  hero->get_sprites()->set_animation_stopped_normal();
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::FreeState::set_animation_walking(void) {
  hero->get_sprites()->set_animation_walking_normal();
}

