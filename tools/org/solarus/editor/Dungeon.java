/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 * 
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
package org.solarus.editor;

import org.ini4j.Ini;
import java.awt.*;
import java.util.*;
import java.io.*;
import org.solarus.editor.entities.*;

/**
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * These properties are loaded and saved from file dungeons.dat.
 *
 * TODO: split dungeons.dat in one file per dungeon to simplify
 */
public class Dungeon {

    private int dungeonNumber;

    private int lowestFloor;
    private Vector<Dimension> floorSizes;

    private int nbChestsSaved;
    private int nbBossesSaved;

    /**
     * Creates a dungeon and loads it from file dungeons.dat.
     * @param dungeon_number the number of the dungeon to load, between 1 and 20
     */
    public Dungeon(int dungeon_number) {
	this.dungeonNumber = dungeon_number;
	load();
    }

    /**
     * Returns the ini file name.
     * @return the ini file name
     */
    private static String getFileName() {
	return Project.getDataPath() + "/maps/dungeons/dungeons.dat";
    }

    /**
     * Loads the data of this dungeon from file dungeons.dat.
     */
    private void load() {

	String fileName = getFileName();
	try {
	    lowestFloor = 100;
	    floorSizes = new Vector<Dimension>();

	    Ini ini = new Ini(new FileReader(fileName));

	    // parse the floors (the floors must be before the chests and the bosses)
	    String floorPrefix = "dungeon_" + dungeonNumber + ".floor_";
	    for (Ini.Section section: ini.values()) {
		String sectionName = section.getName();
		if (sectionName.startsWith(floorPrefix)) {

		    String suffix = sectionName.substring(floorPrefix.length());
		    int floor = Integer.parseInt(suffix);
		    int width = Integer.parseInt(section.get("width"));
		    int height = Integer.parseInt(section.get("height"));
		    floorSizes.add(new Dimension(width, height));

		    if (floor < lowestFloor) {
			lowestFloor = floor;
		    }
		}
	    }
	}
	catch (IOException ex) {
	    System.err.println("Cannot load the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
	catch (IllegalArgumentException ex) {
	    System.err.println("Cannot load the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Saves the data of a map of this dungeon in the file dungeons.dat.
     * This function is called when a map has just been saved. It updates the dungeons.dat file
     * with the new information of that map.
     * @param map a map of this dungeon
     */
    public static void saveMapInfo(Map map) {

	String fileName = getFileName();
	try {
	    Ini ini = new Ini(new FileReader(fileName));

	    if (map.isInDungeon()) {
		Dungeon dungeon = map.getDungeon();
		dungeon.saveDungeonElements(ini, map);
	    }
	    else {
		removeDungeonElements(ini, map);
	    }
	    ini.store(new FileWriter(fileName));
	}
	catch (IOException ex) {
	    System.err.println("Cannot save the map information in the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns all sections of dungeon.dat referring to the specified map.
     * @param ini the ini file
     * @param map a map
     */
    private static Vector<String> getMapSections(Ini ini, Map map) {

	Vector<String> sections = new Vector<String>();

	String mapString = "map_" + map.getId();
	for (Ini.Section section: ini.values()) {
	    String sectionName = section.getName();
	    if (sectionName.contains(mapString)) {
	        sections.add(sectionName);
	    }
	}
	return sections;
    }

    /**
     * Removes from file dungeons.dat any element (chests, or bosses)
     * referencing the specified map.
     * @param ini the ini file
     * @param map a map
     */
    private static void removeDungeonElements(Ini ini, Map map) throws IOException {

        Vector<String> sectionsToRemove = getMapSections(ini, map);

	for (String sectionName: sectionsToRemove) {
	    ini.remove(sectionName);
	}
    }

    /**
     * Saves in file dungeons.dat the dungeon elements (chests and bosses)
     * of the specified map, and removes elements that are not on the map
     * anymore.
     * 
     * @param ini the ini file
     * @param map a map
     */
    private void saveDungeonElements(Ini ini, Map map) throws IOException {

        Vector<String> currentSections = getMapSections(ini, map);
        Vector<String> updatedSections = new Vector<String>();

	nbChestsSaved = 0;
	nbBossesSaved = 0;

	// add/update the existing elements
	MapEntities[] allEntities = map.getAllEntities();
	for (Layer layer: Layer.values()) {
	    for (MapEntity entity: allEntities[layer.getId()].getDynamicEntities()) {

		if (entity instanceof Chest) {
		    Chest chest = (Chest) entity;
		    String section = saveDungeonElement(ini, map, "chest_" + nbChestsSaved,
			    chest.getX(), chest.getY(),
			    chest.getIntegerProperty("treasureSavegameVariable"), chest.isBigChest());
		    updatedSections.add(section);
		    nbChestsSaved++;
		}
		else if (entity instanceof Enemy) {
		    Enemy enemy = (Enemy) entity;
		    Enemy.Rank rank = Enemy.Rank.get(enemy.getIntegerProperty("rank"));
		    
		    if (rank != Enemy.Rank.NORMAL) {
			String section = saveDungeonElement(ini, map, "boss_" + nbBossesSaved,
				enemy.getX(), enemy.getY(), enemy.getIntegerProperty("savegameVariable"),
				rank == Enemy.Rank.BOSS); 
			updatedSections.add(section);
			nbBossesSaved++;
		    }
		}
	    }
	}

	// remove non-existing elements
	for (String section: currentSections) {
	    if (!updatedSections.contains(section)) {
		ini.remove(section);
	    }
	}
    }

    /**
     * Saves into the dungeons.dat file an element of a map (chest, miniboss or boss).
     * @param ini the ini file
     * @param map the map
     * @param name name of this element (ex: "chest_4", "miniboss_1" or "boss")
     * @param x x position of the element, relative to the map
     * @param y y position of the element, relative to the map
     * @param big is this a big chest/boss
     * @param save index of the boolean that stores this element's state
     * @return name of the section created in the ini file
     */
    private String saveDungeonElement(Ini ini, Map map, String name, int x, int y, int save, boolean big) {

        Point mapLocation = map.getLocation();
	x += mapLocation.x;
	y += mapLocation.y;

	StringBuffer sectionName = new StringBuffer();
	sectionName.append("dungeon_");
	sectionName.append(dungeonNumber);
	sectionName.append('.');
	sectionName.append("map_");
	sectionName.append(map.getId());
	sectionName.append('.');
	sectionName.append(name);
	
	Ini.Section section = ini.new Section(sectionName.toString());
	section.put("floor", Integer.toString(map.getFloor()));
	section.put("x", Integer.toString(x));
	section.put("y", Integer.toString(y));
	section.put("save", Integer.toString(save));
	section.put("big", big ? "1" : "0");

	ini.put(sectionName.toString(), section);

	return sectionName.toString();
    }

    // floors

    /**
     * Returns the number of floors in this dungeon.
     */
    public int getNbFloors() {
	return floorSizes.size();
    }

    /**
     * Returns the lowest floor in this dungeon.
     * @return the lowest floor, between -16 and 15
     */
    public int getLowestFloor() {
	return lowestFloor;
    }

    /**
     * Returns the highest floor in this dungeon.
     * @return the highest floor, between -16 and 15
     */
    public int getHighestFloor() {
	return lowestFloor + getNbFloors() - 1;
    }

    /**
     * Returns a default floor, to propose initially to the user when he
     * associates a map to this dungeon.
     * @return a default floor
     */
    public int getDefaultFloor() {

	int result;

	if (hasFloor(0)) {
	    result = 0;
	}

	else if (getLowestFloor() > 0) {
	    result = getLowestFloor();
	}

	else {
	    result = getHighestFloor();
	}

	return result;
    }

    /**
     * Returns whether a floor exists in this dungeon
     * @param floor a floor (between -16 and 15)
     * @return true if this floor exists in this dungeon
     */
    public boolean hasFloor(int floor) {
	return floor >= getLowestFloor() && floor <= getHighestFloor();
    }
}
