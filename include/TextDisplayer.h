#ifndef ZSDX_TEXT_DISPLAYER_H
#define ZSDX_TEXT_DISPLAYER_H

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
 * This class displays some text with the ZSDX font.
 */
class TextDisplayer {

 private:

  TTF_Font *font;
  HorizontalAlignment horizontal_alignment;
  VerticalAlignment vertical_alignment;
  TextRenderingMode rendering_mode;
  SDL_Color text_color;
  SDL_Color background_color; // only for the TEXT_SHADED rendering

 public:

  TextDisplayer(void);
  ~TextDisplayer(void);

  void set_alignment(HorizontalAlignment horizontal_alignment,
		     VerticalAlignment vertical_alignment);

  void set_rendering_mode(TextRenderingMode rendering_mode);

  void set_text_color(int r, int g, int b);
  void set_background_color(int r, int g, int b);

  void show_text(const char *text,
		 SDL_Surface *destination,
		 int x, int y);
};

#endif
