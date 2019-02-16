#include <SFML/Graphics.hpp>
#include "src/Harmonograph.h"
#include "src/Slider.h"

using namespace std ;
using namespace sf ;

int main()
{
	ContextSettings aa ; aa.antialiasingLevel = 8 ;
	RenderWindow window(VideoMode(800, 800, 32), "Window",Style::Default, aa) ;

	Font consolas ; consolas.loadFromFile("consola.ttf") ;    
	Harmonograph graph(Vector2f(400,500), &consolas, &window) ;
	
	Vector2f mousePos ;
	Event event ;
    
	while(window.isOpen())
	{
		while(window.pollEvent(event)) {}

		mousePos = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y) ;

		graph.update() ;
		
		window.clear(Color::White) ;
		graph.draw() ;
		window.display() ;
	}
}
