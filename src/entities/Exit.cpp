#include "entities/Exit.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * Constructor.
 * @param name name of the exit
 * @param layer layer of the exit
 * @param x x position of the exit's rectangle
 * @param y y position of the exit's rectangle
 * @param width width of the exit's rectangle
 * @param height height of the exit's rectangle
 * @param transition_style style of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param entrance_name initial state on the destination map
 */
Exit::Exit(string name, MapEntity::Layer layer, int x, int y, int width, int height,
	   Transition::Style transition_style, MapId destination_map_id, string entrance_name):
  EntityDetector(COLLISION_ORIGIN_POINT, name, layer, x, y, width, height),
  transition_style(transition_style), destination_map_id(destination_map_id), entrance_name(entrance_name) {
  
}

/**
 * Destructor.
 */
Exit::~Exit(void) {
}

/**
 * This function is called by the engine when an entity overlaps the exits.
 * This is a redefinition of EntityDetector::entity_collision().
 * The map is not notified anymore: here we just make Link leave the map.
 * @param entity_overlapping the entity overlapping the detector
 */
void Exit::entity_collision(MapEntity *entity_overlapping) {
  
  if (entity_overlapping->is_hero()) {
    zsdx->game->set_current_map(destination_map_id, entrance_name, transition_style);
  }
}
