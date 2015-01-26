//
//  Scene.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Scene.h"

using namespace std;

Scene::Scene()
{
    
}

void Scene::addScrollBlock(ScrollBlock sb)
{
    this->scrollBlocks.push_back(sb);
}

void Scene::addButton(Button b)
{
    this->buttons.push_back(b);
}

void Scene::addLabel(Label l)
{
    this->labels.push_back(l);
}

void Scene::addField(Field f)
{
    this->fields.push_back(f);
}

void Scene::clearAll()
{
    for(int i = 0; i < scrollBlocks.size(); i++)
    {
        scrollBlocks[i].getButtons().clear();
        scrollBlocks[i].getFields().clear();
        scrollBlocks[i].getLabels().clear();
    }
    scrollBlocks.clear();
    buttons.clear();
    fields.clear();
    labels.clear();
}

vector<ScrollBlock>& Scene::getScrollBlocks()
{
    return this->scrollBlocks;
}

vector<Label>& Scene::getLabels()
{
    return this->labels;
}

vector<Field>& Scene::getFields()
{
    return this->fields;
}

vector<Button>& Scene::getButtons()
{
    return this->buttons;
}