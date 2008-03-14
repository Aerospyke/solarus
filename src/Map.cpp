/**
 * This module defines the class Map.
 */

#include <string>
#include <sstream>
#include <fstream>
#include "Map.h"
#include "ZSDX.h"
#include "Game.h"
#include "GameResource.h"
#include "AnimatedSprite.h"
#include "Link.h"
#include "Tileset.h"
#include "Tile.h"
#include "TileOnMap.h"
#include "MapEntrance.h"
#include "MapExit.h"
#include "FileTools.h"

/**
 * Creates a map.
 * @param id id of the map, used to determine the description file
 * and the script file of the map
 */
Map::Map(MapId id):
id(id), started(false), width(0) {

}

/**
 * Destructor.
 */
Map::~Map(void) {

  if (is_loaded()) {
    unload();
  }
}

/**
 * Returns whether the map is loaded.
 * @return true if the map is loaded, false otherwise
 */
bool Map::is_loaded(void) {
  return this->width != 0;
}

/**
 * Unloads the map.
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload(void) {

  SDL_FreeSurface(visible_surface);

  // delete the tiles
  for (int layer = 0; layer < LAYER_NB; layer++) {
    for (unsigned int i = 0; i < tiles[layer]->size(); i++) {
      delete tiles[layer]->at(i);
    }
    tiles[layer]->clear();
    delete tiles[layer];
    delete[] obstacle_tiles[layer];
  }

  // delete the entrances
  for (unsigned int i = 0; i < entrances->size(); i++) {
    delete entrances->at(i);
  }
  entrances->clear();
  delete entrances;

  // delete the entity detectors
  for (unsigned int i = 0; i < entity_detectors->size(); i++) {
    delete entity_detectors->at(i);
  }
  entity_detectors->clear();
  delete entity_detectors;

  width = 0;
}

/**
 * Loads the map.
 * Reads the description file of the map.
 */
void Map::load() {

  this->visible_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);

  // read the file

  char file_name[20];
  sprintf(file_name, "maps/map%04d.zsd", (int) id);
  ifstream map_file(FileTools::data_file_add_prefix(file_name));

  if (!map_file) {
    cerr << "Cannot open file '" << file_name << "'" << endl;
    exit(1);
  }

  string line;
  TilesetId tileset_id;

  // first line: map general info
  if (!std::getline(map_file, line)) {
    cerr << "Empty file '" << file_name << "'" << endl;
    exit(1);
  }

  istringstream iss0(line);
  iss0 >> width >> height >> tileset_id >> music_id;

  this->width8 = width / 8;
  this->height8 = height / 8;
  this->obstacle_tiles_size = width8 * height8;

  tileset = zsdx->game_resource->get_tileset(tileset_id);
  if (!tileset->is_loaded()) {
    tileset->load();
  }

  entrances = new vector<MapEntrance*>();
  entity_detectors = new vector<EntityDetector*>();

  // create the vectors of entities and initialize obstacle tile
  for (int layer = 0; layer < LAYER_NB; layer++) {

    tiles[layer] = new vector<TileOnMap*>();

    obstacle_tiles[layer] = new Obstacle[obstacle_tiles_size];
    for (int i = 0; i < obstacle_tiles_size; i++) {
      obstacle_tiles[layer][i] = OBSTACLE_NONE;
    }
  }

  // read the entities
  int entity_type, layer, x, y, width, height, direction;
  string entity_name;
	
  while (std::getline(map_file, line)) {

    istringstream iss(line);
    iss >> entity_type >> layer >> x >> y;

    switch (entity_type) {

    case ENTITY_TILE:
      {
	int tile_id;

	iss >> width >> height >> tile_id;
	add_new_tile(tile_id, (Layer) layer, x, y, width, height);
	break;
      }
      
    case ENTITY_ENTRANCE:
      {
	iss >> entity_name >> direction;
	add_entrance(entity_name, (Layer) layer, x, y, direction);
	break;
      }
      
    case ENTITY_EXIT:
      {
	int transition_type;
	MapId destination_map_id;	
	string entrance_name;
	iss >> width >> height >> entity_name >> transition_type >> destination_map_id >> entrance_name;
	add_exit(entity_name, (Layer) layer, x, y, width, height,
		 (TransitionType) transition_type, destination_map_id, entrance_name);
	break;
      }
    }
  }
}

/**
 * Creates a tile on the map.
 * This function is called for each tile when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_id id of the tile in the tileset
 * @param layer layer of the tile to create
 * @param position_in_map x position of the tile on the map
 * @param position_in_map y position of the tile on the map
 * @param width width in pixels (the pattern will be repeated on x to fit this width)
 * @param height height in pixels (the pattern will be repeated on y to fit this height
 */
