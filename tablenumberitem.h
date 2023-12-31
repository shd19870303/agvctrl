﻿#ifndef TABLENUMBERITEM_H
#define TABLENUMBERITEM_H

#include <QTableWidgetItem>
//未使用
class TableNumberItem : public QTableWidgetItem
{
public:
    TableNumberItem(const QString txt = QString("0"))
        :QTableWidgetItem(txt)
    {
    }

    bool operator < (const QTableWidgetItem &other) const
    {
        QString str1 = text();
        QString str2 = other.text();

//        if (str1[0] == '$' || str1[0] == '€') {
//            str1.remove(0, 1);
//            str2.remove(0, 1); // we assume both items have the same format
//        }

        if (str1[str1.length() - 1] == '%') {
            str1.chop(1);
            str2.chop(1); // this works for "N%" and for "N %" formatted strings
        }

        double f1 = str1.toDouble();
        double f2 = str2.toDouble();

        return str1.toDouble() < str2.toDouble();
    }
};

#endif // TABLENUMBERITEM_H
