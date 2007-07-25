#include <SDL/SDL.h>
#include "Map.h"
#include "GameResource.h"
#include "maps/LinkHouse.h"
#include "tilesets/TilesetHouse.h"
#include "Tile.h"
#include "Color.h"
#include "ZSDX.h"

MapLinkHouse::MapLinkHouse(void):
  Map(320, 240,
      get_color(88, 72, 72),
      ZSDX::game_resource->get_tileset(TILESET_HOUSE),
      ZSDX::game_resource->get_music(MUSIC_VILLAGE)) {

}

void MapLinkHouse::load(void) {
  Tile *tile;
  SDL_Rect position_in_map;

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  tile = tileset->get_tile(1);
  position_in_map.x = 32;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 16, 10);

  tile = tileset->get_tile(19);
  position_in_map.x = 0;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 208;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(20);
  position_in_map.x = 200;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 312;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(21);
  position_in_map.x = 0;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 208;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(22);
  position_in_map.x = 200;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 312;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(28);
  position_in_map.x = 8;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 24, 1);

  position_in_map.x = 216;
  position_in_map.y = 16;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 12, 1);

  tile = tileset->get_tile(30);
  position_in_map.x = 8;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 24, 1);

  position_in_map.x = 216;
  position_in_map.y = 216;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 12, 1);

  tile = tileset->get_tile(29);
  position_in_map.x = 0;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 24);

  position_in_map.x = 208;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 24);

  tile = tileset->get_tile(27);
  position_in_map.x = 200;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 24);

  position_in_map.x = 312;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 24);

  tile = tileset->get_tile(31);
  position_in_map.x = 8;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 216;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(32);
  position_in_map.x = 184;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 296;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(33);
  position_in_map.x = 8;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 216;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(34);
  position_in_map.x = 184;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 296;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(40);
  position_in_map.x = 24;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 10, 1);

  position_in_map.x = 232;
  position_in_map.y = 24;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 4, 1);

  tile = tileset->get_tile(42);
  position_in_map.x = 24;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 10, 1);

  position_in_map.x = 232;
  position_in_map.y = 200;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 4, 1);

  tile = tileset->get_tile(41);
  position_in_map.x = 8;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 10);

  position_in_map.x = 216;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 10);

  tile = tileset->get_tile(39);
  position_in_map.x = 184;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 10);

  position_in_map.x = 296;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 10);

  tile = tileset->get_tile(3);
  position_in_map.x = 24;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 232;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(4);
  position_in_map.x = 176;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 288;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(5);
  position_in_map.x = 24;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 232;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(6);
  position_in_map.x = 176;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 288;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  tile = tileset->get_tile(12);
  position_in_map.x = 32;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 18, 1);

  position_in_map.x = 240;
  position_in_map.y = 40;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 6, 1);

  tile = tileset->get_tile(14);
  position_in_map.x = 32;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 18, 1);

  position_in_map.x = 240;
  position_in_map.y = 192;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 6, 1);

  tile = tileset->get_tile(13);
  position_in_map.x = 24;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 18);

  position_in_map.x = 232;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 18);

  tile = tileset->get_tile(11);
  position_in_map.x = 176;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 18);

  position_in_map.x = 288;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW, 1, 18);

  tile = tileset->get_tile(17);
  position_in_map.x = 0;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 0;
  position_in_map.y = 176;
  add_new_tile(tile, position_in_map, LAYER_BELOW);
  
  tile = tileset->get_tile(15);
  position_in_map.x = 304;
  position_in_map.y = 48;
  add_new_tile(tile, position_in_map, LAYER_BELOW);

  position_in_map.x = 304;
  position_in_map.y = 176;
  add_new_tile(tile, position_in_map, LAYER_BELOW);
  
  /* test of animated tile:
  tile = tileset->get_tile(72);
  position_in_map.x = 48;
  position_in_map.y = 64;
  add_new_tile(tile, position_in_map, LAYER_BELOW);
  */

  // link start position
  link_start_x = 80;
  link_start_y = 80;
}
