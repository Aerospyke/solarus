#include "TransitionImmediate.h"

/**
 * Constructor.
 */
TransitionImmediate::TransitionImmediate(Transition::Direction direction):
  Transition(direction) {

}

/**
 * Destructor.
 */
TransitionImmediate::~TransitionImmediate(void) {

}

/**
 * Starts this transition effect.
 */
void TransitionImmediate::start(void) {
  
}

/**
 * Returns whether the transition effect is started.
 * @return false
 */
bool TransitionImmediate::is_started(void) {
  return false;
}

/**
 * Returns whether the transition effect is over.
 * @return true
 */
bool TransitionImmediate::is_over(void) {
  return true;
}

/**
 * Displays the transition effect on a surface.
 */
void TransitionImmediate::display(SDL_Surface *surface) {

}
