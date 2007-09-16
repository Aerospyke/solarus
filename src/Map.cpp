/**
 * This module defines the class Map.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL.h>
#include "Map.h"
#include "ZSDX.h"

/**
 * Creates a new map.
 * @param width the map width in pixels
 * @param height the map height in pixels
 * @param background_color the background_color
 * @param tileset_id the map tileset
 * @param default_music_id the map default music
 */
Map::Map(int width, int height, zsdx_color_t background_color,
	 TilesetID tileset_id, MusicID default_music_id):

  width(width), height(height), width8(width / 8), height8(height / 8),
  obstacle_tiles_size(width8 * height8),
  background_color(background_color),
  tileset(ZSDX::game_resource->get_tileset(tileset_id)),
  default_music_id(default_music_id) {

  screen_position.w = 320;
  screen_position.h = 240;

  initial_states = new vector<MapInitialState*>();
  entity_detectors = new vector<EntityDetector*>();

  // create the vectors of entities and initialize obstacle tile
  for (int layer = 0; layer < LAYER_NB; layer++) {

    tiles[layer] = new vector<TileOnMap*>();

    obstacle_tiles[layer] = new Obstacle[obstacle_tiles_size];
    for (int i = 0; i < obstacle_tiles_size; i++) {
      obstacle_tiles[layer][i] = OBSTACLE_NONE;
    }
  }
}

/**
 * Destructor.
 */
Map::~Map() {
  unload();
  for (int layer = 0; layer < LAYER_NB; layer++) {
    delete tiles[layer];
    delete[] obstacle_tiles[layer];
  }

  delete initial_states;
  delete entity_detectors;
}

/**
 * Creates a tile on the map.
 * This function is called for each tile when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_id id of the tile in the tileset
 * @param layer layer of the tile to create
 * @param position_in_map x position of the tile on the map
 * @param position_in_map y position of the tile on the map
 * @param repeat_x how many times the tile pattern is repeated on x
 * @param repeat_x how many times the tile pattern is repeated on y
 */
void Map::add_new_tile(int tile_id, Layer layer, int x, int y, int repeat_x, int repeat_y) {

  // get the tile in the tileset
  Tile *tile = tileset->get_tile(tile_id);
  Obstacle obstacle = tile->get_obstacle();

  // create the tile object
  TileOnMap *tileOnMap = new TileOnMap(tile, layer, x, y, repeat_x, repeat_y);

  // add it to the map
  tiles[layer]->push_back(tileOnMap);

  // update the collision list
  int tile_x8 = x / 8;
  int tile_y8 = y / 8;
  int tile_width8 = (tile->get_width() / 8) * repeat_x;
  int tile_height8 = (tile->get_height() / 8) * repeat_y;

  int index, i, j;

  switch (obstacle) {

    /* If the tile is entirely an obstacle or entirely no obstacle,
     * then all 8*8 squares of the tile have the same property.
     */
  case OBSTACLE_NONE:
  case OBSTACLE:
    for (i = 0; i < tile_height8; i++) {
      index = (tile_y8 + i) * width8 + tile_x8;
      for (j = 0; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = obstacle;
      }
    }
    break;

    /* If the top right corner of the tile is an obstacle,
     * then the top right 8*8 squares are OBSTACLE, the bottom left
     * 8*8 squares are NO_OBSTACLE and the 8*8 squares on the diagonal
     * are OBSTACLE_TOP_RIGHT.
     */
  case OBSTACLE_TOP_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += i;
      obstacle_tiles[layer][index++] = OBSTACLE_TOP_RIGHT;

      // right part of the row: we are in the top-right corner
      for (j = i + 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }
    }
    break;
    
  case OBSTACLE_TOP_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the top-left corner
      for (j = 0; j < tile_width8 - i - 1; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = OBSTACLE_TOP_LEFT;
    }
    break;
    
  case OBSTACLE_BOTTOM_LEFT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // left part of the row: we are in the bottom-left corner
      for (j = 0; j < i; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }

      // 8*8 square on the diagonal
      obstacle_tiles[layer][index] = OBSTACLE_BOTTOM_LEFT;
    }
    break;
    
  case OBSTACLE_BOTTOM_RIGHT:
    // we traverse each row of 8*8 squares on the tile
    for (i = 0; i < tile_height8; i++) {

      index = (tile_y8 + i) * width8 + tile_x8;

      // 8*8 square on the diagonal
      index += tile_height8 - i - 1;
      obstacle_tiles[layer][index++] = OBSTACLE_BOTTOM_RIGHT;

      // right part of the row: we are in the bottom-right corner
      for (j = tile_width8 - i - 1; j < tile_width8; j++) {
	obstacle_tiles[layer][index++] = OBSTACLE;
      }
    }
    break;
  }
}

