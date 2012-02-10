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
#include "lua/MapScript.h"
#include "lua/Scripts.h"
#include "Map.h"
#include "Game.h"
#include "Sprite.h"
#include "InventoryItem.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "entities/EntityType.h"
#include "entities/MapEntities.h"
#include "entities/InteractiveEntity.h"
#include "entities/Hero.h"
#include "entities/Chest.h"
#include "entities/Block.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include "entities/Door.h"
#include "entities/Sensor.h"
#include "entities/Enemy.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>
#include <iomanip>

/**
 * @brief Creates a map script.
 * @param scripts the list of scripts
 * @param map the map
 */
MapScript::MapScript(Scripts &scripts, Map &map):
  GameScript(scripts, map.get_game()), map(map), hero(map.get_game().get_hero()) {

}

/**
 * @brief Desctructor.
 */
MapScript::~MapScript() {

}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void MapScript::register_available_functions() {

  // functions available to all game scripts
  GameScript::register_available_functions();

  // functions specific to map scripts
  lua_register(context, "dialog_start", l_dialog_start);
  lua_register(context, "dialog_set_variable", l_dialog_set_variable);
  lua_register(context, "dialog_set_style", l_dialog_set_style);
  lua_register(context, "hud_set_enabled", l_hud_set_enabled);
  lua_register(context, "hud_set_pause_enabled", l_hud_set_pause_enabled);
  lua_register(context, "treasure_give", l_treasure_give);
  lua_register(context, "camera_move", l_camera_move);
  lua_register(context, "camera_restore", l_camera_restore);
  lua_register(context, "hero_freeze", l_hero_freeze);
  lua_register(context, "hero_unfreeze", l_hero_unfreeze);
  lua_register(context, "hero_set_map", l_hero_set_map);
  lua_register(context, "hero_set_direction", l_hero_set_direction);
  lua_register(context, "hero_align_on_sensor", l_hero_align_on_sensor);
  lua_register(context, "hero_walk", l_hero_walk);
  lua_register(context, "hero_jump", l_hero_jump);
  lua_register(context, "hero_start_victory_sequence", l_hero_start_victory_sequence);
  lua_register(context, "hero_set_visible", l_hero_set_visible);
  lua_register(context, "npc_get_position", l_npc_get_position);
  lua_register(context, "npc_set_position", l_npc_set_position);
  lua_register(context, "npc_walk", l_npc_walk);
  lua_register(context, "npc_random_walk", l_npc_random_walk);
  lua_register(context, "npc_jump", l_npc_jump);
  lua_register(context, "npc_set_animation", l_npc_set_animation);
  lua_register(context, "npc_set_animation_ignore_suspend", l_npc_set_animation_ignore_suspend);
  lua_register(context, "npc_set_direction", l_npc_set_direction);
  lua_register(context, "npc_remove", l_npc_remove);
  lua_register(context, "chest_set_open", l_chest_set_open);
  lua_register(context, "chest_set_hidden", l_chest_set_hidden);
  lua_register(context, "chest_is_hidden", l_chest_is_hidden);
  lua_register(context, "tile_set_enabled", l_tile_set_enabled);
  lua_register(context, "tile_set_group_enabled", l_tile_set_group_enabled);
  lua_register(context, "tile_is_enabled", l_tile_is_enabled);
  lua_register(context, "block_reset", l_block_reset);
  lua_register(context, "block_reset_all", l_block_reset_all);
  lua_register(context, "interactive_entity_get_animation", l_interactive_entity_get_animation);
  lua_register(context, "interactive_entity_get_animation_delay", l_interactive_entity_get_animation_delay);
  lua_register(context, "interactive_entity_get_animation_frame", l_interactive_entity_get_animation_frame);
  lua_register(context, "interactive_entity_get_direction", l_interactive_entity_get_direction);
  lua_register(context, "interactive_entity_is_animation_paused", l_interactive_entity_is_animation_paused);
  lua_register(context, "interactive_entity_set_animation", l_interactive_entity_set_animation);
  lua_register(context, "interactive_entity_set_animation_delay", l_interactive_entity_set_animation_delay);
  lua_register(context, "interactive_entity_set_animation_frame", l_interactive_entity_set_animation_frame);
  lua_register(context, "interactive_entity_set_direction", l_interactive_entity_set_direction);
  lua_register(context, "interactive_entity_set_animation_paused", l_interactive_entity_set_animation_paused);
  lua_register(context, "interactive_entity_set_animation_ignore_suspend", l_interactive_entity_set_animation_ignore_suspend);
  lua_register(context, "interactive_entity_fade", l_interactive_entity_fade);
  lua_register(context, "interactive_entity_remove", l_interactive_entity_remove);
  lua_register(context, "shop_item_remove", l_shop_item_remove);
  lua_register(context, "switch_is_enabled", l_switch_is_enabled);
  lua_register(context, "switch_set_enabled", l_switch_set_enabled);
  lua_register(context, "switch_set_locked", l_switch_set_locked);
  lua_register(context, "enemy_is_dead", l_enemy_is_dead);
  lua_register(context, "enemy_is_group_dead", l_enemy_is_group_dead);
  lua_register(context, "enemy_set_enabled", l_enemy_set_enabled);
  lua_register(context, "enemy_start_boss", l_enemy_start_boss);
  lua_register(context, "enemy_end_boss", l_enemy_end_boss);
  lua_register(context, "enemy_start_miniboss", l_enemy_start_miniboss);
  lua_register(context, "enemy_end_miniboss", l_enemy_end_miniboss);
  lua_register(context, "sensor_remove", l_sensor_remove);
  lua_register(context, "door_open", l_door_open);
  lua_register(context, "door_close", l_door_close);
  lua_register(context, "door_is_open", l_door_is_open);
  lua_register(context, "door_set_open", l_door_set_open);
}

