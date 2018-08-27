cmake_minimum_required(VERSION 3.4)
PROJECT (BIONET)

# using Qt meta-system (precompiler)
# Tell CMake to run moc when necessary:
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (UNIX AND NOT APPLE)
  message(status "Setting G++ flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -Wall -Wno-deprecated -DIL_STD -std=gnu++11 -frounding-math -Wunused-variable")
  include_directories(
      /usr/include
      /usr/X11R6/include
  	  /opt/local/include
      /opt/ibm/ILOG/CPLEX_Studio128/cplex/include
      /opt/ibm/ILOG/CPLEX_Studio128/concert/include
      ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/src
      /home/$ENV{USER}/MyLib/release/sbml/include
      /home/$ENV{USER}/MyLib/release/graphviz/include
#      /home/$ENV{USER}/MyLib/release/qt5/qtbase/include
      /home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64/include
#      /home/$ENV{USER}/Desktop/gitroot/others/ogdf/build/include
#      /home/$ENV{USER}/Desktop/gitroot/others/ogdf/include
  )
  link_directories(
      /usr/X11R6/lib
      /opt/local/lib
      /usr/lib/x86_64-linux-gnu
      /home/$ENV{USER}/MyLib/release/sbml/lib
      /home/$ENV{USER}/MyLib/release/graphviz/lib
#      /home/$ENV{USER}/MyLib/release/qt5/qtbase/lib
      /home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64/lib
#      /home/$ENV{USER}/Desktop/gitroot/others/ogdf/build
      /opt/ibm/ILOG/CPLEX_Studio128/cplex/lib/x86-64_linux/static_pic
      /opt/ibm/ILOG/CPLEX_Studio128/concert/lib/x86-64_linux/static_pic
  )
  # add the libraries you need below
  set( OPENGL_FLAG GL GLU glut )
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(CGAL QUIET COMPONENTS Core REQUIRED)
elseif(APPLE)
  message(status "Setting CLANG flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -DIL_STD -std=c++11")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -DIL_STD -std=c++11 -stdlib=libc++")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -DIL_STD -std=c++0x")
  include_directories(
      /opt/local/include
  	  /usr/local/include
  	  /usr/X11R6/include
	  ${CMAKE_SOURCE_DIR}/include
      ${CMAKE_SOURCE_DIR}/src
#      /Users/$ENV{USER}/Desktop/gitroot/others/ogdf/build/include
#      /Users/$ENV{USER}/Desktop/gitroot/others/ogdf/include
      /Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/cplex/include
      /Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/concert/include
  )
  link_directories(
      /usr/X11R6/lib
      /opt/local/lib
	  /Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/cplex/lib/x86-64_osx/static_pic
	  /Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/concert/lib/x86-64_osx/static_pic
#      /Users/$ENV{USER}/Desktop/gitroot/others/ogdf/build
  )
  # add the libraries you need below
  set( OPENGL_FLAG "-framework OpenGL" )
  find_package(GLUT REQUIRED)
  find_package(OpenGL REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# automatically grab all the source files
file(GLOB SOURCES src/*.cpp src/graph/*.cpp src/render/*.cpp src/optimization/*.cpp src/hola/*.cpp src/xml/*.cpp)
file(GLOB HEADERS include/*.h include/graph/*.h include/render/*.h include/optimization/*.h include/hola/*.h include/xml/*.h)
file(GLOB UI_FILES include/*.ui)
file(GLOB RESOURCES *.qrc)


set( DEFINEQT5 ON )
#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/yun/MyLib/release/qt5/qtbase/lib/cmake/Qt5" )
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/yun/MyLib/release/qt5-package/5.8/gcc_64/lib/cmake/Qt5" )
find_package(Qt5 COMPONENTS Core Gui Svg Widgets OpenGL REQUIRED)
# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )
QT5_WRAP_CPP( MOC_SRCS ${UI_HEADERS})
QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/DiaLayout.h )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/render/Window.h )
QT5_WRAP_CPP( MOC_WIDGET ${CMAKE_CURRENT_SOURCE_DIR}/include/render/Widget.h )
QT5_WRAP_CPP( MOC_WIDGET ${CMAKE_CURRENT_SOURCE_DIR}/include/render/GraphicsView.h )
QT5_WRAP_CPP( MOC_WIDGET ${CMAKE_CURRENT_SOURCE_DIR}/include/render/GraphicsVertexItem.h )

set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/moc_ui_DiaLayout.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/moc_DiaLayout.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_Widget.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_GraphicsView.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_GraphicsVertexItem.cpp PROPERTY SKIP_AUTOGEN ON )

#add_executable (bionet ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS} ${MOC_WINDOW} ${MOC_WIDGET})
add_executable (bionet ${SOURCES} ${MOC_SRCS} ${MOC_WINDOW} ${MOC_WIDGET})
if (UNIX AND NOT APPLE)
    target_link_libraries (bionet pthread tinyxml sbml cgraph gvc boost_thread boost_system CGAL gmp mpfr ${OPENGL_FLAG} ${QT_LIBRARIES} pthread dl ilocplex concert cplex boost_graph)
elseif (APPLE)
    target_link_libraries (bionet pthread tinyxml sbml cgraph gvc CGAL gmp mpfr ${OPENGL_FLAG} ${QT_LIBRARIES} ilocplex concert cplex)
endif()
# qt stuff
qt5_use_modules(bionet Core Gui Svg Widgets OpenGL)
