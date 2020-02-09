//  
//  menuUI.hpp
//  drink_project
//
//  Created by Tucker Bell on 5/1/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#ifndef menuUI_hpp
#define menuUI_hpp

#include <vector>
#include "RecipeList.hpp"
#include "IngredientGraph.hpp"


class menuUI{
private:
    static void ingredient_command(SuperRecipeList* matchingRecipes,
                                   std::vector<IngredientNode> *searchedIngredients,
                                   IngredientGraph* graph, std::string userString);
    static void open_command(SuperRecipeList* matchingRecipes,
                             int list, int index);
    static void open_command(SuperRecipeList* matchingRecipes, std::string name);
    static void menu_command(SuperRecipeList* matchingRecipes);
    static void clear_command(SuperRecipeList *allRecipes,
                              SuperRecipeList *matchingRecipes,
                              std::vector<IngredientNode> *searchedIngredients);
    static void open_splash_screen(std::string filepath);
    static int narrowMatches(SuperRecipeList* matchingRecipes,
                             std::vector<int> *matchingIngredients);
public:
    static void run_ui();
};
#endif /* menuUI_hpp */
