#pragma once

#include "guard.hpp"
#include "lispmessage.hpp"
#include <string>

/**
 * @class MessageDispatcher
 * @brief Accepts messages (LispMessage) from MessageReader and perfoms
 * appropriate actions
 */
class MessageDispatcher {
public:
  /**
   * @brief Constructor for Message Dispatcher
   * @param guard The Guard object
   */
  MessageDispatcher(guard::Guard &guard) noexcept;
  /**
   * @brief Perfom an appropriate action for msg
   * @param msg LispMessage from MessageReader
   */
  bool Dispatch(const LispMessage &msg) noexcept;

private:
  bool SaveChangeRules(const LispMessage &msg) const noexcept;
  bool ListUsbGuardRules(const LispMessage &msg) const noexcept;
  bool ListUsbDevices() const noexcept;
  bool AllowDevice(const LispMessage &msg) const noexcept;
  bool BlockDevice(const LispMessage &msg) const noexcept;
  bool CheckConfig() const noexcept;

  guard::Guard &guard;
  const std::string mess_beg = "(";
  const std::string mess_end = ")";
};
