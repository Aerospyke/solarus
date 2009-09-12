/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Sound.h"
#include "Music.h"
#include "FileTools.h"
#include <AL/alut.h>

ALCdevice * Sound::device = NULL;
ALCcontext * Sound::context = NULL;
bool Sound::initialized = false;
std::list<ALuint> Sound::sources;

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(const SoundId &sound_id):
  buffer(AL_NONE) {

  if (is_initialized()) {

    std::string file_name = (std::string) "sounds/" + sound_id + ".wav";

    // load the wav file
    size_t wav_size;
    char *wav_data;
    FileTools::data_file_open_buffer(file_name, &wav_data, &wav_size);

    // create an OpenAL buffer with the sound decoded by ALUT
    buffer = alutCreateBufferFromFileImage((ALvoid*) wav_data, wav_size);
    FileTools::data_file_close_buffer(wav_data);

    if (buffer == AL_NONE) {
      std::cerr << "Cannot decode WAV data for sound '" << file_name << "': " << alutGetErrorString(alutGetError()) << std::endl;
    }
  }
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {

  if (is_initialized()) {
    alDeleteBuffers(1, &buffer);
  }
}

/**
 * Initializes the audio (music and sound) system.
 * This method should be called when the application starts.
 */
void Sound::initialize(void) {

  // initialize OpenAL
  /*
  if (!alutInit(NULL, NULL)) {
    std::cout << "Cannot initialize alut: " << alutGetErrorString(alutGetError()) << std::endl;
    return;
  }
  */

  alutInitWithoutContext(NULL, NULL);
/*
  const ALCchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
  while (devices[0] != '\0') {
    std::cout << "Audio device: " << devices << std::endl;
    devices += strlen(devices) + 1;
  }
  */

  device = alcOpenDevice(NULL);
//  device = alcOpenDevice("ALSA Software on ATI IXP");
  if (!device) {
    std::cout << "Cannot open audio device" << std::endl;
    return;
  }

  const int attr[2] = {ALC_FREQUENCY, 32000}; // 32 KHz is the SPC output sampling rate
  context = alcCreateContext(device, attr);
  if (!context) {
    std::cout << "Cannot create audio context" << std::endl;
    alcCloseDevice(device);
    return;
  }
  if (!alcMakeContextCurrent(context)) {
    std::cout << "Cannot activate audio context" << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return;
  }

  initialized = true;

  // initialize the music system
  Music::initialize();

  /* Let the audio thread enough time to get initialized well,
   * otherwise the very first sound may be rendered badly.
   * I have this problem with OpenAL and Ubuntu 9.04 (Jaunty).
   * openal version: 1.4.272
   * pulseaudio version: 0.9.14
   */
  //SDL_Delay(500); // not working when going directly to game phase
}

/**
 * Closes the audio (music and sound) system.
 * This method should be called when exiting the application.
 */
void Sound::quit(void) {

  if (is_initialized()) {
  
    // stop the sound sources
    ALuint source;
    std::list<ALuint>::iterator it;
    for (it = sources.begin(); it != sources.end(); it++) {
      alSourcei(source, AL_BUFFER, 0);
      alDeleteSources(1, &source);
    }

    // uninitialize the music subsystem
    Music::quit();

    // uninitialize OpenAL
   
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    context = NULL;
    alcCloseDevice(device);
    device = NULL;
    
    alutExit();
    initialized = false;
  }
}

/**
 * Returns whether the audio (music and sound) system is initialized.
 * @return true if the audio (music and sound) system is initilialized
 */
bool Sound::is_initialized(void) {
  return initialized;
}

/**
 * Updates the audio (music and sound) system.
 * This function is called repeatedly by the game.
 */
void Sound::update(void) {
 
  // see whether a sound source has finished playing
  if (sources.size() > 0) {
    ALuint source = *sources.begin();
    ALint status;
    alGetSourcei(source, AL_SOURCE_STATE, &status);

    if (status != AL_PLAYING) {
      alSourcei(source, AL_BUFFER, 0);
      alDeleteSources(1, &source);
      sources.pop_front();
    }
  }

  // also update the music
  Music::update();
}

/**
 * Loads the file and plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;

  if (is_initialized()) {

    // create a source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // play the sound
    int error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "Cannot attach the buffer to the source to play sound: error " << error << std::endl;
      alDeleteSources(1, &source);
    }
    else {
      sources.push_back(source);
      alSourcePlay(source);
      error = alGetError();
      if (error != AL_NO_ERROR) {
	std::cerr << "Cannot play sound: error " << error << std::endl;
      }
      else {
	success = true;
      }
    }
  }
  return success;
}

