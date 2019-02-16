#include <math.h>
#include "Pendulum.h"



Pendulum::Pendulum()
{
	A=10 ;
	f=1 ;
	p=0 ;
	d=1 ;
}

Pendulum::Pendulum(Vector2f const& _sliderPosition, Font *_font, RenderWindow *_window)
{
	font = _font ;

	sliderPosition = _sliderPosition ;

	Aslider = Slider(Vector2f(100,2), Vector2f(2,4), sliderPosition+Vector2f(0,0), 0, 400, "A : ", *font) ;
	fslider = Slider(Vector2f(100,2), Vector2f(2,4), sliderPosition+Vector2f(0,30), 0, 40, "f : ", *font) ;
	pslider = Slider(Vector2f(100,2), Vector2f(2,4), sliderPosition+Vector2f(0,60), 0, 360, "p : ", *font) ;
	dslider = Slider(Vector2f(100,2), Vector2f(2,4), sliderPosition+Vector2f(0,90), 0, 0.5, "d : ", *font) ;

	A=Aslider.getSliderValue() ;
	f=fslider.getSliderValue() ;
	p=pslider.getSliderValue() ;
	d=dslider.getSliderValue() ;

	window = _window ;
}

float Pendulum::getPosition(float const& t)
{
	return A*sin(t*f + p*3.1415/180.f)*exp(-d*t) ;
}

void Pendulum::draw()
{
	Vector2f mousePos(Mouse::getPosition(*window).x, Mouse::getPosition(*window).y) ;

	Aslider.isSliderClicked(window) ;
	fslider.isSliderClicked(window) ;
	pslider.isSliderClicked(window) ;
	dslider.isSliderClicked(window) ;

	A = Aslider.getSliderValue() ;
	f = fslider.getSliderValue() ;
	p = pslider.getSliderValue() ;
	d = dslider.getSliderValue() ;

	Aslider.draw(*window) ;
	fslider.draw(*window) ;
	pslider.draw(*window) ;
	dslider.draw(*window) ;
}
