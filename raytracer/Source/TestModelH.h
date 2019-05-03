#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <string>
#include "RotationHandler.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;
using glm::vec2;

// Used to describe a triangular or spherical surface:
class Object
{
public:
	glm::vec4 v0;
	glm::vec4 v1;
	glm::vec4 v2;
	float radius;
	glm::vec4 normal;
	glm::vec3 color;
	string type;
	int material;

	Object( glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, float radius, glm::vec3 color, string type, int material )
		: v0(v0), v1(v1), v2(v2), radius(radius), color(color), type(type), material(material)
	{
		if (type == "triangle") {
			ComputeNormal();
		} else if (type == "sphere") {
			ComputeSphereNormal();
		}
	}

	void ReverseNormal() {
		normal *= -1;
		normal.w *= -1;
	}

	void ComputeSphereNormal() {

	}

	void ComputeNormal() {
	  glm::vec3 e1 = glm::vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
	  glm::vec3 e2 = glm::vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
	  glm::vec3 normal3 = glm::normalize( glm::cross( e2, e1 ) );
	  normal.x = normal3.x;
	  normal.y = normal3.y;
	  normal.z = normal3.z;
	  normal.w = 1.0;
	}
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<Object>& objects ) {
	using glm::vec3;
	using glm::vec4;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 gray(   0.75f, 0.75f, 0.75f );
	vec3 white(  0.95f, 0.95f, 0.95f );
	vec3 pink(     1.f, 0.75f, 0.79f );
	vec3 violet( 0.57f, 0.44f, 0.85f );

	objects.clear();
	objects.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec4 A(L,0,0,1);
	vec4 B(0,0,0,1);
	vec4 C(L,0,L,1);
	vec4 D(0,0,L,1);

	vec4 E(L,L,0,1);
	vec4 F(0,L,0,1);
	vec4 G(L,L,L,1);
	vec4 H(0,L,L,1);

	// Floor:
	objects.push_back( Object( C, B, A, 0.f, gray, "triangle", 0 ) );
	objects.push_back( Object( C, D, B, 0.f, gray, "triangle", 0 ) );

	// Left wall
	objects.push_back( Object( A, E, C, 0.f, red, "triangle", 0 ) );
	objects.push_back( Object( C, E, G, 0.f, red, "triangle", 0 ) );

	// Right wall
	objects.push_back( Object( F, B, D, 0.f, green, "triangle", 0 ) );
	objects.push_back( Object( H, F, D, 0.f, green, "triangle", 0 ) );

	// Ceiling
	objects.push_back( Object( E, F, G, 0.f, gray, "triangle", 0 ) );
	objects.push_back( Object( F, H, G, 0.f, gray, "triangle", 0 ) );

	// Back wall
	objects.push_back( Object( G, D, C, 0.f, gray, "triangle", 0 ) );
	objects.push_back( Object( G, H, D, 0.f, gray, "triangle", 0 ) );

	// ---------------------------------------------------------------------------
	// Short block
	/*
	A = vec4(290,0,114,1);
	B = vec4(130,0, 65,1);
	C = vec4(240,0,272,1);
	D = vec4( 82,0,225,1);

	E = vec4(290,165,114,1);
	F = vec4(130,165, 65,1);
	G = vec4(240,165,272,1);
	H = vec4( 82,165,225,1);

	int shortStart = objects.size();

	// Front
	objects.push_back( Object(E,B,A, 0.f, pink, "triangle", 0) );
	objects.push_back( Object(E,F,B, 0.f, pink, "triangle", 0) );

	// RIGHT
	objects.push_back( Object(F,D,B, 0.f, pink, "triangle", 0) );
	objects.push_back( Object(F,H,D, 0.f, pink, "triangle", 0) );

	// BACK
	objects.push_back( Object(H,C,D, 0.f, pink, "triangle", 0) );
	objects.push_back( Object(H,G,C, 0.f, pink, "triangle", 0) );

	// LEFT
	objects.push_back( Object(G,E,C, 0.f, pink, "triangle", 0) );
	objects.push_back( Object(E,A,C, 0.f, pink, "triangle", 0) );

	// TOP
	objects.push_back( Object(G,F,E, 0.f, pink, "triangle", 0) );
	objects.push_back( Object(G,H,F, 0.f, pink, "triangle", 0) );

	int shortEnd = objects.size();
	
	// ---------------------------------------------------------------------------
	// Tall block

	A = vec4(423,0,247,1);
	B = vec4(265,0,296,1);
	C = vec4(472,0,406,1);
	D = vec4(314,0,456,1);

	E = vec4(423,330,247,1);
	F = vec4(265,330,296,1);
	G = vec4(472,330,406,1);
	H = vec4(314,330,456,1);

	int tallStart = objects.size();

	// Front
	objects.push_back( Object(E,B,A,0.f, gray, "triangle", 0) );
	objects.push_back( Object(E,F,B,0.f, gray, "triangle", 0) );

	// RIGHT
	objects.push_back( Object(F,D,B,0.f, gray, "triangle", 0) );
	objects.push_back( Object(F,H,D,0.f, gray, "triangle", 0) );

	// BACK
	objects.push_back( Object(H,C,D,0.f, gray, "triangle", 0) );
	objects.push_back( Object(H,G,C,0.f, gray, "triangle", 0) );

	// LEFT
	objects.push_back( Object(G,E,C,0.f, gray, "triangle", 0) );
	objects.push_back( Object(E,A,C,0.f, gray, "triangle", 0) );

	// TOP
	objects.push_back( Object(G,F,E,0.f, gray, "triangle", 0) );
	objects.push_back( Object(G,H,F,0.f, gray, "triangle", 0) );

	int tallEnd = objects.size();

	// ----------------------------------------------
	// Scale to the volume [-1,1]^3
	*/

	for( size_t i=0; i<objects.size(); ++i )
	{
		objects[i].v0 *= 2/L;
		objects[i].v1 *= 2/L;
		objects[i].v2 *= 2/L;

		objects[i].v0 -= vec4(1,1,1,1);
		objects[i].v1 -= vec4(1,1,1,1);
		objects[i].v2 -= vec4(1,1,1,1);

		objects[i].v0.x *= -1;
		objects[i].v1.x *= -1;
		objects[i].v2.x *= -1;

		objects[i].v0.y *= -1;
		objects[i].v1.y *= -1;
		objects[i].v2.y *= -1;

		objects[i].v0.w = 1.0;
		objects[i].v1.w = 1.0;
		objects[i].v2.w = 1.0;

		objects[i].ComputeNormal();
	}
	/*
	mat4 R;
	RotationMatrixByAngle(0.0f, 5.0f, 0.0f, R);

	for (int i = tallStart; i < tallEnd; i++) {
		objects[i].v0 = R * objects[i].v0;
		objects[i].v1 = R * objects[i].v1;
		objects[i].v2 = R * objects[i].v2;

		objects[i].v0 -= vec4(0.5,0,0,0);
		objects[i].v1 -= vec4(0.5,0,0,0);
		objects[i].v2 -= vec4(0.5,0,0,0);
	}

	RotationMatrixByAngle(0.0f, -5.0f, 0.0f, R);

	for (int i = shortStart; i < shortEnd; i++) {
		objects[i].v0 = R * objects[i].v0;
		objects[i].v1 = R * objects[i].v1;
		objects[i].v2 = R * objects[i].v2;

		objects[i].v0 -= vec4(0.1,0,0.5,0);
		objects[i].v1 -= vec4(0.1,0,0.5,0);
		objects[i].v2 -= vec4(0.1,0,0.5,0);
	}
	*/
}

