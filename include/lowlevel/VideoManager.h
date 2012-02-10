/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_VIDEO_MANAGER_H
#define SOLARUS_VIDEO_MANAGER_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
 
/**
 * @brief Displays the window and handles the video mode.
 *
 * TODO: don't hardcode the screen size 320*240
 */
class VideoManager {

 public:

  /**
   * @brief The different possible video modes.
   */
  enum VideoMode {
    WINDOWED_STRETCHED,       /**< the 320*240 game surface is stretched into a 640*480 window (default) */
    WINDOWED_SCALE2X,         /**< the 320*240 game surface is scaled into a 640*480 window with the Scale2x algorithm */
    WINDOWED_NORMAL,          /**< the 320*240 game surface is directly displayed on a 320*240 window */
    FULLSCREEN_NORMAL,        /**< the 320*240 game surface is directly displayed on a 320*240 screen */
    FULLSCREEN_WIDE,          /**< the 320*240 game surface is stretched into a 640*480 surface
                               * and then displayed on a widescreen resolution if possible (768*480 or 720*480)
                               * with two black side bars */
    FULLSCREEN_SCALE2X,       /**< the 320*240 game surface is scaled into a 640*480 screen with the Scale2x algorithm */
    FULLSCREEN_SCALE2X_WIDE,  /**< the 320*240 game surface is scaled into a 640*480 surface with the Scale2x algorithm
                               * and then displayed on a widescreen resolution if possible (768*480 or 720*480)
                               * with two black side bars */
    FULLSCREEN_CENTERED,      /**< the 320*240 game surface is displayed as a box inside a 640*480 screen */
    FULLSCREEN_CENTERED_WIDE, /**< the 320*240 game surface is displayed as a box inside a widescreen resolution (768*480 or 720*480) */
    NB_MODES                  /**< number of existing video modes */
  };

 private:

  static VideoManager* instance;                    /**< the only instance */
  static Rectangle default_mode_sizes[NB_MODES];    /**< default size of the surface for each video mode */
  static const int surface_flags;                   /**< SDL flags for surfaces */

  bool disable_window;                              /**< indicates that no window is displayed (used for unitary tests) */
  Rectangle mode_sizes[NB_MODES];                   /**< verified size of the surface for each video mode */
  Rectangle dst_position_wide;                      /**< position of the 640*480 surface on the 768*480 or 720*480
                                                     * video surface */

  VideoMode video_mode;                             /**< current video mode of the screen */
  Surface* screen_surface;                          /**< the screen surface */

  Rectangle dst_position_centered;                  /**< position of the 320*240 game surface on the screen surface */
  int width;                                        /**< width of the current screen surface */
  int offset;                                       /**< width of a side bar when using a widescreen resolution */
  int end_row_increment;                            /**< increment used by the stretching and scaling functions
                                                     * when changing the row */

  VideoManager(bool disable_window);
  ~VideoManager();

  void blit(Surface* src_surface, Surface* dst_surface);
  void blit_stretched(Surface* src_surface, Surface* dst_surface);
  void blit_scale2x(Surface* src_surface, Surface* dst_surface);
  void blit_centered(Surface* src_surface, Surface* dst_surface);

 public:

  static void initialize(int argc, char** argv);
  static void quit();
  static VideoManager* get_instance();

  void switch_video_mode();
  void set_initial_video_mode();
  void set_default_video_mode();
  bool is_mode_supported(VideoMode mode);
  void set_video_mode(VideoMode mode);
  VideoMode get_video_mode();

  bool is_fullscreen(VideoMode mode);
  bool is_fullscreen();
  void switch_fullscreen();

  void display(Surface* src_surface);
};

#endif

