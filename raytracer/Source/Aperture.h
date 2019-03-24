#ifndef APERTURE_H
#define APERTURE_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Aperture {
  public:
    glm::vec4 v0;
  	glm::vec4 v1;
  	glm::vec4 v2;
    glm::vec4 v3;
    glm::vec4 v4;
    glm::vec4 v5;
    glm::vec3 colour;
  	glm::vec4 normal;

    Aperture( glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec4 v3, glm::vec4 v4, glm::vec4 v5, glm::vec3 colour )
      : v0(v0), v1(v1), v2(v2), v3(v3), v4(v4), v5(v5), colour(colour) {
        ComputeNormal();
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

void LoadApertureHexagon(vector<Aperture> &hexagon) {
  vec4 A( -0.5, 0.5, 1.0, 1.0 );
  vec4 B( 0.5, 0.5, 1.0, 1.0 );
  vec4 C( 0.75, 0, 1.0, 1.0 );
  vec4 D( 0.5, -0.5, 1.0, 1.0 );
  vec4 E( -0.5, -0.5, 1.0, 1.0 );
  vec4 F( -0.75, 0, 1.0, 1.0 );


  hexagon.push_back(Aperture(A, B, C, D, E, F, vec3(0, 0, 1)));
  hexagon[0].ComputeNormal();
}

#endif
