#pragma once
#include <QtWidgets/QMainWindow>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAbstractButton>
#include <string>
#include <iterator>

#include "levelviewer.h"
#include "graphics/textureatlas.h"
#include "level/level.h"
#include "textureview.h"
#include "lighttablemodel.h"
#include "backend/backend.h"

namespace Ui {
	class MainWindow;
	class NewLevelDialog;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget* parent = nullptr);
	virtual ~MainWindow();

public slots:
	void onPickTile();
	void onSetTile();
	void onSetTileFlood();
	void onTileListSelected(QModelIndex index);
	
	void onSaveLoad(QAbstractButton* button);
	void onLevelTextApplyReset(QAbstractButton* button);
	
	void onSettingsGrid(bool draw);
	void onSettingsLighting(bool draw);
	
	void onEntitySave();
	void onEntityDelete();
	
	void onEntitySelected();
	void onEntityMoved();
	
	void onBlockSelected();
	void onCopyTexName();
	
	void onDecalsUpdate();
	
	void onLightAdd();
	void onLightRemove();
	void onCalcLights();
	
	void onNewLevelChooseImage();
protected:
	Block cur_block;
	
private:
	void loadLevel(const std::string& path);
	void saveLevel(const std::string& path);
	void initLevel();
	
	void prepareTiles();
	void prepareEntities();
	
	TextureListModel* tilesModel;
	TextureListModel* entModel;
	
	Ui::MainWindow* ui;
	Ui::NewLevelDialog* uiLevel;
	LevelViewer* levelViewer;
	const TextureAtlas* ents;
	const TextureAtlas* atlas;
	Level* level;
    
	Backend *backend;
};
