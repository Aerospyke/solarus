/**
 * This module defines the class Tileset.
 */

#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include <SDL/SDL.h>

class Tile;

/**
 * Abstract class for a tileset.
 * A tileset is an image with a set of elements (tiles)
 * one can use to compose a map.
 * See the directory images/tilesets.
 */
class Tileset {

 protected:
  /**
   * Tiles of the tileset.
   * The maximum id is 1024.
   */
  Tile *tiles[1024];
  
  /**
   * Number of tiles in the tileset.
   */
  int nb_tiles;
  
  /**
   * Image from which the tiles are extracted.
   */
  SDL_Surface *tileset_image;
  
  /**
   * Constructor.
   */
  Tileset(void);

  /**
   * Loads the tileset image.
   * This function should be called by load().
   * @param file_name name of the tileset image file
   */
  void load_tileset_image(const char *file_name);

  /**
   * Creates a new tile in the tileset.
   * This function is called by load().
   * @param tile the tile to add
   * @param id id of this tile (1 to 1024)
   */
  void create_tile(Tile *tile, int id);

 public:

  /**
   * Destructor.
   */
  virtual ~Tileset(void);

  /**
   * Loads the tileset by creating all tiles.
   * This function is abstract so that each tileset can load its own tiles.
   * The tiles can be instances of SimpleTile, AnimatedTile and ExtensibleTiles.
   * Extensible tiles are handled
   */
  virtual void load(void) = 0;

  /**
   * Destroys the tiles and frees the memory used
   * by the tileset image.
   */
  void unload(void);

  /**
   * Returns whether the tileset is loaded.
   * @return true if the tileset is loaded
   */
  inline bool is_loaded(void) { return nb_tiles != 0; }

  /**
   * Returns the tileset image.
   * @return the tileset image
   */
  inline SDL_Surface *get_image(void) { return tileset_image; }

  /**
   * Returns a tile from this tileset.
   * @param id id of the tile to get
   * @return the tile with this id
   */
  inline Tile *get_tile(int id) { return tiles[id - 1]; }
};

#endif
