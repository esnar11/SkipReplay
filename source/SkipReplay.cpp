#include "skipReplay.h"

BAKKESMOD_PLUGIN(SkipReplay, "Skip Replay", "1.5", PLUGINTYPE_REPLAY)

void SkipReplay::onLoad()
{
	std::string logoPath = std::string("./bakkesmod/data/assets/skipreplay_logo.tga");
	logo = std::make_unique<ImageWrapper>(logoPath, false, false);
	gameWrapper->LoadToastTexture("skipreplay_logo", logoPath);
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay", std::bind(&SkipReplay::Skip, this));

	cvarManager->registerNotifier("toggleskipreplay", [this](std::vector<std::string> params) {
		if (enabled = !enabled) {
			gameWrapper->ExecuteUnrealCommand("ReadyUp");
			gameWrapper->Toast("SkipReplay", "Auto Skipping is now enabled!", "skipreplay_logo", 2.0f);
			gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay", std::bind(&SkipReplay::Skip, this));
		}
		else {
			gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay");
			gameWrapper->Toast("SkipReplay", "Auto Skipping is now disabled!", "skipreplay_logo", 2.0f);
		}}, "Bind to Toggle Replay Skip", PERMISSION_ALL);

	keybindCvar = std::make_unique<CVarWrapper>(cvarManager->registerCvar("skipKeybind", "0", "Toggle SkipReplay keybind", false));
	missingCvar = std::make_unique<CVarWrapper>(cvarManager->registerCvar("skipMissingCheckbox", "0", "Don't skip when teammate is missing", false));
	(reEnableCvar = std::make_unique<CVarWrapper>(cvarManager->registerCvar("skipEnableCheckbox", "0", "Re-enable skipping when match ends", false)))->addOnValueChanged([this](std::string, CVarWrapper cvar) {
		if (cvar.getBoolValue())
			gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", [&](std::string eventName) {
			if (!enabled) {
				gameWrapper->Toast("SkipReplay", "Auto Skipping is re-enabled!", "skipreplay_logo", 5.0f);
				gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.ShouldPlayReplay", [&](std::string eventName) { gameWrapper->ExecuteUnrealCommand("ReadyUp"); });
				enabled = true;
			}});
		else
			gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
		});;
}

void SkipReplay::Skip()
{
	if (missingCvar->getBoolValue()){
		unsigned char team = gameWrapper->GetPlayerController().GetPRI().GetTeamNum();
		ServerWrapper server = gameWrapper->GetCurrentGameState();
		ArrayWrapper<PriWrapper> players = server.GetPRIs();
		unsigned int teamCount = 0;

		for (int i = 0; i < players.Count(); i++)
			if (players.Get(i).GetTeamNum() == team)
				teamCount++;

		if (teamCount >= server.GetMaxTeamSize())
			gameWrapper->ExecuteUnrealCommand("ReadyUp");
		else
			gameWrapper->Toast("SkipReplay", "Not skipping because teammate is missing!", "skipreplay_logo", 5.0f);
	} else
		gameWrapper->ExecuteUnrealCommand("ReadyUp");

}

void SkipReplay::RenderSettings()
{
	static const char *keyText = "Key List";
	static const char *hintText = "Type to Filter";
	static bool reEnable = reEnableCvar->getBoolValue();
	static bool missing = missingCvar->getBoolValue();

	if (!keyIndex) {
		keybind = keybindCvar->getStringValue();
		keyIndex = (keysIt = find(keys.begin(), keys.end(), keybind)) != keys.end() ? (int)(keysIt - keys.begin()) : -1;
		logo->LoadForCanvas();
	}

	if (ImGui::Checkbox("Re-enable skipping when match ends", &reEnable))
		reEnableCvar->setValue(reEnable);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("For if you forget to toggle it back on");
	if (ImGui::Checkbox("Don't skip when teammate is missing", &missing))
		missingCvar->setValue(missing);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("To allow disconnected teammate to reconnect");

	ImGui::TextUnformatted("Press the Set Keybind button below to bind command toggleskipreplay to a key:");
	if (ImGui::SearchableCombo("##keybind combo", &keyIndex, keys, keyText, hintText, 20))
		OnBind(keys[keyIndex]);

	ImGui::SameLine();
	if (ImGui::ButtonEx("Set Keybind", ImVec2(0, 0), ImGuiButtonFlags_AlignTextBaseLine)) {
		gameWrapper->Execute([this](GameWrapper *gw) {
			cvarManager->executeCommand("closemenu settings; openmenu skipreplaybind");
			gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.GameViewportClient_TA.HandleKeyPress", std::bind(&SkipReplay::OnKeyPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			});
	}

	ImGui::SameLine();
	if (ImGui::ButtonEx("Unbind", ImVec2(0, 0), ImGuiButtonFlags_AlignTextBaseLine)) {
		if (keyIndex != -1) {
			keybindCvar->setValue("0");
			cvarManager->executeCommand("unbind " + keys[keyIndex]);
			gameWrapper->Toast("SkipReplay", "toggleskipreplay is now unbound!", "skipreplay_logo", 5.0f);
			keyIndex = -1;
		}
	}

	ImGui::TextUnformatted("The toggleskipreplay keybind can be used to disable and enable autoskipping for that rare occasion that you dont want to skip the replay!");
	ImGui::Separator();
	ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 65);
	ImGui::Image(logo->GetImGuiTex(), { logo->GetSizeF().X, logo->GetSizeF().Y });
	ImGui::TextUnformatted("v1.5 made by Esnar#0600 and Insane#0418");
}

void SkipReplay::Render()
{
	ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(140, 40), ImGuiCond_Once);
	ImGui::Begin("Set Keybind", &isWindowOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowFontScale(1.6f);
	ImGui::TextUnformatted("Press any key");
	ImGui::End();
}

void SkipReplay::OnBind(std::string key)
{
	if (key != (keybind = keybindCvar->getStringValue())) {
		std::string toastMsg = "toggleskipreplay bound to " + keys[keyIndex];
		if (keybind != "0") {
			toastMsg += " and " + keybind + " is now unbound";
			cvarManager->executeCommand("unbind " + keybind);
		}
		toastMsg += "!";
		keybindCvar->setValue(key);
		cvarManager->executeCommand("bind " + key + " toggleskipreplay");
		gameWrapper->Toast("SkipReplay", toastMsg, "skipreplay_logo", 5.0f);
	}
}

void SkipReplay::OnKeyPressed(ActorWrapper aw, void *params, std::string eventName)
{
	std::string key = gameWrapper->GetFNameByIndex(((keypress_t *)params)->key.Index);
	keyIndex = ((keysIt = find(keys.begin(), keys.end(), key)) != keys.end()) ? (int)(keysIt - keys.begin()) : -1;
	cvarManager->executeCommand("closemenu skipreplaybind; openmenu settings");
	gameWrapper->UnhookEvent("Function TAGame.GameViewportClient_TA.HandleKeyPress");
	gameWrapper->Execute([this](GameWrapper *gw) { OnBind(keys[keyIndex]); });
}

void SkipReplay::OnOpen()
{
	isWindowOpen = true;
}

void SkipReplay::OnClose()
{
	isWindowOpen = false;
}

std::string SkipReplay::GetPluginName()
{
	return "Skip Replay";
}

std::string SkipReplay::GetMenuName()
{
	return "skipreplaybind";
}

std::string SkipReplay::GetMenuTitle()
{
	return "";
}

void SkipReplay::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

bool SkipReplay::ShouldBlockInput()
{
	return false;
}

bool SkipReplay::IsActiveOverlay()
{
	return true;
}

void SkipReplay::onUnload()
{
}