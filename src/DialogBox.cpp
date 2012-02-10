/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "DialogBox.h"
#include "Message.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "lua/MapScript.h"
#include "entities/Hero.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"

/**
 * @brief Creates a new dialog box.
 * @param game the game this dialog box belongs to
 */
DialogBox::DialogBox(Game &game):
  game(game),
  current_message(NULL) {

  // initialize the surface
  dialog_surface = new Surface(320, 240);
  dialog_surface->set_transparency_color(Color::get_black());

  // load the images
  box_img = new Surface("hud/dialog_box.png");
  icons_img = new Surface("hud/dialog_icons.png");
  box_src_position = Rectangle(0, 0, 220, 60);
  box_dst_position.set_size(box_src_position);
  question_src_position = Rectangle(96, 60, 8, 8);

  // create the sprites
  end_message_sprite = new Sprite("hud/dialog_box_message_end");

  set_style(STYLE_WITH_FRAME);
}

/**
 * @brief Destructor.
 */
DialogBox::~DialogBox() {

  // free the memory
  delete dialog_surface;
  delete box_img;
  delete icons_img;
  delete end_message_sprite;
  delete current_message;
}

/**
 * @brief Returns the game where this dialog box is displayed.
 * @return the current game
 */
Game& DialogBox::get_game() {
  return game;
}

/**
 * @brief Returns whether the dialog box is currently active.
 * @return true if the dialog box is enabled
 */
bool DialogBox::is_enabled() {
  return current_message != NULL;
}

/**
 * @brief Sets the dialog box style for all subsequent dialogs.
 *
 * The default style is DialogBox::STYLE_WITH_FRAME.
 *
 * @param style the new style to set
 */
void DialogBox::set_style(Style style) {

  this->style = style;

  if (style != STYLE_WITHOUT_FRAME) {
    dialog_surface->set_opacity(216);
  }
}

/**
 * @brief Sets the vertical position of the dialog box.
 * @param vertical_position the vertical position
 */
void DialogBox::set_vertical_position(VerticalPosition vertical_position) {

  if (vertical_position == POSITION_AUTO) {
    // determine the position
    const Rectangle &camera_position = game.get_current_map().get_camera_position();
    vertical_position = POSITION_BOTTOM;

    if (game.get_hero().get_y() >= camera_position.get_y() + 130) {
      vertical_position = POSITION_TOP;
    }
  }

  // set the coordinates of graphic objects
  int x = 50;
  int y = (vertical_position == POSITION_TOP) ? 32 : 144;

  if (style == STYLE_WITHOUT_FRAME) {
    y += (vertical_position == POSITION_TOP) ? (-24) : 24;
  }

  box_dst_position.set_xy(x, y);
  question_dst_position.set_xy(x + 18, y + 27);
  icon_dst_position.set_xy(x + 18, y + 22);
}

/**
 * @brief Returns the speed of the text.
 * @return the speed
 */
DialogBox::Speed DialogBox::get_speed() {
  return speed;
}

/**
 * @brief Sets the speed of the text.
 * @param speed the new speed
 */
void DialogBox::set_speed(Speed speed) {
  this->speed = speed;
}

/**
 * @brief Returns the current skip mode of the dialog box.
 * @return the skip mode
 */
Dialog::SkipMode DialogBox::get_skip_mode() {
  return skip_mode;
}

/**
 * @brief Sets the skip mode of the dialog box.
 * @param skip_mode the new skip mode
 */
void DialogBox::set_skip_mode(Dialog::SkipMode skip_mode) {
  this->skip_mode = skip_mode;
}

/**
 * @brief Returns the index of the icon displayed in this dialog box.
 *
 * If there is no icon, -1 is returned.
 *
 * @return the icon number
 */
int DialogBox::get_icon_number() {
  return icon_number;
}

/**
 * @brief Sets the index of the icon displayed in this dialog box,
 * @param icon_number the icon number, or -1 to remove any previous icon
 */
void DialogBox::set_icon_number(int icon_number) {
  this->icon_number = icon_number;
}

/**
 * @brief Returns whether a sound should be played when displaying the letters.
 * @return true if a sound should be played when displaying the letters
 */
bool DialogBox::is_letter_sound_enabled() {
  return style != STYLE_WITHOUT_FRAME;
}

/**
 * @brief Specifies the value of a variable that will occur in a dialog.
 *
 * A value is expected in a dialog when the '$v' sequence is read.
 * You can specify only one variable at the same time per dialog.
 * If a variable was already specified for this dialog, it is replaced.
 *
 * @param dialog_id id of the dialog where this value will appear
 * @param value the value to add
 */
void DialogBox::set_variable(const std::string& dialog_id, const std::string& value) {
  variables[dialog_id] = value;
}

