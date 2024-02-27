#pragma once
#include "serializible_for_lisp.hpp"
#include <boost/json.hpp>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#ifdef UNIT_TEST
#include "test.hpp"
#endif

namespace guard {

// clang-format off

/// @brief Target ::= allow | block | reject
enum class Target { allow, block, reject };

/**
 * @brief Operators used in usbguard rules
 *
 */
enum class RuleOperator {
  all_of,  // Evaluate to true if all of the specified conditions evaluated to true.
  one_of,  // Evaluate to true if one of the specified conditions evaluated to true.
  none_of, // Evaluate to true if none of the specified conditions evaluated to true.
  equals,  //  Same as all-of.
  equals_ordered, //  Same as all-of.
  no_operator     // if no operators are used
};

/**
 * @brief Conditions used in usbguard rules
 *
 */
enum class RuleConditions {
  localtime,    //  Evaluates to true if the local time is in the specified time range.
  allowed_matches, // Evaluates to true if an allowed device matches the specified query.
  rule_applied, // Evaluates to true if the rule currently being evaluated ever matched a device.
  rule_applied_past, // Evaluates to true if the rule currently being evaluatedmatched a device in the past duration of time specified by the parameter.
  rule_evaluated, // Evaluates to true if the rule currently being evaluated was ever evaluated before.
  rule_evaluated_past, // Evaluates to true if the rule currently being evaluated was evaluated in the pas duration of timespecified by the parameter.
  random,       // Evaluates to true/false with a probability of p=0.5
  random_with_propability, // Evaluates to true with the specified probability p_true.
  always_true,  //  Evaluates always to true.
  always_false, // Evaluates always to false
  no_condition
};

/**
 * @brief Strictness levels 
 * 
 */
enum class StrictnessLevel{
  hash, // the most strict
  vid_pid, 
  interface,
  non_strict
};

// clang-format on

using RuleWithOptionalParam =
    std::pair<RuleConditions, std::optional<std::string>>;
using RuleWithBool = std::pair<bool, RuleWithOptionalParam>;

/**
 * @brief Parse and store rules from usbguard rules.conf file
 * @class GuardRule
 * @throws Constructor - std::logical_error
 */
class GuardRule : public SerializableForLisp<GuardRule> {

  /// @brief Maps Target enum to a string.
  static const std::map<Target, std::string> map_target;
  /// @brief Maps RuleConditions enum to a string.
  static const std::map<RuleConditions, std::string> map_conditions;
  /// @brief Maps RuleOperator enum to a string.
  static const std::map<RuleOperator, std::string> map_operator;

public:
  uint number = 0; ///@brief number of line (from the beginnig of file)
  Target target;
  std::optional<std::string> vid;
  std::optional<std::string> pid;
  std::optional<std::string> hash;
  std::optional<std::string> parent_hash;
  std::optional<std::string> device_name;
  std::optional<std::string> serial;
  std::optional<std::pair<RuleOperator, std::vector<std::string>>> port;
  std::optional<std::pair<RuleOperator, std::vector<std::string>>>
      with_interface;
  std::optional<std::string> conn_type;
  std::optional<std::pair<RuleOperator, std::vector<RuleWithBool>>> cond;
  StrictnessLevel level = StrictnessLevel::hash;
  std::optional<std::string> vendor_name;

  /**
   * @brief Construct a new Guard Rule object
   * @param str String rule from usbguard rules file
   * @throws std::logical_error
   */
  explicit GuardRule(const std::string &raw_str);

  GuardRule &operator=(const GuardRule &) noexcept = default;
  GuardRule &operator=(GuardRule &&) noexcept = default;
  GuardRule(GuardRule &&) noexcept = default;
  GuardRule(const GuardRule &) noexcept = default;

  /**
   * @brief Build rule string for usbguard
   *
   * @param build_parent_hash include parent hash to rule string
   * @param false skip operator "equals" for an array of interface
   * @return std::string
   * @details The parameters are used to build the same rule as usbguard cli
   * builds
   */
  std::string
  BuildString(bool build_parent_hash = true,
              bool with_interface_array_no_operator = true) const noexcept;

  vecPairs SerializeForLisp() const;

  /// @brief Converts a string representation of Rule StricnessLevel to a
  /// StrictnessLevel
  /// @param str Stricness level ("hash","vid_pid","interface");
  /// @return StrictnessLevel - non_strict if no corresondent level is found.
  static StrictnessLevel StrToStrictnessLevel(const std::string &str) noexcept;

private:
  static bool VidPidValidator(const std::string &val);
  static bool InterfaceValidator(const std::string &val);

