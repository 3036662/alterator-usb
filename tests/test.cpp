#include "guard.hpp"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

void Test::Run1() {
  std::cout << "Test udev rules files search" << std::endl;
  // guard::Guard guard_obj;
  std::string curr_path = std::filesystem::current_path().string();

  // create files
  const std::string file1 = curr_path + "/rule1.rules";
  const std::string file2 = curr_path + "/rule2.rules";
  const std::string file3 = curr_path + "/rule3.rules";
  const std::string file4 = curr_path + "/rule4.rules";
  const std::string file5 = curr_path + "/rule5.rules";
  const std::string file6 = curr_path + "/rule6.rules";

  // usb + authorized
  std::ofstream os(file1);
  if (os.is_open())
    os << "blalala usb blabla authorized" << std::endl;
  os.close();

  // not usb neither authorized
  os = std::ofstream(file2);
  if (os.is_open())
    os << "bla bla \n bla bla" << std::endl;
  os.close();

  // simple long file
  os = std::ofstream(file3);
  if (os.is_open()) {
    for (int i = 0; i < 2048; ++i)
      os << i;
  }
  os.close();

  // only usb
  os = std::ofstream(file4);
  if (os.is_open())
    os << "bla usb \n bla bla" << std::endl;
  os.close();

  // CASE different cases
  os = std::ofstream(file5);
  if (os.is_open())
    os << "bla uSB \n bla bla AutHorizeD"
       << "" << std::endl;
  os.close();

  // CASE different
  os = std::ofstream(file6);
  if (os.is_open())
    os << "bla uS \n bla bla AutHorizeD"
       << "" << std::endl;
  os.close();

  // check
  std::vector<std::string> vec_mock{
      curr_path,
      "sdgzg098gav\\c" // bad path
  };
  const std::unordered_map<std::string, std::string> map =
      guard::InspectUdevRules(&vec_mock);
  const std::unordered_map<std::string, std::string> expected_map{
      std::pair<std::string, std::string>{file1, "usb_rule"},
      std::pair<std::string, std::string>{file5, "usb_rule"}};
  assert(map == expected_map);
  std::cout << "TEST1 ... OK" << std::endl;
}

void Test::Run2() {
  guard::ConfigStatus cs;
  cs.CheckDaemon();
  std::cout << " ACTIVE " << cs.guard_daemon_active << " ENABLED "
            << cs.guard_daemon_enabled << " DAEMON_OK " << cs.guard_daemon_OK
            << " UDEV_RULES_OK " << cs.udev_rules_OK << std::endl;
  assert(cs.guard_daemon_active == true);
  assert(cs.guard_daemon_enabled == true);
  std::cout << "TEST2 ... OK" << std::endl;
}

void Test::Run3() {
  guard::ConfigStatus cs;
  std::string res = cs.GetDamonConfigPath();
  std::cerr << "Config path is " << res << std::endl;
  assert(std::filesystem::exists(res));
  std::cout << "TEST3 ... OK" << std::endl;
}
