#include <iostream>
#include <glm/glm.hpp>
#include "SDL.h"
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include "Light.h"
#include <stdint.h>
#include <math.h>
#include <complex>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE false
#define _USE_MATH_DEFINES

struct Intersection {
  vec4 position;
  float distance;
  int objectIndex;
};

float xaw = 0.f;
float yaw = 0.f;
float zaw = 0.f;

vec4 lightPos(0, -0.5, 0.f, 1.0);
vec3 lightColour = 7.f * vec3(1, 1, 1);
float z = -2.f;
float focalLength = SCREEN_WIDTH/2;
vec4 cameraPos ( 0.0f, 0.0f, z, 1.0f);
Intersection currentIntersection;

mat4 R;
/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */


bool Update(vector<Object>& objects);
void Draw(screen* screen, float focalLength, vector<Object>& objects, vector<Light>& lights);
vec3 GetColourForRay(vec4 start, vec4 direction, vector<Object>& objects, vector<Light>& lights, int depth);
bool ClosestIntersection(vec4 start, vec4 dir, vector<Object>& objects, Intersection& closestIntersection);
vec3 GetDirectLightForPixel(const vec4 position, const vec4 normal, const vector<Object>& objects, const vector<Light>& lights);
vec3 GetIndirectLightForPixel(const vec4 position, const vec4 normal, vector<Object>& objects, int depth);
vec3 DirectLight( const vec4 position, const vec4 normal );
vec3 SpecularLight(const vec4 position, const vec4 normal, vec4 direction);
float findNorm(vec4 vector);
vec3 DirectLightMirror(Intersection& i, vector<Object> objects);
vec3 castRay(vec4 position, vec4 normal, vec4 direction, int depth, vector<Object> objects, vec3 colour, int material);
void createCoordinateSystem(const vec4 &normal, vec3 &Nt, vec3 &Nb);
vec3 uniformSampleHemisphere(const float &r1, const float &r2);
bool FindClosestLight(vec4 position, vec4 normal, Intersection& closestLight, vector<Object> objects);
bool FindClosestLightMirror(const Intersection& i, Intersection& closestLight, vector<Object> objects);
vec4 reflect(const vec4 &I, const vec4 &N);
vec4 refract (vec4& I, vec4& N, float& eta);
void fresnel(const vec4 &I, const vec4 &N, const float &ior, float &kr);
bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

int main( int argc, char* argv[] ) {

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  vector<Light> lights;

  vector<Object> objects;
  LoadTestModel(objects);
  LoadLights(objects);
  lightPos = (objects[objects.size()-1].v0 + objects[objects.size()-1].v1 + objects[objects.size()-1].v2 + objects[objects.size()-2].v2)/4.f;
  lightPos.y += 0.01f;
  LoadSphereModel(objects, vec3(0.4f, -0.1f, 0.1f), 0.1f, 2);
  LoadSphereModel(objects, vec3(-0.4f, 0.6f, -0.4f), 0.1f, 3);
  // LoadBunnyModel(objects);
  // LoadSphereModel(objects, vec3(0.5f, 0.67f, 0.f), 0.1f, 3);

  // while( Update(objects) ) {
  //   SDL_Renderframe(screen);
  // }
  Draw(screen, focalLength, objects, lights);

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen, float focalLength, vector<Object>& objects, vector<Light>& lights) {
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  vec4 tmp_cameraPos = R * cameraPos;
  vec3 colour = vec3(0, 0, 0);
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      vec4 one = vec4(x+drand48()-SCREEN_WIDTH/2, y+drand48()-SCREEN_HEIGHT/2, focalLength, 1.0f);
      colour += GetColourForRay(/*start*/ tmp_cameraPos, /*direction*/ R*one, objects, lights, 0);
      vec4 two = vec4(x+drand48()-SCREEN_WIDTH/2, y+drand48()-SCREEN_HEIGHT/2, focalLength, 1.0f);
      colour += GetColourForRay(/*start*/ tmp_cameraPos, /*direction*/ R*two, objects, lights, 0);
      vec4 three = vec4(x+drand48()-SCREEN_WIDTH/2, y+drand48()-SCREEN_HEIGHT/2, focalLength, 1.0f);
      colour += GetColourForRay(/*start*/ tmp_cameraPos, /*direction*/ R*three, objects, lights, 0);
      vec4 four = vec4(x+drand48()-SCREEN_WIDTH/2, y+drand48()-SCREEN_HEIGHT/2, focalLength, 1.0f);
      colour += GetColourForRay(/*start*/ tmp_cameraPos, /*direction*/ R*four, objects, lights, 0);

      colour /= 4.f;

      PutPixelSDL(screen, x, y, colour);
    }
  }
}

