#include "filetree.h"



//formatを変更する際には、editorやtableに変換できるようなものや、変換する関数を用意する
QStringList ScriptInfo::formatList = QStringList() << ".txt";
QStringList SheetInfo::formatList = QStringList() << ".csv";

FileTree::FileTree(QWidget *parent)
    : QTreeWidget(parent)
{
    /* 右クリックメニューの設定 */
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &FileTree::customContextMenuRequested, this, &FileTree::onCustomContextMenu);
    initializeContextMenu();

    /* ファイルの変更があれば更新するようにする */
    dirWatcher = new QFileSystemWatcher;
    connect(dirWatcher, &QFileSystemWatcher::directoryChanged, this, &FileTree::updateFileTree);

    connect(this, &FileTree::itemDoubleClicked, this, &FileTree::pushClickedItem);

    /* インスタンス化後、GnuplotEditorでsetFolderPath()が呼ばれる */
}

FileTree::~FileTree()
{
    saveAllScript();
    saveAllSheet();
    clearAllList();
}

void FileTree::initializeContextMenu()
{
    if(!normalMenu) delete normalMenu;

    normalMenu = new QMenu(this);

    QAction *const actAdd = new QAction("Add", normalMenu);
    QAction *const actNew = new QAction("New", normalMenu);
    QAction *const actRename = new QAction("Rename", normalMenu);
    QAction *const actRemove = new QAction("Remove", normalMenu);
    QAction *const actExport = new QAction("Export", normalMenu);

    normalMenu->addAction(actAdd);
    normalMenu->addAction(actNew);
    normalMenu->addAction(actRename);
    normalMenu->addAction(actRemove);
    normalMenu->addAction(actExport);

    connect(actAdd, &QAction::triggered, this, &FileTree::addFile);
    connect(actNew, &QAction::triggered, this, &FileTree::newFile);
    connect(actRename, &QAction::triggered, this, &FileTree::renameFile);
    connect(actRemove, &QAction::triggered, this, &FileTree::removeFile);
    connect(actExport, &QAction::triggered, this, &FileTree::exportFile);
}

void FileTree::loadFileTree()
{
    /* treeをクリア */
    clear();

    /* セーブ */
    saveAllScript();
    saveAllSheet();

    /* リストをクリア */
    clearAllList();

    /* ヘッダーを非表示 */
    setHeaderHidden(true);

    /* scriptフォルダーを作成 */
    scriptTree = new QTreeWidgetItem(this);
    scriptTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    scriptTree->setText(0, "Script");
    /* sheetフォルダーの作成 */
    sheetTree = new QTreeWidgetItem(this);
    sheetTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    sheetTree->setText(0, "Sheet");
    /* otherフォルダーの作成 */
    otherTree = new QTreeWidgetItem(this);
    otherTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    otherTree->setText(0, "Other");

    /* ファイルの追加 */
    updateFileTree();
}

void FileTree::updateFileTree()
{
    /* ディレクトリのファイル情報を取得 */
    QDir dir(folderPath);
    QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Files);

    /* ファイルを順に捜査して、拡張子ごとにフォルダーに分ける。 */
    for(const QFileInfo& fileInfo : fileList)
    {
        const QString fileName = fileInfo.fileName();

        if(ScriptInfo::isValidFormat(fileName)){
            if(!scriptList.contains(fileName)) addScript(fileName);
            loadScript(fileName);
        }
        else if(SheetInfo::isValidFormat(fileName)){
            if(!sheetList.contains(fileName)) addSheet(fileName);
            loadSheet(fileName);
        }
        else if(!otherList.contains(fileName))
            addOther(fileName);
        else
            continue;
    }
}

void FileTree::setFolderPath(const QString &folderPath)
{
    this->folderPath = folderPath;
    dirWatcher->removePath(this->folderPath);
    dirWatcher->addPath(folderPath);
    loadFileTree();
}

void FileTree::pushClickedItem(QTreeWidgetItem *item, int column)
{
    /* フォルダーがクリックされた場合は無効 */
    if(item->parent() == nullptr) return;

    /* クリックされたファイルのフォルダー名(Script, Sheet, Other)を取得 */
    const QString folderName = item->parent()->text(column);
    const QString fileName = item->text(0);

    if(folderName == "Script")
        emit scriptSelected(fileName, scriptList.value(fileName));
    else if(folderName == "Sheet")
        emit sheetSelected(fileName, sheetList.value(fileName));
    else if(folderName == "Other")
        emit otherSelected(fileName, otherList.value(fileName));
}



