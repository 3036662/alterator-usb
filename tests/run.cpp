#include "test.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "Tests..." << std::endl;
  Test test;
  test.Run1();

  // test2 passes only if usbguard is active and service is enabled
  test.Run2();
  // test3 passes only if usbguard is installed
  test.Run3();
  // test4 passes only if usbguard is installed
  // needs root
  test.Run4();
  // test5 passes only if usbguard is installed
  // needs root
  test.Run5();
  // test6 passes only if usbguard is installed and config is default
  // needs root
  test.Run6();
  // test 7 test string  	with-interface { 0e:01:00 0e:02:00 0e:02:00
  // 0e:02:00 0e:02:00 0e:02:00 0e:02:00 0e:02:00 0e:02:00 } folding t0
  // ["0e:*:*"] vector
  test.Run7();
  return 0;
}