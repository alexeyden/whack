#include "lighttablemodel.h"

LightTableModel::LightTableModel(Level* level, QObject* parent): QAbstractTableModel(parent)
{
	_level = level;
}


QVariant LightTableModel::data(const QModelIndex& index, int role) const
{
	const Light& l =  _level->lights()[index.row()];
	
	if(role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();
	
	switch(index.column()) {
		case 0: return l.x;
		case 1: return l.y;
		case 2: return l.z;
		case 3: return l.red;
		case 4: return l.green;
		case 5: return l.blue;
		case 6: return l.intensity;
	}
	
	return QVariant();
}

bool LightTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	Light& l =  _level->lights()[index.row()];
	switch(index.column()) {
		case 0: l.x = value.toFloat(); break;
		case 1: l.y = value.toFloat(); break;
		case 2: l.z = value.toFloat(); break;
		case 3: l.red = value.toUInt(); break;
		case 4: l.green = value.toUInt(); break;
		case 5: l.blue = value.toUInt(); break;
		case 6: l.intensity = value.toFloat();
	}
	
	if(l.intensity == 0.0f) {
		l.type = LT_SUN;
	}
	else {
		l.type = LT_POINT;
	}
	
	emit dataChanged(index, index);
	
	return true;
}

int LightTableModel::columnCount(const QModelIndex& parent) const
{
	return 7;
}

int LightTableModel::rowCount(const QModelIndex& parent) const
{
	return _level->lights().size();
}

QVariant LightTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	
	if(role != Qt::DisplayRole)
		return QVariant();
	
	QString headers[8] = {
		"X", "Y", "Z",
		"R", "G", "B",
		"*"
	};
	
	if(orientation != Qt::Horizontal)
		return QVariant();
	
	if(section < 7)
		return headers[section];
	
	return QVariant();
}

Qt::ItemFlags LightTableModel::flags(const QModelIndex& index) const
{
	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool LightTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);
	
	for(int i = 0; i < count; i++) {
		_level->addLight(Light());
	}
	
	endInsertRows();
	return true;
}

bool LightTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row+count - 1);
	
	for(int i = row; i < row + count; i++) {
		const Light& l = _level->lights()[i];
		_level->removeLight(l);
	}
	
	endRemoveRows();
	return true;
}

