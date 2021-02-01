#include "menuscreen.h"

#include "SDL_log.h"

#include "controller/inputmanager.h"

#include "graphics/texture.h"
#include "graphics/textureatlas.h"

#include "ui/uilinearlayout.h"
#include "ui/uilabel.h"
#include "ui/uibutton.h"
#include "ui/uiimage.h"

#include "util/util.h"

#include "application.h"
#include "settingsscreen.h"
#include "gamescreen.h"


MenuScreen::MenuScreen()
: _bgTexture(Resources::i()->atlas("tiles")->textureByName("green_bricks_wall"))
{
	SDL_Log("Initializing main menu screen");
    
	_ui = new UIManager(APP::I()->globalStyle());

	UIImage* titleImage = new UIImage(_ui, &Resources::i()->atlas("ui")->textureByName("logo"));
	titleImage->tintColor = 0x00203090;
	
	UIButton* buttonPlay = new UIButton(_ui);
	buttonPlay->text = "New game";
	buttonPlay->paddingY = 3;
	buttonPlay->addClickHandler([this]() {
		APP::I()->setScreen(new GameScreen());
	});
	
	UIButton* buttonSettings = new UIButton(_ui);
	buttonSettings->text = "Settings";
	buttonSettings->paddingY = 3;
	buttonSettings->addClickHandler([this]() {
		APP::I()->setScreen(new SettingsScreen());
	});
	
	UIButton* buttonExit = new UIButton(_ui);
	buttonExit->text = "Exit";
	buttonExit->paddingY = 3;
	buttonExit->addClickHandler([]() {
		APP::I()->quit();
	});
	
	UILinearLayout* layout = new UILinearLayout(_ui);
	layout->vertical = true;
	layout->paddingX = 30;
	layout->spacing = 5;
	layout->paddingY = 10;
	
	layout->addChild(titleImage);
	layout->addChild(buttonPlay);
	layout->addChild(buttonSettings);
	layout->addChild(buttonExit);
	
	layout->bounds.x1 = 0;
	layout->bounds.y1 = 0;
	layout->bounds.x2 = Application::I()->backend()->viewWidth() - 1;
	layout->bounds.y2 = Application::I()->backend()->viewHeight() - 1;
	
	layout->arrange();
	
	_ui->root = layout;
    
	InputManager::addController(_ui);
}

MenuScreen::~MenuScreen()
{
	SDL_Log("Destroying main menu screen");
    
	InputManager::removeController(_ui);
	delete _ui;
}

void MenuScreen::render(Texture* out) 
{
	Renderer2D p(out);
	p.drawRectScaledTiled(
		Rect<int>(0, 0, APP::I()->backend()->viewWidth()-1, APP::I()->backend()->viewHeight()-1),
		_bgTexture,
		4.0f
	);
	_ui->root->render(out);
}

void MenuScreen::update(float dt)
{
	_ui->root->update(dt);
}
