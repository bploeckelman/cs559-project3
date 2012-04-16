// Very simple shader for CS559 Example Code
// doesn't do much
//

varying vec3 normal;

void main()
{
	// inline comments should work
	gl_Position = ftransform();	// like this one
	normal = gl_Normal;
}
