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
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "entities/Teletransporter.h"
#include "entities/MapEntities.h"
#include "entities/Enemy.h"
#include "entities/ConveyorBelt.h"
#include "movements/PlayerMovement.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "movements/TargetMovement.h"
#include "movements/PathMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Equipment.h"
#include "Sprite.h"
#include "Treasure.h"
#include "Controls.h"
#include "InventoryItem.h"
using namespace std;

/**
 * Returns the hero's state.
 * @return the state of the hero
 */
Hero::State Hero::get_state(void) {
  return state;
}

/**
 * Sets the hero's state.
 * @param state the state of the hero
 */
void Hero::set_state(State state) {

  this->state = state;

  if (!zsdx->game->is_suspended()) {
    get_normal_movement()->set_moving_enabled(state < PUSHING, state <= CONVEYOR_BELT);
  }
}

/**
 * Returns whether the animation direction is locked.
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns false, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 * @return true if the animation direction is locked
 */
bool Hero::is_direction_locked(void) {
  return state == SWORD_LOADING || state == PUSHING || state == PULLING || state == SWORD_TAPPING;
}

/**
 * Sets the ground displayed under the hero.
 * @param ground the ground to display
 */
void Hero::set_ground(Ground ground) {

  if (ground != this->ground) {
    this->ground = ground;
  }
}

/**
 * Starts displaying the ground specified by the last set_ground() call.
 */
void Hero::start_ground(void) {

  switch (ground) {

  case GROUND_NORMAL:
    // normal ground: remove any special sprite displayed under the hero
    delete ground_sprite;
    ground_sprite = NULL;
    get_normal_movement()->set_moving_speed(walking_speed);
    break;

  case GROUND_DEEP_WATER:
    // deep water: plunge if the hero is not jumping
    if (state != JUMPING && state != HURT) {
      start_deep_water();
    }
    break;

  case GROUND_HOLE:
    // hole: attract the hero towards the hole
    if (state != JUMPING) {
      start_hole();
    }
    break;

  case GROUND_SHALLOW_WATER:
  case GROUND_GRASS:
    {
      // display a special sprite below the hero
      ground_sprite = new Sprite(ground_sprite_ids[ground - 1]);

      if (ground != GROUND_SHALLOW_WATER) {
	ground_sprite->set_current_animation(walking ? "walking" : "stopped");
      }

      Uint32 now = SDL_GetTicks();
      next_ground_date = MAX(next_ground_date, now);
      ground_sound = ResourceManager::get_sound(ground_sound_ids[ground - 1]);
    }
    break;
  }
}

/**
 * Updates the ground under the hero.
 */
void Hero::update_ground(void) {

  // see if it's time to do something (depending on the ground)
  Uint32 now = SDL_GetTicks();
  if (now >= next_ground_date) {

    if (is_ground_visible()) {
      // time to play a sound
      if (walking && state <= SWORD_LOADING) {
	ground_sound->play();
      }

      next_ground_date = now + 300;
    }

    if (ground == GROUND_HOLE &&
	state != FALLING && state != RETURNING_TO_SOLID_GROUND && state != JUMPING) {

      if (get_distance(last_solid_ground_coords.x, last_solid_ground_coords.y) >= 8) {
	start_falling();
      }
      else {

	// time to move the hero on a hole one more pixel away from the solid ground
	SDL_Rect collision_box = get_position_in_map();
	collision_box.x += hole_dx;
	collision_box.y += hole_dy;

	if (!map->collision_with_obstacles(get_layer(), collision_box, this)) {
	  set_position_in_map(collision_box);
	  just_moved();
	}
	next_ground_date = now + 60;
      }
    }
  }
}

/**
 * Returns whether the hero is in a state such that
 * a ground can be displayed under him.
 */
