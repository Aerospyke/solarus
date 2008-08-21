#ifndef ZSDX_RESOURCE_MANAGER_H
#define ZSDX_RESOURCE_MANAGER_H

#include "Common.h"
#include <map>

/**
 * This class provides access to all external resources:
 * images, sounds, maps...
 * Except for the images, all resources are stored into
 * memory when you get them for the first time and you
 * don't have to delete them. They are deleted when the application
 * ends. This does not consumes much memory because:
 * - the maps and tilesets have an unload() method you can call,
 * - the musics and sounds close their audio files when they stop.
 * The images are the only kind of resources that you have to
 * free yourself.
 */
class ResourceManager {

 private:

  static ResourceManager *instance;

  std::map<TilesetId, Tileset*> tilesets;
  std::map<MapId, Map*> maps;
  std::map<MusicId, Music*> musics;
  std::map<SoundId, Sound*> sounds;
  std::map<SpriteAnimationsId, SpriteAnimations*> sprite_animations;

  // items
  // ...

  ResourceManager(void);
  ~ResourceManager(void);

 public:

  static void initialize(void);
  static void quit(void);

  static SDL_Surface* load_image(ImageId id);

  static Tileset* get_tileset(TilesetId id);
  static Map* get_map(MapId id);
  static Music* get_music(MusicId id);
  static Sound* get_sound(SoundId id);
  static SpriteAnimations* get_sprite_animations(SpriteAnimationsId id);
};

#endif
