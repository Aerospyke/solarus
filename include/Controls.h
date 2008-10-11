#ifndef ZSDX_CONTROLS_H
#define ZSDX_CONTROLS_H

#include "Common.h"
#include <map>

/**
 * Represents the game controls.
 * This class handles the customization of the keyboard and joypad keys.
 * It receives the SDL keyboard and joypad events that occur during
 * the game and notifies the appropriate objects
 * that a key was pressed, depending on the game situation.
 */
class Controls {

 public:

  /**
   * The game keys that can be customized.
   */
  enum GameKey {
    NONE = 0,
    ACTION,
    SWORD,
    ITEM_1,
    ITEM_2,
    PAUSE,
    RIGHT,
    UP,
    LEFT,
    DOWN
  };
  
 private:

  /**
   * The different kinds of joypad controls.
   */
  enum JoypadControlType {
    BUTTON,
    AXIS,
    HAT,
  };

  /**
   * Represent a joypad event that can be associated
   * to a customizable game key.
   */
  struct JoypadControl {

    JoypadControlType type;

    union {
      int button;      // button pressed
      struct {
	int index;     // index of the hat or the axis
	int direction; // 0 to 3
      } movement;
    } control;
  };

  Game *game;
  Savegame *savegame;

  map<SDLKey, GameKey> keyboard_mapping;
  map<string, GameKey> joypad_mapping;

  bool customizing;
  GameKey key_to_customize;

  void game_key_pressed(GameKey key);
  void game_key_released(GameKey key);

  // keyboard mapping
  void key_pressed(const SDL_keysym &keysym);
  void key_released(const SDL_keysym &keysym);
  SDLKey get_keyboard_key(GameKey game_key);
  string get_joypad_action(GameKey game_key);

 public:

  // creation and destruction
  Controls(Game *game);
  ~Controls(void);

  // controls
  string get_key_name(GameKey game_key);
  string get_keyboard_string(GameKey game_key);
  string get_joypad_string(GameKey key);
  void handle_event(const SDL_Event &event);
  bool is_key_pressed(GameKey game_key);

  // customization
  void customize(GameKey key);
  bool is_customization_done(void);
};

#endif
