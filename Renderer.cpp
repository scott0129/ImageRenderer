#include "Renderer.h"

//Some constants:
int const RES_WIDTH= 500;
int const RES_HEIGHT = 500;

int const NUM_SPHERES = 500;

double const PIX_SIZE = .1;

double const FOCAL_DIST = 50;

//Anti-aliasing coeff. Number of vectors shot through each pixel.
int const AA_COEFF = 1;

Vector const LIGHT_DIRECTION = Vector(-0.6, 0.6, 0.6).normalize();
png::color const LIGHT_COLOR = png::color(255, 255, 255);

png::color const AMBIENT_COLOR = png::color(255, 255, 255);

bool const ORTHOGRAPHIC = false
;

double Renderer::logit(double input) {
  return -log(1.0/((input+1)/2) - 1);
}

double Renderer::sigmoid(double input) {
  return (2/(1 + exp(-input))) - 1;
}

png::color Renderer::pixelScalar(png::color color1, double multip) {

  double red =  color1.red*multip;
  double green = color1.green*multip;
  double blue = color1.blue*multip;

  double max = std::max( std::max(red, green), blue);
  if (max > 255) {
    double multiplier = 255.0/max;
    red *= multiplier;
    green *= multiplier;
    blue *= multiplier;
  }

  return png::color(red, green, blue);
}

png::color Renderer::pixelMultiply(png::color color1, png::color color2) {

  double red = (color1.red * color2.red)/255.0;
  double green = (color1.green * color2.green)/255.0;
  double blue = (color1.blue * color2.blue)/255.0;

  return png::color(red, green, blue);

}


png::color Renderer::pixelAdd(png::color color1, png::color color2) {
  //sigmoid method
  double red = sigmoid( (logit(color1.red/255.0) + logit(color2.red/255.0)) ) * 255.0;
  double green = sigmoid( (logit(color1.green/255.0) + logit(color2.green/255.0)) ) * 255.0;
  double blue = sigmoid( (logit(color1.blue/255.0) + logit(color2.blue/255.0)) )* 255.0;


  return png::color(red, green, blue);

}

png::color Renderer::phongModel(Vector castPoint, Vector castRay, double tVal, Vector toLight, Hittable* object) {
  //ambient term
  png::color ambient = pixelMultiply(object->getAmbientK(), AMBIENT_COLOR);

  //diffuse term
  Vector collisionPoint = castPoint + castRay.scalar(tVal);

  png::color diffuse = png::color(0, 0, 0);;
  //check if the object's blocking the light. leave at 0 if it is.
  Vector normal = object->getNorm(collisionPoint);
  if ( normal.dot(toLight) > 0) {

    double coefficient = toLight.normalize().dot(object->getNorm(collisionPoint));

    diffuse = pixelMultiply(object->getDiffuseK(), LIGHT_COLOR);
    diffuse = pixelScalar(diffuse, coefficient);

  }

  //specular term

  //reflection equation
  png::color specular = png::color(0, 0, 0);
  Vector fromLight = toLight.scalar(-1);

  //Finding "reflection" vector, using projection of light onto normal.
  Vector projection = normal.scalar(normal.dot(toLight)/(normal.length() * normal.length()));
  Vector reflection = fromLight + projection.scalar(2);


  if ( /*normal.dot(toLight)*/ -reflection.dot(castRay) > 0) {
    double specularCoeff = pow(-reflection.dot(castRay), object->getSpecularExp() );

    specular = pixelScalar(object->getSpecularK(), specularCoeff);
    specular = pixelMultiply(specular, LIGHT_COLOR);

  }


  png::color totalColor = pixelAdd(pixelAdd(ambient, diffuse), specular);
  //png::color totalColor = pixelAdd(ambient, diffuse);


  return totalColor;
}

