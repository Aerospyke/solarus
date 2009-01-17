#include "MapScript.h"
#include "Map.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Game.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Savegame.h"
#include "Timer.h"
#include "entities/Detector.h"
#include <iomanip>
#include <lua5.1/lua.hpp>
#include <stdarg.h>

static void check_nb_arguments(lua_State *context, int nb_arguments);

/**
 * Declaration of all functions that can be called by the LUA code.
 */

typedef int (FunctionAvailableToScript) (lua_State *l);

static FunctionAvailableToScript l_play_sound,
  l_start_message, l_set_message_variable, l_give_treasure, l_give_treasure_with_amount, 
  l_get_savegame_string, l_get_savegame_integer, l_get_savegame_boolean, 
  l_set_savegame_string, l_set_savegame_integer, l_set_savegame_boolean, 
  l_start_timer;

/**
 * Creates the script of a map.
 * @param map the map
 */
MapScript::MapScript(Map *map) {

  // get the id of the map
  int id = (int) map->get_id();

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id << ".lua";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // create an execution context
  context = lua_open();
  luaL_openlibs(context);

  // register the C functions accessible to the script
  register_c_functions();

  // load the script
  if (luaL_dofile(context, file_name.c_str()) != 0) {
    DIE("Cannot load the script of map " << id << ": " << lua_tostring(context, -1));
  }
}

/**
 * Desctructor.
 */
MapScript::~MapScript(void) {

  // close the Lua execution context
  lua_close(context);

  // delete the timers
  std::list<Timer*>::iterator it;

  for (it = timers.begin(); it != timers.end(); it++) {
    delete *it;
  }
}

/**
 * Tells the Lua context what C functions it can call.
 */
void MapScript::register_c_functions(void) {

  lua_register(context, "play_sound", l_play_sound);
  lua_register(context, "start_message", l_start_message);
  lua_register(context, "set_message_variable", l_set_message_variable);
  lua_register(context, "give_treasure", l_give_treasure);
  lua_register(context, "give_treasure_with_amount", l_give_treasure_with_amount);
  lua_register(context, "get_savegame_string", l_get_savegame_string);
  lua_register(context, "get_savegame_integer", l_get_savegame_integer);
  lua_register(context, "get_savegame_boolean", l_get_savegame_boolean);
  lua_register(context, "set_savegame_string", l_set_savegame_string);
  lua_register(context, "set_savegame_integer", l_set_savegame_integer);
  lua_register(context, "set_savegame_boolean", l_set_savegame_boolean);
  lua_register(context, "start_timer", l_start_timer);
}

/**
 * Ensures the C function currently called by the script has the right number of arguments.
 * The program stops if the number of arguments is not correct
 * @param context the Lua context
 * @param nb_arguments the right number of arguments
 */
static void check_nb_arguments(lua_State *context, int nb_arguments) {

  if (lua_gettop(context) != nb_arguments) {
    DIE("Invalid number of arguments");
  }
}

/**
 * Calls a function without arguments in the script.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not need to handle.
 * @param function_name name of the function to call
 */
void MapScript::call_lua_function(const char *function_name) {
  call_lua_function(function_name, 0);
}

/**
 * Calls a function with arguments in the script.
 * The arguments must be of type char*.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not want to handle.
 * @param function_name name of the function to call
 */
void MapScript::call_lua_function(const char *function_name, int nb_arguments, ...) {

  lua_getglobal(context, function_name);
  if (lua_isfunction(context, -1)) {

    // the function exists: push the arguments
    va_list arg;
    va_start(arg, nb_arguments);
    for (int i = 0; i < nb_arguments; i++) {
      const char* value = va_arg(arg, char*);
      lua_pushstring(context, value);
    }
    va_end(arg);

    // call the function
    lua_call(context, nb_arguments, 0);
  }
  else {
    lua_pop(context, -1);
  }
}

/**
 * This function is called when the game is being suspended
 * or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void MapScript::set_suspended(bool suspended) {
  
  // suspend or resume the timers
  std::list<Timer*>::iterator it;
  for (it = timers.begin(); it != timers.end(); it++) {
    (*it)->set_suspended(suspended);
  }
}

/**
 * Updates the script.
 */
void MapScript::update(void) {

  // update the timers
  std::list<Timer*>::iterator it;

  for (it = timers.begin(); it != timers.end(); it++) {

    Timer *timer = *it;

    timer->update();
    if (timer->is_finished()) {
      call_lua_function(timer->get_callback_name());
      delete timer;
      timers.erase(it);
      it = timers.begin();
    }
  }

  // update the script
  call_lua_function("event_update");
}

/**
 * Adds a timer to the script.
 * @param timer the timer
 */
void MapScript::add_timer(Timer *timer) {
  timers.push_back(timer);
}

// functions that can be called by the Lua script

/**
 * Creates a dialog box and starts displaying a message.
 * If the message is followed by other messages, they are also
 * displayed, posting a message_started() event each time.
 * If the message (or one of its next messages) contains a variable,
 * then call set_message_variable() to specify its value.
 * Argument 1 (string): id of the message to display
 */
int l_start_message(lua_State *l) {

  check_nb_arguments(l, 1);
  MessageId message_id = lua_tostring(l, 1);

  zsdx->game->show_message(message_id);

  return 0;
}

/**
 * Set the value of the variable in a diabog box message.
 * The function has to be called after the dialog box is created,
 * i.e. after calling show_message.
 * Argument 1 (string): id of the message containing the variable
 * Argument 2 (string): value of the variable
 */
