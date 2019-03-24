#ifndef LENS_H
#define LENS_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

class Lens {
  public:
    glm::vec4 top;
    glm::vec4 bottom;
    glm::vec4 centre;
    float radius;
    float thickness;
    vector<float> thicknesses;

    Lens( glm::vec4 top, glm::vec4 bottom, glm::vec4 centre, float thickness )
      : top(top), bottom(bottom), centre(centre), thickness(thickness) {
        ComputeRadius();
        ComputeThickness();
    }

    void ComputeRadius() {
      radius = glm::distance(top, centre);
    }

    void ComputeThickness() {
      vector<float> tmp_thicknesses(21);
      int size = 20;

      for (int i = 0; i < size; i++) {
        tmp_thicknesses[i] = thickness/(i+1);
      }
      tmp_thicknesses[20] = 0.f;

      for (int i = 0; i < tmp_thicknesses.size(); i++) {
        cout << tmp_thicknesses[i] << endl;
      }

      thicknesses = tmp_thicknesses;

    }
};

void LoadLens(vector<Lens> &lens, float thickness) {
  vec4 A( 0, 0.75, 1.0, 1.0 );
  vec4 B( 0, -0.75, 1.0, 1.0 );
  vec4 C( 0, 0, 0.8, 1.0 );

  lens.push_back(Lens(A, B, C, thickness));
}

#endif
