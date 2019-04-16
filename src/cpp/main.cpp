#include "cpp/main.h"

int main() {

  std::cout << "\nHello World\n";

  int x = testGSL();
  std::cout << "Returned " << x;

  int y = testMDSplus();
  std::cout << "Returned " << y;

  return 0;

}
