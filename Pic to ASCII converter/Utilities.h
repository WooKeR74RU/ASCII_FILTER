#pragma once

#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Image.hpp>

int rand32();

void drawFrame(sf::IntRect frame, sf::Color color = sf::Color::Red);

sf::Image getFromWindow(sf::IntRect region);
sf::Image getScreenshot();

sf::Image rescaleImage(const sf::Image& image, int width, int height);
