#include "Glyph.h"

#include "Resources.h"

Glyph::Glyph(int index, sf::Color color)
{
	set(index, color);
}
int Glyph::getCharCode() const
{
	return charCode_;
}
bool Glyph::isBold() const
{
	return isBold_;
}
void Glyph::set(int index, sf::Color color)
{
	index_ = index;
	color_ = color;
	if (index_ < RS().CHAR_COUNT / 2)
	{
		charCode_ = index_ + RS().ASCII_FIRST;
		isBold_ = false;
	}
	else
	{
		charCode_ = index_ + RS().ASCII_FIRST - RS().CHAR_COUNT / 2;
		isBold_ = true;
	}
	text_.setFont(RS().FONT);
	text_.setCharacterSize(RS().CHAR_SIZE);
	text_.setString(std::string(1, charCode_));
	text_.setFillColor(color_);
	text_.move(0, -text_.getLocalBounds().top / 2);
}
void Glyph::setPosition(float x, float y)
{
	text_.setPosition(x, y - text_.getLocalBounds().top / 2);
}
void Glyph::draw() const
{
	RS().windows[std::this_thread::get_id()].draw(text_);
}
