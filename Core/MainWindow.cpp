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
	, mouseView(true)
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

	setupOpenGLState();
	setupPerspective();


	SetCursorPosition(640, 480);
	ShowMouseCursor(false);

	scene.setup();

	mainLoop();
}

void MainWindow::cleanup()
{
	Log(title + " cleaning up...");
}

void MainWindow::mainLoop()
{
	Log("Entering main loop...");

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
	if( mouseView)
	{
		SetCursorPosition(640, 480);
		ShowMouseCursor(false);
	}
}

void MainWindow::render(const sf::Clock& clock)
{
	SetActive();

	scene.render(clock);

	Display();

}

void MainWindow::setupOpenGLState()
{
	SetFramerateLimit(60);

	glDisable(GL_LIGHTING);

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINTS);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	setupTempLights();
}

void MainWindow::setupTempLights()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	GLfloat gAmbient[] = { 0.2f, 0.2f, 0.2f, 1.f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gAmbient);

	GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.f };
	GLfloat diffuse[] = {.3f, .3f, .3f, 1.f }; //diffuse is gray
	GLfloat specular[] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat position1[] = { 0.f, 10.f, 0.f, 1.f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position1);

	GLfloat lightPosition2[] = {10.f, 10.f, 10.f, 1.f};
	GLfloat lightPosition3[] = {-10.f, 10.f, -10.f, 1.f};
	GLfloat yellowLight[] = {0.5f, 0.5f, 0.1f, 1.f};
	GLfloat greenLight[] = {0.0f, 1.0f, 0.0f, 1.f};

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, greenLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, yellowLight);
}

// TODO: this should be the Camera's job
void MainWindow::setupPerspective()
{
	const float aspect = static_cast<float>(GetWidth())
					   / static_cast<float>(GetHeight());
	const float fov    = 70.f;
	const float _near  = 1.f;
	const float _far   = 1000.f;

	glm::mat4 m(glm::perspective(fov, aspect, _near, _far));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(m));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MainWindow::handleEvents()
{
	sf::Event ev;
	while( GetEvent(ev) )
	{
		// Close the window
		if( ev.Type == sf::Event::Closed
		|| (ev.Type == sf::Event::KeyPressed && ev.Key.Code == sf::Key::Escape) )
		{
			Close();
		}
		if( ev.Type == sf::Event::KeyPressed && ev.Key.Code == sf::Key::RControl)
		{
			if(mouseView)
			{
				SetCursorPosition(640, 480);
				ShowMouseCursor(true);
				mouseView = false;
				scene.setMouseView(false);
			}
			else if(!mouseView)
			{
				SetCursorPosition(640, 480);
				ShowMouseCursor(false);
				mouseView = true;
				scene.setMouseView(true);
			}
		}

		// Toggle rendering states
/*
		if( ev.Type == sf::Event::KeyPressed )
		{
			if( ev.Key.Code == sf::Key::Num1 )
				scene.toggleRenderState(Scene::textures);
			if( ev.Key.Code == sf::Key::Num2 )
				scene.toggleRenderState(Scene::wireframe);
			if( ev.Key.Code == sf::Key::Num3 )
				scene.toggleRenderState(Scene::points);
		}
*/
	}
}
