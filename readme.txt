This file explains how to compile and install the project.


--------
Contents
--------

1  Overview
2  External dependencies
  2.1  Solarus
  2.2  Quests
3  Compilation instructions
  3.1  Default settings
  3.2  Change the install directory
  3.3  More about the datapath
  3.4  Windows developers
  3.5  Mac OS X developers


-----------
1  Overview
-----------

When it is installed, the project is divided in two parts:
- Solarus (the engine), which is an executable file written in C++ and called solarus.
- One (or several) quests, each quest corresponding to a zip archive called data.solarus and
  containing all the data.

When you execute the solarus binary, you have to specify the path of the quest to launch.
This can be done at runtime by using the -datapath option of solarus (more details below).

The engine source files are located in the src and include directories. The C++ code is under GPL.

The quests source files of are located in the quests directory. Each quest
has a specific directory and its data files are in a data subdirectory.
For example, for the zsdx quest, the data files are in a directory called
quests/zsdx/data. These data files represent all resources used by the quest, such as
sounds, music, images, sprites, maps and scripts. The scripts are written in Lua.
When building the quest, an zip archive called data.solarus is created with all data files.

Note that the engine itself relies on some of these resources (for instance a sprite for the hero).

For now, only one quest is available (zsdx), and this quest is not under GPL since it uses
copyrighted elements from Nintendo.
To compile and install the engine without any quest, remove the quests directory and
follow the instructions normally.


------------------------
2  External dependencies
------------------------

The build process uses cmake, both for the engine and the quests.
CMake allows to compile the project on any OS. On Unix, it generates the appropriate makefiles; on Windows,
it can generate a project for most environments, including Visual C++, Code::Blocks and MinGW.
You can also compile the engine without CMake (by creating a project from the sources with your IDE
and link with the libraries listed above) and build a quest without CMake
(by compiling the Lua scripts and building the archive yourself).


2.1  Solarus
------------

To compile Solarus (the engine), you need a C++ compiler.
The following libraries are required to compile and to execute Solarus:

SDL
SDLmain
SDL_image
SDL_ttf
lua5.1
physfs
openal
libsndfile


Note that two other libraries are directly embedded in the source code: SimpleIni,
an ini parsing library which consists in only two header files (no source files),
and snes_spc, an SPC music decoding library.


Linux developers:

Just install the corresponding packages. For example, with Ubuntu or Debian:

libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev liblua5.1-0-dev libphysfs-dev libopenal-dev libsndfile-dev lua5.1 zip


Windows developers:

Download the binary version of each one of the development libraries listed above, on their official websites.
Install the header files (the .h files) in your compiler's include directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\include.
Similarly, install the library files (the .lib files) in your compiler's lib directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\lib.
Install the dynamic library files (the .dll files) in your system32 directory
(which may be C:\WINDOWS\system32).

The directory libraries/win32 contain some required static and dynamic libraries that are not provided as binaries
by their authors.
For SDL, SDL_image and SDL_ttf, the header files must be placed in an "SDL" subdirectory of the include directory.
For OpenAL, the header files must be placed in an "AL" subdirectory of the include directory.


2.2  Quests
-----------

To build a Solarus quest, you will need zip and luac (the Lua compiler).


---------------------------
3  Compilation instructions
---------------------------


3.1  Default settings
----------------------

To compile solarus and the quests provided (only zsdx for now) with cmake, go to the solarus directory and just type:

cmake .

This generates the appropriate Makefiles for your system.
Then you can compile the engine and the quest with:

$ make

and install both of them with (as root):

# make install

This installs the following three files (assuming that the install directory is /usr/local):
- the solarus binary (the engine) in /usr/local/bin/
- the quest data archive in /usr/local/share/solarus/zsdx/
- a script called zsdx in /usr/local/bin/

The zsdx script launches the engine with the appropriate -datapath flag.
This means that you can launch the zsdx game with the command:

$ zsdx

which is equivalent to:

$ solarus -datapath=/usr/local/share/solarus/zsdx

If you want to compile the engine without quests (i.e. you have no quests directory),
cmake detects it and only builds and installs the solarus binary.


3.2  Change the install directory
---------------------------------

You may want to install the engine and the quests in another directory
(e.g. so that no root access is necessary). You can specify this directory
as a parameter of cmake:

$ cmake -D CMAKE_INSTALL_PREFIX=/home/your_directory .
$ make
$ make install

This installs the three files as described above except that the
/usr/local prefix is replaced by the one you specified.
The script generated launches the engine with the appropriate datapath flag.


3.3  More about the datapath
-----------------------------

There are several ways to make the engine find the quest archive data.solarus.
If the -datapath flag is set (see above), then the engine looks into the directory specified.
Otherwise, if the DATAPATH preprocessor constant is set, then this directory is used.
The DATAPATH constant can be set from your cmake command, by using:

$ cmake -D DATAPATH=/path/to/your/quest .

Finally, if the DATAPATH constant is not set either, then the engine looks into the current directory.

This DATAPATH constant may be used if you want the engine to launch a default quest 
when it is called without the -datapath flag.
You can still launch another quest by using the -datapath flag, which overwrites
the DATAPATH constant.


3.4  Windows developers
-----------------------

I have managed to compile Solarus with Code::Blocks, without using CMake.
It is necessary to additionally link with libz.


3.5  Mac OS X developers
---------------------

I don't have Mac OS X, so this readme is for now very incomplete in this section.
See the development blog for more information.


----------------
4.  Known issues
----------------

You may have sound issues, which I experienced under Ubuntu Linux 9.04 (Jaunty) and was able to fix as explained below.
Edit the OpenAL configuration file (normally /etc/openal/alsoft.conf) and try the following settings:

- If the sounds play always with a delay (typically, a fraction of second), try reducing the "refresh" value to 2048
  (the default value is 8192 with old versions of OpenAL).
- If the sound plays scratched in the first few seconds or longer, try forcing the driver to OSS,
  by setting the "driver" list value to "oss" (without the quotes).

Newer versions of OpenAL and PulseAudio may work better.

