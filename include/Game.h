#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "Common.h"
#include "Screen.h"
#include "Transition.h"

/**
 * This class handles the game.
 * The game has a current map, can be suspended or not.
 */
class Game: public Screen {

 private:

  /**
   * The game data saved.
   */
  Savegame *savegame;

  /**
   * Link.
   */
  Link *link;
  Movement8ByPlayer *link_movement;

  /**
   * True if the game is paused.
   */
  bool paused;

  /**
   * The dialog box currently shown
   * or NULL if no message is being shown.
   */
  DialogBox *dialog_box;

  /**
   * The treasure currently being given to the player
   * or NULL if it is not the case.
   */
  Treasure *treasure;

  /**
   * Current effect associated to the main game keys
   * (represented on the HUD by the action icon, the objects icons, etc.).
   */
  KeysEffect *keys_effect;

  /**
   * This object receives the SDL keyboard events and treats them.
   */
  Keyboard *keyboard;

  /**
   * The map currently displayed.
   */
  Map *current_map;
  
  /**
   * The map where Link is going to.
   * If not NULL, it means that Link is changing from
   * current_map to next_map.
   */
  Map *next_map;

  /**
   * The transition between the current map and the next one.
   */
  Transition::Style transition_style;
  Transition *transition; // NULL if no transition is playing

  /**
   * The game HUD (displaying hearts, rupees, etc.).
   */
  HUD *hud;

  /**
   * Id of the music currently played (a valid music, or MUSIC_NONE if no music is being played).
   */
  MusicId current_music_id;

  /**
   * The music currently played (a valid music, or NULL if no music is being played).
   */
  Music *current_music;

  void update_keys_effect(void);
  void update_dialog_box(void);
  void update_transitions(void);
  void update_treasure(void);

 public:

  Game(Savegame *savegame);
  ~Game(void);

  Savegame *get_savegame(void);
  Link *get_link(void);

  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);

  Map *get_current_map(void);
  void set_current_map(MapId map_id, unsigned int entrance_index, Transition::Style transition_style);
  void set_current_map(MapId map_id, string entrance_name, Transition::Style transition_style);

  bool is_paused(void);
  bool is_showing_message(void);
  bool is_playing_transition(void);
  bool is_suspended(void);
  KeysEffect *get_keys_effect(void);

  void play_music(MusicId new_music_id);
  void pause_or_resume_music(void);
  void stop_music(void);

  DialogBox *get_dialog_box(void);
  void show_message(MessageId message_id);

  void give_treasure(Treasure *treasure);
  bool is_giving_treasure(void);

  void set_paused(bool paused);
};

#endif
