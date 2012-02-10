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
#include "ZSDX.h"
#include "lowlevel/System.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "StringResource.h"
#include "menus/TitleScreen.h"
#include "menus/SelectionMenuSelectFile.h"

/**
 * Global variable to get the ZSDX instance
 * from anywhere in the code.
 */
ZSDX *zsdx = NULL;

/**
 * Initializes the game engine.
 */
ZSDX::ZSDX(int argc, char **argv) {

  // initialize the lowlevel features (audio, video, files...)
  System::initialize(argc, argv);

  // create the surface where everything is drawn
  root_surface = new Surface(320, 240);

  // create the first screen
  current_screen = new TitleScreen();
  exiting = false;
  game = NULL;
}

/**
 * Cleans everything.
 */
ZSDX::~ZSDX(void) {
  delete root_surface;
  delete current_screen;
 }

/**
 * Sets the current game.
 * @param game the game
 */
void ZSDX::set_game(Game *game) {
  this->game = game;
}

/**
 * Returns whether the user just closed the window.
 * When this function returns true, you should stop immediately
 * whatever you are doing, free your memory and let the program quit.
 * @return true if the user wants to exit the program
 */
bool ZSDX::is_exiting(void) {
  return exiting;
}

/**
 * Sets whether the user wants to quit the program.
 */
void ZSDX::set_exiting(void) {
  exiting = true;
}

/**
 * The main function. The SDL loop is executed here.
 * The SDL events are forwarded to the current screen.
 * The current screen is redrawn when necessary.
 */
void ZSDX::main(void) {

  // SDL main loop
  SDL_Event event = {};
  uint32_t now;
  uint32_t next_frame_date = SDL_GetTicks();
  uint32_t frame_interval = 25; // time interval between to displays
  int delay;
  bool just_displayed = false; // to detect when the FPS number needs to be decreased

  while (!is_exiting()) {

    // handle the SDL events
    if (SDL_PollEvent(&event)) {
      handle_event(event);
    }

    // update the current screen
    update();

    // go to another screen?
    if (current_screen->is_screen_finished()) {
      Screen *next_screen = current_screen->get_next_screen();
      delete current_screen;

      if (next_screen != NULL) {
	current_screen = next_screen;
      }
      else {
	current_screen = new TitleScreen();
      }
    }
    else {

      now = System::now();
      delay = next_frame_date - now;
      // delay is the time remaining before the next display

      if (delay <= 0) { // it's time to display

	// see whether the FPS number is too high
	if (just_displayed && frame_interval <= 30) {
	  frame_interval += 5; // display the screen less often
	  //std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
	}

	next_frame_date = now + frame_interval;
	just_displayed = true;
	display();
      }
      else {
	just_displayed = false;

	// if we have time, let's sleep to avoid using all the processor
	System::sleep(1);

	if (delay >= 15) {
	  // if we have much time, increase the FPS number
	  frame_interval--;
	  //std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
	}
      }
    }
  }
}

/**
 * This function handles an SDL event.
 * It handles the events common to all screens:
 * closing the window and pressing escape or F5.
 * The handle_event() method of the current screen
 * is then called.
 */
void ZSDX::handle_event(const SDL_Event &event) {

  // handle the common events
  switch (event.type) {

    // quit if the user closes the window
  case SDL_QUIT:
    exiting = true;
    break;

    // a key is pressed
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {

      // F5: change the video mode
    case SDLK_F5:
      VideoManager::get_instance()->switch_video_mode();
      break;

    default:
      break;
    }
    break;
  }

  // handle the specific events depending on the current screen
  current_screen->handle_event(event);
}

/**
 * Updates the current screen.
 * This function is called repeatedly by the main loop.
 */
void ZSDX::update(void) {
  current_screen->update();
  System::update();
}

/**
 * Redraws the current screen.
 * This function is called repeatedly by the main loop.
 */
void ZSDX::display(void) {

  root_surface->fill_with_color(Color::get_black());
  current_screen->display(root_surface);
  VideoManager::get_instance()->display(root_surface);
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  zsdx = new ZSDX(argc, argv);
  zsdx->main();

  delete zsdx;

  return 0;
}

