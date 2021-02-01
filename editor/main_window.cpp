#include "main_window.h"
#include "ui_main_window.h"
#include "ui_new_level.h"

#include <QtGui>
#include <QtGui/QStandardItemModel>
#include <QtGui/QPixmap>
#include <QtGui/QClipboard>
#include <QtWidgets/QFileDialog>

#include <fstream>
#include "graphics/textureatlasreader.h"
#include "graphics/textureatlas.h"
#include "level/levelloader.h"
#include "level/entityvisualinfoextractor.h"
#include "level/lightingcalculator.h"
#include "util/util.h"
#include "resources/resources.h"
#include "game/application.h"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);
    
    backend = new Backend("Whack", 640, 480);
    
    Resources::init(backend);
    
    Resources::i()->loadAtlas("textures/font.bmp", "font");
	Resources::i()->loadAtlas("textures/entities.json", "entities");
	Resources::i()->loadAtlas("textures/ui.json", "ui");
	Resources::i()->loadAtlas("textures/tiles.json", "tiles");
	Resources::i()->loadAtlas("textures/skydomes.json", "skydomes");
	
	atlas = Resources::i()->atlas("tiles");
	ents = Resources::i()->atlas("entities");

	Block proto(1.0f, atlas->textureID("dummy"));
	level = new Level(64, 64, proto, backend);
	
	QSurfaceFormat format;
	format.setMajorVersion(3);
	format.setMinorVersion(2);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setOption(QSurfaceFormat::DebugContext);
	QSurfaceFormat::setDefaultFormat(format);
	
	levelViewer = new LevelViewer(level, atlas, Resources::i()->atlas("entities"), this);
	
	this->setCentralWidget(levelViewer);
	
	connect(levelViewer, SIGNAL(entitySelected()), SLOT(onEntitySelected()));
	connect(levelViewer, SIGNAL(entityMoved()), SLOT(onEntityMoved()));
	connect(levelViewer, SIGNAL(blockSelected()), SLOT(onBlockSelected()));
	ui->tile_list->addAction(ui->texCopy);
	
	
	prepareTiles();
	prepareEntities();
	
	initLevel();
}

MainWindow::~MainWindow()
{
	delete ui;
	delete atlas;
}

void MainWindow::loadLevel(const std::string& path)
{
	delete level;
	
	std::fstream level_file(path, std::ios_base::binary | std::ios_base::in);
	LevelLoader loader(level_file, atlas, ents, backend);
	loader.emptyBehaviuour = true;
	level = loader.load();
	level_file.close();
	
	levelViewer->setLevel(level); 

	initLevel();
}

void MainWindow::saveLevel(const std::string& path)
{
	std::fstream level_file(path, std::ios_base::binary | std::ios_base::out);
	LevelLoader loader(level_file, atlas, ents, backend);
	loader.save(level);
	level_file.close();
}

void MainWindow::initLevel()
{
	level->enablePhysics = false;
	
	ui->level_name->setText(QString::fromStdString(level->name));
	ui->level_text->setPlainText(QString::fromStdString(level->description));
	
	ui->player_x->setValue(level->player()->x());
	ui->player_y->setValue(level->player()->y());
	ui->player_z->setValue(level->player()->z());
	ui->player_dir->setValue(level->player()->dir() * 180/ M_PI);

	/*	
	Decoration* t1 = new Decoration(Resources::i()->atlas("entities"), "tree", 15, 15, 1);
	t1->visualHeight(4); t1->visualWidth(3);
	
	Decoration* t2 = new Decoration(Resources::i()->atlas("entities"), "tree_dark", 12, 17, 1);
	t2->visualHeight(4); t2->visualWidth(3);
	
	level->addEntity(t1);
	level->addEntity(t2);
	*/
	
	
	auto tm = new LightTableModel(level, this);
	ui->lightList->setModel(tm);
}