/**
 * Creates an initial state on the map.
 * @param music_id id of the music to play in this state
 * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
 * @param link_x x initial position of link in this state
 * (set -1 to indicate that the x coordinate is kept the same from the previous map)
 * @param link_y y initial position of link in this state
 * (set -1 to indicate that the y coordinate is kept the same from the previous map)
 * @param link_direction initial direction of link in this state (0 to 3)
 */
void Map::add_initial_state(MusicID music_id, int link_x, int link_y, int link_direction) {
  
  MapInitialState *initial_state = new MapInitialState(music_id, link_x, link_y, link_direction);
  initial_states->push_back(initial_state);
}

/**
 * Creates an exit on the map.
 * When Link walks on the exit, he leaves the map and enters another one.
 * @param layer layer of the exit to create
 * @param x x position of the exit rectangle
 * @param y y position of the exit rectangle
 * @param w width of the exit rectangle
 * @param h height of the exit rectangle
 * @param map_id id of the next map
 * @param initial_state_index initial state of the next map
 */
void Map::add_exit(Layer layer, int x, int y, int w, int h, MapID map_id, int initial_state_index) {
  
  MapExit *exit = new MapExit(layer, x, y, w, h, map_id, initial_state_index);
  entity_detectors->push_back(exit);
}

/**
 * Unloads the map.
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload(void) {

  // delete the tiles
  for (int layer = 0; layer < LAYER_NB; layer++) {
    for (unsigned int i = 0; i < tiles[layer]->size(); i++) {
      delete tiles[layer]->at(i);
    }
    tiles[layer]->clear();
  }

  // delete the initial states
  for (unsigned int i = 0; i < initial_states->size(); i++) {
    delete initial_states->at(i);
  }
  initial_states->clear();

  // delete the entity detectors
  for (unsigned int i = 0; i < entity_detectors->size(); i++) {
    delete entity_detectors->at(i);
  }
  entity_detectors->clear();
}

/**
 * Sets the initial state of the map when it is loaded.
 * @param initial_state_index index of the initial state you want to load
 */
void Map::set_initial_state(unsigned int initial_state_index) {
  this->initial_state_index = initial_state_index;
}

/**
 * Returns the SDL surface where the map is displayed (normally the screen).
 * @return the surface where the map is displayed
 */
SDL_Surface *Map::get_surface(void) {
  return ZSDX::get_screen();
}

/**
 * Updates the animation and the position of each sprite, including Link.
 */
void Map::update_sprites(void) {
  Link *link = ZSDX::game_resource->get_link();
  link->update_position();
  link->get_sprite()->update_current_frame();
}

/**
 * Displays the map with all its entities on the screen.
 */
void Map::display() {
  
  // Link
  Link* link = ZSDX::game_resource->get_link();

  // screen
  SDL_Surface *surface = get_surface();
  screen_position.x = MIN(MAX(link->get_x() - 160, 0), width - 320);
  screen_position.y = MIN(MAX(link->get_y() - 120, 0), height - 240);  

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map entities
  for (int layer = 0; layer < LAYER_NB; layer++) {

    // put the tiles
    for (unsigned int i = 0; i < tiles[layer]->size(); i++) {
      tiles[layer]->at(i)->display_on_map(this);
    }

    // put link if he is in this layer
    if (link->get_layer() == layer) {
      link->display_on_map(this);
    }
  }

  SDL_Flip(surface);
}

