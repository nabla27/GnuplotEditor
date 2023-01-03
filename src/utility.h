#ifndef UTILITY_H
#define UTILITY_H

#include <QMetaEnum>
#include <QRect>


namespace mutility
{




template <typename QEnum>
QString enumToString(const QEnum value)
{
    return QString(QMetaEnum::fromType<QEnum>().valueToKey((int)value));
}

template <class QEnum>
QStringList enumToStrings(const QEnum)
{
    QStringList enumStrList;
    const int enumCount = QMetaEnum::fromType<QEnum>().keyCount();
    for(int i = 0; i < enumCount; ++i){
        enumStrList << QStringList(QMetaEnum::fromType<QEnum>().valueToKey(i));
    }

    return enumStrList;
}

//template <class QEnum>
//QStringList enumToStrings(const QEnum)
//{
//    QStringList enumStrList;
//    const int enumCount = QMetaEnum::fromType<QEnum>().keyCount();
//    for(int i = 0, count = 0; count < enumCount; ++i)
//    {
//        const QString str = QMetaEnum::fromType<QEnum>().valueToKey(i);
//        if(!str.isEmpty())
//        {
//            enumStrList << str;
//            count++;
//        }
//    }
//
//    return enumStrList;
//}

template <class QEnum>
int getEnumCount(const QEnum)
{
    return QMetaEnum::fromType<QEnum>().keyCount();
}

template <class QEnum>
int getEnumIndex(const QString enumStr)
{
    const int enumCount = getEnumCount(QEnum(0));
    for(int i = 0; i < enumCount; ++i){
        if(QMetaEnum::fromType<QEnum>().valueToKey(i) == enumStr) { return i; }
    }

    return -1;
}


inline const QRect getRectFromScreenRatio(const QSize& screenSize, const float& rw, const float& rh)
{
    const float px = (1.0f - rw) * screenSize.width() / 2.0f;
    const float py = (1.0f - rh) * screenSize.height() / 2.0f;
    return QRect(px, py, screenSize.width() * rw, screenSize.height() * rh);
}





}//namespace mutility



#endif // UTILITY_H
