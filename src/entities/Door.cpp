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
#include "entities/MapEntities.h"
#include "entities/Door.h"
#include "entities/Hero.h"
#include "entities/DynamicTile.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "Game.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Savegame.h"
#include "Map.h"
#include "FileTools.h"
#include <list>

const std::string Door::animations[] = {
  "closed", "small_key", "small_key_block", "big_key", "boss_key", "weak", "very_weak", ""
};

const MessageId key_required_message_ids[] = {
  "", "_small_key_required", "_small_key_required", "_big_key_required", "_boss_key_required"
};

/**
 * Creates a door.
 * @param name name identifying this entity
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction direction of the door
 * @param subtype the subtype of door
 * @param savegame_variable variable where the door's state is saved
 * (can be -1 for the subtype CLOSED)
 */
Door::Door(const std::string &name, Layer layer, int x, int y,
	     int direction, Subtype subtype, int savegame_variable):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), savegame_variable(savegame_variable), door_open(true),
  changing(false), initialized(false) {

  if (subtype == SMALL_KEY_BLOCK) {
    set_size(16, 16);
  }
  else if (direction % 2 == 0) {
    set_size(16, 32);
  }
  else {
    set_size(32, 16);
  }

  if (subtype != WEAK_INVISIBLE) {
    create_sprite("entities/door");
  }

  if (savegame_variable != -1) {
    set_open(zsdx->game->get_savegame()->get_boolean(savegame_variable));
  }
  else {
    set_open(false);
  }
  get_sprite()->set_current_direction(direction);
}

/**
 * Destructor.
 */
