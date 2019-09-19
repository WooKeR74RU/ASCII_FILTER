#include "Symbol.h"

#include "Resources.h"

Symbol::Symbol(int id, sf::Color color)
{
	set(id, color);
}
int Symbol::getCharCode() const
{
	return charCode_;
}
bool Symbol::isBold() const
{
	return isBold_;
}
sf::Vector2i Symbol::getSize() const
{
	const sf::Glyph& glyph = RS().FONT.getGlyph(charCode_, RS().CHARACTERS_SIZE, isBold_);
	const sf::IntRect& bounds = glyph.textureRect;
	return sf::Vector2i(bounds.width, bounds.height);
}
int Symbol::getBrightness() const
{
	const sf::Glyph& glyph = RS().FONT.getGlyph(charCode_, RS().CHARACTERS_SIZE, isBold_);
	const sf::IntRect& bounds = glyph.textureRect;
	int curBrightness = 0;
	for (int y = bounds.top; y < bounds.top + bounds.height; y++)
	{
		for (int x = bounds.left; x < bounds.left + bounds.width; x++)
			curBrightness += RS().symbolsImage.getPixel(x, y).a;
	}
	return curBrightness;
}
void Symbol::set(int id, sf::Color color)
{
	id_ = id;
	color_ = color;
	if (!RS().USE_BOLD || id_ < RS().charCount / 2)
	{
		charCode_ = id_ + RS().ASCII_FIRST;
		isBold_ = false;
	}
	else
	{
		charCode_ = id_ + RS().ASCII_FIRST - RS().charCount / 2;
		isBold_ = true;
	}

	text_.setFont(RS().FONT);
	text_.setCharacterSize(RS().CHARACTERS_SIZE);
	if (isBold_)
		text_.setStyle(sf::Text::Bold);
	text_.setFillColor(color_);
	text_.setString(std::string(1, charCode_));
	text_.move(0, -text_.getLocalBounds().top / 2);
}
void Symbol::setPosition(float x, float y)
{
	text_.setPosition(x, y - text_.getLocalBounds().top / 2);
}
void Symbol::draw() const
{
	RS().windows[std::this_thread::get_id()].draw(text_);
}