bool Hero::is_ground_visible(void) {

  return (ground == GROUND_GRASS || ground == GROUND_SHALLOW_WATER)
    && state != PLUNGING && state != SWIMMING && state != JUMPING
    && state != HURT && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * This function is called when a teletransporter detects a collision with the hero.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void Hero::collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {

  if (state != JUMPING) {

    if (ground == GROUND_HOLE) {
      this->hole_teletransporter = teletransporter; // fall first, transport later
    }
    else {
      teletransporter->transport_hero(this);
    }
  }
}

/**
 * This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void Hero::collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy) {

  on_conveyor_belt = true;

  if (state != JUMPING && state != CONVEYOR_BELT && state != PUSHING) {

    // check that a significant part of the hero is on the conveyor belt
    SDL_Rect center = get_center_point();
    center.x -= 1;
    center.y -= 1;
    center.w = 2;
    center.h = 2;

    if (conveyor_belt->overlaps(center)) {

      // check that the movement is possible for at least one pixel
      SDL_Rect collision_box = get_position_in_map();
      collision_box.x += dx;
      collision_box.y += dy;
 
      if (!map->collision_with_obstacles(get_layer(), collision_box, this)) {

	// remove the carried item
	if (state == CARRYING) {
	  start_throwing();
	}

	// set the state
	this->current_conveyor_belt = conveyor_belt;
	set_state(CONVEYOR_BELT);
	set_animation_stopped();
	zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

	// set the movement
	conveyor_belt_snapping = true;
	set_movement(new TargetMovement(conveyor_belt, walking_speed));
      }
    }
  }
}

/**
 * Updates the hero when he is on a conveyor belt.
 * The state should be CONVEYOR_BELT.
 */
void Hero::update_conveyor_belt(void) {

  get_movement()->update();
  if (conveyor_belt_snapping && get_movement()->is_finished()) {
    clear_movement();
    conveyor_belt_snapping = false;
    std::string path = "  ";
    path[0] = path[1] = '0' + current_conveyor_belt->get_sprite()->get_current_direction();
    set_movement(new PathMovement(path, walking_speed * 2 / 3, false, true, false));
  }
  else {
    if (get_movement()->is_finished() || !on_conveyor_belt) {

      this->current_conveyor_belt = NULL;
      clear_movement();
      set_movement(normal_movement);
      start_free();
    }
    on_conveyor_belt =  false;
  }
}

/**
 * Lets the hero walk.
 * Moves to the state FREE and updates the animations accordingly.
 */
void Hero::start_free(void) {

  set_state(FREE);

  get_normal_movement()->compute_movement();
  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }

  // to check the facing entity and the ground
  just_moved();
}

/**
 * Makes the hero swing his sword if this action is possible.
 * The game should not be suspended.
 * Moves to the state SWORD_SWINGING, plays the sword sound
 * and updates the animations accordingly.
 */
void Hero::start_sword(void) {
  if (can_start_sword()) {

    // remove the carried item
    if (state == CARRYING) {
      start_throwing();
    }

    set_state(SWORD_SWINGING);
    sword_sound->play();
    set_animation_sword();
  }
}

/**
 * Returns whether the hero can swing his sword right now.
 * The function returns true if the game is not suspended and the hero
 * is in state FREE, PUSHING,
 * CARRYING or SWORD_SWINGING.
 * @return true if the hero can swing his sword, false otherwise
 */
bool Hero::can_start_sword(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

  return !zsdx->game->is_suspended()
    && (state <= CARRYING || state == SWORD_SWINGING)
    && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD;
}

/**
 * This function is called repeatedly while the hero is swinging his sword.
 * The state must be SWORD_SWINGING.
 */
void Hero::update_sword_swinging(void) {

  if (tunic_sprite->is_animation_finished()) {

    // if the player is still pressing the sword key, start loading the sword
    if (zsdx->game->get_controls()->is_key_pressed(Controls::SWORD)) {
      start_sword_loading();
    }
    else {
      start_free();
    }
  }
}

/**
 * Lets the hero load his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_sword_loading(void) {
  set_state(SWORD_LOADING);

  // initialize the counter to detect when the sword is loaded
  counter = 0;
  next_counter_date = SDL_GetTicks();
  sword_loaded = false;

  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }
}

/**
 * This function is called repeatedly while the hero is loading his sword.
 * It stops the loading if the sword key is released.
 * The state must be SWORD_LOADING.
 */
