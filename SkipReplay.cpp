#include "skipReplay.h"
#include "bakkesmod/wrappers/includes.h"

BAKKESMOD_PLUGIN(SkipReplay, "Skip Replay", "1.3", PLUGINTYPE_REPLAY)

void SkipReplay::onLoad()
{
	cvarManager->registerNotifier("toggleskipreplay", [this](std::vector<std::string> commands) {
		gameWrapper->Toast("SkipReplay", (enabled = !enabled) ? "Auto Skipping is now enabled!" : "Auto Skipping is now disabled!", "default", 2.0F);
		}, "Bind to Toggle Replay Skip", PERMISSION_ALL);

	// Hook readUp() to event that only triggers once at the start of replay
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay", std::bind(&SkipReplay::readyUp, this));

	cvarManager->registerCvar("skipKeybind", "None", "Toggle Skip Replays Keybind", false);
	cvarManager->getCvar("skipKeybind").addOnValueChanged(std::bind(&SkipReplay::onKeybindValueChanged, this, std::placeholders::_1, std::placeholders::_2));

	CVarWrapper skipEnableCheckbox = cvarManager->registerCvar("skipEnableCheckbox", "None", "Re-enable Skip Replays at Match End", false);
	skipEnable = std::make_shared<bool>(false);
	skipEnableCheckbox.bindTo(skipEnable);
	cvarManager->getCvar("skipEnableCheckbox").addOnValueChanged(std::bind(&SkipReplay::onSkipEnableChanged, this));
}

void SkipReplay::onUnload()
{
}

void SkipReplay::readyUp() 
{
	if (enabled)
		gameWrapper->ExecuteUnrealCommand("ReadyUp");
}

void SkipReplay::onMatchEndEnable()
{
	if (!enabled) {
		enabled = true;
		gameWrapper->Toast("SkipReplay", "Auto Skipping is re-enabled!", "default", 5.0F);
	}
}

void SkipReplay::onSkipEnableChanged()

{
	if (*skipEnable)
		gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&SkipReplay::onMatchEndEnable, this));
	else
		gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
}

void SkipReplay::onKeybindValueChanged(std::string oldValue, CVarWrapper cvar)
{
	static bool loaded = false;
	std::string toastMsg, newValue = cvar.getStringValue();
	
	if (loaded) {
		if (!oldValue.empty() && !newValue.empty() && oldValue != newValue) {
			if (newValue != "None") {
				cvarManager->executeCommand("bind " + newValue + " toggleskipreplay", false);
				toastMsg = "toggleskipreplay bound to " + newValue;
			}

			if (oldValue != "None") {
				cvarManager->executeCommand("unbind " + oldValue, false);
				toastMsg += ((newValue != "None") ? " and " : "") + oldValue + " is now unbound";
			}
			toastMsg += "!";
			gameWrapper->Toast("SkipReplay", toastMsg, "default", 5.0F);
		}
	} else
		loaded = true;
}