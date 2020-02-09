//
//  DataManager.cpp
//  drink_project
//
//  Created by Tucker Bell on 5/7/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#include "DataManager.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>

/* globals */
std::string materialsPath = "/Users/Tucker/programs/drink_menu/materials/";

void RecipeDataFilesList::loadFromFile(std::string filePath)
{
    //make infile from fileName
    std::ifstream metaFile(filePath);
    if(metaFile.is_open()){
        std::string line;
        while(getline(metaFile, line)){
            //tokenize each line:
            //  first token = name of the list
            //  second token = file name
            std::string token;
            std::vector<std::string> tokens;
            std::stringstream tokenizer;
            tokenizer << line;
            while(getline(tokenizer, token, ',')){
                tokens.push_back(token);
            }
            //add the name + file to the list
            filesList.push_back(std::make_pair(tokens[0], tokens[1]));
        }
    }
}

std::string RecipeDataFilesList::getName(int i)
{
    if(i >= 0 && i < size()){
        return(filesList.at(i).first);
    }
    return NULL;
}

std::string RecipeDataFilesList::getDirectory(int i)
{
    if(i >= 0 && i < size()){
        return(filesList.at(i).second);
    }
    return NULL;
}

int RecipeDataFilesList::size()
{
    return (int)filesList.size();
}

void DataManager::generateGraphFile(std::string inFilePath,
                                    std::string outFilePath)
{
    //ingredient ids = index within name_degree_vector (+ 10000)
    std::vector<std::pair<std::string, int> > name_degree_vector;
    std::vector<std::pair<int, int> > paths_vector;

    //parse inFile for ingredient names and degrees of indentation
    std::ifstream inFile;
    inFile.open(inFilePath);
    if(inFile.is_open()){
        std::string line;
        int i, degree, whiteChar;
        while (getline(inFile, line)){
            if(line.size() == 0){
                std::cout << "skipped line generating ingredient graph: size=0\n";
                continue;
            }
//            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
//                return !std::isspace(ch);
//            }).base(), line.end());
            degree = 0;
            //detect if tabs or spaces are being used to indent the line
            whiteChar = (line[0] == ' ' ? ' ' : '\t');
            for(i = 0; line[i] == whiteChar; ++i);
            //degree = # of indentations (tabs / 4 spaces)
            degree = (whiteChar == ' ' ? (i / 4) : i);
            name_degree_vector.push_back(std::make_pair(line.substr(i), degree));
            std::cout << line.substr(i) << " " << degree << std::endl;
        }
    
        inFile.close();
    }
    else{
        std::cout << "generateGraphFile could not open inFile: " << inFilePath
            << std::endl;
    }
    
    //populate the paths vector based off the name, indentation, and order of
    //ingredients
    pathsGenerator(&name_degree_vector, &paths_vector);
    
    //generate new IngredientGraphFile
    std::ofstream outFile;
    outFile.open(outFilePath);
    if(outFile.is_open()){
        //write ingredient names and ids
        for(int i = 0; i < name_degree_vector.size(); ++i){
            outFile << i + 10000 << "," << name_degree_vector.at(i).first
                << std::endl;
        }
        outFile << "#" << std::endl; //seperate the sections
        //write ingredient paths (Id-from,Id-to)
        for(int i = 0; i < paths_vector.size(); ++i){
            outFile << paths_vector.at(i).first + 10000 << "," <<
                paths_vector.at(i).second + 10000 << std::endl;
        }
    }
    else{
        std::cout << "generateGraphFile could not open outFile: " << outFilePath
        << std::endl;
    }
}

/*
 *  use name_degree_vector to determine the graph paths. Save the paths in the
 *  empty paths_vector
 */
void DataManager::pathsGenerator(
    std::vector<std::pair<std::string, int> > *name_degree_vector,
    std::vector<std::pair<int, int> > *paths_vector)
{
    //call the recursive helper function once for each non-indented line
    for(int i = 0; i < name_degree_vector -> size(); ++i)
        if(name_degree_vector -> at(i).second == 0){
            paths_vector -> push_back(std::make_pair(89999, i));
            pathsGeneratorHelper(name_degree_vector, paths_vector, 0, i);
        }
}

/*
 *  recursive function for collecting the paths in a given "branch" of the
 *  ingredient list. The root of each branch is a non-indented line. This
 *  function runs recursively from one non-indented line to the next.
 */
void DataManager::pathsGeneratorHelper(
    std::vector<std::pair<std::string, int> > *name_degree_vector,
    std::vector<std::pair<int, int> > *paths_vector, int degree, int index)
{
    int currDegree;
    for(int i = 1; index + i < (name_degree_vector -> size()) && (currDegree = name_degree_vector->at(index+i).second) > degree; ++i){
        if(currDegree == (degree + 1)){
            paths_vector -> push_back(std::make_pair(index, index+i));
            pathsGeneratorHelper(name_degree_vector, paths_vector, degree+1,
                index+i);
        }
    }
}