void Hero::update_sword_loading(void) {

  Uint32 now = SDL_GetTicks();
  while (!sword_loaded && now >= next_counter_date) {
    counter++;
    next_counter_date += 100;
  }

  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && counter >= 10) {
      ResourceManager::get_sound("sword_spin_attack_load")->play();
      sword_loaded = true;
      counter = 0;
  }

  Controls *controls = zsdx->game->get_controls();

  if (!controls->is_key_pressed(Controls::SWORD)) {
    // the player just released the sword key

    // stop loading the sword, go to normal state or spin attack
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      start_free();
    }
    else {
      // the sword is loaded: release a spin attack
      start_spin_attack();
    }
  }
}

/**
 * Makes the hero tap the wall he is facing with his sword.
 * Moves to the state SWORD_TAPPING
 * and updates the animations accordingly.
 */
void Hero::start_sword_tapping(void) {
  set_state(SWORD_TAPPING);
  set_animation_sword_tapping();
  next_hit_sound_date = SDL_GetTicks() + 100;
}

/**
 * This function is called repeatedly while the hero is tapping a wall with his sword.
 * The state must be SWORD_TAPPING.
 */
void Hero::update_sword_tapping(void) {

  Controls *controls = zsdx->game->get_controls();
  const SDL_Rect &facing_point = get_facing_point();

  if (!controls->is_key_pressed(Controls::SWORD)
      || get_movement_direction() != get_animation_direction() * 90
      || !map->collision_with_obstacles(get_layer(), facing_point.x, facing_point.y, this)) {
    // the sword key has been released, the player has moved or the obstacle is gone

    if (tunic_sprite->get_current_frame() >= 5) {
      // stop tapping the wall, go back to state SWORD_LOADING
      start_sword_loading();
    }
  }
  else {
    Uint32 now = SDL_GetTicks();
    if (tunic_sprite->get_current_frame() == 3 && now >= next_hit_sound_date) {
      ResourceManager::get_sound("sword_hit")->play();
      next_hit_sound_date = now + 100;
    }
  }
}

/**
 * Makes the hero lift a destructible item.
 * @param item_to_lift the destructible item to lift
 */
void Hero::start_lifting(DestructibleItem *item_to_lift) {

  // create the corresponding carried item
  this->lifted_item = new CarriedItem(this, item_to_lift);
  lifted_item->set_map(map);

  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  set_state(LIFTING);
  set_animation_lifting();
  set_facing_entity(NULL);
}

/**
 * Makes the hero carry the item he was lifting.
 */
void Hero::start_carrying(void) {
  set_state(CARRYING);

  if (get_normal_movement()->is_started()) {
    set_animation_walking();
  }
  else {
    set_animation_stopped();
  }

  // action icon "throw"
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
}

/**
 * Removes the item carried by the hero, without throwing it.
 * Change the hero state after calling this function.
 */
void Hero::stop_carrying(void) {
  delete lifted_item;
  lifted_item = NULL;
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

/**
 * Makes the hero throw the item he was carrying.
 */
void Hero::start_throwing(void) {

  // remove the "throw" icon
  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  // we check the state because the "throw" icon is actually shown as soon as
  // the hero starts lifting the item
  if (state == CARRYING) {

    lifted_item->throw_item(map, get_animation_direction());

    if (thrown_item != NULL) {
      delete thrown_item;
    }

    thrown_item = lifted_item;
    lifted_item = NULL;
    start_free();
  }
}

/**
 * Suspends or resumes the animation of the hero's carried items.
 * This function is called when the game is suspended or resumed.
 * @param suspended true to suspend the game, false to resume it
 */
void Hero::set_suspended_carried_items(bool suspended) {

  if (lifted_item != NULL) {
    lifted_item->set_suspended(suspended);
  }

  if (thrown_item != NULL) {
    thrown_item->set_suspended(suspended);
  }
}

/**
 * Updates the carried items.
 */
void Hero::update_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->update();
  }
  else { // no more lifted item: remove the "throw" icon if it is still here
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_THROW) {
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
  }

  if (!suspended && thrown_item != NULL) {
    thrown_item->update();
    if (thrown_item->is_broken()) {
      delete thrown_item;
      thrown_item = NULL;
    }
  }
}

