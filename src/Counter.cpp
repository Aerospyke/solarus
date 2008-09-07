#include "Counter.h"
#include "ResourceManager.h"
#include "Color.h"
#include "Counter.h"

/**
 * Constructor.
 * The counter's value is initialized to zero.
 * @param nb_digits number of digits to display
 * @param fill_with_zeros true to fill with zeros when the current number of digits is lower than nb_digits
 * @param x x coordinate of the top-left corner of the counter on the destination surface
 * @param y y coordinate of the top-left corner of the counter on the destination surface
 */
Counter::Counter(unsigned int nb_digits, bool fill_with_zeros,
			 int x, int y):
  nb_digits(nb_digits), fill_with_zeros(fill_with_zeros) {

  surface_drawn = SDL_CreateRGBSurface(SDL_HWSURFACE, 8 * nb_digits, 8, 32, 0, 0, 0, 0);
  SDL_SetColorKey(surface_drawn, SDL_SRCCOLORKEY, Color::black);
  img_digits = ResourceManager::load_image("hud/digits.png");

  destination_position.x = x;
  destination_position.y = y;

  rebuild_with_value(0);
}

/**
 * Destructor.
 */
Counter::~Counter(void) {
  SDL_FreeSurface(surface_drawn);
  SDL_FreeSurface(img_digits);
}

/**
 * Returns the value currently displayed by the counter.
 * @return the value currently displayed by the counter
 */
unsigned int Counter::get_value(void) {
  return value;
}

/**
 * Changes the counter's value.
 * The surface is redrawn such that the counter displays
 * the new value.
 * Nothing is done if the value is the same.
 * @param value the new value to display
 */
void Counter::set_value(unsigned int value) {

  if (value != this->value) {
    rebuild_with_value(value);
  }
}

/**
 * Redraws the surface to represent a new value.
 * @param value the new value to display
 */
void Counter::rebuild_with_value(unsigned int value) {

  this->value = value;

  // fill with transparent color
  SDL_FillRect(surface_drawn, NULL, Color::black);

  SDL_Rect digit_position_in_src = {0, 0, 8, 8};
  SDL_Rect digit_position_in_counter = {0, 0, 8, 8};

  bool right_digit = true;

  // draw the digit from the right to the left
  for (int i = nb_digits - 1; i >= 0; i--) {

    // compute each digit
    Uint8 digit = value % 10;
    value /= 10;

    if (value != 0 || digit != 0 || fill_with_zeros || right_digit) {

      // draw the surface
      digit_position_in_src.x = digit * 8;
      digit_position_in_counter.x = i * 8;

      SDL_BlitSurface(img_digits, &digit_position_in_src,
		      surface_drawn, &digit_position_in_counter);

      right_digit = false;
    }
  }
}

/**
 * Adds 1 to the counter.
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void Counter::increase(void) {
  set_value(value + 1);
}

/**
 * Substracts 1 from the counter.
 * The surface is redrawn such that the counter displays
 * the new value.
 */
void Counter::decrease(void) {
  set_value(value - 1);
}

/**
 * Displays the counter on a surface, at the position specified
 * when you called the constructor.
 * @param destination the destination surface
 */
void Counter::display(SDL_Surface *destination) {
  SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position);
}

/**
 * Displays the counter on a surface, specifying the position.
 * @param destination the destination surface
 * @param x x coordinate of the top-left corner of the counter on the destination surface
 * @param y y coordinate of the top-left corner of the counter on the destination surface
 */
void Counter::display(SDL_Surface *destination, int x, int y) {
  destination_position.x = x;
  destination_position.y = y;
  SDL_BlitSurface(surface_drawn, NULL, destination, &destination_position);
}

