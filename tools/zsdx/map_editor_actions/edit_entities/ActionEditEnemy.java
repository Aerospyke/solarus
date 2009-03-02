package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.Enemy.*;

/**
 * Editing the properties specific to an enemy.
 */
public class ActionEditEnemy extends MapEditorAction {

    private Enemy enemy;

    private EntitySubtype subtypeBefore;
    private EntitySubtype subtypeAfter;

    private Rank rankBefore;
    private Rank rankAfter;

    private int savegameVariableBefore;
    private int savegameVariableAfter;

    private PickableItem.Subtype pickableItemSubtypeBefore;
    private PickableItem.Subtype pickableItemSubtypeAfter;

    private int pickableItemSavegameVariableBefore;
    private int pickableItemSavegameVariableAfter;

    /**
     * Constructor.
     * @param map the map
     * @param enemy the enemy to edit
     * @param subtype the subtype of enemy
     * @param rank rank of the enemy
     * @param savegameVariable the savegame variable where the enemy is saved
     * @param pickableItemSubtype the type of pickable item
     * @param pickableItemSavegameVariable the savegame variable where the pickable item is saved
     */
    public ActionEditEnemy(Map map, Enemy enemy, Subtype subtype, Rank rank,
	    int savegameVariable, PickableItem.Subtype pickableItemSubtype, int pickableItemSavegameVariable) {
	super(map);
	
	this.enemy = enemy;

	subtypeBefore = enemy.getSubtype();
	subtypeAfter = subtype;

	rankBefore = enemy.getRank();
	rankAfter = rank;

	savegameVariableBefore = enemy.getSavegameVariable();
	savegameVariableAfter = savegameVariable;

	pickableItemSubtypeBefore = enemy.getPickableItemSubtype();
	pickableItemSubtypeAfter = pickableItemSubtype;
	
	pickableItemSavegameVariableBefore = enemy.getPickableItemSavegameVariable();
	pickableItemSavegameVariableAfter = pickableItemSavegameVariable;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	enemy.setSubtype(subtypeAfter);
	enemy.setRank(rankAfter);
	enemy.setSavegameVariable(savegameVariableAfter);
	enemy.setPickableItem(pickableItemSubtypeAfter, pickableItemSavegameVariableAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	enemy.setSubtype(subtypeBefore);
	enemy.setRank(rankBefore);
	enemy.setSavegameVariable(savegameVariableBefore);
	enemy.setPickableItem(pickableItemSubtypeBefore, pickableItemSavegameVariableBefore);
    }
}