/**
 * Displays the carried items.
 */
void Hero::display_carried_items(void) {

  if (lifted_item != NULL) {
    lifted_item->display_on_map();
  }

  if (thrown_item != NULL) {
    thrown_item->display_on_map();
  }
}

/**
 * Deletes the carried items.
 */
void Hero::destroy_carried_items(void) {

  if (lifted_item != NULL) {
    delete lifted_item;
    lifted_item = NULL;
  }

  if (thrown_item != NULL) {
    delete thrown_item;
    thrown_item = NULL;
  }
}

/**
 * Makes the hero push something.
 * Moves to the state PUSHING and updates the animations accordingly.
 */
void Hero::start_pushing(void) {
  set_state(PUSHING);
  set_animation_pushing();
  pushing_direction_mask = get_normal_movement()->get_direction_mask();
}

/**
 * Updates the hero pushing, whether
 * his state is already PUSHING or not.
 */
void Hero::update_pushing(void) {

  // no change when the game is suspended
  if (zsdx->game->is_suspended()) {
    return;
  }

  // get the direction of the arrows
  Uint16 direction_mask = get_normal_movement()->get_direction_mask();

  if (move_tried) {
    // the hero is trying to move

    // see if the move has failed (i.e. if the hero's coordinates have not changed)
    if (get_x() == old_x && get_y() == old_y) {

      // the hero is facing an obstacle

      if (state == FREE) { // is state FREE: see when we can start animation "pushing"

	Uint32 now = SDL_GetTicks();
	if (pushing_direction_mask == 0xFFFF) { // we start counting to trigger animation "pushing"
	  counter = 0;
	  next_counter_date = now;
	  pushing_direction_mask = direction_mask;
	}

	// increment the counter every 100 ms
	while (now >= next_counter_date && counter < 8) {
	  counter++;
	  next_counter_date += 100;
	}

	if (counter >= 8) {
	  start_pushing(); // start animation "pushing" when the counter reaches 8
	}
      }
      else if (state == SWORD_LOADING
	       && get_movement_direction() == get_animation_direction() * 90
	       && (facing_entity == NULL || !facing_entity->is_sword_ignored())) {
	// in state SWORD_LOADING: hit the wall with the sword
	pushing_direction_mask = direction_mask;
	start_sword_tapping();
      }
    }
    else if (state == FREE) {
      // the hero has just moved successfuly: reset the counter
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }
  }
  else {

    // stop trying to push if the player changes his direction
    if (pushing_direction_mask != 0xFFFF && // the hero is pushing or about to push
	direction_mask != pushing_direction_mask) {

      // reset the counter
      counter = 0;
      pushing_direction_mask = 0xFFFF;
    }

    // if the hero is pushing an obstacle
    if (state == PUSHING && !is_moving_grabbed_entity()) {

      int straight_direction = get_animation_direction();

      // stop pushing if there is no more obstacle
      if (!is_facing_obstacle()) {
	start_free();
      }

      // stop pushing if the player changes his direction
      if (get_movement_direction() != straight_direction * 90) {
	start_grabbing();
      }

      // see if the obstacle is an entity that the hero can push
      else if (facing_entity != NULL && grabbed_entity == NULL) {

	if (facing_entity->get_type() == BLOCK) {
	  try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  string path = "  ";
	  int direction = get_animation_direction();
	  path[0] = path[1] = '0' + direction * 2;

	  set_movement(new PathMovement(path, 8, false, true, false));
	  grabbed_entity = facing_entity;
	}
      }
    }
  }
}

/**
 * Makes the hero grab the object he is facing.
 * @param delay delay before the hero can push or pull 
 * the entity he is facing (if any)
 */
void Hero::start_grabbing(void) {
  stop_displaying_sword();
  set_state(GRABBING);
  set_animation_grabbing();
  grabbed_entity = NULL;
}

/**
 * Makes the hero pull the object he is grabbing.
 */
void Hero::start_pulling(void) {
  set_state(PULLING);
  set_animation_pulling();
}

/**
 * This function is called repeatedly while the hero is grabbing something.
 * The state must be GRABBING or PULLING.
 */
