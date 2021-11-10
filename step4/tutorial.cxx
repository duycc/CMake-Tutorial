// A simple program that computes the square root of a number
#include <cmath>
#include <iostream>

#include "TutorialConfig.h"

#ifdef USE_MYMATH
#include "MathFunctions.h"
#endif

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " number" << std::endl;

    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << std::endl;
    return 1;
  }
  // double inputVal = std::atof(argv[1]);
  const double inputVal = std::stod(argv[1]);

#ifdef USE_MYMATH
  const double outputVal = mysqrt(inputVal);
#else
  const double outputVal = std::sqrt(inputVal);
#endif

  std::cout << "The square root of " << inputVal << " is " << outputVal << std::endl;
  return 0;
}