Door::~Door(void) {

}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Door::parse(std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int direction, subtype, savegame_variable;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  FileTools::read(is, savegame_variable);

  return new Door(name, Layer(layer), x, y, direction, Subtype(subtype), savegame_variable);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Door::get_type(void) {
  return DOOR;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Door::is_obstacle_for(MapEntity *other) {
  return !is_open() || changing;
}

/**
 * Returns whether this door is open.
 * @return true if this door is open
 */
bool Door::is_open(void) {
  return door_open;
}

/**
 * Makes the door open or closed.
 * @param door_open true to make it open, false to make it closed
 */
void Door::set_open(bool door_open) {
  
  this->door_open = door_open;

  if (!door_open) {
    get_sprite()->set_current_animation(animations[subtype]);
  }

  if (map != NULL) {
    update_dynamic_tiles();
  }

  if (savegame_variable != -1) {
    zsdx->game->get_savegame()->set_boolean(savegame_variable, door_open);
  }
}

/**
 * Enables or disable the dynamic tiles having the same prefix than the door
 * depending on the door state.
 */
void Door::update_dynamic_tiles(void) {
  
  std::list<MapEntity*> *tiles = map->get_entities()->get_entities_with_prefix(DYNAMIC_TILE, get_name() + "_closed");
  std::list<MapEntity*>::iterator it;
  for (it = tiles->begin(); it != tiles->end(); it++) {
    DynamicTile *tile = (DynamicTile*) *it;
    tile->set_enabled(!door_open);
  }
  delete tiles;

  tiles = map->get_entities()->get_entities_with_prefix(DYNAMIC_TILE, get_name() + "_open");
  for (it = tiles->begin(); it != tiles->end(); it++) {
    DynamicTile *tile = (DynamicTile*) *it;
    tile->set_enabled(door_open);
  }
  delete tiles;
}

/**
 * This function is called by the engine when there is a collision with another entity.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Door::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (!is_open() && entity_overlapping->is_hero() && requires_key()) {

    Hero *hero = (Hero*) entity_overlapping;
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the action icon
      keys_effect->set_action_key_effect(can_open() ? KeysEffect::ACTION_KEY_OPEN : KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * Returns whether this door requires a key to be open.
 * @return true if this door requires a key to be open
 */
bool Door::requires_key(void) {
  return requires_small_key() || subtype == BIG_KEY || subtype == BOSS_KEY;
}

/**
 * Returns whether this door must be open with a small key.
 * @return true if this door must be open with a small key
 */
bool Door::requires_small_key(void) {
  return subtype == SMALL_KEY || subtype == SMALL_KEY_BLOCK;
}

/**
 * Returns whether this door must be open with a bomb explosion.
 * @return true if this door must be open with a bomb explosion
 */
bool Door::requires_bomb(void) {
  return subtype == WEAK || subtype == VERY_WEAK || subtype == WEAK_INVISIBLE;
}

/**
 * Returns whether the player has the right key to open this door.
 * If the door cannot be open with a key, false is returned.
 * @return true if the player has the key corresponding to this door
 */
bool Door::can_open(void) {

  Equipment *equipment = zsdx->game->get_equipment();
  DungeonEquipment *dungeon_equipment = zsdx->game->get_dungeon_equipment();

  return (requires_small_key() && equipment->has_small_key())
    || (subtype == BIG_KEY && dungeon_equipment->has_big_key())
    || (subtype == BOSS_KEY && dungeon_equipment->has_boss_key());
}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity
 */
void Door::set_suspended(bool suspended) {
  // we never suspend a door to allow the sprite animation even when the camera is moving
}

/**
 * Updates the entity.
 */
void Door::update(void) {
  Detector::update();

  if (!initialized) {
    update_dynamic_tiles();
    initialized = true;
  }

  if (changing && get_sprite()->is_animation_finished()) {
    changing = false;
    set_open(!is_open());
  }

  if (savegame_variable != -1) {
    bool open_in_savegame = zsdx->game->get_savegame()->get_boolean(savegame_variable);
    if (open_in_savegame && !is_open() && !changing) {
      set_opening();
    }
    else if (!open_in_savegame && is_open() && !changing) {
      set_closing();
    }
  }
}

/**
 * Displays the entity on the map.
 */
void Door::display_on_map(void) {
  if (has_sprite() && (!is_open() || changing)) {
    Detector::display_on_map();
  }
}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero opens the door if possible, otherwise a message is shown.
 */
void Door::action_key_pressed(void) {

  Hero *hero = zsdx->game->get_hero();
  Equipment *equipment = zsdx->game->get_equipment();

  if (hero->get_state() == Hero::FREE) {
    if (can_open()) {
      ResourceManager::get_sound("door_unlocked")->play();
      ResourceManager::get_sound("door_open")->play();

      zsdx->game->get_savegame()->set_boolean(savegame_variable, true);
      if (subtype == SMALL_KEY_BLOCK) {
	set_open(true);
      }
      else {
	set_opening();
      }

      if (requires_small_key()) {
	equipment->remove_small_key();
      }
    }
    else {
      ResourceManager::get_sound("wrong")->play();
      zsdx->game->show_message(key_required_message_ids[subtype]);
    }
  }
}

/**
 * This function is called when the player is tapping his sword against this detector.
 * @return the sound to play when tapping this detector with the sword
 */
SoundId Door::get_sword_tapping_sound(void) {
  return requires_bomb() ? "sword_tapping_weak_wall" : "sword_tapping";
}

/**
 * Opens the door.
 * This function can be called only for a door with subtype CLOSED.
 */
void Door::open(void) {

  if (subtype != CLOSED) {
    DIE("This kind of door cannot be open or closed directly");
  }

  if (is_open()) {
    DIE("This door is already open");
  }

  set_opening();

  if (savegame_variable != -1) {
    zsdx->game->get_savegame()->set_boolean(savegame_variable, true);
  }
}

/**
 * Makes the door being open.
 */
void Door::set_opening(void) {

  std::string animation = requires_key() ? "opening_key" : "opening";
  get_sprite()->set_current_animation(animation);
  changing = true;
}

/**
 * Closes the door.
 * This function can be called only for a door with subtype CLOSED.
 */
void Door::close(void) {

  if (!is_open()) {
    DIE("This door is already closed");
  }

  set_closing();

  if (savegame_variable != -1) {
    zsdx->game->get_savegame()->set_boolean(savegame_variable, false);
  }
}

/**
 * Makes the door being closed.
 */
void Door::set_closing(void) {
  get_sprite()->set_current_animation("opening");
  changing = true;
}

