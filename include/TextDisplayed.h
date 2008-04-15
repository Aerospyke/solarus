#ifndef ZSDX_TEXT_DISPLAYED_H
#define ZSDX_TEXT_DISPLAYED_H

#include "Common.h"

/**
 * Horizontal alignment of the text.
 */
enum HorizontalAlignment {
  ALIGN_LEFT,
  ALIGN_CENTER,
  ALIGN_RIGHT
};

/**
 * Vertical alignment of the text.
 */
enum VerticalAlignment {
  ALIGN_TOP,
  ALIGN_MIDDLE,
  ALIGN_BOTTOM
};

/**
 * Rendering mode of the text.
 */
enum TextRenderingMode {
  TEXT_SOLID,  /**< the text is displayed without smooth effect */
  TEXT_SHADED, /**< the text is displayed with a smooth effect for an unicolor background */
  TEXT_BLENDED /**< the text is displayed with a smooth effect for any background */
};

/**
 * The fonts available.
 */
enum FontId {
  FONT_LA       = 0,   /**< Link's Awakening font (default) */
  FONT_STANDARD = 1,   /**< a more common font, with fixed width too */
};

/**
 * This class displays some text with the ZSDX font.
 */
class TextDisplayed {

 private:

  static TTF_Font *fonts[2];

  FontId font_id;
  HorizontalAlignment horizontal_alignment;
  VerticalAlignment vertical_alignment;
  TextRenderingMode rendering_mode;
  SDL_Color text_color;
  SDL_Color background_color; // only for the TEXT_SHADED rendering

  int x;
  int y;
  SDL_Surface *text_surface;
  SDL_Rect text_position;

  const char *text;

 public:

  static void initialize(void);
  static void quit(void);

  TextDisplayed(int x, int y);
  TextDisplayed(int x, int y,
		HorizontalAlignment horizontal_alignment,
		VerticalAlignment vertical_alignment);
  ~TextDisplayed(void);

  void set_font(FontId font);
  void set_alignment(HorizontalAlignment horizontal_alignment,
		     VerticalAlignment vertical_alignment);
  void set_rendering_mode(TextRenderingMode rendering_mode);
  void set_text_color(int r, int g, int b);
  void set_background_color(int r, int g, int b);
  void set_position(int x, int y);

  void set_text(const char *text);
  void display(SDL_Surface *destination);

  const char *get_text(void);
};

#endif
