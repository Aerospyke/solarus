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
#include "DungeonEquipment.h"
#include "Game.h"
#include "Map.h"
#include "Savegame.h"

/**
 * Constructor.
 * @param savegame the savegame
 */
DungeonEquipment::DungeonEquipment(Savegame *savegame):
  savegame(savegame), game(NULL) {

}

/**
 * Destructor.
 */
DungeonEquipment::~DungeonEquipment(void) {

}

/**
 * Sets the current game.
 * @param game the game
 */
void DungeonEquipment::set_game(Game *game) {
  this->game = game;
}

/**
 * Returns the current dungeon number.
 * The dungeon number returned is between 1 and 20.
 * The programs stops with an error message if the player is not in a dungeon.
 * @return the number of the current dungeon between 1 and 20
 */
int DungeonEquipment::get_current_dungeon(void) {

  Map *current_map = game->get_current_map();

  if (!current_map->is_in_dungeon()) {
    DIE("The player is not in a dungeon");
  }

  return current_map->get_world_number();
}

/**
 * Returns whether the player has found the map in the current dungeon.
 * @return true if the player has found the map in the current dungeon
 */
bool DungeonEquipment::has_map(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_MAP + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's map to the player.
 */
void DungeonEquipment::add_map(void) {

  if (has_map()) {
    DIE("The player already has the map");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_MAP + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the compass in the current dungeon.
 * @return true if the player has found the compass in the current dungeon
 */
bool DungeonEquipment::has_compass(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_COMPASS + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's compass to the player.
 */
void DungeonEquipment::add_compass(void) {

  if (has_compass()) {
    DIE("The player already has the compass");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_COMPASS + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the big key in the current dungeon.
 * @return true if the player has found the big key in the current dungeon
 */
bool DungeonEquipment::has_big_key(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BIG_KEY + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's big key to the player.
 */
void DungeonEquipment::add_big_key(void) {

  if (has_big_key()) {
    DIE("The player already has the big key");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BIG_KEY + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}

/**
 * Returns whether the player has found the boss key in the current dungeon.
 * @return true if the player has found the boss key in the current dungeon
 */
bool DungeonEquipment::has_boss_key(void) {

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BOSS_KEY + 10 * (dungeon - 1);
  return savegame->get_integer(index) != 0;
}

/**
 * Give the current dungeon's boss key to the player.
 */
void DungeonEquipment::add_boss_key(void) {

  if (has_boss_key()) {
    DIE("The player already has the boss key");
  }

  int dungeon = get_current_dungeon();
  int index = Savegame::FIRST_DUNGEON_BOSS_KEY + 10 * (dungeon - 1);
  savegame->set_integer(index, 1);
}
