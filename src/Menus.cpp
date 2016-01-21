#include "Menus.h"

Broadcaster<GeneralMsg> Menus::bc;
Scene* Menus::mainMenu = nullptr;
Scene* Menus::saveMenu = nullptr;
Scene* Menus::deleteConfirmMenu = nullptr;
Scene* Menus::renameMenu = nullptr;
Scene* Menus::createNewMenu = nullptr;

GeneralMsg::GeneralMsg(MenuEvent eventType, void* data)
{
    this->eventType = eventType;
    this->data = data;
}

void Menus::initAll()
{
    //Main menu
    mainMenu = new Scene;
    new Label(220, 40, 200, 100, StickyDirs::top, "Magnate", mainMenu);
    new Button(200, 150, 240, 100, StickyDirs::none, "Start Game", Callback(mainStart, nullptr), mainMenu);
    new Button(200, 270, 240, 100, StickyDirs::none, "Quit Game", Callback(mainQuit, nullptr), mainMenu);
    //Save menu
    saveMenu = new Scene;
    
    deleteConfirmMenu = new Scene;
    renameMenu = new Scene;
    createNewMenu = new Scene;
    GUI::init(mainMenu);
}

void Menus::mainStart(void *inst, void *)
{
    //Transition to save menu
    GUI::transition(saveMenu);
}

void Menus::mainQuit(void *inst, void *)
{
    //Signal to quit
    PRINT("Firing quit message.");
    bc.send({MenuEvent::GAME_QUIT, nullptr});
}