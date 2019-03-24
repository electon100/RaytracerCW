#ifndef TEST_MODEL_BUNNY_H
#define TEST_MODEL_BUNNY_H

#include <iostream>
#include<fstream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>
#include <sstream>
#include <string>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;
using glm::vec2;

// Used to describe a triangular surface:
class Model
{
public:
	glm::vec4 v0;
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 normal;
	glm::vec3 color;

	Model( glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = glm::vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
	  glm::vec3 e2 = glm::vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
	  glm::vec3 normal3 = glm::normalize( glm::cross( e2, e1 ) );
	  normal.x = normal3.x;
	  normal.y = normal3.y;
	  normal.z = normal3.z;
	  normal.w = 1.0;
	}
};

void LoadBunnyModel( std::vector<Model>& model ) {

  float L = 555;
  vector<vec4> temp_vertices;

  model.clear();
  model.reserve( 5*2*3 );

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

      model.push_back(Model(temp_vertices[a-1], temp_vertices[b-1], temp_vertices[c-1], vec3(1, 1, 1)));

    }
  }

  // ----------------------------------------------
  // Scale to the volume [-1,1]^3

  for( size_t i=0; i<model.size(); ++i )
  {
    model[i].v0 *= 2/L;
    model[i].v1 *= 2/L;
    model[i].v2 *= 2/L;

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

    model[i].ComputeNormal();
  }
}

#endif
