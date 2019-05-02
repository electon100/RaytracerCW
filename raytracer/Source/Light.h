#ifndef LIGHT_H
#define LIGHT_H

// Defines a simple test model: The Cornel Box

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <stdint.h>
#include <sstream>
#include <string>

vec4 centrePos(0, -0.5, 0.f, 1.0);
vec3 colour = 7.f * vec3(1, 1, 1);

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;
using glm::vec2;

// Used to describe a triangular or spherical surface:
class Light {
public:
	glm::vec4 position;
	glm::vec3 colour;

	Light( glm::vec4 position, glm::vec3 colour )
		: position(position), colour(colour) {

	}

};

void LoadLights(vector<Light>& lights) {

  lights.clear();
  lights.reserve( 10*10 );

  for (int i = -3; i < 4; i++) {
    for (int j = -3; j < 4; j++) {
      float offsetX = float(i)/float(10);
      float offsetZ = float(j)/float(10);
      vec4 newLightPos = centrePos + vec4(offsetX, 0, offsetZ, 0);
      lights.push_back(Light(newLightPos, colour));
    }
  }
}

#endif
