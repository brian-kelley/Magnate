#include "Menus.h"

using namespace std;

Broadcaster<GeneralMsg> Menus::bc;
Scene* Menus::debugScene = nullptr;
Label* Menus::fpsLabel = nullptr;
Scene* Menus::mainMenu = nullptr;
Scene* Menus::saveMenu = nullptr;
ScrollBlock* Menus::saveScroll = nullptr;
MultiSelect* Menus::saveSelect = nullptr;
Scene* Menus::deleteConfirmMenu = nullptr;
Label* Menus::deleteConfirmLabel = nullptr;
Scene* Menus::renameMenu = nullptr;
Field* Menus::renamingField = nullptr;
Scene* Menus::createMenu = nullptr;
Field* Menus::creatingField = nullptr;
Scene* Menus::gameScene = nullptr;

GeneralMsg::GeneralMsg(MenuEvent eventType, void* data)
{
    this->eventType = eventType;
    this->data = data;
}

void Menus::initAll()
{
    //Debug scene
    debugScene = new Scene;
    fpsLabel = new Label(10, 10, 100, 35, StickyDirs::left | StickyDirs::top | StickyDirs::fixedWidth | StickyDirs::fixedHeight, "", debugScene);
    //Main menu
    mainMenu = new Scene;
    new Label(220, 40, 200, 100, StickyDirs::top, "Magnate", mainMenu);
    new Button(200, 150, 240, 100, StickyDirs::none, "Start Game", Callback(mainStart, nullptr), mainMenu);
    new Button(200, 270, 240, 100, StickyDirs::none, "Quit Game", Callback(mainQuit), mainMenu);
    //Save menu
    saveMenu = new Scene;
    new Label(220, 20, 200, 60, StickyDirs::none, "Select a save file", saveMenu);
    new Button(9, 400, 110, 60, StickyDirs::none, "Back", Callback(saveToMain), saveMenu);
    new Button(137, 400, 110, 60, StickyDirs::none, "Rename", Callback(saveToRename), saveMenu);
    new Button(265, 400, 110, 60, StickyDirs::none, "Create", Callback(saveToCreate), saveMenu);
    new Button(393, 400, 110, 60, StickyDirs::none, "Delete", Callback(saveToDelete), saveMenu);
    new Button(521, 400, 110, 60, StickyDirs::none, "Load", Callback(saveToGame), saveMenu);
    saveScroll = new ScrollBlock(45, 70, 550, 300, StickyDirs::none, saveMenu, 1);
    saveSelect = new MultiSelect(5, 5, saveScroll->getScreenRect().w - 3 * 5 - 6, 280, StickyDirs::none, 50, saveScroll, false);
    deleteConfirmMenu = new Scene;
    deleteConfirmLabel = new Label(20, 20, 600, 80, StickyDirs::top | StickyDirs::left | StickyDirs::right, "", deleteConfirmMenu);
    new Button(400, 300, 100, 50, StickyDirs::none, "OK", deleteOK, deleteConfirmMenu);
    new Button(240, 300, 100, 50, StickyDirs::none, "Cancel", deleteCancel, deleteConfirmMenu);
    new Label(320, 100, 630, 50, StickyDirs::none, "Are you sure you want to delete", deleteConfirmMenu);
    renameMenu = new Scene;
    renamingField = new Field(320, 100, 600, 50, StickyDirs::none, "", Callback(nullptr), renameMenu);
    new Button(250, 250, 100, 50, StickyDirs::none, "OK", Callback(renameOK), renameMenu);
    new Button(390, 250, 100, 50, StickyDirs::none, "Cancel", Callback(renameCancel), renameMenu);
    createMenu = new Scene;
    new Button(250, 250, 100, 50, StickyDirs::none, "OK", Callback(createOK), createMenu);
    new Button(390, 250, 100, 50, StickyDirs::none, "Cancel", Callback(createCancel), createMenu);
    creatingField = new Field(320, 100, 600, 50, StickyDirs::none, "", nullptr, createMenu);
    gameScene = new Scene;
    new Minimap(gameScene);
    GUI::init(mainMenu, debugScene);
}

