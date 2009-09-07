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
#include "menus/SelectionMenuPhase.h"
#include "menus/SelectionMenuCommon.h"
#include "ZSDX.h"
#include "Transition.h"
#include "Sound.h"
#include "Sprite.h"
#include "TextSurface.h"
#include "StringResource.h"
#include "hud/HeartsView.h"

/**
 * Creates a selection menu phase.
 * @param previous the previous phase (if we were already in the selection menu), or NULL
 * @param title_string_key key of the title string to display in this phase
 */
SelectionMenuPhase::SelectionMenuPhase(SelectionMenuPhase *previous, const std::string &title_string_key):
  Screen() {

  if (previous != NULL) {
    this->common_part = previous->common_part;
  }
  else {
    this->common_part = new SelectionMenuCommon();
  }

  this->title_string = StringResource::get_string(title_string_key);
  this->destination_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  this->transition = NULL;
}

/**
 * Destroys the selection menu phase.
 * If there was a transition, it is destroyed.
 */
SelectionMenuPhase::~SelectionMenuPhase(void) {

  SDL_FreeSurface(destination_surface);

  if (transition != NULL) {
    delete transition;
  }

  if (zsdx->is_exiting() || zsdx->game != NULL) {
    delete common_part;
  }
}

/**
 * Sets the two options to display in the bottom of the screen.
 * @param left_string_key key of the first option string (on the left)
 * @param right_string_key key of the second option string (on the right)
 */
void SelectionMenuPhase::set_bottom_options(const std::string &left_string_key, const std::string &right_string_key) {

  common_part->text_option1->set_text(StringResource::get_string(left_string_key));
  common_part->text_option2->set_text(StringResource::get_string(right_string_key));
}


/**
 * Updates the selection menu in this phase.
 * The subclasses should call this function to
 * update the elements common to all phases of
 * the selection menu.
 */
void SelectionMenuPhase::update(void) {

  // update the elements common to all phases
  common_part->update();

  // remove the transition if necessary
  if (transition != NULL && transition->is_over()) {
    delete transition;
    transition = NULL;
  }
}

/**
 * Starts to display the selection menu in this phase.
 * The subclasses should call this function to
 * display the elements common to all the phases of
 * the selection menu, then display their own elements,
 * and finally call finish_display().
 * @param screen_surface surface where this screen has to be drawn
 */
void SelectionMenuPhase::start_display(SDL_Surface *screen_surface) {

  // display the elements common to all phases
  common_part->display(destination_surface);

  // title
  display_title_text();
}

/**
 * Validates the displaying of the selection menu in this phase.
 * The subclasses should call display() to
 * display the elements common to all the phases of
 * the selection menu, then display their own elements,
 * and finally call finish_display().
 * @param screen_surface surface where this screen has to be drawn
 */
void SelectionMenuPhase::finish_display(SDL_Surface *screen_surface) {

  // transition
  if (transition != NULL && transition->is_started()) {
    transition->display(destination_surface);
  }

  SDL_BlitSurface(destination_surface, NULL, screen_surface, NULL);
}

/**
 * Plays the "cursor" sound.
 */
void SelectionMenuPhase::play_cursor_sound(void) {
  common_part->cursor_sound->play();
}

/**
 * Plays the "ok" sound.
 */
void SelectionMenuPhase::play_ok_sound(void) {
  common_part->ok_sound->play();
}

/**
 * Plays the "error" sound.
 */
void SelectionMenuPhase::play_error_sound(void) {
  common_part->error_sound->play();
}

/**
 * Returns the keys effects.
 * @return the keys effects
 */
KeysEffect * SelectionMenuPhase::get_keys_effect(void) {
  return common_part->keys_effect;
}

/**
 * Returns the 3 savegames.
 * @return the 3 savegames
 */
Savegame ** SelectionMenuPhase::get_savegames(void) {
  return common_part->savegames;
}

/**
 * Returns the specified savegame.
 * @param save_number index of the savegame to get (0 to 2)
 * @return the savegame
 */
Savegame * SelectionMenuPhase::get_savegame(int save_number) {
  return common_part->savegames[save_number];
}

/**
 * Reloads the savegames.
 * The subclasses should function has to be called when
 * they create or remove a savegame.
 */
void SelectionMenuPhase::reload_savegames(void) {
  common_part->read_savegames();
}

/**
 * Returns the cursor's sprite.
 * @return the cursor's sprite
 */
Sprite * SelectionMenuPhase::get_cursor(void) {
  return common_part->cursor;
}

/**
 * Returns the current position of the cursor.
 * @return the current position of the cursor
 */