void Hero::update_grabbing_pulling(void) {

  Controls *controls = zsdx->game->get_controls();

  // the hero is grabbing an obstacle
  if (state == GRABBING) {
    int keys_direction = get_normal_movement()->get_direction();
    int sprite_direction = get_animation_direction() * 90;

    // push the obstacle
    if (keys_direction == sprite_direction) {
      start_pushing();
    }

    // pull the obstacle
    else if (keys_direction == (sprite_direction + 180) % 360) {
      start_pulling();
    }

    // release the obstacle
    if (!is_moving_grabbed_entity()) {
      if (!controls->is_key_pressed(Controls::ACTION)) {
	start_free();
      }
    }
  }

  // the hero is pulling an obstacle
  if (state == PULLING && !is_moving_grabbed_entity()) {

    int opposite_direction = (get_animation_direction() + 2) % 4;

    // stop pulling the obstacle if the player changes his direction
    if (get_movement_direction() != opposite_direction * 90) {
      start_grabbing();
    }

    // stop pulling if the action key is released or if there is no more obstacle
    if (!controls->is_key_pressed(Controls::ACTION) ||
	!is_facing_obstacle()) {
      start_free();
    }

    // see if the obstacle is an entity that the hero can pull
    if (facing_entity != NULL) {

      if (facing_entity->get_type() == BLOCK) {
	try_snap_to_facing_entity();
      }

      if (facing_entity->moved_by_hero()) {

	string path = "  ";
	int opposite_direction = (get_animation_direction() + 2) % 4;
	path[0] = path[1] = '0' + opposite_direction * 2;

	set_movement(new PathMovement(path, 8, false, true, false));
	grabbed_entity = facing_entity;
      }
    }
  }
}

/**
 * This function is called repeatedly in order to update the hero when he
 * is pushing or pulling the entity he is grabbing.
 */
void Hero::update_moving_grabbed_entity(void) {

  // detect when the hero movement is finished
  // because the hero has covered 16 pixels or has reached an obstacle
  if (is_moving_grabbed_entity() && get_movement()->is_finished()) {
    stop_moving_grabbed_entity();
  }
}

/**
 * Returns whether the hero is moving the entity he is grabbing.
 * @return true if the hero is moving the entity he is grabbing
 */
bool Hero::is_moving_grabbed_entity(void) {
  return grabbed_entity != NULL;
}

/**
 * Returns whether the hero is in state GRABBING or PULLING.
 * @return true if the state is GRABBING or PULLING
 */
bool Hero::is_grabbing_or_pulling(void) {
  return state == GRABBING || state == PULLING;
}

/**
 * Notifies the hero that the entity he is pushing or pulling
 * cannot move any more because of a collision.
 */
void Hero::grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  int direction_back = get_animation_direction();

  if (state == PUSHING) {
    direction_back = (direction_back + 2) % 4;
  }

  // go back one pixel in that direction
  switch (direction_back) {

  case 0:
    set_x(get_x() + 1);
    break;

  case 1:
    set_y(get_y() - 1);
    break;

  case 2:
    set_x(get_x() - 1);
    break;

  case 3:
    set_y(get_y() + 1);
    break;

  }

  stop_moving_grabbed_entity();
}

/**
 * Makes the hero stop pushing or pulling the entity he is grabbing.
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::stop_moving_grabbed_entity(void) {
  clear_movement();
  set_movement(normal_movement);

  Controls *controls = zsdx->game->get_controls();
  if (state == PUSHING && !controls->is_key_pressed(Controls::ACTION)) {
    // the hero was pushing an entity without grabbing it
    grabbed_entity = NULL;

    // stop the animation pushing if his direction changed
    int straight_direction = get_animation_direction();
    if (get_movement_direction() != straight_direction * 90) {
      start_free();
    }
  }
  else {
    start_grabbing();
  }
}

/**
 * Forbids the hero to move until unfreeze() is called.
 * The current animation of the hero's sprites is stopped and the "stopped" animation is played.
 */
void Hero::freeze(void) {
  get_normal_movement()->set_moving_enabled(false, false);
  set_animation_stopped();
  destroy_carried_items();
  zsdx->game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  set_state(FREEZED);
}