/**
 * @brief Loads the script and starts it.
 *
 * This function is called when the map starts.
 *
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::start(const std::string &destination_point_name) {

  // compute the file name, depending on the id of the map
  int id = (int) map.get_id();
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id;

  // load the script
  load(oss.str());

  // notify the map
  scripts.event_map_started(destination_point_name);
}

/**
 * @brief Checks the number of arguments provided to a C++ function called by the Lua script.
 *
 * In any C++ function called by the Lua script (i.e. a function prefixed by "l_"),
 * the first instruction calls this function.
 * It checks the number of arguments provided to the C++ function called by the Lua script
 * and retrieves the current MapScript object.
 *
 * @param context the Lua context
 * @param nb_arguments the number of arguments to check (if it is incorrect, the program stops)
 * @param map_script if not NULL, a pointer to the MapScript object will be copied there so that the static C++ function
 * called by the Lua script can access it
 */

void MapScript::called_by_script(lua_State *context, int nb_arguments, MapScript **map_script) {

  Script *script;
  Script::called_by_script(context, nb_arguments, &script);
  *map_script = (MapScript*) script;
}

// functions that can be called by the Lua script

/**
 * @brief Creates a dialog box and starts displaying a message.
 *
 * If the message is followed by other messages, they are also
 * displayed.
 * If the message (or one of its next messages) contains a variable,
 * then you have to call dialog_set_variable() to specify its value.
 * 
 * - Argument 1 (string): id of the message to display
 */
int MapScript::l_dialog_start(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  const std::string &message_id = lua_tostring(l, 1);

  script->game.get_dialog_box()->start_dialog(message_id);

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
 */
int MapScript::l_dialog_set_variable(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);
  const MessageId &message_id = lua_tostring(l, 1);
  const std::string &value = lua_tostring(l, 2);

  script->game.get_dialog_box()->set_variable(message_id, value);

  return 0;
}

/**
 * @brief Changes the style of the future dialog boxes.
 * 
 * - Argument 1 (integer): the style to set (see the DialogBox::Style enum)
 */
int MapScript::l_dialog_set_style(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  int style = lua_tointeger(l, 1);

  script->game.get_dialog_box()->set_style(DialogBox::Style(style));

  return 0;
}

/**
 * @brief Enables or disables the head up display.
 *
 * - Argument 1 (boolean): true to enable it, false to disable it
 */
int MapScript::l_hud_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  bool enabled = lua_toboolean(l, 1) != 0;

  script->game.set_hud_enabled(enabled);
  return 0;
}

/**
 * @brief Sets whether the player can pause the game.
 * - Argument 1 (boolean): true to enable the pause key
 */
int MapScript::l_hud_set_pause_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  bool pause_key_available = lua_toboolean(l, 1) != 0;

  script->game.set_pause_key_available(pause_key_available);

  return 0;
}

/**
 * @brief Moves the camera towards a target point.
 *
 * - Argument 1 (integer): x coordinate of the target point
 * - Argument 2 (integer): y coordinate of the target point
 * - Argument 3 (integer): speed of the camera movement (10 is normal)
 */
