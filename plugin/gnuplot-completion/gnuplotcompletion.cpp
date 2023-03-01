#include "pch.h"
#include "gnuplotcompletion.h"


extern "C" GNUPLOTEDITOR_DLL AbstractPlugin* createPluginInstance()
{
	PluginInfo info;
	info.name = "gnuplot plugin";
	info.type = PluginInfo::PluginType::EditorManager;

	std::vector<AbstractPlugin::SettingItem> items;

	GnuplotPlugin* p = new GnuplotPlugin(info, items);
	return p;
}


