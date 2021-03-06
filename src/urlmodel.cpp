/*
* Copyright (c) 2015 CuteRest
* CuteRest, a json REST viewer [https://github.com/dridk/cuterest]
* This file is part of CuteRest Project.
* CuteRest is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "urlmodel.h"

UrlModel::UrlModel(QObject * parent):
    QAbstractListModel(parent)
{
    load();
}

UrlModel::~UrlModel()
{
    save();
}

int UrlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mUrls.count();

}

QVariant UrlModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if (role == Qt::DisplayRole)
        return mUrls.at(index.row());


    return QVariant();

}

void UrlModel::save()
{
    QSettings settings;
    settings.beginWriteArray("urls");
    for (int i=0; i<mUrls.count(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("url", mUrls.at(i));
    }

    settings.endArray();

}

void UrlModel::load()
{
    QSettings settings;
    int size = settings.beginReadArray("urls");
    for (int i=0; i<size; ++i)
    {
        settings.setArrayIndex(i);
        append(settings.value("url").toUrl());
    }

    settings.endArray();

}

void UrlModel::append(const QUrl &url)
{
    if (mUrls.contains(url))
        return;

    beginInsertRows(QModelIndex(),0,0);
    mUrls.append(url);
    endInsertRows();


}

