//
//  data.hpp
//  drink_project
//
//  Created by Tucker Bell on 5/7/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <utility>

class RecipeDataFilesList{
private:
    std::vector<std::pair<std::string, std::string> > filesList;
public:
    void loadFromFile(std::string filePath);
    std::string getName(int i);
    std::string getDirectory(int i);
    int size();
};

class DataManager{
private:
    static void pathsGenerator(std::vector<std::pair<std::string, int> > *name_degree_vector,
        std::vector<std::pair<int, int> > *paths_vector);
    static void pathsGeneratorHelper (
        std::vector<std::pair<std::string, int> > *name_degree_vector,
        std::vector<std::pair<int, int> > *paths_vector, int degree, int index);
    static int mapIngredientList(std::string filepath,
                                  std::map<std::string, int> *ingrMap);
public:
    static void generateGraphFile(
        std::string infile=(materialsPath + "data/ingredient_list.txt"),
        std::string outfile=(materialsPath + "data/new_ingredient_data.csv"));
    
    static void refactorRecipeFile(
        std::string oldIngredientFile=(materialsPath + "data/old_ingredient_data.csv"),
        std::string newIngredientFile=(materialsPath + "data/new_ingredient_data.csv"),
        std::string oldRecipeFile=(materialsPath + "data/old_recipe_data.csv"),
        std::string newRecipeFile=(materialsPath + "data/new_recipe_data.csv"));
};

#endif /* data_hpp */
