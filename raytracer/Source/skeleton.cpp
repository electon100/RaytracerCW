#include <iostream>
#include <glm/glm.hpp>
#include "SDL.h"
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>
#include <math.h>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE false

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

struct Intersection {
  vec4 position;
  float distance;
  int triangleIndex;
};

bool Update(vec4& cameraPos, vector<Triangle>& triangles, float &yaw);
void Draw(screen* screen, float focalLength, vector<Triangle> triangles, vec4 cameraPos);
bool ClosestIntersection(vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);
vec3 DirectLight( const Intersection& i, const vector<Triangle> triangles);

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

<<<<<<< HEAD
  float z = -2.f;
  float focalLength = SCREEN_WIDTH/2;
=======
  float z = -2.8f;
  float focalLength = SCREEN_HEIGHT;
>>>>>>> 853468ad879a795a7a1d9e166b07d821e86e8452
  vec4 cameraPos ( 0.0f, 0.0f, z, 1.0f);

  vector<Triangle> triangles;
  LoadTestModel(triangles);
  float yaw = 0.0f;
  while( Update(cameraPos, triangles, yaw) )
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

  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      vec4 start = cameraPos;
      vec4 direction = vec4(x-SCREEN_WIDTH/2, y-SCREEN_HEIGHT/2, focalLength, 1.0f);
      if (ClosestIntersection(start, direction, triangles, closestIntersection)) {
        vec3 p = triangles[closestIntersection.triangleIndex].color;
        vec3 D = DirectLight(closestIntersection, triangles);
        PutPixelSDL(screen, x, y, p*D);
      }
      else {
        PutPixelSDL(screen, x, y, vec3(0, 0, 0));
      }
    }
  }
}

/*Place updates of parameters here*/
bool Update(vec4& cameraPos, vector<Triangle>& triangles, float& yaw)
{
  static int t = SDL_GetTicks();
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  /*Good idea to remove this*/
  std::cout << "Render time: " << dt << " ms." << std::endl;
  /* Update variables*/
  vec4 a(glm::cos(yaw), 0.0f, glm::sin(yaw), 0.0f);
  vec4 b(0.0f, 1.0f, 0.0f, 0.0f);
  vec4 c(-glm::sin(yaw), 0.0f, glm::cos(yaw), 0.0f);
  vec4 zs(0.0f, 0.0f, 0.0f, 1.0f);

  mat4 R(a, b, c, zs);
  mat4 RC = glm::inverse(R);
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
          yaw += 0.1f;
          cameraPos = cameraPos * R;
          break;
        case SDLK_RIGHT:
          /* Move camera right */
          yaw -= 0.1f;
          cameraPos = cameraPos * RC;
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

    mat3 A(vec3(-d),e1,e2);
    vec3 x = glm::inverse(A)*b;

    float t = x.x;
    float u = x.y;
    float v = x.z;

    vec4 ray = s + t*d;

    if (u >= 0 && v >= 0 && ((u + v) <= 1) && t >= 0) {
      float currentDistance = glm::distance(ray, s);
      if (currentDistance < closestDistance) {
        closestDistance = currentDistance;
        closestIntersection.position = ray;
        closestIntersection.distance = closestDistance;
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
  vec4 start = lightPos;
  vec4 directionToLight = i.position - lightPos;
  if (ClosestIntersection(start, directionToLight, triangles, closestIntersection)){
    if (abs(closestIntersection.triangleIndex - i.triangleIndex) < 2) {
      return D;
    }
    else return vec3(0, 0, 0);
  }
  return D;
}
