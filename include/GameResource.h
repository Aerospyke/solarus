/**
 * This module handles the game resources.
 */

#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "Link.h"
#include "Tileset.h"
#include "Map.h"

// tilesets

enum TilesetID {
#include "TilesetList.inc"
  NB_TILESETS
};

// maps
enum MapID {
#include "MapList.inc"
  NB_MAPS
};

// musics
enum MusicID {
  MUSIC_VILLAGE,
  MUSIC_OVERWORLD,
  MUSIC_BOSS,
  MUSIC_CASTLE,
  MUSIC_CREDITS,
  MUSIC_TITLE_SCREEN,
  MUSIC_FANFARE,
  MUSIC_GAME_OVER,
  MUSIC_RABBIT,
  MUSIC_MENU,
  MUSIC_MINI_GAME,
  MUSIC_DARK_WORLD,
  MUSIC_SOLDIERS,
  NB_MUSICS
};

/* This class is a database of all game resources:
 * - the tileset list
 * - the map list
 * - the music list
 * - the sound list
 * - the colors
 * - ...
 */
class GameResource {

 private:

  /**
   * Link.
   */
  Link *link;

  /**
   * The tilesets.
   */
  Tileset *tilesets[NB_TILESETS];

  /**
   * The maps.
   */
  Map *maps[NB_MAPS];

  /**
   * The musics.
   */
  Music *musics[NB_MUSICS];

  // musics
  // sounds
  // items
  // saved data?
  // ...

 public:

  /**
   * Constructor.
   */
  inline GameResource(void) { }

  /**
   * Destructor.
   * Destroys all the game resources.
   */
  ~GameResource(void);

  /**
   * Creates all resources.
   */
  void create_resources(void);

  /**
   * Returns Link.
   * @return Link
   */
  inline Link *get_link() { return link; }

  /**
   * Returns a tileset.
   * @param tileset_id id of the tileset to get
   */
  Tileset *get_tileset(TilesetID tileset_id);

  /**
   * Returns a map.
   * @param map_id id of the map to get
   */
  Map *get_map(MapID map_id);

  /**
   * Returns a music.
   * @param music_id id of the music to get
   */
  Music *get_music(MusicID music_id);
};

#endif