/*Place updates of parameters here*/
bool Update(vector<Object>& objects) {
  static int t = SDL_GetTicks();
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  /*Good idea to remove this*/
  std::cout << "Render time: " << dt << " ms." << std::endl;

  SDL_Event e;
  while(SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    }
    else if (e.type == SDL_KEYDOWN) {
      int key_code = e.key.keysym.sym;
      switch(key_code) {
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
        case SDLK_1:
          lightPos.x += 0.1f;
          break;
        case SDLK_2:
          lightPos.x -= 0.1f;
          break;
        case SDLK_3:
          lightPos.y += 0.1f;
          break;
        case SDLK_4:
          lightPos.y -= 0.1f;
          break;
        case SDLK_ESCAPE:
          /* Move camera quit */
          return false;
      }
    }
  }
  RotationMatrixByAngle(xaw, yaw, zaw, R);
  return true;
}

vec3 GetColourForRay(vec4 start, vec4 direction, vector<Object>& objects, vector<Light>& lights, int depth) {
  int maxDepth = 4;
  if (depth > maxDepth) return vec3(0, 0, 0);

  Intersection closestIntersection = {
    vec4(0, 0, 0, 0),
    0.0f,
    0
  };

  /* Colours and lights */
  vec3 returnColour = vec3(0, 0, 0);
  vec3 directLight = vec3(0, 0, 0);
  vec3 indirectLight = vec3(0, 0, 0);

  if (ClosestIntersection(start, direction, objects, closestIntersection)) {
    int material = objects[closestIntersection.objectIndex].material;
    vec3 colour = objects[closestIntersection.objectIndex].color;
    vec4 normal = objects[closestIntersection.objectIndex].normal;
    vec4 position = closestIntersection.position;
    vec4 bias;
    bool outside;
    vec4 reflectionDirection;
    vec4 reflectionRayOrig;
    vec3 reflectionColor;
    vec4 refractionDirection;
    vec4 refractionRayOrig;
    vec3 refractionColor;
    float ior;

    switch(material) {
      case 0: /* diffuse */
        directLight = GetDirectLightForPixel(position, normal, objects, lights);
        indirectLight = GetIndirectLightForPixel(position, normal, objects, 0);
        returnColour = colour * (indirectLight);
        break;
      case 2: /* mirror */
        bias = 0.001f*normal;
        outside = dot(direction, normal) < 0;

        reflectionDirection = reflect(direction, normal);
        reflectionRayOrig = outside ? position + bias : position - bias;

        reflectionColor += GetColourForRay(reflectionRayOrig, reflectionDirection, objects, lights, depth+1);

        returnColour = reflectionColor;
        break;
      case 3: /* glass */
        bias = 0.001f*normal;
        outside = dot(direction, normal) < 0;
        ior = 1.5;

        float kr;
        fresnel(direction, normal, ior, kr);

        if (kr < 1) {
          refractionRayOrig = outside ? position - bias : position + bias;
          refractionDirection = refract(direction, normal, ior);
          refractionColor += GetColourForRay(refractionRayOrig, refractionDirection, objects, lights, depth+1);
        }

        reflectionDirection = reflect(direction, normal);
        reflectionRayOrig = outside ? position + bias : position - bias;
        reflectionColor += GetColourForRay(reflectionRayOrig, reflectionDirection, objects, lights, depth+1);

        returnColour = reflectionColor*kr + refractionColor*(1-kr);
        break;
      case 4: /* light */
        returnColour = 8.1f*vec3(1, 1, 1) * colour;
        break;
      default:
        returnColour = returnColour;
        break;
    }
  }

  return returnColour;
}

