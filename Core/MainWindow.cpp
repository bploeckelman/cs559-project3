/************************************************************************/
/* MainWindow
/* ----------
/* The main SFML window for an application
/************************************************************************/
#include "MainWindow.h"
#include "Common.h"
#include "../Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Randomizer.hpp>

// Application window settings, video mode, window style
const std::string MainWindow::title("CS559 - Project 3 - Graphics Town");

const unsigned int width  = 1280;
const unsigned int height = 1024;
const unsigned int bpp    = 32;
const sf::VideoMode MainWindow::videoMode(width, height, bpp);

const unsigned int depth		= 24;
const unsigned int stencil		= 0;
const unsigned int antialiasing = 2;
const sf::WindowSettings MainWindow::windowSettings(depth, stencil, antialiasing);
const unsigned int MainWindow::windowStyle = sf::Style::Close | sf::Style::Resize;


MainWindow::MainWindow()
	: sf::Window(videoMode, title, windowStyle, windowSettings)
	, scene()
	, timer()
{
	init();
	sf::Randomizer::SetSeed(0);
}

MainWindow::~MainWindow()
{
	cleanup();
}

void MainWindow::init()
{
	Log(title + " initializing...");

	SetFramerateLimit(60);
	SetCursorPosition(MainWindow::videoMode.Width  / 2
					, MainWindow::videoMode.Height / 2);
	ShowMouseCursor(false);

	scene.init();

	mainLoop();
}

void MainWindow::cleanup()
{
	Log("\n" + title + " cleaning up...");
}

void MainWindow::mainLoop()
{
	Log("\n[Entering main loop...]");

	sf::Clock clock;
	timer.Reset();
	while( IsOpened() )
	{
		update(clock);
		render(clock);
		clock.Reset();
	}
}

void MainWindow::update(const sf::Clock& clock)
{
	handleEvents();

	scene.update(clock, GetInput());

	if( scene.isMouseLook() )
	{
		SetCursorPosition(MainWindow::videoMode.Width  / 2
						, MainWindow::videoMode.Height / 2);
		ShowMouseCursor(false);
	}
	else
	{
		ShowMouseCursor(true);
	}
}

void MainWindow::render(const sf::Clock& clock)
{
	SetActive();

	scene.render(clock);

	Display();
}

void MainWindow::handleEvents()
{
	sf::Event ev;
	while( GetEvent(ev) )
	{
		// Let the scene handle events 
		scene.handle(ev);

		// Close the window
		if( ev.Type == sf::Event::Closed
		|| (ev.Type == sf::Event::KeyPressed && ev.Key.Code == sf::Key::Escape) )
		{
			Close();
		}
	}
}
