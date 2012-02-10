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
#include "lua/Script.h"
#include "Game.h"
#include "Map.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "entities/MapEntities.h"
#include "entities/Door.h"
#include "entities/InteractiveEntity.h"
#include "entities/Sensor.h"
#include "entities/Chest.h"
#include "entities/DynamicTile.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "entities/Hero.h"
#include "entities/PickableItem.h"
#include "entities/Bomb.h"
#include "lowlevel/Sound.h"
#include <lua.hpp>

/**
 * @brief Creates a dialog box and starts displaying a message.
 *
 * If the message is followed by other messages, they are also
 * displayed.
 * If the message (or one of its next messages) contains a variable,
 * then you have to call dialog_set_variable() to specify its value.
 *
 * - Argument 1 (string): id of the message to display
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_dialog_start(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &message_id = luaL_checkstring(l, 1);

  script->get_game().get_dialog_box().start_dialog(message_id, script);

  return 0;
}

/**
 * @brief Sets the value of the variable in a diabog.
 *
 * The function has to be called after the dialog box is created,
 * i.e. after calling dialog_start().
 *
 * - Argument 1 (string): id of the message containing the variable
 * - Argument 2 (string): value of the variable
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_dialog_set_variable(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const MessageId &message_id = luaL_checkstring(l, 1);
  const std::string &value = luaL_checkstring(l, 2);

  script->get_game().get_dialog_box().set_variable(message_id, value);

  return 0;
}

/**
 * @brief Changes the style of the future dialog boxes.
 *
 * - Argument 1 (integer): the style to set (see the DialogBox::Style enum)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_dialog_set_style(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int style = luaL_checkinteger(l, 1);

  script->get_game().get_dialog_box().set_style(DialogBox::Style(style));

  return 0;
}

/**
 * @brief Enables or disables the head up display.
 *
 * - Argument 1 (boolean): true to enable it, false to disable it
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hud_set_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  bool enabled = lua_toboolean(l, 1) != 0;

  script->get_game().set_hud_enabled(enabled);
  return 0;
}

/**
 * @brief Sets whether the player can pause the game.
 *
 * - Argument 1 (boolean): true to enable the pause key
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hud_set_pause_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  bool pause_key_available = lua_toboolean(l, 1) != 0;

  script->get_game().set_pause_key_available(pause_key_available);

  return 0;
}

/**
 * @brief Returns the current level of light of the map.
 *
 * - Return value (integer): 0 for no light, a positive value if there is some light
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_light_get(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int light = script->get_map().get_light();

  lua_pushinteger(l, light);

  return 1;
}

/**
 * @brief Sets the level of light of the map.
 *
 * - Argument 1 (integer): the level of light
 * (0: no light, a positive value: full light)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_light_set(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int light = luaL_checkinteger(l, 1);

  script->get_map().set_light(light);

  return 0;
}

/**
 * @brief Moves the camera towards a target point.
 *
 * - Argument 1 (integer): x coordinate of the target point
 * - Argument 2 (integer): y coordinate of the target point
 * - Argument 3 (integer): speed of the camera movement in pixels per second (150 is a normal speed)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_camera_move(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  int x = luaL_checkinteger(l, 1);
  int y = luaL_checkinteger(l, 2);
  int speed = luaL_checkinteger(l, 3);

  script->get_game().get_current_map().move_camera(x, y, speed);

  return 0;
}

/**
 * @brief Moves the camera back to the hero.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_camera_restore(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_current_map().restore_camera();

  return 0;
}

/**
 * @brief Gives a treasure to the hero.
 *
 * If the treasure comes from a chest, you don't have to call this function:
 * the treasure will be given to the player automatically when he opens the chest.
 * You can use this function to make a non-playing character
 * give a treasure to the player.
 *
 * - Argument 1 (integer): name of the item to give (according to the item list of items.dat)
 * - Argument 2 (integer): variant of this item (1 if the item has only one variant)
 * - Argument 3 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_treasure_give(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  const std::string &item_name = luaL_checkstring(l, 1);
  int variant = luaL_checkinteger(l, 2);
  int savegame_variable = luaL_checkinteger(l, 3);

  script->get_game().get_hero().start_treasure(Treasure(script->get_game(), item_name, variant, savegame_variable));

  return 0;
}

/**
 * @brief Prevents the player from moving until hero_unfreeze() is called.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_freeze(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_freezed();

  return 0;
}

/**
 * @brief Allows the player to move again after a call to hero_freeze().
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_unfreeze(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_free();

  return 0;
}

/**
 * @brief Sends the hero to a map.
 *
 * - Argument 1 (int): id of the destination map (can be the same one)
 * - Argument 2 (string): name of the destination point on that map
 * - Argument 3 (int): type of transition to play
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_set_map(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);

  MapId map_id = luaL_checkinteger(l, 1);
  const std::string &destination_point_name = luaL_checkstring(l, 2);
  Transition::Style transition_style = Transition::Style(luaL_checkinteger(l, 3));

  script->get_game().set_current_map(map_id, destination_point_name, transition_style);

  return 0;
}

/**
 * @brief Hides or shows the hero.
 *
 * Hiding the hero does not disable its movements, so when using this function
 * you will usually also need to freeze the hero.
 * - Argument 1 (boolean): true to make the hero visible
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_set_visible(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  bool visible = lua_toboolean(l, 1) != 0;

  script->get_game().get_hero().set_visible(visible);

  return 0;
}

/**
 * @brief Returns the current direction of the hero's sprite.
 *
 * - Return value (integer): the direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_get_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  int direction = script->get_game().get_hero().get_animation_direction();
  lua_pushinteger(l, direction);

  return 1;
}

/**
 * @brief Sets the direction of the hero's sprite.
 *
 * - Argument 1 (integer): the direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_set_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int direction = luaL_checkinteger(l, 1);

  script->get_game().get_hero().set_animation_direction(direction);

  return 0;
}

/**
 * @brief Returns the position of the hero.
 *
 * - Return value 1 (integer): x coordinate
 * - Return value 2 (integer): y coordinate
 * - Return value 3 (integer): layer
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_get_position(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  Hero &hero = script->get_game().get_hero();

  lua_pushinteger(l, hero.get_x());
  lua_pushinteger(l, hero.get_y());
  lua_pushinteger(l, hero.get_layer());

  return 3;
}
/**
 * @brief Places the hero on the exact position of a sensor's name.
 *
 * - Argument 1 (string): name of the sensor
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_align_on_sensor(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Sensor *sensor = (Sensor*) entities.get_entity(SENSOR, name);
  script->get_game().get_hero().set_xy(sensor->get_xy());

  return 0;
}

/**
 * @brief Makes the hero walk with respect to a path.
 *
 * - Argument 1 (string): the path (each character is a direction between '0' and '7')
 * - Argument 2 (boolean): true to make the movement loop
 * - Argument 3 (boolean): true to make the movement sensible to obstacles
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_walk(lua_State *l) {

  called_by_script(l, 3, NULL);

  /* TODO
  const std::string &path = luaL_checkstring(l, 1);
  bool loop = lua_toboolean(l, 2) != 0;
  bool ignore_obstacles = lua_toboolean(l, 3) != 0;

  script->get_game().get_hero()->walk(path, loop, ignore_obstacles);
  */

  return 0;
}

