#ifndef ZSDX_DYNAMIC_TILE_H
#define ZSDX_DYNAMIC_TILE_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * A dynamic tile is a tile placed on the map 
 * that can be enabled or disabled by the script,
 * contrary to the TileOnMap instances that are static for performance reasons.
 */
class DynamicTile: public MapEntity {

 private:

  TilePattern *tile_pattern;   /**< pattern of the tile: instance of SimpleTile or AnimatedTile */
  bool enabled;                /**< indicates that the tile is enabled (visible) */

 public:

  DynamicTile(string name, TilePattern *tile_pattern, Layer layer, int x, int y,
	      int width, int height, bool visible);
  ~DynamicTile(void);

  EntityType get_type(void);
  bool is_obstacle_for(MapEntity *other);
  void display_on_map(void);
};

#endif
