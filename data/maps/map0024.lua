-------------------------
-- Dungeon 1 B1 script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    boss_start_battle("boss")
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "se_switch" or switch_name == "ne_switch" then
    move_camera(960, 496, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "se_room_timer", false)
end

function se_room_timer()
  open_se_door()
  start_timer(1000, "restore_camera", false)
end

function open_se_door()
  play_sound("secret")
  door_open("se_door")
  switch_set_enabled("se_switch", true)
  switch_set_enabled("ne_switch", true)
end

function event_obtained_treasure(content, savegame_variable)
  if content == 80 then
    boss_end_battle()
  end
end

function event_dungeon_ending_sequence()
  play_music("dungeon_finished.spc")
  npc_set_position("solarus_child", 648, 117)
  npc_set_animation("solarus_child", "stopped")
end

