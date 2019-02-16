#pragma once

#include <SFML/Graphics.hpp>
#include "Slider.h"

using namespace std ;
using namespace sf ;

class Pendulum
{
public:
	Pendulum() ;
	Pendulum(Vector2f const& _sliderPosition, Font *_font, RenderWindow *_window) ;

	float getPosition(float const& t) ;

	void draw() ;

private:
	float A ;
	float f ;
	float p ;
	float d ;

	Vector2f sliderPosition ;
	Font *font ;
	Slider Aslider ;
	Slider fslider ;
	Slider pslider ;
	Slider dslider ;

	RenderWindow *window ;
};