void Map::add_new_tile(int tile_id, Layer layer, int x, int y, int width, int height) {

  // get the tile in the tileset
  Tile *tile = tileset->get_tile(tile_id);
  Obstacle obstacle = tile->get_obstacle();

  int repeat_x = width / tile->get_width();
  int repeat_y = height / tile->get_height();

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
 * Creates an entrance on the map.
 * @param entrance_name a string identifying this new entrance
 * @param layer the layer of Link's position
 * @param link_x x initial position of link in this state
 * (set -1 to indicate that the x coordinate is kept the same from the previous map)
 * @param link_y y initial position of link in this state
 * (set -1 to indicate that the y coordinate is kept the same from the previous map)
 * @param link_direction initial direction of link in this state (0 to 3)
 */
void Map::add_entrance(string entrance_name, Layer layer, int link_x, int link_y, int link_direction) {
  
  MapEntrance *entrance = new MapEntrance(entrance_name, layer, link_x, link_y, link_direction);
  entrances->push_back(entrance);
}

/**
 * Creates an exit on the map.
 * When Link walks on the exit, he leaves the map and enters another one.
 * @param exit_name a string identifying this new exit
 * @param layer layer of the exit to create
 * @param x x position of the exit rectangle
 * @param y y position of the exit rectangle
 * @param w width of the exit rectangle
 * @param h height of the exit rectangle
 * @param transition_type type of transition between the two maps
 * @param map_id id of the next map
 * @param entrance_name name of the entrance of the next map
 */
void Map::add_exit(string exit_name, Layer layer, int x, int y, int w, int h,
		   TransitionType transition_type, MapId map_id, string entrance_name) {
  
  MapExit *exit = new MapExit(exit_name, layer, x, y, w, h, transition_type, map_id, entrance_name);
  entity_detectors->push_back(exit);
}

/**
 * Sets the current entrance of the map.
 * @param entrance_index index of the entrance you want to use
 */
void Map::set_entrance(unsigned int entrance_index) {

  if (entrance_index < 0 || entrance_index >= entrances->size()) {
    cerr << "Fatal error: unknown entrance '" << entrance_index << "' on map '" << id << '\'' << endl;
    exit(1);
  }

  this->entrance_index = entrance_index;
}

/**
 * Sets the current entrance of the map.
 * @param entrance_name name of the entrance you want to use
 */
void Map::set_entrance(string entrance_name) {

  bool found = false;
  unsigned int i;
  for (i = 0; i < entrances->size() && !found; i++) { 
    found = (entrances->at(i)->get_name() == entrance_name);
  }

  if (found) {
    this->entrance_index = i - 1;
  }
  else {
    cerr << "Fatal error: unknown entrance '" << entrance_name << "' on map '" << id << '\'' << endl;
    exit(1);
  }
}

/**
 * Returns the SDL surface where the map is displayed.
 * This surface is only the visible part of the map, so the
 * coordinates on this surface are relative to the screen,
 * not to the map.
 * @return the surface where the map is displayed
 */
SDL_Surface *Map::get_visible_surface(void) {
  return visible_surface;
}

/**
 * Updates the animation and the position of each sprite, including Link.
 */
void Map::update_sprites(void) {
  Link *link = zsdx->game_resource->get_link();
  link->update_position();
  link->update_sprites();
}

/**
 * Displays the map with all its entities on the screen.
 */
void Map::display() {
  
  // Link
  Link* link = zsdx->game_resource->get_link();

  // screen
  screen_position.x = MIN(MAX(link->get_x() - 160, 0), width - 320);
  screen_position.y = MIN(MAX(link->get_y() - 120, 0), height - 240);  

  // background color
  SDL_FillRect(visible_surface, NULL, tileset->get_background_color());

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
}

/**
 * Displays a sprite on the map surface.
 * @param sprite the sprite to display
 * @param x x coordinate of the sprite's hotspot in the map
 * @param x y coordinate of the sprite's hotspot in the map
 */
void Map::display_sprite(AnimatedSprite *sprite, int x, int y) {
  
  // the position is given in the map coordinate system:
  // convert it to the visible surface coordinate system
  sprite->display(visible_surface, x - screen_position.x, y - screen_position.y);
}

/**
 * Starts the map. The map must be loaded.
 * Link is placed on the map and the background music starts.
 */
void Map::start(void) {

  MapEntrance *entrance = entrances->at(entrance_index);

  zsdx->game->play_music(music_id);

  // put Link
  Link *link = zsdx->game_resource->get_link();
  link->set_map(this, entrance->get_direction());

  int x = entrance->get_x();
  int y = entrance->get_y();

  if (x != -1) {
    link->set_x(x);
  }

  if (y != -1) {
    link->set_y(y);
  }

  SDL_SetAlpha(visible_surface, SDL_SRCALPHA, 255);

  started = true;
}

/**
 * Exits the map.
 * This function is called when Link leaves the map.
 */
void Map::leave(void) {
  started = false;
}

/**
 * Returns whether the map is started, i.e. whether it is the current
 * map and Link is on it.
 * @return true if the map is started
 */
bool Map::is_started(void) {
  return started;
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

  // if the point is outside the map, there is no obstacle (useful when Link goes on a map exit)
  if (x < 0 || x >= width
      || y < 0 || y >= height) {
    return OBSTACLE_NONE;
  }

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
	&& entity->is_hotspot_in(detector->get_position_in_map())) {

      detector->entity_overlaps(entity); // notify the detector
    }
  }
}

/**
 * This function is called by a detector when an entity is overlapping it.
 * I think think function (as all "events") should call the LUA script of the map.
 * Note that all detectors don't notify the map (i.e. they don't call this function)
 * because they may have their own behavior.
 * @param detector the detector
 * @param entity the entity
 */
void Map::event_entity_on_detector(EntityDetector *detector, MapEntity *entity) {

}
