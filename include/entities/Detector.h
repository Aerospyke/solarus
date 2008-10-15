#ifndef ZSDX_DETECTOR_H
#define ZSDX_DETECTOR_H

#include "Common.h"
#include "MapEntity.h"

/**
 * A detector is an object placed somewhere on the map
 * to detect the presence of Link or other moving entities.
 * Examples of detectors are exits, switches, bushes and enemies.
 */
class Detector: public MapEntity {

 public:

  /**
   * Collisions modes of a detector.
   */
  enum CollisionMode {
    COLLISION_NONE         = 0x0000, /**< no collision will be detected
				      * (the detector doesn't detect any entity) */
    COLLISION_RECTANGLE    = 0x0001, /**< collision if an entity's rectangle
				      * overlaps the detector's rectangle */
    COLLISION_ORIGIN_POINT = 0x0002, /**< collision if an entity's origin point
				      * is inside the detector's rectangle */
    COLLISION_FACING_POINT = 0x0004, /**< collision if an entity's facing point
				      * overlaps the detector's rectangle */
    COLLISION_SPRITE       = 0x0008, /**< collision if an entity's sprite has pixels
				      * overlapping pixels of the detector's sprite */
  };

 private:

  int collision_mode; /**< collision mode of the detector (can be an OR combination of CollisionMode values) */

  bool layer_ignored;

  bool check_collision_rectangle(MapEntity *entity);
  bool check_collision_origin_point(MapEntity *entity);
  bool check_collision_facing_point(MapEntity *entity);
  bool check_collision_sprite(MapEntity *entity, Sprite *sprite);
  
 protected:

  Detector(int collision_mode, string name, Layer layer,
	   int x, int y, int width, int height);

  void set_collision_mode(int collision_mode);
  void set_layer_ignored(bool layer_ignored);

  virtual void collision(MapEntity *entity_overlapping);
  virtual void collision(MapEntity *entity_overlapping, Sprite *sprite_overlapping);

 public:

  virtual ~Detector(void);

  virtual void check_collision(MapEntity *entity);
  virtual void check_collision(MapEntity *entity, Sprite *sprite);
  virtual void action_key_pressed(void);
};

#endif
