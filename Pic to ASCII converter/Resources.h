#pragma once

/*
	Resources / constants singleton class
*/

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Glyph.h"

class Resources
{
	friend Resources& RS();
private:
	static void initConstantsInfo();
	static void loadConstants();
	static void clearTempData();
public:
	const sf::Font FONT; //CONFIG
	const int CHAR_SIZE; //CONFIG
	const int SYMBOL_WIDTH;
	const int SYMBOL_HEIGHT;
	const int ASCII_FIRST; //CONFIG
	const int ASCII_LAST; //CONFIG
	const int CHAR_COUNT;
	const float BRIGHTNESS_FACTOR; //CONFIG
	const int ADAPTIVE_BRIGHTNESS; //CONFIG
	const int DISPERSION; //CONFIG
	const int OUTPUT_RESOLUTION_WIDTH; //CONFIG
	const int OUTPUT_RESOLUTION_HEIGHT; //CONFIG
	const int MATRIX_WIDTH;
	const int MATRIX_HEIGHT;
	const sf::Color BACKGROUND_COLOR; //CONFIG
	const bool STABLE_TEXT_COLOR; //CONFIG
	const bool IS_COLORED; //CONFIG
	const sf::Color TEXT_COLOR; //CONFIG
	const int FRAMES_COUNT; //CONFIG
	const std::string FRAMES_FOLDER; //CONFIG
	const std::string FRAMES_FILENAME_EXTENSION; //CONFIG
	const std::string OUTPUT_FOLDER; //CONFIG
	const std::string OUTPUT_FILENAME_EXTENSION; //CONFIG
	const int THREADS_COUNT; //CONFIG

	std::map<std::thread::id, sf::RenderWindow> windows;
	std::map<std::thread::id, std::vector<std::vector<Glyph>>> matrices;

	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;
	Resources& operator=(Resources&&) = delete;

private:
	Resources();
};

Resources& RS();
