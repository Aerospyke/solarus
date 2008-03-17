/**
 * This module defines the class Sound.
 */

#ifndef ZSDX_SOUND_H
#define ZSDX_SOUND_H

#include <fmodex/fmod.h>
#include "Common.h"

/**
 * This class represents a sound effet that can be played in ZSDX.
 * This class also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD Ex.
 */
class Sound {

 protected:

  static FMOD_SYSTEM *system;

  FMOD_SOUND *sound;
  FMOD_CHANNEL *channel;

  /**
   * Name of the sound file.
   */
  string file_name;

  inline Sound(void) { }

 public:

  Sound(SoundId sound_id);
  virtual ~Sound(void);

  static void initialize(void);
  static void exit(void);
  static bool is_initialized(void);

  virtual bool play(void);
  bool is_playing(void);
};

#endif
