#ifndef LIGHTTABLEMODEL_H
#define LIGHTTABLEMODEL_H

#include <QtCore/QAbstractTableModel>
#include "level/level.h"
#include "level/lightingcalculator.h"

class LightTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
explicit LightTableModel(Level* level, QObject* parent = nullptr);

virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
virtual int columnCount(const QModelIndex& parent) const;
virtual int rowCount(const QModelIndex& parent) const;
virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
virtual Qt::ItemFlags flags(const QModelIndex& index) const;
virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private:
	Level* _level;
};

#endif // LIGHTTABLEMODEL_H
