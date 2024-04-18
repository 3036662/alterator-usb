#include "usb_mount.hpp"
#include "active_device.hpp"
#include "log.hpp"
#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/object.hpp>
#include <exception>
#include <sdbus-c++/sdbus-c++.h>
#include <stdexcept>
#include <string>

namespace alterator::usbmount {
using common_utils::Log;

UsbMount::UsbMount() noexcept : dbus_proxy_(nullptr) {
  try {
    dbus_proxy_ = sdbus::createProxy(kDest, kObjectPath);
  } catch (const std::exception &ex) {
    Log::Warning() << "[Usbmount] Can't create dbus proxy ";
  }
}

std::vector<ActiveDevice> UsbMount::ListDevices() const noexcept {
  namespace json = boost::json;
  std::vector<ActiveDevice> res;
  if (!dbus_proxy_)
    return res;
  auto method = dbus_proxy_->createMethodCall(kInterfaceName, "ListDevices");
  auto reply = dbus_proxy_->callMethod(method);
  std::string json_string;
  reply >> json_string;
  try {
    auto value = json::parse(json_string);
    const json::array &arr = value.as_array();
    size_t counter = 1;
    for (const auto &device : arr) {
      if (!device.is_object())
        throw std::runtime_error("Can't parse a Json strinh");
      res.emplace_back(device.as_object());
      res.back().index = counter;
      ++counter;
    }
  } catch (const std::exception &ex) {
    Log::Error() << "Error parsing the daemon response";
    Log::Error() << ex.what();
  }
  // Log::Debug() << "[ListDevices]" << json_string;
  return res;
}

} // namespace alterator::usbmount