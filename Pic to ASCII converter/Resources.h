#pragma once

/*
Resources / constants singleton class
*/

#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Symbol.h"

class Resources
{
	friend Resources& RS();
private:
	static void initConstantsInfo();
	static void loadConstants();
	static void clearTempData();
public:
	const sf::Font FONT;
	const int CHARACTERS_SIZE;
	const int ASCII_FIRST;
	const int ASCII_LAST;
	const int OUTPUT_RESOLUTION_WIDTH;
	const int OUTPUT_RESOLUTION_HEIGHT;
	const bool COMPRESS_TEXT_MATRIX;
	const int DISPERSION;
	const bool ADAPTIVE_BRIGHTNESS;
	const float BRIGHTNESS_FACTOR;
	const sf::Color BACKGROUND_COLOR;
	const bool IS_COLORED;
	const sf::Color TEXT_COLOR;
	const bool MULTICOLOR_CHARACTERS;
	const bool INDEPENDENT_CHARACTERS_COLOR;
	const int FRAMES_COUNT;
	const std::string INPUT_FOLDER;
	const std::string INPUT_FILENAME_EXTENSION;
	const std::string OUTPUT_FOLDER;
	const std::string OUTPUT_FILENAME_EXTENSION;
	const int THREADS_COUNT;

	int charCount;
	std::map<std::thread::id, sf::RenderWindow> windows;

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;

private:
	Resources();
};

Resources& RS();
