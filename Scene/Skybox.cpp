/************************************************************************/
/* Skybox 
/* ------
/* A collection of 6 textures to apply to the interior of a cube
/* drawn around a camera to simulate a distant environment

TODO: update Skybox to use ImageManager to get its textures from SFML
instead of generating them itself and messing up the texture env mode

/************************************************************************/
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

// A way to print the Face enumeration values as strings
const string face_names[] = { 
	"front", "back", 
	"left", "right", 
	"top", "bottom" 
};
const string Skybox::directory("./Resources/images/skybox/");
const float faceSize = 5.f;
const float Skybox::vertices[] = {
	-faceSize, -faceSize, 0.f,		// vertex 0
	 faceSize, -faceSize, 0.f,		// vertex 3
	 faceSize,  faceSize, 0.f,		// vertex 6 
	-faceSize,  faceSize, 0.f		// vertex 9 
};
const float Skybox::texcoords[] = { 
	0.f, 0.f,	// texcoord 0
	1.f, 0.f,	// texcoord 2
	1.f, 1.f,	// texcoord 4 
	0.f, 1.f	// texcoord 6 
};


Skybox::Skybox()
	: textures() 
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
	// Get filenames from the image file directory
	vector<string> filenames;
	if( !getFilenames(filenames) )
		return false;

	// Match each face to an image filename 
	map<Face, string> faceImages;
	if( !getFaceImageMap(filenames, faceImages) )
		return false;

	// Generate texture objects for each image
	if( !buildTextureObjects(faceImages) )
		return false;

	cout << "Initialized skybox..." << endl;
	return true;
}

void Skybox::cleanup()
{
	for(auto t = textures.begin(); t != textures.end(); ++t)
	{
		if( (*t).second != 0 )
			glDeleteTextures(1, &(*t).second);
		else
			cerr << "Error cleaning up skybox texture: " << endl 
				 << "\tface: " << face_names[(*t).first] << endl;
	}
}

void Skybox::drawFace(const Face& face)//, const RenderState& state)
{
	glBlendFunc(GL_ONE, GL_ZERO);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[face]);

	glColor4f(1.f, 1.f, 1.f, 1.f);
	glBegin(GL_QUADS);
	glTexCoord2fv(&texcoords[0]);
		glVertex3fv(&vertices[0]);
	glTexCoord2fv(&texcoords[2]);
		glVertex3fv(&vertices[3]);
	glTexCoord2fv(&texcoords[4]);
		glVertex3fv(&vertices[6]);
	glTexCoord2fv(&texcoords[6]);
		glVertex3fv(&vertices[9]);
	glEnd();
}

void Skybox::render(const Camera& camera)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Undo camera translation, and 
	// roll the coordinate system 180 degrees
	glm::mat4 m;
	m = glm::translate(m, camera.position());
	m = glm::rotate(m, 180.f, glm::vec3(0,0,1));
	glMultMatrixf(glm::value_ptr(m));

	glDepthMask(GL_FALSE);

	// half the size of one cube face
	const float T = faceSize; 

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

	// Re-clear the depth buffer so we don't have to turn off depth testing
	glClear(GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_TRUE);

	glPopMatrix();
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

		// Generate a new opengl texture object id
		glGenTextures(1, &textures[face]);
		glBindTexture(GL_TEXTURE_2D, textures[face]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				
		// Load the image file
		stringstream ss;
		ss << directory << filename; 
		sf::Image image;
		if( !image.LoadFromFile(ss.str()) ) 
		{
			Log("Error opening : " + ss.str());
			cout << "Error opening : " << ss.str() << endl;
			return false;
		}

		// Generate the opengl TexImage
		const unsigned int width  = image.GetWidth();
		const unsigned int height = image.GetHeight(); 
		const GLvoid* pixels = image.GetPixelsPtr();
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGBA, 
			width, height, 
			0, 
			GL_RGBA, 
			GL_UNSIGNED_BYTE, 
			pixels );

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
