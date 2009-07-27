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
#include "FileTools.h"

/**
 * @brief Converts the parameter as a <code>char*</code> constant string.
 * @param x the text to convert
 */
#define STRING(x) STRING2(x)
#define STRING2(x) #x

/*
 * If we are not under Windows, the data files will be picked
 * in the data directory (this is the normal behavior on Unix environments when the
 * user compiles the program). Note that the data directory is set at the
 * compilation time and hardcoded in the binary.
 * Otherwise, the data directory is relative to the current directory (which should
 * be the location of the executable file). This is the normal behavior on Windows
 * platforms, because the user downloads a binary version of the game.
 */
#ifdef DATADIR_ 
#define DATADIR ((std::string) STRING(DATADIR_))
#else
#define DATADIR ((std::string) "./data")
#endif

/**
 * @brief Returns the directory of the data files.
 * @return the directory of the data files
 */
const std::string FileTools::data_file_get_prefix(void) {

  return DATADIR;
}

/**
 * @brief Returns the path of a data file.
 *
 * The \c file_name parameter is relative to the ZSDX data directory
 * (which could be for example /usr/local/share/zsdx/data or
 * C:\\Program Files\\zsdx\\data).
 * This function returns the same file name, prefixed by the data directory. 
 *
 * @param file_name name of a data file
 */
const std::string FileTools::data_file_add_prefix(const std::string &file_name) {

  const std::string &prefixed_file_name = DATADIR + "/" + file_name;
  return prefixed_file_name;
}

/**
 * @brief Opens in reading a file in the ZSDX data directory.
 * 
 * The file name is relative to the ZSDX data directory (which could be
 * for example /usr/local/share/zsdx/data or C:\\Program Files\\zsdx\\data).
 *
 * @param file_name name of the file to open
 * @return the file, or NULL if it couldn't be open.
 */
FILE *FileTools::open_data_file(const std::string &file_name) {

  const std::string &full_file_name = data_file_add_prefix(file_name);

  FILE *f = fopen(full_file_name.c_str(), "r");

  return f;
}

/**
 * @brief Loads an image file.
 * 
 * The file name is relative to the ZSDX sprites directory (which may be
 * for example /usr/local/share/zsdx/data/sprites or C:\\Program Files\\zsdx\\data\\sprites).
 * The program is stopped with an error message if the image cannot be loaded.
 *
 * @param file_name name of the image file to open
 * @return the file
 */
SDL_Surface *FileTools::open_image(const std::string &file_name) {

  const std::string &full_file_name = DATADIR + "/sprites/" + file_name;

  SDL_Surface *image = IMG_Load(full_file_name.c_str());

  if (image == NULL) {
    DIE("Cannot load image '" << full_file_name << "'");
  }

  return image;
}
