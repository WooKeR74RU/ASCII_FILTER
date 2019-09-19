#include "Resources.h"

#include <vector>
#include <fstream>
#include <SFML/Graphics/Font.hpp>

#include "Colors.h"

#define toCStr(x) #x

bool stob(const std::string& str)
{
	return str == "true";
}

sf::Font loadFont(const std::string& fontName)
{
	sf::Font res;
	res.loadFromFile("fonts/" + fontName + ".ttf");
	return res;
}

std::map<std::string, std::string> constantsType;
std::map<std::string, int> constantsId;
std::vector<void*> constants;
void Resources::initConstantsInfo()
{
	constantsType[toCStr(FONT)] = "sf::Font";
	constantsType[toCStr(CHARACTERS_SIZE)] = "int";
	constantsType[toCStr(ASCII_FIRST)] = "int";
	constantsType[toCStr(ASCII_LAST)] = "int";
	constantsType[toCStr(OUTPUT_RESOLUTION_WIDTH)] = "int";
	constantsType[toCStr(OUTPUT_RESOLUTION_HEIGHT)] = "int";
	constantsType[toCStr(COMPRESS_TEXT_MATRIX)] = "bool";
	constantsType[toCStr(DISPERSION)] = "int";
	constantsType[toCStr(ADAPTIVE_BRIGHTNESS)] = "bool";
	constantsType[toCStr(BRIGHTNESS_FACTOR)] = "float";
	constantsType[toCStr(BACKGROUND_COLOR)] = "sf::Color";
	constantsType[toCStr(IS_COLORED)] = "bool";
	constantsType[toCStr(TEXT_COLOR)] = "sf::Color";
	constantsType[toCStr(MULTICOLOR_CHARACTERS)] = "bool";
	constantsType[toCStr(INDEPENDENT_CHARACTERS_COLOR)] = "bool";
	constantsType[toCStr(FRAMES_COUNT)] = "int";
	constantsType[toCStr(INPUT_FOLDER)] = "std::string";
	constantsType[toCStr(INPUT_FILENAME_EXTENSION)] = "std::string";
	constantsType[toCStr(OUTPUT_FOLDER)] = "std::string";
	constantsType[toCStr(OUTPUT_FILENAME_EXTENSION)] = "std::string";
	constantsType[toCStr(THREADS_COUNT)] = "int";

	int id = 0;
	constantsId[toCStr(FONT)] = id++;
	constantsId[toCStr(CHARACTERS_SIZE)] = id++;
	constantsId[toCStr(ASCII_FIRST)] = id++;
	constantsId[toCStr(ASCII_LAST)] = id++;
	constantsId[toCStr(OUTPUT_RESOLUTION_WIDTH)] = id++;
	constantsId[toCStr(OUTPUT_RESOLUTION_HEIGHT)] = id++;
	constantsId[toCStr(COMPRESS_TEXT_MATRIX)] = id++;
	constantsId[toCStr(DISPERSION)] = id++;
	constantsId[toCStr(ADAPTIVE_BRIGHTNESS)] = id++;
	constantsId[toCStr(BRIGHTNESS_FACTOR)] = id++;
	constantsId[toCStr(BACKGROUND_COLOR)] = id++;
	constantsId[toCStr(IS_COLORED)] = id++;
	constantsId[toCStr(TEXT_COLOR)] = id++;
	constantsId[toCStr(MULTICOLOR_CHARACTERS)] = id++;
	constantsId[toCStr(INDEPENDENT_CHARACTERS_COLOR)] = id++;
	constantsId[toCStr(FRAMES_COUNT)] = id++;
	constantsId[toCStr(INPUT_FOLDER)] = id++;
	constantsId[toCStr(INPUT_FILENAME_EXTENSION)] = id++;
	constantsId[toCStr(OUTPUT_FOLDER)] = id++;
	constantsId[toCStr(OUTPUT_FILENAME_EXTENSION)] = id++;
	constantsId[toCStr(THREADS_COUNT)] = id++;
	
	constants.resize(id);
}
void Resources::loadConstants()
{
	initConstantsInfo();

	static const std::string CONFIG_FILENAME = "config.ini";
	std::ifstream configFile(CONFIG_FILENAME);
	for (int i = 0; i < constants.size(); i++)
	{
		std::string argName, argValue;
		configFile >> argName >> argValue;
		if (constantsType.count(argName) == 0)
			continue;
		if (constantsType[argName] == "bool")
			constants[constantsId[argName]] = new bool(stob(argValue));
		if (constantsType[argName] == "int")
			constants[constantsId[argName]] = new int(stoi(argValue));
		if (constantsType[argName] == "float")
			constants[constantsId[argName]] = new float(stof(argValue));
		if (constantsType[argName] == "std::string")
			constants[constantsId[argName]] = new std::string(argValue);
		if (constantsType[argName] == "sf::Font")
			constants[constantsId[argName]] = new sf::Font(loadFont(argValue));
		if (constantsType[argName] == "sf::Color")
			constants[constantsId[argName]] = new sf::Color(getColor(argValue));
	}
}
void Resources::clearTempData()
{
	for (auto it = constantsId.begin(); it != constantsId.end(); it++)
	{
		std::string name = it->first;
		int id = it->second;
		if (constantsType[name] == "bool")
			delete static_cast<bool*>(constants[id]);
		if (constantsType[name] == "int")
			delete static_cast<int*>(constants[id]);
		if (constantsType[name] == "float")
			delete static_cast<float*>(constants[id]);
		if (constantsType[name] == "std::string")
			delete static_cast<std::string*>(constants[id]);
		if (constantsType[name] == "sf::Font")
			delete static_cast<sf::Font*>(constants[id]);
		if (constantsType[name] == "sf::Color")
			delete static_cast<sf::Color*>(constants[id]);
	}
	constantsId.clear();
	constantsType.clear();
	constants.clear();
}

