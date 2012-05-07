/************************************************************************/
/* Skybox 
/* ------
/* A collection of 6 textures to apply to the interior of a cube
/* drawn around a camera to simulate a distant environment
/************************************************************************/
#include "../Lib/glee/GLee.h"

#include "Skybox.h"
#include "Camera.h"
#include "../Core/Common.h"
#include "../Utility/dirent.h"

#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

// HACK (could also get and include glext.h)
#define GL_CLAMP_TO_EDGE 0x812F

using namespace std;
using namespace glm;

// A way to print the Face enumeration values as strings
const string face_names[] = { 
	"front", "back", 
	"left", "right", 
	"top", "bottom" 
};

const string Skybox::dayDir("./Resources/images/skybox/day/");
const string Skybox::nightDir("./Resources/images/skybox/night/");
string Skybox::directory = Skybox::nightDir;

const float faceSize = 5.f;
const vec3 Skybox::vertices[] = 
{
	vec3(-faceSize, -faceSize, 0.f),
	vec3( faceSize, -faceSize, 0.f),
	vec3( faceSize,  faceSize, 0.f),
	vec3(-faceSize,  faceSize, 0.f)
};
const vec2 Skybox::texcoords[] = 
{ 
	vec2(0,0),
	vec2(1,0),
	vec2(1,1),
	vec2(0,1)
};
const unsigned char Skybox::indices[] = 
{
	0, 2, 3, 
	0, 1, 2
};


Skybox::Skybox()
	: textures(nullptr) 
	, dayTextures()
	, nightTextures()
	, timer()
	, toggleDayNight(true)
{
	if( !init() )
		throw exception("Error initializing skybox.");
}

Skybox::~Skybox()
{
	cleanup();
}

bool Skybox::init()
{
	// Get the day time textures -------------------
	directory = dayDir;
	textures  = &dayTextures;
	getTextures();

	// Get the night time textures -----------------
	directory = nightDir;
	textures  = &nightTextures;
	getTextures();

	// Set textures to day -------------------------
	textures = &dayTextures;
	toggleDayNight = true;

	// Start the day-night cycle timer -------------
	timer.Reset();

	cout << "Initialized skybox..." << endl;
	return true;
}

void Skybox::cleanup()
{
	textures = nullptr;

	for each(auto namedImage in dayTextures)
		delete namedImage.second;
	dayTextures.clear();

	for each(auto namedImage in nightTextures)
		delete namedImage.second;
	nightTextures.clear();
}

void Skybox::render(const Camera& camera)
{
	// Handle fading between night and day -----------------------
	// TODO: expose this to user 
	static const float timeLimit = 60.f; // in seconds

	static bool  toggle    = true;
	static float deltaTime = 0.f;

	if( toggle )
	{
		deltaTime += timer.GetElapsedTime();
		if( deltaTime > timeLimit )
		{
			toggle = !toggle;
			deltaTime = timeLimit;
		}
	}
	else 
	{
		deltaTime -= timer.GetElapsedTime();
		if( deltaTime < 0.f )
		{
			toggle = !toggle;
			deltaTime = 0.f;
		}
	}

	timer.Reset();

	// Day-night cycle delta [0,1]
	const float delta = deltaTime / timeLimit;

	// Setup skybox drawing --------------------------------------
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glMatrixMode(GL_MODELVIEW);

	// Draw day --------------------------------------------------
	setDay();
	glColor4f(1.f, 1.f, 1.f, delta);
	drawSkybox(camera);

	// Draw night ------------------------------------------------
	setNight();
	glColor4f(1.f, 1.f, 1.f, 1.f - delta);
	drawSkybox(camera);

	// Re-clear the depth buffer so we 
	// don't have to turn off depth testing
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Skybox::drawFace(const Face& face)
{
	assert(textures != nullptr);
	(*textures)[face]->Bind();

	glVertexPointer  (3, GL_FLOAT, 0, value_ptr( vertices[0]));
	glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(texcoords[0]));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Skybox::drawSkybox(const Camera& camera)
{
	// half the size of one cube face
	const float T = faceSize; 

	// Undo camera translation, and 
	// roll the coordinate system 180 degrees
	glm::mat4 m;
	m = glm::translate(m, camera.position());
	m = glm::rotate(m, 180.f, glm::vec3(0,0,1));

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(m));
		glPushMatrix();
			glTranslatef(0.f, 0.f, -T);
			drawFace(front);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.f, 0.f, T);
			glRotatef(180.f, 0.f, 1.f, 0.f);
			drawFace(back);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-T, 0.f, 0.f);
			glRotatef(90.f, 0.f, 1.f, 0.f);
			drawFace(left);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(T, 0.f, 0.f);
			glRotatef(270.f, 0.f, 1.f, 0.f);
			drawFace(right);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.f, T, 0.f);
			glRotatef(90.f, 1.f, 0.f, 0.f);
			drawFace(bottom);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.f, -T, 0.f);
			glRotatef(270.f, 1.f, 0.f, 0.f);
			drawFace(top);
		glPopMatrix();
	glPopMatrix();
}

