#include "SDL/SDL_config_lib.h"
#include "Message.h"
#include "DialogBox.h"
#include "FileTools.h"
#include "TextDisplayed.h"

/**
 * Delay between two chars, depending on the dialog speed.
 */
static const Uint32 char_delays[3] = {
  100, // slow
  60,  // medium
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

  // initialize the state
  this->line_index = 0;
  this->char_index = 0;
  this->next_char_date = SDL_GetTicks();
  set_char_delay(dialog_box->get_speed());
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

  for (int i = 0; i < 3; i++) {
    text_surfaces[i] = new TextDisplayed(69, 158 + i * 13,
					 ALIGN_LEFT, ALIGN_TOP);
  }

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
  first_answer = true;

  // next message
  next_message_id = CFG_ReadText("next", "");
  next_message_id_2 = CFG_ReadText("next2", "");

  // cancel mode
  string cancel_mode_text = CFG_ReadText("cancel", "no");
  DialogCancelMode cancel_mode;
  if (cancel_mode_text == "current") {
    cancel_mode = DIALOG_CANCEL_NONE;
  }
  else if (cancel_mode_text == "all") {
    cancel_mode = DIALOG_CANCEL_CURRENT;
  }
  else {
    cancel_mode = DIALOG_CANCEL_NONE;
  }
  dialog_box->set_cancel_mode(cancel_mode);

  // close the file
  CFG_CloseFile(&ini);
}

/**
 * Returns the id of the next message to display, or
 * an empty string if this is the last message.
 * @return the id of the message to display when this one
 * is over
 */
MessageId Message::get_next_message_id(void) {

  if (question && !first_answer) {
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
 * Sets the delay between two chars, depending on the
 * speed specified.
 * @param speed the speed
 */
void Message::set_char_delay(DialogSpeed speed) {

  delay = char_delays[speed];
  next_char_date = SDL_GetTicks() + delay;
}

/**
 * Adds the next character to the message.
 * If this is a special character (like $0, $v, etc.),
 * the corresponding action is performed.
 */
void Message::add_character(void) {

  unsigned char current_char = lines[line_index][char_index];

  /*
   * TODO special characters:
   * - $1, $2 and $3: slow, medium and fast
   * - $0: pause
   * - $v: variable
   * - space: don't add the delay
   * - 110xxxx: multibyte character
   */
  
  text_surfaces[line_index]->add_char(current_char);
  char_index++;
  
  // if this is a multibyte character, add the next byte
  if ((current_char & 0xE0) == 0xC0) {
    // the first byte is 110xxxxx: this means the character is stored with two bytes (utf-8)
    
    current_char = lines[line_index][char_index];
    text_surfaces[line_index]->add_char(current_char);
    char_index++;	
  }
  
  next_char_date += delay;
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
