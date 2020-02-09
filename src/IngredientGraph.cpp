//
//  RecipeGraph.cpp
//  drink_project
//
//  Created by Tucker Bell on 4/24/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#include "RecipeList.hpp"
#include "IngredientGraph.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

/* globals */
std::string ingredientDataPath = "../../materials/ingredient_data/";

/* IngredientNode implementation */
IngredientNode::IngredientNode(int id, std::string name)
{
    this->id = id;
    this->name = name;
}
int IngredientNode::getId()
{
    return id;
}
std::string IngredientNode::getName()
{
    return name;
}
void IngredientNode::setId(int id)
{
    this->id = id;
}
void IngredientNode::setName(std::string name)
{
    this->name = name;
}

/* IngredientGraph implementation*/
IngredientGraph::IngredientGraph(std::string filePath)
{
    addIngredient(99999, "all");
    loadFromFile(filePath);
}

void IngredientGraph::addIngredient(int ingr_id, std::string name)
{
    ingredients.push_back(IngredientNode(ingr_id, name));
    adjList.push_back(std::vector<int>());
}
void IngredientGraph::addPath(int top_ingr, int bottom_ingr)
{
    int currId, topIndex, bottomIndex;
    topIndex = bottomIndex = -1;
    for(int i = 0; i < ingredients.size(); ++i){
        currId = ingredients.at(i).getId();
        if(currId == top_ingr){
            topIndex = i;
        }
        if(currId == bottom_ingr){
            bottomIndex = i;
        }
    }
    
    if(top_ingr < 0 || bottom_ingr < 0){
        return;
    }
    adjList.at(topIndex).push_back(bottomIndex);
}

void IngredientGraph::printIngredientsHelper(int id, int layer)
{
    if(id != 99999){
        std::cout << "|";
    }
    for(int i = 0; i < layer; ++i)
        std::cout << "....";
    for(int i = 0; i < ingredients.size(); ++i){
        if(ingredients.at(i).getId() == id){
            if(id != 99999)
                std::cout << ingredients.at(i).getName() << std::endl;
            ++layer;
            for(int j = 0; j < adjList.at(i).size(); ++j){
                printIngredientsHelper((ingredients.at(adjList.at(i).at(j))).getId(), layer);
            }
            return;
        }
    }
}
void IngredientGraph::printIngredients()
{
    if(!ingredients.size())
        return;
    printIngredientsHelper(ingredients.at(0).getId(), -1);
}

void IngredientGraph::loadFromFile(std::string filePath)
{
    std::string line;
    std::ifstream myfile;
    myfile.open(filePath);
    if (myfile.is_open())
    {
        int read_nodes = 1;
        int topNode, bottomNode;
        std::string name;
        while ( getline(myfile,line) )
        {
            if(*line.begin() == '#'){
                read_nodes = 0;
                continue;
            }
            if(read_nodes){
                topNode = std::stoi(line.substr(0, 5));
                name = line.substr(5);
                name.erase(name.begin());
                addIngredient(topNode, name);
                //std::cout << topNode << " " << name << std::endl;
            }
            else{
                topNode = std::stoi(line.substr(0,5));
                bottomNode = std::stoi(line.substr(6));
                addPath(topNode, bottomNode);
                //std::cout << topNode << " " << bottomNode << std::endl;
            }
        }
        myfile.close();
    }
    else
        std::cout << "Could not open file " << filePath <<std::endl;
}

int IngredientGraph::search(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    for(int i = 0; i < ingredients.size(); ++i){
        std::string curr = ingredients.at(i).getName();
        std::transform(curr.begin(), curr.end(), curr.begin(), ::tolower);
        if(curr.compare(name) == 0){
            return ingredients.at(i).getId();
        }
    }
    return -1;
}

void IngredientGraph::queueBranch(std::string name, std::vector<int> *userIngredients)
{
    userIngredients -> clear();
    int id;
    if((id = search(name)) > 0){
        for(int i = 0; i < ingredients.size(); ++i){
            if(ingredients.at(i).getId() == id){
                userIngredients -> push_back(ingredients.at(i).getId());
                queueBranchHelper(id, userIngredients);
                break;
            }
        }
    }
    else return;
}

void IngredientGraph::queueBranchHelper(int id, std::vector<int> *userIngredients)
{
    //find id in ingredients vector
    for(int i = 0; i < ingredients.size(); ++i){
        if(ingredients.at(i).getId() == id){
            //loop through every adjacent ingredient
            int currIndex;
            for(int j = 0; j < adjList.at(i).size(); ++j){
                //queue all children; recursive call all children
                currIndex = adjList.at(i).at(j);
                userIngredients -> push_back(ingredients.at(currIndex).getId());
                queueBranchHelper(ingredients.at(currIndex).getId(), userIngredients);
            }
            break;
        }
    }
}
