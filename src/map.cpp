#include <SDL/SDL.h>
#include "map.h"
#include "tile.h"
#include "zsdx.h"

const int FRAMES_PER_SECOND = 50;
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // delay between two frames in millisecond

Map::Map(int width, int height, zsdx_color_t background_color,
	 Tileset *tileset):
  width(width), height(height), width8(width / 8), height8(height / 8),
  background_color(background_color),
  tileset(tileset),
  entities(new DynamicArray<MapEntity*>(256)),
  obstacle_tiles_size(width8 * height8),
  obstacle_tiles(new tile_obstacle_t[obstacle_tiles_size])
{
  // no obstacle tile
  for (int i = 0; i < obstacle_tiles_size; i++) {
    obstacle_tiles[i] = NO_OBSTACLE;
  }
}

Map::~Map() {
  unload();
  delete entities;
  delete[] obstacle_tiles;
}

void Map::add_new_tile(TileData *tile_data, SDL_Rect &where_in_map) {
  // add the tile to the map objects
  add_entity(new Tile(tile_data, where_in_map));

  // update the collision list
  int tile_x8 = where_in_map.x / 8;
  int tile_y8 = where_in_map.y / 8;
  int tile_width8 = tile_data->get_width() / 8;
  int tile_height8 = tile_data->get_height() / 8;

  // we traverse each 8*8 square in the tile
  tile_obstacle_t *obstacle;
  int index;
  for (int j = 0; j < tile_height8; j++) {
    index = (tile_y8 + j) * width8 + tile_x8;
    for (int i = 0; i < tile_width8; i++) {
      obstacle = &obstacle_tiles[index++];
      if (*obstacle != NO_OBSTACLE) {
	*obstacle = tile_data->get_obstacle();
      }
    }
  }
}

void Map::unload(void) {
  for (int i = 0; i < entities->get_size(); i++) {
    delete entities->get(i);
  }
  entities->clear();
}

SDL_Surface *Map::get_surface(void) {
  return zsdx.get_screen();
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map objects
  for (int i = 0; i < entities->get_size(); i++) {
    entities->get(i)->display_on_map(this);
  }

  // link
  game_resource->get_link()->display_on_map(this);

  SDL_Flip(surface);
}

void Map::launch(void) {
  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  bool quit = false;

  Link *link = game_resource->get_link();
  link->set_position(link_start_x, link_start_y);

  //  SDL_EnableKeyRepeat(5, 10);

  while (!quit) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
	
      case SDL_QUIT:
	quit = true;
	break;
	
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	  quit = true;
	  break;
	case SDLK_RIGHT:
	  link->start_right();
	  break;
	case SDLK_UP:
	  link->start_up();
	  break;
	case SDLK_LEFT:
	  link->start_left();
	  break;
	case SDLK_DOWN:
	  link->start_down();
	  break;
	default:
	  break;
	}
	break;
	
      case SDL_KEYUP:
	switch (event.key.keysym.sym) {
	case SDLK_RIGHT:
	  link->stop_right();
	  break;
	case SDLK_UP:
	  link->stop_up();
	  break;
	case SDLK_LEFT:
	  link->stop_left();
	  break;
	case SDLK_DOWN:
	  link->stop_down();
	  break;
	default:
	  break;
	}
	break;
      }
    }

    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_DELAY) {
      last_frame_date = ticks;
      display(zsdx.get_screen());
    }
  }
}
