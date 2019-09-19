#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

class Symbol
{
private:
	int id_;
	int charCode_;
	bool isBold_;
	sf::Color color_;
	sf::Text text_;
public:
	explicit Symbol(int id = 0, sf::Color color = sf::Color::White);
	int getCharCode() const;
	bool isBold() const;
	sf::Vector2i getSize() const;
	int getBrightness() const;
	void set(int id, sf::Color color = sf::Color::White);
	void setPosition(float x, float y);
	void draw() const;
};
