// source from: https://mightynotes.wordpress.com/2019/07/09/real-time-plotting-in-c-using-root/
// c++ root-realtime-plot.cpp -o root-realtime-plot -O3 `root-config --cflags --glibs` -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lfftw3f

// Using SFML as out audio loader and player
#include <SFML/Audio.hpp>

// The standard headers we need
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <cassert>

using namespace std::chrono_literals;

// xtensor and xtensor-fftw
#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp>
#include <xtensor-fftw/basic.hpp>

// ROOT headers
#include "TStyle.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TAxis.h"
#include "TROOT.h"

constexpr int window_size = 2048;

const xt::xarray<float> elc_data = {
	{76.55, 65.62, 55.12, 45.53, 37.63, 30.86, 25.02, 20.51, 16.65, 13.12, 10.09, 7.54, 5.11, 3.06, 1.48, 0.3, -0.3, -0.01, 1.03, -1.19, -4.11, -7.05, -9.03, -8.49, -4.48, 3.28, 9.83, 10.48, 8.38, 14.1, 79.65},
	{83.75, 75.76, 68.21, 61.14, 54.96, 49.01, 43.24, 38.13, 33.48, 28.77, 24.84, 21.33, 18.05, 15.14, 12.98, 11.18, 9.99, 10, 11.26, 10.43, 7.27, 4.45, 3.04, 3.8, 7.46, 14.35, 20.98, 23.43, 22.33, 25.17, 81.47},
	{89.58, 82.65, 75.98, 69.62, 64.02, 58.55, 53.19, 48.38, 43.94, 39.37, 35.51, 31.99, 28.69, 25.67, 23.43, 21.48, 20.1, 20.01, 21.46, 21.4, 18.15, 15.38, 14.26, 15.14, 18.63, 25.02, 31.52, 34.43, 33.04, 34.67, 84.18},
	{99.85, 93.94, 88.17, 82.63, 77.78, 73.08, 68.48, 64.37, 60.59, 56.7, 53.41, 50.4, 47.58, 44.98, 43.05, 41.34, 40.06, 40.01, 41.82, 42.51, 39.23, 36.51, 35.61, 36.65, 40.01, 45.83, 51.8, 54.28, 51.49, 51.96, 92.77},
	{109.51, 104.23, 99.08, 94.18, 89.96, 85.94, 82.05, 78.65, 75.56, 72.47, 69.86, 67.53, 65.39, 63.45, 62.05, 60.81, 59.89, 60.01, 62.15, 63.19, 59.96, 57.26, 56.42, 57.57, 60.89, 66.36, 71.66, 73.16, 68.63, 68.43, 104.92},
	{118.99, 114.23, 109.65, 105.34, 101.72, 98.36, 95.17, 92.48, 90.09, 87.82, 85.92, 84.31, 82.89, 81.68, 80.86, 80.17, 79.67, 80.01, 82.48, 83.74, 80.59, 77.88, 77.07, 78.31, 81.62, 86.81, 91.41, 91.74, 85.41, 84.67, 118.95},
	{128.41, 124.15, 120.11, 116.38, 113.35, 110.65, 108.16, 106.17, 104.48, 103.03, 101.85, 100.97, 100.3, 99.83, 99.62, 99.5, 99.44, 100.01, 102.81, 104.25, 101.18, 98.48, 97.67, 99, 102.3, 107.23, 111.11, 110.23, 102.07, 100.83, 133.73}
};

const xt::xarray<float> elc_phons = {0, 10, 20, 40, 60, 80, 100};
const xt::xarray<float> elc_freqs = {20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};

inline float linear_interp(float x1, float y1, float x2, float y2, float v)
{
	return y1 + (v-x1)*(y2-y1)/(x2-x1);
}

inline float dbspl_to_phons(float frequency, float dbspl)
{
	if(frequency <= 20 || frequency >= 20000)
		return 0;
	
	int elc_freq_index = 0;
	for(int i=0;i<elc_freqs.size();i++) {
		if(elc_freqs[i] > frequency) {
			elc_freq_index = i;
			break;
		}
	}
	assert(elc_freq_index >= 1 && elc_freq_index < elc_freqs.size());

	const float freq0 = elc_freqs[elc_freq_index-1];
	const float freq1 = elc_freqs[elc_freq_index];

	const float hearing_lower_thr = linear_interp(freq0, xt::view(elc_data, 0, elc_freq_index-1)[0]
						, freq1, xt::view(elc_data, 0, elc_freq_index)[0]
						, frequency);
	if(dbspl < hearing_lower_thr)
		return 0;
	
	int elc_phones_index = 0;
	for(int i=0;i<elc_phons.size();i++) {
		float v = linear_interp(freq0, xt::view(elc_data, i, elc_freq_index-1)[0]
					, freq1, xt::view(elc_data, i, elc_freq_index)[0]
					, frequency);
		if(v > dbspl) {
			elc_phones_index = i;
			break;
		}
	}
	assert(elc_phones_index >= 1);

	const float presure0 = linear_interp(freq0, xt::view(elc_data, elc_phones_index-1, elc_freq_index-1)[0]
					, freq1, xt::view(elc_data, elc_phones_index-1, elc_freq_index)[0]
					, frequency);
	const float presure1 = linear_interp(freq0, xt::view(elc_data, elc_phones_index, elc_freq_index-1)[0]
					, freq1, xt::view(elc_data, elc_phones_index, elc_freq_index)[0]
					, frequency);
	const float phon = linear_interp(presure0, elc_phons[elc_phones_index-1]
					, presure1, elc_phons[elc_phones_index]
					, dbspl);
	assert(phon >= 0);
	return phon;
}

