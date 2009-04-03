#include <SDL/SDL_config_lib.h>
#include "Dungeon.h"
#include "FileTools.h"
using namespace std;

/**
 * Name of each dungeon, displayed in the map submenu.
 */
static const string dungeon_names[] = {
  // TODO: load this from some external file (for future translation)

  "", // 0 is not a valid dungeon number
  "Donjon de la Forêt",
  "Caverne de Roc",
  "Antre de\nMaître Arbror",
  "Palais de Beaumont",
  "Ancien\nChâteau d'Hyrule",
  "Caverne d'Inferno",
  "Temple de Glace",
  "Donjon des\nPics Rocheux",
  "Temple des Souvenirs",
};

/**
 * Creates the specified dungeon.
 * @param dungeon_number a dungeon number between 1 and 20
 */
Dungeon::Dungeon(int dungeon_number):
  dungeon_number(dungeon_number), chests(NULL), bosses(NULL) {

  this->dungeon_number = dungeon_number;
  load();
}

/**
 * Destructor.
 */
Dungeon::~Dungeon(void) {
  if (chests != NULL) {
    delete[] chests;
    delete[] bosses;
  }
}

/**
 * Returns this dungeon number.
 * @return the dungeon number, between 1 and 20
 */
int Dungeon::get_number(void) {
  return dungeon_number;
}

/**
 * Returns the name of the dungeon, in the current langage.
 * @return the dungeon name
 */
string Dungeon::get_name(void) {
  return dungeon_names[dungeon_number];
}

/**
 * Loads the dungeon properties from file dungeons.zsd.
 */
void Dungeon::load(void) {

  // open the file
  string file_name = FileTools::data_file_add_prefix("dungeons.zsd");

  CFG_File ini;
  if (CFG_OpenFile(file_name.c_str(), &ini) != CFG_OK) {
    DIE("Cannot load the dungeon file '" << file_name << "': " << CFG_GetError());
  }

  // parse the floors (the floors must be before the chests and the bosses)
  std::ostringstream floor_oss;
  floor_oss << "dungeon_" << dungeon_number << ".floor_";
  string floor_prefix = floor_oss.str();
  lowest_floor = 100;
  for (CFG_StartGroupIteration(CFG_SORT_ORIGINAL); !CFG_IsLastGroup(); CFG_SelectNextGroup()) {

    string group_name = CFG_GetSelectedGroupName();

    // parse the floors
    if (group_name.substr(0, floor_prefix.length()) == floor_prefix) {
      // we found a group describing a floor of to this dungeon

      string suffix = group_name.substr(floor_prefix.length());

      int floor;
      std::istringstream iss(suffix);
      iss >> floor;

      SDL_Rect size;
      size.w = CFG_ReadInt("width", 0);
      size.h = CFG_ReadInt("height", 0);
      floor_sizes.push_back(size);

      if (floor < lowest_floor) {
	lowest_floor = floor;
      }
    }
  }

  // now we now how many floors there are
  int nb_floors = get_nb_floors();
  chests = new std::vector<DungeonElement>[nb_floors];
  bosses = new std::vector<DungeonElement>[nb_floors];

  // parse the rest: chests and bosses
  string elements_prefix;
  std::ostringstream elements_oss;
  elements_oss << "dungeon_" << dungeon_number << ".map_";
  for (CFG_StartGroupIteration(CFG_SORT_ORIGINAL); !CFG_IsLastGroup(); CFG_SelectNextGroup()) {

    string group_name = CFG_GetSelectedGroupName();

    if (group_name.substr(0, elements_prefix.length()) == elements_prefix) {
      // we found a group describing an element in this dungeon

      string suffix = group_name.substr(elements_prefix.length());

      // is it a chest?
      if (suffix.find("chest") != string::npos) {
	DungeonElement chest;
	chest.floor = CFG_ReadInt("floor", 0);
	chest.x = CFG_ReadInt("x", 0);
	chest.y = CFG_ReadInt("y", 0);
	chest.savegame_variable = CFG_ReadInt("save", 0);
	chest.big = CFG_ReadBool("big", 0);

	chests[chest.floor - lowest_floor].push_back(chest);
      }

      // is it a boss or a miniboss?
      else if (suffix.find("boss") != string::npos) {
	DungeonElement boss;
	boss.floor = CFG_ReadInt("floor", 0);
	boss.x = CFG_ReadInt("x", 0);
	boss.y = CFG_ReadInt("y", 0);
	boss.savegame_variable = CFG_ReadInt("save", 0);
	boss.big = CFG_ReadBool("big", false);

	if (boss.big) {
	  boss_floor = boss.floor;
	}

	chests[boss.floor - lowest_floor].push_back(boss);
      }
    }
  }

  // close the file
  CFG_CloseFile(&ini);
}

/**
 * Returns the number of floors in this dungeon.
 * @return the number of floors
 */
int Dungeon::get_nb_floors(void) {
  return floor_sizes.size();
}

/**
 * Returns the lowest floor in this dungeon.
 * @return the lowest floor
 */
int Dungeon::get_lowest_floor(void) {
  return lowest_floor;
}

/**
 * Returns the highest floor in this dungeon.
 * @return the highest floor
 */
int Dungeon::get_highest_floor(void) {
  return get_lowest_floor() + get_nb_floors() - 1;
}

/**
 * Returns the size of the specified floor.
 * @param floor a floor of this dungeon
 * @return the size of this floor
 */
const SDL_Rect * Dungeon::get_floor_size(int floor) {
  return &floor_sizes[floor - get_lowest_floor()];
}

/**
 * Returns the number of floors that can be displayed for this dungeon.
 * No more that 7 floors can be displayed simultaneously.
 * This function is equivalent to MIN(7, get_nb_floors()).
 * @return the number of floors that can be displayed for this dungeon
 */
int Dungeon::get_nb_floors_displayed(void) {
  return MIN(7, get_nb_floors());
}

/**
 * Returns the highest floor that can be displayed when the specified
 * current floor has to be visible.
 */
int Dungeon::get_highest_floor_displayed(int current_floor) {

  int highest_floor = get_highest_floor();
  int highest_floor_displayed;

  // if there are less that 7 loors, they are all visible
  if (get_nb_floors() <= 7) {
    highest_floor_displayed = highest_floor;
  }

  // otherwise we only display 7 floors including the current one
  else if (current_floor >= highest_floor - 2) {
    highest_floor_displayed = highest_floor;	  
  }
  else if (current_floor <= lowest_floor + 2) {
    highest_floor_displayed = lowest_floor + 6;
  }
  else {
    highest_floor_displayed = current_floor + 3;
  }

  return highest_floor_displayed;
}

/**
 * Returns the floor where the boss of this dungeon is.
 * @return the floor of the boss
 */
int Dungeon::get_boss_floor(void) {
  return boss_floor;
}

/**
 * Returns the information about the bosses and the minibosses
 * of this dungeon that are on the specified floor.
 * @param floor a floor of this dungeon
 * @return the information about the bosses and minibosses of this floor
 */
const std::vector<Dungeon::DungeonElement> Dungeon::get_bosses(int floor) {
  return bosses[floor - get_lowest_floor()];
}

/**
 * Returns the information about the chests
 * of this dungeon that are on the specified floor.
 * @param floor a floor of this dungeon
 * @return the information about the chests of this floor
 */
const std::vector<Dungeon::DungeonElement> Dungeon::get_chests(int floor) {
  return chests[floor - get_lowest_floor()];
}
