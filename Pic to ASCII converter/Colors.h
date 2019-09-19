#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>

class RGB;
class HSV;

class RGB
{
public:
	int r; // 0 <= r <= 255
	int g; // 0 <= g <= 255
	int b; // 0 <= b <= 255

	RGB(int r, int g, int b);
	RGB(const HSV& hsv);
	RGB(const sf::Color& color);
	sf::Color getColor() const;
};

class HSV
{
public:
	int h; // 0 <= h <= 359
	int s; // 0 <= s <= 255
	int v; // 0 <= v <= 255
	HSV(int h, int s, int v);
	HSV(const RGB& rgb);
	HSV(const sf::Color& color);
	sf::Color getColor() const;
};

sf::Color setSaturation(const sf::Color& color, int value);
sf::Color setBrightness(const sf::Color& color, int value);
sf::Color getGrayEquivalent(const sf::Color& color);

const sf::Color& getColor(const std::string& colorName);