/**
 * @brief Same thing as set_variable(string, string) but with an integer parameter.
 *
 * This function just converts the integer value to a string
 * add calls the other function.
 *
 * @param dialog id id of the dialog where this value will appear
 * @param value the value to set
 */
void DialogBox::set_variable(const std::string& dialog_id, int value) {

  std::ostringstream oss;
  oss << value;
  set_variable(dialog_id, oss.str());
}

/**
 * @brief Returns the variable specified by a previous
 * call to set_variable(), for the current dialog.
 * This function is called by
 * the current message when it reads the '$v' sequence.
 * @return the value of the variable
 */
const std::string& DialogBox::get_variable() {

  const std::string& value = variables[dialog_id];

  Debug::check_assertion(value.size() > 0, StringConcat()
      << "Missing variable in dialog '" << dialog_id << "'");

  return value;
}

/**
 * @brief Returns the answer selected by the player in the last message with a question.
 * @return the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
int DialogBox::get_last_answer() {
  return last_answer;
}

/**
 * @brief Remembers the answer selected by the player in the last dialog with a question.
 * @param answer the answer selected: 0 for the first one, 1 for the second one,
 * -1 if the last dialog was not a question
 */
void DialogBox::set_last_answer(int answer) {

  Debug::check_assertion(answer >= -1 && answer <= 1, StringConcat() << "Invalid value of answer: " << answer);
  this->last_answer = answer;
}

/**
 * @brief Starts a dialog.
 *
 * The dialog box should not be enabled already when you call this function.
 *
 * @param dialog_id of the dialog
 * @param issuer_script the script that issued the request to start a dialog
 * (will be notified when the dialog finishes), or NULL
 * @param vertical_position vertical position where to display the dialog box (default: auto)
 */
void DialogBox::start_dialog(const std::string& dialog_id, Script* issuer_script,
    VerticalPosition vertical_position) {

  Debug::check_assertion(!is_enabled(), StringConcat() <<
      "Cannot start dialog '" << dialog_id << ": the dialog box is already enabled");

  // save the action and sword keys
  KeysEffect& keys_effect = game.get_keys_effect();
  action_key_effect_saved = keys_effect.get_action_key_effect();
  sword_key_effect_saved = keys_effect.get_sword_key_effect();

  // initialize the dialog box with the default parameters
  set_vertical_position(vertical_position);
  set_speed(SPEED_FAST);
  set_skip_mode(Dialog::SKIP_NONE);
  set_icon_number(-1);
  this->skipped = false;
  this->dialog_id = dialog_id;
  show_message();

  // notify the scripts
  game.get_map_script().event_dialog_started(dialog_id);
  this->issuer_script = issuer_script;
  if (issuer_script != NULL) {
    issuer_script->event_dialog_started(dialog_id);
  }
}

/**
 * @brief Shows a new message in the dialog box.
 */
void DialogBox::show_message() {

  // create the message
  delete current_message;
  current_message = new Message(*this, dialog_id,
      box_dst_position.get_x(), box_dst_position.get_y());

  if (current_message->is_question()) {
    set_last_answer(0);
  }
  else {
    set_last_answer(-1);
  }
  question_dst_position.set_y(box_dst_position.get_y() + 27);
  
  // hide the action icon
  KeysEffect &keys_effect = game.get_keys_effect();
  keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (get_skip_mode() != Dialog::SKIP_NONE) {
    keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_SKIP);
    keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
  }
  else {
    keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
  }
  end_message_sprite->stop_animation();
}

/** 
 * @brief Shows the next message (if any).
 */
void DialogBox::show_next_message() {

  const std::string next_dialog_id; // TODO

  if (next_dialog_id != "" && next_dialog_id != "_unknown") {
    show_message();
  }
  else {
    close();
  }
}

/**
 * @brief Closes the dialog box.
 */
void DialogBox::close() {

  // the dialog box is being closed
  delete current_message;
  current_message = NULL;

  // restore the action and sword keys
  KeysEffect &keys_effect = game.get_keys_effect();
  keys_effect.set_action_key_effect(action_key_effect_saved);
  keys_effect.set_sword_key_effect(sword_key_effect_saved);

  // notify the script if necessary
  if (!skipped && dialog_id[0] != '_') { // FIXME: remove the '_' restriction
    // a dialog of the quest was just finished: notify the scripts
    Script& map_script = game.get_map_script();
    map_script.event_dialog_finished(dialog_id, last_answer);

    if (issuer_script != NULL && issuer_script != &map_script) {
      // also notify the issuer script if different
      issuer_script->event_dialog_finished(dialog_id, last_answer);
    }
  }
}

/**
 * @brief This function is called by the game when a key is pressed
 * while displaying a message.
 * @param key the key pressed
 */
