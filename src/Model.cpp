//
//  Model.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Model.h"

using namespace std;

Model::Model()
{
    this->currentWorld = nullptr;
}

Model::~Model()
{
    
}

void Model::update()
{
    
}

World* Model::getCurrentWorld()
{
    return this->currentWorld;
}