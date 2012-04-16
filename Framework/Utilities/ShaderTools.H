// CS559 Utility Code: Shader Tools
// a collection of useful routines for making use of Shaders
//
// written November 2008 by Michael Gleicher
// with code borrowed from the tutorial by Cody Robson
//
// warning: this code assumes that you're using GLEE

#include <string>
#include <vector>

// because we don't want to do the gl include for you, the code 
// says "unsigned int" instead of GLuint

// read a shader - keep the lines seperate (otherwise, comments get messed up)
// reads it into a C++ data structure
// note: this searches the global list of paths "shaderPaths"
bool readShaderFile(const char* filename, std::vector<std::string>& program);

// these routines check for errors after compiling/linking
// not only do they do the test, but they print the error log
// if there is one

// see if the shader compiled correctly
// if it did not, return a true value, and print an error
// call this after compileShader
bool checkShaderError(unsigned int shader);

// see if the program linked correctly
// if it did not, return a true value, and print an error
// call this after linkProgram
bool checkProgramError(unsigned int program);


// load in and compile a vertex and fragment shader into a "program"
// this is taken from Cody's tutorial
// if there is an error, the the "error" variable points to a string
// for an error message (otherwise its set to zero)
// (notice that error is call by reference so we can return a value)
//
// note: this routine is sloppy about freeing memory in error cases
// (since we assume the program will abort).
// also: this routine doesn't give you handles to the shader objects,
// so you can't free them or re-use them
// in a big system, this may be problematic, but for writing 559 projects
// it should be OK
//
// remember, this searches the list of shader paths
unsigned int loadShader(const char* vertexFileName, const char* fragmentFileName,
				        char*& error);

// we'll look for shader programs on this list of paths
extern std::vector<std::string> shaderPaths;

#define _USE_MATH_DEFINES
// $Header: /p/course/cs559-gleicher/private/CVS/Utilities/ShaderTools.H,v 1.3 2008/11/11 04:27:57 gleicher Exp $
