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
#include "Message.h"
#include "DialogBox.h"
#include "Game.h"
#include "ResourceManager.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/IniFile.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Delay between two chars, depending on the dialog speed.
 */
static const uint32_t char_delays[3] = {
  60, // slow
  45,  // medium
  30   // fast (default)
};

/**
 * @brief Creates a new message.
 * @param dialog_box the dialog box
 * @param message_id id of the message
 * @param x x position of the dialog box
 * @param y y position of the dialog box
 */
Message::Message(DialogBox *dialog_box, MessageId message_id, int x, int y):
  dialog_box(dialog_box), x(x), y(y) {

  // parse the message
  parse(message_id);

  // create the text surfaces
  int text_x = x + ((dialog_box->get_icon_number() == -1) ? 16 : 48);
  int text_y = y - 1;
  for (int i = 0; i < 3; i++) {
    text_y += 13;
    text_surfaces[i] = new TextSurface(text_x, text_y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
  }

  if (question) {
    text_x += 24;
    text_surfaces[1]->set_x(text_x);
    text_surfaces[2]->set_x(text_x);
  }

  // initialize the state
  this->line_index = 0;
  this->char_index = 0;
  this->next_char_date = this->next_sound_date = System::now();
  this->show_all = false;
  update_char_delay();
}

/**
 * @brief Destructor.
 */
Message::~Message(void) {

  for (int i = 0; i < 3; i++) {
    delete text_surfaces[i];
  }
}

/**
 * @brief Reads the message from the data file and initializes
 * the fields accordingly.
 * @param message_id id of the message
 */
void Message::parse(MessageId message_id) {

  // open the file
  std::string file_name;
  if (message_id[0] == '_') {
    // message from the engine
    file_name = "text/engine_dialogs.dat";
  }
  else {
    file_name = "text/game_dialogs.dat";
  }

  // parse the message
  IniFile ini_file(file_name, IniFile::READ_LANGUAGE);

  Debug::assert(ini_file.has_group(message_id), StringConcat() << "The message '" << message_id << "' does not exist");
  ini_file.set_group(message_id);

  // text
  lines[0] = ini_file.get_string_value("line1", "");
  lines[1] = ini_file.get_string_value("line2", "");
  lines[2] = ini_file.get_string_value("line3", "");
  for (int i = 0; i < 3; i++) {
    int size = lines[i].size();
    if (lines[i][0] == '"') {
      lines[i] = lines[i].substr(1);
      size--;
    }
    if (lines[i][size - 1] == '"') {
      lines[i] = lines[i].substr(0, size - 1);
    }
  }

  // icon
  int icon_number = ini_file.get_integer_value("icon", -2);
  if (icon_number != -2) {
    // if an icon number is specified (even -1)
    dialog_box->set_icon_number(icon_number);
  }

  // next message
  next_message_id = ini_file.get_string_value("next", "");
  next_message_id_2 = ini_file.get_string_value("next2", "");

  // question
  question = ini_file.get_boolean_value("question", false);

  // skip mode
  const std::string &skip_mode_text = ini_file.get_string_value("skip", "");

  if (skip_mode_text != "") { // a skip mode is specified
    DialogBox::SkipMode skip_mode;
    if (skip_mode_text == "current") {
      skip_mode = DialogBox::SKIP_CURRENT;
    }
    else if (skip_mode_text == "all") {
      skip_mode = question ? DialogBox::SKIP_CURRENT : DialogBox::SKIP_ALL;
    }
    else {
      skip_mode = DialogBox::SKIP_NONE;
    }
    dialog_box->set_skip_mode(skip_mode);
  }
}

/**
 * @brief Returns whether this message is a question.
 * @return true if the message is a question
 */
bool Message::is_question(void) {
  return question;
}

/**
 * @brief Returns the id of the next message to display.
 *
 * If this is the last message, an empty string is returned.
 *
 * @return the id of the message to display when this one is over
 */
MessageId Message::get_next_message_id(void) {

  if (question && dialog_box->get_last_answer() == 1) {
    return next_message_id_2;
  }

  return next_message_id;
}

/**
 * @brief Returns whether the message is now completely displayed.
 * @return true if the message is over
 */
bool Message::is_finished(void) {
  return line_index == 3;
}

/**
 * @brief Shows all characters of the message now.
 */
void Message::show_all_now(void) {
  show_all = true;
  update_char_delay();
}

/**
 * @brief Sets the delay between two chars, depending on the
 * speed specified by the dialog box.
 * @param speed the speed
 */
void Message::update_char_delay(void) {

  if (!show_all) {
    delay = char_delays[dialog_box->get_speed()];
  }
  else {
    delay = 0;
  }
  next_char_date = System::now() + delay;
}

/**
 * @brief Adds the next character to the message.
 *
 * If this is a special character (like $0, $v, etc.),
 * the corresponding action is performed.
 */
void Message::add_character(void) {

  unsigned char current_char = lines[line_index][char_index++];

  /*
   * Special characters:
   * - $1, $2 and $3: slow, medium and fast
   * - $0: pause
   * - $v: variable
   * - space: don't add the delay
   * - 110xxxx: multibyte character
   */

  bool special = false;
  if (current_char == '$') {
    // special character

    special = true;
    current_char = lines[line_index][char_index++];

    switch (current_char) {

    case '0':
      // pause
      next_char_date += 1000;
      break;

    case '1':
      // slow
      dialog_box->set_speed(DialogBox::SPEED_SLOW);
      update_char_delay();
      break;

    case '2':
      // medium
      dialog_box->set_speed(DialogBox::SPEED_MEDIUM);
      update_char_delay();
      break;

    case '3':
      // fast
      dialog_box->set_speed(DialogBox::SPEED_FAST);
      update_char_delay();
      break;

    case 'v':
      set_variable(dialog_box->get_variable());
      break;

    default:
      // not a special char, actually
      text_surfaces[line_index]->add_char('$');
      special = false;
    }
  }

  if (!special) {
    // normal character
    text_surfaces[line_index]->add_char(current_char);

    // if this is a multibyte character, also add the next byte
    if ((current_char & 0xE0) == 0xC0) {
      // the first byte is 110xxxxx: this means the character is stored with two bytes (utf-8)

      current_char = lines[line_index][char_index++];
      text_surfaces[line_index]->add_char(current_char);
    }

    if (current_char != ' ') {
      next_char_date += delay;
    }

    uint32_t now = System::now();
    if (now >= next_sound_date && dialog_box->is_letter_sound_enabled()) {
      ResourceManager::get_sound("message_letter")->play();
      next_sound_date = now + 100;
    }
  }
}

/**
 * @brief Replaces the first occurence of "$v" by the specified value.
 * @param value the value to set
 */
void Message::set_variable(const std::string &value) {
  char_index -= 2;
  lines[line_index] = lines[line_index].replace(char_index, 2, value);
}

/**
 * @brief Updates the message.
 */
void Message::update(void) {

  uint32_t now = System::now();
  while (!is_finished() && now >= next_char_date) {

    // check the end of the current line
    while (!is_finished() && char_index >= lines[line_index].length()) {
      char_index = 0;
      line_index++;
    }

    if (!is_finished()) {
      // add a character
      add_character();
    }
  }
}

/**
 * @brief Displays the message on a surface.
 * @param destination_surface the surface to draw
 */
void Message::display(Surface *destination_surface) {

  for (int i = 0; i < 3; i++) {
    text_surfaces[i]->display(destination_surface);
  }
}

