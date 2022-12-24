#ifdef GNUPLOTEDITOR_DLL_EXPORTS
#define GNUPLOTEDITOR_DLL __declspec(dllexport)
#else
#define GNUPLOTEDITOR_DLL __declspec(dllimport)
#endif


#include "editor_plugin.h"
using namespace editorplugin;


class GnuplotEditorManager : public EditorManager
{
public:
	void setup() override
	{

	}

	void toolTip(const std::string& text, ToolTip& toolTip) override
	{

	}

	void completions(const std::string& text, std::unordered_set<Completion>& completions) override
	{

	}
};

class GnupltEditorPlugin : public EditorPlugin
{
public:
	void info(PluginInfo& info) override
	{
		info.name = "Gnuplot Editor Plugin";
		info.version = "1.0";
	}

	EditorManager* createInstance() const override
	{
		return new GnuplotEditorManager;
	}
};








extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(GnupltEditorPlugin *&p);
//extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(int* p);