vec3 GetDirectLightForPixel(const vec4 position, const vec4 normal, const vector<Object>& objects, const vector<Light>& lights) {
  Intersection closestLight = {
    vec4(0, 0, 0, 0),
    0.0f,
    0
  };

  float r = glm::distance(position, lightPos);
  float A = 4 * M_PI * r * r;
  vec3 B = lightColour / A;
  vec4 r_hat = (lightPos - position);
  vec4 n = normal;
  vec3 D = B * max(glm::dot(n, r_hat), 0.0f);

  if (FindClosestLight(position, normal, closestLight, objects)){
    float lightDistance = glm::distance(position, lightPos);
    float vectDistance = closestLight.distance;
    if (vectDistance < lightDistance) { /* Case where the object is in shadow */
      D = vec3(0, 0, 0);
    }
  }

  return D;
}

vec3 GetIndirectLightForPixel(const vec4 position, const vec4 normal, vector<Object>& objects, int depth) {
  int maxDepth = 5;
  if (depth > maxDepth) return vec3(0, 0, 0);

  int someNumberOfRays = 32;
  vec3 Nt;
  vec3 Nb;
  vec3 lightFromSource = vec3(0, 0, 0);
  vec3 indirectLight = vec3(0, 0, 0);
  vec3 hitColour = vec3(0, 0, 0);
  Intersection closestObject = {
    vec4(0, 0, 0, 0),
    0.0f,
    0
  };

  // Compute space
  createCoordinateSystem(normal, Nt, Nb);

  // Create vectors at random directions and see what they hit
  for (int i = 0; i < someNumberOfRays; i++) {
    float r1 = drand48();
    float r2 = drand48();
    vec3 sample = uniformSampleHemisphere(r1, r2);
    vec3 sampleWorld = vec3(sample.x * Nb.x + sample.y * normal.x + sample.z * Nt.x,
                            sample.x * Nb.y + sample.y * normal.y + sample.z * Nt.y,
                            sample.x * Nb.z + sample.y * normal.z + sample.z * Nt.z);

    vec4 start = vec4(sampleWorld, 1.0f);
    vec4 direction = position - start;

    // Get the hit object's colour and add it on to the final colour
    if (ClosestIntersection(start + 0.001f * normal, direction, objects, closestObject)) {
      hitColour = objects[closestObject.objectIndex].color;
      int material = objects[closestObject.objectIndex].material;
      if (material == 4) {
        lightFromSource += vec3(0.95, 0.95, 0.95);
      }
      indirectLight += glm::dot(vec3(normal), sampleWorld) * hitColour + GetIndirectLightForPixel(closestObject.position, objects[closestObject.objectIndex].normal, objects, depth+1);
    }
  }

  indirectLight /= float(someNumberOfRays)*float(maxDepth+1);

  indirectLight += lightFromSource;

  return indirectLight;
}

