/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_TEXT_SURFACE_H
#define SOLARUS_TEXT_SURFACE_H

#include "Common.h"
#include "DynamicDisplayable.h"
#include "lowlevel/Color.h"
#include "lowlevel/Rectangle.h"
#include <SDL_ttf.h>
#include <map>

/**
 * @brief Displays a line of text on a surface.
 *
 * This class encapsulates a graphic surface and handles text rendering,
 * horizontal and vertical text alignment, color and other properties.
 * This is the only class related to the font library (SDL_ttf).
 */
class TextSurface: public DynamicDisplayable {

  public:

    /**
     * Horizontal alignment of the text.
     */
    enum HorizontalAlignment {
      ALIGN_LEFT,                                     /**< the text is aligned to the left */
      ALIGN_CENTER,                                   /**< the text is centered */
      ALIGN_RIGHT                                     /**< the text is aligned to the right */
    };

    /**
     * Vertical alignment of the text.
     *
     */
    enum VerticalAlignment {
      ALIGN_TOP,                                      /**< the text is aligned to the top */
      ALIGN_MIDDLE,                                   /**< the text is aligned in the middle */
      ALIGN_BOTTOM                                    /**< the text is aligned to the bottom */
    };

    /**
     * Rendering mode of the text.
     */
    enum RenderingMode {
      TEXT_SOLID,                                     /**< the text is displayed without smooth effect */
      TEXT_SHADED,                                    /**< the text is displayed with a smooth effect for an unicolor background */
      TEXT_BLENDED                                    /**< the text is displayed with a smooth effect for any background */
    };

  private:

    /**
     * This structures stores the data of a font.
     */
    struct FontData {
      std::string file_name;                          /**< name of the font file, relative to the data directory */
      int font_size;                                  /**< size of the characters */
      char *buffer;                                   /**< the file loaded into memory */
      SDL_RWops *rw;                                  /**< read/write object used to open the font file from memory */
      TTF_Font *internal_font;                        /**< the library-dependent font object */
    };

    static std::map<std::string, FontData> fonts;     /**< the data of each font, loaded from the file text/fonts.dat
                                                       * (fond id -> font data) */
    static std::string default_font_id;               /**< id of the default font to use */

    std::string font_id;                              /**< id of the font of the current text surface */
    HorizontalAlignment horizontal_alignment;         /**< horizontal alignment of the current text surface */
    VerticalAlignment vertical_alignment;             /**< vertical alignment of the current text surface */
    RenderingMode rendering_mode;                     /**< rendering mode of the current text surface */
    Color text_color;                                 /**< color of the text */
    Color background_color;                           /**< color of the background (only for the TEXT_SHADED rendering mode) */

    int x;                                            /**< x coordinate of where the text is aligned */
    int y;                                            /**< y coordinate of where the text is aligned */
    Surface *surface;                                 /**< the surface to draw */
    Rectangle text_position;                          /**< position of the top-left corner of the surface on the screen */

    std::string text;                                 /**< the string to display (only one line) */

    void rebuild();

  public:

    static void initialize();
    static void quit();

    TextSurface(int x, int y);
    TextSurface(int x, int y,
        HorizontalAlignment horizontal_alignment,
        VerticalAlignment vertical_alignment);
    ~TextSurface();

    void set_font(const std::string font_id);
    void set_horizontal_alignment(HorizontalAlignment horizontal_alignment);
    void set_vertical_alignment(VerticalAlignment vertical_alignment);
    void set_alignment(HorizontalAlignment horizontal_alignment,
        VerticalAlignment vertical_alignment);
    void set_rendering_mode(RenderingMode rendering_mode);
    void set_text_color(Color& color);
    void set_text_color(int r, int g, int b);
    void set_background_color(Color& color);
    void set_position(int x, int y);
    void set_x(int x);
    void set_y(int y);
    void set_text(const std::string& text);
    const std::string& get_text();
    bool is_empty();
    void add_char(char c);

    int get_width();
    int get_height();
    const Rectangle get_size();

    void raw_display(Surface& dst_surface, const Rectangle& dst_position);
    Surface* get_surface();
};

#endif

