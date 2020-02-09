//
//  menuUI.cpp
//  drink_project
//
//  Created by Tucker Bell on 5/1/19.
//  Copyright © 2019 Tucker Bell. All rights reserved.
//

#include "menuUI.hpp"
#include "RecipeList.hpp"
#include "IngredientGraph.hpp"
#include <fstream>
#include <iostream>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

/* external variables: ingredientDataPath, recipeDataPath
 *   the string variables "ingredientDataPath" and and "recipeDataPath" give the
 *   relative path from the executable to the directories containing the
 *   csv files for initializing the IngredientGraph and RecipeList, respecively.
 *
 *   The strings are defined in RecipeList.hpp and IngredientGraph.hpp
 */


void menuUI::run_ui()
{
    //open splash screen
    open_splash_screen("../../materials/splash_screen.txt");
    
    //load recipe data
    SuperRecipeList allRecipes(recipeDataPath + "recipe_data.csv");
    SuperRecipeList matchingRecipes = allRecipes.copy();
    
    
    //load ingredient data
    IngredientGraph ingredients(ingredientDataPath + "ingredient_data.csv");
    std::vector<IngredientNode> searchedIngredients;
    
    
    //run program
    std::string prompt = "| > ";
    std::string usage = "Usage: <command letter> <optional argument>";

    char command = 'b';
    std::string userInput = "";
    std::string userArgument;
    std::string::iterator sitr;
     
    while(command != 'q'){
        std::cout << prompt;
        std::getline(std::cin, userInput);
        sitr = userInput.begin();
        if(!std::isalpha(*sitr) || (userInput.size() > 1 && *(sitr + 1) != ' ')){
            std::cout << "| " << usage << std::endl;
            command = 'b';
            continue;
        }
        command = userInput[0];
        userArgument = (userInput.size() > 2 ? userInput.substr(2) : "#");

        switch(command)
        {
            case 'i': {
                ingredient_command(&matchingRecipes, &searchedIngredients, &ingredients, userInput);
                break;
            }
            case 'c': {
                clear_command(&allRecipes, &matchingRecipes, &searchedIngredients);
                break;
            }
            case 's': {
                ingredients.printIngredients();
                break;
            }
            case 'm': {
                menu_command(&matchingRecipes);
                break;
            }
            case 'o': {
                //invalid argument length error
                if(userArgument[0] == '#'){
                    std::cout << "| " << "To open recipe file: o <recipe name or number>" << std::endl;
                    break;
                }
                //check if the argument is a decimal representation
                bool isNumber = true;
                for(int i = 0; i < userArgument.size(); ++i){
                    if(!(isdigit(userArgument[i]) || userArgument[i] == '.')){
                        isNumber = false;
                    }
                }
                //open command call by recipe number
                if(isNumber){
                    //tokenize the number with '.' delimiter
                    std::string token;
                    std::stringstream tokenizer;
                    std::vector<int> tokens;
                    tokenizer << userArgument;
                    while(getline(tokenizer, token, '.')){
                        tokens.push_back(atoi(token.c_str()));
                    }
                    //too many tokens error
                    if(tokens.size() != 2){
                        std::cout << "| use \"o #.#\" to open recipe by number" << std::endl;
                        break;
                    }
                    open_command(&matchingRecipes, tokens[0], tokens[1]);
                    break;
                }
                //open_command call by recipe name (uses full recipe list)
                open_command(&allRecipes, userArgument);
                break;
            }
            case 'q': {
                break;
            }
            default: {
                std::cout << "| " << "Command not recognized: " << command << std::endl;
                continue;
            }
        }
    }
    std::cout << "|______________________________________________________|" << std::endl;
}

void menuUI::ingredient_command(SuperRecipeList *matchingRecipes,
                                std::vector<IngredientNode> *searchedIngredients,
                                IngredientGraph *ingredientGraph, std::string userString)
{
    //printout of searched-for ingredients (when no ingredient is named)
    if(userString.size() < 2){
        if(searchedIngredients->size() == 0){
            std::cout << "| no ingredients selected" << std::endl;
            return;
        }
        for(int i = 0; i < searchedIngredients -> size(); ++i){
            std::cout << "| " << (i + 1) << ") " << searchedIngredients -> at(i).getName()
            << std::endl;
        }
        return;
    }

    //search for the ingredient in graph
    int id;
    std::string newIngredient = userString.substr(2);
    if((id = ingredientGraph -> search(newIngredient)) >= 0){
        //add the selected ingredient to searchedIngredients
        IngredientNode newNode(id, newIngredient);
        searchedIngredients -> push_back(newNode);
        //add the selected ingredient and any sub-ingredients to matchingIngredients
        std::vector<int> subIngredients;
        ingredientGraph -> queueBranch(newIngredient, &subIngredients);
        int numMatches = narrowMatches(matchingRecipes, &subIngredients);
        std::cout << "| narrowed to " << numMatches << " recipe";
        std::cout << (numMatches == 1 ? "" : "s") << std::endl;
    }
    else{
        std::cout << "| Ingredient <" << newIngredient << "> not found." << std::endl;
    }
}

