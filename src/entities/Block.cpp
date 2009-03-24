#include "entities/Block.h"
#include "entities/Hero.h"
#include "movements/FollowMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "ResourceManager.h"
#include "KeysEffect.h"
#include "Sound.h"
#include "Sprite.h"

/**
 * Creates a block.
 * @param name name identifying this block
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param subtype the subtype of block
 * @param skin the skin (an animation name for the block sprite)
 * @param maximum_moves indicates how many times the block can
 * be moved (0: none, 1: once: 2: infinite)
 */
Block::Block(string name, Layer layer, int x, int y,
	     Subtype subtype, string skin, int maximum_moves):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), maximum_moves(maximum_moves), sound_played(false),
  when_can_move(SDL_GetTicks()) {

  create_sprite("entities/block");
  set_origin(8, 13);
  if (subtype == STATUE) {
    get_sprite()->set_current_animation("statue");
  }
  else {
    get_sprite()->set_current_animation(skin);
  }

  initial_position.x = last_position.x = x;
  initial_position.y = last_position.y = y;
  initial_maximum_moves = maximum_moves;
}

/**
 * Destructor.
 */
Block::~Block() {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType Block::get_type(void) {
  return BLOCK;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Block::is_obstacle_for(MapEntity *other) {

  if (other->is_hero() && get_movement() != NULL) {
    return false;
  }

  return true;
}

/**
 * This function is called by the engine when there is a collision with another entity.
 * This is a redefinition of Detector::collision().
 * If the entity is the hero and this block can be pulled, we show the grab icon.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Block::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the action icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_GRAB);
    }
  }
}

/**
 * This function is called when the player tries to push or pull this block.
 * @param hero the hero
 * @return true if the player can be move this block
 */
bool Block::moved_by_hero(void) {

  if (get_movement() != NULL || maximum_moves == 0 || SDL_GetTicks() < when_can_move) {
    /*
    if (maximum_moves == 0)
      std::cout << "not moving because cannot move any more\n";
    else if (get_movement() != NULL) 
      std::cout << "not moving because already moving\n";
    else
      std::cout << "not moving because just moved\n";
    */      
    return false;
  }

  Hero *hero = zsdx->game->get_hero();

  if (hero->is_grabbing_or_pulling() && subtype != STATUE) {
    return false;
  }

  int dx = get_x() - hero->get_x();
  int dy = get_y() - hero->get_y();

  set_movement(new FollowMovement(map, hero, dx, dy, true));
  sound_played = false;

  return true;
}

/**
 * Updates the entity.
 */
void Block::update(void) {

  Detector::update();

  Hero *hero = zsdx->game->get_hero();

  if (movement != NULL) {
    // the block is being pushed or pulled by the hero

    // determine whether the movement is finished
    bool finished = false;

    if (((FollowMovement*) movement)->is_finished()) {
      // the block was just stopped by an obstacle: notify the hero
      hero->grabbed_entity_collision();
      finished = true;
    }
    else if (!hero->is_moving_grabbed_entity()) {
      // the hero stopped the movement, either because the 16 pixels were
      // covered or because the hero met an obstacle
      finished = true;
    }

    if (finished) {
      // the movement is finished (note that the block may have not moved)
      clear_movement();
      when_can_move = SDL_GetTicks() + 500;

      // see if the block has moved
      if (get_x() != last_position.x || get_y() != last_position.y) {

	// the block has moved
	last_position.x = get_x(); // save the new position for next time
	last_position.y = get_y();

	if (maximum_moves == 1) { // if the block could be moved only once
	  maximum_moves = 0;      // it cannot move any more
	}
      }
    }
  }
}

/**
 * Notifies the block that it has just moved.
 */
void Block::just_moved(void) {

  // now we now that the block moves at least of 1 pixel:
  // we can play the sound
  if (!sound_played) {
    ResourceManager::get_sound("hero_pushes")->play();
    sound_played = true;
  }
}

/**
 * Displays the entity again if it is a statue whose y position is greater than the hero's y position.
 */
void Block::display_on_map_above_hero(void) {

  Hero *hero = zsdx->game->get_hero();
  if (subtype == STATUE && get_y() > hero->get_y() + 8) {
    MapEntity::display_on_map();
  }
}

/**
 * Resets the block at its initial position.
 */
void Block::reset(void) {
  set_position_in_map(initial_position.x, initial_position.y);
  this->maximum_moves = initial_maximum_moves;
}
