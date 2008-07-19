package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a pickable item.
 */
public class EditPickableItemComponent extends EditEntityComponent {
    
    // specific fields of a pickable item
    private PickableItemTypeChooser typeField;
    private NumberChooser uniqueIdField; // enabled only for certain types of pickable items

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditPickableItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// pickable item type
	typeField = new PickableItemTypeChooser(false);
	addField("Pickable item type", typeField);

	// unique id
	uniqueIdField = new NumberChooser();
	addField("Unique id", uniqueIdField);

	// enable or disable the 'unique id' field depending on the pickable item type
	typeField.addActionListener(new ActionListenerEnableUniqueId());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	PickableItem pickableItem = (PickableItem) entity;
	
	typeField.setPickableItemType(pickableItem.getPickableItemType());
	uniqueIdField.setNumber(pickableItem.getUniqueId());
	new ActionListenerEnableUniqueId().actionPerformed(null);
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

	// add the properties specific to a pickable item
	PickableItem pickableItem = (PickableItem) entity;

	int type = typeField.getPickableItemType();
	int uniqueId = uniqueIdField.getNumber();

	action.setSpecificAction(new ActionEditPickableItem(map, pickableItem, type, uniqueId));

	return action;
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'unique id' field depending on the type.
     */
    private class ActionListenerEnableUniqueId implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    int type = typeField.getPickableItemType();
	    if (type <= PickableItem.PICKABLE_ITEM_ARROW_10) {
		uniqueIdField.setEnabled(false);
		uniqueIdField.setNumber(0);
	    }
	    else {
		uniqueIdField.setEnabled(true);
	    }
	}
    }
}
