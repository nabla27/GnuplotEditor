#include "pch.h"
#include "gnuplotcompletion.h"

#include <iostream>

//extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(GnupltEditorPlugin*& p)
//{
//	p = new GnupltEditorPlugin;
//}

extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(int& a)
{
	a = 2;
}


std::vector<EditorPlugin::SettingItem> GnuplotEditorManager::items = {};

std::unordered_map<std::string, GnuplotEditorManager::CommandInfo>
GnuplotEditorManager::commands = {};


void GnuplotEditorManager::setup()
{
	GnuplotEditorManager::items.push_back(EditorPlugin::SettingItem{ 0, "toolTip", "debug", "plot"});
	GnuplotEditorManager::items.push_back(EditorPlugin::SettingItem{ 1, "double", "detail2", 5.5 });
	GnuplotEditorManager::items.push_back(EditorPlugin::SettingItem{ 2, "string", "detail3", "test"});
	GnuplotEditorManager::items.push_back(EditorPlugin::SettingItem{ 4, "int", "detail4", 10 });


	commands.insert(std::pair<std::string, CommandInfo>("plot", CommandInfo()));
}

void GnuplotEditorManager::toolTip(const std::string& text, ToolTip& toolTip)
{
	if(items.at(0).variant.index() == 2)
		toolTip.text = std::get<2>(items.at(0).variant);
}

void GnuplotEditorManager::completions(const std::string& text, std::unordered_set<Completion>& completions)
{
	completions.clear();
	completions.insert(Completion{ "plot" });
	completions.insert(Completion{ "replot" });
	completions.insert(Completion{ "with" });
	completions.insert(Completion{ "lines" });
	completions.insert(Completion{ "pause" });
}
