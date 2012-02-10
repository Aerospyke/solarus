/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "lowlevel/IniFile.h"
#include "lowlevel/FileTools.h"
#include "simpleini/SimpleIni.h"

/**
 * @brief Encapsulates the objects managed by the SimpleIni library.
 */
struct SimpleIni {
  CSimpleIniA data;                             /**< the library-dependent ini file object encapsulated */
  CSimpleIniA::TNamesDepend groups;             /**< the groups currently traversed by a group iteration */
  CSimpleIniA::TNamesDepend::iterator iterator; /**< the iteration */
}

/**
 * @brief Creates an object to read or write an ini file.
 *
 * Opens the specified file.
 *
 * @param file_name name of the file to write, relative to the data location
 * @param mode mode to open the file
 */
IniFile::IniFile(const std::string &file_name, Mode mode):
  file_name(file_name), mode(mode) {

  ini = new SimpleIni();

  ini->data.SetUnicode();
  if (mode != WRITE || FileTools::data_file_exists(file_name)) {
    // read the ini file
    char *buffer;
    size_t size;
    FileTools::data_file_open_buffer(file_name, &buffer, &size, (mode == READ_LANGUAGE));

    if (ini->data.Load(buffer, size) != SI_OK) {
      DIE("Cannot load the ini file '" << file_name << "'");
    }

    FileTools::data_file_close_buffer(buffer);
  }
}

/**
 * @brief Destroys the object.
 */
IniFile::~IniFile(void) {
  delete ini;
}

/**
 * @brief Saves the ini data into the file specified with the constructor.
 */
void IniFile::save(void) {

  if (mode != WRITE) {
    DIE("Cannot save ini file: the mode should be WRITE");
  }

  // save the data into a buffer
  std::string s;
  ini->data.Save(s);
  FileTools::data_file_save_buffer(file_name, s.c_str(), s.size());
}

/**
 * @brief Returns whether the specified group exists.
 * @param group name of the group to check
 * @return true if this group exists
 */
bool IniFile::has_group(const std::string &group) {
  return ini->data.GetSection(group.c_str()) != NULL;
}

/**
 * @brief Sets the current group, that will be used for the next operations.
 *
 * If the file mode is WRITE and the group does not exist, it is created.
 * If the file mode is READ and the group does not exist, a fatal error is raised.
 *
 * @param group name of the group to set (an empty name means the global group)
 */
void IniFile::set_group(const std::string &group) {

  if (mode != WRITE && !has_group(group)) {
    DIE("Cannot select group '" << group << "' in ini file: no such group");
  }
  this->group = group;

  // debug
  /*
  std::cout << "keys in group '" << group << "':\n";

  CSimpleIniA::TNamesDepend keys;
  ini.GetAllKeys(group.c_str(), keys);
  CSimpleIniA::TNamesDepend::iterator it = keys.begin();
  for (it = keys.begin(); it != keys.end(); it++) {
    std::cout << "key " << (*it).pItem << "\n";
  }
  */
}

/**
 * @brief Returns the name of the current group.
 *
 * This name was typically set by set_group() or next_group().
 *
 * @return name of the current group (an empty name means the global group)
 */
const std::string & IniFile::get_group(void) {

  return group;
}

/**
 * @brief Returns the integer value corresponding to the specified key in the current group.
 * @param key the key
 * @param default_value a default value to return if the key does not exist
 * @return the value of this key
 */
int IniFile::get_integer_value(const std::string &key, int default_value) {

  long value = ini->data.GetLongValue(group.c_str(), key.c_str(), default_value);
  return (int) value;
}

/**
 * @brief Returns the boolean value corresponding to the specified key in the current group.
 * @param key the key
 * @param default_value a default value to return if the key does not exist
 * @return the value of this key
 */
bool IniFile::get_boolean_value(const std::string &key, bool default_value) {

  return ini->data.GetBoolValue(group.c_str(), key.c_str(), default_value);
}

/**
 * @brief Returns the string value corresponding to the specified key in the current group.
 * @param key the key
 * @param default_value a default value to return if the key does not exist
 * @return the value of this key
 */
const std::string IniFile::get_string_value(const std::string &key, const std::string &default_value) {

  std::string value = ini->data.GetValue(group.c_str(), key.c_str(), default_value.c_str());
  return value;
}

/**
 * @brief Returns the string value corresponding to the specified key in the current group.
 *
 * If the string is not defined, the application stops on an error message.
 *
 * @param key the key
 * @return the value of this key
 */
const std::string IniFile::get_string_value(const std::string &key) {

  std::string value = get_string_value(key, "");
  if (value.size() == 0) {
    DIE("No value for key '" << key << "' in file '" << file_name << "'");
  }
  return value;
}


/**
 * @brief Sets an integer value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_integer_value(const std::string &key, int value) {

  ini->data.SetLongValue(group.c_str(), key.c_str(), value);
}

/**
 * @brief Sets a boolean value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_boolean_value(const std::string &key, bool value) {

  ini->data.SetBoolValue(group.c_str(), key.c_str(), value);
}

/**
 * @brief Sets a string value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_string_value(const std::string &key, const std::string &value) {

  ini->data.SetValue(group.c_str(), key.c_str(), value.c_str());
}

/**
 * @brief Starts an iteration over the groups of this ini file.
 *
 * While has_more_groups() returns true, call next_group() to get the next element of your iteration.
 */
void IniFile::start_group_iteration(void) {

  ini->data.GetAllSections(ini->groups);
  ini->iterator = ini->groups.begin();
}

/**
 * @brief During a group iteration, returns whether there are at least one group remaining.
 * @return true if the group iteration can continue (i.e. you can call next_group())
 */
bool IniFile::has_more_groups(void) {

  if (ini->iterator != ini->groups.end()) {
    CSimpleIniA::Entry entry = *ini->iterator;
    set_group(entry.pItem);
    return true;
  }

  return false;
}

/**
 * @brief Selects the next group during a group iteration.
 *
 * This function should be called only when has_more_groups() returns true.
 * You don't have to call set_group() to select this group, it is already done by this function.
 * To know the group name, call get_group().
 */
void IniFile::next_group(void) {
  ini->iterator++;
}