/**
 * @brief Makes the hero jump into a direction.
 *
 * - Argument 1 (integer): the jump direction, between 0 and 7
 * - Argument 2 (integer): the jump length in pixels
 * - Argument 3 (boolean): true to enable the collisions
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_start_jumping(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);

  int direction = luaL_checkinteger(l, 1);
  int length = luaL_checkinteger(l, 2);
  bool ignore_obstacles = lua_toboolean(l, 3) != 0;

  script->get_game().get_hero().start_jumping(direction, length, ignore_obstacles, false);

  return 0;
}

/**
 * @brief Makes the hero brandish his sword meaning a victory
 * and plays the corresponding sound.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_start_victory_sequence(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_victory();

  return 0;
}

/**
 * @brief Makes the hero shoot a boomerang.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_start_boomerang(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_boomerang();

  return 0;
}

/**
 * @brief Makes the hero shoot an arrow with a bow.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_start_bow(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_bow();

  return 0;
}

/**
 * @brief Makes the hero run.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_hero_start_running(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_game().get_hero().start_running();

  return 0;
}

/**
 * @brief Returns the position of an NPC.
 *
 * - Argument 1 (string): name of the NPC
 * - Return value 1 (integer): x position
 * - Return value 2 (integer): y position
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_get_position(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities.get_entity(INTERACTIVE_ENTITY, name);
  const Rectangle &coordinates = npc->get_xy();

  lua_pushinteger(l, coordinates.get_x());
  lua_pushinteger(l, coordinates.get_y());

  return 2;
}

/**
 * @brief Sets the position of an NPC.
 *
 * - Argument 1 (string): name of the NPC
 * - Argument 2 (integer): x position to set
 * - Argument 3 (integer): y position to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_set_position(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);

  const std::string &name = luaL_checkstring(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);

  MapEntities &entities = script->get_map().get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities.get_entity(INTERACTIVE_ENTITY, name);
  npc->set_xy(x, y);

  return 0;
}

/**
 * @brief Makes an NPC move.
 *
 * - Argument 1 (string): name of the NPC to make move
 * - Argument 2 (movement): the movement to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_start_movement(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  int movement_handle = luaL_checkinteger(l, 2);

  MapEntities &entities = script->get_map().get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities.get_entity(INTERACTIVE_ENTITY, name);
  Movement &movement = script->start_movement(movement_handle);

  npc->clear_movement();
  npc->set_movement(&movement);

  return 0;
}

/**
 * @brief Makes the sprite of an NPC accessible from the script.
 *
 * - Argument 1 (string): name of the NPC
 * - Return value (sprite): the sprite of this NPC (your script can then pass it as a parameter
 * to all sol.main.sprite_* functions)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_get_sprite(lua_State *l) {

  // an NPC is actually a subtype of interactive entity
  return map_api_interactive_entity_get_sprite(l);
}

/**
 * @brief Removes an NPC from the map.
 *
 * - Argument 1 (string): name of the NPC
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_remove(lua_State *l) {

  // an NPC is actually a subtype of interactive entity
  return map_api_interactive_entity_remove(l);
}

/**
 * @brief Returns whether an NPC exists on the map.
 *
 * - Argument 1 (string): name of the NPC to check
 * - Return value (boolean): true if an NPC with this name exists on the map
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_npc_exists(lua_State *l) {

  // an NPC is actually a subtype of interactive entity
  return map_api_interactive_entity_exists(l);
}

/**
 * @brief Makes the sprite of an interactive entity accessible from the script.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (sprite): the sprite of this interactive entity (your script can then pass it as a parameter
 * to all sol.main.sprite_* functions)
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_interactive_entity_get_sprite(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &entity_name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities.get_entity(INTERACTIVE_ENTITY, entity_name);

  int handle = script->create_sprite_handle(entity->get_sprite());
  lua_pushinteger(l, handle);

  return 1;
}

/**
 * @brief Removes an interactive entity from the map.
 *
 * - Argument 1 (string): name of the interactive entity
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_interactive_entity_remove(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  entities.remove_entity(INTERACTIVE_ENTITY, name);

  return 0;
}

/**
 * @brief Returns whether an interactive entity exists on the map.
 *
 * - Argument 1 (string): name of the interactive entity to check
 * - Return value (boolean): true if an interactive entity with this name exists on the map
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_interactive_entity_exists(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities.find_entity(INTERACTIVE_ENTITY, name);

  lua_pushboolean(l, entity != NULL);

  return 1;
}

/**
 * @brief Sets the chest open or closed.
 *
 * Only the chest sprite is affected (use give_treasure to give a treasure to the player).
 * This function is useful for chests whose content is managed by the script.
 * - Argument 1 (string): name of the chest
 * - Argument 2 (boolean): true to make the chest open, false to make it closed
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_chest_set_open(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  Chest *chest = (Chest*) entities.get_entity(CHEST, name);
  chest->set_open(open);

  return 0;
}

/**
 * @brief Hides or unhides a chest.
 *
 * If the chest is already open, hiding it has not effect.
 * - Argument 1 (string): name of the chest
 * - Argument 2 (boolean): true to make the chest hidden, false to make it appear
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_chest_set_hidden(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool hidden = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  Chest *chest = (Chest*) entities.get_entity(CHEST, name);
  chest->set_visible(!hidden);

  return 0;
}

/**
 * @brief Returns whether a chest is hidden.
 *
 * - Argument 1 (string): name of the chest
 * - Return value (boolean): true if this chest is hidden
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_chest_is_hidden(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Chest *chest = (Chest*) entities.get_entity(CHEST, name);
  lua_pushboolean(l, chest->is_visible() ? 0 : 1);

  return 1;
}

/**
 * @brief Enables or disables a dynamic tile.
 *
 * - Argument 1 (string): name of the dynamic tile
 * - Argument 2 (boolean): true to enable it, false to disable it
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_tile_set_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities.get_entity(DYNAMIC_TILE, name);
  dynamic_tile->set_enabled(enable);

  return 0;
}

/**
 * @brief Enables or disables a set of dynamic tiles.
 *
 * - Argument 1 (string): prefix of the name of the dynamic tiles to disable
 * - Argument 2 (boolean): true to enable them, false to disable them
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_tile_set_group_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = luaL_checkstring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> dynamic_tiles = entities.get_entities_with_prefix(DYNAMIC_TILE, prefix);

  std::list<MapEntity*>::iterator it;
  for (it = dynamic_tiles.begin(); it != dynamic_tiles.end(); it++) {
    DynamicTile *dynamic_tile = (DynamicTile*) (*it);
    dynamic_tile->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Returns whether a dynamic tile is enabled.
 *
 * - Argument 1 (string): name of the dynamic tile
 * - Return value (boolean): true if this tile is enabled
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_tile_is_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities.get_entity(DYNAMIC_TILE, name);
  lua_pushboolean(l, dynamic_tile->is_enabled() ? 1 : 0);

  return 1;
}

/**
 * @brief Replaces a block at its initial position.
 *
 * - Argument 1 (string): name of the block to reset
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_block_reset(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &block_name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Block *block = (Block*) entities.get_entity(BLOCK, block_name);
  block->reset();

  return 0;
}

/**
 * @brief Replaces all blocks of the map at their initial position.
 * @param l the Lua context that is calling this function
 */
