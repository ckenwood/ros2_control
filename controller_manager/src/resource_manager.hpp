// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RESOURCE_MANAGER_HPP_
#define RESOURCE_MANAGER_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "controller_manager/loaned_command_interface.hpp"
#include "controller_manager/loaned_state_interface.hpp"

namespace hardware_interface
{
namespace components
{
class ActuatorInterface;
class SensorInterface;
class SystemInterface;
}  // namespace components
}  // namespace hardware_interface

namespace controller_manager
{

class ResourceStorage;

class ResourceManager
{
public:
  /// Constructor for the Resource Manager.
  /**
   * The implementation loads the specified urdf and initializes the
   * hardware components listed within as well as populate their respective
   * state and command interfaces.
   *
   * If the interfaces ought to be validated, the constructor throws an exception
   * in case the URDF lists interfaces which are not available.
   *
   * \param urdf string containing the URDF.
   * \param validate_interfaces boolean argument indicating whether the exported
   * interfaces ought to be validated. Defaults to true.
   */
  explicit ResourceManager(
    const std::string & urdf, bool validate_interfaces = true);

  ResourceManager(const ResourceManager &) = delete;

  ~ResourceManager();

  /// Claim a state interface given its key.
  /**
   * The resource is claimed as long as being in scope.
   * Once the resource is going out of scope, the destructor
   * returns.
   *
   * \param key String identifier which state interface to claim
   * \return state interface
   */
  LoanedStateInterface claim_state_interface(const std::string & key);

  /// Returns all registered state interfaces keys.
  /**
   * The keys are collected from each loaded hardware component.
   *
   * \return vector of strings, containing all registered keys.
   */
  std::vector<std::string> state_interface_keys() const;

  /// Checks whether a state interface is registered under the given key.
  /**
   * \return true if interface exist, false otherwise.
   */
  bool state_interface_exists(const std::string & key) const;

  /// Checks whether a command interface is already claimed.
  /**
   * Any command interface can only be claimed by a single instance.
   * \note the equivalent function does not exist for state interfaces.
   * These are solely read-only and can thus be used by multiple instances.
   *
   * \param key string identifying the interface to check.
   * \return true if interface is already claimed, false if available.
   */
  bool command_interface_is_claimed(const std::string & key) const;

  /// Claim a command interface given its key.
  /**
   * The resource is claimed as long as being in scope.
   * Once the resource is going out of scope, the destructor
   * returns and thus frees the resource to claimed by others.
   *
   * \param key String identifier which command interface to claim
   * \return command interface
   */
  LoanedCommandInterface claim_command_interface(const std::string & key);

  /// Returns all registered command interfaces keys.
  /**
   * The keys are collected from each loaded hardware component.
   *
   * \return vector of strings, containing all registered keys.
   */
  std::vector<std::string> command_interface_keys() const;

  /// Checks whether a command interface is registered under the given key.
  /**
   * \return true if interface exist, false otherwise.
   */
  bool command_interface_exists(const std::string & key) const;

  /// Return the number of loaded actuator components.
  /**
   * \return number of actuator components.
   */
  size_t actuator_components_size() const;

  /// Import a hardware component which is not listed in the URDF
  /**
   * Components which are initialized outside a URDF can be added post initialization.
   *
   * \note this might invalidate existing state and command interfaces and should thus
   * not be called when a controller is running.
   * \note given that no hardware_info is available, the component has to be configured
   * externally and prior to the call to import.
   * \param actuator pointer to the actuator interface.
   */
  void import_component(
    std::unique_ptr<hardware_interface::components::ActuatorInterface> actuator);

  /// Return the number of loaded sensor components.
  /**
   * \return number of sensor components.
   */
  size_t sensor_components_size() const;

  /// Import a hardware component which is not listed in the URDF
  /**
   * Components which are initialized outside a URDF can be added post initialization.
   *
   * \note this might invalidate existing state and command interfaces and should thus
   * not be called when a controller is running.
   * \note given that no hardware_info is available, the component has to be configured
   * externally and prior to the call to import.
   * \param sensor pointer to the sensor interface.
   */
  void import_component(std::unique_ptr<hardware_interface::components::SensorInterface> sensor);

  /// Return the number of loaded system components.
  /**
   * \return number of system components.
   */
  size_t system_components_size() const;

  /// Import a hardware component which is not listed in the URDF
  /**
   * Components which are initialized outside a URDF can be added post initialization.
   *
   * \note this might invalidate existing state and command interfaces and should thus
   * not be called when a controller is running.
   * \note given that no hardware_info is available, the component has to be configured
   * externally and prior to the call to import.
   * \param system pointer to the system interface.
   */
  void import_component(std::unique_ptr<hardware_interface::components::SystemInterface> system);

private:
  void release_command_interface(const std::string & key);

  // TODO(karsten1987): Optimize this with std::vector, maps are bad
  std::unordered_map<std::string, bool> claimed_command_interface_map_;

  mutable std::recursive_mutex resource_lock_;
  std::unique_ptr<ResourceStorage> resource_storage_;
};

}  // namespace controller_manager
#endif  // RESOURCE_MANAGER_HPP_
