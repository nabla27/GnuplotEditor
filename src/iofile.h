#ifndef IOFILE_H
#define IOFILE_H


#include <QString>
#include <QFile>
#include <QTextStream>

inline void toFileTxt(const QString& fileName, const QString& data, bool* ok = nullptr)
{
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << data;
        file.close();
        if(ok) *ok = true;
    }
    else
        if(ok) *ok = false;
}

inline void toFileCsv(const QString& filename, const QList<QList<QString> >& sheet, bool *ok = nullptr)
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

    toFileTxt(filename, data, ok);
}

inline void toFileTsv(const QString& filename, const QList<QList<QString> >& sheet, bool *ok = nullptr)
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

    toFileTxt(filename, data, ok);
}

inline QString readFileTxt(const QString& fileName, bool *ok = nullptr)
{
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        const QString data = in.readAll();
        if(ok) *ok = true;

        return data;
    }
    else
    {
        if(ok) *ok = false;
        return QString();
    }
}

inline void readFileTxt(const QString& fileName, QString& text, bool *ok = nullptr)
{
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        text = in.readAll();

        if(ok) *ok = true;
    }
    else
        if(ok) *ok = false;
}

inline QList<QList<QString> > readFileCsv(const QString& fileName, bool *ok = nullptr)
{
    const QString data = readFileTxt(fileName, ok);  //ファイルの内容(text)をQString型で保持

    /* okがnullptrじゃなく、falseの場合 または 空文字だった場合 */
    if((ok && !*ok) || data.isEmpty())
    {
        return QList<QList<QString> >();
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

        if(ok) *ok = true;
        return sheet;
    }
}

inline QList<QList<QString> > readFileTsv(const QString& fileName, bool *ok = nullptr)
{
    const QString data = readFileTxt(fileName, ok);

    if((ok && !*ok) || data.isEmpty())
    {
        return QList<QList<QString> >();
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

        if(ok) *ok = true;
        return sheet;
    }
}



















#endif // IOFILE_H
