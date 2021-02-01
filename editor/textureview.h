#ifndef TEXTURE_VIEW_H
#define TEXTURE_VIEW_H

#include <QtGui/QStandardItemModel>
#include "graphics/animation.h"
#include "graphics/textureatlas.h"

Q_DECLARE_METATYPE(Texture);
Q_DECLARE_METATYPE(Animation);

class TextureListModel : public QAbstractListModel {
	Q_OBJECT

public:
	enum TextureListItemRole {
		StaticFlagRole = Qt::UserRole + 1,
		TextureRole,
		AnimationRole,
		IdRole
	};
	
public:
    TextureListModel(const TextureAtlas* atlas, QObject* parent = 0);
		
		int rowCount(const QModelIndex& parent = QModelIndex()) const {
			return sizeStatic + sizeAnim;
		}
		
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		QModelIndex indexByName(const QString& name) const;
		
protected:
	const TextureAtlas* atlas;
private:
	int sizeStatic, sizeAnim;
	
	QVector<QPixmap> pixmaps;
};

#endif
