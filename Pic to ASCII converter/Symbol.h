#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Color.hpp>

class Symbol
{
private:
	int index_;
	int charCode_;
	bool isBold_;
	sf::Color color_;
	sf::Text text_;
public:
	Symbol(int index = 0, sf::Color color = sf::Color::White);
	int getCharCode() const;
	bool isBold() const;
	void set(int index, sf::Color color = sf::Color::White);
	void setPosition(float x, float y);
	void draw() const;
};
