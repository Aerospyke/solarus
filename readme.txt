This file explains how to compile Zelda: Mystery Solarus DX.
Only the developing team has to compile the project. The users download directly a compiled binary for Windows, Linux or Mac OS.

---------------------
External dependencies
---------------------

The following libraries are required to compile and to execute:

SDL
SDLmain
SDL_image
SDL_ttf
lua5.1
physfs
openal
libsndfile

Note that two other libraries are directly included in the source code: SDL_Config
(an ini parsing library, with some modifications to avoid compilation warning and 64-bit issues)
and snes_spc (an SPC music decoding library).

The compilation process also relies on zip and luac (the Lua compiler) to manage the data files.


Linux developers:

Install the corresponding packages. For example, with Ubuntu or Debian:
libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev liblua5.1-0-dev libphysfs-dev libopenal-dev libsndfile-dev lua5.1 zip


Windows developers:

Download the binary version of each one of the development libraries listed above, on their official websites.
Install the header files (the .h files) in your compiler's include directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\include.
Similarly, install the library files (the .lib files) in your compiler's lib directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\lib.
Install the dynamic library files (the .dll files) in your system32 directory
(which may be C:\WINDOWS\system32).

The directory win32/libraries contain all static and dynamic libraries that are not provided as binaries
by their authors.
For SDL, SDL_image and SDL_ttf, the header files must be placed in an "SDL" subdirectory of the include directory.
For OpenAL, the header files must be placed in an "AL" subdirectory of the include directory.
You may need to additionally link with libz.


------------------------
Compilation instructions
------------------------

ZSDX uses CMake to manage the compilation process. CMake permits to compile
the project on any OS. On Unix, it generates the appropriate makefiles; on Windows,
it can generate a project for most of the IDEs, including Visual C++, Code::Blocks or MinGW.
You can also compile ZSDX without CMake, by creating a project from the sources with your IDE
and link with the libraries listed above.

Note that some Lua scripts need to be compiled to and that an archive of all data files has to be created.
Those tasks are performed the script data/make_zip, which is called by cmake when you compile the project.
To make this work you need sh, zip and luac (the Lua compiler).


Linux developers:

Go to the zsdx directory and just type:

cmake .

This generates the appropriate Makefiles for your system.
Then you can compile with:

$ make

and install with:

# make install

Of course you need make and a compiler like gcc.
This will install the zsdx binary in a default directory such as /usr/local/bin. So you must be root to run the last command.
Then, you can launch the game with the simple command:

$ zsdx

You can instead install the binary in another directory, so that no root access is necessary. You can specify this directory as a parameter of cmake like this:

$ cmake -D CMAKE_INSTALL_PREFIX=/home/your_directory .
$ make
$ make install

This will install the zsdx binary in /home/your_directory/bin. Then, you can launch the game with:

$ /home/your_directory/bin/zsdx

or just with:

$ zsdx

if /home/your_directory/bin is in your PATH. There is no need to be root.

Note that for now, the current directory must be the one containing the progam itselft, otherwise the data will not be found. 


By default, the game looks for the data files into the current directory. To change this, you can use the -datapath option, for example:

$ zsdx -datapath=/usr/share/games/zsdx


Windows developers:

I have managed to compile ZSDX with Code::Blocks, without using CMake.


Mac OS developers

I don't have Mac OS, so this readme is for now very incomplete in this section.

Notes:

- If SDL and SDL_image are installed with fink, an 'export CPPFLAGS="-I/sw/include -L/sw/lib"' is necessary before launching the configure script
- When linking, use the following option: -framework Carbon


------------------
Known sound issues
------------------

With linux, you may have sound issues, which I experienced under Ubuntu 9.04 (Jaunty) and was able to fix as explained below.
Edit the OpenAL configuration file (normally /etc/openal/alsoft.conf) and try the following settings:

- If the sounds play always with a delay (typically, a fraction of second), try reducing the "refresh" value to 2048 (the default value is 8192 with old versions of OpenAL).
- If the sound plays scratched in the first few seconds or longer, try forcing the driver to OSS, by setting the "driver" list value to "oss" (without the quotes).

Newer versions of OpenAL and PulseAudio may work better.

