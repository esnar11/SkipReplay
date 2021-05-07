#include "skipReplay.h"
#include "bakkesmod/wrappers/includes.h"

BAKKESMOD_PLUGIN(SkipReplay, "Skip Replay", "1.0a", PLUGINTYPE_REPLAY)

bool enabled = 1;

void SkipReplay::onLoad()
{
	cvarManager->registerNotifier("toggleskipreplay", [this](std::vector<std::string> commands) {
		gameWrapper->Toast("SkipReplay", (enabled = !enabled) ? "Auto Skipping is now enabled." : "Auto Skipping is now disabled.", "default", 2.0F);
		}, "Bind to Toggle Replay Skip", PERMISSION_ALL);

	cvarManager->log("Skip Replay Plugin Loaded Succesfully!");
	// Subscribe to event that only triggers once at the start of replay
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay", std::bind(&SkipReplay::readyUp, this));
}

void SkipReplay::onUnload()
{
	cvarManager->log("Skip Replay Plugin Unloaded!");
}

void SkipReplay::readyUp() {
	if (enabled)
		cvarManager->executeCommand("unreal_command ReadyUp");
}