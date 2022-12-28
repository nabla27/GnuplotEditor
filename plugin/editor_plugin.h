/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef EDITOR_PLUGIN_H
#define EDITOR_PLUGIN_H

#include <string>
#include <unordered_set>
#include "plugin_base.h"


namespace editorplugin
{




struct ToolTip
{
    std::string text;
};


struct Completion
{
    std::string text;

    bool operator ==(const Completion& c) const noexcept
    {
        return (text == c.text);
    }
};




class EditorManager : public Manager
{
public:
    virtual void toolTip(const std::string& text, ToolTip& toolTip) = 0;
    virtual void completions(const std::string& text, std::unordered_set<Completion>& completions) = 0;
};






}

template <>
struct std::hash<editorplugin::Completion>
{
    std::size_t operator()(const editorplugin::Completion& c) const noexcept
    {
        std::size_t seed = 0;
        auto n_hash = std::hash<std::string>()(c.text);
        seed ^= n_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};



#endif // EDITOR_PLUGIN_H
