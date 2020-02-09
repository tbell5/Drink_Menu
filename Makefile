all: menu.out
	cd ./bin/Debug; ./menu.out

menu.out:
	g++ -o ./bin/Debug/menu.out ./src/main.cpp ./src/IngredientGraph.cpp ./src/RecipeList.cpp ./src/menuUI.cpp
