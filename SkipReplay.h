#pragma once
#pragma comment( lib, "pluginsdk.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class SkipReplay : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();
	void readyUp();
	void onMatchEndEnable();
	void onSkipEnableChanged(std::string oldValue, CVarWrapper cvar);
	void onKeybindValueChanged(std::string oldValue, CVarWrapper cvar);
};