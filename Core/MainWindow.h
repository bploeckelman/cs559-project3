#pragma once
/************************************************************************/
/* MainWindow
/* ----------
/* The main SFML window for an application
/************************************************************************/
#include "../Scene/Scene.h"

#include <SFML/Window.hpp>

#include <string>


class MainWindow : public sf::RenderWindow
{
public:
	static const std::string		title;
	static const sf::VideoMode		videoMode;
	static const unsigned int		windowStyle;
	static const sf::WindowSettings	windowSettings;

	MainWindow();
	~MainWindow();

private:
	Scene scene;
	sf::Clock timer;

	void init();
	void cleanup();
	void mainLoop();
	void update(const sf::Clock& clock);
	void render(const sf::Clock& clock);

	void handleEvents();
};