void MainWindow::onSaveLoad(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton b = ui->level_save_load_buttons->standardButton(button);
	if(b == QDialogButtonBox::Open) {
		QFileDialog open(this);
		open.setFileMode(QFileDialog::ExistingFile);
		open.setNameFilter("Level files (*.level)");
		int res = open.exec();
		if(res) loadLevel(open.selectedFiles().first().toStdString());
	}
	else if(b == QDialogButtonBox::Save){
		QFileDialog save(this);
		save.setFileMode(QFileDialog::AnyFile);
		save.setNameFilter("Level files (*.level)");
		save.setAcceptMode(QFileDialog::AcceptSave);
		int res = save.exec();
		if(res) saveLevel(save.selectedFiles().first().toStdString());
	}
	else if(b == QDialogButtonBox::Reset) {
		uiLevel = new Ui::NewLevelDialog;
		
		QDialog dialog(this);
		uiLevel->setupUi(&dialog);
		
		QObject::connect(uiLevel->newLevelChooseImg, SIGNAL(clicked(bool)), this, SLOT(onNewLevelChooseImage()));
		
		if(dialog.exec() == QDialog::Accepted) {
			delete level;
			
			Block proto(1.0f, atlas->textureID("dummy"));
			level = new Level(uiLevel->level_w->value(), uiLevel->level_h->value(), proto, backend);
			level->name = uiLevel->level_name->text().toStdString();
			level->description = uiLevel->level_descr->toPlainText().toStdString();
			
			if(uiLevel->newLevelImg->pixmap() != nullptr) {
				auto p =uiLevel->newLevelImg->pixmap();
				auto i = p->toImage();
				for(int x =0; x < level->sizeX(); x++) {
					for(int y = 0; y < level->sizeY(); y++) {
						auto c = i.pixel(x,y); 
						level->block(x, y).height = qRed(c) * 0.1;
					}
				}
			}
	
			levelViewer->setLevel(level); 
			initLevel();
		}
	}
}

void MainWindow::onLevelTextApplyReset(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton b = ui->level_buttons->standardButton(button);
	if(b == QDialogButtonBox::Apply) {
		level->name = ui->level_name->text().toStdString();
		level->description= ui->level_text->toPlainText().toStdString();
		level->player()->x(ui->player_x->value());
		level->player()->y(ui->player_y->value());
		level->player()->z(ui->player_z->value());
		level->player()->dir(ui->player_dir->value() * M_PI / 180.0f);
	}
	else {
		ui->level_name->setText(QString::fromStdString(level->name));
		ui->level_text->setPlainText(QString::fromStdString(level->description));
		ui->player_x->setValue(level->player()->x());
		ui->player_y->setValue(level->player()->y());
		ui->player_z->setValue(level->player()->z());
		ui->player_dir->setValue(level->player()->dir() * 180.0f / M_PI);
	}
}