/* Finds closest intersection*/
bool ClosestIntersection(vec4 s, vec4 d, vector<Object>& objects, Intersection& closestIntersection) {
   float maximum = std::numeric_limits<float>::max();
   float closestDistance = maximum;

   d = glm::normalize(d);


   for (size_t i = 0; i < objects.size(); i++) {
     if (objects[i].type == "sphere") {
       float t0, t1; // solutions for t if the ray intersects
       // analytic solution
       vec4 L = s - objects[i].v0;
       float a = dot(d, d);
       float b = 2 * dot(d, L);
       float c = dot(L, L) - objects[i].radius;
       if (solveQuadratic(a, b, c, t0, t1)) {
         if (t0 > t1) std::swap(t0, t1);

         if (t0 < 0) {
           t0 = t1; // if t0 is negative, let's use t1 instead
         }

         if (t0 > 0) {
           t1 = t0;

           vec4 ray = s + t0 * normalize(d);
           float currentDistance = glm::distance(ray, s);
           if (currentDistance < closestDistance) {
             closestDistance = currentDistance;
             closestIntersection.position = ray;
             closestIntersection.distance = currentDistance;
             closestIntersection.objectIndex = i;
             objects[i].normal = normalize(ray - objects[i].v0);
           }
         }
       }
     } else if (objects[i].type == "triangle") {
       vec4 v0 = objects[i].v0;
       vec4 v1 = objects[i].v1;
       vec4 v2 = objects[i].v2;

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

       vec4 ray = s + t*d;

       if (u >= 0 && v >= 0 && ((u + v) <= 1) && t > 0) {
         float currentDistance = glm::distance(ray, s);
         if (currentDistance < closestDistance) {
           closestDistance = currentDistance;
           closestIntersection.position = ray;
           closestIntersection.distance = currentDistance;
           closestIntersection.objectIndex = i;
         }
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

bool FindClosestLight(vec4 position, vec4 normal, Intersection& closestLight, vector<Object> objects) {
  vec4 start = position + 0.001f * normal;
  vec4 directionToLight = lightPos - start;

  if (ClosestIntersection(start, directionToLight, objects, closestLight)){
    return true;
  }
  else return false;
}

vec3 SpecularLight(const vec4 position, const vec4 normal, vec4 direction) {
  float r = glm::distance(position, lightPos);
  float A = 4 * M_PI * r * r;
  vec3 B = lightColour / A;
  vec4 r_hat = normalize(lightPos - position);
  vec4 cameraRay = normalize(R*cameraPos - position);
  vec4 reflected = (cameraRay + r_hat);
  float norm = findNorm(reflected);
  float r_dot_v = max(glm::dot(reflected/norm, cameraRay), 0.0f);
  vec3 specular = B * pow(r_dot_v, 1.0f/20.0f);

  return specular;
}

float findNorm(vec4 vector) {
  float norm = vector.x*vector.x + vector.y*vector.y + vector.z*vector.z + vector.w*vector.w;

  return sqrtf(norm);
}

vec3 DirectLightMirror(Intersection& i, vector<Object> objects){
  float r = glm::distance(i.position, lightPos);
  float A = 4 * M_PI * r * r;
  vec3 B = lightColour / A;
  vec4 r_hat = glm::normalize(lightPos - i.position);
  vec4 n = objects[i.objectIndex].normal;
  vec3 D = B * max(glm::dot(n, r_hat), 0.0f);

  return D;
}

bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);

    return true;
}

float clamp(const float &lo, const float &hi, const float &v){
  return std::max(lo, std::min(hi, v));
}

vec4 reflect(const vec4 &I, const vec4 &N) {
  return I + 2 * dot(N, -I) * N;
}

vec4 refract (vec4& I, vec4& N, float& ior) {
  bool outside = dot(I, N) < 0;
  float eta = (outside) ? 1/ior : ior; // are we inside or outside the surface?
  float cosi = dot(N, I);
  float k = 1 - eta * eta * (1 - cosi * cosi);
  vec4 refrdir = I * eta + N * (eta *  cosi - sqrtf(k));

  return refrdir;
}

void fresnel(const vec4 &I, const vec4 &N, const float &ior, float &kr) {
    float cosi = clamp(-1, 1, dot(I, N));
    float etai = 1, etat = ior;
    if (cosi > 0) { std::swap(etai, etat); }
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection
    if (sint >= 1) {
        kr = 1;
    }
    else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

void createCoordinateSystem(const vec4 &normal, vec3 &Nt, vec3 &Nb) {
  if (std::fabs(normal.x) > std::fabs(normal.y))
    Nt = glm::normalize(vec3(normal.z, 0, -normal.x));
  else
    Nt = glm::normalize(vec3(0, -normal.z, normal.y));
  Nb = cross(vec3(normal), Nt);
}

vec3 uniformSampleHemisphere(const float &r1, const float &r2) {
  // cos(theta) = r1 = y
  // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
  float sinTheta = sqrtf(1 - r1 * r1);
  float phi = 2 * M_PI * r2;
  float x = sinTheta * cosf(phi);
  float z = sinTheta * sinf(phi);
  return vec3(x, r1, z);
}
