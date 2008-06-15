#include "EntityDetector.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"

/**
 * Constructor.
 * @param collision_mode the detector's collision mode
 * @param name a name identifying the detector (or an empty string if you
 * don't need to identify the detector)
 * @param layer layer of the detector
 * @param x x position of the detector's rectangle
 * @param y y position of the detector's rectangle
 * @param width width of the detector's rectangle
 * @param height height of the detector's rectangle
 */
EntityDetector::EntityDetector(CollisionMode collision_mode, string name, Layer layer,
			       int x, int y, int width, int height):
  MapEntity(name, 0, layer, x, y, width, height), mode(collision_mode) {
  
}

/**
 * Destructor.
 */
EntityDetector::~EntityDetector(void) {

}

/**
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on the detector collision mode, one of the check_collision_*
 * functions is called.
 * If there is a collision, the entity_collision() method is called.
 * @param entity the entity to check
 */
void EntityDetector::check_entity_collision(MapEntity *entity) {
  
  if (get_layer() == entity->get_layer()) { // the entity is in the same layer as the detector
  
    bool collision;
  
    // detect the collision depending on the collision mode
    switch (mode) {
      
    case COLLISION_WITH_ENTITY_POINT:
      collision = check_collision_point(entity);
      break;

    case COLLISION_WITH_ENTITY_RECTANGLE:
      collision = check_collision_rectangle(entity);
      break;
    }

    // if there is a collision, notify the detector
    if (collision) {
      entity_collision(entity);
    }
  }
}

/**
 * Checks whether the entity's origin point is overlapping the detector's rectangle.
 * This method is called by check_entity_collision() when the detector's collision
 * mode is COLLISION_WITH_ENTITY_POINT.
 * @param entity the entity
 * @return true if the entity's origin point is overlapping the detector's rectangle
 */
bool EntityDetector::check_collision_point(MapEntity *entity) {

  return entity->is_origin_point_in(get_position_in_map());
}

/**
 * Checks whether the entity's rectangle is overlapping the detector's rectangle.
 * This method is called by check_entity_collision() when the detector's collision
 * mode is COLLISION_WITH_ENTITY_RECTANGLE.
 * @param entity the entity
 * @return true if the entity's rectangle is overlapping the detector's rectangle
 */
bool EntityDetector::check_collision_rectangle(MapEntity *entity) {

  return entity->overlaps(get_position_in_map());
}

/**
 * This function is called by check_entity_overlapping()
 * when an entity overlaps the detector.
 * By default, the map is notified.
 * @param entity_overlapping the entity overlapping the detector
 */
void EntityDetector::entity_collision(MapEntity *entity_overlapping) {

  Map *map = zsdx->game->get_current_map();
  map->event_entity_on_detector(this, entity_overlapping);
}
