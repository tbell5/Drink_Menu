//
//  IngredientGraph.hpp
//  drink_project
//
//  Created by Tucker Bell on 4/24/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#ifndef IngredientGraph_hpp
#define IngredientGraph_hpp

#include <string>
#include <vector>

/* globals */
extern std::string ingredientDataPath;

class IngredientNode{
private:
    int id;
    std::string name;
public:
    IngredientNode(int id, std::string name);
    int getId();
    std::string getName();
    void setId(int id);
    void setName(std::string name);
};

class IngredientGraph{
    
    
private:
    std::vector<std::vector<int> > adjList;
    std::vector<IngredientNode> ingredients;
    void printIngredientsHelper(int id, int degree);
    void loadFromFile(std::string filePath);
    void queueBranchHelper(int id, std::vector<int> *branchIngredients);
    
public:
    IngredientGraph(std::string filepath);
    void addIngredient(int ingr_id, std::string name);
    void addPath(int top_ingr, int bottom_ingr);
    int search(std::string name);
    void queueBranch(std::string name,
                     std::vector<int> *branchIngredients);
    void printIngredients();
};

#endif // IngredientGraph_hpp