void menuUI::menu_command(SuperRecipeList* matchingRecipes)
{
    std::cout << "|======================================================|\n";
    for(int i = 0; i < matchingRecipes -> size(); ++i){
        RecipeList* curr = matchingRecipes->getList(i);
        if(curr->size() == 0){
            continue;
        }
        std::cout << "| " << curr->getListName() << ": " << curr->size();
        std::cout << " recipe" << (curr->size() == 1 ? "" : "s") << std::endl;
        for(int j = 0; j < curr->size(); ++j){
            std::cout << "| " << i+1 << "." << j+1;
            std::cout << ") " << (j < 9 ? " " : "");
            std::cout << curr -> get(j)->getName() << "\n";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "|======================================================|\n";
}

void menuUI::open_command(SuperRecipeList *matchingRecipes, int list, int index)
{
    list -= 1;
    index -= 1;
    //check if index is in the right range
    //error: invalid list index
    if(list < 0 || list >= matchingRecipes->size()){
        std::cout << "| \"o\" command error: invalid category number: ";
        std::cout << "<" << list + 1 << ">" << std::endl;
        return;
    }
    RecipeList *currList = matchingRecipes->getList(list);
    
    //error: list has 0 matching recipes
    if(currList->size() == 0){
        std::cout << "| \"o\" command error: no matching recipes for category ";
        std::cout << "<" << list + 1 << ">" << std::endl;
        return;
    }
    //error: index is not valid for list
    if(index < 0 || index >= currList->size()){
        std::cout << "| \"o\" command error: no recipe of number <" << index+1;
        std::cout << "> in category <" << list + 1 << ">" << std::endl;
        return;
    }
    
    //make recipe file name
    std::string filePath = recipeDataPath + "drink_recipes/" +
        currList->getDirectoryName() + currList->get(index)->getFileName();
     
    //output recipe file
    std::string line;
    std::ifstream myfile (filePath);
    if (myfile.is_open()){
        std::cout << "|======================================================|\n";
        std::cout << "| " << currList->get(index)->getName() << std::endl;
        std::cout << "| " << std::endl;
        while ( getline (myfile,line) ){
            std::cout << "| " << line << '\n';
        }
        std::cout << "|======================================================|\n";
        myfile.close();
    }
    //error: bad file name
    else{
        std::cout << "| could not open recipe file: <" << filePath << ">" << std::endl;
    }
}

void menuUI::open_command(SuperRecipeList* allRecipes, std::string name)
{
    
    //search for first instance of name in matchingRecipes
    RecipeNode *currNode, *matchingNode;
    matchingNode = NULL;
    int listIndex;
    for(listIndex = 0; listIndex < allRecipes -> size(); ++listIndex){
        if((currNode = allRecipes -> getList(listIndex)->search(name)) != NULL){
            matchingNode = currNode;
            break;
        }
    }
    if(matchingNode == NULL){
        std::cout << "| no recipes of name: \"" << name << "\"" << std::endl;
        return;
    }
    //open recipe file
    std::string listFileName = allRecipes->getList(listIndex)->getDirectoryName();
    std::string recipeFileName = matchingNode->getFileName();
    std::string filePath = recipeDataPath + "drink_recipes/" + listFileName +
        recipeFileName;
    
    std::string line;
    std::ifstream myfile (filePath);
    if (myfile.is_open()){
        std::cout << "|======================================================|\n";
        std::cout << "| " << matchingNode->getName() << std::endl;
        std::cout << "| " << std::endl;
        while ( getline (myfile,line) ){
            std::cout << "| " << line << '\n';
        }
        std::cout << "|======================================================|\n";
        myfile.close();
    }
    else{
        std::cout << "| could not open recipe file <" << filePath <<">\n";
    }
}

void menuUI::clear_command(SuperRecipeList *allRecipes,
                           SuperRecipeList *matchingRecipes,
                           std::vector<IngredientNode> *searchedIngredients)
{
    searchedIngredients->clear();
    *matchingRecipes = allRecipes -> copy();
}

int menuUI::narrowMatches(SuperRecipeList* matchingRecipes, std::vector<int> *newIngredients)
{
    //run the queryByIngredient method on each list in the superRecipesList
    int count = 0;
    for(int i = 0; i < matchingRecipes->size(); ++i){
        matchingRecipes->getList(i)->queryByIngredient(newIngredients);
        count += matchingRecipes->getList(i)->size();
    }
    
    return count;
}

void menuUI::open_splash_screen(std::string filePath){
    std::ifstream myFile(filePath);
    std::string line;
    if(myFile.is_open()){
        while( getline(myFile,line) ){
            std::cout << line <<std::endl;
        }
    }
    else{
        std::cout << "could not open file from <" << filePath << ">\n";
    }
}
