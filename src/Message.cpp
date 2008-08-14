#include "SDL/SDL_config_lib.h"
#include "Message.h"
#include "DialogBox.h"
#include "FileTools.h"
#include "TextDisplayed.h"

/**
 * Delay between two chars, depending on the dialog speed.
 */
static const Uint32 char_delays[3] = {
  200, // slow
  80,  // medium
  30   // fast (default)
};

/**
 * Creates a new message.
 * @param dialog_box the dialog box
 * @param message_id id of the message
 */
Message::Message(DialogBox *dialog_box, MessageId message_id) {

  this->dialog_box = dialog_box;

  // parse the message
  parse(message_id);

  // create the text surfaces
  int x = (icon == NULL) ? 67 : 99;
  for (int i = 0; i < 3; i++) {
    text_surfaces[i] = new TextDisplayed(x, 158 + i * 13,
					 ALIGN_LEFT, ALIGN_TOP);
  }

  if (question) {
    x += 24;
    text_surfaces[1]->set_x(x);
    text_surfaces[2]->set_x(x);
  }

  // initialize the state
  this->line_index = 0;
  this->char_index = 0;
  this->next_char_date = SDL_GetTicks();
  this->show_all = false;
  update_char_delay();
}

/**
 * Destructor.
 */
Message::~Message(void) {

  if (icon != NULL) {
    SDL_FreeSurface(icon);
  }

  for (int i = 0; i < 3; i++) {
    delete text_surfaces[i];
  }
}

/**
 * Reads the message from the data file and initializes
 * the fields accordingly.
 * @param message_id id of the message
 */
void Message::parse(MessageId message_id) {

  // open the file
  string file_name;
  if (message_id[0] == '_') {
    // message from the engine
    file_name = "text/engine.msg";
  }
  else {
    file_name = "text/game.msg";
  }
  file_name = FileTools::data_file_add_prefix(file_name);

  // parse the message
  CFG_File ini;

  if (CFG_OpenFile(file_name.c_str(), &ini) != CFG_OK) {
    DIE("Cannot load the message file '" << file_name << "': " << CFG_GetError());
  }

  if (CFG_SelectGroup(message_id.c_str(), 0) != CFG_OK) {
    DIE("Cannot find message '" << message_id << "'");
  }

  // text
  lines[0] = CFG_ReadText("line1", "");
  lines[1] = CFG_ReadText("line2", "");
  lines[2] = CFG_ReadText("line3", "");

  // icon
  int icon_number = CFG_ReadInt("icon", 0);
  if (icon_number != 0) {
    icon = NULL; // TODO
  }
  else {
    icon = NULL;
  }

  // question
  question = CFG_ReadBool("question", false);

  // next message
  next_message_id = CFG_ReadText("next", "");
  next_message_id_2 = CFG_ReadText("next2", "");

  // cancel mode
  string cancel_mode_text = CFG_ReadText("cancel", "");

  if (cancel_mode_text != "") { // a cancel mode is specified
    DialogCancelMode cancel_mode;
    if (cancel_mode_text == "current") {
      cancel_mode = DIALOG_CANCEL_CURRENT;
    }
    else if (cancel_mode_text == "all") {
      cancel_mode = DIALOG_CANCEL_ALL;
    }
    else {
      cancel_mode = DIALOG_CANCEL_NONE;
    }
    dialog_box->set_cancel_mode(cancel_mode);
  }

  // close the file
  CFG_CloseFile(&ini);
}

/**
 * Returns whether this message is a question.
 * @return true if the message is a question
 */
bool Message::is_question(void) {
  return question;
}

/**
 * Returns the id of the next message to display, or
 * an empty string if this is the last message.
 * @return the id of the message to display when this one
 * is over
 */
MessageId Message::get_next_message_id(void) {

  if (question && !dialog_box->is_first_answer()) {
    return next_message_id_2;
  }

  return next_message_id;
}

/**
 * Returns whether the message is now completely displayed.
 * @return true if the message is over
 */
bool Message::is_over(void) {
  return line_index == 3;
}

/**
 * Shows all characters of the message now.
 */
void Message::show_all_now(void) {
  show_all = true;
  update_char_delay();
}

/**
 * Sets the delay between two chars, depending on the
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
  next_char_date = SDL_GetTicks() + delay;
}

/**
 * Adds the next character to the message.
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

  if (current_char == '$') {
    // special character

    current_char = lines[line_index][char_index++];

    switch (current_char) {

    case '0':
      // pause
      next_char_date += 1000;
      break;

    case '1':
      // slow
      dialog_box->set_speed(DIALOG_SPEED_SLOW);
      update_char_delay();
      break;

    case '2':
      // medium
      dialog_box->set_speed(DIALOG_SPEED_MEDIUM);
      update_char_delay();
      break;

    case '3':
      // fast
      dialog_box->set_speed(DIALOG_SPEED_FAST);
      update_char_delay();
      break;

    case 'v':
      set_variable(dialog_box->get_variable());
      break;

    default:
      // not a special char, actually
      text_surfaces[line_index]->add_char('$');
      text_surfaces[line_index]->add_char(current_char);
    }
  }
  else {
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
  }
}

/**
 * Replaces the first occurence of "$v" by the specified value.
 * @param value the value to set
 */
void Message::set_variable(string value) {
  char_index -= 2;
  lines[line_index] = lines[line_index].replace(char_index, 2, value);
}

/**
 * Updates the message.
 */
void Message::update(void) {

  Uint32 now = SDL_GetTicks();
  while (!is_over() && now >= next_char_date) {
    
    // check the end of the current line
    while (!is_over() && char_index >= lines[line_index].length()) {
      char_index = 0;
      line_index++;
    }

    if (!is_over()) {
      // add a character
      add_character();
    }
  }
}

/**
 * Displays the message on a surface.
 */
void Message::display(SDL_Surface *destination_surface) {
  
  for (int i = 0; i < 3; i++) {
    text_surfaces[i]->display(destination_surface);
  }
}
