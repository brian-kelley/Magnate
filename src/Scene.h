//
//  Scene.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Scene__
#define __MagIndev__Scene__

#include <stdio.h>
#include <vector>
#include <string>
#include "Button.h"
#include "Field.h"
#include "Label.h"

class Scene
{
public:
    Scene();
    Scene(Scene& toCopy);
    ~Scene();
    void addButton(Button* b);
    void addLabel(Label* l);
    void addField(Field* f);
    std::vector<Button>* getButtons();
    std::vector<Label>* getLabels();
    std::vector<Field>* getFields();
    void clearAll();
private:
    std::vector<Button> buttons;
    std::vector<Field> fields;
    std::vector<Label> labels;
};

#endif /* defined(__MagIndev__Scene__) */