int MapScript::l_camera_move(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);
  int x = lua_tointeger(l, 1);
  int y = lua_tointeger(l, 2);
  int speed = lua_tointeger(l, 3);

  script->game.get_current_map()->move_camera(x, y, speed);

  return 0;
}

/**
 * @brief Moves the camera back to the hero.
 */
int MapScript::l_camera_restore(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->game.get_current_map()->restore_camera();

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
 */
int MapScript::l_treasure_give(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);
  const std::string &item_name = lua_tostring(l, 1);
  int variant = lua_tointeger(l, 2);
  int savegame_variable = lua_tointeger(l, 3);

  script->hero.start_treasure(new Treasure(&script->game, item_name, variant, savegame_variable));

  return 0;
}

/**
 * @brief Prevents the player from moving until hero_unfreeze() is called.
 */
int MapScript::l_hero_freeze(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->hero.start_freezed();

  return 0;
}

/**
 * @brief Allows the player to move again after a call to hero_freeze().
 */
int MapScript::l_hero_unfreeze(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->hero.start_free();

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
int MapScript::l_hero_set_map(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  MapId map_id = lua_tointeger(l, 1);
  const std::string &destination_point_name = lua_tostring(l, 2);
  Transition::Style transition_style = Transition::Style(lua_tointeger(l, 3));

  script->game.set_current_map(map_id, destination_point_name, transition_style);

  return 0;
}

/**
 * @brief Sets the direction of the hero's sprite.
 *
 * - Argument 1 (integer): the direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_hero_set_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  int direction = lua_tointeger(l, 1);

  script->hero.set_animation_direction(direction);

  return 0;
}

/**
 * @brief Places the hero on the exact position of a sensor's name.
 *
 * - Argument 1 (string): name of the sensor
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_hero_align_on_sensor(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Sensor *sensor = (Sensor*) entities->get_entity(SENSOR, name);
  script->hero.set_xy(sensor->get_xy());

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
int MapScript::l_hero_walk(lua_State *l) {

  called_by_script(l, 3, NULL);

  /* TODO
  const std::string &path = lua_tostring(l, 1);
  bool loop = lua_toboolean(l, 2) != 0;
  bool ignore_obstacles = lua_toboolean(l, 3) != 0;

  script->hero->walk(path, loop, ignore_obstacles);
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
int MapScript::l_hero_jump(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  int direction = lua_tointeger(l, 1);
  int length = lua_tointeger(l, 2);
  bool ignore_obstacles = lua_toboolean(l, 3) != 0;

  script->hero.start_jumping(direction, length, ignore_obstacles, false);

  return 0;
}

/**
 * @brief Makes the hero brandish his sword meaning a victory
 * and plays the corresponding sound.
 * @param l the Lua context that is calling this function
 */
int MapScript::l_hero_start_victory_sequence(lua_State *l) {
  
  MapScript *script;
  called_by_script(l, 0, &script);

  script->hero.start_victory();

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
int MapScript::l_hero_set_visible(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  bool visible = lua_toboolean(l, 1) != 0;

  script->hero.set_visible(visible);

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
int MapScript::l_npc_get_position(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
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
int MapScript::l_npc_set_position(lua_State *l) {

  MapScript *script;
  called_by_script(l, 3, &script);

  const std::string &name = lua_tostring(l, 1);
  int x = lua_tointeger(l, 2);
  int y = lua_tointeger(l, 3);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_xy(x, y);

  return 0;
}

/**
 * @brief Makes an NPC walk with respect to a path.
 *
 * - Argument 1 (string): name of the NPC to make move
 * - Argument 2 (string): the path (each character is a direction between '0' and '7'
 * - Argument 3 (boolean): true to make the movement loop
 * - Argument 4 (boolean): true to make the movement ignore obstacles
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_walk(lua_State *l) {

  MapScript *script;
  called_by_script(l, 4, &script);

  const std::string &name = lua_tostring(l, 1);
  const std::string &path = lua_tostring(l, 2);
  bool loop = lua_toboolean(l, 3) != 0;
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->walk(path, loop, ignore_obstacles);

  return 0;
}

/**
 * @brief Makes an NPC walk randomly.
 *
 * - Argument 1 (string): name of the NPC to make move
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_random_walk(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->walk_random();

  return 0;
}

/**
 * @brief Makes an NPC jump into a direction.
 *
 * The NPC's sprite must have an animation "jumping".
 * - Argument 1 (string): name of the NPC to make move
 * - Argument 2 (integer): the jump direction, between 0 and 7
 * - Argument 3 (integer): the jump length in pixels
 * - Argument 4 (boolean): true to make the movement ignore obstacles
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_jump(lua_State *l) {

  MapScript *script;
  called_by_script(l, 4, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);
  int length = lua_tointeger(l, 3);
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->jump(direction, length, ignore_obstacles);

  return 0;
}

/**
 * @brief Sets the animation of an NPC's sprite.
 *
 * - Argument 1 (string): name of the NPC
 * - Argument 2 (string): name of the animation to set
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_set_animation(lua_State *l) {
  return l_interactive_entity_set_animation(l);
}

/**
 * @brief Sets whether the animation of an NPC should continue even when the game is suspended.
 *
 * Argument 1 (string): name of the NPC
 * Argument 2 (boolean): true to ignore when the game is suspended
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_set_animation_ignore_suspend(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool ignore_suspend = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  MapEntity *npc = entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_animation_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * @brief Sets the direction of an NPC's sprite.
 *
 * - Argument 1 (string): name of the NPC
 * - Argument 2 (integer): the sprite's direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_set_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);

  Debug::assert(direction >= 0 && direction < 4, StringConcat() << "Invalid NPC direction: " << direction);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *npc = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  npc->set_sprite_direction(direction);

  return 0;
}

/**
 * @brief Removes an NPC from the map.
 *
 * - Argument 1 (string): name of the NPC
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_npc_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  entities->remove_entity(INTERACTIVE_ENTITY, name);

  return 0;
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
int MapScript::l_chest_set_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
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
int MapScript::l_chest_set_hidden(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool hidden = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
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
int MapScript::l_chest_is_hidden(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Chest *chest = (Chest*) entities->get_entity(CHEST, name);
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
int MapScript::l_tile_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities->get_entity(DYNAMIC_TILE, name);
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
int MapScript::l_tile_set_group_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *dynamic_tiles = entities->get_entities_with_prefix(DYNAMIC_TILE, prefix);

  std::list<MapEntity*>::iterator it;
  for (it = dynamic_tiles->begin(); it != dynamic_tiles->end(); it++) {
    DynamicTile *dynamic_tile = (DynamicTile*) (*it);
    dynamic_tile->set_enabled(enable);
  }

  delete dynamic_tiles;
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
int MapScript::l_tile_is_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  DynamicTile *dynamic_tile = (DynamicTile*) entities->get_entity(DYNAMIC_TILE, name);
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
int MapScript::l_block_reset(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &block_name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Block *block = (Block*) entities->get_entity(BLOCK, block_name);
  block->reset();

  return 0;
}

/**
 * @brief Replaces all blocks of the map at their initial position.
 * @param l the Lua context that is calling this function
 */
int MapScript::l_block_reset_all(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *blocks = entities->get_entities(BLOCK);

  std::list<MapEntity*>::iterator i;
  for (i = blocks->begin(); i != blocks->end(); i++) {
    ((Block*) (*i))->reset();
  }

  delete blocks;

  return 0;
}

/**
 * @brief Returns the current animation of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (string): name of the current animation
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_get_animation(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  std::string animation = entity->get_sprite()->get_current_animation();

  lua_pushstring(l, animation.c_str());

  return 1;
}

/**
 * @brief Returns the animation speed of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (integer): delay between two frames in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_get_animation_delay(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  uint32_t delay = entity->get_sprite()->get_frame_delay();

  lua_pushinteger(l, delay);

  return 1;
}

/**
 * @brief Returns the current animation frame of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (integer): frame number
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_get_animation_frame(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  int frame = entity->get_sprite()->get_current_frame();

  lua_pushinteger(l, frame);

  return 1;
}

/**
 * @brief Returns the current direction of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (integer): the direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_get_direction(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  int frame = entity->get_sprite()->get_current_direction();

  lua_pushinteger(l, frame);

  return 1;
}

/**
 * @brief Returns whether the animation of an interactive entity's sprite is paused.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Return value (boolean): true if the animation is paused
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_is_animation_paused(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  bool paused = entity->get_sprite()->is_paused();

  lua_pushboolean(l, paused ? 1 : 0);

  return 1;
}

/**
 * @brief Sets the animation of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (string): name of the animation to set
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_animation(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  const std::string &animation = lua_tostring(l, 2);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_animation(animation);
  entity->get_sprite()->restart_animation();

  return 0;
}

/**
 * @brief Sets the animation speed of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (integer): delay between two frames in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_animation_delay(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  uint32_t delay = lua_tointeger(l, 2);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_frame_delay(delay);

  return 0;
}

/**
 * @brief Sets the current animation frame of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (integer): frame number
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_animation_frame(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int frame = lua_tointeger(l, 2);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_frame(frame);

  return 0;
}

/**
 * @brief Sets the direction of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (integer): the sprite's direction between 0 and 3
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_direction(lua_State *l) {

  return l_npc_set_direction(l);
}

/**
 * @brief Pauses or resumes the animation of an interactive entity's sprite.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (boolean): true to pause, false to resume
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_animation_paused(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool paused = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_paused(paused);

  return 0;
}

/**
 * @brief Starts a fade-in or a fade-out effect on an interactive entity
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (integer): direction of the effect: 0 for fade-in, 1 for fade-out
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_fade(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);

  MapEntities *entities = script->map.get_entities();
  InteractiveEntity *entity = (InteractiveEntity*) entities->get_entity(INTERACTIVE_ENTITY, name);
  entity->start_fading(direction);

  return 0;
}

/**
 * @brief Sets whether the animation of an interactive entity should continue even when the game is suspended.
 *
 * - Argument 1 (string): name of the interactive entity
 * - Argument 2 (boolean): true to ignore when the game is suspended
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_set_animation_ignore_suspend(lua_State *l) {
  return l_npc_set_animation_ignore_suspend(l);
}

/**
 * @brief Removes an interactive entity from the map.
 *
 * - Argument 1 (string): name of the interactive entity
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_interactive_entity_remove(lua_State *l) {
  return l_npc_remove(l);
}

/**
 * @brief Removes a shop item from the map.
 *
 * - Argument 1 (string): name of the shop item
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_shop_item_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  script->map.get_entities()->remove_entity(SHOP_ITEM, name);

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
int MapScript::l_switch_is_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);

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
int MapScript::l_switch_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);
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
int MapScript::l_switch_set_locked(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool lock = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  Switch *sw = (Switch*) entities->get_entity(SWITCH, name);
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
int MapScript::l_enemy_is_dead(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name);

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
int MapScript::l_enemy_is_group_dead(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *enemies = entities->get_entities_with_prefix(ENEMY, prefix);

  lua_pushboolean(l, enemies->empty());

  delete enemies;
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
int MapScript::l_enemy_set_enabled(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &name = lua_tostring(l, 1);
  bool enable = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  Enemy *enemy = (Enemy*) entities->get_entity(ENEMY, name);
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
int MapScript::l_enemy_start_boss(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name); 
  entities->start_boss_battle(enemy);

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
int MapScript::l_enemy_end_boss(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->map.get_entities()->end_boss_battle();

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
int MapScript::l_enemy_start_miniboss(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Enemy *enemy = (Enemy*) entities->find_entity(ENEMY, name); 
  entities->start_miniboss_battle(enemy);

  return 0;
}

/**
 * @brief Ends the battle against a miniboss.
 *
 * Calling this function plays the appropriate music.
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_enemy_end_miniboss(lua_State *l) {

  MapScript *script;
  called_by_script(l, 0, &script);

  script->map.get_entities()->end_miniboss_battle();

  return 0;
}

/**
 * @brief Removes a sensor from the map.
 *
 * - Argument 1 (string): name of the sensor
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_sensor_remove(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &name = lua_tostring(l, 1);

  script->map.get_entities()->remove_entity(SENSOR, name);

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
int MapScript::l_door_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->open();
  }
  script->game.play_sound("door_open");
  delete doors;

  return 0;
}

/**
 * @brief Closes one or several doors.
 *
 * The doors must be normal, open door
 * (not doors for keys or bombs).
 * - Argument 1 (string): prefix of the name of the doors to close
 *
 * @param l the Lua context that is calling this function
 */
int MapScript::l_door_close(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);

  const std::string &prefix = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->close();
  }
  script->game.play_sound("door_closed");
  delete doors;

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
int MapScript::l_door_is_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 1, &script);
  const std::string &name = lua_tostring(l, 1);

  MapEntities *entities = script->map.get_entities();
  Door *door = (Door*) entities->get_entity(DOOR, name);
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
int MapScript::l_door_set_open(lua_State *l) {

  MapScript *script;
  called_by_script(l, 2, &script);

  const std::string &prefix = lua_tostring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  MapEntities *entities = script->map.get_entities();
  std::list<MapEntity*> *doors = entities->get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors->begin(); it != doors->end(); it++) {
    Door *door = (Door*) (*it);
    door->set_open(open);
  }
  delete doors;

  return 0;
}

