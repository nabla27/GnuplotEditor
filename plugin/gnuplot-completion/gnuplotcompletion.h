#ifdef GNUPLOTEDITOR_DLL_EXPORTS
#define GNUPLOTEDITOR_DLL __declspec(dllexport)
#else
#define GNUPLOTEDITOR_DLL __declspec(dllimport)
#endif


#include "editor_plugin.h"
#include <unordered_map>
#include <iostream>
using namespace editorplugin;





class GnuplotEditorManager : public EditorManager
{
public:
	/* reimplement */
	void toolTip(const std::string& text, ToolTip& toolTip) override;
	void completions(const std::string& text, std::unordered_set<Completion>& completions);
	/* original */
	static void setup();
	static std::vector<AbstractPlugin::SettingItem> items;

private:
	struct CommandInfo
	{
		std::string toolTip;
		std::string formula;
	};
	static std::unordered_map<std::string, CommandInfo> commands;
};






















class GnupltEditorPlugin : public EditorPlugin
{
public:
	void info(PluginInfo& info) override
	{
		info.name = "Gnuplot Editor Manager";
		info.version = "1.0";
	}

	void setup() override
	{
		GnuplotEditorManager::setup();
	}

	void settingItems(std::vector<SettingItem>*& items)
	{
		items = &GnuplotEditorManager::items;
	}

	EditorManager* createInstance() const override { return new GnuplotEditorManager; }
};








extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(GnupltEditorPlugin *&p);

