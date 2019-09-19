#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <iterator>
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

	RS().charCount = (RS().ASCII_LAST - RS().ASCII_FIRST + 1) * 2;

	for (int i = 0; i < RS().charCount; i++)
		RS().FONT.getGlyph(Symbol(i).getCharCode(), RS().CHARACTERS_SIZE, Symbol(i).isBold());
	sf::Image glyphImage(RS().FONT.getTexture(RS().CHARACTERS_SIZE).copyToImage());

	std::set<Symbol> symbols;
	auto initSymbolsByBrightness = [&]() -> void
	{
		int minBright = INT_MAX;
		int maxBright = 0;
		std::vector<std::pair<int, Symbol>> glyphsBright;
		for (int i = 0; i < RS().charCount; i++)
		{
			Symbol symbol(i);
			const sf::Glyph& glyph = RS().FONT.getGlyph(symbol.getCharCode(), RS().CHARACTERS_SIZE, symbol.isBold());
			const sf::IntRect& bounds = glyph.textureRect;
			int curBrightness = 0;
			for (int y = bounds.top; y < bounds.top + bounds.height; y++)
			{
				for (int x = bounds.left; x < bounds.left + bounds.width; x++)
					curBrightness += glyphImage.getPixel(x, y).a;
			}
			minBright = std::min(minBright, curBrightness);
			maxBright = std::max(maxBright, curBrightness);
			glyphsBright.push_back(std::make_pair(curBrightness, symbol));
		}

		for (int i = 0; i < glyphsBright.size(); i++)
			glyphsBright[i].first = (float)(glyphsBright[i].first - minBright) / (maxBright - minBright) * 255;
		std::map<int, Symbol> temp(glyphsBright.begin(), glyphsBright.end());
		glyphsBright.assign(temp.begin(), temp.end());

		static const float ALLOWANCE = 1.5;
		int maxInterval = (float)255 / (glyphsBright.size() - 1) * ALLOWANCE;
		static const float LEFT_TRIM_PERCENT = 0.05;
		int redundantLeft = 0;
		for (int i = 0; i < LEFT_TRIM_PERCENT * glyphsBright.size(); i++)
		{
			int left = i;
			int right = i + 1;
			int curInterval = glyphsBright[right].first - glyphsBright[left].first;
			if (maxInterval < curInterval)
				redundantLeft = i + 1;
		}
		static const float RIGHT_TRIM_PERCENT = 0.05;
		int redundantRight = 0;
		for (int i = 0; i < RIGHT_TRIM_PERCENT * glyphsBright.size(); i++)
		{
			int left = glyphsBright.size() - redundantRight - 2;
			int right = glyphsBright.size() - redundantRight - 1;
			int curInterval = glyphsBright[right].first - glyphsBright[left].first;
			if (maxInterval < curInterval)
				redundantRight = i + 1;
		}
		
		//////
		redundantLeft = 0;
		redundantRight = 0;
		//////

		for (int i = redundantLeft; i < glyphsBright.size() - redundantRight; i++)
			symbols.insert(glyphsBright[i].second);
	};
	initSymbolsByBrightness();

	int symbolWidth;
	int symbolHeight;
	auto filterSymbolsByBounds = [&]() -> void
	{
		std::map<int, std::vector<Symbol>, std::greater<int>> counterWidth;
		std::map<int, std::vector<Symbol>, std::greater<int>> counterHeight;
		for (auto it = symbols.begin(); it != symbols.end(); it++)
		{
			const sf::Glyph& glyph = RS().FONT.getGlyph(it->getCharCode(), RS().CHARACTERS_SIZE, it->isBold());
			const sf::IntRect& bounds = glyph.textureRect;
			counterWidth[bounds.width].push_back(*it);
			counterHeight[bounds.height].push_back(*it);
		}

		// TODO: проверить на корректность

		int symbolsCount = symbols.size();
		static const float TRIM_PERCENT = 0.1;

		int totalSumW = 0;
		auto itW = counterWidth.begin();
		symbolWidth = itW->first;
		while (true)
		{
			auto left = itW;
			auto right = ++itW;
			if (left->first == right->first || TRIM_PERCENT * symbolsCount < totalSumW + left->second.size())
				break;
			for (int i = 0; i < left->second.size(); i++)
				symbols.erase(left->second[i]);
			totalSumW += left->second.size();
			symbolWidth = right->first;
		}

		int totalSumH = 0;
		auto itH = counterHeight.begin();
		symbolHeight = itH->first;
		while (true)
		{
			auto left = itH;
			auto right = ++itH;
			if (left->first == left->first || TRIM_PERCENT * symbolsCount < totalSumH + left->second.size())
				break;
			for (int i = 0; i < left->second.size(); i++)
				symbols.erase(left->second[i]);
			totalSumH += left->second.size();
			symbolHeight = right->first;
		}

		// TODO: Тест - картинка из запятых

		// TODO: Центрировать символ в ячейке
	};
	//filterSymbolsByBounds();

	//////
	symbolWidth = 7;
	symbolHeight = 14;
	//////

	int matrixWidth = RS().OUTPUT_RESOLUTION_WIDTH / symbolWidth;
	int matrixHeight = RS().OUTPUT_RESOLUTION_HEIGHT / symbolHeight;

	std::vector<std::pair<int, Symbol>> glyphsBrightness;
	auto assignGlyphBrightness = [&]() -> void
	{
		int minBright = INT_MAX;
		int maxBright = 0;
		for (auto it = symbols.begin(); it != symbols.end(); it++)
		{
			const sf::Glyph& glyph = RS().FONT.getGlyph(it->getCharCode(), RS().CHARACTERS_SIZE, it->isBold());
			const sf::IntRect& bounds = glyph.textureRect;
			int curBrightness = 0;
			for (int y = bounds.top; y < bounds.top + bounds.height; y++)
			{
				for (int x = bounds.left; x < bounds.left + bounds.width; x++)
					curBrightness += glyphImage.getPixel(x, y).a;
			}
			minBright = std::min(minBright, curBrightness);
			maxBright = std::max(maxBright, curBrightness);
			glyphsBrightness.push_back(std::make_pair(curBrightness, *it));
		}
		for (int i = 0; i < glyphsBrightness.size(); i++)
			glyphsBrightness[i].first = (float)(glyphsBrightness[i].first - minBright) / (maxBright - minBright) * 255;
		std::map<int, Symbol> temp(glyphsBrightness.begin(), glyphsBrightness.end());
		glyphsBrightness.assign(temp.begin(), temp.end());
	};
	assignGlyphBrightness();

	auto getSymbol = [&](int brightness) -> Symbol
	{
		auto it = std::lower_bound(glyphsBrightness.begin(), glyphsBrightness.end(), std::make_pair(brightness, Symbol()));
		if (it == glyphsBrightness.end())
			it--;
		int index = it - glyphsBrightness.begin();
		if (index == 0)
			index++;
		int dist1 = glyphsBrightness[index].first - brightness;
		int dist2 = brightness - glyphsBrightness[index - 1].first;
		int symbIndex = dist1 < dist2 ? index : index - 1;

		symbIndex = symbIndex + rand32() % RS().DISPERSION - RS().DISPERSION / 2;
		symbIndex = std::max(symbIndex, 0);
		symbIndex = std::min(symbIndex, (int)glyphsBrightness.size() - 1);

		return glyphsBrightness[symbIndex].second;
	};

	std::map<std::thread::id, std::vector<std::vector<Symbol>>> matrices;
	auto transformPicture = [&](const std::string& sourceFilename, const std::string& resultFilename) -> void
	{
		sf::Image sourceImage;
		sourceImage.loadFromFile(sourceFilename);

		sf::Image rescaledImage = rescaleImage(sourceImage, matrixWidth, matrixHeight);

		int minPixelBrightness;
		int maxPixelBrightness;
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
			static const float ALLOWANCE = 2;
			int maxInterval = (float)(maxPixelBrightness - minPixelBrightness) / (pixelsBrightness.size() - 1) * ALLOWANCE;
			int elementsCount = matrixHeight * matrixWidth;
			static const float LEFT_TRIM_PERCENT = 0.05;
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
			static const float RIGHT_TRIM_PERCENT = 0.05;
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

		if (RS().INDEPENDENT_CHARACTERS_COLOR)
		{
			auto& matrix = matrices[std::this_thread::get_id()];
			for (int i = 0; i < matrixHeight; i++)
			{
				for (int j = 0; j < matrixWidth; j++)
				{
					sf::Color color = rescaledImage.getPixel(j, i);
					int brightness = std::max(color.r, std::max(color.g, color.b));
					brightness = std::max(brightness, minPixelBrightness);
					brightness = std::min(brightness, maxPixelBrightness);
					int curGlyphBrightness = (float)(brightness - minPixelBrightness) / (maxPixelBrightness - minPixelBrightness) * 255;
					matrix[i][j] = getSymbol(curGlyphBrightness * RS().BRIGHTNESS_FACTOR);
					matrix[i][j].setPosition(j * symbolWidth, i * symbolHeight);
					matrix[i][j].setColor(RS().TEXT_COLOR);
				}
			}

			RS().windows[std::this_thread::get_id()].clear(RS().BACKGROUND_COLOR);

			for (int i = 0; i < matrixHeight; i++)
			{
				for (int j = 0; j < matrixWidth; j++)
					matrix[i][j].draw();
			}
			getScreenshot().saveToFile(resultFilename);

			RS().windows[std::this_thread::get_id()].display();
		}

		//auto& matrix = matrices[std::this_thread::get_id()];
		//for (int i = 0; i < matrixHeight; i++)
		//{
		//	for (int j = 0; j < matrixWidth; j++)
		//	{
		//		sf::Color color = rescaledImage.getPixel(j, i);
		//		int brightness = std::max(color.r, std::max(color.g, color.b));
		//		if (!RS().IS_COLORED)
		//		{
		//			int gray = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
		//			if (RS().TEXT_COLOR == sf::Color::White && RS().BACKGROUND_COLOR == sf::Color::Black)
		//				color = sf::Color(gray, gray, gray);
		//			else
		//				color = setBrightness(RS().TEXT_COLOR, brightness);
		//		}
		//		if (RS().STABLE_TEXT_COLOR)
		//			color = RS().TEXT_COLOR;
		//		brightness = std::max(brightness, minPixelBrightness);
		//		brightness = std::min(brightness, maxPixelBrightness);
		//		int curGlyphBrightness = (float)(brightness - minPixelBrightness) / (maxPixelBrightness - minPixelBrightness) * 255;
		//		int index = glyphsBrightness[getSymbol(curGlyphBrightness * RS().BRIGHTNESS_FACTOR)].second;
		//		int symbIndex = index + rand32() % RS().DISPERSION - RS().DISPERSION / 2;
		//		symbIndex = std::max(symbIndex, 0);
		//		symbIndex = std::min(symbIndex, RS().CHAR_COUNT - 1);
		//		matrix[i][j].set(symbIndex, color);
		//		matrix[i][j].setPosition(j * RS().SYMBOL_WIDTH, i * RS().SYMBOL_HEIGHT);
		//	}
		//}

		//
		//
		//sf::Image symbMatrix;
		//RS().windows[std::this_thread::get_id()].clear(RS().BACKGROUND_COLOR);
		//for (int i = 0; i < RS().matrixHeight; i++)
		//{
		//	for (int j = 0; j < RS().matrixWidth; j++)
		//		matrix[i][j].draw();
		//}
		//symbMatrix = getScreenshot();
		//RS().windows[std::this_thread::get_id()].display();

		//sf::Image origMatrix = rescaleImage(sourceImage, RS().OUTPUT_RESOLUTION_WIDTH, RS().OUTPUT_RESOLUTION_HEIGHT);

		//sf::Image resultImage;
		//resultImage.create(RS().OUTPUT_RESOLUTION_WIDTH, RS().OUTPUT_RESOLUTION_HEIGHT);
		//for (int i = 0; i < RS().OUTPUT_RESOLUTION_HEIGHT; i++)
		//{
		//	for (int j = 0; j < RS().OUTPUT_RESOLUTION_WIDTH; j++)
		//	{
		//		sf::Color symbPixel = symbMatrix.getPixel(j, i);
		//		sf::Color origPixel = origMatrix.getPixel(j, i);
		//		if (!RS().IS_COLORED)
		//		{
		//			if (RS().TEXT_COLOR == sf::Color::White && RS().BACKGROUND_COLOR == sf::Color::Black)
		//			{
		//				int gray = 0.2126f * origPixel.r + 0.7152f * origPixel.g + 0.0722f * origPixel.b;
		//				origPixel = sf::Color(gray, gray, gray);
		//			}
		//			else
		//			{
		//				int brightness = std::max(origPixel.r, std::max(origPixel.g, origPixel.b));
		//				origPixel = setBrightness(RS().TEXT_COLOR, brightness);
		//			}
		//		}
		//		// TODO: STABLE_TEXT_COLOR
		//		float brightFactor = (float)symbPixel.r / 255;
		//		sf::Color resultPixel(origPixel.r * brightFactor, origPixel.g * brightFactor, origPixel.b * brightFactor);
		//		resultImage.setPixel(j, i, resultPixel);
		//	}
		//}
		//resultImage.saveToFile(resultFilename);
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
	};

	auto runThreads = [&]() -> void
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
	};
	runThreads();

	return 0;
}
