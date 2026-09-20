cmake_minimum_required(VERSION 3.8)

project(control_asv)



if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

  add_compile_options(-Wall -Wextra -Wpedantic)

endif()



# find dependencies

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)

find_package(std_msgs REQUIRED)

find_package(sensor_msgs REQUIRED)   # para nodo_ap_dds_joy (sensor_msgs::msg::Joy)

find_package(mavros_msgs REQUIRED)   #para nodo_mavros_override (mavros_msgs::msg::OverrideRCIn)



if(BUILD_TESTING)

  find_package(ament_lint_auto REQUIRED)

  # the following line skips the linter which checks for copyrights

  # comment the line when a copyright and license is added to all source files

  set(ament_cmake_copyright_FOUND TRUE)

  # the following line skips cpplint (only works in a git repo)

  # comment the line when this package is in a git repo and when

  # a copyright and license is added to all source files

  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()

endif()


#Para el nodo principal (no tiene nada por ahora)
 add_executable(nodo_principal src/nodo_principal.cpp)

ament_target_dependencies(nodo_principal rclcpp std_msgs)



#nodo MAVROS (PWM crudo)

#add_executable(nodo_mavros_override src/nodo_mavros_override.cpp)

#ament_target_dependencies(nodo_mavros_override rclcpp mavros_msgs)



#nodo AP_DDS (traduce PWM -> eje normalizado antes de publicar)

#add_executable(nodo_ap_dds_joy src/nodo_ap_dds_joy.cpp)

#ament_target_dependencies(nodo_ap_dds_joy rclcpp sensor_msgs)



install(TARGETS

 # nodo_principal

  #nodo_mavros_override

  #nodo_ap_dds_joy

  DESTINATION lib/${PROJECT_NAME}

)

ament_package()