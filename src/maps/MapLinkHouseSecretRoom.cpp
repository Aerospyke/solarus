/*
 * Map generated automatically by the map editor.
 */

#include <SDL/SDL.h>
#include "Map.h"
#include "GameResource.h"
#include "Tile.h"
#include "Color.h"
#include "ZSDX.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapLinkHouseSecretRoom.h"

/**
 * Constructor.
 */
MapLinkHouseSecretRoom::MapLinkHouseSecretRoom(void):
  Map(320, 240,
      get_color(88, 72, 72),
      TILESET_HOUSE,
      MUSIC_VILLAGE),
  initial_state(16, 128, 0) {

  default_initial_state = &initial_state;
}

/**
 * Loads the map.
 */
void MapLinkHouseSecretRoom::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"
}
