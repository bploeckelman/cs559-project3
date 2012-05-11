#pragma once
/************************************************************************/
/* ImageManager
/* ------------
/* A manager for SFML images
/************************************************************************/
#include <SFML/Graphics/Image.hpp>

#include <string>
#include <set>
#include <map>

typedef std::map<std::string, sf::Image> StringImageMap;
typedef StringImageMap::iterator         StringImageMapIter;
typedef StringImageMap::const_iterator   StringImageMapConstIter;

typedef std::set<std::string>     StringSet;
typedef StringSet::iterator       StringSetIter;
typedef StringSet::const_iterator StringSetConstIter;

// Foo text...
class ImageManager
{
private:
	StringImageMap images;    // map of <"filename": image>
	StringSet resourceDirs;   // directories to look in for image resources

public:
	// Look up the specified image, loading if necessary.
	// Checks loaded images, if it is not yet loaded 
	// tries to load from the working directory, 
	// if that fails, tries to load from each directory 
	// specified in resourceDirs.
	sf::Image& getImage(const std::string& filename);

	// Delete the specified image, by value
	void deleteImage(const sf::Image& image);
	// Delete the specified image, by filename
	void deleteImage(const std::string& filename);

	// Add a new directory to look in for resources 
	void addResourceDir(const std::string& directory);
	// Delete a resource directory reference
	void delResourceDir(const std::string& directory);

	// Class is singleton, instance is accessed through ImageManager::get()
	static ImageManager& get() { static ImageManager m; return m; }

private:
	ImageManager() { }
	// Singletons don't implement these...
	ImageManager(const ImageManager& other);
	ImageManager& operator=(const ImageManager& other);
};

sf::Image& GetImage(const std::string& filename)
{
	return ImageManager::get().getImage(filename);
}
