/**
 * This header should be included by each class header file.
 */

#ifndef ZSDX_COMMON_H
#define ZSDX_COMMON_H

#define _CRT_SECURE_NO_DEPRECATE // allow sprintf in Visual C++...

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "Types.h"

#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

/**
 * This macro should be used to exit the program properly on an error message.
 * The message parameter can contain several elements separated by the '<<' operator.
 */
#define DIE(message) do { std::ostringstream oss; oss << message; throw oss.str(); } while (0)

/**
 * Mathematic constants.
 * We don't use the constants from math.h
 * because they are not ANSI.
 */
extern const double PI;
extern const double TWO_PI;
extern const double PI_OVER_2;
extern const double THREE_PI_OVER_2;
extern const double SQRT_2;

#endif