int l_set_message_variable(lua_State *l) {

  check_nb_arguments(l, 2);
  MessageId message_id = lua_tostring(l, 1);
  MessageId value = lua_tostring(l, 2);

  zsdx->game->get_dialog_box()->set_variable(message_id, value);

  return 0;
}

/**
 * Plays a sound.
 * Argument 1 (string): name of the sound
 */
int l_play_sound(lua_State *l) {

  check_nb_arguments(l, 1);
  SoundId sound_id = lua_tostring(l, 1);

  ResourceManager::get_sound(sound_id)->play();

  return 0;
}

/**
 * Gives a treasure to the player.
 * If the treasure comes from a chest, you don't have to call this function:
 * the treasure will be given to the player automatically when he opens the chest.
 * You can use this function to make a non-playing character
 * give a treasure to the player.
 * Argument 1 (integer): content of the treasure (see Treasure.h)
 * Argument 2 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int l_give_treasure(lua_State *l) {

  check_nb_arguments(l, 2);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int savegame_variable = lua_tointeger(l, 2);

  zsdx->game->give_treasure(new Treasure(content, savegame_variable));

  return 0;
}

/**
 * Gives a treasure to the player, specifying the amount.
 * This function should be called only for for treasures with an amount, like arrows, apples, etc.,
 * otherwise the amount parameter will be ignored.
 * For example you can use this function to make a non-playing character
 * give a treasure to the player.
 * Argument 1 (integer): content of the treasure (see Treasure.h)
 * Argument 2 (integer): amount to give
 * Argument 3 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int l_give_treasure_with_amount(lua_State *l) {

  check_nb_arguments(l, 3);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int amount = lua_tointeger(l, 2);
  int savegame_variable = lua_tointeger(l, 3);

  zsdx->game->give_treasure(new Treasure(content, amount, savegame_variable));

  return 0;
}

/**
 * Returns a string value saved.
 * Argument 1 (integer): index of the string value to get (0 to 63)
 * Return value (string): the string saved at this index
 */
int l_get_savegame_string(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  string value = zsdx->game->get_savegame()->get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * Returns an integer value saved.
 * Argument 1 (integer): index of the integer value to get (0 to 2047)
 * Return value (integer): the integer saved at this index
 */
int l_get_savegame_integer(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  int value = zsdx->game->get_savegame()->get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * Returns a boolean value saved.
 * Argument 1 (integer): index of the boolean value to get
 * Return value (boolean): the boolean saved at this index
 */
int l_get_savegame_boolean(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  bool value = zsdx->game->get_savegame()->get_string(index);
  lua_pushboolean(l, value ? 1 : 0);

  return 1;
}

/**
 * Sets a string value saved.
 * Argument 1 (integer): index of the string value to set, between 32 and 63
 * (lower indices are writable only by the game engine)
 * Argument 2 (string): the string value to store at this index
 */
int l_set_savegame_string(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  const char *value = lua_tostring(l, 2);

  if (index < 32) {
    DIE("Cannot change savegame string '" << index << "': string variables below 32 are read-only");
  }

  zsdx->game->get_savegame()->set_string(index, value);

  return 0;
}

/**
 * Sets an integer value saved.
 * Argument 1 (integer): index of the integer value to set, between 1024 and 2047
 * (lower indices are writable only by the game engine)
 * Argument 2 (integer): the integer value to store at this index
 */
int l_set_savegame_integer(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  int value = lua_tointeger(l, 2);

  if (index < 1024) {
    DIE("Cannot change savegame integer '" << index << "': integer variables below 1024 are read-only");
  }

  zsdx->game->get_savegame()->set_integer(index, value);

  return 0;
}

/**
 * Sets a boolean value saved.
 * Argument 1 (integer): index of the boolean value to set, between 0 and 32767
 * Argument 2 (boolean): the boolean value to store at this index
 */
int l_set_savegame_boolean(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  int value = lua_toboolean(l, 2);

  zsdx->game->get_savegame()->set_boolean(index, value != 0);

  return 0;
}

/**
 * Starts a timer to run a Lua function after a delay.
 * Argument 1 (integer): the timer duration in milliseconds
 * Argument 2 (string): name of the Lua function to call when the timer is finished
 * (no argument, no return value)
 * Argument 3 (boolean): plays a sound until the timer expires
 */
int l_start_timer(lua_State *l) {

  check_nb_arguments(l, 3);
  Uint32 duration = lua_tointeger(l, 1);
  const char *callback_name = lua_tostring(l, 2);
  bool with_sound = lua_toboolean(l, 3) != 0;

  Timer *timer = new Timer(duration, callback_name, with_sound);
  MapScript *script = zsdx->game->get_current_script();
  script->add_timer(timer);

  return 0;
}

// event functions, i.e. functions called by the C++ engine to notify the map script that something happened

/**
 * Notifies the script that the map has just been started.
 */
void MapScript::event_map_started(void) {
  call_lua_function("event_map_started");
}

/**
 * Notifies the script that a message has just started to be displayed
 * in the dialog box.
 * @param messageId id of the message
 */
void MapScript::event_message_started(MessageId messageId) {
  call_lua_function("event_message_started", 1, messageId.c_str());
}

/**
 * Notifies the script that an entity is overlapping a detector.
 * Note that all detectors don't notify the script (i.e. they don't call this function)
 * because they may have their own behavior.
 * @param detector the detector
 * @param entity the entity
 */
void MapScript::event_entity_on_detector(Detector *detector, MapEntity *entity) {
  call_lua_function("event_entity_on_detector", 2, detector->get_name().c_str(), entity->get_name().c_str());
}
