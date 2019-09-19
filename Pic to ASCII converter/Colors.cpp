#include "Colors.h"

#include <algorithm>
#include <map>

RGB::RGB(int r, int g, int b) : r(r), g(g), b(b)
{ }
RGB::RGB(const HSV& hsv)
{
	float sFactor = (float)hsv.s / 255;
	int hue = hsv.h / 60;
	float f = (float)hsv.h / 60 - hue;
	float p = hsv.v * (1 - sFactor);
	float q = hsv.v * (1 - (f * sFactor));
	float t = hsv.v * (1 - (1 - f) * sFactor);
	if (hue == 0)
		r = hsv.v, g = t, b = p;
	if (hue == 1)
		r = q, g = hsv.v, b = p;
	if (hue == 2)
		r = p, g = hsv.v, b = t;
	if (hue == 3)
		r = p, g = q, b = hsv.v;
	if (hue == 4)
		r = t, g = p, b = hsv.v;
	if (hue == 5)
		r = hsv.v, g = p, b = q;
}
RGB::RGB(const sf::Color& color) : RGB(color.r, color.g, color.b)
{ }
sf::Color RGB::getColor() const
{
	return sf::Color(r, g, b);
}

HSV::HSV(int h, int s, int v) : h(h), s(s), v(v)
{ }
HSV::HSV(const RGB& rgb)
{
	int minVal = std::min(rgb.r, std::min(rgb.g, rgb.b));
	int maxVal = std::max(rgb.r, std::max(rgb.g, rgb.b));
	v = maxVal;
	s = maxVal == 0 ? 0 : (maxVal - minVal) * 255 / maxVal;
	if (s == 0)
	{
		h = 0;
		return;
	}
	float rDist = (float)(maxVal - rgb.r) / (maxVal - minVal); // distance from color to red
	float gDist = (float)(maxVal - rgb.g) / (maxVal - minVal); // distance from color to green
	float bDist = (float)(maxVal - rgb.b) / (maxVal - minVal); // distance from color to blue
	float hue;
	if (rgb.r == maxVal) // between yellow and magenta
		hue = bDist - gDist;
	if (rgb.g == maxVal) // between cyan and yellow
		hue = 2 + rDist - bDist;
	if (rgb.b == maxVal) // between magenta and cyan
		hue = 4 + gDist - rDist;
	if (hue < 0)
		hue += 6;
	h = hue * 60;
	if (h > 359)
		h = 0;
}
HSV::HSV(const sf::Color& color) : HSV(RGB(color))
{ }
sf::Color HSV::getColor() const
{
	return RGB(*this).getColor();
}

sf::Color setSaturation(const sf::Color& color, int value)
{
	HSV hsv(color);
	hsv.s = value;
	return hsv.getColor();
}
sf::Color setBrightness(const sf::Color& color, int value)
{
	HSV hsv(color);
	hsv.v = value;
	return hsv.getColor();
}

const sf::Color& getColor(const std::string& colorName)
{
	static std::map<std::string, sf::Color> colors;
	static bool once = false;
	if (!once)
	{
		colors["Aqua"] = sf::Color(000, 255, 255);
		colors["Black"] = sf::Color::Black;
		colors["Blue"] = sf::Color::Blue;
		colors["Fuchsia"] = sf::Color(255, 000, 255);
		colors["Gray"] = sf::Color(128, 128, 128);
		colors["Green"] = sf::Color::Green;
		colors["Lime"] = sf::Color(000, 255, 000);
		colors["Maroon"] = sf::Color(128, 000, 000);
		colors["Navy"] = sf::Color(000, 000, 128);
		colors["Olive"] = sf::Color(128, 128, 000);
		colors["Purple"] = sf::Color(128, 000, 128);
		colors["Red"] = sf::Color::Red;
		colors["Silver"] = sf::Color(192, 192, 192);
		colors["Teal"] = sf::Color(000, 128, 128);
		colors["White"] = sf::Color::White;
		colors["Yellow"] = sf::Color::Yellow;
		once = true;
	}
	return colors[colorName];
}