int SelectionMenuPhase::get_cursor_position(void) {
  return common_part->cursor_position;
}

/**
 * Sets the position of the cursor.
 * @param position the new position of the cursor
 */
void SelectionMenuPhase::set_cursor_position(int position) {
  common_part->cursor_position = position;
}

/**
 * This function is called when the user wants to move the cursor
 * upwards (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_up(void) {

  common_part->cursor_sound->play();
  common_part->cursor->restart_animation();
  common_part->cursor_position--;

  if (common_part->cursor_position == 0) {
    common_part->cursor_position = 4;
  }
  else if (common_part->cursor_position == 4) {
    common_part->cursor_position = 3;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * downwards (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_down(void) {

  common_part->cursor_sound->play();
  common_part->cursor->restart_animation();
  common_part->cursor_position++;

  if (common_part->cursor_position >= 5) {
    common_part->cursor_position = 1;
  }
}

/**
 * This function is called when the user wants to move the cursor
 * to the left or to the right (except for the letter cursor).
 */
void SelectionMenuPhase::move_cursor_left_or_right(void) {

  if (common_part->cursor_position == 4) {
    common_part->cursor_sound->play();
    common_part->cursor->restart_animation();
    common_part->cursor_position = 5;
  }
  else if (common_part->cursor_position == 5) {
    common_part->cursor_sound->play();
    common_part->cursor->restart_animation();
    common_part->cursor_position = 4;
  }
}

/**
 * Draws the title string on destination_surface.
 */
void SelectionMenuPhase::display_title_text(void) {
  
  common_part->text_title->set_text(title_string);
  common_part->text_title->display(destination_surface);
}

/**
 * Displays a savegame on the destination_surface.
 * @param save_number number of the savegame to display (0 to 2)
 */
void SelectionMenuPhase::display_savegame(int save_number) {

  display_savegame(save_number, destination_surface);
}

/**
 * Displays a savegame on the specified surface.
 * @param save_number number of the savegame to display (0 to 2)
 * @param destination_surface the surface where you want to display
 * the savegame (may be different from this->destination_surface)
 */
void SelectionMenuPhase::display_savegame(int save_number, SDL_Surface *destination_surface) {

  SDL_Rect position;

  // draw the container
  position.x = 57;
  position.y = 75 + save_number * 27;
  SDL_BlitSurface(common_part->img_save_container, NULL, destination_surface, &position);

  // draw the player's name
  common_part->text_player_names[save_number]->display(destination_surface);

  // draw the hearts
  common_part->hearts_views[save_number]->display(destination_surface);
}

/**
 * Displays the number of a savegame on destination_surface.
 * This function is separate from the display_savegame() function
 * because the cursor has to be displayed after the savegame images
 * but before the savegame number.
 * @param save_number number to display (0 to 2)
 */
void SelectionMenuPhase::display_savegame_number(int save_number) {

  display_savegame_number(save_number, destination_surface);
}

/**
 * Displays the number of a savegame.
 * This function is separate from the display_savegame() function
 * because the cursor has to be displayed after the savegame images
 * but before the savegame number.
 * @param save_number number to display (0 to 2)
 * @param destination_surface the surface where you want to display
 * the savegame (may be different from this->destination_surface)
 */
void SelectionMenuPhase::display_savegame_number(int save_number, SDL_Surface *destination_surface) {

  SDL_Rect position;

  position.x = 62;
  position.y = 80 + 27 * save_number;
  SDL_BlitSurface(common_part->img_numbers[save_number], NULL, destination_surface, &position);
}

/**
 * Displays the two options in the bottom of the screen.
 * @param left_string text of the first option (on the left)
 * @param right_string text of the second option (on the right)
 */
void SelectionMenuPhase::display_bottom_options(void) {

  SDL_Rect position;

  position.x = 57;
  position.y = 158;
  SDL_BlitSurface(common_part->img_option_container, NULL, destination_surface, &position);
  position.x = 165;
  SDL_BlitSurface(common_part->img_option_container, NULL, destination_surface, &position);

  common_part->text_option1->display(destination_surface);
  common_part->text_option2->display(destination_surface);
}

/**
 * Displays the normal cursor (i.e. the cursor to select a file)
 * at its current position.
 */
void SelectionMenuPhase::display_normal_cursor(void) {

  SDL_Rect position;

  if (common_part->cursor_position != 5) {
    position.x = 58;
  }
  else {
    position.x = 166;
  }

  if (common_part->cursor_position < 4) {
    position.y = 49 + common_part->cursor_position * 27;
  }
  else {
    position.y = 159;
  }
  common_part->cursor->display(destination_surface, position.x, position.y);
}