Resources::Resources() :
	FONT(*static_cast<sf::Font*>(constants[constantsId[toCStr(FONT)]])),
	CHARACTERS_SIZE(*static_cast<int*>(constants[constantsId[toCStr(CHARACTERS_SIZE)]])),
	ASCII_FIRST(*static_cast<int*>(constants[constantsId[toCStr(ASCII_FIRST)]])),
	ASCII_LAST(*static_cast<int*>(constants[constantsId[toCStr(ASCII_LAST)]])),
	OUTPUT_RESOLUTION_WIDTH(*static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_WIDTH)]])),
	OUTPUT_RESOLUTION_HEIGHT(*static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_HEIGHT)]])),
	COMPRESS_TEXT_MATRIX(*static_cast<bool*>(constants[constantsId[toCStr(COMPRESS_TEXT_MATRIX)]])),
	DISPERSION(*static_cast<int*>(constants[constantsId[toCStr(DISPERSION)]])),
	ADAPTIVE_BRIGHTNESS(*static_cast<bool*>(constants[constantsId[toCStr(ADAPTIVE_BRIGHTNESS)]])),
	BRIGHTNESS_FACTOR(*static_cast<float*>(constants[constantsId[toCStr(BRIGHTNESS_FACTOR)]])),
	BACKGROUND_COLOR(*static_cast<sf::Color*>(constants[constantsId[toCStr(BACKGROUND_COLOR)]])),
	IS_COLORED(*static_cast<bool*>(constants[constantsId[toCStr(IS_COLORED)]])),
	TEXT_COLOR(*static_cast<sf::Color*>(constants[constantsId[toCStr(TEXT_COLOR)]])),
	MULTICOLOR_CHARACTERS(*static_cast<bool*>(constants[constantsId[toCStr(MULTICOLOR_CHARACTERS)]])),
	INDEPENDENT_CHARACTERS_COLOR(*static_cast<bool*>(constants[constantsId[toCStr(INDEPENDENT_CHARACTERS_COLOR)]])),
	FRAMES_COUNT(*static_cast<int*>(constants[constantsId[toCStr(FRAMES_COUNT)]])),
	INPUT_FOLDER(*static_cast<std::string*>(constants[constantsId[toCStr(INPUT_FOLDER)]])),
	INPUT_FILENAME_EXTENSION(*static_cast<std::string*>(constants[constantsId[toCStr(INPUT_FILENAME_EXTENSION)]])),
	OUTPUT_FOLDER(*static_cast<std::string*>(constants[constantsId[toCStr(OUTPUT_FOLDER)]])),
	OUTPUT_FILENAME_EXTENSION(*static_cast<std::string*>(constants[constantsId[toCStr(OUTPUT_FILENAME_EXTENSION)]])),
	THREADS_COUNT(*static_cast<int*>(constants[constantsId[toCStr(THREADS_COUNT)]]))
{ }

Resources& RS()
{
	static bool once = false;
	if (!once)
		Resources::loadConstants();
	static Resources resources;
	if (!once)
	{
		Resources::clearTempData();
		once = true;
	}
	return resources;
}
