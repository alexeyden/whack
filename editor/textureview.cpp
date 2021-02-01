#include "textureview.h"
#include <QPainter>

TextureListModel::TextureListModel(const TextureAtlas* atlas, QObject* parent): QAbstractListModel(parent)
{
	this->atlas = atlas;
	sizeStatic = atlas->textureNameList().size();
	sizeAnim = atlas->animationList().size();
	
	QImage img(atlas->asTexture().data(),
					atlas->asTexture().width(), atlas->asTexture().height(),
					atlas->asTexture().dataWidth() * 3, QImage::Format_RGB888
				);
	img = img.rgbSwapped();

	QPixmap pixmap = QPixmap::fromImage(img);
	
	for(auto img : atlas->textureNameList()) {
		Texture t = atlas->texture(img.second);
		QPixmap ico = pixmap.copy(t.x(), t.dataHeight() - t.y() - t.height(), t.width(), t.height());
		ico = ico.transformed(QTransform().scale(1, -1));
		pixmaps.append(ico);
	}
	
	for(auto img : atlas->animationList()) {
		Texture t = atlas->texture(img.second[AnimationType::IDLE].startFrame[0]);
		QPixmap ico = pixmap.copy(t.x(), t.dataHeight() - t.y() - t.height(), t.width(), t.height());
		ico = ico.transformed(QTransform().scale(1, -1));
		QPainter p;
		p.begin(&ico);
		p.setBrush(Qt::red);
		p.drawEllipse(0, 0, 8, 8);
		p.end();
		pixmaps.append(ico);
	}
}

QVariant TextureListModel::data(const QModelIndex& index, int role) const
{
	switch(role) {
		case Qt::DisplayRole:
		{
			if(index.row() > sizeStatic - 1)
			{
				auto iter = atlas->animationList().begin();
				std::advance(iter, index.row() - sizeStatic);
				return QString::fromStdString(iter->first);
			}
			
			auto iter = atlas->textureNameList().begin();
			std::advance(iter, index.row());
			return QString::fromStdString(iter->first);
		} break;
		case Qt::DecorationRole:
		{
			return pixmaps[index.row()];
		} break;
		case StaticFlagRole:
		{
			return index.row() < sizeStatic;
		} break;
		case TextureRole: 
		{
			auto iter = atlas->textureNameList().begin();
			std::advance(iter, index.row());
			
			if(index.row() < sizeStatic)
				return QVariant::fromValue(atlas->textureByName(iter->first)); 
			
			return QVariant();
		} break;
		case AnimationRole: {
			if(index.row() >= sizeStatic) {
				auto iter = atlas->animationList().begin();
				std::advance(iter, index.row() - sizeStatic);
				return QVariant::fromValue(atlas->animation(iter->first, AnimationType::IDLE)); 
			}
			
			return QVariant();
		} break;
		case IdRole: {
			auto iter = atlas->textureNameList().begin();
			std::advance(iter, index.row());
			if(index.row() < sizeStatic)
				return atlas->textureID(iter->first);
		} break;
		default: return QVariant();
	}
}

QModelIndex TextureListModel::indexByName(const QString& name) const
{
	auto st_iter = atlas->textureNameList().find(name.toStdString()); 
	
	if(st_iter != atlas->textureNameList().end())
		return this->index(std::distance(atlas->textureNameList().begin(), st_iter));
	
	auto iter = atlas->animationList().find(name.toStdString());
	return this->index(std::distance(atlas->animationList().begin(), iter) + atlas->textureNameList().size());
}