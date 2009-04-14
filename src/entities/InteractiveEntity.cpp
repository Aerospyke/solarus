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
#include "entities/InteractiveEntity.h"
#include "entities/Hero.h"
#include "movements/PathMovement.h"
#include "movements/RandomWalkMovement.h"
#include "movements/JumpMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"
#include "Sprite.h"
#include "Equipment.h"

/**
 * Action icon depending on the type of interaction.
 */
const KeysEffect::ActionKeyEffect InteractiveEntity::action_key_effects[] = {

  KeysEffect::ACTION_KEY_LOOK,
  KeysEffect::ACTION_KEY_SPEAK, // NPC
  KeysEffect::ACTION_KEY_LOOK,
  KeysEffect::ACTION_KEY_LOOK,
};

/**
 * Indicates the direction of an NPC's animation (from 0 to 3)
 * depending on the movement direction
 */
const int InteractiveEntity::animation_directions[] = {
  0,
  0,
  1,
  2,
  2,
  2,
  3,
  0
};

/**
 * Creates an interactive entity.
 * @param name name identifying this interactive entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param subtype the subtype of interaction
 * @param y y coordinate of the entity to create
 * @param sprite_name sprite animation set of the entity, or "_none" to create no sprite
 * @param initial_direction direction of the entity's sprite (only used if the entity has a sprite)
 * @param message_to_show id of the message to show when the player presses the action key in front
 * of this entity, or "_none" to call the script instead (with an event_interaction() call)
 */
InteractiveEntity::InteractiveEntity(std::string name, Layer layer, int x, int y,
				     Subtype subtype, SpriteAnimationSetId sprite_name,
				     int initial_direction, MessageId message_to_show):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 0, 0),
  subtype(subtype), message_to_show(message_to_show) {

  switch (subtype) {

  case CUSTOM:
    initialize_sprite(sprite_name, 0); // the direction is ignored
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case NON_PLAYING_CHARACTER:
    initialize_sprite(sprite_name, initial_direction);
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case SIGN:
    create_sprite("entities/sign");
    set_size(16, 16);
    set_origin(8, 13);
    break;

  case WATER_FOR_BOTTLE:
    set_size(16, 16);
    break;
  }
}

/**
 * Destructor.
 */
