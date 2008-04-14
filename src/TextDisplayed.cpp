/**
 * This module defines the class TextDisplayed.
 */
#include "TextDisplayed.h"
#include "FileTools.h"

/**
 * The two fonts, created in the initialize() function.
 */
TTF_Font *TextDisplayed::fonts[2] = {NULL, NULL};

/**
 * Initializes the font system.
 */
void TextDisplayed::initialize(void) {

  TTF_Init();

  fonts[FONT_LA] = TTF_OpenFont(FileTools::data_file_add_prefix("zsdx.ttf"), 11);
  if (fonts[FONT_LA] == NULL) {
    cerr << "Cannot load font 'zsdx.ttf'." << endl;
    exit(1);
  }

  fonts[FONT_STANDARD] = TTF_OpenFont(FileTools::data_file_add_prefix("fixed8.fon"), 11);
  if (fonts[FONT_STANDARD] == NULL) {
    cerr << "Cannot load font 'fixed8.fon'." << endl;
    exit(1);
  }
}

/**
 * Closes the font system.
 */
void TextDisplayed::quit(void) {

  TTF_CloseFont(fonts[FONT_LA]);
  TTF_CloseFont(fonts[FONT_STANDARD]);
  TTF_Quit();
}

/**
 * Creates a text to display with the default properties.
 */
TextDisplayed::TextDisplayed(void):
  font_id(FONT_LA),
  horizontal_alignment(ALIGN_LEFT),
  vertical_alignment(ALIGN_MIDDLE),
  rendering_mode(TEXT_SOLID),
  text_surface(NULL) {

  text_color.r = 255;
  text_color.g = 255;
  text_color.b = 255;

  background_color.r = 0;
  background_color.g = 0;
  background_color.b = 0;
}

/**
 * Creates a text to display with the specified properties.
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
TextDisplayed::TextDisplayed(HorizontalAlignment horizontal_alignment,
			     VerticalAlignment vertical_alignment):

  font_id(FONT_LA),
  horizontal_alignment(horizontal_alignment),
  vertical_alignment(vertical_alignment),
  rendering_mode(TEXT_SOLID),
  text_surface(NULL) {

  text_color.r = 255;
  text_color.g = 255;
  text_color.b = 255;

  background_color.r = 0;
  background_color.g = 0;
  background_color.b = 0;  
}

/**
 * Destructor.
 */
TextDisplayed::~TextDisplayed(void) {
  SDL_FreeSurface(text_surface);
}

/**
 * Sets the font to use.
 * @param font_id a font: FONT_LA or FONT_STANDARD
 */
void TextDisplayed::set_font(FontId font_id) {
  this->font_id = font_id;
}

/**
 * Sets the text alignment.
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
void TextDisplayed::set_alignment(HorizontalAlignment horizontal_alignment,
				  VerticalAlignment vertical_alignment) {
  this->horizontal_alignment = horizontal_alignment;
  this->vertical_alignment = vertical_alignment;
}

/**
 * Sets the rendering mode of the text.
 * @param rendering_mode rendering mode: TEXT_SOLID, TEXT_SHADED or TEXT_BLENDED
 */
void TextDisplayed::set_rendering_mode(TextRenderingMode rendering_mode) {
  this->rendering_mode = rendering_mode;
}

/**
 * Sets the color of the text.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayed::set_text_color(int r, int g, int b) {
  this->text_color.r = r;
  this->text_color.g = g;
  this->text_color.b = b;
}

/**
 * Sets the background color of the text.
 * This is only useful for the TEXT_SHADED rendering.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayed::set_background_color(int r, int g, int b) {
  this->background_color.r = r;
  this->background_color.g = g;
  this->background_color.b = b;  
}

/**
 * Create the text surface with the current color and the current alignment.
 * The coordinate system depends on the current alignment.
 * @param text the text to display
 * @param x x coordinate of the text
 * @param y y coordinate of the text
 */
void TextDisplayed::create_text(const char *text,
				int x, int y) {

  if (text_surface != NULL) {
    SDL_FreeSurface(text_surface);
    text_surface = NULL;
  }
  
  if (text[0] == '\0') {
    return;
  }
  
  // create the text surface

  switch (rendering_mode) {

  case TEXT_SOLID:
    text_surface = TTF_RenderText_Solid(fonts[font_id], text, text_color);
    break;

  case TEXT_SHADED:
    text_surface = TTF_RenderText_Shaded(fonts[font_id], text, text_color, background_color);
    break;

  case TEXT_BLENDED:
    text_surface = TTF_RenderText_Blended(fonts[font_id], text, text_color);
    break;
  }

  // calculate the coordinates of the top-left corner
  int x_left = 0, y_top = 0;

  switch (horizontal_alignment) {

  case ALIGN_LEFT:
    x_left = x;
    break;

  case ALIGN_CENTER:
    x_left = x - text_surface->w / 2;
    break;

  case ALIGN_RIGHT:
    x_left = x - text_surface->w;
    break;
  }

  switch (vertical_alignment) {

  case ALIGN_TOP:
    y_top = y;
    break;

  case ALIGN_MIDDLE:
    y_top = y - text_surface->h / 2;
    break;

  case ALIGN_BOTTOM:
    y_top = y - text_surface->h;
    break;
  }

  text_position.x = x_left;
  text_position.y = y_top;
}

/**
 * Displays the text on a surface.
 * @param destination the destination surface
 */
void TextDisplayed::display(SDL_Surface *destination) {

  if (text_surface != NULL) {
    SDL_BlitSurface(text_surface, NULL, destination, &text_position);
  }
}