/**
 * Lets the hero move again after a freeze() call.
 */
void Hero::unfreeze(void) {
  start_free();
}

/**
 * Makes the hero brandish a treasure.
 * @param treasure the treasure to give him (you have to delete it after the hero brandishes it)
 */
void Hero::give_treasure(Treasure *treasure) {

  this->treasure = treasure;

  // goto the right state
  stop_carrying();
  set_state(BRANDISHING_TREASURE);

  // show the animation
  save_animation_direction();
  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(0);

  // the shield and the sword are not visible when the hero is brandishing a treasure
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();
}

/**
 * Updates the hero when he is brandishing a treasure.
 */
void Hero::update_treasure(void) {

  if (!zsdx->game->is_giving_treasure()) {

    /* The treasure message is over: if the treasure was a tunic,
     * a sword or a shield, then we must reload the hero's sprites now
     */
    Treasure::Content content = treasure->get_content();
    if (content >= Treasure::BLUE_TUNIC && content <= Treasure::SWORD_4) {
      rebuild_equipment();
    }

    // restore the hero's state
    treasure = NULL;
    restore_animation_direction();
    start_free();
  }
}

/**
 * Displays the treasure the hero is brandishing.
 */
void Hero::display_treasure(void) {

  int x = position_in_map.x;
  int y = position_in_map.y;

  const SDL_Rect &camera_position = map->get_camera_position();
  treasure->display(map->get_visible_surface(),
		    x - camera_position.x,
		    y - 24 - camera_position.y);
}

/**
 * Makes the hero load his sword.
 * Moves to the state SWORD_LOADING
 * and updates the animations accordingly.
 */
void Hero::start_spin_attack(void) {
  set_state(SPIN_ATTACK);
  sword_loaded = false;

  // play the sound
  ResourceManager::get_sound("sword_spin_attack_release")->play();

  // start the animation
  tunic_sprite->set_current_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {
    shield_sprite->stop_animation(); // the shield is not visible during a spin attack
  }
}

/**
 * This function is called repeatedly during the spin attack.
 * The state must be SPIN_ATTACK.
 */
void Hero::update_spin_attack(void) {

  if (tunic_sprite->is_animation_finished()) {
    start_free();
  }
}

/**
 * Makes the hero jump in a direction.
 * While he is jumping, the player does not control him anymore.
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 */
void Hero::start_jumping(int direction, int length, bool with_collisions, bool with_sound) {
  start_jumping(direction, length, with_collisions, with_sound, get_layer());
}

/**
 * Makes the hero jump in a direction.
 * While he is jumping, the player does not control him anymore.
 * @param direction direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to stop the movement if there is a collision
 * @param with_sound true to play the "jump" sound
 * @param layer_after_jump the layer to set when the jump is finished
 */
void Hero::start_jumping(int direction, int length, bool with_collisions, bool with_sound, Layer layer_after_jump) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  stop_displaying_sword();

  // jump
  set_state(JUMPING);
  set_movement(new JumpMovement(direction, length, with_collisions));
  set_animation_jumping();

  if (with_sound) {
    ResourceManager::get_sound("jump")->play();
  }
  jump_y = get_y();
  this->layer_after_jump = layer_after_jump;
}

/**
 * Updates the jump action.
 */
void Hero::update_jumping(void) {

  JumpMovement *movement = (JumpMovement*) get_movement();
  movement->update();
  jump_y = get_y() - movement->get_jump_height();

  if (movement->is_finished()) {

    map->get_entities()->set_entity_layer(this, layer_after_jump);
    clear_movement();
    set_movement(normal_movement);

    if (ground == GROUND_DEEP_WATER) {
      start_deep_water();
    }
    else {
      start_free();
    }
  }
}

/**
 * Returns whether the hero can be hurt.
 * @return true if the hero can be hurt in its current state
 */
bool Hero::can_be_hurt(void) {
  return state <= SPIN_ATTACK && !tunic_sprite->is_blinking() && get_movement() == normal_movement;
}