void Renderer::run() {
  srand(4);

  std::cout << "Beginning to draw " << NUM_SPHERES << " spheres, starting at: " << (float)clock()/CLOCKS_PER_SEC << " seconds" << std::endl;


  Hittable* objectsList[NUM_SPHERES];

  //generate all spheres and save in array
  for (int i = 0; i < NUM_SPHERES; i++) {
    Vector sphereLoc = Vector(300 + rand()%100, rand()%200 - 100, rand()%200 - 100);
    png::color ambient = png::color(rand()%255, rand()%255, rand()%255);
    png::color diffuse = png::color(rand()%255, rand()%255, rand()%255);
    png::color specular = png::color(rand()%255, rand()%255, rand()%255);
    Sphere* s = new Sphere(sphereLoc, 5, ambient, diffuse, specular, 30);
    objectsList[i] = s;
    if (sphereLoc[0] == 373.0) {
      std::cout << "brokenSphere High: " << s->hiCorner().toString() << " low: " << s->loCorner().toString() << std::endl;
    }
  }



  //find lowest point and highest bound of all objects
  Vector lowest = objectsList[0]->loCorner();
  Vector highest = objectsList[0]->hiCorner();
  for (int i = 1; i < NUM_SPHERES; i++) {
    Vector currentLo = objectsList[i]->loCorner();
    Vector currentHi = objectsList[i]->hiCorner();
    for (int dim = 0; dim < 3; dim++) {
      if (currentLo[dim] < lowest[dim]) {
        lowest[dim] = currentLo[dim];
      }
      if (currentHi[dim] > highest[dim]) {
        highest[dim] = currentHi[dim];
      }
    }
  }

  std::cout << highest.toString() << " " << lowest.toString() << std::endl;

  // png::color ambient1 = png::color(rand()%255, rand()%255, rand()%255);
  // png::color diffuse1 = png::color(rand()%255, rand()%255, rand()%255);
  // png::color specular1 = png::color(rand()%255, rand()%255, rand()%255);
  // Sphere* s1 = new Sphere(lowest, 3, ambient1, diffuse1, specular1, 30);
  //
  // png::color ambient2 = png::color(rand()%255, rand()%255, rand()%255);
  // png::color diffuse2 = png::color(rand()%255, rand()%255, rand()%255);
  // png::color specular2 = png::color(rand()%255, rand()%255, rand()%255);
  // Sphere* s2 = new Sphere(highest, 3, ambient2, diffuse2, specular2, 30);
  // objectsList[NUM_SPHERES - 1] = s1;
  // objectsList[NUM_SPHERES - 2] = s2;


  BVM objCollection = BVM(objectsList, NUM_SPHERES, lowest, highest);
  //nonBVM objCollection = nonBVM(objectsList, NUM_SPHERES);

  objCollection.printTree();

  std::cout << "Finished building tree by "  << (float)clock()/CLOCKS_PER_SEC << " seconds" << std::endl;

  Vector camPoint = Vector(0, 0, 0);
  Vector camVect = Vector(1, 0, 0);

  png::image<png::rgb_pixel> picture = png::image<png::rgb_pixel>(RES_WIDTH, RES_HEIGHT);


  for(int c = 0; c < RES_WIDTH; c++) {
    for (int r = 0; r < RES_HEIGHT; r++) {
        //Setting the location of each pixel, its point, and vector.
      double pixY = PIX_SIZE * (c - RES_WIDTH/2 + 0.5);
      double pixZ = PIX_SIZE * (-r + RES_HEIGHT/2 + 0.5);

      int allReds = 0;
      int allGreens = 0;
      int allBlues = 0;

      for(int vectNum = 0; vectNum < AA_COEFF; vectNum++) {
        double yJitter;
        double zJitter;
        if (AA_COEFF == 1) {
          yJitter = 0;
          zJitter = 0;
        } else {
          yJitter = PIX_SIZE*(rand()%1000 - 500)/1000.0;
          zJitter = PIX_SIZE*(rand()%1000 - 500)/1000.0;
        }
        Vector pixPoint = Vector(0, pixY + yJitter, pixZ + zJitter);
        Vector pixVect;

        if (ORTHOGRAPHIC) {
          pixVect = Vector(camVect);
        } else {
          Vector focalPoint = camPoint - camVect.normalize().scalar(FOCAL_DIST);
          pixVect = (pixPoint-focalPoint).normalize();
        }

        //Finding the closest "collision", looking through every object
        double minT = nan("1");

        Hittable* closestObj = objCollection.intersect(pixPoint, pixVect);
        if (closestObj != NULL) {
          minT = closestObj->collideT(pixPoint, pixVect);

          //set the pixel color to the color of the object doing crazy math
          png::color phongColor = phongModel(pixPoint, pixVect, minT, LIGHT_DIRECTION, closestObj);
          allReds += phongColor.red;
          allGreens += phongColor.green;
          allBlues += phongColor.blue;

        }
      }
      int avgRed = allReds/AA_COEFF;
      int avgGreen = allGreens/AA_COEFF;
      int avgBlue = allBlues/AA_COEFF;

      png::rgb_pixel pix = png::rgb_pixel(avgRed, avgGreen, avgBlue);

      picture.set_pixel(c, r, pix);
    }

  }

  picture.write("test.png");

  std::cout << "done! At " << (float)clock()/CLOCKS_PER_SEC << " seconds" << std::endl;


}
