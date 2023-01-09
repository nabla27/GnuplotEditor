/*!
 * GnuplotEditor
 *
 * Copyright (c) 2022 yuya
 *
 * This software is released under the GPLv3.
 * see https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#ifndef TEXTCODEC_H
#define TEXTCODEC_H

#include <QTextCodec>
#include <QObject>

class TextCodec : public QObject
{
    Q_OBJECT
public:
    enum class CharCode
    {
        Shift_JIS,
        EUC_JP,
        JIS,
        Utf_8,
        Utf_16LE,
        Utf_16BE,
    };
    Q_ENUM(CharCode)

    static QString QStringFrom(const QByteArray& array, const CharCode& code)
    {
        QTextCodec *codec = nullptr;

        switch(code)
        {
        case CharCode::Shift_JIS: codec = shiftJisCodec; break;
        case CharCode::EUC_JP: codec = eucJpCodec; break;
        case CharCode::JIS: codec = jisCodec; break;
        case CharCode::Utf_8: codec = utf8Codec; break;
        case CharCode::Utf_16LE: codec = utf16LeCodec; break;
        case CharCode::Utf_16BE: codec = utf16BeCodec; break;
        default: break;
        }

        if(codec)
            return codec->toUnicode(array);
        else
            return array;
    }

private:
    inline static QTextCodec *const shiftJisCodec = QTextCodec::codecForName("Shift-JIS");
    inline static QTextCodec *const eucJpCodec = QTextCodec::codecForName("EUC-JP");
    inline static QTextCodec *const jisCodec = QTextCodec::codecForName("JIS");
    inline static QTextCodec *const utf8Codec = QTextCodec::codecForName("UTF-8");
    inline static QTextCodec *const utf16LeCodec = QTextCodec::codecForName("UTF-16LE");
    inline static QTextCodec *const utf16BeCodec = QTextCodec::codecForName("UTF-16BE");
};

#endif // TEXTCODEC_H
