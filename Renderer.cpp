#include "Renderer.h"
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <map>

//Some constants:
std::string const OUTPUT_FILENAME = "teapot_BVH_1200x1200.png";
int const DEFAULT_IMG_WIDTH= 600;
int const DEFAULT_IMG_HEIGHT = 600;

double const PIX_SIZE = .01;

/*
takes 52 seconds:
400x400
teapot
pix_size = 0.01
standard -5, 5, 5, 1 -1 -1
*/

//for 900x900 bunny
//double const PIX_SIZE = .016;

double const FOCAL_DIST = 50;

//Anti-aliasing coeff. Number of vectors shot through each pixel.
int const AA_COEFF = 1;

Vector const LIGHT_DIRECTION = Vector(0.7, -0.3, -0.4).normalize();

png::color const LIGHT_COLOR = png::color(255, 255, 255);

png::color const AMBIENT_COLOR = png::color(255, 255, 255);

bool const ORTHOGRAPHIC = false;

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

png::color Renderer::phongModel(const Vector* castPoint, const Vector* castRay, double tVal, const Vector* fromLight, Hittable* object) {
    Vector toLight = fromLight->scalar(-1);

    //ambient term
    png::color ambient = pixelMultiply(object->getAmbientK(), AMBIENT_COLOR);

    //diffuse term
    Vector collisionPoint = *castPoint + castRay->scalar(tVal);

    png::color diffuse = png::color(0, 0, 0);;
    //check if the object's blocking the light. leave at 0 if it is.
    Vector normal = object->getNorm(&collisionPoint);

    if ( normal.dot(&toLight) > 0) {
        double coefficient = toLight.normalize().dot(&normal);

        diffuse = pixelMultiply(object->getDiffuseK(), LIGHT_COLOR);
        diffuse = pixelScalar(diffuse, coefficient);
    }

    //specular term

    png::color specular = png::color(0, 0, 0);

    //Finding "reflection" vector, using projection of light onto normal.
    Vector projection = normal.scalar(normal.dot(&toLight)/(normal.length() * normal.length()));
    Vector reflection = *fromLight + projection.scalar(2);


    if ( -reflection.dot(castRay) > 0) {
        double specularCoeff = pow(-reflection.dot(castRay), object->getSpecularExp() );

        specular = pixelScalar(object->getSpecularK(), specularCoeff);
        specular = pixelMultiply(specular, LIGHT_COLOR);
    }


    png::color totalColor = pixelAdd(pixelAdd(ambient, diffuse), specular);
    //png::color totalColor = pixelAdd(ambient, diffuse);
    return totalColor;
}

Vector Renderer::getVertex(const std::string& line) {
    int end = line.length() - 1;
    while (line.substr(end, 1) == " ") {
        end--;
    }

  double axes[3];
  int count = 2;

  for (int i = end - 1; i > 0; i--) {
    if (line.substr(i, 1) == " ") {
      const char* charArray = line.substr(i + 1, end - i + 1).c_str();

      axes[count] = strtod(charArray, NULL);
      count--;
      end = i;
    }
  }

  return Vector(axes[0], axes[1], axes[2]);
}

void Renderer::getArguments(int const argc, char* const argv[], std::string* inputFilename, std::string* outputFilename, int* width, int* height) {
    if (argc < 3 || argc > 5) {
        std::cout << "\n\n\tusage: ./main <input obj filename> <output png filename> [img Height] [img Width] \n\n" <<std::endl;;
        exit(1);
    }

    *inputFilename = argv[1];
    *outputFilename = argv[2];

    *width = DEFAULT_IMG_WIDTH;
    *height = DEFAULT_IMG_HEIGHT;

    if (argc > 3) {
        *width = atoi(argv[3]);
    }
    if (argc > 4) {
        *height = atoi(argv[4]);
    }
}


