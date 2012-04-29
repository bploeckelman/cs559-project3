/************************************************************************/
/* ImageManager
/* ------------
/* A manager for SFML images
/************************************************************************/
#include "ImageManager.h"
#include "../Utility/Logger.h"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Color.hpp>

#include <sstream>
#include <stdio.h>
#include <string>
#include <set>
#include <map>
#include <iostream>

using namespace sf;
using namespace std;


sf::Image& ImageManager::getImage( const std::string& filename )
{	
	//error stream to toss
	stringstream toss;
	streambuf* old = cerr.rdbuf(toss.rdbuf());

	// See if it is already loaded
	StringImageMapIter it = images.find(filename);

	if( it != images.end() )
	{
		//return cerr to original stream
		cerr.rdbuf(old);
		return (*it).second;
	}
	else // Not loaded yet
	{
		stringstream ss; // for Log output

		// Try working directory
		Image image;
		if( image.LoadFromFile(filename) )
		{

			ss << "Loaded image: " << filename;
			Log(ss);

			images[filename] = image;
			//return cerr to original stream
			cerr.rdbuf(old);
			return images[filename];
		}
		else // Try other resource directories
		{
			for each(const auto& dir in resourceDirs)
			{
				if( image.LoadFromFile(dir + filename) )
				{

					ss << "Loaded image: " << (dir + filename);
					Log(ss);

					images[filename] = image;
					//return cerr to original stream
					cerr.rdbuf(old);
					return images[filename];
				}
			}
			// Couldn't find it anywhere
			ss  << "Warning: unable to find image " << filename
				<< " returning blank image.";
			Log(ss);

			image.Create(32, 32, Color(255, 0, 255, 255));
			images[filename] = image;
			//return cerr to original stream
			cerr.rdbuf(old);
			return images[filename];
		}
	}
}

void ImageManager::deleteImage( const Image& image )
{
	StringImageMapConstIter it  = images.begin();
	StringImageMapConstIter end = images.end();
	for(; it != end; ++it)
	{
		const string& filename((*it).first);
		const Image&  itImage((*it).second);
		if( &image == &itImage )
		{
			images.erase(filename);
			return;
		}
	}
}

void ImageManager::deleteImage( const string& filename )
{
	images.erase(filename);
}

void ImageManager::addResourceDir( const string& directory )
{
	resourceDirs.insert(directory);
}

void ImageManager::delResourceDir( const string& directory )
{
	resourceDirs.erase(directory);
}