int main(int argc, char* argv[])
{
	// Read the audio to play
	sf::SoundBuffer buffer;
	if(buffer.loadFromFile("ghostrule.wav") == false)
		throw std::runtime_error("Cannot open audio");
	sf::Sound sound(buffer);

	// Initalize ROOT
	TApplication rootapp("spectrum", &argc, argv);

	// Reate the canvas we are drawing on
	auto c1 = std::make_unique<TCanvas>("c1", "Spectrum analyzer in ROOT");
	c1->SetWindowSize(1550, 700);
	//gStyle->SetCanvasPreferGL(true);

	// Create the spectrogram
	auto f1 = std::make_unique<TGraph>(window_size/2);
	f1->SetTitle("spectrum");
	f1->GetXaxis()->SetTitle("Frequency (Hz)");
	f1->GetYaxis()->SetTitle("Phons");
	f1->SetMinimum(0);
	f1->SetMaximum(100);

	// Creathe the waveform plot
	auto f2 = std::make_unique<TGraph>(window_size);
	f2->SetTitle("Waveform");
	f2->GetXaxis()->SetTitle("Sample");
	f2->GetYaxis()->SetTitle("Amplitude");
	f2->SetMinimum(-1);
	f2->SetMaximum(1);

	// divide the canvas into two vertical sub-canvas
	c1->Divide(1, 2);

	// "Register" the plots for each canvas slot
	c1->cd(1); // Set current canvas to canvas 1 (yes, 1 based indexing)
	c1->Pad()->SetLeftMargin(0.08);
	c1->Pad()->SetRightMargin(0.01);
	f1->Draw();
	c1->cd(2); // Set current canvas to canvas 2
	c1->Pad()->SetLeftMargin(0.08);
	c1->Pad()->SetRightMargin(0.01);
	f2->Draw();

	// Let's get some audio related parameters first
	size_t samples_per_channel = buffer.getSampleCount()/buffer.getChannelCount();
	size_t channels = buffer.getChannelCount();
	size_t sample_rate = buffer.getSampleRate();
	std::vector<size_t> shape = {samples_per_channel, channels};
	// Copy the entire audio buffer and turn it into floating point
	xt::xarray<float> sound_data = xt::adapt(buffer.getSamples(), shape)/(float)0x7fff;
	// Merge and average the two channels
	sound_data = xt::sum(sound_data, 1)/channels;

	// Reference value for converting magnitude into dB SPL
	float ref = 2e-5f;

	//Start audio playback
	sound.play();
	auto t1 = std::chrono::high_resolution_clock::now();

	while(sound.getStatus() == sf::SoundSource::Playing) {
		// ROOT atomatically deactivates the canvas if the user closes the canvas
		// Check if the canvas still exists
		if(gROOT->GetListOfCanvases()->FindObject("c1") == nullptr)
			break;

		float time = sound.getPlayingOffset().asSeconds();
		size_t start = time*sample_rate+0.016*sample_rate/2;
		if(start+window_size >= sound_data.size())
			break;
		// Get the current playing samples
		xt::xarray<float> v = xt::view(sound_data, xt::range(start, start+window_size))*0.2;
		// Perform FFT on the samples
		xt::xarray<float> fft = xt::abs(xt::fftw::rfft(v))/window_size;
		auto magnitude = fft*2; //*2 because we are only using half of the FFT later on
		auto db_spl = 20*xt::log10(magnitude/ref);

		xt::xarray<float> phons = xt::zeros<float>({window_size/2});
		for(int i=0;i<window_size/2;i++)
			phons[i] = dbspl_to_phons(i*(float)buffer.getSampleRate()/window_size, db_spl[i]);



		// Update the spectrogram
		for(size_t i=0;i<window_size/2;i++) {
			float f = i*(float)buffer.getSampleRate()/window_size;
			if(f < 20) {
				f1->SetPoint(i, -60, 0);
				continue;
			}
				
			float n = - (12*(2*log2(2) - log(f/55)))/log(2);
			f1->SetPoint(i, n, phons[i]);
		}

		// Update the waveform
		for(size_t i=0;i<window_size;i++)
			f2->SetPoint(i, i, v[i]);
		
		//Notify ROOT that the plots have been modified and needs update
		c1->cd(1);
		c1->Update();
		c1->Pad()->Draw();
		c1->cd(2);
		c1->Update();
		c1->Pad()->Draw();
		
		gSystem->ProcessEvents();

		// Calculate how long it takes to run fft and update and limit the update rate to 60Hz
		auto t2 = std::chrono::high_resolution_clock::now();
		auto diff = t2 - t1;
		if(diff < 16ms)
			std::this_thread::sleep_for(16ms - diff);
		t1 = t2;
	}
}
