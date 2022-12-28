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
    enum class PluginType { EditorManager };

    const std::string name;
    const std::string version;
    const std::string description;
    const PluginType type;
};




class Manager
{
    virtual void setEnable(bool enable) = 0;
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

        const std::string name;
        const std::string detail;
        std::variant<int, double, std::string> variant;
    };

    AbstractPlugin(const PluginInfo& info,
                   const std::vector<SettingItem>& items)
        : info(info)
        , items(items) {}

    //virtual void info(PluginInfo& info) = 0;
    virtual void setup() = 0;
    //virtual void settingItems(std::vector<SettingItem>*& items) = 0;
    virtual Manager* createInstance() const = 0;

    const PluginInfo info;
    std::vector<SettingItem> items;
};



typedef AbstractPlugin*(*CreatePluginInstanceFuncType)(void);






#endif // PLUGIN_BASE_H
