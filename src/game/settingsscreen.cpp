#include "settingsscreen.h"

#include "SDL_log.h"

#include "controller/inputmanager.h"
#include "graphics/renderer2d.h"
#include "ui/uimanager.h"
#include "ui/uicheckbox.h"
#include "ui/uiimage.h"
#include "ui/uilabel.h"
#include "ui/uilinearlayout.h"
#include "ui/uibutton.h"

#include "util/util.h"

#include "menuscreen.h"
#include "application.h"

SettingsScreen::SettingsScreen()
: _bgTexture(Resources::i()->atlas("tiles")->textureByName("green_bricks_wall"))
{
	SDL_Log("Settings screen init");
    
	_ui = new UIManager(APP::I()->globalStyle());
	
	UIImage* titleImage = new UIImage(_ui, &Resources::i()->atlas("ui")->textureByName("logo"));
	titleImage->tintColor = 0x00203090;
	
	UILabel* labelSettings = new UILabel(_ui, "Settings");
	labelSettings->paddingY = 10;
	
	UICheckBox* checkboxLimitFPS = new UICheckBox(_ui);
	checkboxLimitFPS->text = "Limit FPS to 30";
	checkboxLimitFPS->checked = APP::I()->limitFPS;
	checkboxLimitFPS->addClickHandler([](bool checked) {
		APP::I()->limitFPS = checked;
	});
	
	UICheckBox* checkboxDebug = new UICheckBox(_ui);
	checkboxDebug->text = "Debug";
	checkboxDebug->checked = APP::I()->debug;
	checkboxDebug->addClickHandler([](bool checked) {
		APP::I()->debug = checked;
	});
	
	UICheckBox* checkboxVsync = new UICheckBox(_ui);
	checkboxVsync->text = "Enable VSync";
	checkboxVsync->checked = APP::I()->vsync();
	checkboxVsync->addClickHandler([](bool checked) {
		APP::I()->setVSync(checked);
	});
	
	UIButton* buttonBack = new UIButton(_ui);
	buttonBack->text = "Back";
	buttonBack->paddingY = 10;
	buttonBack->addClickHandler([this]() {
		APP::I()->setScreen(new MenuScreen());
	});
	
	UILinearLayout* layout = new UILinearLayout(_ui);
	layout->vertical = true;
	layout->paddingX = 30;
	layout->spacing = 5;
	layout->paddingY = 10;
	
	layout->addChild(titleImage);
	layout->addChild(labelSettings);
	layout->addChild(checkboxLimitFPS);
	layout->addChild(checkboxDebug);
	layout->addChild(checkboxVsync);
	layout->addChild(buttonBack);
	
	layout->bounds.x1 = 0;
	layout->bounds.y1 = 0;
	layout->bounds.x2 = Application::I()->backend()->viewWidth() - 1;
	layout->bounds.y2 = Application::I()->backend()->viewHeight() - 1;
	
	layout->arrange();
	
	_ui->root = layout;
	InputManager::addController(_ui);
}

SettingsScreen::~SettingsScreen()
{
	SDL_Log("Settings screen destroy");
    
	InputManager::removeController(_ui);
	delete _ui;
}

void SettingsScreen::render(Texture* out)
{
	Renderer2D p(out);
	p.drawRectScaledTiled(
		Rect<int>(0, 0, APP::I()->backend()->viewWidth() - 1, APP::I()->backend()->viewHeight() - 1),
		_bgTexture,
		4.0f
	);
	
	_ui->root->render(out);
}

void SettingsScreen::update(float dt)
{
	_ui->root->update(dt);
}
