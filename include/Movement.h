#ifndef ZSDX_MOVEMENT_H
#define ZSDX_MOVEMENT_H

#include "Common.h"

/**
 * Abstract class for the movement of an entity on the map, visible or not.
 * A moving entity has an instance of Movement which modifies its coordinates.
 * An instance of Movement is attached to one and only one instance of MapEntity.
 */
class Movement {

 protected:

  /**
   * The entity controlled by this movement.
   */
  MapEntity *entity;

  // speed vector

  /**
   * X speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving to the right
   * negative value: moving to the left
   */
  int x_speed;

  /**
   * Y speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving downwards
   * negative value: moving upwards
   */
  int y_speed;

  /**
   * Date of the next x move in ticks.
   */
  Uint32 next_move_date_x;

  /**
   * Date of the next y move in ticks.
   */
  Uint32 next_move_date_y;

  /**
   * Date of the last x or y move.
   */
  Uint32 last_move_date;

  // the following fields are redundant and can be computed
  // with x_speed and y_speed

  /**
   * Delay in ticks between an x move of 2 pixels.
   * x_delay = 200 / |x_speed|
   */
  Uint32 x_delay;

  /**
   * Delay in ticks between an y move of 2 pixels.
   * y_delay = 200 / |y_speed|
   */
  Uint32 y_delay;

  /**
   * Number of pixels of the next x move : 0, 2 or -2.
   * The smallest move is two pixels because an entity
   * doesn't need to make a move of only one pixel. 
   */
  int x_move;

  /**
   * Number of pixels of the next y move : 0, 2 or -2.
   */
  int y_move;

  Movement(void);

  void translate_x(int dx);
  void translate_y(int dy);
  void translate(int dx, int dy);

  virtual void update_x(void);
  virtual void update_y(void);

 public:

  virtual ~Movement(void);

  // entity
  void set_entity(MapEntity *entity);

  // position

  int get_x(void);
  int get_y(void);
  virtual void set_x(int x);
  virtual void set_y(int y);
  void set_position(int x, int y);

  virtual void update(void);
  bool has_to_move_now(void);

  // movement

  int get_x_speed();
  int get_y_speed();
  int get_speed(void);

  void set_x_speed(int x_speed);
  void set_y_speed(int y_speed);
  void set_speed(int speed);
  void stop(void);

  void set_direction(int direction);
};

#endif
