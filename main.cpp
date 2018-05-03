#include "Renderer.h"

Vector getVertex(const std::string& line) {
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

void test() {
    std::vector<Vector> vertexes;

    std::ofstream myfile;
    myfile.open ("reWritten.txt");
    std::fstream inFile;
    inFile.open("bunny.obj");

    std::string line;
    if (inFile.is_open()) {
          while (getline(inFile, line)) {
            if (line.substr(0, 1) == "v") {
              Vector vertex = getVertex(line);
              myfile << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
            } else if (line.substr(0, 1) == "f") {
              Vector points = getVertex(line);
              myfile << "f " << (int)points[0] << " " << (int)points[1] << " " << (int)points[2] << "\n";
            }
          }
      }
  myfile.close();

    inFile.close();
}

int main() {
    Renderer r;
    r.run();

  //std::string s = "v -0.167078 -8.13302e-005 -0.576475";
  //std::cout << r.getVertex(s).toString() << std::endl;
  //test();
  return 0;
}