/**
 * Hurts the hero if possible.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::hurt(MapEntity *source, int life_points, int magic_points) {

  if (can_be_hurt()) {

    // remove the carried item
    if (state == LIFTING || state == CARRYING) {
      start_throwing();
    }
    stop_displaying_sword();

    ResourceManager::get_sound("hero_hurt")->play();

    life_points = MAX(1, life_points / (equipment->get_tunic() + 1));

    equipment->remove_hearts(life_points);
    if (magic_points > 0 && equipment->get_magic() > 0) {
      equipment->remove_magic(magic_points);
      ResourceManager::get_sound("magic_bar")->play();
    }
    blink();
    set_state(HURT);
    set_animation_hurt();

    double angle = source->get_vector_angle(this);
    set_movement(new StraightMovement(12, angle, 200));
  }
}

/**
 * Updates the HURT state.
 */
void Hero::update_hurt(void) {

  get_movement()->update();
  if (get_movement()->is_finished()) {

    clear_movement();
    set_movement(normal_movement);

    if (ground == GROUND_DEEP_WATER) {
      start_deep_water();
    }
    else {
      start_free();
    }
  }
}

/**
 * Returns whether the hero is in a state such that the game over
 * sequence can start.
 * @return true if the game over sequence can start
 */
bool Hero::can_start_gameover_sequence(void) {
  return state != HURT && state != PLUNGING && state != FALLING && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * This function is called when the hero was dead but saved by a fairy.
 */
void Hero::get_back_from_death(void) {
  blink();
  start_free();
  when_suspended = SDL_GetTicks();
}

/**
 * This function is called when an enemy collides with the hero.
 * @param enemy the enemy
 */
void Hero::collision_with_enemy(Enemy *enemy) {
  enemy->attack_hero(this);
}

/**
 * This function is called when an enemy's sprite collides with a sprite of the hero.
 * @param enemy the enemy
 * @param sprite_overlapping the hero sprite that collides with the enemy
 */
void Hero::collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping) {

  if (sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {
    enemy->try_hurt(ATTACK_SWORD, this);
  }
}

/**
 * Notifies this entity that it has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was just ignored 
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 */
void Hero::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result) {

  if (result == 0) {
    return;
  }

  switch (attack) {

  case ATTACK_SWORD:
    if (get_state() == SWORD_LOADING) {
      stop_displaying_sword();
      start_free();
    }
    break;

  default:
    DIE("Unknown attack '" << attack << "'");
  }
}

/**
 * Makes the hero drown or swim.
 */
void Hero::start_deep_water(void) {

  // stop the sword
  stop_displaying_sword();

  // throw any carried item
  if (state == CARRYING) {
    start_throwing();
  }

  if (state == JUMPING || state == HURT) {
    // plunge into the water
    start_plunging();
  }
  else {
    // move to state swimming or jumping
    if (equipment->has_inventory_item(INVENTORY_FLIPPERS)) {
      start_swimming();
    }
    else {
      start_jumping(get_movement_direction() / 45, 32, true, true);
      ((JumpMovement*) get_movement())->set_delay(13);
    }
  }
}

/**
 * Makes the hero plunge into the water.
 */
void Hero::start_plunging(void) {
  set_state(PLUNGING);
  set_animation_plunging();
  ResourceManager::get_sound("splash")->play();
}

/**
 * Updates the PLUNGING state.
 */
void Hero::update_plunging(void) {

  if (tunic_sprite->is_animation_finished()) {

    if (ground != GROUND_DEEP_WATER) {
      start_free();
    }
    else if (equipment->has_inventory_item(INVENTORY_FLIPPERS)) {
      start_swimming();
    }
    else {
      ResourceManager::get_sound("message_end")->play();
      start_returning_to_solid_ground(last_solid_ground_coords);
      equipment->remove_hearts(1);
    }
  }
}

/**
 * Makes the hero swim.
 */
void Hero::start_swimming(void) {
  set_state(SWIMMING);
  // TODO
}

/**
 * Makes the hero stop swimming.
 */
void Hero::stop_swimming(void) {
  start_free();
}

/**
 * Makes the hero move towards a hole of fall into it.
 */
