#include "HUD.h"
#include "Game.h"
#include "Savegame.h"
#include "HeartsView.h"
#include "RupeesView.h"
#include "MagicBar.h"
#include "ActionIcon.h"
#include "SwordIcon.h"
#include "PauseIcon.h"
#include "ItemIcon.h"

/**
 * Constructor.
 */
HUD::HUD(Game *game) {

  Savegame *savegame = game->get_savegame();
  Equipment *equipment = savegame->get_equipment();
  KeysEffect *keys_effect = game->get_keys_effect();

  nb_elements = 0;

  elements[nb_elements++] = new HeartsView(equipment, 216, 14);
  elements[nb_elements++] = new RupeesView(equipment, 8, 216);
  elements[nb_elements++] = new MagicBar(equipment, 216, 35);
  elements[nb_elements++] = new ItemIcon(0, savegame, 8, 29);
  elements[nb_elements++] = new ItemIcon(1, savegame, 60, 29);
  elements[nb_elements++] = new SwordIcon(keys_effect, equipment, 10, 29);
  elements[nb_elements++] = new PauseIcon(keys_effect, -3, 7);
  elements[nb_elements++] = new ActionIcon(keys_effect, 23, 51);
}

/**
 * Destructor.
 */
HUD::~HUD(void) {

  for (int i = 0; i < nb_elements; i++) {
    delete elements[i];
  }
}

/**
 * Updates the information displayed by HUD.
 */
void HUD::update(void) {

  for (int i = 0; i < nb_elements; i++) {
    elements[i]->update();
  }
}

/**
 * Displays the HUD.
 * @param destination the destination surface
 */
void HUD::display(SDL_Surface *destination) {

  for (int i = 0; i < nb_elements; i++) {
    elements[i]->display(destination);
  }
}