void Renderer::run(int const argc, char* const argv[]) {

    std::string inputFileName;
    std::string outputFileName;
    int pngWidth;
    int pngHeight;

    getArguments(argc, argv, &inputFileName, &outputFileName, &pngWidth, &pngHeight);

    srand(time(NULL));


    std::cout << "Beginning to draw objects" << std::endl;;
    double startTime = (double)clock()/CLOCKS_PER_SEC;

    //Bounding Volume Heirarchy
    std::vector<Hittable*> objectsList;

    //Various color coeffecients
    png::color ambient = png::color(10, 10, 10);
    png::color diffuse = png::color(60, 60, 60);
    png::color specular = png::color(255, 255, 255);

    std::vector<Vector> vertexes;

    std::fstream inFile;
    inFile.open(inputFileName);

    std::string line;
    if (inFile.is_open()) {
        while (getline(inFile, line)) {
            if (line.substr(0, 1) == "v") {
                vertexes.push_back(getVertex(line));
            } else if (line.substr(0, 1) == "f") {
                Vector tuple = getVertex(line);
                Triangle* newShape = new Triangle(vertexes[(int) tuple[0] - 1], vertexes[(int)tuple[1] - 1], vertexes[(int) tuple[2] - 1],
                                  ambient, diffuse, specular, 10);
                objectsList.push_back(newShape);
            }
        }
    }

    inFile.close();

    //Randomly shuffle objects to create a more evenly distributed BVH
    std::random_shuffle( objectsList.begin(), objectsList.end() );

    //find lowest point and highest bound of all objects
    Vector lowest = objectsList[0]->loCorner();
    Vector highest = objectsList[0]->hiCorner();
    for (size_t i = 1; i < objectsList.size(); i++) {
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

    std::cout << "Highest point in object is: " << highest.toString() << std::endl;
    std::cout << "Lowest point in object is: " << lowest.toString() << std::endl;

    //nonBVH objCollection = nonBVH(objectsList, &lowest, &highest);
    BVH objCollection = BVH(objectsList, &lowest, &highest);

    double bvhTime = (float)clock()/CLOCKS_PER_SEC;
    std::cout << "Building BVH tree took "  << bvhTime - startTime << " seconds" << std::endl;

    Vector camPoint = Vector(-5, 6.5, 5);
    Vector camVect = Vector(1, -1, -1).normalize();

    png::image<png::rgb_pixel> picture = png::image<png::rgb_pixel>(pngWidth, pngHeight);


    for (int c = 0; c < pngWidth; c++) {
        for (int r = 0; r < pngHeight; r++) {
            //Setting the location of each pixel, its point, and vector.
            Vector horizUnit = camVect.getOrthoNormRight().normalize().scalar(PIX_SIZE);
            Vector vertUnit = horizUnit.cross(&camVect).normalize().scalar(PIX_SIZE);


            Vector pixelLoc = camPoint + horizUnit.scalar(-c + pngWidth/2 + 0.5) + vertUnit.scalar(-r + pngHeight/2 + 0.5);

            int allReds = 0;
            int allGreens = 0;
            int allBlues = 0;

            for (int vectNum = 0; vectNum < AA_COEFF; vectNum++) {
                Vector yJitter;
                Vector zJitter;
                if (AA_COEFF == 1) {
                    yJitter = Vector(0, 0, 0);
                    zJitter = Vector(0, 0, 0);
                } else {
                    yJitter = horizUnit.scalar((rand()%1000 - 500)/1000.0);
                    zJitter = vertUnit.scalar((rand()%1000 - 500)/1000.0);
                }
                Vector pixPoint = pixelLoc + yJitter + zJitter;
                Vector pixVect;

                if (ORTHOGRAPHIC) {
                    pixVect = Vector(camVect);
                } else {
                    Vector focalPoint = camPoint - camVect.normalize().scalar(FOCAL_DIST);
                    pixVect = (pixPoint-focalPoint).normalize();
                }

                //Finding the closest "collision", looking through every object
                double minT = nan("1");

                Hittable* closestObj = objCollection.intersect(&pixPoint, &pixVect);
                if (closestObj != NULL) {
                    minT = closestObj->collideT(&pixPoint, &pixVect);

                    //set the pixel color to the color of the object
                    png::color phongColor = phongModel(&pixPoint, &pixVect, minT, &LIGHT_DIRECTION, closestObj);
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

  picture.write(outputFileName);

  double endTime = (double)clock()/CLOCKS_PER_SEC;
  std::cout << "Finished rendering, took " << endTime << " seconds." << std::endl;
}