void Hero::start_hole(void) {

  next_ground_date = SDL_GetTicks();
  hole_teletransporter = NULL;

  // push the hero into a direction
  if (is_moving_towards(0)) {
    hole_dx = 1;
  }
  else if (is_moving_towards(2)) {
    hole_dx = -1;
  }
  else {
    hole_dx = 0;
  }

  if (is_moving_towards(1)) {
    hole_dy = -1;
  }
  else if (is_moving_towards(3)) {
    hole_dy = 1;
  }
  else {
    hole_dy = 0;
  }

  get_normal_movement()->set_moving_speed(3);
}

/**
 * Makes the hero fall into a hole.
 */
void Hero::start_falling(void) {

  // remove the carried item
  if (state == CARRYING) {
    start_throwing();
  }

  save_animation_direction();
  set_state(FALLING);
  set_animation_falling();
  ResourceManager::get_sound("hero_falls")->play();
}

/**
 * Updates the FALLING state.
 */
void Hero::update_falling(void) {

  if (tunic_sprite->is_animation_finished()) {

    // the hero has just finished falling
    if (hole_teletransporter != NULL) {
      // special hole with a teletransporter
      hole_teletransporter->transport_hero(this);
    }
    else {
      // hole that hurts the hero
      if (target_solid_ground_coords.x != -1) {
	// go back to a target point specified earlier
	start_returning_to_solid_ground(target_solid_ground_coords);
      }
      else {
	// nothing was specified: just go back to the last solid ground location
	start_returning_to_solid_ground(last_solid_ground_coords);
      }
   
      equipment->remove_hearts(2);
      set_animation_stopped();
      restore_animation_direction();
    }
  }
  else if (get_movement() != normal_movement) {
    // delete a possible hurt movement
    clear_movement();
    set_movement(normal_movement);
  }
}

/**
 * Specifies a point of the map where the hero will go back if he falls
 * into a hole or some other bad ground.
 * This function is usually called when the hero walks on a special sensor.
 * @param target_solid_ground_coords coordinates of the position where
 * the hero will go if he falls into a hole or some other bad ground
 */
void Hero::set_target_solid_ground_coords(const SDL_Rect &target_solid_ground_coords) {
  this->target_solid_ground_coords = target_solid_ground_coords;
}

/**
 * Hides the hero and makes him move back to the specified solid ground location.
 * @param target coordinates of the solid ground location
 */
void Hero::start_returning_to_solid_ground(const SDL_Rect &target) {
  map->get_entities()->remove_boomerang();
  set_movement(new TargetMovement(target.x, target.y, walking_speed));
  set_state(RETURNING_TO_SOLID_GROUND);
}

/**
 * Updates the RETURNING_TO_SOLID_GROUND state.
 */
void Hero::update_returning_to_solid_ground(void) {

  // the current movement is an instance of TargetMovement
  get_movement()->update();

  if (get_movement()->is_finished()) {
    clear_movement();
    set_movement(normal_movement);
    blink();
    start_free();
  }
}

/**
 * Returns whether the hero can use an item from
 * the inventory now.
 * @param item_id id of the item to check
 */
bool Hero::can_start_inventory_item(InventoryItemId item_id) {
  return state == FREE && InventoryItem::can_be_assigned(item_id)
    && equipment->has_inventory_item(item_id)
    && SDL_GetTicks() >= when_can_use_inventory_item;
}

/**
 * Starts using an item from the inventory.
 * @param item the item to use.
 */
void Hero::start_inventory_item(InventoryItemId item_id) {

  this->current_inventory_item = new InventoryItem(item_id);
  this->when_can_use_inventory_item = SDL_GetTicks() + 500;
  set_state(USING_INVENTORY_ITEM);
  current_inventory_item->start(zsdx->game);
}

/**
 * Updates the USING_INVENTORY_ITEM state.
 */
void Hero::update_inventory_item(void) {

  current_inventory_item->update();
  if (current_inventory_item->is_finished()) {
    delete current_inventory_item;
    current_inventory_item = NULL;

    if (get_state() == USING_INVENTORY_ITEM) {
      // if the state was not modified by the item, return to the normal state
      start_free();
    }
  }
}


/**
 * Updates the FREEZED state.
 */
void Hero::update_freezed(void) {
}

