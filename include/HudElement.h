#ifndef ZSDX_HUD_ELEMENT_H
#define ZSDX_HUD_ELEMENT_H

#include "Common.h"

/**
 * Abstract class for displaying an element of the HUD.
 * The HUD calls update() repeatedly. The update() function
 * should check whether the information displayed has changed,
 * and call rebuild() if necessary.
 * The rebuild() function build the surface of this HUD element.
 */
class HudElement {

 protected:

  /**
   * The surface on which the hud element is drawn when rebuild() is called.
   * When display() is called, this surface is blitted
   * on the destination surface (usually the screen).
   */
  SDL_Surface *surface_drawn;

  /**
   * Position of this hud element on the destination surface.
   */
  SDL_Rect destination_position;
  
  HudElement(int x, int y, int width, int height);

 public:

  virtual ~HudElement(void);

  virtual void update(void) = 0;
  virtual void rebuild(void);
  virtual bool is_visible(void);
  void display(SDL_Surface *destination);
};

#endif
