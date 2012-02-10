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
#ifndef SOLARUS_GAME_H
#define SOLARUS_GAME_H

#include "Common.h"
#include "Screen.h"
#include "Transition.h"
#include "GameControls.h"

/**
 * @brief Represents the game currently running.
 *
 * The game is the main screen of the program.
 * It shows the current map and displays all game elements:
 * the map entities, the HUD, the pause menu, the dialog box, etc.
 */
class Game: public Screen {

  private:

    // savegame
    Savegame *savegame;        /**< the game data saved */

    // the hero
    Hero *hero;

    // current game state (elements currently shown)
    bool pause_key_available;  /**< indicates that the player is allowed to use the pause key */
    PauseMenu *pause_menu;     /**< the current pause menu, or NULL if the game is not paused */
    Treasure *treasure;        /**< the treasure currently being given to the player or NULL if it is not the case */
    GameoverSequence *gameover_sequence; /**< the game over sequence (if currently shown) */
    bool reseting;             /**< true if the game will be reset */
    bool restarting;           /**< true if the game will be restarted */

    // controls
    GameControls *controls;    /**< this object receives the keyboard and joypad events */
    KeysEffect *keys_effect;   /**< current effect associated to the main game keys
				* (represented on the HUD by the action icon, the objects icons, etc.) */

    // map
    Map *current_map;          /**< the map currently displayed */
    Map *next_map;             /**< the map where the hero is going to; if not NULL, it means that the hero 
				* is changing from current_map to next_map */
    Surface *previous_map_surface;  /**< a copy of the previous map surface for transition effects that display two maps */

    Transition::Style transition_style; /**< the transition style between the current map and the next one */
    Transition *transition;             /**< the transition currently shown, or NULL if no transition is playing */

    // world (i.e. the current set of maps)
    Dungeon *dungeon;          /**< the dungeon of the current map, of NULL if we are not in a dungeon */
    bool crystal_switch_state; /**< indicates that a crystal switch has been enabled (i.e. the orange blocks are raised) */
    static Rectangle outside_world_size; /**< size of the outside world in pixels */

    // graphics
    HUD *hud;                  /**< the game HUD (displaying hearts, rupees, key icons, etc.) */
    bool hud_enabled;          /**< true if the HUD is currently displayed */
    DialogBox *dialog_box;     /**< the dialog box manager */

    // music
    MusicId current_music_id;  /**< id of the music currently played (a valid music,
				* or MUSIC_NONE if no music is being played) */
    Music *current_music;      /**< the music currently played, or NULL if no music is being played */
    MusicId previous_music_id; /**< id of the previous music played (useful after a mini-boss) */

    // update functions
    void update_dialog_box(void);
    void update_transitions(void);
    void update_treasure(void);
    void update_gameover_sequence(void);

    void load_dungeon(void);

  public:

    // creation and destruction
    Game(Solarus *solarus, Savegame *savegame);
    ~Game(void);

    // global objects
    Hero *get_hero(void);
    const Rectangle & get_hero_xy(void);
    GameControls *get_controls(void);
    KeysEffect *get_keys_effect(void);
    Savegame *get_savegame(void);
    Equipment *get_equipment(void);
    DungeonEquipment *get_dungeon_equipment(void);
    MapScript *get_current_script(void);

    // functions called by the main loop
    void notify_event(InputEvent &event);
    void update(void);
    void display(Surface *screen_surface);

    // game controls
    void key_pressed(GameControls::GameKey key);
    void key_released(GameControls::GameKey key);

    // map
    Map *get_current_map(void);
    void set_current_map(MapId map_id, const std::string &destination_point_name,
	Transition::Style transition_style);

    // world
    const Rectangle & get_outside_world_size(void);
    bool is_in_dungeon(void);
    Dungeon *get_current_dungeon(void);
    bool get_crystal_switch_state(void);
    void change_crystal_switch_state(void);

    // audio
    void play_sound(SoundId sound_id);
    void play_music(MusicId new_music_id);
    void pause_or_resume_music(void);
    void stop_music(void);
    void restore_music(void);
    const MusicId & get_current_music_id(void);

    // current game state
    bool is_paused(void);
    bool is_showing_message(void);
    bool is_playing_transition(void);
    bool is_showing_gameover(void);
    bool is_suspended(void); // true if at least one of the three functions above returns true
    void reset(void);
    void restart(void);

    // HUD
    void set_hud_enabled(bool hud_enabled);

    // pause
    bool can_pause(void);
    bool is_pause_key_available(void);
    void set_pause_key_available(bool pause_key_available);
    void set_paused(bool paused);
    PauseMenu *get_pause_menu(void);

    // dialog box
    DialogBox *get_dialog_box(void);

    // treasure
    void give_treasure(Treasure *treasure);
    bool is_giving_treasure(void);

    // game over
    void start_gameover_sequence(void);
    void get_back_from_death(void);
};

#endif

