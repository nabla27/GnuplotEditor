#include "iofile.h"


void ReadTxtFile::getTxt(const QString& path, QString& text, bool& ok)
{
    QFile file(path);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        text = in.readAll();

        file.close();
        ok = true;
    }
    else
        ok = false;
}

void ReadTxtFile::read(const QString& path)
{
    QString text;
    bool ok;

    getTxt(path, text, ok);

    emit finished(text, ok);
}

void WriteTxtFile::toTxt(const QString &path, const QString &text, bool &ok)
{
    QFile file(path);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << text;

        file.close();
        ok = true;
    }
    else
        ok = false;
}

void WriteTxtFile::write(const QString& path, const QString& text)
{
   bool ok = false;

   toTxt(path, text, ok);

    emit finished(ok);
}


void ReadCsvFile::read(const QString& path)
{
    QString data;
    bool ok = false;
    ReadTxtFile::getTxt(path, data, ok);                 //ファイルの内容(text)をQString型で保持

    /* ファイルの読み取り失敗 または 空文字だった場合 */
    if(!ok || data.isEmpty())
    {
        emit finished(QList<QList<QString> >(), ok);
        return;
    }
    else
    {
        QList<QList<QString> > sheet(1);                 //csvデータを2次元のQString型で返す
        qsizetype row = 0;                               //行数
        QString stack = "";                              //スタック

        for(const QChar& c : data)
        {
            if(c == '\n'){
                sheet[row] << stack;
                stack.clear();
                sheet.append(QList<QString>(0));
                row++;
            }
            else if(c == ','){
                sheet[row] << stack;
                stack.clear();
            }
            else
                stack += c;
        }
        sheet[row] << stack;

        emit finished(sheet, true);
    }
}

void WriteCsvFile::write(const QString& path, const QList<QList<QString> >& sheet)
{
    QString data;
    for(qsizetype row = 0; row < sheet.size(); ++row){
        for(qsizetype col = 0; col < sheet.at(row).size(); ++col)
        {
            data += sheet.at(row).at(col);
            if(col != sheet.at(row).size() - 1) { data += ','; }
        }
        if(row != sheet.size() - 1) { data += '\n'; }
    }

    bool ok = false;
    WriteTxtFile::toTxt(path, data, ok);

    emit finished(ok);
}

void ReadTsvFile::read(const QString& path)
{
    bool ok = false;
    QString data;

    ReadTxtFile::getTxt(path, data, ok);

    if(!ok || data.isEmpty())
    {
        emit finished(QList<QList<QString> >(), ok);
        return;
    }
    else
    {
        QList<QList<QString> > sheet(1);   //tsvデータを2次元のQString型で返す
        qsizetype row = 0;
        QString stack = "";

        for(const QChar& c : data)
        {
            if(c == '\n'){
                sheet[row] << stack;
                stack.clear();
                sheet.append(QList<QString>(0));
                row++;
            }
            else if(c == '\t'){
                sheet[row] << stack;
                stack.clear();
            }
            else
                stack += c;
        }
        sheet[row] << stack;

        emit finished(sheet, true);
    }
}

void WriteTsvFile::write(const QString& path, const QList<QList<QString> >& sheet)
{
    QString data;
    for(qsizetype row = 0; row < sheet.size(); ++row){
        for(qsizetype col = 0; col < sheet.at(row).size(); ++col)
        {
            data += sheet.at(row).at(col);
            if(col != sheet.at(row).size() - 1) { data += '\t'; }
        }
        if(row != sheet.size() - 1) { data += '\n'; }
    }

    bool ok = false;
    WriteTxtFile::toTxt(path, data, ok);

    emit finished(ok);
}















void ExcelDocument::readXml()
{
    QDomDocument doc("test");
    QFile file("F:/repos/book1/xl/worksheets/sheet1.xml");

    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    if(!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();


    static const QString tag_sheet = "sheetData";
    static const QString tag_row = "row";
    static const QString tag_col = "c";
    static const QString tag_index = "r";
    static const QString tag_value = "v";


    const QDomNodeList sheetData_list = doc.elementsByTagName(tag_sheet);
    if(sheetData_list.count() < 1) return;

    const QDomNodeList row_list = sheetData_list.at(0).childNodes();

    const int rowCount = row_list.count();
    for(int i = 0; i < rowCount; ++i)
    {
        const QDomNode row = row_list.at(i);
        if(row.nodeName() == tag_row)
        {
            const QString row_str = row.attributes().namedItem(tag_index).nodeValue();

            const QDomNodeList col_list = row.childNodes();
            const size_t colCount = col_list.count();

            for(int j = 0; j < colCount; ++j)
            {
                const QDomNode col = col_list.at(j);

                if(col.nodeName() == tag_col)
                {
                    const QString col_str = col.attributes().namedItem(tag_index).nodeValue();

                    QString value_str;
                    const QDomNodeList value_list = col.firstChildElement(tag_value).childNodes();
                    const int valueCount = value_list.size();

                    for(int k = 0; k < valueCount; ++k)
                    {
                        const QDomNode value = value_list.at(k);
                        if(value.nodeType() == QDomNode::NodeType::TextNode)
                        {
                            value_str += value.nodeValue();
                        }

                        setTableData(row_str, col_str, value_str);
                    }
                }
            }
        }
    }
}

void ExcelDocument::setTableData(const QString &row, const QString& col, const QString &value)
{
    const int rowIndex = row.toInt();
    const int colIndex = alphabetToInt(col.left(col.length() - row.length()));

    if(tableData.size() < rowIndex + 1) tableData.resize(rowIndex + 1);
    if(tableData.at(rowIndex).size() < colIndex + 1) tableData[rowIndex].resize(colIndex + 1);

    tableData[rowIndex][colIndex] = value;
}

QString ExcelDocument::intToAlphabet(int index)
{
    QString result;
    constexpr int alphabetCount = 26;

    do
    {
        const int quo = index / alphabetCount; //商
        const int mod = index % alphabetCount; //余り

        result = QString((char)((int)'A' + mod)) + result;

        index = quo - 1;
    } while(index >= 0);

    return result;
}


int ExcelDocument::alphabetToInt(const QString& alphabet)
{
    const size_t length = alphabet.length();
    int index = 0;

    /* 最小桁以外 */
    for(size_t i = 0; i < length - 1; ++i)
    {
        const QChar c = alphabet.at(i);

        index += ((int)c.toLatin1() - (int)'A' + 1) * std::pow(26, length - i - 1);
    }

    /* 最小桁 */
    index += ((int)alphabet.at(length - 1).toLatin1() - (int)'A');

    return index;
}
