#include "pch.h"
#include "gnuplotcompletion.h"

#include <iostream>

extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(GnupltEditorPlugin*& p)
{
	p = new GnupltEditorPlugin;
}

//extern "C" GNUPLOTEDITOR_DLL void createPluginInstance(int* p)
//{
//	*p = 2;
//}


