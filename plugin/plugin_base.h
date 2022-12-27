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
#include <variant>
#include <vector>



struct PluginInfo
{
    std::string name;
    std::string version;
};






class AbstractPlugin
{
public:
    struct SettingItem
    {
    private:
        template <int> struct _VariantType;
        template <> struct _VariantType<0> { using Type = int; };
        template <> struct _VariantType<1> { using Type = double; };
        template <> struct _VariantType<2> { using Type = std::string; };

    public:
        template <int index>
        using VariantTypeOf = typename _VariantType<index>::Type;

        enum class VariantType { Int, Double, StdString };

        const int id;
        const std::string name;
        const std::string detail;
        std::variant<int, double, std::string> variant;
    };

    virtual void info(PluginInfo& info) = 0;
    virtual void setup() = 0;
    virtual void settingItems(std::vector<SettingItem>*& items) = 0;
};






#endif // PLUGIN_BASE_H
