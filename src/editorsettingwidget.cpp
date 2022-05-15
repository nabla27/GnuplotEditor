#include "editorsettingwidget.h"


EditorSettingWidget::EditorSettingWidget(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(240, 520);
    setMaximumSize(240, 520);

    initializeLayout();

    loadXmlSetting();
}

EditorSettingWidget::~EditorSettingWidget()
{
    saveXmlSetting();
}

void EditorSettingWidget::initializeLayout()
{
    if(!layout()) delete layout();

    QVBoxLayout *layout = new QVBoxLayout(this);
    backgroundColorCombo = new ComboEditLayout(this, "Background color");
    backgroundColorRgb = new RGBEditLayout(this);
    textColorCombo = new ComboEditLayout(this, "Text color");
    textColorRgb = new RGBEditLayout(this);
    textSizeSpin = new SpinBoxEditLayout(this, "Text size");
    tabSpaceEdit = new LineEditLayout(this, "Tab spacing");
    checkWrap = new CheckBoxLayout(this, "Wrap line");
    checkItaric = new CheckBoxLayout(this, "Itaric text");
    checkBold = new CheckBoxLayout(this ,"Bold text");
    mainCmdColorCombo = new ComboEditLayout(this, "Main cmd color");
    mainCmdColorRgb = new RGBEditLayout(this);
    commentColorCombo = new ComboEditLayout(this, "Comment color");
    commentColorRgb = new RGBEditLayout(this);
    cursorLineColorCombo = new ComboEditLayout(this, "Cursor line color");
    cursorLineColorRgb = new RGBEditLayout(this);
    stringColorCombo = new ComboEditLayout(this, "String color");
    stringColorRgb = new RGBEditLayout(this);
    BlankSpaceLayout *blank = new BlankSpaceLayout(0, 30);
    PushButtonLayout *setDefaultButton = new PushButtonLayout(this, "Default");
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(backgroundColorCombo);
    layout->addLayout(backgroundColorRgb);
    layout->addLayout(textColorCombo);
    layout->addLayout(textColorRgb);
    layout->addLayout(textSizeSpin);
    layout->addLayout(tabSpaceEdit);
    layout->addLayout(checkWrap);
    layout->addLayout(checkItaric);
    layout->addLayout(checkBold);
    layout->addLayout(mainCmdColorCombo);
    layout->addLayout(mainCmdColorRgb);
    layout->addLayout(commentColorCombo);
    layout->addLayout(commentColorRgb);
    layout->addLayout(cursorLineColorCombo);
    layout->addLayout(cursorLineColorRgb);
    layout->addLayout(stringColorCombo);
    layout->addLayout(stringColorRgb);
    layout->addLayout(blank);
    layout->addLayout(setDefaultButton);
    layout->addItem(spacer);

    const int labelWidth = 100;
    backgroundColorCombo->setLabelMinimumWidth(labelWidth);
    backgroundColorRgb->setLabelMinimumWidth(labelWidth);
    textColorCombo->setLabelMinimumWidth(labelWidth);
    textColorRgb->setLabelMinimumWidth(labelWidth);
    textSizeSpin->setLabelMinimumWidth(labelWidth);
    tabSpaceEdit->setLabelMinimumWidth(labelWidth);
    checkWrap->setLabelMinimumWidth(labelWidth);
    checkItaric->setLabelMinimumWidth(labelWidth);
    checkBold->setLabelMinimumWidth(labelWidth);
    mainCmdColorCombo->setLabelMinimumWidth(labelWidth);
    mainCmdColorRgb->setLabelMinimumWidth(labelWidth);
    commentColorCombo->setLabelMinimumWidth(labelWidth);
    commentColorRgb->setLabelMinimumWidth(labelWidth);
    cursorLineColorCombo->setLabelMinimumWidth(labelWidth);
    cursorLineColorRgb->setLabelMinimumWidth(labelWidth);
    stringColorCombo->setLabelMinimumWidth(labelWidth);
    stringColorRgb->setLabelMinimumWidth(labelWidth);

    backgroundColorCombo->insertComboItems(0, colorNameList());
    textColorCombo->insertComboItems(0, colorNameList());
    mainCmdColorCombo->insertComboItems(0, colorNameList());
    commentColorCombo->insertComboItems(0, colorNameList());
    cursorLineColorCombo->insertComboItems(0, colorNameList());
    stringColorCombo->insertComboItems(0, colorNameList());

    connect(backgroundColorCombo, &ComboEditLayout::currentComboIndexChanged, backgroundColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(backgroundColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::backgroundColorSet);
    connect(textColorCombo, &ComboEditLayout::currentComboIndexChanged, textColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(textColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::textColorSet);
    connect(textSizeSpin, &SpinBoxEditLayout::spinBoxValueChanged, this, &EditorSettingWidget::textSizeSet);
    connect(tabSpaceEdit, &LineEditLayout::lineValueEdited, this, &EditorSettingWidget::tabSpaceSet);
    connect(checkWrap, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkWrapSet);
    connect(checkItaric, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkItaricSet);
    connect(checkBold, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkBoldSet);
    connect(mainCmdColorCombo, &ComboEditLayout::currentComboIndexChanged, mainCmdColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(mainCmdColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::mainCmdColorSet);
    connect(commentColorCombo, &ComboEditLayout::currentComboIndexChanged, commentColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(commentColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::commentColorSet);
    connect(cursorLineColorCombo, &ComboEditLayout::currentComboIndexChanged, cursorLineColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(cursorLineColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::cursorLineColorSet);
    connect(stringColorCombo, &ComboEditLayout::currentComboIndexChanged, stringColorRgb, &RGBEditLayout::setColorAndEditable);
    connect(stringColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::stringColorSet);
    connect(setDefaultButton, &PushButtonLayout::buttonReleased, this, &EditorSettingWidget::loadDefaultSetting);
}

void EditorSettingWidget::loadXmlSetting()
{
    using namespace boost::property_tree;

    if(QFile::exists(settingFolderPath + "/" + settingFileName))
    {
        ptree pt;
        read_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData(), pt); //存在しないファイルやフォルダーを指定するとエラー(落ちる)

        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.backgroundColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.backgroundColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.backgroundColor.b"))
                b = blue.value();
            backgroundColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            backgroundColorRgb->setColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.textColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.textColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.textColor.b"))
                b = blue.value();
            textColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            textColorRgb->setColor(QColor(r, g, b));
        }
        if(boost::optional<int> ps = pt.get_optional<int>("root.textSize"))
            textSizeSpin->setSpinBoxValue(ps.value());
        if(boost::optional<double> space = pt.get_optional<double>("root.tabSpace"))
            tabSpaceEdit->setLineEditText(QString::number(space.value()));
        if(boost::optional<bool> wrap = pt.get_optional<bool>("root.wrap"))
            checkWrap->setChecked(wrap.value());
        if(boost::optional<bool> itaric = pt.get_optional<bool>("root.itaric"))
            checkItaric->setChecked(itaric.value());
        if(boost::optional<bool> bold = pt.get_optional<bool>("root.bold"))
            checkBold->setChecked(bold.value());
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.mainCmdColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.mainCmdColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.mainCmdColor.b"))
                b = blue.value();
            mainCmdColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            mainCmdColorRgb->setColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.commentColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.commentColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.commentColor.b"))
                b = blue.value();
            commentColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            commentColorRgb->setColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.cursorLineColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.cursorLineColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.cursorLineColor.b"))
                b = blue.value();
            cursorLineColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            cursorLineColorRgb->setColor(QColor(r, g, b));
        }
        {
            int r = 0, g = 0, b = 0;
            if(boost::optional<int> red = pt.get_optional<int>("root.stringColor.r"))
                r = red.value();
            if(boost::optional<int> green = pt.get_optional<int>("root.stringColor.g"))
                g = green.value();
            if(boost::optional<int> blue = pt.get_optional<int>("root.stringColor.b"))
                b = blue.value();
            stringColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
            stringColorRgb->setColor(QColor(r, g, b));
        }
    }
    else
    {
        loadDefaultSetting();
    }
}

void EditorSettingWidget::loadDefaultSetting()
{
    backgroundColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    backgroundColorRgb->setColor(Qt::GlobalColor::black);
    textColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    textColorRgb->setColor(Qt::GlobalColor::white);
    textSizeSpin->setSpinBoxValue(9);
    tabSpaceEdit->setLineEditText("20.0");
    checkWrap->setChecked(true);
    checkItaric->setChecked(false);
    checkBold->setChecked(false);
    mainCmdColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    mainCmdColorRgb->setColor(Qt::GlobalColor::cyan);
    commentColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    commentColorRgb->setColor(Qt::GlobalColor::lightGray);
    cursorLineColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    cursorLineColorRgb->setColor(QColor(50, 50, 50));
    stringColorCombo->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    stringColorRgb->setColor(QColor(255, 200, 0));
}

void EditorSettingWidget::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    ptree& backgroundColor = pt.add("root.backgroundColor", "");
    backgroundColor.add("r", backgroundColorRgb->getColorR());
    backgroundColor.add("g", backgroundColorRgb->getColorG());
    backgroundColor.add("b", backgroundColorRgb->getColorB());

    ptree& textColor = pt.add("root.textColor", "");
    textColor.add("r", textColorRgb->getColorR());
    textColor.add("g", textColorRgb->getColorG());
    textColor.add("b", textColorRgb->getColorB());

    pt.add("root.textSize", textSizeSpin->spinBoxValue());
    pt.add("root.tabSpace", tabSpaceEdit->lineEditText().toUtf8().constData());
    pt.add("root.wrap", checkWrap->isChecked());
    pt.add("root.itaric", checkItaric->isChecked());
    pt.add("root.bold", checkBold->isChecked());

    ptree& mainCmdColor = pt.add("root.mainCmdColor", "");
    mainCmdColor.add("r", mainCmdColorRgb->getColorR());
    mainCmdColor.add("g", mainCmdColorRgb->getColorG());
    mainCmdColor.add("b", mainCmdColorRgb->getColorB());

    ptree& commentColor = pt.add("root.commentColor", "");
    commentColor.add("r", commentColorRgb->getColorR());
    commentColor.add("g", commentColorRgb->getColorG());
    commentColor.add("b", commentColorRgb->getColorB());

    ptree& cursorLineColor = pt.add("root.cursorLineColor", "");
    cursorLineColor.add("r", cursorLineColorRgb->getColorR());
    cursorLineColor.add("g", cursorLineColorRgb->getColorG());
    cursorLineColor.add("b", cursorLineColorRgb->getColorB());

    ptree& stringColor = pt.add("root.stringColor", "");
    stringColor.add("r", stringColorRgb->getColorR());
    stringColor.add("g", stringColorRgb->getColorG());
    stringColor.add("b", stringColorRgb->getColorB());

    const int indent = 4;

    //保存用のフォルダが無ければ作成
    QDir dir(settingFolderPath);
    if(!dir.exists()) {
        QDir dir;
        const bool success = dir.mkdir(settingFolderPath);
        if(!success) {
            emit errorCaused("failed to create the folder \"" + settingFolderPath + "\", so it could not save settings.", BrowserWidget::MessageType::FileSystemErr);
            return;
        }
    }

    //存在しないフォルダーを含むパスを指定した場合はクラッシュする
    //存在しないファイルは指定しても大丈夫
    write_xml((settingFolderPath + "/" + settingFileName).toUtf8().constData()
              , pt, std::locale(), xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}

void EditorSettingWidget::set(TextEdit *const editor)
{
    if(!editor) return;

    editor->setBackgroundColor(backgroundColorRgb->getColor());
    editor->setTextColor(textColorRgb->getColor());
    editor->setTextSize(textSizeSpin->spinBoxValue());
    editor->setTabSpace(tabSpaceEdit->lineEditText().toDouble());
    editor->setWrap(checkWrap->isChecked());
    editor->setItaric(checkItaric->isChecked());
    editor->setBold(checkBold->isChecked());
    editor->setMainCmdColor(mainCmdColorRgb->getColor());
    editor->setCommentColor(commentColorRgb->getColor());
    editor->setCursorLineColor(cursorLineColorRgb->getColor());
    editor->setStringColor(stringColorRgb->getColor());
}




















