This is a [BakkesMod](https://www.bakkesmod.com/) plugin that automatically skips the replay for you(simulates skip replay key press). 

Bind command `toggleskipreplay` through the SkipReplay plugin settings menu.
The `toggleskipreplay` keybind can be used to disable and enable autoskipping for that rare occasion that you do want to see the replay!

An option is available in the plugin settings menu to re-enable auto skipping after a match ends for when you forget to re-enable auto skipping.

**NOTE:** If you have installed this plugin before version 1.2 you will have an unused `SkipReplay.dll` and a `settings` folder in your bakkesmod root directory.

## Changelog 
### v1.4:
- Move settings menu to imgui (implement PluginSettingsWindow)
- "Press any key" and searchable combobox options to set `toggleskipmenu` keybind
### v1.3:
- Added option to re-enable auto skipping after match end
- Added custom toast logo
- Simulate skip replay key press when toggling `toggleskipreplay` on
### v1.2: 
- Added keybind option in the plugin settings menu
### v1.1:
- Changed EventHook to one that also triggers on offline matches