int Script::map_api_block_reset_all(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> blocks = entities.get_entities(BLOCK);

  std::list<MapEntity*>::iterator i;
  for (i = blocks.begin(); i != blocks.end(); i++) {
    ((Block*) (*i))->reset();
  }

  return 0;
}

/**
 * @brief Removes a shop item from the map.
 *
 * - Argument 1 (string): name of the shop item
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_shop_item_remove(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  script->get_map().get_entities().remove_entity(SHOP_ITEM, name);

  return 0;
}

/**
 * @brief Returns whether a switch is currently enabled.
 *
 * - Argument 1 (string): name of the switch
 * - Return value (boolean): true if the switch is enabled
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_switch_is_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Switch *sw = (Switch*) entities.get_entity(SWITCH, name);

  lua_pushboolean(l, sw->is_enabled());

  return 1;
}

/**
 * @brief Enables or disables a switch.
 *
 * - Argument 1 (string): name of the switch
 * - Argument 2 (boolean): true to enable the switch, false to disable it
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_switch_set_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  Switch *sw = (Switch*) entities.get_entity(SWITCH, name);
  sw->set_enabled(enable);

  return 0;
}

/**
 * @brief Locks a switch in its current state or unlocks it.
 *
 * - Argument 1 (string): name of the switch
 * - Argument 2 (boolean): true to lock the switch, false to unlock it
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_switch_set_locked(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool lock = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  Switch *sw = (Switch*) entities.get_entity(SWITCH, name);
  sw->set_locked(lock);

  return 0;
}

/**
 * @brief Returns whether an enemy is dead.
 *
 * An enemy is considered as dead if it is not present on the map.
 * - Argument 1 (string): name of the enemy
 * - Return value (boolean): true if the enemy is not on the map, false if it is alive
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_is_dead(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Enemy *enemy = (Enemy*) entities.find_entity(ENEMY, name);

  lua_pushboolean(l, (enemy == NULL));

  return 1;
}

/**
 * @brief Returns whether a set of enemies are dead.
 *
 * An enemy is considered as dead if it is not present on the map.
 * - Argument 1 (string): prefix of the name of the enemies to check
 * - Return value (boolean): true if there is no enemy left with this prefix on the map,
 * false if at least one of them is alive
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_is_group_dead(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> enemies = entities.get_entities_with_prefix(ENEMY, prefix);

  lua_pushboolean(l, enemies.empty());

  return 1;
}

/**
 * @brief Enables or disables an enemy.
 *
 * A normal enemy is enabled by default. A boss or a miniboss is disabled by default.
 * - Argument 1 (string): name of the enemy
 * - Argument 2 (boolean): true to enable the enemy, false to disable it
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_set_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &name = luaL_checkstring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  Enemy *enemy = (Enemy*) entities.get_entity(ENEMY, name);
  enemy->set_enabled(enable);

  return 0;
}

/**
 * @brief Starts the battle against a boss.
 *
 * Calling this function enables the boss if he is alive and plays the appropriate music.
 * If the boss was already killed, nothing happens.
 * - Argument 1 (string): name of the boss
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_start_boss(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Enemy *enemy = (Enemy*) entities.find_entity(ENEMY, name);
  entities.start_boss_battle(enemy);

  return 0;
}

/**
 * @brief Ends the battle against a boss.
 *
 * Calling this function plays the appropriate music and freezes the hero.
 * The next step is usually to start the dungeon end sequence.
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_end_boss(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_map().get_entities().end_boss_battle();

  return 0;
}

/**
 * @brief Starts the battle against a miniboss.
 *
 * Calling this function enables the miniboss if he is alive and plays the appropriate music.
 * If the miniboss was already killed, nothing happens.
 * - Argument 1 (string): name of the miniboss
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_start_miniboss(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Enemy *enemy = (Enemy*) entities.find_entity(ENEMY, name);
  entities.start_miniboss_battle(enemy);

  return 0;
}

/**
 * @brief Ends the battle against a miniboss.
 *
 * Calling this function plays the appropriate music.
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_enemy_end_miniboss(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->get_map().get_entities().end_miniboss_battle();

  return 0;
}

/**
 * @brief Removes a sensor from the map.
 *
 * - Argument 1 (string): name of the sensor
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_sensor_remove(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &name = luaL_checkstring(l, 1);

  script->get_map().get_entities().remove_entity(SENSOR, name);

  return 0;
}

/**
 * @brief Opens one or several doors.
 *
 * The doors must be normal closed doors
 * (not doors for keys or explosions).
 * - Argument 1 (string): prefix of the name of the doors to open
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_door_open(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = luaL_checkstring(l, 1);

  bool done = false;
  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door *door = (Door*) (*it);
    if (!door->is_changing()) {
      done = true;
    }
    door->open();
  }

  // make sure the sound is played only once even if the script calls this function repeatedly while the door is still changing
  if (done) {
    Sound::play("door_open");
  }

  return 0;
}

#include <iostream>
/**
 * @brief Closes one or several doors.
 *
 * The doors must be normal, open door
 * (not doors for keys or bombs).
 * - Argument 1 (string): prefix of the name of the doors to close
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_door_close(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = luaL_checkstring(l, 1);

  bool done = false;
  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door *door = (Door*) (*it);
    if (!door->is_changing()) {
      done = true;
    }
    door->close();
  }

  // make sure the sound is played only once even if the script calls this function repeatedly while the door is still changing
  if (done) {
    Sound::play("door_closed");
  }

  return 0;
}

/**
 * @brief Returns whether a door is open
 *
 * - Argument 1 (string): name of the door
 * - Return value (boolean): true if this door is open
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_door_is_open(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &name = luaL_checkstring(l, 1);

  MapEntities &entities = script->get_map().get_entities();
  Door *door = (Door*) entities.get_entity(DOOR, name);
  lua_pushboolean(l, door->is_open() ? 1 : 0);

  return 1;
}

/**
 * @brief Makes one or several doors open or closed.
 *
 * - Argument 1 (string): prefix of the name of the doors to close
 * - Argument 2 (boolean): true to make them open, false to make them closed
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_door_set_open(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = luaL_checkstring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities &entities = script->get_map().get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door *door = (Door*) (*it);
    door->set_open(open);
  }

  return 0;
}

/**
 * @brief Places a new pickable item on the map.
 *
 * - Argument 1 (string): name of the item to create (according to items.dat)
 * - Argument 2 (integer): variant of the item
 * - Argument 3 (integer): savegame variable (-1: not saved)
 * - Argument 4 (integer): x
 * - Argument 5 (integer): y
 * - Argument 6 (integer): layer
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_pickable_item_create(lua_State *l) {

  Script *script;
  called_by_script(l, 6, &script);

  const std::string &item_name = luaL_checkstring(l, 1);
  int variant = luaL_checkinteger(l, 2);
  int savegame_variable = luaL_checkinteger(l, 3);
  int x = luaL_checkinteger(l, 4);
  int y = luaL_checkinteger(l, 5);
  Layer layer = Layer(luaL_checkinteger(l, 6));

  Game &game = script->get_game();
  MapEntities &entities = script->get_map().get_entities();
  entities.add_entity(PickableItem::create(
      game, layer, x, y,
      Treasure(game, item_name, variant, savegame_variable),
      FALLING_MEDIUM, false
      ));

  return 0;
}

/**
 * @brief Places a bomb on the map.
 *
 * - Argument 1 (integer): x
 * - Argument 2 (integer): y
 * - Argument 3 (integer): layer
 *
 * @param l the Lua context that is calling this function
 */
int Script::map_api_bomb_create(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);

  int x = luaL_checkinteger(l, 1);
  int y = luaL_checkinteger(l, 2);
  Layer layer = Layer(luaL_checkinteger(l, 3));

  MapEntities &entities = script->get_map().get_entities();
  entities.add_entity(new Bomb(layer, x, y));

  return 0;
}
