/**
 * This module defines the class MapExit.
 */

#ifndef ZSDX_MAP_EXIT_H
#define ZSDX_MAP_EXIT_H

#include "Common.h"
#include "EntityDetector.h"

/**
 * A map exit is a detector placed on a map.
 * When Link walks on this detector, he leaves the map and enters another one.
 */
class MapExit: public EntityDetector {

 private:

  /**
   * Type of transition between the two maps.
   */
  Transition transition;

  /**
   * Id of the destination map.
   */
  MapId destination_map_id;

  /**
   * Initial point of the destination map.
   */
  string entrance_name;

 public:

  MapExit(string name, Layer layer, int x, int y, int width, int height,
	  Transition transition, MapId destination_map_id, string entrance_name);
  virtual ~MapExit(void) { };

  void entity_overlaps(MapEntity *entity_overlapping);

};

#endif
