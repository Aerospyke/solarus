#include "entities/Boomerang.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/CollisionMovement.h"
#include "movements/PlayerMovement.h"
#include "movements/TargetMovement.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Map.h"

/**
 * Creates a boomerang.
 * @param hero the hero
 */
Boomerang::Boomerang(Hero *hero):
  MapEntity(), hero(hero), has_to_go_back(false), going_back(false) {

  // initialize the entity
  set_layer(hero->get_layer());
  create_sprite("entities/boomerang");
  set_origin(8, 8);
  set_rectangle_from_sprite();

  // determine the boomerang direction
  int boomerang_direction = hero->get_animation_direction() * 90;
  // TODO take into account diagonal directions (but we cannot use PlayerMovement since the movement is disabled)

  int hero_x = hero->get_top_left_x();
  int hero_y = hero->get_top_left_y();
  switch (boomerang_direction) {

    case 0:
      set_coordinates(hero_x + 12, hero_y + 8);
      break;

    case 90:
      set_coordinates(hero_x + 8, hero_y - 12);
      break;

    case 180:
      set_coordinates(hero_x - 12, hero_y + 8);
      break;

    case 270:
      set_coordinates(hero_x + 8, hero_y + 12);
      break;

  }

  initial_coords.x = get_x();
  initial_coords.y = get_y();

  CollisionMovement *movement = new CollisionMovement();
  movement->set_speed(16);
  movement->set_direction(boomerang_direction);
  set_movement(movement);

  next_sound_date = SDL_GetTicks();
}

/**
 * Destructor.
 */
Boomerang::~Boomerang(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Boomerang::get_type() {
  return BOOMERANG;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Boomerang::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Boomerang::can_detect_entities(void) {
  return false;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Boomerang::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Boomerang::is_displayed_in_y_order(void) {
  return true;
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Boomerang::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool Boomerang::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Boomerang::is_hole_obstacle(void) {
  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param block a crystal switch block raised
 * @return false 
 */
bool Boomerang::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  // the boomerang can traverse the crystal switch blocks
  return false;
}

/**
 * Returns whether the boomerang is going back towards the hero, i.e. if go_back() has been called.
 * @return true if the boomerang is going back
 */
bool Boomerang::is_going_back(void) {
  return has_to_go_back || going_back;
}

/**
 * Makes the boomerang go back towards the hero.
 */
void Boomerang::go_back(void) {

  if (is_going_back()) {
    DIE("The boomerang is already going back");
  }

  has_to_go_back = true;
}

/**
 * Updates the boomerang.
 */
void Boomerang::update(void) {

  MapEntity::update();

  if (suspended) {
    return;
  }

  Uint32 now = SDL_GetTicks();
  if (now >= next_sound_date) {
    ResourceManager::get_sound("boomerang")->play();
    next_sound_date = now + 150;
  }

  if (!going_back) {

    if (has_to_go_back) {
      going_back = true;
      clear_movement();
      set_movement(new TargetMovement(hero, 16));
    }
    else if (get_movement()->is_stopped() || get_distance(initial_coords.x, initial_coords.y) >= 96) {
      // collision with an obstacle or time to go back
      ResourceManager::get_sound("sword_hit")->play();
      go_back();
    }
  }
  else {
    TargetMovement *movement = (TargetMovement*) get_movement();
    if (movement->is_finished()) {
      get_map()->get_entities()->remove_entity(this);
    }
  }
}

