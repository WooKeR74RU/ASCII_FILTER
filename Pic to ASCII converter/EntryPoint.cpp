#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <functional>
#include <thread>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Resources.h"
#include "Symbol.h"
#include "Colors.h"
#include "Utilities.h"

int main()
{
	sf::RenderWindow uselessWindow(sf::VideoMode(0, 0), "", sf::Style::None); // mmm.. SFML
	uselessWindow.setVisible(false); // :3

	srand(time(nullptr));

	RS().charCount = RS().ASCII_LAST - RS().ASCII_FIRST + 1;
	if (RS().USE_BOLD)
		RS().charCount *= 2;

	for (int i = 0; i < RS().charCount; i++)
		RS().FONT.getGlyph(Symbol(i).getCharCode(), RS().CHARACTERS_SIZE, Symbol(i).isBold());
	RS().symbolsImage = RS().FONT.getTexture(RS().CHARACTERS_SIZE).copyToImage();

	std::vector<std::pair<int, int>> symbolsBrightness; // (symbolBrightness, symbolId)
	auto alignSymbolsBrightness = [&]() -> void
	{
		int minSymbolBrightness = INT_MAX;
		int maxSymbolBrightness = 0;
		for (int i = 0; i < symbolsBrightness.size(); i++)
		{
			minSymbolBrightness = std::min(minSymbolBrightness, symbolsBrightness[i].first);
			maxSymbolBrightness = std::max(maxSymbolBrightness, symbolsBrightness[i].first);
		}
		int symbolBrightnessRange = maxSymbolBrightness - minSymbolBrightness;
		for (int i = 0; i < symbolsBrightness.size(); i++)
			symbolsBrightness[i].first = (float)(symbolsBrightness[i].first - minSymbolBrightness) / symbolBrightnessRange * 255;

		auto compareByPriority = [](int symbolIdA, int symbolIdB) -> bool
		{
			char chA = Symbol(symbolIdA).getCharCode();
			char chB = Symbol(symbolIdB).getCharCode();
			static const int PRIORITY_SIZE = 94;
			static const char PRIORITY[PRIORITY_SIZE] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
				'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
				'w', 'x', 'y', 'z', '+', '*', '=', '-', '_', '.', ':', '^', '<', '>', '~', '\'', '"', '@', '!', '#', '$', '%',
				'&', '?', '`', ',', ';', '|', '/', '\\', '(', ')', '[', ']', '{', '}' };
			int indexA, indexB;
			for (int i = 0; i < PRIORITY_SIZE; i++)
			{
				if (chA == PRIORITY[i])
					indexA = i;
				if (chB == PRIORITY[i])
					indexB = i;
			}
			return indexA < indexB;
		};
		auto cmp = [&](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool
		{
			if (a.first == b.first)
				return compareByPriority(a.second, b.second);
			return a.first < b.first;
		};
		std::sort(symbolsBrightness.begin(), symbolsBrightness.end(), cmp);
		auto isEqual = [](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool
		{
			return a.first == b.first;
		};
		auto lastIt = std::unique(symbolsBrightness.begin(), symbolsBrightness.end(), isEqual);
		symbolsBrightness.resize(lastIt - symbolsBrightness.begin());
	};

	std::set<int> symbolsId;
	[&]() -> void
	{
		for (int i = 0; i < RS().charCount; i++)
			symbolsBrightness.push_back(std::make_pair(Symbol(i).getBrightness(), i));
		alignSymbolsBrightness();

		int symbolsCount = symbolsBrightness.size();
		static const float ALLOWANCE = 2;
		int maxInterval = 255.0f / (symbolsCount - 1) * ALLOWANCE;

		static const float LEFT_TRIM_PERCENT = 0.05;
		int redundantLeft = 0;
		for (int i = 0; i < LEFT_TRIM_PERCENT * symbolsCount; i++)
		{
			int curInterval = symbolsBrightness[i + 1].first - symbolsBrightness[i].first;
			if (maxInterval < curInterval)
				redundantLeft = i + 1;
		}

		static const float RIGHT_TRIM_PERCENT = 0.05;
		int redundantRight = 0;
		for (int i = 0; i < RIGHT_TRIM_PERCENT * symbolsCount; i++)
		{
			int curInterval = symbolsBrightness[symbolsCount - i - 1].first - symbolsBrightness[symbolsCount - i - 2].first;
			if (maxInterval < curInterval)
				redundantRight = i + 1;
		}

		for (int i = redundantLeft; i < symbolsBrightness.size() - redundantRight; i++)
			symbolsId.insert(symbolsBrightness[i].second);
		symbolsBrightness.clear();
	}();

	int symbolWidth;
	int symbolHeight;
	[&]() -> void
	{
		if (!RS().COMPRESS_TEXT_MATRIX)
		{
			sf::Text text;
			text.setFont(RS().FONT);
			text.setCharacterSize(RS().CHARACTERS_SIZE);
			text.setString(std::string(1, RS().ASCII_FIRST));
			symbolWidth = 2 * text.getLocalBounds().left + text.getLocalBounds().width;

			symbolHeight = RS().FONT.getLineSpacing(RS().CHARACTERS_SIZE);
		}
		else
		{
			std::map<int, std::vector<int>, std::greater<int>> counterWidth;
			std::map<int, std::vector<int>, std::greater<int>> counterHeight;
			for (auto it = symbolsId.begin(); it != symbolsId.end(); it++)
			{
				counterWidth[Symbol(*it).getSize().x].push_back(*it);
				counterHeight[Symbol(*it).getSize().y].push_back(*it);
			}

			int symbolsCount = symbolsId.size();
			static const float TRIM_PERCENT = 0.25;

			int totalSumW = 0;
			auto itW = counterWidth.begin();
			symbolWidth = itW->first + 1;
			while (true)
			{
				auto left = itW;
				auto right = ++itW;
				if (TRIM_PERCENT * symbolsCount < totalSumW + left->second.size())
					break;
				for (int i = 0; i < left->second.size(); i++)
					symbolsId.erase(left->second[i]);
				totalSumW += left->second.size();
				symbolWidth = right->first + 1;
			}

			int totalSumH = 0;
			auto itH = counterHeight.begin();
			symbolHeight = itH->first + 1;
			while (true)
			{
				auto left = itH;
				auto right = ++itH;
				if (TRIM_PERCENT * symbolsCount < totalSumH + left->second.size())
					break;
				for (int i = 0; i < left->second.size(); i++)
					symbolsId.erase(left->second[i]);
				totalSumH += left->second.size();
				symbolHeight = right->first + 1;
			}
		}

		symbolWidth += RS().COLUMNS_INDENT;
		symbolHeight += RS().LINES_INDENT;
	}();

	int matrixWidth = RS().OUTPUT_RESOLUTION_WIDTH / symbolWidth;
	int matrixHeight = RS().OUTPUT_RESOLUTION_HEIGHT / symbolHeight;

	[&]() -> void
	{
		for (auto it = symbolsId.begin(); it != symbolsId.end(); it++)
			symbolsBrightness.push_back(std::make_pair(Symbol(*it).getBrightness(), *it));
		alignSymbolsBrightness();
	}();
	
	auto getSymbolId = [&](int brightness) -> int
	{
		auto it = std::lower_bound(symbolsBrightness.begin(), symbolsBrightness.end(), std::make_pair(brightness, 0));
		if (it == symbolsBrightness.end())
			it--;
		int index = it - symbolsBrightness.begin();
		if (index == 0)
			index++;
		int dist1 = symbolsBrightness[index].first - brightness;
		int dist2 = brightness - symbolsBrightness[index - 1].first;
		index = dist1 < dist2 ? index : index - 1;

		index = index + rand32() % RS().DISPERSION - RS().DISPERSION / 2;
		index = std::max(index, 0);
		index = std::min(index, (int)symbolsBrightness.size() - 1);

		return symbolsBrightness[index].second;
	};

	std::map<std::thread::id, std::vector<std::vector<Symbol>>> matrices;
	auto transformPicture = [&](const std::string& sourceFilename, const std::string& resultFilename) -> void
	{
		sf::Image sourceImage;
		sourceImage.loadFromFile(sourceFilename);

		sf::Image rescaledImage = rescaleImage(sourceImage, matrixWidth, matrixHeight);

		int minPixelBrightness;
		int maxPixelBrightness;
		[&]() -> void
		{
			if (!RS().ADAPTIVE_BRIGHTNESS)
			{
				minPixelBrightness = 0;
				maxPixelBrightness = 255;
			}
			else
			{
				std::map<int, int> counter;
				for (int i = 0; i < matrixHeight; i++)
				{
					for (int j = 0; j < matrixWidth; j++)
					{
						sf::Color color = rescaledImage.getPixel(j, i);
						counter[std::max(color.r, std::max(color.g, color.b))]++;
					}
				}
				std::vector<std::pair<int, int>> pixelsBrightness(counter.begin(), counter.end());
				minPixelBrightness = pixelsBrightness.front().first;
				maxPixelBrightness = pixelsBrightness.back().first;
				static const float ALLOWANCE = 1.5;
				int maxInterval = (float)(maxPixelBrightness - minPixelBrightness) / (pixelsBrightness.size() - 1) * ALLOWANCE;
				int elementsCount = matrixHeight * matrixWidth;
				static const float LEFT_TRIM_PERCENT = 0.025;
				int leftRedundant = 0;
				for (int i = 0; i < pixelsBrightness.size() - 1; i++)
				{
					if (LEFT_TRIM_PERCENT * elementsCount < leftRedundant + pixelsBrightness[i].second)
						break;
					int curInterval = pixelsBrightness[i + 1].first - pixelsBrightness[i].first;
					if (maxInterval < curInterval)
						minPixelBrightness = pixelsBrightness[i + 1].first;
					leftRedundant += pixelsBrightness[i].second;
				}
				static const float RIGHT_TRIM_PERCENT = 0.025;
				int rightRedundant = 0;
				for (int i = pixelsBrightness.size() - 1; 0 < i; i--)
				{
					if (RIGHT_TRIM_PERCENT * elementsCount < rightRedundant + pixelsBrightness[i].second)
						break;
					int curInterval = pixelsBrightness[i].first - pixelsBrightness[i - 1].first;
					if (maxInterval < curInterval)
						maxPixelBrightness = pixelsBrightness[i - 1].first;
					rightRedundant += pixelsBrightness[i].second;
				}
			}
		}();

		auto& matrix = matrices[std::this_thread::get_id()];
		for (int i = 0; i < matrixHeight; i++)
		{
			for (int j = 0; j < matrixWidth; j++)
			{
				sf::Color pixel = rescaledImage.getPixel(j, i);
				int brightness = std::max(pixel.r, std::max(pixel.g, pixel.b));
				brightness = std::max(brightness, minPixelBrightness);
				brightness = std::min(brightness, maxPixelBrightness);
				int curSymbolBrightness = (float)(brightness - minPixelBrightness) / (maxPixelBrightness - minPixelBrightness) * 255;

				sf::Color symbolColor;
				if (RS().INDEPENDENT_CHARACTERS_COLOR)
				{
					symbolColor = RS().TEXT_COLOR;
				}
				else
				{
					if (RS().MULTICOLOR_CHARACTERS)
					{
						symbolColor = sf::Color::White;
					}
					else
					{
						if (RS().IS_COLORED)
						{
							symbolColor = pixel;
						}
						else
						{
							if (RS().TEXT_COLOR == sf::Color::White)
								symbolColor = getGrayEquivalent(pixel);
							else
								symbolColor = setBrightness(RS().TEXT_COLOR, brightness);
						}
					}
				}
				
				matrix[i][j].set(getSymbolId(curSymbolBrightness * RS().BRIGHTNESS_FACTOR), symbolColor);
				matrix[i][j].setPosition(j * symbolWidth, i * symbolHeight);
			}
		}

		RS().windows[std::this_thread::get_id()].clear();
		for (int i = 0; i < matrixHeight; i++)
		{
			for (int j = 0; j < matrixWidth; j++)
				matrix[i][j].draw();
		}
		sf::Image resImage = getScreenshot();
		RS().windows[std::this_thread::get_id()].display();

		if (!RS().INDEPENDENT_CHARACTERS_COLOR && RS().MULTICOLOR_CHARACTERS)
		{
			sf::Image originalImage = rescaleImage(sourceImage, RS().OUTPUT_RESOLUTION_WIDTH, RS().OUTPUT_RESOLUTION_HEIGHT);
			for (int i = 0; i < RS().OUTPUT_RESOLUTION_HEIGHT; i++)
			{
				for (int j = 0; j < RS().OUTPUT_RESOLUTION_WIDTH; j++)
				{
					sf::Color symbPixel = resImage.getPixel(j, i);
					sf::Color origPixel = originalImage.getPixel(j, i);
					if (!RS().IS_COLORED)
					{
						if (RS().TEXT_COLOR == sf::Color::White)
							origPixel = getGrayEquivalent(origPixel);
						else
							origPixel = setBrightness(RS().TEXT_COLOR, std::max(origPixel.r, std::max(origPixel.g, origPixel.b)));
					}
					float brightFactor = (float)symbPixel.r	/ 255;
					sf::Color resPixel(origPixel.r * brightFactor, origPixel.g * brightFactor, origPixel.b * brightFactor);
					resImage.setPixel(j, i, resPixel);
				}
			}
		}

		resImage.saveToFile(resultFilename);
	};

	auto processPart = [&](int firstFrame, int lastFrame) -> void
	{
		RS().windows[std::this_thread::get_id()].create(sf::VideoMode(RS().OUTPUT_RESOLUTION_WIDTH,
			RS().OUTPUT_RESOLUTION_HEIGHT), "", sf::Style::None);
		RS().windows[std::this_thread::get_id()].setVisible(false);
		matrices[std::this_thread::get_id()].resize(matrixHeight, std::vector<Symbol>(matrixWidth));
		for (int i = firstFrame; i <= lastFrame; i++)
		{
			std::string sourceFilename = RS().INPUT_FOLDER + "/" + std::to_string(i + 1) + "." + RS().INPUT_FILENAME_EXTENSION;
			std::string resultFilename = RS().OUTPUT_FOLDER + "/" + std::to_string(i + 1) + "." + RS().OUTPUT_FILENAME_EXTENSION;
			transformPicture(sourceFilename, resultFilename);
		}
		RS().windows[std::this_thread::get_id()].close();
		matrices[std::this_thread::get_id()].clear();
	};

	[&]() -> void
	{
		std::vector<std::thread> threads;
		int partSize = RS().FRAMES_COUNT / RS().THREADS_COUNT;
		for (int i = 0; i < RS().THREADS_COUNT; i++)
		{
			int first = i * partSize;
			int last = first + partSize - 1;
			if (i == RS().THREADS_COUNT - 1)
				last += RS().FRAMES_COUNT % RS().THREADS_COUNT;
			if (first <= last)
				threads.emplace_back(processPart, first, last);
		}
		for (int i = 0; i < threads.size(); i++)
			threads[i].join();

		RS().windows.clear();
		matrices.clear();
	}();

	return 0;
}
