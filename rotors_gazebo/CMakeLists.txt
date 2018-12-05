cmake_minimum_required(VERSION 2.8.3)
project(rotors_gazebo)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON) #...with compiler extensions like gnu++11

add_definitions(-std=c++11)

#--------------------------#
# Generation of SDF models #
#--------------------------#

# We need Gazebo version >= 3.0.0 to generate iris.sdf file
# (gz sdf ... command needs to be available)
find_package(gazebo REQUIRED)
find_package(OpenCV REQUIRED)
find_package(aruco REQUIRED)

if(${gazebo_VERSION_MAJOR} GREATER 2)
    message(STATUS "Building iris.sdf.")
    
    set(enable_mavlink_interface "true")
    set(enable_ground_truth "false")
    set(enable_logging "false")
    set(enable_camera "false")
    set(enable_wind "false")
    set(rotors_description_dir "${CMAKE_CURRENT_SOURCE_DIR}/models/rotors_description")
    set(scripts_dir "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

    	set(sources
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/cameraparameters.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/cvdrawingutils.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/dictionary.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/ippe.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/marker.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerdetector.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabeler.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markermap.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/posetracker.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabelers/dictionary_based.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/debug.cpp
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabelers/svmmarkers.cpp)

    	set(headers
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/aruco_export.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/cameraparameters.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/cvdrawingutils.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/dictionary.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/ippe.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/marker.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerdetector.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabeler.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markermap.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/posetracker.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabelers/dictionary_based.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/timers.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/debug.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/aruco.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/markerlabelers/svmmarkers.h)

    add_library(aruco ${sources} ${headers})
    
    # Creates shell commands to generate .sdf file
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/models/iris/iris.sdf
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/models/iris/iris.sdf
      COMMAND python ${scripts_dir}/xacro.py -o  ${rotors_description_dir}/urdf/iris_base.urdf  ${rotors_description_dir}/urdf/iris_base.xacro enable_mavlink_interface:=${enable_mavlink_interface} enable_ground_truth:=${enable_ground_truth} enable_wind:=${enable_wind} enable_logging:=${enable_logging} rotors_description_dir:=${rotors_description_dir}
      COMMAND gz sdf -p  ${rotors_description_dir}/urdf/iris_base.urdf >> ${CMAKE_CURRENT_SOURCE_DIR}/models/iris/iris.sdf
      COMMAND rm -f ${rotors_description_dir}/urdf/iris_base.urdf
      DEPENDS ${rotors_description_dir}/urdf/iris.xacro
      DEPENDS ${rotors_description_dir}/urdf/iris_base.xacro
      DEPENDS ${rotors_description_dir}/urdf/component_snippets.xacro
      )
    add_custom_target(sdf ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/models/iris/iris.sdf)
else()
  message(STATUS "Gazebo version is less than 3, cannot generate iris.sdf (gz tool not available).")
endif()

if(NO_ROS)
  message(STATUS "NO_ROS = true, not building waypoint publisher or hovering example, returning now.")
  return()
endif()

find_package(catkin REQUIRED COMPONENTS 
  gazebo_msgs 
  geometry_msgs 
  mav_msgs 
  roscpp 
  sensor_msgs
  image_transport 
  cv_bridge
  )

catkin_package(
  CATKIN_DEPENDS
    gazebo_msgs
    geometry_msgs
    mav_msgs
    roscpp
    sensor_msgs
       
)

include_directories(
  include ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCUDE_DIRS}
  )



add_executable(waypoint_publisher src/waypoint_publisher.cpp)
target_link_libraries(waypoint_publisher ${catkin_LIBRARIES})
add_dependencies(waypoint_publisher ${catkin_EXPORTED_TARGETS})

add_executable(waypoint_publisher_file src/waypoint_publisher_file.cpp)
target_link_libraries(waypoint_publisher_file ${catkin_LIBRARIES})
add_dependencies(waypoint_publisher_file ${catkin_EXPORTED_TARGETS})

add_executable(hovering_example src/hovering_example.cpp)
target_link_libraries(hovering_example ${catkin_LIBRARIES})
add_dependencies(hovering_example ${catkin_EXPORTED_TARGETS})

add_executable(image_publisher src/image_publisher.cpp)
target_link_libraries(image_publisher ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(image_publisher ${catkin_EXPORTED_TARGETS})

foreach(dir launch models resource worlds)
   install(DIRECTORY ${dir}/
      DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${dir})
endforeach(dir)

install(TARGETS waypoint_publisher waypoint_publisher_file hovering_example
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
