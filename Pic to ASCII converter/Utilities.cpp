#include "Utilities.h"

#include <thread>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Resources.h"

int rand32()
{
	return rand() << 16 | rand();
}

void drawFrame(sf::IntRect frame, sf::Color color)
{
	sf::RectangleShape shape(sf::Vector2f(frame.width - 2, frame.height - 2));
	shape.setPosition(frame.left + 1, frame.top + 1);
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineThickness(1);
	shape.setOutlineColor(color);
	RS().windows[std::this_thread::get_id()].draw(shape);
}

sf::Image getFromWindow(sf::IntRect region)
{
	sf::Texture texture;
	int windowW = RS().windows[std::this_thread::get_id()].getSize().x;
	int windowH = RS().windows[std::this_thread::get_id()].getSize().y;
	texture.create(windowW, windowH);
	texture.update(RS().windows[std::this_thread::get_id()]);
	sf::Image image(texture.copyToImage());
	sf::Image result;
	result.create(region.width, region.height);
	result.copy(image, 0, 0, region);
	return result;
}

sf::Image getScreenshot()
{
	int windowW = RS().windows[std::this_thread::get_id()].getSize().x;
	int windowH = RS().windows[std::this_thread::get_id()].getSize().y;
	return getFromWindow(sf::IntRect(0, 0, windowW, windowH));
}

sf::Image rescaleImage(const sf::Image& image, int width, int height)
{
	sf::Texture texture;
	texture.loadFromImage(image);
	sf::Sprite sprite(texture);
	float factorX = (float)width / image.getSize().x;
	float factorY = (float)height / image.getSize().y;
	sprite.setScale(factorX, factorY);
	RS().windows[std::this_thread::get_id()].clear();
	RS().windows[std::this_thread::get_id()].draw(sprite);
	sf::Image result = getFromWindow(sf::IntRect(0, 0, width, height));
	RS().windows[std::this_thread::get_id()].display();
	return result;
}
