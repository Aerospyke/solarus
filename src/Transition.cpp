#include "Transition.h"
#include "TransitionImmediate.h"
#include "TransitionFade.h"
#include "TransitionScrolling.h"

/**
 * Constructor.
 */
Transition::Transition(Transition::Direction direction):
direction(direction), previous_surface(NULL) {

}

/**
 * Destructor.
 */
Transition::~Transition(void) {

}

/**
 * Creates a transition effect with the specified type and direction.
 * @param style style of the transition: Transition::IMMEDIATE, Transition::FADE, etc.
 * @param direction Transition::IN or Transition::OUT
 */
Transition * Transition::create(Transition::Style style, Transition::Direction direction) {

  Transition *transition = NULL;

  switch (style) {

  case Transition::IMMEDIATE:
    transition = new TransitionImmediate(direction);
    break;

  case Transition::FADE:
    transition = new TransitionFade(direction);
    break;

  case Transition::SCROLLING:
    transition = new TransitionScrolling(direction);
    break;
  }

  return transition;
}

/**
 * Returns the direction of this transition effect.
 * @returns the direction of this transition effect: Transition::IN or Transition::OUT
 */
Transition::Direction Transition::get_direction(void) {
  return direction;
}

/**
 * Indicates the surface that was shown during the OUT transition
 * that was played before this IN transition.
 * @param previous_surface the previous surface
 */
void Transition::set_previous_surface(SDL_Surface *previous_surface) {

  if (get_direction() == OUT) {
    DIE("Cannot show a previous surface with an OUT transition effect");
  }

  this->previous_surface = previous_surface;
}

/**
 * Returns whether this transition effect needs the previous surface.
 * @return false
 */
bool Transition::needs_previous_surface(void) {
  return false;
}