void LoadBunnyModel( std::vector<Object>& model ) {

  vector<vec4> temp_vertices;
	int prevIndex = model.size();

  std::ifstream infile("bunny.obj");
  std::string line;

  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    char l;

    if (!(iss >> l)) { break; } // error
    if (l == 'v') {
      float a, b, c;
      if (!(iss >> a >> b >> c)) { break; }
      temp_vertices.push_back(vec4(a, b, c, 1.0f));
    } else if (l == 'f') {
      float a, b, c;
      if (!(iss >> a >> b >> c)) { break; }

      model.push_back(Object(temp_vertices[a-1], temp_vertices[b-1], temp_vertices[c-1], 0.f, vec3(1, 1, 1), "triangle", 2));

    }
  }

  // ----------------------------------------------
  // Scale to the volume [-1,1]^3
	mat4 R;
	RotationMatrixByAngle(0.0f, -35.0f, 0.0f, R);

  for( size_t i=prevIndex; i<model.size(); ++i )
  {

		model[i].v0 = R * model[i].v0;
		model[i].v1 = R * model[i].v1;
		model[i].v2 = R * model[i].v2;

		model[i].v0 *= 5;
		model[i].v1 *= 5;
		model[i].v2 *= 5;

		model[i].v0 += vec4(1,1,1,1);
		model[i].v1 += vec4(1,1,1,1);
		model[i].v2 += vec4(1,1,1,1);

    model[i].v0 -= vec4(1,1,1,1);
    model[i].v1 -= vec4(1,1,1,1);
    model[i].v2 -= vec4(1,1,1,1);

    model[i].v0.x *= -1;
    model[i].v1.x *= -1;
    model[i].v2.x *= -1;

    model[i].v0.y *= -1;
    model[i].v1.y *= -1;
    model[i].v2.y *= -1;

		model[i].v0.w = 1.0;
		model[i].v1.w = 1.0;
		model[i].v2.w = 1.0;

		model[i].v0 += vec4(0.,0.6,0.1,0);
		model[i].v1 += vec4(0.,0.6,0.1,0);
		model[i].v2 += vec4(0.,0.6,0.1,0);

    model[i].ComputeNormal();
		model[i].ReverseNormal();
  }
}

void LoadSphereModel(vector<Object>& objects, vec3 offset, float radius, int material) {
	Object sphere = Object(vec4(offset.x,offset.y,offset.z,1), vec4(0,0,0,1), vec4(0,0,0,1), radius, vec3(0.75f, 0.75f, 0.75f), "sphere", material);
	objects.push_back(sphere);
}

#endif