  /**
   * @brief Parse conditions
   *
   * @param splitted Vector - a splitted rule string.
   * @return std::optional<
   * std::pair<RuleOperator, std::vector<std::pair<RuleConditions, bool>>>>
   */
  static std::optional<std::pair<RuleOperator, std::vector<RuleWithBool>>>
  ParseConditions(std::vector<std::string> &splitted);

  /**
   * @brief Parses one condition with or without params
   *
   * @param it_range_beg An iterator, pointing to a token where to start.
   * @param it_range_end An iterator, pointing to the end of token sequence.
   * @return !Rule -> <false,Rule> | Rule -> <true,Rule>
   * @warning Changes a velue of begin iterator.
   * A way to inform the calling function how many tokens were read during
   * a parse process.
   */
  static RuleWithBool
  ParseOneCondition(std::vector<std::string>::const_iterator &it_range_beg,
                    std::vector<std::string>::const_iterator it_range_end);

  /**
   * @brief Build a condition string from this object.
   *
   * @return std::string
   */
  std::string ConditionsToString() const;

  std::string PortsToString() const;
  std::string
  InterfacesToString(bool with_interface_array_no_operator = true) const;

  /*--------------------- static -------------------------*/

  /**
   * @brief Checks if sring looks like one of parameter reserve words for rule
   *
   * @param str parameter
   * @return true if string is normal value
   * @return false if string looks like reserved word
   */
  static bool IsReservedWord(const std::string &str) noexcept;

  /**
   * @brief Split string by space, dont split double-quouted substrings
   *
   * @param raw_str String to split
   * @return std::vector<std::string>
   * @example "id    1    name   "Long Name"" -> { "id","1",""Long Name""}
   */
  static std::vector<std::string> SplitRawRule(std::string raw_str) noexcept;
  /// @brief  // Inline wrap all curly and round braces and exclamation points
  /// with spaces
  static void WrapBracesWithSpaces(std::string &str);

  /**
   * @brief Parse token
   *
   * @param splitted vector - a splitted rule string.
   * @param name String name of a parameter to look for.
   * @param predicat bool(sting&) function returning true, if string is valid
   * value
   * @return std::optional<std::string> value for parameter
   */
  static std::optional<std::string>
  ParseToken(std::vector<std::string> &splitted, const std::string &name,
             const std::function<bool(const std::string &)> &predicat);

  /**
   * @brief Parse token when operators are possible for token
   *
   * @param splitted vector - a splitted rule string.
   * @param name String name of a parameter to look for.
   * @param predicat bool(sting&) function returning true, if string is valid
   * value
   * @return std::optional<std::pair<RuleOperator, std::vector<std::string>>>
   * value for parameter
   */
  static std::optional<std::pair<RuleOperator, std::vector<std::string>>>
  ParseTokenWithOperator(
      std::vector<std::string> &splitted, const std::string &name,
      const std::function<bool(const std::string &)> &predicat);

  /**
   * @brief Parses a condition parameter
   * A parameter is expected to be enclosed in round braces.
   *
   * @param it_start An iterator, pointing to a token where to start. "("
   * @param it_end  An iterator, pointing to the end of token sequence.
   * @return A string value of parameter.
   */
  static std::string
  ParseConditionParameter(std::vector<std::string>::const_iterator it_start,
                          std::vector<std::string>::const_iterator it_end,
                          bool must_have_params = false);
  /**
   * @brief Parses array {val1 val2 ...}
   *
   * @param it_range_begin Iterator to last token befor array
   * @param it_end Iterator to end bound of sequence
   * @param predicat Validator for a value
   * @param res_array Array where values must be appended
   * @return An iterator to the end of an array aka "}" token
   */
  static std::vector<std::string>::const_iterator ParseCurlyBracesArray(
      std::vector<std::string>::const_iterator it_range_begin,
      std::vector<std::string>::const_iterator it_end,
      const std::function<bool(const std::string &)> &predicat,
      std::vector<std::string> &res_array);

  /**
   * @brief Checks if a condition is allowed to have parameters.
   *
   * @param cond RuleConditions
   * @return true
   * @return false
   */
  static bool CanConditionHaveParams(RuleConditions cond);

  static bool MustConditionHaveParams(RuleConditions cond);
  /**
   * @brief Choose an appropriate emun for RuleConditions
   * in case it has parameters.
   * @param cond
   * @return RuleConditions
   */
  static RuleConditions ConvertToConditionWithParam(RuleConditions cond);

#ifdef UNIT_TEST
  friend class ::Test;
#endif
};

} // namespace guard