InteractiveEntity::~InteractiveEntity(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType InteractiveEntity::get_type() {
  return INTERACTIVE_ENTITY;
}

/**
 * Creates the sprite specified.
 * @param sprite_name sprite animation set of the entity, or "_none" to create no sprite
 * @param initial_direction direction of the entity's sprite (ignored if there is no sprite)
 */
void InteractiveEntity::initialize_sprite(SpriteAnimationSetId sprite_name, int initial_direction) {

  if (sprite_name != "_none") {
    create_sprite(sprite_name);
    get_sprite()->set_current_direction(initial_direction);
  }
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool InteractiveEntity::is_obstacle_for(MapEntity *other) {

  if (other->get_type() != INTERACTIVE_ENTITY) {
    return true;
  }

  return subtype != NON_PLAYING_CHARACTER ||
    ((InteractiveEntity*) other)->subtype != NON_PLAYING_CHARACTER;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool InteractiveEntity::is_water_obstacle(void) {
  return true;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool InteractiveEntity::is_hole_obstacle(void) {
  return true;
}

/**
 * Returns whether a teletransporter is considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true
 */
bool InteractiveEntity::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return true;
}

/**
 * This function is called by the engine when there is a collision with another entity.
 * This is a redefinition of Detector::collision().
 * If the entity is the hero, we allow him to interact with this entity.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void InteractiveEntity::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE
	&& (subtype != SIGN || hero->get_animation_direction() == 1) /* TODO move to future class Sign */) {

      // we show the action icon
      keys_effect->set_action_key_effect(action_key_effects[subtype]);
    }
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 */
void InteractiveEntity::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();
  Hero *hero = zsdx->game->get_hero();

  if (hero->get_state() == Hero::FREE) {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

    // for a place with water: start the dialog
    if (subtype == WATER_FOR_BOTTLE) {
      zsdx->game->get_equipment()->found_water();
    }
    else {

      // for an NPC: look in the hero's direction 
      if (subtype == NON_PLAYING_CHARACTER) {
	int direction = (hero->get_animation_direction() + 2) % 4;
	get_sprite()->set_current_direction(direction);

      }

      // start the message or call the script
      if (message_to_show != "_none") {
	zsdx->game->show_message(message_to_show);
      }
      else {
	// there is no message specified: we call the script
	call_script();
      }
    }
  }
}

/**
 * Notifies the script that the player is interacting with this entity.
 */
void InteractiveEntity::call_script(void) {

  if (subtype == NON_PLAYING_CHARACTER) {
    map->get_script()->event_npc_dialog(get_name());
  }
  else {
    map->get_script()->event_interaction(get_name());
  }
}

/**
 * Updates the entity.
 */
void InteractiveEntity::update(void) {

  Detector::update();

  if (subtype == NON_PLAYING_CHARACTER && get_movement() != NULL) {

    bool finished = false;
    std::string animation = get_sprite()->get_current_animation();
    if (animation == "walking") {
      finished = ((PathMovement*) get_movement())->is_finished();
    }
    else if (animation == "jumping") {
      finished = ((JumpMovement*) get_movement())->is_finished();
    }

    if (finished) {
      get_sprite()->set_current_animation("stopped");
      clear_movement();
      map->get_script()->event_npc_movement_finished(get_name());
    }
  }
}

/**
 * Makes the entity walk (only for an NPC).
 * The NPC's sprite must have an animation "walking".
 * @param path the path to follow (see class PathMovement)
 * @param loop true to make the movement loop
 * @param with_collisions true to make the movement sensitive to obstacles
 */
void InteractiveEntity::walk(std::string path, bool loop, bool with_collisions) {

  if (subtype != NON_PLAYING_CHARACTER) {
    DIE("This entity is not a non-playing character");
  }

  clear_movement();
  set_movement(new PathMovement(map, path, 6, loop, with_collisions));
  get_sprite()->set_current_animation("walking");
}

/**
 * Makes the entity walk randomly (only for NPC).
 * The NPC's sprite must have an animation "walking".
 */
void InteractiveEntity::walk_random(void) {

  if (subtype != NON_PLAYING_CHARACTER) {
    DIE("This entity is not a non-playing character");
  }

  clear_movement();
  set_movement(new RandomWalkMovement(map, 3));
  get_sprite()->set_current_animation("walking");
}

/**
 * Makes the entity jump into a direction (only for NPC).
 * The NPC's sprite must have an animation "jumping".
 * @param direction direction of the movement (0 to 7)
 * @param length length of the jump in pixels
 * @param with_collisions true to make the movement sensitive to the collisions
 */
void InteractiveEntity::jump(int direction, int length, bool with_collisions) {

  if (subtype != NON_PLAYING_CHARACTER) {
    DIE("This entity is not a non-playing character");
  }

  clear_movement();
  JumpMovement *movement = new JumpMovement(map, direction, length, with_collisions);
  movement->set_delay(20);
  set_movement(movement);
  get_sprite()->set_current_animation("jumping");
}

/**
 * This function is called when the entity has just moved.
 * If it is an NPC, its sprite's direction is updated.
 */
void InteractiveEntity::just_moved(void) {

  if (subtype == NON_PLAYING_CHARACTER) {

    if (get_sprite()->get_current_animation() == "walking") {
      PathMovement *movement = (PathMovement*) get_movement();
      int movement_direction = movement->get_current_direction();
      get_sprite()->set_current_direction(animation_directions[movement_direction]);
    }

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();
    if (hero->get_facing_entity() == this &&
	keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_SPEAK &&
	!hero->is_facing_point_in(get_position_in_map())) {

      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    }
  }
}

/**
 * Sets the direction of the entity's sprite.
 * @param direction a direction between 0 and 3
 */
void InteractiveEntity::set_sprite_direction(int direction) {
  get_sprite()->set_current_direction(direction);
}

/**
 * Displays the entity on the map.
 * This is a redefinition of MapEntity::display_on_map to handle the special
 * display when the entity is jumping.
 */
void InteractiveEntity::display_on_map(void) {

  if (subtype == NON_PLAYING_CHARACTER &&
      get_sprite()->get_current_animation() == "jumping") {

    int jump_height = ((JumpMovement*) get_movement())->get_jump_height();
    map->display_sprite(get_sprite(), get_x(), get_y() - jump_height);
  }
  else {
    MapEntity::display_on_map();
  }
}
