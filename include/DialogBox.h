#ifndef ZSDX_DIALOG_BOX_H
#define ZSDX_DIALOG_BOX_H

#include "Common.h"
#include "KeysEffect.h"
#include <map>
#include <vector>

/**
 * The possible speeds of a dialog's text.
 */
enum DialogSpeed {
  DIALOG_SPEED_SLOW,
  DIALOG_SPEED_MEDIUM,
  DIALOG_SPEED_FAST
};

/**
 * Indicates what happens when the user tries to cancel
 * the current message.
 */
enum DialogCancelMode {
  DIALOG_CANCEL_NONE,
  DIALOG_CANCEL_CURRENT,
  DIALOG_CANCEL_ALL
};

/**
 * A dialog box where a succession of messages can be displayed.
 * This class displays the dialog box and handles its properties:
 * the text speed and the state of the action and sword keys.
 */
class DialogBox {

 private:
  
  // current message
  Message *current_message;
  MessageId current_message_id;
  std::map<MessageId, string> variables;
  static bool answer_1_selected;           // the answer selected

  // dialog properties
  DialogSpeed speed;
  DialogCancelMode cancel_mode;
  bool cancel_dialog;
  int icon_number;             // index of the 16*16 icon displayed, or -1 if there is no icon

  // graphics
  SDL_Surface *img_box;
  SDL_Surface *img_icons;
  Sprite *sprite_message_end_arrow;

  // sounds
  Sound *end_message_sound;
  Sound *switch_answer_sound;

  void show_message(MessageId messageId);

 public:

  // creation and destruction
  DialogBox(MessageId first_message_id);
  ~DialogBox(void);

  // dialog properties
  DialogSpeed get_speed(void);
  void set_speed(DialogSpeed speed);
  DialogCancelMode get_cancel_mode(void);
  void set_cancel_mode(DialogCancelMode cancel_mode);
  int get_icon_number(void);
  void set_icon_number(int icon_number);

  void set_variable(MessageId messageId, string value);
  void set_variable(MessageId messageId, int value);
  string get_variable(void);
  
  // current message
  void action_key_pressed(void);
  void sword_key_pressed(void);
  void up_or_down_key_pressed(void);
  bool is_over(void);
  static bool was_answer_1_selected(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