/**
 * Starts the map. The map must be loaded.
 * Link is placed on the map and the background music starts.
 */
void Map::start(void) {

  if (initial_states->size() == 0) {
    cerr << "Fatal error: no initial state defined on the map\n";
  }

  MapInitialState *initial_state = initial_states->at(initial_state_index);

  // get the new music to play
  MusicID new_music_id = initial_state->get_music_id();

  if (new_music_id == MUSIC_DEFAULT) {
    new_music_id = default_music_id;
  }

  ZSDX::game->play_music(new_music_id);

  // put Link
  Link *link = ZSDX::game_resource->get_link();
  link->set_map(this);
  link->get_sprite()->set_current_animation_direction(initial_state->get_link_direction());

  int x = initial_state->get_link_x();
  int y = initial_state->get_link_y();

  if (x != -1) {
    link->set_x(x);
  }

  if (y != -1) {
    link->set_y(y);
  }
}

/**
 * Exits the map.
 * This function is called when Link leaves the map.
 */
void Map::leave(void) {
  // nothing is done for now
}

/**
 * Tests whether a point collides with a map tile.
 * @param layer layer of the point
 * @param x x of the point in pixels
 * @param y y of the point in pixels
 * @return the obstacle property of this point
 */
Obstacle Map::pixel_collision(int layer, int x, int y) {
  Obstacle obstacle_type;
  bool on_obstacle = false;
  int x_in_tile, y_in_tile;

  // get the obstacle property of this point's 8*8 square
  obstacle_type = obstacle_tiles[layer][(y / 8) * width8 + (x / 8)];

  // test the obstacle property of this square
  switch (obstacle_type) {

  case OBSTACLE_NONE:
    // the square is not an obstacle
    on_obstacle = false;
    break;

  case OBSTACLE:
    // the square is entirely an obstacle
    on_obstacle = true;
    break;

  case OBSTACLE_TOP_RIGHT:
    // the upper right half of the square is an obstacle
    // so we have to test the position of the point
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < x_in_tile;
    break;

  case OBSTACLE_TOP_LEFT:
    // same thing
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < 8 - x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > 8 - x_in_tile;
    break;
  }

  return on_obstacle ? obstacle_type : OBSTACLE_NONE;
}

/**
 * Tests whether a rectangle collides with the map tiles.
 * @param layer layer of the rectangle in the map
 * @param collision_box the rectangle to check
 * (its dimensions should be multiples of 8)
 * @return true if the rectangle is overlapping an obstacle, false otherwise
 */
bool Map::collision_with_tiles(int layer, SDL_Rect &collision_box) {
  int x1, x2, y1, y2;
  bool collision = false;

  // we check the 4 corners of each 8*8 square in the collision box
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1 += 8) {
    y2 = y1 + 7;
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1 += 8) {
      x2 = x1 + 7;
      collision = pixel_collision(layer, x1, y1) || pixel_collision(layer, x2, y1)
	|| pixel_collision(layer, x1, y2) || pixel_collision(layer, x2, y2);
    }
  }
  
  return collision;   
}

/**
 * This function is called by an entity sensible to the obstacles
 * when this entity has just moved on the map.
 * We check whether or not the entity overlaps an entity detector.
 * @param entity the entity that has just moved
 */
void Map::entity_just_moved(MovingWithCollision *entity) {

  EntityDetector *detector;

  // check each detector
  for (unsigned int i = 0; i < entity_detectors->size(); i++) {
    
    detector = entity_detectors->at(i);
    if (detector->get_layer() == entity->get_layer()
	&& entity->overlaps(detector->get_position_in_map())) {

      detector->entity_overlaps(entity); // notify the detector
    }
  }
}

/**
 * This function is called by the game engine when an entity overlaps a detector.
 * Redefine this function to create a behavior for your detectors.
 * @param detector the detector
 * @param entity the entity
 */
void Map::event_entity_on_detector(EntityDetector *dectector, MapEntity *entity) {

}
