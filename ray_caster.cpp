#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "vector3d.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "ellipsoid.hpp"
#include "scene.hpp"
#include "intersections.hpp"

using namespace std;

#ifndef M_PI
  #define M_PI 3.1415926535
#endif

string traceRay(const Ray& ray, const Scene& scene) {
  int nearestObjIndex = -1;
  float minT = -1;
  MaterialColor nearestObjColor;
  // Inspect intersection with all spheres
  for (int i = 0; i < scene.spheres.size(); ++i) {
    float t = smallestPositiveT(ray, scene.spheres[i]);
    if (t >= 0) {
      if (nearestObjIndex == -1) {
        nearestObjIndex = i;
        nearestObjColor = scene.spheres[i].materialColor;
        minT = t;
      } else {
        if (t < minT) {
          minT = t;
          nearestObjIndex = i;
          nearestObjColor = scene.spheres[i].materialColor;
        }
      }
    }
  }

  // Inspect intersection with all ellipsoids
  for (int i = 0; i < scene.ellipsoids.size(); ++i) {
    float t = smallestPositiveT(ray, scene.ellipsoids[i]);
    if (t >= 0) {
      if (nearestObjIndex == -1) {
        nearestObjIndex = i;
        nearestObjColor = scene.ellipsoids[i].color;
        minT = t;
      } else {
        if (t < minT) {
          minT = t;
          nearestObjIndex = i;
          nearestObjColor = scene.ellipsoids[i].color;
        }
      }
    }
  }

  if (nearestObjIndex >= 0) {
    return nearestObjColor.to8BitScale();
  } else {
    return (scene.bgColor * ray.direction.dot(scene.viewDir.unit())).to8BitScale();
  }
}

int main(int argc, char *argv[]) {
  // Restricting number of commandline arguments
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <inputfile>" << endl;
    exit(-1);
  }
  string filename(argv[1]);

  // Read scene description from input file
  Scene scene(filename);
  if (!scene.readAndValidate()) {
    return -1;
  };

  cout << scene << endl;

  // Preliminary calculations
  Vector3D u = scene.viewDir.cross(scene.upDir);
  Vector3D v = u.cross(scene.viewDir);
  Vector3D n = u.cross(v);

  float d = scene.imHeight / (2 * tan(scene.vFovDeg * M_PI / 360));

  Vector3D imageCenter = scene.eye + scene.viewDir.unit() * d;
  Vector3D ul = imageCenter - u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
  Vector3D ur = imageCenter + u * (scene.imWidth / 2) + v * (scene.imHeight / 2);
  Vector3D ll = imageCenter - u * (scene.imWidth / 2) - v * (scene.imHeight / 2);
  Vector3D lr = imageCenter + u * (scene.imWidth / 2) - v * (scene.imHeight / 2);

  Vector3D delWidth = (ur - ul) * (1 / (scene.imWidth - 1));
  Vector3D delHeight = (ll - ul) * (1 / (scene.imHeight - 1));

  // Initialize pixel array for output image
  vector<vector<string> > colors;
  for (int i = 0; i < scene.imWidth; i++) {
    vector<string> col;
    col.resize(scene.imHeight);
    colors.push_back(col);
  }

  // Ray tracing per pixel
  for (int j = 0; j < scene.imHeight; j++) {
    for (int i = 0; i < scene.imWidth; i++) {
      // (i, j) pixel coordinate
      Vector3D pixelCoordinate = ul + delWidth * i + delHeight * j;
      // ray from eye to that pixel
      Ray ray(scene.eye, (pixelCoordinate - scene.eye).unit());
      // trace this ray in the scene to produce a color for the pixel
      string pixelValue = traceRay(ray, scene);
      // Keep track of color
      colors[i][j] = pixelValue;
    }
  }


  // Write the final image to an output file
  string outputfileString(filename);
  int len = outputfileString.size();
  outputfileString.replace(len - 3, 3, "ppm");

  // Creating PPM image
  ofstream outputfile;
  outputfile.open(outputfileString);

  // Filling in the header
  outputfile << "P3" << endl << "# image autogenerated using a simple ray tracer" << endl;
  outputfile << scene.imWidth << " " << scene.imHeight << endl;
  outputfile << 255 << endl;

  // Filling in the body
  for (int j = 0; j < scene.imHeight; ++j) {
    for (int i = 0; i < scene.imWidth; ++i) {
      // Write the R G B values of a pixel in seperate line
      outputfile << colors[i][j] << endl;
    }
  }

  // Closing the output stream
  outputfile.close();

  return 0;
}
