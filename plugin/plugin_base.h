/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef PLUGIN_BASE_H
#define PLUGIN_BASE_H

#include <string>

struct PluginInfo
{
    std::string name;
    std::string version;
};




class AbstractPlugin
{
public:
    virtual void info(PluginInfo& info) = 0;
};



#endif // PLUGIN_BASE_H
