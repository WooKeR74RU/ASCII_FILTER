#include "Symbol.h"

#include "Resources.h"

Symbol::Symbol(int index, sf::Color color)
{
	set(index, color);
}
int Symbol::getCharCode() const
{
	return charCode_;
}
bool Symbol::isBold() const
{
	return isBold_;
}
void Symbol::set(int index, sf::Color color)
{
	index_ = index;
	color_ = color;
	if (index_ < RS().charCount / 2)
	{
		charCode_ = index_ + RS().ASCII_FIRST;
		isBold_ = false;
	}
	else
	{
		charCode_ = index_ + RS().ASCII_FIRST - RS().charCount / 2;
		isBold_ = true;
	}
	text_.setFont(RS().FONT);
	text_.setCharacterSize(RS().CHARACTERS_SIZE);
	text_.setString(std::string(1, charCode_));
	text_.setFillColor(color_);
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
