#include "zsdx.h"
#include "game.h"
#include "game_resource.h"
#include <iostream>

int main(int argc, char **argv) {
  Map *map = game_resource->get_map(MAP_LINK_HOUSE);
  for (int i = 0; i < 1; i++) {
    map->load();
    map->start();
    map->exit();
    map->unload();
  }
  
  return 0;
}
