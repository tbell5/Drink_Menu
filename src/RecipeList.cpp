//
//  RecipeList.cpp
//  drink_project
//
//  Created by Tucker Bell on 4/24/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#include "RecipeList.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

/*
 * RecipeNode implementation
 */

/* globals */
std::string recipeDataPath = "../../materials/recipe_databases/the_ultimate_bar_book/";

RecipeNode::RecipeNode(std::string name, std::string fileName)
{
    this -> name = name;
    this -> fileName = fileName;
}

void RecipeNode::setName(std::string name)
{
    this -> name = name;
}
void RecipeNode::setFileName(std::string fileName)
{
    this -> fileName = fileName;
}
void RecipeNode::setIngredients(std::vector<int> ingredients)
{
    for(int i = 0; i < ingredients.size(); ++i){
        this ->ingredientIds.push_back(ingredients.at(i));
    }
}

std::string RecipeNode::getName()
{
    return name;
}
std::string RecipeNode::getFileName()
{
    return fileName;
}
std::vector<int> RecipeNode::getIngredientIds()
{
    return ingredientIds;
}

/*
 * RecipeList implementation
 */

RecipeList::RecipeList(std::string listName, std::string directoryName)
{
    this -> listName = listName;
    this -> directoryName = directoryName;
}

std::string RecipeList::getListName()
{
    return listName;
}

std::string RecipeList::getDirectoryName()
{
    return directoryName;
}

void RecipeList::setListName(std::string name)
{
    listName = name;
}

void RecipeList::setDirectoryName(std::string name)
{
    directoryName = name;
}

RecipeNode* RecipeList::get(int index)
{
    if(index >= recipes.size() || index < 0){
        return NULL;
    }
    return &(recipes.at(index));
}

RecipeNode* RecipeList::search(std::string name)
{
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    for(int i = 0; i < recipes.size(); ++i){
        std::string curr = recipes.at(i).getName();
        std::transform(curr.begin(), curr.end(), curr.begin(), ::tolower);
        if(curr.compare(name) == 0){
            return &(recipes.at(i));
        }
    }
    return NULL;
}

void RecipeList::queryByIngredient(std::vector<int>* userIds)
{
    //loop through each recipe
    for(std::vector<RecipeNode>::iterator recipeItr = recipes.begin();
        recipeItr != recipes.end(); ++recipeItr){
        int matching = 0;
        //check if at least one of the ingredients in the userIds matches
        // at least one of the ingredients in the recipe
        std::vector<int> currIds = recipeItr->getIngredientIds();
        for(int i = 0; i < currIds.size(); ++i){
            for(int j = 0; j < userIds->size(); ++j){
                if(userIds->at(j) == currIds.at(i)){
                    matching = 1;
                    break;
                }
            }
            if(matching)
                break;
        }
        //remove recipes from the list which do not match
        if(!matching){
            recipes.erase(recipeItr);
            --recipeItr;
        }
    }
}

void RecipeList::add(std::string name, std::string fileName,
                     std::vector<int> ingredientsIds)
{
    RecipeNode newNode(name, fileName);
    newNode.setIngredients(ingredientsIds);
    recipes.push_back(newNode);
}

int RecipeList::size()
{
    return (int)recipes.size();
}

void RecipeList::printList()
{
    std::cout << "size: " << recipes.size() << std::endl;
    std::cout << "name: " << listName << std::endl;
    std::cout << "directory: " << directoryName << std::endl;
    for(int i = 0; i < recipes.size(); ++i){
        std::cout << i + 1 << ") " << recipes.at(i).getName() << std::endl;
        std::cout << "   ingredient ids: ";
        for(int j = 0; j < recipes.at(i).getIngredientIds().size(); ++j){
            std::cout << recipes.at(i).getIngredientIds().at(j) << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/*
 * SuperRecipeList implementation
 */
SuperRecipeList::SuperRecipeList(){}

SuperRecipeList::SuperRecipeList(std::string filePath){
    loadFromFile(filePath);
}


void SuperRecipeList::addList(std::string listName, std::string directoryName)
{
    RecipeList* listPtr;
    listPtr = new RecipeList(listName, directoryName);
    lists.push_back(listPtr);
}

RecipeList* SuperRecipeList::getList(int index)
{
    return lists.at(index);
}

int SuperRecipeList::size()
{
    return (int)lists.size();
}

int SuperRecipeList::loadFromFile(std::string infile)
{
    //return 0 on failure, 1 on success
    std::ifstream recipeDataFile;
    recipeDataFile.open(infile);
    if (recipeDataFile.is_open()){
        /* PART 1: initialize SuperRecipeList with names and folder names of
         *         each list.
         */
        std::vector<std::string> lineTokens;
        std::stringstream lineStream;
        std::string temp, line;
        
        while(getline(recipeDataFile, line) && line[0] != '#'){
            lineStream << line;
            while(getline(lineStream, temp, ',')){
                lineTokens.push_back(temp);
            }
            addList(lineTokens[0], lineTokens[1]);
            
            lineTokens.clear();
            lineStream.str("");
            lineStream.clear();
        }
        
        /* PART 2: populate each list with recipes, changing lists at the lines
         *         beginning with '#'
         */
        int index = 0;
        std::string recipeName, recipeFile;
        std::vector<int> ingredientIDs;
        while(getline(recipeDataFile, line)){
            if(line[0] == '#'){
                index++;
                continue;
            }
            
            lineStream << line;
            while(getline(lineStream, temp, ',')){
                lineTokens.push_back(temp);
            }
            
            recipeName = lineTokens[0];
            recipeFile = lineTokens[1];
            for(int i = 2; i < lineTokens.size(); ++i){
                ingredientIDs.push_back(atoi(lineTokens[i].c_str()));
            }
            getList(index) -> add(recipeName, recipeFile, ingredientIDs);
            
            ingredientIDs.clear();
            lineTokens.clear();
            lineStream.clear();
            lineStream.str("");
        }
    }
    else{
        std::cout << "| Could not open file: " << infile << std::endl;
        return 0;
    }
    return 1;
}

SuperRecipeList SuperRecipeList::copy()
{
    SuperRecipeList copyList;
    for(int i = 0; i < size(); ++i){
        copyList.addList(getList(i)->getListName(), getList(i)->getDirectoryName());
        RecipeList* newList = copyList.getList(i);
        RecipeList* oldList = getList(i);
        for(int j = 0; j < oldList->size(); ++j){
            newList->add(oldList->get(j)->getName(), oldList->get(j)->getFileName(), oldList->get(j)->getIngredientIds());
        }
    }
    copyList.setName("is this thing on?");
    return copyList;
}

void SuperRecipeList::printLists()
{
    for(int i = 0; i < lists.size(); ++i){
        lists.at(i)->printList();
    }
}

void SuperRecipeList::clear()
{
    lists.clear();
}

void SuperRecipeList::setName(std::string name)
{
    superListName = name;
}

std::string SuperRecipeList::getName()
{
    return superListName;
}
