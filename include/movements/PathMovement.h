#ifndef ZSDX_PATH_MOVEMENT_H
#define ZSDX_PATH_MOVEMENT_H

#include "Common.h"
#include "Movement.h"

/**
 * Movement for an entity that follows a predetermined path.
 * A path is a succession of translation vectors.
 */
class PathMovement: public Movement {

 private:

  // movement properties

  /**
   * The succession of translations.
   * Each element of the array represents a translation vector
   * in pixels (only the fields x and y of the SDL_Rect are used).
   */
  const SDL_Rect *translation_vectors;

  /**
   * Number of translation vectors: this is the size of the
   * translation_vectors array.
   */
  const int nb_vectors;

  /**
   * Delay in milliseconds between two translations.
   */
  const Uint32 delay;

  /**
   * Should the movement return to the beginning once finished?
   */
  const bool loop;

  // current state

  /**
   * Current translation vector in the array.
   */
  int vector_index;

  /**
   * Indicates whether the entity has reached the end of the path
   * (only when loop is false, i.e. the path does have an end).
   */
  bool path_ended;

 protected:

  void set_translation_vectors(const SDL_Rect *translation_vectors);
  void update_x(void);
  virtual void make_next_move(void);
  int get_vector_index(void);
  int get_length(void);

 public:

  PathMovement(int nb_vectors, Uint32 delay, bool loop);
  PathMovement(const SDL_Rect *translation_vectors, int nb_vectors, 
	       Uint32 delay, bool loop);
  virtual ~PathMovement(void);

  bool is_finished(void);

};

#endif
