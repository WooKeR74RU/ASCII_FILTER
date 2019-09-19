#include "Resources.h"

#include <vector>
#include <fstream>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

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
	constantsType[toCStr(CHAR_SIZE)] = "int";
	constantsType[toCStr(SYMBOL_WIDTH)] = "int";
	constantsType[toCStr(SYMBOL_HEIGHT)] = "int";
	constantsType[toCStr(ASCII_FIRST)] = "int";
	constantsType[toCStr(ASCII_LAST)] = "int";
	constantsType[toCStr(CHAR_COUNT)] = "int";
	constantsType[toCStr(BRIGHTNESS_FACTOR)] = "float";
	constantsType[toCStr(ADAPTIVE_BRIGHTNESS)] = "int";
	constantsType[toCStr(DISPERSION)] = "int";
	constantsType[toCStr(OUTPUT_RESOLUTION_WIDTH)] = "int";
	constantsType[toCStr(OUTPUT_RESOLUTION_HEIGHT)] = "int";
	constantsType[toCStr(MATRIX_WIDTH)] = "int";
	constantsType[toCStr(MATRIX_HEIGHT)] = "int";
	constantsType[toCStr(BACKGROUND_COLOR)] = "sf::Color";
	constantsType[toCStr(STABLE_TEXT_COLOR)] = "bool";
	constantsType[toCStr(IS_COLORED)] = "bool";
	constantsType[toCStr(TEXT_COLOR)] = "sf::Color";
	constantsType[toCStr(FRAMES_COUNT)] = "int";
	constantsType[toCStr(FRAMES_FOLDER)] = "std::string";
	constantsType[toCStr(FRAMES_FILENAME_EXTENSION)] = "std::string";
	constantsType[toCStr(OUTPUT_FOLDER)] = "std::string";
	constantsType[toCStr(OUTPUT_FILENAME_EXTENSION)] = "std::string";
	constantsType[toCStr(THREADS_COUNT)] = "int";

	int id = 0;
	constantsId[toCStr(FONT)] = id++;
	constantsId[toCStr(CHAR_SIZE)] = id++;
	constantsId[toCStr(SYMBOL_WIDTH)] = id++;
	constantsId[toCStr(SYMBOL_HEIGHT)] = id++;
	constantsId[toCStr(ASCII_FIRST)] = id++;
	constantsId[toCStr(ASCII_LAST)] = id++;
	constantsId[toCStr(CHAR_COUNT)] = id++;
	constantsId[toCStr(BRIGHTNESS_FACTOR)] = id++;
	constantsId[toCStr(ADAPTIVE_BRIGHTNESS)] = id++;
	constantsId[toCStr(DISPERSION)] = id++;
	constantsId[toCStr(OUTPUT_RESOLUTION_WIDTH)] = id++;
	constantsId[toCStr(OUTPUT_RESOLUTION_HEIGHT)] = id++;
	constantsId[toCStr(MATRIX_WIDTH)] = id++;
	constantsId[toCStr(MATRIX_HEIGHT)] = id++;
	constantsId[toCStr(BACKGROUND_COLOR)] = id++;
	constantsId[toCStr(STABLE_TEXT_COLOR)] = id++;
	constantsId[toCStr(IS_COLORED)] = id++;
	constantsId[toCStr(TEXT_COLOR)] = id++;
	constantsId[toCStr(FRAMES_COUNT)] = id++;
	constantsId[toCStr(FRAMES_FOLDER)] = id++;
	constantsId[toCStr(FRAMES_FILENAME_EXTENSION)] = id++;
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

	int asciiFirst = *static_cast<int*>(constants[constantsId[toCStr(ASCII_FIRST)]]);
	int asciiLast = *static_cast<int*>(constants[constantsId[toCStr(ASCII_LAST)]]);
	constants[constantsId[toCStr(CHAR_COUNT)]] = new int((asciiLast - asciiFirst + 1) * 2);

	const sf::Font& font = *static_cast<sf::Font*>(constants[constantsId[toCStr(FONT)]]);
	int charSize = *static_cast<int*>(constants[constantsId[toCStr(CHAR_SIZE)]]);
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(charSize);
	text.setString(std::string(1, asciiFirst));
	int symbolWidth = 2 * text.getLocalBounds().left + text.getLocalBounds().width;
	constants[constantsId[toCStr(SYMBOL_WIDTH)]] = new int(symbolWidth);

	int symbolHeight = font.getLineSpacing(charSize);
	constants[constantsId[toCStr(SYMBOL_HEIGHT)]] = new int(symbolHeight);

	int outputResolutionWidth = *static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_WIDTH)]]);
	int matrixWidth = outputResolutionWidth / symbolWidth;
	constants[constantsId[toCStr(MATRIX_WIDTH)]] = new int(matrixWidth);

	int outputResolutionHeight = *static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_HEIGHT)]]);
	int matrixHeight = outputResolutionHeight / symbolHeight;
	constants[constantsId[toCStr(MATRIX_HEIGHT)]] = new int(matrixHeight);
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
	CHAR_SIZE(*static_cast<int*>(constants[constantsId[toCStr(CHAR_SIZE)]])),
	SYMBOL_WIDTH(*static_cast<int*>(constants[constantsId[toCStr(SYMBOL_WIDTH)]])),
	SYMBOL_HEIGHT(*static_cast<int*>(constants[constantsId[toCStr(SYMBOL_HEIGHT)]])),
	ASCII_FIRST(*static_cast<int*>(constants[constantsId[toCStr(ASCII_FIRST)]])),
	ASCII_LAST(*static_cast<int*>(constants[constantsId[toCStr(ASCII_LAST)]])),
	CHAR_COUNT(*static_cast<int*>(constants[constantsId[toCStr(CHAR_COUNT)]])),
	BRIGHTNESS_FACTOR(*static_cast<float*>(constants[constantsId[toCStr(BRIGHTNESS_FACTOR)]])),
	ADAPTIVE_BRIGHTNESS(*static_cast<int*>(constants[constantsId[toCStr(ADAPTIVE_BRIGHTNESS)]])),
	DISPERSION(*static_cast<int*>(constants[constantsId[toCStr(DISPERSION)]])),
	OUTPUT_RESOLUTION_WIDTH(*static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_WIDTH)]])),
	OUTPUT_RESOLUTION_HEIGHT(*static_cast<int*>(constants[constantsId[toCStr(OUTPUT_RESOLUTION_HEIGHT)]])),
	MATRIX_WIDTH(*static_cast<int*>(constants[constantsId[toCStr(MATRIX_WIDTH)]])),
	MATRIX_HEIGHT(*static_cast<int*>(constants[constantsId[toCStr(MATRIX_HEIGHT)]])),
	BACKGROUND_COLOR(*static_cast<sf::Color*>(constants[constantsId[toCStr(BACKGROUND_COLOR)]])),
	STABLE_TEXT_COLOR(*static_cast<bool*>(constants[constantsId[toCStr(STABLE_TEXT_COLOR)]])),
	IS_COLORED(*static_cast<bool*>(constants[constantsId[toCStr(IS_COLORED)]])),
	TEXT_COLOR(*static_cast<sf::Color*>(constants[constantsId[toCStr(TEXT_COLOR)]])),
	FRAMES_COUNT(*static_cast<int*>(constants[constantsId[toCStr(FRAMES_COUNT)]])),
	FRAMES_FOLDER(*static_cast<std::string*>(constants[constantsId[toCStr(FRAMES_FOLDER)]])),
	FRAMES_FILENAME_EXTENSION(*static_cast<std::string*>(constants[constantsId[toCStr(FRAMES_FILENAME_EXTENSION)]])),
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
