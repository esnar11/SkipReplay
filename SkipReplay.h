#pragma once
#pragma comment( lib, "pluginsdk.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class SkipReplay : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	std::shared_ptr<bool>  skipEnable;
	bool enabled = true;

public:
	virtual void onLoad();
	virtual void onUnload();
	void readyUp();
	void onMatchEndEnable();
	void onSkipEnableChanged();
	void onKeybindValueChanged(std::string oldValue, CVarWrapper cvar);
};