void MainWindow::onPickTile()
{
	auto p = levelViewer->selectionStart;
	if(p.x() >= 0 && p.y() >= 0) {
		const Block& block = level->block(p.x(), p.y());
		
		QString tex_name_s = block.isAnimated(BT_SOUTH) ?
			atlas->animationName(*block.animation(BT_SOUTH), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_SOUTH)).c_str();
		QString tex_name_n = block.isAnimated(BT_NORTH) ?
			atlas->animationName(*block.animation(BT_NORTH), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_NORTH)).c_str();
		QString tex_name_w = block.isAnimated(BT_WEST) ?
			atlas->animationName(*block.animation(BT_WEST), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_WEST)).c_str();
		QString tex_name_e = block.isAnimated(BT_EAST) ?
			atlas->animationName(*block.animation(BT_EAST), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_EAST)).c_str();
		
		ui->tile_wall_south->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_s), Qt::DecorationRole).value<QPixmap>());
		ui->tile_wall_north->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_n), Qt::DecorationRole).value<QPixmap>());
		ui->tile_wall_west->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_w), Qt::DecorationRole).value<QPixmap>());
		ui->tile_wall_east->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_e), Qt::DecorationRole).value<QPixmap>());
		
		QString tex_name_f = block.isAnimated(BT_FLOOR) ?
			atlas->animationName(*block.animation(BT_FLOOR), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_FLOOR)).c_str();
		QString tex_name_c = block.isAnimated(BT_CEILING) ?
			atlas->animationName(*block.animation(BT_CEILING), AnimationType::IDLE).c_str() :
			atlas->textureName(block.texture(BT_CEILING)).c_str();
	
		ui->tile_wall_floor->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_f), Qt::DecorationRole).value<QPixmap>());
		ui->tile_wall_ceiling->setIcon(tilesModel->data(tilesModel->indexByName(tex_name_c), Qt::DecorationRole).value<QPixmap>());
		
		cur_block = block;
		
		ui->tile_height->setValue(block.height);
		ui->tile_notch->setValue(block.notch);
		ui->tile_notch_height->setValue(block.notchHeight);
		ui->tile_info->setText(
			QString::number(levelViewer->selectionStart.x()) + "," + 
			QString::number(levelViewer->selectionStart.y()) + " x " + 
			QString::number(levelViewer->selectionEnd.x()) + "," + 
			QString::number(levelViewer->selectionEnd.y()) 
		);
		
		std::map<BlockTex, QString> sides = {
			std::make_pair(BT_CEILING, "c"),
			std::make_pair(BT_FLOOR  , "f"),
			std::make_pair(BT_WEST   , "w"),
			std::make_pair(BT_EAST   , "e"),
			std::make_pair(BT_NORTH  , "n"),
			std::make_pair(BT_SOUTH  , "s")
		};
		ui->decalsText->setPlainText("");
		QString s;
		for(int i = 0; i < 6; i++) {
			for(const auto& d : block.decals((BlockTex) i)) {
				s.append(sides[(BlockTex) i] + " ");
				s.append(QString::number(d.offsetX) + " ");
				s.append(QString::number(d.offsetY) + " ");
				s.append(QString::number(d.sizeX) + " ");
				s.append(QString::number(d.sizeY) + " ");
				s.append(QString::fromStdString(atlas->textureName(d.texture)));
				if(d.blend)
					s.append(" blend");
				s.append("\n");
			}
		}
		s.remove(s.length() - 1, 1);
		ui->decalsText->setPlainText(s);
	}
}

void MainWindow::onSetTile()
{
	cur_block.height = ui->tile_height->value();
	cur_block.notch = ui->tile_notch->value();
	cur_block.notchHeight = ui->tile_notch_height->value();
	
	auto p = levelViewer->selectionStart;
	auto p2 = levelViewer->selectionEnd;
	if(p.x() >= 0 && p.y() >= 0) {
		for(int x = qMin(p.x(), p2.x()); x <= qMax(p.x(), p2.x()); x++) {
			for(int y = qMin(p.y(), p2.y()); y <= qMax(p.y(), p2.y()); y++) {
				Block& block = level->block(x,y);
				Block bp = cur_block;
				if(!ui->tileHeights->isChecked()) {
					bp.height = block.height;
					bp.notch = block.notch;
					bp.notchHeight = block.notchHeight;
				}
				block = bp;
			}
		}
	}
	
	levelViewer->setLevel(level);
}

void MainWindow::onEntitySelected()
{
	VisualEntity* e = levelViewer->selectedEntity;
	
	if(!e) {
		return;
	}
	
	ui->ent_x->setValue(e->x());
	ui->ent_y->setValue(e->y());
	ui->ent_z->setValue(e->z());
	
	ui->ent_width->setValue(e->width());
	ui->ent_height->setValue(e->height());
	
	ui->ent_vis_width->setValue(e->visualWidth());
	ui->ent_vis_height->setValue(e->visualHeight());
	
	EntityVisualInfoExtractor ext;
	EntityVisualInfoExtractor::EntityVisualInfo info = ext.extract(e);
	
	ui->ent_type->setCurrentIndex(info.type - 1);
	
	if(info.texture.size() > 0) {
		auto index = ((TextureListModel*) ui->ent_list->model())->indexByName(QString::fromStdString(info.texture));
		ui->ent_list->setCurrentIndex(index);
	}
}

void MainWindow::onEntityMoved()
{
	VisualEntity* e = levelViewer->selectedEntity;
	
	if(!e)
		return;
	
	ui->ent_x->setValue(e->x());
	ui->ent_y->setValue(e->y());
	ui->ent_z->setValue(e->z());
}

