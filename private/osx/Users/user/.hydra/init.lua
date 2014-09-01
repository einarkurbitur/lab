-- Hi!
-- Save this as ~/.hydra/init.lua and choose Reload Config from the menu (or press cmd-alt-ctrl R}

-- show an alert to let you know Hydra's running
hydra.alert("Hydra config by justmao945 loaded", 1.5)

-- open a repl with mash-R; requires https://github.com/sdegutis/hydra-cli
hotkey.bind({"cmd", "ctrl", "alt"}, "R", repl.open)

-- show a helpful menu
hydra.menu.show(function()
    local t = {
      {title = "Reload Config", fn = hydra.reload},
      {title = "Open REPL", fn = repl.open},
      {title = "-"},
      {title = "About Hydra", fn = hydra.showabout},
      {title = "Check for Updates...", fn = function() hydra.updates.check(nil, true) end},
      {title = "Quit", fn = os.exit},
    }

    if not hydra.license.haslicense() then
      table.insert(t, 1, {title = "Buy or Enter License...", fn = hydra.license.enter})
      table.insert(t, 2, {title = "-"})
    end

    return t
end)

-- uncomment this line if you want Hydra to make sure it launches at login
hydra.autolaunch.set(true)

-- when the "update is available" notification is clicked, open the website
notify.register("showupdate", function() os.execute('open https://github.com/sdegutis/Hydra/releases') end)

-- check for updates every week, and also right now (when first launching)
timer.new(timer.weeks(1), hydra.updates.check):start()
hydra.updates.check()


-- main modkey
local modkey = {"cmd", "ctrl"}

-- move the window
function movewindow(fn)
  local win = window.focusedwindow()
  if win == nil then
    return
  end
  local newframe = win:screen():frame_without_dock_or_menu()
  fn(newframe);
  win:setframe(newframe)
end

-- move the window to the right half of the screen
function movewindow_righthalf()
  movewindow(function(newframe)
    newframe.w = newframe.w / 2
    newframe.x = newframe.w -- comment this line to push it to left half of screen
  end)
end

-- move the window to the left half of the screen
function movewindow_lefthalf()
  movewindow(function(newframe)
    newframe.w = newframe.w / 2
    newframe.x = 0
  end)
end

hotkey.new(modkey, "H", movewindow_lefthalf):enable()
hotkey.new(modkey, "L", movewindow_righthalf):enable()


-- Maximize the window
hotkey.new(modkey, "M", function()
  local win = window.focusedwindow()
  if win ~= nil then
    win:maximize()
  end
end):enable()
