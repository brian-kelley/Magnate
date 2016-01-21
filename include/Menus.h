#ifndef __MENUS_H__

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
    //Main menu
    extern Scene* mainMenu;
    static void mainStart(void* inst, void*);
    static void mainQuit(void* inst, void*);
    //Save menus
    extern Scene* saveMenu;
    extern Scene* deleteConfirmMenu;
    
    extern Scene* renameMenu;
    
    extern Scene* createNewMenu;
    
}

#endif