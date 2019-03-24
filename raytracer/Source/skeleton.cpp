#include <iostream>
#include <glm/glm.hpp>
#include "SDL.h"
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include "Aperture.h"
#include "Lens.h"
#include <stdint.h>
#include <math.h>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE true

float xaw = 0.f;
float yaw = 0.f;
float zaw = 0.f;
/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

struct Intersection {
  vec4 position;
  float distance;
  int triangleIndex;
};

bool Update(vec4& cameraPos, vector<Triangle>& triangles);
void Draw(screen* screen, float focalLength, vector<Triangle> triangles, vec4 cameraPos);
bool ClosestIntersection(vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
vec3 DirectLight( const Intersection& i, const vector<Triangle> triangles);

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  float z = -2.f;
  float focalLength = SCREEN_WIDTH/2;
  vec4 cameraPos ( 0.0f, 0.0f, z, 1.0f);

  vector<Triangle> triangles;
  LoadTestModel(triangles);
  vector<Aperture> aperture;
  LoadApertureHexagon(aperture);
  vector<Lens> lens;
  LoadLens(lens, 0.1);

  mat4 R;
  while( Update(cameraPos, triangles) )
    {
      Draw(screen, focalLength, triangles, cameraPos);
      SDL_Renderframe(screen);
    }
  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen, float focalLength, vector<Triangle> triangles, vec4 cameraPos)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  Intersection closestIntersection = {
    vec4(0, 0, 0, 0),
    0.0f,
    0
  };

  mat4 R ( cos(yaw)*cos(zaw), -cos(yaw)*sin(zaw) + sin(xaw)*sin(yaw)*cos(zaw), sin(xaw)*sin(zaw) + cos(xaw)*sin(yaw)*cos(zaw), 0,
        cos(yaw)*sin(zaw), cos(xaw)*cos(zaw) + sin(xaw)*sin(yaw)*sin(zaw), -sin(xaw)*cos(zaw) + cos(xaw)*sin(yaw)*sin(zaw), 0,
        -sin(yaw)        , sin(xaw)*cos(yaw)                              , cos(xaw)*cos(yaw)                             , 0,
        0                , 0                                              , 0                                             , 1);

  vec4 tmp_cameraPos = R * cameraPos;
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      vec4 direction = vec4(x-SCREEN_WIDTH/2, y-SCREEN_HEIGHT/2, focalLength, 1.0f);
      if (ClosestIntersection(tmp_cameraPos, R * direction, triangles, closestIntersection)) {
        vec3 p = triangles[closestIntersection.triangleIndex].color;
        vec3 D = DirectLight(closestIntersection, triangles);
        vec3 indirectLight = 0.5f*vec3( 1, 1, 1);
        PutPixelSDL(screen, x, y, p*(D+indirectLight));
      }
      else {
        PutPixelSDL(screen, x, y, vec3(0, 0, 0));
      }
    }
  }
}

/*Place updates of parameters here*/
bool Update(vec4& cameraPos, vector<Triangle>& triangles)
{
  static int t = SDL_GetTicks();
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  /*Good idea to remove this*/
  std::cout << "Render time: " << dt << " ms." << std::endl;

  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT)
    {
      return false;
    }
    else if (e.type == SDL_KEYDOWN)
    {
      int key_code = e.key.keysym.sym;
      switch(key_code)
      {
        case SDLK_UP:
          /* Move camera forwards */
          cameraPos.z += 1.0f;
          break;
        case SDLK_DOWN:
          /* Move camera backwards */
          cameraPos.z -= 1.0f;
          break;
        case SDLK_LEFT:
          /* Move camera left */
          cameraPos.x -= 1.0f;
          break;
        case SDLK_RIGHT:
          /* Move camera right */
          cameraPos.x += 1.0f;
          break;
        case SDLK_w:
          xaw += 0.1f;
          break;
        case SDLK_s:
          xaw -= 0.1f;
          break;
        case SDLK_a:
          yaw -= 0.1f;
          break;
        case SDLK_d:
          yaw += 0.1f;
          break;
        case SDLK_ESCAPE:
          /* Move camera quit */
          return false;
      }
    }
  }

  return true;
}

/* Finds closest intersection*/
bool ClosestIntersection(vec4 s, vec4 d, const vector<Triangle>& triangles, Intersection& closestIntersection) {
  float maximum = std::numeric_limits<float>::max();
  float closestDistance = maximum;

  d = glm::normalize(d);

  for (size_t i = 0; i < triangles.size(); i++) {
    vec4 v0 = triangles[i].v0;
    vec4 v1 = triangles[i].v1;
    vec4 v2 = triangles[i].v2;

    vec3 e1 = vec3(v1.x-v0.x, v1.y-v0.y, v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x, v2.y-v0.y, v2.z-v0.z);
    vec3 b = vec3(s.x-v0.x, s.y-v0.y, s.z-v0.z);

    mat3 D(vec3(-d), e1, e2);
    mat3 DX(b, e1, e2);
    mat3 DY(vec3(-d), b, e2);
    mat3 DZ(vec3(-d), e1, b);

    float det_d = determinant(D);
    float det_dx = determinant(DX);
    float det_dy = determinant(DY);
    float det_dz = determinant(DZ);

    float t = det_dx/det_d;
    float u = det_dy/det_d;
    float v = det_dz/det_d;

    // float r = ((double) rand() / (RAND_MAX)) + 1;
    // triangles[i].normal += r;

    vec4 ray = s + t*d;

    if (u >= 0 && v >= 0 && ((u + v) <= 1) && t > 0) {
      float currentDistance = glm::distance(ray, s);
      if (currentDistance < closestDistance) {
        closestDistance = currentDistance;
        closestIntersection.position = ray;
        closestIntersection.distance = currentDistance;
        closestIntersection.triangleIndex = i;
      }
    }
  }

  if (closestDistance < maximum) {
    return true;
  }
  else {
    return false;
  }
}

vec3 DirectLight(const Intersection& i, const vector<Triangle> triangles){
  vec4 lightPos(0, -0.5, -0.7, 1.0);
  vec3 lightColour = 14.f * vec3(1, 1, 1);

  float r = glm::distance(i.position, lightPos);
  float A = 4 * M_PI * r * r;
  vec3 B = lightColour / A;
  vec4 r_hat = glm::normalize(lightPos - i.position);
  vec4 n = triangles[i.triangleIndex].normal;
  vec3 D = B * max(glm::dot(n, r_hat), 0.0f);

  Intersection closestIntersection = {
    vec4(0, 0, 0, 0),
    0.0f,
    0
  };

  vec4 start = i.position + 0.001f * triangles[i.triangleIndex].normal;
  vec4 directionToLight = lightPos - start;
  if (ClosestIntersection(start, directionToLight, triangles, closestIntersection)){
    float lightDistance = r;
    float vectDistance = closestIntersection.distance;
    if (vectDistance < lightDistance) {
      return vec3(0, 0, 0);
    }
    else return D;
  }
  return D;
}
