#ifndef ZSDX_PAUSE_SUBMENU_H
#define ZSDX_PAUSE_SUBMENU_H

#include "Common.h"

/**
 * Abstract class for a submenu of the pause menu.
 */
class PauseSubmenu {

 protected:

  PauseMenu *pause_menu;
  Game *game;

  TextSurface *caption_text_1;
  TextSurface *caption_text_2;

  PauseSubmenu(PauseMenu *pause_menu, Game *game);

  void set_caption_text(string text);

 public:

  virtual ~PauseSubmenu(void);

  virtual void key_pressed(const SDL_keysym &keysym) = 0;
  virtual void update(void) = 0;
  virtual void display(SDL_Surface *destination);
};

#endif