bool Skybox::getTextures()
{
	vector<string> filenames;
	map<Face, string> faceImages;

	// Get filenames from the image file directory
	if( !getFilenames(filenames) )
		return false;

	// Match each face to an image filename 
	if( !getFaceImageMap(filenames, faceImages) )
		return false;

	// Generate texture objects for each image
	if( !buildTextureObjects(faceImages) )
		return false;

	return true;
}

bool Skybox::buildTextureObjects(map<Face, string>& faceImages)
{
	// For each texture image
	for(auto i = faceImages.begin(); i != faceImages.end(); ++i)
	{
		Face   face     = (*i).first;
		string filename = (*i).second;

		// Validate the image filename
		if( filename.empty() || filename.find(".png") == string::npos )
		{
			cout << "Error bad texture file : " << endl
				 << "\tface     = " << face_names[face] << endl 
			     << "\tfilename = " << filename << endl;
			return false;
		}

		// Load the image file
		stringstream ss;
		ss << directory << filename; 
		(*textures)[face] = new sf::Image(); 
		if( !(*textures)[face]->LoadFromFile(ss.str()) )
		{
			Log("Error opening : " + ss.str());
			cout << "Error opening : " << ss.str() << endl;
			return false;
		}

		// Bind texture and update wrap mode
		glEnable(GL_TEXTURE_2D);
		(*textures)[face]->Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return true;
}

bool Skybox::getFilenames(vector<string>& filenames)
{
	DIR* dir;
	dirent* ent;
	if( (dir = opendir(directory.c_str())) != nullptr )
	{
		while( ent = readdir(dir) )
		{
			string filename(ent->d_name);
			if( filename != "." && filename != ".." )
				filenames.push_back(filename);
		}
		closedir(dir);
	} else { 
		cout << "Error opening directory for listing." << endl;
		return false;
	}
	return true;
}

bool Skybox::getFaceImageMap(const vector<string>& filenames
						   , map<Face, string>& faceImages)
{
	// Get the image filename for each face
	for each(const auto& filename in filenames)
	{
		auto found = filename.find("front");
		if( found != string::npos )
		{
			faceImages[front] = filename;											
			continue;
		}
		found = filename.find("back");
		if( found != string::npos )
		{
			faceImages[back] = filename;											
			continue;
		}
		found = filename.find("left");
		if( found != string::npos )
		{
			faceImages[left] = filename;											
			continue;
		}
		found = filename.find("right");
		if( found != string::npos )
		{
			faceImages[right] = filename;											
			continue;
		}
		found = filename.find("top");
		if( found != string::npos )
		{
			faceImages[top] = filename;											
			continue;
		}
		found = filename.find("bottom");
		if( found != string::npos )
		{
			faceImages[bottom] = filename;											
			continue;
		}
	}
	
	// Validate the map
	vector<string> missing;

	if( faceImages[front].empty() )
		missing.push_back(face_names[front]);
	if( faceImages[back].empty() )
		missing.push_back(face_names[back]);
	if( faceImages[left].empty() )
		missing.push_back(face_names[left]);
	if( faceImages[right].empty() )
		missing.push_back(face_names[right]);
	if( faceImages[top].empty() )
		missing.push_back(face_names[top]);
	if( faceImages[bottom].empty() )
		missing.push_back(face_names[bottom]);

	if( !missing.empty() )
	{
		cerr << "Error matching images to faces, missing: " << endl;
		for(auto f = missing.begin(); f != missing.end(); ++f)
			cout << '\t' << (*f) << endl;

		return false;
	}
	return true;
}

const sf::Image& Skybox::getTexture(const Face& face, const bool day)
{
	return *(day ? dayTextures[face] : nightTextures[face]);
}
