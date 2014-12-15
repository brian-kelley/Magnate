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

Scene::Scene(Scene& toCopy)
{
    //Call copy constructors for each vector member of Scene
    this->scrollBlocks = *(new vector<ScrollBlock>(toCopy.scrollBlocks));
    this->buttons = *(new vector<Button>(toCopy.buttons));
    this->labels = *(new vector<Label>(toCopy.labels));
    this->fields = *(new vector<Field>(toCopy.fields));
}

Scene::~Scene()
{
    this->buttons.clear();
    this->fields.clear();
    this->labels.clear();
}

void Scene::addScrollBlock(ScrollBlock* sb)
{
    this->scrollBlocks.push_back(*sb);
}

void Scene::addButton(Button* b)
{
    this->buttons.push_back(*b);
}

void Scene::addLabel(Label* l)
{
    this->labels.push_back(*l);
}

void Scene::addField(Field* f)
{
    this->fields.push_back(*f);
}

void Scene::clearAll()
{
    this->scrollBlocks.clear();
    this->buttons.clear();
    this->fields.clear();
    this->labels.clear();
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