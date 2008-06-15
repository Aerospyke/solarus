#include "Sprite.h"
#include "SpriteAnimations.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "AnimationListener.h"
#include "ZSDX.h"
#include "GameResource.h"

/**
 * Creates a sprite.
 * @param animations the sprite's animations
 */
Sprite::Sprite(SpriteAnimations *animations):
animations(animations), current_direction(0),
suspended(false), over(false), listener(NULL) {
  
  set_current_animation(animations->get_default_animation());
}

/**
 * Creates a sprite.
 * This is equivalent to Sprite(zsdx->game_resource->get_sprite_animations(id)).
 * @param id id of the sprite's animations
 */
Sprite::Sprite(SpriteAnimationsId id):
current_direction(0), suspended(false), over(false), listener(NULL) {
  
  animations = zsdx->game_resource->get_sprite_animations(id);
  set_current_animation(animations->get_default_animation());
}

/**
 * Destructor.
 */
Sprite::~Sprite(void) {

}

/**
 * Returns the size of a frame for the current animation and the current direction.
 * @return the size of a frame
 */
SDL_Rect& Sprite::get_size(void) {

  SpriteAnimation *animation = animations->get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_size();
}

/**
 * Returns the origin point of a frame for the current animation and the current direction.
 * @return the origin point of a frame
 */
SDL_Rect& Sprite::get_origin(void) {

  SpriteAnimation *animation = animations->get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_origin();
}

/**
 * Returns the frame interval of the current animation.
 * A value of 0 (only for 1-frame animations) means that the
 * animation must continue to be displayed: in this case,
 * is_over() returns always false.
 * @return the delay between two frames for the current animation (in miliseconds)
 */
Uint32 Sprite::get_frame_interval(void) {
  return current_animation->get_frame_interval();  
}

/**
 * Returns the next frame of the current frame.
 * @return the next frame of the current frame (or -1 if the animation is over)
 */
int Sprite::get_next_frame(void) {
  return current_animation->get_next_frame(current_direction, current_frame);    
}

/**
 * Returns the current animation of the sprite.
 * @return the name of the current animation of the sprite
 */
string Sprite::get_current_animation(void) {
  return current_animation_name;
}

/**
 * Sets the current animation of the sprite.
 * If the sprite is already playing another animation, this animation is interrupted.
 * If the sprite is already playing the same animation, nothing is done.
 * @param animation_name name of the new animation of the sprite
 */
void Sprite::set_current_animation(string animation_name) {

  if (animation_name != this->current_animation_name || !is_animation_started()) {
  
    SpriteAnimation *animation = animations->get_animation(animation_name);
    
    if (animation == NULL) {
      DIE("Unknown animation '" << animation_name << "'");
    }
    
    this->current_animation_name = animation_name;
    this->current_animation = animation;
    set_current_frame(0);
  }
}

/**
 * Returns the current direction of the sprite's animation.
 * @return the current direction
 */
int Sprite::get_current_direction(void) {
  return current_direction;
}

/**
 * Sets the current direction of the sprite's animation and restarts the animation.
 * If the specified direction is the current direction, nothing is done.
 * @param current_direction the current direction
 */
void Sprite::set_current_direction(int current_direction) {

  if (current_direction != this->current_direction) {
    this->current_direction = current_direction;
    set_current_frame(0);
  }
}

/**
 * Returns the current frame of the sprite's animation.
 * @return the current frame
 */
int Sprite::get_current_frame(void) {
  return current_frame;
}

/**
 * Sets the current frame of the sprite's animation.
 * If the animation was over, it is restarted.
 * If the animation is suspended, it remains suspended
 * but the specified frame is displayed. 
 * @param current_frame the current frame
 */
void Sprite::set_current_frame(int current_frame) {
  
  this->current_frame = current_frame;
  over = false;
  next_frame_date = SDL_GetTicks() + get_frame_interval();
}

/**
 * Returns true if the animation is started.
 * It can be suspended.
 * @return true if the animation is started, false otherwise
 */
bool Sprite::is_animation_started(void) {
  return !is_over();
}

/**
 * Starts the animation.
 */
void Sprite::start_animation(void) {
  restart_animation();
}

/**
 * Restarts the animation.
 */
void Sprite::restart_animation(void) {
  set_current_frame(0);
  set_suspended(false);
}

/**
 * Stops the animation.
 */
void Sprite::stop_animation(void) {
  over = true;
}

/**
 * Returns true if the animation is suspended.
 * @return true if the animation is suspended, false otherwise
 */
bool Sprite::is_suspended(void) {
  return suspended;
}

/**
 * Suspends or resumes the animation.
 * Nothing is done if the parameter specified does not change.
 * @param suspended true to suspend the animation, false to resume it
 */
void Sprite::set_suspended(bool suspended) {

  if (suspended != this->suspended) {
    this->suspended = suspended;
    
    // compte next_frame_date if the animation is being resumed
    if (!suspended) {
      next_frame_date = SDL_GetTicks() + get_frame_interval();
    }
  }
}

/**
 * Returns true if the animation is finished.
 * The animation is over after the last frame is reached
 * and if the frame interval is not zero (a frame interval
 * of zero should be used only for 1-frame animations).
 * @return true if the animation is finished
 */
bool Sprite::is_over(void) {
  return over;
}

/**
 * Associates an animation listener to this sprite.
 * The animation listener will be notified when the animation is over.
 * @param listener the listener to associate,
 * or NULL to just remove the previous listener
 */
void Sprite::set_animation_listener(AnimationListener *listener) {
  this->listener = listener;
}

/**
 * Checks whether the frame has to be changed.
 * If the frame changes, next_frame_date is updated.
 */
void Sprite::update_current_frame(void) {

  int next_frame;

  Uint32 now = SDL_GetTicks();

  while (!over && !suspended && get_frame_interval() > 0
	 && now >= next_frame_date) {

    // we get the next frame
    next_frame = get_next_frame();

    // test whether the animation is over
    if (next_frame == -1) {
  
      over = true;
      
      // tell the listener the animation is over
      if (listener != NULL) {
	listener->animation_over(this);
      }
    }
    else {
      current_frame = next_frame;
      next_frame_date += get_frame_interval();
    }
  }
}

/**
 * Displays the sprite on a surface, with its current animation, direction and frame.
 * @param destination the surface on which the sprite will be displayed
 * @param x x coordinate of the sprite on this surface
 * (the origin will be placed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin will be placed at this position)
 */
void Sprite::display(SDL_Surface *destination, int x, int y) {

  if (!is_over()) {
    current_animation->display(destination, x, y, current_direction, current_frame);
  }
}
