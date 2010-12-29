/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_MAP_ENTITIES_H
#define SOLARUS_MAP_ENTITIES_H

#include "Common.h"
#include "Transition.h"
#include "entities/Obstacle.h"
#include "entities/Layer.h"
#include "entities/EntityType.h"
#include "entities/Enemy.h"
#include <vector>
#include <list>

/**
 * @brief Manages the whole content of a map.
 *
 * Each element of a map is called an entity and is an instance of
 * a subclass of MapEntity.
 * This class stores all entities of the current map:
 * the tiles, the hero, the enemies and all other entities.
 */
class MapEntities {

  private:

    friend class MapLoader;					/**< the map loader initializes the private fields of MapEntities */

    // map
    Game &game;							/**< the game running this map */
    Map &map;							/**< the map */
    int map_width8;						/**< number of 8*8 squares on a row of the map grid */
    int map_height8;						/**< number of 8*8 squares on a column of the map grid */

    // tiles
    std::vector<Tile*> tiles[LAYER_NB];				/**< all tiles of the map (a vector for each layer) */
    int obstacle_tiles_size;					/**< number of elements in the array obstacle_tiles
								 * (obstacle_tiles_size = map_width8 * map_height8) */
    Obstacle *obstacle_tiles[LAYER_NB];				/**< array of Obstacle representing which
								 * tiles are obstacles and how */
    // dynamic entities
    Hero &hero;							/**< the hero (also stored in Game because it is kept when changing maps) */

    std::list<MapEntity*> all_entities;				/**< all map entities execept the tiles and the hero;
								 * this vector is used to delete the entities 
								 * when the map is unloaded */
    std::list<MapEntity*> entities_to_remove;			/**< list of entities that need to be removed right now */

    std::list<MapEntity*>
      entities_displayed_first[LAYER_NB];			/**< all map entities that are displayed in the normal order */

    std::list<MapEntity*>
      entities_displayed_y_order[LAYER_NB]; 			/**< all map entities that are displayed in the order
								 * defined by their y position, including the hero */

    std::list<Detector*> detectors;				/**< all entities able to detect other entities
								 * on this map */

    std::list<MapEntity*>
      obstacle_entities[LAYER_NB];				/**< all entities that might be obstacle for other
								 * entities on this map, including the hero */

    std::list<Stairs*> stairs[LAYER_NB];			/**< all stairs of the map */
    std::list<CrystalSwitchBlock*>
      crystal_switch_blocks[LAYER_NB];				/**< all crystal switch blocks of the map */
    Boomerang *boomerang;					/**< the boomerang if present on the map, NULL otherwise */
    MusicId music_before_miniboss;				/**< the music that was played before starting a miniboss fight */

    void set_obstacle(int layer, int x8, int y8, Obstacle obstacle);
    void remove_marked_entities();
    void update_crystal_switch_blocks();

  public:

    // creation and destruction
    MapEntities(Game &game, Map &map);
    ~MapEntities();

    // information about the entities
    Hero& get_hero();
    Obstacle get_obstacle_tile(Layer layer, int x, int y);
    std::list<MapEntity*>& get_obstacle_entities(Layer layer);
    std::list<Detector*>& get_detectors();
    std::list<Stairs*>& get_stairs(Layer layer);
    std::list<CrystalSwitchBlock*>& get_crystal_switch_blocks(Layer layer);

    MapEntity* get_entity(EntityType type, const std::string &name);
    MapEntity* find_entity(EntityType type, const std::string &name);
    std::list<MapEntity*> get_entities(EntityType type);
    std::list<MapEntity*> get_entities_with_prefix(EntityType type, const std::string &prefix);

    // handle entities
    void add_tile(Tile *tile);
    void add_entity(MapEntity *entity);
    void remove_entity(MapEntity *entity);
    void remove_entity(EntityType type, const std::string &name);
    void bring_to_front(MapEntity *entity);
    void destroy_all_entities();
    static bool compare_y(MapEntity *first, MapEntity *second);
    void set_entity_layer(MapEntity *entity, Layer layer);

    // hero
    bool overlaps_raised_blocks(Layer layer, const Rectangle &rectangle);
    bool is_boomerang_present();
    void remove_boomerang();
    void start_boss_battle(Enemy *boss);
    void end_boss_battle();
    void start_miniboss_battle(Enemy *miniboss);
    void end_miniboss_battle();

    // game loop
    void notify_map_started();
    void set_suspended(bool suspended);
    void update();
    void display();
};

#endif

