#ifndef __MENUS_H__
#define __MENUS_H__

#include <string>
#include "DebugTools.h"
#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "MultiSelect.h"
#include "ScrollBlock.h"
#include "Minimap.h"
#include "GUI.h"
#include "World.h"
#include "SaveManager.h"

enum struct MenuEvent
{
    GAME_QUIT,
    WORLD_LOADED
};

struct GeneralMsg
{
    GeneralMsg(MenuEvent eventType, void* data);
    MenuEvent eventType;
    void* data;
};

namespace Menus
{
    void initAll();
    extern Broadcaster<GeneralMsg> bc;
    void updateFPS(int fps);
    extern Label* fpsLabel;
    extern Scene* debugScene;
    //Main menu
    extern Scene* mainMenu;
    void mainStart(void* inst, void*);
    void mainQuit(void* inst, void*);
    //Save menus
    extern Scene* saveMenu;
    extern ScrollBlock* saveScroll;         //lists saves
    extern MultiSelect* saveSelect;
    void updateSaveList();                  //helper to update saveSelect
    void saveToMain(void* inst, void*);
    void saveToDelete(void* inst, void*);
    void saveToCreate(void* inst, void*);
    void saveToRename(void* inst, void*);
    void saveToGame(void* inst, void*);
    extern Scene* deleteConfirmMenu;
    extern Label* deleteConfirmLabel;
    void deleteOK(void* inst, void*);
    void deleteCancel(void* inst, void*);
    extern Scene* renameMenu;
    extern Field* renamingField;
    extern Label* renamingLabel;
    void renameOK(void* inst, void*);
    void renameCancel(void* inst, void*);
    extern Scene* createMenu;
    extern Field* creatingField;
    void createOK(void* inst, void*);
    void createCancel(void* inst, void*);
    //Main game HUD
    extern Scene* gameScene;
}

#endif

