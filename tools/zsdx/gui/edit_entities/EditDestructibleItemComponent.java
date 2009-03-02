package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.DestructibleItem.Subtype;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a destructible item.
 */
public class EditDestructibleItemComponent extends EditEntityComponent {

    // specific fields of a destructible item
    private EnumerationChooser<Subtype> subtypeField;
    private PickableItemSubtypeChooser pickableItemSubtypeField;
    private NumberChooser pickableItemSavegameVariableField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDestructibleItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// destructible item type
	subtypeField = new EnumerationChooser<Subtype>(Subtype.class);
	addField("Destructible item type", subtypeField);

	// pickable item type
	pickableItemSubtypeField = new PickableItemSubtypeChooser(true);
	addField("Pickable item", pickableItemSubtypeField);

	// savegame index
	pickableItemSavegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Pickable item savegame variable", pickableItemSavegameVariableField);

	// enable or disable the 'savegame index' field depending on the pickable item type
	pickableItemSubtypeField.addActionListener(new ActionListenerEnableSavegameVariable());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DestructibleItem destructibleItem = (DestructibleItem) entity;

	subtypeField.setValue(destructibleItem.getSubtype());
	pickableItemSubtypeField.setValue(destructibleItem.getPickableItemSubtype());
	pickableItemSavegameVariableField.setNumber(destructibleItem.getPickableItemSavegameVariable());
	new ActionListenerEnableSavegameVariable().actionPerformed(null);
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the fields are incorrectly filled
     */
    protected ActionEditEntity getAction() throws ZSDXException {

	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a destructible item
	DestructibleItem destructibleItem = (DestructibleItem) entity;

	Subtype subtype = subtypeField.getValue();
	PickableItem.Subtype pickableItemSubtype = pickableItemSubtypeField.getValue();
	int pickableItemSavegameVariable = pickableItemSavegameVariableField.getNumber();

	action.setSpecificAction(new ActionEditDestructibleItem(map, destructibleItem,
		subtype, pickableItemSubtype, pickableItemSavegameVariable));

	return action;
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame index' field depending on the type.
     */
    private class ActionListenerEnableSavegameVariable implements ActionListener {

	public void actionPerformed(ActionEvent ev) {

	    PickableItem.Subtype subtype = pickableItemSubtypeField.getValue();
	    pickableItemSavegameVariableField.setEnabled(subtype.isSaved());
	}
    }
}
