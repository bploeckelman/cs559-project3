#pragma once
/************************************************************************
     File:        Texture.H

				  The simple texture manager loads in a texture from a file and binds
				  it. All you ever need to do in your program is call fetchTexture.
				  For example:
						fetchTexture("texture.png");

				  The texture manager searchers a list of directories to find the 
				  image file. There is a vector of C strings called "texturePaths"
				  that you should set up before using this.

				  The texture manager uses various tricks to make sure that it only
				  loads each texture once. However, it does have to do a bunch of
				  string compares to check. If you really want performance, you should
				  keep the Texture object returned by fetchTexture and bind to it the next
				  time. For example

						(declared as a member variable and initialized to 0)
						Texture* myTexture;

						(then in the draw method)
						if (!myTexture) fetchTexture(myTexture); else myTexture->bind();

				   Note that fetchTexture returns NULL if it can't load the texture
				   file. While this means your program won't crash if you have the texture
				   wrong, it means you will try to load the texture each time.

				   Some things that can go wrong:
				   		- you can only load textures when you have a gl context
						- you cannot bind/unbind textures within a begin/end
						- don't forget to enable 2D texture mapping before binding
						- don't forget to "unbind" when you're done drawing
						  this usually means doing:
							glBindTexture(GL_TEXTURE_2D,0);


     Author:     
                  Michael L. Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Simple texture manager
	              (c) 2000 Michael L. Gleicher
                  This code may be modified and re-used providing that proper credit
                  is given to the author. 

				  Management of textures
                  probably more complicated than it needs to be, but it makes some
                  other things simple

                  The trick is to only have to load each texture once, no matter how
                  many things use it

				  New for 2007: fetch returns a pointer to the texture object so you
				  don't need to search for it in the list next time.
				  Also, the list is no longer hard coded.

				  2008: get rid of linked lists

				  2009: List of things that can go wrong
						move to utilities

	$Header: /p/course/cs559-gleicher/private/CVS/Utilities/Texture.H,v 1.3 2009/11/11 13:38:33 gleicher Exp $
*************************************************************************/
/* **************************************
   Note : the fltk png lib conflicts with SFML,
   since the framework texture stuff is not being
   used, I'm commenting everything out for now that
   is related in the Texture.cpp file.. 
/**************************************** */
#include <Fl/Fl.h>
#include <Fl/Gl.h>
#include <Gl/Glu.h>
#include <stdio.h>
#include <vector>

class Texture {
	public:
		// do the bind
		void bind();

		// What we give to LibTarga
		std::string name;                   //
		std::string fname;					// the file name

		// What libtarga gives back...
		void* bits;
		int   width;
		int   height;

		GLuint texName;				// GL texture name
};

// All the textures that have been loaded
// note that there's nothing to delete/free them when the
// program is done
extern std::vector<Texture*> theTextures;

// Get a texture- will hunt it down and load it if its not already loaded
// this even does the bind texture for you
// This searches in the "texturePaths" (see below), and can load in
// a variety of image formats (.tga, .png, .jpg, .gif)
// This returns NULL (0) if it has a problem and can't give you a valid
// texture. If you store the pointer to a texture, you don't need to search
// for it 
Texture* fetchTexture(char* name, bool wrapS=false, bool wrapT=false);

// keep the list of directories to search
// add a new directory to the list
extern std::vector<const char*> texturePaths;