// Main menu callbacks
void Menus::mainStart(void *inst, void *)
{
    //Prepare save manager
    updateSaveList();
    //Transition to save menu
    GUI::transition(saveMenu);
}

void Menus::mainQuit(void *inst, void *)
{
    //Signal to quit
    bc.send({MenuEvent::GAME_QUIT, nullptr});
}

// Save menu callbacks
void Menus::saveToMain(void *inst, void *)
{
    GUI::transition(mainMenu);
}

void Menus::saveToRename(void* inst, void* arg)
{
    if(saveSelect->getSelection() != -1)
    {
        renamingField->setText(saveSelect->getSelectionText());
        GUI::transition(renameMenu);
    }
}

void Menus::renameCancel(void* inst, void *arg)
{
    updateSaveList();
    GUI::transition(saveMenu);
}

void Menus::renameOK(void* inst, void* arg)
{
    string oldName = saveSelect->getSelectionText();
    string newName = renamingField->getText();
    if(strpbrk(newName.c_str(), "\\/:*?\"<>|") != NULL)
    {
        new Label(320, 200, 300, 70, StickyDirs::none, "\\/:*?\"<>| not allowed in save file names.", renameMenu);
    }
    else if(newName != "")
    {
        //Actually do the filesystem action of renaming the folder
        SaveManager::renameSave(newName, oldName);
        if(saveSelect->getSelection() != -1)
            saveSelect->getSelectionText() = newName;
    }
    updateSaveList();
    GUI::transition(saveMenu);
}

void Menus::saveToCreate(void* inst, void *arg)
{
    GUI::transition(createMenu);
    creatingField->setText("");
    creatingField->gainFocus();
}

void Menus::createCancel(void* inst, void *arg)
{
    GUI::transition(saveMenu);
}

void Menus::createOK(void* inst, void *arg)
{
    if(creatingField->isActive())
    {
        creatingField->loseFocus();
    }
    string newName = creatingField->getText();
    if(newName.size() != 0)
    {
        //Check for invalid filename characters
        if(strpbrk(newName.c_str(), "\\/:*?\"<>|") != NULL)
        {
            new Label(320, 200, 300, 70, StickyDirs::none, "\\/:*?\"<>| not allowed in save file names.", createMenu);
        }
        else
        {
            saveSelect->addOption(newName);
            ((ScrollBlock*) saveSelect->getParent())->matchCanvasToContents();
        }
    }
    GUI::transition(saveMenu);
}

void Menus::deleteCancel(void* inst, void *arg)
{
    GUI::transition(saveMenu);
}

void Menus::deleteOK(void* inst, void* arg)
{
    //Delete the folder
    SaveManager::deleteSave(saveSelect->getSelectionText());
    updateSaveList();
    GUI::transition(saveMenu);
}

void Menus::saveToGame(void *inst, void*)
{
    //Load selected world
    if(saveSelect->getSelection() != -1)
    {
        SaveManager::loadWorld(saveSelect->getSelectionText());
        GUI::transition(gameScene);
    }
}

void Menus::saveToDelete(void* inst, void*)
{
    if(saveSelect->getSelection() != -1)
    {
        //Initialize delete label text
        string text = "Are you sure you want to delete \"" + saveSelect->getSelectionText() + "\"?";
        deleteConfirmLabel->getText() = text;
    }
}

void Menus::updateSaveList()
{
    SaveManager::refreshSaveList();
    //Update saveSelect's items
    saveSelect->clearSelection();
    saveSelect->setOptions(SaveManager::saves);
    saveScroll->matchCanvasToContents();
}

void Menus::updateFPS(int fps)
{
    char buf[10];
    sprintf(buf, "FPS:%d", fps);
    fpsLabel->getText() = buf;
}