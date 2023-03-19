#ifdef GNUPLOTEDITOR_DLL_EXPORTS
#define GNUPLOTEDITOR_DLL __declspec(dllexport)
#else
#define GNUPLOTEDITOR_DLL __declspec(dllimport)
#endif

#include "editor_plugin.h"

class GnuplotManager : public editorplugin::EditorManager
{
public:
	void toolTip(const std::string& text, editorplugin::ToolTip& toolTip) override
	{
		toolTip.text = "test toolTip";
	}
	void completions(const std::string& text,
		std::unordered_set<editorplugin::Completion>& completions) override
	{
		editorplugin::Completion c;
		c.text = "a";
		completions.insert(c);
	}
	void setEnable(bool enable) override
	{

	}
};

class GnuplotPlugin : public AbstractPlugin
{
public:
	GnuplotPlugin(const PluginInfo& info,
			      const std::vector<SettingItem>& items)
		: AbstractPlugin(info, items)
	{

	}

	void setup() override
	{

	}

	Manager* createInstance() const
	{
		GnuplotManager* m = new GnuplotManager();
		return m;
	}
};

extern "C" GNUPLOTEDITOR_DLL AbstractPlugin* createPluginInstance();