void MainWindow::onTileListSelected(QModelIndex index)
{
	const auto& icon = tilesModel->data(index, Qt::DecorationRole).value<QPixmap>();
	
	if(ui->tile_wall_south->isChecked()) {
		ui->tile_wall_south->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_SOUTH, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_SOUTH, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
	
	if(ui->tile_wall_north->isChecked()) {
		ui->tile_wall_north->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_NORTH, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_NORTH, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
	
	if(ui->tile_wall_west->isChecked()) {
		ui->tile_wall_west->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_WEST, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_WEST, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
	
	if(ui->tile_wall_east->isChecked()) {
		ui->tile_wall_east->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_EAST, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_EAST, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
		
	if(ui->tile_wall_floor->isChecked()) {
		ui->tile_wall_floor->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_FLOOR, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_FLOOR, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
	
	if(ui->tile_wall_ceiling->isChecked()) {
		ui->tile_wall_ceiling->setIcon(icon);
		
		if(tilesModel->data(index, TextureListModel::StaticFlagRole).toBool())
			cur_block.texture(BT_CEILING, tilesModel->data(index, TextureListModel::IdRole).toUInt());
		else
			cur_block.texture(BT_CEILING, tilesModel->data(index, TextureListModel::AnimationRole).value<Animation>());
	}
}

void MainWindow::onSettingsGrid(bool draw)
{
	levelViewer->drawGrid(draw);
}

void MainWindow::onSettingsLighting(bool draw)
{
	levelViewer->useLight(draw);
}

void MainWindow::prepareTiles()
{
	TextureListModel* model = new TextureListModel(atlas, ui->tile_list);
	ui->tile_list->setModel(model);
	
	tilesModel = model;
}

void MainWindow::prepareEntities()
{
	TextureListModel* model = new TextureListModel(ents, ui->ent_list);
	ui->ent_list->setModel(model);
	
	entModel = model;
}

void MainWindow::onEntitySave()
{
	VisualEntity* e = levelViewer->selectedEntity;
	
	if(e != nullptr) {
		level->removeEntity(e);
		delete e;
	}
	
	EntityFactory fac(ents);
	fac.emptyBehaviour = true;
	
	auto tex = ((TextureListModel*) ui->ent_list->model())->data(ui->ent_list->currentIndex(), Qt::DisplayRole).toString().toStdString();
	e = fac.create((EntityFactory::EntityType) (ui->ent_type->currentIndex()  + 1), tex);
	
	e->x(ui->ent_x->value());
	e->y(ui->ent_y->value());
	e->z(ui->ent_z->value());
	
	e->width(ui->ent_width->value());
	e->height(ui->ent_height->value());
	e->visualWidth(ui->ent_vis_width->value());
	e->visualHeight(ui->ent_vis_height->value());
	
	level->addEntity(e);
	levelViewer->selectedEntity = e;
}

void MainWindow::onEntityDelete()
{
	if(levelViewer->selectedEntity) {
		auto e = levelViewer->selectedEntity;
		level->removeEntity(e);
		delete e;
		levelViewer->selectedEntity = nullptr;
	}
}

void MainWindow::onBlockSelected()
{
	if(levelViewer->selectedEntity == nullptr) {
		ui->ent_x->setValue(levelViewer->selectionStart.x() + 0.5f);
		ui->ent_y->setValue(levelViewer->selectionStart.y() + 0.5f);
	}
}

void MainWindow::onSetTileFlood()
{
	const Block& ref = level->block(levelViewer->selectionStart.x(), levelViewer->selectionStart.y());
	
	QSet<Block*> blocks;
	QVector<QPoint> nb;
	
	nb.append(levelViewer->selectionStart);
	blocks.insert(&level->block(levelViewer->selectionStart.x(), levelViewer->selectionStart.y()));
	
	QPoint ds[8] = {
		QPoint(-1, 0),
		QPoint(+1, 0),
		QPoint(0, +1),
		QPoint(0, -1),
		
		QPoint(-1, +1),
		QPoint(-1, -1),
		QPoint(+1, -1),
		QPoint(+1, +1)
		
	};
	
	cur_block.height = ui->tile_height->value();
	cur_block.notch = ui->tile_notch->value();
	cur_block.notchHeight = ui->tile_notch_height->value();
	
	while(nb.size() > 0) {
		QPoint p = nb.front();
		nb.pop_front();
		
		for(auto d : ds) {
			auto np = p + d;
			bool in_bound = np.x() >= 0 && np.y() >= 0 && np.x() < level->sizeX() && np.y() < level->sizeY(); 
			
			if(in_bound) {	
				auto& b = level->block(np.x(), np.y());
				
				if(!blocks.contains(&b) &&
					b.height == ref.height &&
					b.notch == ref.notch &&
					b.notchHeight == ref.notchHeight) {
					
					nb.push_back(np);
					blocks.insert(&b);
				}
			}
		}
	}
	
	for(auto b : blocks) {
		Block bp = cur_block;
		if(!ui->tileHeights->isChecked()) {
			bp.height = b->height;
			bp.notch = b->notch;
			bp.notchHeight = b->notchHeight;
		}
		*b = bp;
	}
	
	
	levelViewer->setLevel(level);
}

void MainWindow::onDecalsUpdate()
{
	const auto& text = ui->decalsText->toPlainText();
	std::vector<BlockDecal> decals[6];
	std::map<QString, BlockTex> sides = {
		std::make_pair("c", BT_CEILING),
		std::make_pair("f", BT_FLOOR),
		std::make_pair("w", BT_WEST),
		std::make_pair("e", BT_EAST),
		std::make_pair("n", BT_NORTH),
		std::make_pair("s", BT_SOUTH)
	};
	
	for(const auto& line :  text.split("\n")) {
		if(line == "")
			continue;
		const auto& parts = line.split(" ");
		BlockTex side = sides[parts[0]];
		BlockDecal decal; 
		decal.offsetX = parts[1].toFloat();
		decal.offsetY = parts[2].toFloat();
		decal.sizeX = parts[3].toFloat();
		decal.sizeY = parts[4].toFloat();
		decal.texture = atlas->textureID(parts[5].toStdString()); 
		if(parts.size() == 7) {
			decal.blend = true;
		}
		else {
			decal.blend = false;
		}
		decals[side].push_back(decal);
	}
	
	for(int i = 0; i < 6; i++) {
		cur_block.decals((BlockTex) i) = decals[i];
	}
}

void MainWindow::onNewLevelChooseImage()
{
 auto path = QFileDialog::getOpenFileName(this);
 
 QImage i(path);
 uiLevel->newLevelImg->setPixmap(QPixmap::fromImage(i));
 
	uiLevel->level_w->setValue(uiLevel->newLevelImg->pixmap()->width());
	uiLevel->level_h->setValue(uiLevel->newLevelImg->pixmap()->height());
}

void MainWindow::onCopyTexName()
{
	const auto& text = tilesModel->data(ui->tile_list->currentIndex(), Qt::DisplayRole).value<QString>();
	QApplication::clipboard()->setText(text);
}

void MainWindow::onLightAdd()
{
	Light l;
	l.x = levelViewer->selectionStart.x() + 0.5;
	l.y = levelViewer->selectionStart.y() + 0.5;
	l.z = level->block(l.x, l.y).standHeight() + 1.0f;
	
	auto model = ui->lightList->model();
	ui->lightList->model()->insertRow(ui->lightList->model()->rowCount());
	
	ui->lightList->model()->setData(model->index(model->rowCount()-1, 0), l.x);
	ui->lightList->model()->setData(model->index(model->rowCount()-1, 1), l.y);
	ui->lightList->model()->setData(model->index(model->rowCount()-1, 2), l.z);
}

void MainWindow::onLightRemove()
{
	auto model = ui->lightList->model();
	auto index =  ui->lightList->selectionModel()->currentIndex();
	
	model->removeRow(index.row());
}

void MainWindow::onCalcLights()
{
	level->updateLights();
}
