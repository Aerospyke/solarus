/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/Tileset.h"
#include "entities/SimpleTilePattern.h"
#include "entities/AnimatedTilePattern.h"
#include "entities/ParallaxTilePattern.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include <iomanip>
using namespace std;

/**
 * Constructor.
 * @param id id of the tileset to create
 */
Tileset::Tileset(TilesetId id):
  id(id), nb_tile_patterns(0), max_tile_id(0),
  tiles_image(NULL), entities_image(NULL) {
  
  for (int i = 0; i < 1024; i++) {
    tile_patterns[i] = NULL;
  }
}

/**
 * Destructor.
 */
Tileset::~Tileset(void) {
  if (is_loaded()) {
    unload(); // destroy the tiles
  }
}

/**
 * Adds a tile pattern in this tileset.
 * This function is called by load().
 * @param id id of this tile pattern (1 to 1024)
 * @param tile_pattern the tile pattern to add
 */
void Tileset::add_tile_pattern(int id, TilePattern *tile) {
  tile_patterns[id - 1] = tile;
  nb_tile_patterns++;

  max_tile_id = MAX(id, max_tile_id);
}

/**
 * Loads the tileset by creating all tile patterns.
 */
void Tileset::load(void) {

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "tilesets/tileset" << std::setfill('0') << std::setw(4) << id << ".zsd";
  const string &file_name = FileTools::data_file_add_prefix(oss.str());

  // open the tileset file
  std::ifstream tileset_file(file_name.c_str());

  if (!tileset_file) {
    DIE("Cannot open file '" << file_name << "'");
  }

  // parse the tileset file
  string line;

  // first line: tileset general info
  if (!std::getline(tileset_file, line)) {
    DIE("Empty file '" << file_name << "'");
  }

  int r, g, b;

  std::istringstream iss0(line);
  iss0 >> r >> g >> b;
  background_color = Color::create(r, g, b);

  // read the tile patterns
  int tile_pattern_id, animation, obstacle, default_layer;
  while (std::getline(tileset_file, line)) {

    std::istringstream iss(line);
    iss >> tile_pattern_id >> animation >> obstacle >> default_layer;

    int width, height;

    if (animation == 0 || animation == 2) { // simple tile pattern or tile pattern with parallax

      int x, y;

      iss >> x >> y >> width >> height;

      TilePattern *pattern;
      if (animation == 0) {
	      pattern = new SimpleTilePattern(Obstacle(obstacle), x, y, width, height);
      }
      else {
	      pattern = new ParallaxTilePattern(Obstacle(obstacle), x, y, width, height);
      }
      add_tile_pattern(tile_pattern_id, pattern);
    }
    else { // animated tile pattern
      int sequence, x1, y1, x2, y2, x3, y3;

      iss >> sequence >> width >> height >> x1 >> y1 >> x2 >> y2 >> x3 >> y3;
      add_tile_pattern(tile_pattern_id,
		       new AnimatedTilePattern(Obstacle(obstacle),
					       AnimatedTilePattern::AnimationSequence(sequence),
					       width, height, x1, y1, x2, y2, x3, y3));
    }
  }

  // load the tileset images
  std::ostringstream oss2;
  oss2 << "../tilesets/tileset" << std::setfill('0') << std::setw(4) << id << "_tiles.png";
  tiles_image = ResourceManager::load_image(oss2.str());

  if (tiles_image == NULL) {
    DIE("Cannot load the image '" << file_name << "'");
  }

  std::ostringstream oss3;
  oss3 << "../tilesets/tileset" << std::setfill('0') << std::setw(4) << id << "_entities.png";
  entities_image = ResourceManager::load_image(oss3.str());

  if (entities_image == NULL) {
    DIE("Cannot load the image '" << file_name << "'");
  }
}

/**
 * Destroys the tile patterns and frees the memory used
 * by the tileset image.
 */
void Tileset::unload(void) {
  int i;

  for (i = 0; i < max_tile_id; i++) {
    if (tile_patterns[i] != NULL) {
      delete tile_patterns[i];
    }
  }
  nb_tile_patterns = 0;

  SDL_FreeSurface(tiles_image);
  tiles_image = NULL;

  SDL_FreeSurface(entities_image);
  entities_image = NULL;
}

/**
 * Returns the background color of this tileset.
 * @return the background color
 */
Uint32 Tileset::get_background_color(void) {
  return background_color;
}

/**
 * Returns whether this tileset is loaded.
 * @return true if this tileset is loaded
 */
bool Tileset::is_loaded(void) {
  return tiles_image != NULL;
}

/**
 * Returns the image containing the tiles of this tileset.
 * @return the tiles image
 */
SDL_Surface * Tileset::get_tiles_image(void) {
  return tiles_image;
}

/**
 * Returns the image containing the skin-dependent dynamic entities for this tileset.
 * @return the image containing the skin-dependent dynamic entities for this tileset
 */
SDL_Surface * Tileset::get_entities_image(void) {
  return entities_image;
}

/**
 * Returns a tile pattern from this tileset.
 * @param id id of the tile pattern to get
 * @return the tile pattern with this id
 */
TilePattern * Tileset::get_tile_pattern(int id) {

  TilePattern *tile_pattern =  tile_patterns[id - 1];
  if (tile_pattern == NULL) {
    DIE("There is not tile pattern with id '" << id << "' in this tileset'");
  }
  return tile_pattern;
}
