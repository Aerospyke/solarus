#include "GameResource.h"
#include "Link.h"
#include "Map.h"
#include "Tileset.h"
#include "Music.h"
#include "SpriteAnimations.h"

/**
 * Constructor.
 */
GameResource::GameResource(void) {

  link = NULL;
}

/**
 * Destructor.
 * Destroys all the game resources.
 */
GameResource::~GameResource(void) {

  delete link;

  // tilesets
  {
    map<TilesetId, Tileset*>::const_iterator it;
    
    for (it = tilesets.begin(); it != tilesets.end(); it++) {
      delete it->second;
    }
    tilesets.clear();
  }

  // maps
  {
    map<MapId, Map*>::const_iterator it;
    
    for (it = maps.begin(); it != maps.end(); it++) {
      delete it->second;
    }
    maps.clear();
  }

  // musics
  {
    map<MusicId, Music*>::const_iterator it;
    
    for (it = musics.begin(); it != musics.end(); it++) {
      delete it->second;
    }
    musics.clear();
  }

  // sprites
  {
    map<SpriteId, SpriteAnimations*>::const_iterator it;
    
    for (it = sprites.begin(); it != sprites.end(); it++) {
      delete it->second;
    }
    sprites.clear();
  }
}

/**
 * Returns Link.
 * @return link
 */
Link* GameResource::get_link(void) {
  
  if (link == NULL) {
    link = new Link();
  }
  
  return link;
}

/**
 * Returns a tileset.
 * @param id id of the tileset to get
 * @return the tileset
 */
Tileset* GameResource::get_tileset(TilesetId id) {
  
  if (tilesets[id] == NULL) {
    tilesets[id] = new Tileset(id);
  }
  
  return tilesets[id];
}

/**
 * Returns a map.
 * @param id id of the map to get
 * @return the map
 */
Map* GameResource::get_map(MapId id) {
  
  if (maps[id] == NULL) {
    maps[id] = new Map(id);
  }
  
  return maps[id];
}

/**
 * Returns a music.
 * @param id id of the music to get
 * @return the music
 */
Music* GameResource::get_music(MusicId id) {
  
  if (musics[id] == NULL) {
    musics[id] = new Music(id);
  }
  
  return musics[id];
}

/**
 * Returns a sprite.
 * @param id id of the sprite to get
 * @return the sprite
 */
SpriteAnimations* GameResource::get_sprite(SpriteId id) {
  
  if (sprites[id] == NULL) {
    sprites[id] = new SpriteAnimations(id);
  }
  
  return sprites[id];
}
