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

	RS().charCount = (RS().ASCII_LAST - RS().ASCII_FIRST + 1) * 2;

	sf::Text text;
	text.setFont(RS().FONT);
	text.setCharacterSize(RS().CHARACTERS_SIZE);
	text.setString(std::string(1, RS().ASCII_FIRST));
	int symbolWidth = 2 * text.getLocalBounds().left + text.getLocalBounds().width;

	int symbolHeight = RS().FONT.getLineSpacing(RS().CHARACTERS_SIZE);

	int matrixWidth = RS().OUTPUT_RESOLUTION_WIDTH / symbolWidth;
	int matrixHeight = RS().OUTPUT_RESOLUTION_HEIGHT / symbolHeight;

	std::map<std::thread::id, std::vector<std::vector<Symbol>>> matrices;

	std::vector<std::pair<int, int>> glyphBrightness;
	int minGlyphBrightness;
	int maxGlyphBrightness;
	int glyphBrightnessRange;
	auto calcGlyphBrightness = [&]() -> void
	{
		for (int i = 0; i < RS().charCount; i++)
			RS().FONT.getGlyph(Symbol(i).getCharCode(), RS().CHARACTERS_SIZE, Symbol(i).isBold());
		sf::Image glyphImage(RS().FONT.getTexture(RS().CHARACTERS_SIZE).copyToImage());

		std::map<int, int> glyphIdByBrightness;
		for (int i = 0; i < RS().charCount; i++)
		{
			const sf::Glyph& glyph = RS().FONT.getGlyph(Symbol(i).getCharCode(), RS().CHARACTERS_SIZE, Symbol(i).isBold());
			const sf::IntRect& bounds = glyph.textureRect;
			int curBrightness = 0;
			for (int y = bounds.top; y < bounds.top + bounds.height; y++)
			{
				for (int x = bounds.left; x < bounds.left + bounds.width; x++)
					curBrightness += glyphImage.getPixel(x, y).a;
			}
			curBrightness /= symbolHeight * symbolWidth;
			if (glyphIdByBrightness.count(curBrightness) == 0)
				glyphIdByBrightness[curBrightness] = i;
		}
		glyphBrightness.assign(glyphIdByBrightness.begin(), glyphIdByBrightness.end());

		minGlyphBrightness = glyphBrightness.front().first;
		maxGlyphBrightness = glyphBrightness.back().first;
		glyphBrightnessRange = maxGlyphBrightness - minGlyphBrightness;

		static const int ALLOWANCE = 2;
		int maxInterval = (float)glyphBrightnessRange / (glyphBrightness.size() - 1) * ALLOWANCE;
		int glyphsCount = glyphBrightness.size();
		static const float LEFT_TRIM_PERCENT = 0.05;
		int redundantLeft = 0;
		for (int i = 0; i < LEFT_TRIM_PERCENT * glyphsCount; i++)
		{
			int curInterval = glyphBrightness[i + 1].first - glyphBrightness[i].first;
			if (maxInterval < curInterval)
				redundantLeft = i + 1;
		}
		glyphBrightness.erase(glyphBrightness.begin(), glyphBrightness.begin() + redundantLeft);
		static const float RIGHT_TRIM_PERCENT = 0.05;
		int redundantRight = 0;
		for (int i = 0; i < RIGHT_TRIM_PERCENT * glyphsCount; i++)
		{
			int left = glyphBrightness.size() - redundantRight - 2;
			int right = glyphBrightness.size() - redundantRight - 1;
			int curInterval = glyphBrightness[right].first - glyphBrightness[left].first;
			if (maxInterval < curInterval)
				redundantRight = i + 1;
		}
		glyphBrightness.erase(glyphBrightness.end() - redundantRight, glyphBrightness.end());
	};
	calcGlyphBrightness();

	auto findGlyph = [&](int brightness) -> int
	{
		int brightnessForGlyph = minGlyphBrightness + brightness / 255.0f * glyphBrightnessRange;
		auto it = std::lower_bound(glyphBrightness.begin(), glyphBrightness.end(), std::make_pair(brightnessForGlyph, 0));
		if (it == glyphBrightness.end())
			it--;
		int index = it - glyphBrightness.begin();
		if (index == 0)
			index++;
		int dist1 = glyphBrightness[index].first - brightnessForGlyph;
		int dist2 = brightnessForGlyph - glyphBrightness[index - 1].first;
		if (dist1 < dist2)
			return index;
		return index - 1;
	};

	auto transformPicture = [&](const std::string& sourceFilename, const std::string& resultFilename) -> void
	{
		sf::Image sourceImage;
		sourceImage.loadFromFile(sourceFilename);

		sf::Image rescaledImage = rescaleImage(sourceImage, matrixWidth, matrixHeight);

		int minPixelBrightness;
		int maxPixelBrightness;
		if (RS().ADAPTIVE_BRIGHTNESS == false)
		{
			minPixelBrightness = 0;
			maxPixelBrightness = 255;
		}
		//if (RS().ADAPTIVE_BRIGHTNESS == 1)
		//{
		//	minPixelBrightness = 255;
		//	maxPixelBrightness = 0;
		//	for (int i = 0; i < matrixHeight; i++)
		//	{
		//		for (int j = 0; j < matrixWidth; j++)
		//		{
		//			sf::Color color = rescaledImage.getPixel(j, i);
		//			int brightness = std::max(color.r, std::max(color.g, color.b));
		//			minPixelBrightness = std::min(minPixelBrightness, brightness);
		//			maxPixelBrightness = std::max(maxPixelBrightness, brightness);
		//		}
		//	}
		//}
		if (RS().ADAPTIVE_BRIGHTNESS == true)
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
			static const int ALLOWANCE = 2;
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

		auto& matrix = matrices[std::this_thread::get_id()];
		for (int i = 0; i < matrixHeight; i++)
		{
			for (int j = 0; j < matrixWidth; j++)
			{
				sf::Color color = rescaledImage.getPixel(j, i);
				int brightness = std::max(color.r, std::max(color.g, color.b));
				if (!RS().IS_COLORED)
				{
					int gray = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
					if (RS().TEXT_COLOR == sf::Color::White && RS().BACKGROUND_COLOR == sf::Color::Black)
						color = sf::Color(gray, gray, gray);
					else
						color = setBrightness(RS().TEXT_COLOR, brightness);
				}
				if (RS().INDEPENDENT_CHARACTERS_COLOR)
					color = RS().TEXT_COLOR;
				brightness = std::max(brightness, minPixelBrightness);
				brightness = std::min(brightness, maxPixelBrightness);
				int curGlyphBrightness = (float)(brightness - minPixelBrightness) / (maxPixelBrightness - minPixelBrightness) * 255;
				int index = glyphBrightness[findGlyph(curGlyphBrightness * RS().BRIGHTNESS_FACTOR)].second;
				int symbIndex = index + rand32() % RS().DISPERSION - RS().DISPERSION / 2;
				symbIndex = std::max(symbIndex, 0);
				symbIndex = std::min(symbIndex, RS().charCount - 1); // TODO: исправить
				matrix[i][j].set(symbIndex, color);
				matrix[i][j].setPosition(j * symbolWidth, i * symbolHeight);
			}
		}

		////////////////////////////// BEG

		sf::Image symbMatrix;
		RS().windows[std::this_thread::get_id()].clear(RS().BACKGROUND_COLOR);
		for (int i = 0; i < matrixHeight; i++)
		{
			for (int j = 0; j < matrixWidth; j++)
				matrix[i][j].draw();
		}
		symbMatrix = getScreenshot();
		RS().windows[std::this_thread::get_id()].display();

		symbMatrix.saveToFile(resultFilename);

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

		////////////////////////////// END
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

	auto threadsProcess = [&]() -> void
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
	};
	threadsProcess();

	return 0;
}