/*
 *  Based on a comparison of an old IngredientGraphFile to a new
 *  IngredientGraphFile, generate a new RecipeFile based on an old RecipeFile,
 *  updating the ingredient ids per the adjustment in the ingredient list.
 */
void DataManager::refactorRecipeFile(std::string oldIngredientFilePath,
                                     std::string newIngredientFilePath,
                                     std::string oldRecipeFilePath,
                                     std::string newRecipeFilePath)
{
    //PART 1: form the "idKey" map
    std::map<std::string, int> oldIngredientsMap;
    std::map<std::string, int> newIngredientsMap;
    if(mapIngredientList(oldIngredientFilePath, &oldIngredientsMap) || mapIngredientList(newIngredientFilePath, &newIngredientsMap)){
        std::cout << "could not interpret ingredientFiles. Aborting";
        std::cout << std::endl;
        return;
    }
    
    std::map<int, int> idKey;   //maps old ids to new ids
    std::map<std::string, int>::iterator newItr;
    for(std::map<std::string, int>::iterator oldItr = oldIngredientsMap.begin();
            oldItr != oldIngredientsMap.end(); ++oldItr){
        if((newItr = newIngredientsMap.find(oldItr -> first)) == newIngredientsMap.end()){
            std::cout << "could not translate the ingredients: failed to find ";
            std::cout << "corresponding ingredients for <" << oldItr -> first;
            std::cout << ">. Aborting." << std::endl;
            return;
        }
        idKey.insert(std::pair<int, int>(oldItr -> second, newItr -> second));
    }
    
    //PART 2: remap the ingredient ids in the oldRecipeFile
    std::ifstream oldRecipeFile;
    std::ofstream newRecipeFile;
    oldRecipeFile.open(oldRecipeFilePath);
    newRecipeFile.open(newRecipeFilePath);
    
    std::string line, temp;
    std::stringstream buffer;
    std::vector<std::string> tokens;
    std::map<int, int>::iterator idKeyItr;
    int oldId;
    if(oldRecipeFile.is_open() && newRecipeFile.is_open()){
        //output the header straight to the new file
        while(getline(oldRecipeFile, line) && line[0] != '#'){
            newRecipeFile << line << std::endl;
        }
        while(getline(oldRecipeFile, line)){
            //output '#' lines straight to the new file
            if(line[0] == '#'){
                newRecipeFile << line << std::endl;
                continue;
            }
//            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
//                return !std::isspace(ch);
//            }).base(), line.end());
            buffer << line;
            while(getline(buffer, temp, ',')){
                if(temp.size() > 0)
                    tokens.push_back(temp);
            }
            newRecipeFile << tokens[0] << "," << tokens[1] << ",";
            for(int i = 2; i < tokens.size(); ++i){
                oldId = atoi(tokens.at(i).c_str());
                if((idKeyItr = idKey.find(oldId)) == idKey.end()){
                    std::cout << "could not remap ingredients for recipe: <";
                    std::cout << tokens[0] << ">.\n\tIngredient id: <";
                    std::cout << tokens.at(i) << ">. Aborting.\n";
                    return;
                }
                newRecipeFile << idKeyItr -> second << ",";
            }
            newRecipeFile << std::endl;
            buffer.str(std::string());
            buffer.clear();
            tokens.clear();
        }
    }
    else{
        std::cout << "could not open new and/or old recipeFile\n";
        return;
    }
}

/*
 *  return: -1 on found duplicate, -2 on could not open file, 0 on success
 */
int DataManager::mapIngredientList(std::string filePath, std::map<std::string, int> *ingrMap)
{
    std::ifstream ingredientsFile;
    ingredientsFile.open(filePath);
    if(ingredientsFile.is_open()){
        std::string line, temp;
        int currId;
        std::vector<std::string> tokens;
        std::stringstream lineStream;
        while(getline(ingredientsFile, line) && line[0] != '#'){
            //tokenize the line
//            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) {
//                return !std::isspace(ch);
//            }).base(), line.end());
            lineStream << line;
            while(getline(lineStream, temp, ',')){
                tokens.push_back(temp);
            }
            if(tokens.size() == 2){
                currId = atoi(tokens.at(0).c_str());
                if(ingrMap->find(tokens.at(1)) != ingrMap->end()){
                    std::cout << "found duplicate ingredient in " << filePath;
                    std::cout << ", name: <" << tokens.at(1) << ">; aborting.\n";
                    return -1;
                }
                ingrMap->insert(std::pair<std::string, int>(tokens.at(1), currId));
            }
            else{
                std::cout << "could not interpret ingredientFile line: "
                << line << std::endl;
            }
            lineStream.str(std::string());
            lineStream.clear();
            tokens.clear();
        }
    }
    else{
        std::cout << "DataManager:refactorGraphFile: could not open"
        << " ingredientFile: " << filePath <<std::endl;
        return -2;
    }
    return 0;
}
