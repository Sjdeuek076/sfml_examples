#include "FFT.h"
#include <iostream>


FFT::FFT(string const& _path,int const& _bufferSize)
{
	path = _path ;
	if(!buffer.loadFromFile(path)) cout<<"Unable to load buffer"<<endl ;

    
	sound.setBuffer(buffer) ;
	sound.setLoop(true);    
	sound.play();
    sound.setVolume(100); //0 to 100

	VA1.setPrimitiveType(LineStrip) ; // red line
	VA2.setPrimitiveType(Lines) ; // bar
	VA3.setPrimitiveType(LineStrip) ; //scrolling map

	sampleRate = buffer.getSampleRate()*buffer.getChannelCount() ;
	sampleCount = buffer.getSampleCount() ;
	if(_bufferSize < sampleCount) bufferSize = _bufferSize ;
	else bufferSize = sampleCount ;
	mark = 0 ;

	for(int i(0) ; i < bufferSize ; i++) window.push_back(0.54-0.46*cos(2*PI*i/(float)bufferSize)) ;

	sample.resize(bufferSize) ;
	VA1.resize(bufferSize);
}

void FFT::hammingWindow()
{
	mark = sound.getPlayingOffset().asSeconds()*sampleRate ;
	if(mark+bufferSize < sampleCount)
	{
		for(int i(mark) ; i < bufferSize+mark ; i++)
		{
			sample[i-mark] = Complex(buffer.getSamples()[i]*window[i-mark],0) ;
			VA1[i-mark] = Vertex(Vector2f(20,250)+Vector2f((i-mark)/(float)bufferSize*700,sample[i-mark].real()*0.005),Color(255,0,0,50)) ; // VertexArray <- Vertex(position, color)
		}
	}	
}

void FFT::fft(CArray &x)
{
	const int N = x.size();
	if(N <= 1) return;

	CArray even = x[slice(0,N/2,2)];
	CArray  odd = x[slice(1,N/2,2)];

	fft(even);
	fft(odd);

	for(int k = 0 ; k < N/2 ; k++)
	{
		Complex t = polar(1.0,-2 * PI * k / N) * odd[k];
		x[k] = even[k] + t;
		x[k+N/2] = even[k] - t;
	}
}

void FFT::update()
{
	hammingWindow() ;

	VA2.clear() ;
	VA3.clear() ;

	bin = CArray(sample.data(),bufferSize) ;
	fft(bin) ;
	float max = 100000000 ;
	
	lines(max) ;
	bars(max) ;
}

void FFT::bars(float const& max)
{
	VA2.setPrimitiveType(Lines) ;
	Vector2f position(0,800) ;
	for(float i(3) ; i < min(bufferSize/2.f,20000.f) ; i*=1.01)
	{
		Vector2f samplePosition(log(i)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)i])) ;
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Yellow)) ; //above line upper body
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color::Red)) ; //above line lower body 
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,0),Color(255,255,255,100))) ; //below line, lower body(note, below the line)
		VA2.append(Vertex(position+Vector2f(samplePosition.x*800,samplePosition.y/max*500/2.f),Color(255,255,255,0))) ; //below line, upper body(note, below the line)
	}
}

void FFT::lines(float const& max)
{
    /*
     * VA3--| (VertexArray(), primitive type = LineStrip)
     *    cascade--| (vector<sf::Vertex>, static vector, the same as VertexArray())
     *           Vertex--| (the smallest graphical entity that you can manipulate. 3attributes, position, color, texCoords)
     *                 Vertex.postition = samplePosition 
     */
    
	VA3.setPrimitiveType(LineStrip) ;
	Vector2f position(0,800) ; // VA3 starting point
	Vector2f samplePosition ;
	//float colorDecay = 1 ;
	
	for(float i(std::max((double)0,cascade.size()-3e5)) ; i < cascade.size() ; i++)
	{
		cascade[i].position -= Vector2f(-0.8,1) ;
		if(cascade[i].color.a != 0) cascade[i].color = Color(255,255,255,20) ;
	}
	samplePosition = Vector2f(log(3)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)3])) ;
    
    //output for testing
    //std::cout<<"samplePosition(x,y): "<<samplePosition.x<<", "<<samplePosition.y<<std::endl;
	
    cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent)) ;
	for(float i(3) ; i < bufferSize/2.f ; i*=1.02)
	{
		samplePosition = Vector2f(log(i)/log(min(bufferSize/2.f,20000.f)),abs(bin[(int)i])) ;
		cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color(255,255,255,20))) ;
	}
	cascade.push_back(Vertex(position+Vector2f(samplePosition.x*800,-samplePosition.y/max*500),Color::Transparent)) ;
    
    //output for testing, cascade vector<sf::Vertex>
    //std::cout<<"cascade: "<<cascade[1].position.x<<" , "<<cascade[1].position.y<<std::endl;
    
	VA3.clear() ;
	for(unsigned int i(std::max((double)0,cascade.size()-3e5)) ; i < cascade.size() ; i++) VA3.append(cascade[i]) ;
    //output for testing, VA3 VertexArray
    //std::cout<<"VA3 position: "<<VA3[1].position.x<<" , "<<VA3[1].position.y<<std::endl;
    
    
}

void FFT::draw(RenderWindow &window)
{
 	window.draw(VA1) ; //red
 	window.draw(VA3) ; //scrolling map
 	window.draw(VA2) ; //bar
}
