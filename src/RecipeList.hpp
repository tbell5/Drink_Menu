//
//  RecipeList.hpp
//  drink_project
//
//  Created by Tucker Bell on 4/24/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//


#ifndef RecipeList_hpp
#define RecipeList_hpp

#include <vector>
#include <string>
#include <fstream>

extern std::string recipeDataPath;

class RecipeNode{
private:
    std::string name;
    std::string fileName;
    std::vector<int> ingredientIds;
public:
    RecipeNode(std::string name, std::string fileName);
    
    void setName(std::string name);
    void setFileName(std::string fileName);
    void setIngredients(std::vector<int> ingredients);
    
    std::string getName();
    std::string getFileName();
    std::vector<int> getIngredientIds();
};

class RecipeList{
private:
    std::vector<RecipeNode> recipes;
    std::string listName;
    std::string directoryName;
public:
    RecipeList(std::string listName, std::string directoryName);
    void setListName(std::string name);
    void setDirectoryName(std::string name);
    std::string getListName();
    std::string getDirectoryName();
    RecipeNode* get(int index);
    RecipeNode* search(std::string name);
    void add(std::string name, std::string filePath, std::vector<int> ingredientsIds);
    void printList();
    int size();
    void queryByIngredient(std::vector<int>* ingredientIds);
};

class SuperRecipeList{
private:
    std::vector<RecipeList*> lists;
    std::string superListName;
    //load from file: return 0 on failure, 1 on success
    int loadFromFile(std::string filePath);
public:
    SuperRecipeList();
    SuperRecipeList(std::string filePath);
    void addList(std::string listName, std::string directoryName);
    RecipeList* getList(int index);
    int size();
    SuperRecipeList copy();
    void printLists();
    void clear();
    void setName(std::string);
    std::string getName();
};
#endif /* RecipeList_hpp */