void DialogBox::key_pressed(GameControls::GameKey key) {

  switch (key) {

    // action key
    case GameControls::ACTION:
      action_key_pressed();
      break;

      // sword key
    case GameControls::SWORD:
      sword_key_pressed();
      break;

    case GameControls::UP:
    case GameControls::DOWN:
      up_or_down_key_pressed();

    default:
      break;
  }
}

/**
 * @brief This function is called when the user presses the action key.
 */
void DialogBox::action_key_pressed() {

  if (current_message->is_finished()) { // the current message is over
    show_next_message();
  }
  else if (skip_mode != Dialog::SKIP_NONE) {
    show_all_now();
  }
}

/**
 * @brief This function is called when the user presses the sword key.
 */
void DialogBox::sword_key_pressed() {

  if (skip_mode == Dialog::SKIP_ALL) {
    skipped = true;
    close();
  }
  else if (current_message->is_finished()) {
    show_next_message();
  }
  else if (skip_mode == Dialog::SKIP_CURRENT) {
    show_all_now();
  }
}

/**
 * @brief This function is called when the user pressed the up or down arrow key.
 */
void DialogBox::up_or_down_key_pressed() {

  if (current_message->is_question() && current_message->is_finished()) {

    // switch the answer
    int answer = get_last_answer();
    set_last_answer(1 - answer);
    question_dst_position.set_y(box_dst_position.get_y() + ((answer == 1) ? 27 : 40));
    Sound::play("cursor");
  }
}

/**
 * @brief Shows immediately this message up to the end.
 * If the message was already finished, the next message starts.
 */
void DialogBox::show_all_now() {

  if (current_message->is_finished()) {
    show_next_message();
  }

  if (current_message != NULL) {
    current_message->show_all_now();
  }
}

/**
 * @brief Returns the id of the current dialog.
 * @return the id of the dialog currently shown
 */
const std::string& DialogBox::get_dialog_id() {
  return dialog_id;
}

/**
 * @brief Returns whether the dialog box has been closed, i.e.
 * whether the last message was shown and the
 * user has pressed the key, or the dialog was skiped.
 * @return true if the dialog is finished
 */
bool DialogBox::is_finished() {
  return current_message == NULL || skipped;
}

/**
 * @brief When the dialog box is finished, returns whether
 * it was skipped.
 * @return true if the dialog was skipped
 */
bool DialogBox::was_skipped() {
  return skipped;
}

/**
 * @brief Updates the dialog box.
 *
 * This function is called repeatedly by the game
 * while the dialog box exists.
 */
void DialogBox::update() {

  if (current_message == NULL) {
    return; // nothing to update
  }

  // update the text displaying
  current_message->update();

  // handle the end of the message
  if (current_message->is_finished()) {

    // update the message end arrow
    end_message_sprite->update();

    // show the appropriate action icon
    KeysEffect &keys_effect = game.get_keys_effect();
    if (!end_message_sprite->is_animation_started()) {

      const std::string next_dialog_id; // TODO
      if (next_dialog_id != "" || current_message->is_question()) {
        end_message_sprite->set_current_animation("next");
        keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_NEXT);
      }
      else {
        keys_effect.set_action_key_effect(KeysEffect::ACTION_KEY_RETURN);
        end_message_sprite->set_current_animation("last");
      }

      keys_effect.set_sword_key_effect(KeysEffect::SWORD_KEY_HIDDEN);
      Sound::play("message_end");
    }
  }
}

/**
 * @brief Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(Surface *destination_surface) {

  int x = box_dst_position.get_x();
  int y = box_dst_position.get_y();

  dialog_surface->fill_with_color(Color::get_black());

  if (style == STYLE_WITHOUT_FRAME) {
    // display a dark rectangle
    destination_surface->fill_with_color(Color::get_black(), box_dst_position);
  }
  else {
    // display the dialog box
    box_img->blit(box_src_position, dialog_surface, box_dst_position);
  }

  // display the message
  current_message->display(dialog_surface);

  // display the icon
  if (icon_number != -1) {
    Rectangle src_position(0, 0, 16, 16);
    src_position.set_xy(16 * (icon_number % 10), 16 * (icon_number / 10));
    icons_img->blit(src_position, dialog_surface, icon_dst_position);

    question_dst_position.set_x(x + 50);
  }
  else {
    question_dst_position.set_x(x + 18);
  }

  // display the question arrow
  if (current_message->is_question() && current_message->is_finished()) {
    box_img->blit(question_src_position, dialog_surface, question_dst_position);
  }

  // display the end message arrow
  if (current_message->is_finished()) {
    end_message_sprite->display(dialog_surface, x + 103, y + 56);
  }

  // final blit
  dialog_surface->blit(destination_surface);
}