void FileTree::onCustomContextMenu(const QPoint& point)
{
    if(selectedItems().count() < 1) return;

    //Widgetでのカーソル位置をグローバル座標に変換して渡す
    normalMenu->exec(viewport()->mapToGlobal(point));
}

void FileTree::addScript(const QString& fileName)
{
    /* ツリーへ追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(scriptTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    scriptList.insert(fileName, new ScriptInfo(new QProcess(), new TextEdit()));
}

void FileTree::addSheet(const QString& fileName)
{
    /* ツリーへ追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(sheetTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    sheetList.insert(fileName, new SheetInfo(new GnuplotTable()));
}

void FileTree::addOther(const QString& fileName)
{
    /* ツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(otherTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    otherList.insert(fileName, new OtherInfo());
}

void FileTree::loadScript(const QString& fileName)
{
    bool ok = false;

    const QString text = readFileTxt(folderPath + "/" + fileName, &ok);

    if(!ok)
        emit errorCaused("failed to load the file \"" + fileName + "\"", BrowserWidget::MessageType::FileSystemErr);
    else
        scriptList.value(fileName)->editor->setPlainText(text);
}

void FileTree::loadAllScript()
{
    foreach(const QString& fileName, scriptList.keys())
        loadScript(fileName);
}

void FileTree::saveScript(const QString& fileName)
{
    if(!scriptList.contains(fileName)) return;

    bool ok = false;

    toFileTxt(folderPath + "/" + fileName, scriptList.value(fileName)->editor->toPlainText(), &ok);

    if(!ok)
        emit errorCaused("failed to save the file \"" + fileName + "\"", BrowserWidget::MessageType::FileSystemErr);
}

void FileTree::saveAllScript()
{
    foreach(const QString& fileName, scriptList.keys())
        saveScript(fileName);
}

void FileTree::loadSheet(const QString& fileName)
{
    bool ok = false;
    const QList<QList<QString> > data = readFileCsv(folderPath + "/" + fileName, &ok);

    if(ok)
        sheetList.value(fileName)->table->setData<QString>(readFileCsv(folderPath + "/" + fileName));
    else
        emit errorCaused("failed to load the sheet \"" + fileName + "\"", BrowserWidget::MessageType::FileSystemErr);
}

void FileTree::loadAllSheet()
{
    foreach(const QString& fileName, sheetList.keys())
        loadSheet(fileName);
}

void FileTree::saveSheet(const QString& fileName)
{
    if(!sheetList.contains(fileName)) return;

    bool ok = false;
    toFileCsv(folderPath + "/" + fileName, sheetList.value(fileName)->table->getData<QString>(), &ok);

    if(!ok)
        emit errorCaused("failed to save the file \"" + fileName + "\"", BrowserWidget::MessageType::FileSystemErr);
}

void FileTree::saveAllSheet()
{
    foreach(const QString& fileName, sheetList.keys())
        saveSheet(fileName);
}

void FileTree::clearAllList()
{
    foreach(const ScriptInfo *info, scriptList) { delete info; info = nullptr; }
    foreach(const SheetInfo *info, sheetList) { delete info; info = nullptr; }
    foreach(const OtherInfo *info, otherList) { delete info; info = nullptr; }
    scriptList.clear();
    sheetList.clear();
    otherList.clear();
}

/* 既存のファイルをインポートする */
void FileTree::addFile()
{
    /* ファイルダイアログの表示とファイル名の取得 */
    const QStringList filePath = QFileDialog::getOpenFileNames(this);  //選択された(複数可)ファイルのフルパスを取得

    /* 選択されたファイルを一つずつ処理 */
    for(const QString& fullPath : filePath)
    {
        const QString fileName = (fullPath.split('/')).constLast();    //選択されたファイル名

        if(!containsFile(fileName))                                    //もし同名のファイルがなければ
        {
            const bool success = QFile::copy(fullPath, folderPath + "/" + fileName);                         //コピーしてもってくる
            if(!success) { QMessageBox::critical(this, "Error", "could not copy the " + fileName); }         //コピー失敗時のエラーメッセージ
        }
        else                                                           //もし同名のファイルがあれば、エラーメッセージの表示
            QMessageBox::critical(this, "Error", "Same name \"" + fileName + "\" already exists!!");
    }
}

void FileTree::newFile()
{
    /* 追加するファイルの項目名(Script or Sheet or Other)を取得 */
    QString parentTitle;
    if(this->selectedItems().takeAt(0)->parent() == nullptr)               //項目名がクリックされた場合(=その親treeはなし)
        parentTitle = this->selectedItems().takeAt(0)->text(0);
    else                                                                   //ファイル名がクリックされた場合(=その親treeが存在)
        parentTitle = this->selectedItems().takeAt(0)->parent()->text(0);  //親treeの名前を取得

    /* デフォルトで表示するテキスト */
    QString defaultStr;
    if(parentTitle == "Script") defaultStr = ScriptInfo::formatList.constFirst();
    else if(parentTitle == "Sheet") defaultStr = SheetInfo::formatList.constFirst();

    /* 新規ファイルの名前を入力するダイアログを表示 */
    QString newFileName;
    for(;;)                                                            //有効なファイル名が入力されるまでループ
    {
        bool isok = false;
        newFileName = QInputDialog::getText(this, "FileTree", "new file name", QLineEdit::EchoMode::Normal, defaultStr, &isok);

        if(!isok || newFileName.isEmpty()) return;                     //×ボタンでnewFileキャンセルされたら抜ける

        if(!newFileName.contains('.'))                                 //ファイル名に拡張子が含まれていない場合、エラーメッセージを表示
            QMessageBox::critical(this, "Error", "extension is not included.");
        else if(containsFile(newFileName))                             //すでに同じ名前のファイルがあればエラーメッセージを表示
            QMessageBox::critical(this, "Error", "Same name \"" + newFileName + "\" already exists!!");
        else
            break;                                                     //同じ名前がなければ決定
    }

    /* ファイルの作成 */
    QFile file(folderPath + "/" + newFileName);
    const bool success = file.open(QIODevice::OpenModeFlag::NewOnly);  //ファイルの作成

    /* ファイルをリストへ追加 */
    if(success)
    {
        if(ScriptInfo::isValidFormat(newFileName))
            addScript(newFileName);
        else if(SheetInfo::isValidFormat(newFileName))
            addSheet(newFileName);
        else
            addOther(newFileName);
    }
    else //ファイルの作成に失敗したらエラーメッセージ
        QMessageBox::critical(this, "Error", "could not create file.");
}

void FileTree::renameFile()
{
    /* ファイル名でなく、項目名(Script,Sheet,Other)が押された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    /* renameするファイルの元の名前 */
    const QString oldFileName = selectedItems().takeAt(0)->text(0);

    bool isok = false;
    const QString message = "new file name";
    QString newFileName;

    /* ファイル名を入力するダイアログの表示。有効な名前が入力されるまでループ */
    for(;;)
    {
        newFileName = QInputDialog::getText(this, "rename", message, QLineEdit::EchoMode::Normal, oldFileName, &isok);

        if(!isok || newFileName.isEmpty()) return;                     //×などウィンドウが閉じられたら中断

        if(!newFileName.contains('.'))                                 //拡張子が含まれていない場合はエラー
            QMessageBox::critical(this, "Error", "extension is not included.");
        else if(containsFile(newFileName))                             //すでに同じ名前があればエラー
            QMessageBox::critical(this, "Error", "Same name \"" + newFileName + "\" already exists!!");
        else if(oldFileName.mid(oldFileName.lastIndexOf('.')) != newFileName.mid(newFileName.lastIndexOf('.')))  //拡張子が変わればエラー
            QMessageBox::critical(this, "Error", "extension is changed.");
        else
            break;
    }

    /* ディレクトリのファイル名とfileTreeのファイル名を変更 */
    QDir dir(folderPath);
    dir.rename(oldFileName, newFileName);
    selectedItems().takeAt(0)->setText(0, newFileName);

    /* リストの変更 */
    if(ScriptInfo::isValidFormat(newFileName)){
        ScriptInfo *info = scriptList.take(oldFileName);
        scriptList.insert(newFileName, info);
    }
    else if(SheetInfo::isValidFormat(newFileName)){
        SheetInfo *info = sheetList.take(oldFileName);
        sheetList.insert(newFileName, info);
    }
    else{
        OtherInfo *info = otherList.take(oldFileName);
        otherList.insert(newFileName, info);
    }

    emit fileNameChanged(oldFileName, newFileName);
}

void FileTree::removeFile()
{
    /* ファイル名ではなく、項目名(Script,Sheet,Other)が選択された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    const QString parentTitle = selectedItems().takeAt(0)->parent()->text(0);                      //選択されたファイルの項目名
    const QString selectedFile = selectedItems().takeAt(0)->text(0);                               //選択されたファイルの名前
    const QString message = "Are you want to remove this \"" + selectedFile + "\" ??   ";          //ダイアログメッセージ

    /* 確認のためのメッセージボックス */
    QMessageBox::StandardButton reply = QMessageBox::question(this, "remove", message, QMessageBox::Yes | QMessageBox::No);

    /* 確認してNoまたは×ボタンが押されたらなら無効 */
    if(reply == QMessageBox::No) return;

    /* リストの削除 */
    if(parentTitle == "Script"){
        delete scriptList.value(selectedFile);
        scriptList.remove(selectedFile);
        emit scriptRemoved(selectedFile, scriptList.value(selectedFile));
    }
    else if(parentTitle == "Sheet"){
        delete sheetList.value(selectedFile);
        sheetList.remove(selectedFile);
        emit sheetRemoved(selectedFile, sheetList.value(selectedFile));
    }
    else if(parentTitle == "Other"){
        delete otherList.value(selectedFile);
        otherList.remove(selectedFile);
        emit otherRemoved(selectedFile, otherList.value(selectedFile));
    }

    /* ファイルの削除 */
    QDir dir(folderPath);
    dir.remove(selectedFile);

    /* リストからファイルを削除 */
    selectedItems().takeAt(0)->parent()->removeChild(selectedItems().takeAt(0));
}

void FileTree::exportFile()
{
    /* ファイル名以外が選択された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    /* ディレクトリダイアログの表示と保存するフォルダーのフルパス取得 */
    const QString pathForSave = QFileDialog::getExistingDirectory(this);

    /* ディレクトリが選択されなければ無効 */
    if(pathForSave.isEmpty()) return;

    /* 選択されたファイルの名前 */
    const QString fileName = this->selectedItems().takeAt(0)->text(0);

    /* コピーする前に保存 */
    if(ScriptInfo::isValidFormat(fileName)) saveScript(fileName);
    else if(SheetInfo::isValidFormat(fileName)) saveSheet(fileName);

    /* ファイルをコピーして保存 */
    const bool success = QFile::copy(folderPath + "/" + fileName, pathForSave + "/" + fileName);
    if(!success) emit errorCaused("could not copy a file \"" + fileName + "\"", BrowserWidget::MessageType::FileSystemErr);

    /* ファイルの削除とその確認 */
    removeFile();
}

void FileTree::addFolder()
{
    /* フォルダーの選択 */
    const QString& folder = QFileDialog::getExistingDirectory(this);

    /* 選択したフォルダー内のファイルを取得 */
    const QDir dir(folder);
    const QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Files);

    /* コピーしてもってくる */
    for(const QFileInfo& info : fileList)
    {
        const QString fileName = info.fileName();
        const bool success = QFile::copy(folder + "/" + fileName, folderPath + "/" + fileName);
        if(!success) emit errorCaused("could not copy a file \"" + info.absoluteFilePath() + "\"", BrowserWidget::MessageType::FileSystemErr);
    }

    /* ファイルツリーの更新 */
    updateFileTree();
}

void FileTree::saveFolder()
{
    /* フォルダーの選択 */
    const QString& folder = QFileDialog::getExistingDirectory(this);

    /* 作業ディレクトリのファイル情報 */
    const QDir dir(folderPath);
    const QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Files);

    /* コピーする */
    for(const QFileInfo& info : fileList)
    {
        const QString& fileName = info.fileName();
        const bool success = QFile::copy(folderPath + "/" +  fileName, folder + "/" + fileName);
        if(!success) emit errorCaused("could not copy a file \"" + info.absoluteFilePath() + "\"", BrowserWidget::MessageType::FileSystemErr);
    }
}



















