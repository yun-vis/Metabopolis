cmake_minimum_required(VERSION 3.4)
PROJECT (METABOPOLIS)

# using Qt meta-system (precompiler)
# Tell CMake to run moc when necessary:
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-deprecated -DIL_STD")

# compiler setting
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else()
    set(CMAKE_CXX_STANDARD 11)
endif()

if (UNIX AND NOT APPLE)

  message(status "Setting G++ flags")

  set(CPLEX_INCLUDE "/opt/ibm/ILOG/CPLEX_Studio128/cplex/include/")
  set(CONCERT_INCLUDE "/opt/ibm/ILOG/CPLEX_Studio128/concert/include/")
  set(CPLEX_LIB "/opt/ibm/ILOG/CPLEX_Studio128/cplex/lib/x86-64_linux/static_pic/")
  set(CONCERT_LIB "/opt/ibm/ILOG/CPLEX_Studio128/concert/lib/x86-64_linux/static_pic/")
  set( GRAPHVIZ_PATH_DIR "/home/$ENV{USER}/MyLib/release/graphviz")
  set( QT5_PATH_DIR "/home/$ENV{USER}/MyLib/release/qt5-package/5.8/gcc_64")

  # add the libraries you need below
  set( OPENGL_FLAG -lGL -lGLU -lglut )
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  find_package(CGAL QUIET COMPONENTS Core REQUIRED)

  set( CXX_FLAGS "${CXX_FLAGS}-ltinyxml -lsbml -lcgraph -lgvc -lCGAL -lgmp -lmpfr -lboost_thread -lboost_system -lilocplex -lconcert -lcplex -ldl -lpthread ${OPENGL_FLAG}")

elseif(APPLE)

  message(status "Setting CLANG flags")
  set(CPLEX_INCLUDE "/Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/cplex/include/")
  set(CONCERT_INCLUDE "/Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/concert/include/")
  set(CPLEX_LIB "/Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/cplex/lib/x86-64_osx/static_pic/")
  set(CONCERT_LIB "/Users/$ENV{USER}/Applications/IBM/ILOG/CPLEX_Studio128/concert/lib/x86-64_osx/static_pic/")

  # add the libraries you need below
  set( OPENGL_FLAG "-framework OpenGL" )
  find_package(GLUT REQUIRED)
  find_package(OpenGL REQUIRED)

  set(CXX_FLAGS "${CXX_FLAGS}-ltinyxml -lsbml -lcgraph -lgvc -lCGAL -lgmp -lmpfr -lilocplex -lconcert -lcplex ${OPENGL_FLAG}")

endif()

include_directories(
    /opt/local/include
    ${CMAKE_SOURCE_DIR}/include
    ${CPLEX_INCLUDE}
    ${CONCERT_INCLUDE}
    ${GRAPHVIZ_PATH_DIR}/include
    ${QT5_PATH_DIR}/include
)

link_directories(
    /opt/local/lib
    ${CMAKE_SOURCE_DIR}/src
    ${CPLEX_LIB}
    ${CONCERT_LIB}
    ${GRAPHVIZ_PATH_DIR}/lib
    ${QT5_PATH_DIR}/lib
)

# automatically grab all the source files
set(UI_FILES include/DiaLayout.ui)
#set(RESOURCES *.qrc)

set(HEADERS
    include/boost/create_dual_graph.hpp
    include/graph/BaseEdgeProperty.h
    include/graph/BaseGraph.h
    include/graph/BaseGraphProperty.h
    include/graph/BaseVertexProperty.h
    include/graph/FlowEdgeProperty.h
    include/graph/FlowGraph.h
    include/graph/FlowVertexProperty.h
    include/graph/MetaboliteEdgeProperty.h
    include/graph/MetaboliteGraph.h
    include/graph/MetaboliteVertexProperty.h
    include/graph/PropertyGraph.h
    include/graph/RelationEdgeProperty.h
    include/graph/RelationGraph.h
    include/graph/RelationVertexProperty.h
    include/graph/RoadEdgeProperty.h
    include/graph/RoadGraph.h
    include/graph/RoadVertexProperty.h
    include/hola/Hola.h
    include/hola/TreeMap.h
    include/optimization/Deformation.h
    include/optimization/Floorplan.h
    include/optimization/Flow.h
    include/optimization/Layout.h
    include/optimization/MIP.h
    include/optimization/QMIP.h
    include/render/GraphicsArrowItem.h
    include/render/GraphicsBallItem.h
    include/render/GraphicsBase.h
    include/render/GraphicsBaseItem.h
    include/render/GraphicsCurveItem.h
    include/render/GraphicsDomainItem.h
    include/render/GraphicsEdgeItem.h
    include/render/GraphicsFlowItem.h
    include/render/GraphicsMetaboliteItem.h
    include/render/GraphicsNameItem.h
    include/render/GraphicsReactionItem.h
    include/render/GraphicsRouterItem.h
    include/render/GraphicsTreemapItem.h
    include/render/GraphicsTreemapNameItem.h
    include/render/GraphicsVertexItem.h
    include/render/GraphicsView.h
    include/render/Widget.h
    include/render/Window.h
    include/xml/bionetsbgnml.h
    include/xml/bionetsbml.h
    include/Common.h
    include/CommonClass.h
    include/ConfigSpace.h
    include/Coord2.h
    include/DiaLayout.h
    include/GraphVizAPI.h
    include/Line2.h
    include/Pathway.h
    include/PathwayData.h
    include/Rectangle2.h
)

set(SOURCES
    src/graph/BaseEdgeProperty.cpp
    src/graph/BaseGraph.cpp
    src/graph/BaseGraphProperty.cpp
    src/graph/BaseVertexProperty.cpp
    src/graph/FlowEdgeProperty.cpp
    src/graph/FlowGraph.cpp
    src/graph/FlowVertexProperty.cpp
    src/graph/MetaboliteEdgeProperty.cpp
    src/graph/MetaboliteGraph.cpp
    src/graph/MetaboliteVertexProperty.cpp
    src/graph/RelationEdgeProperty.cpp
    src/graph/RelationGraph.cpp
    src/graph/RelationVertexProperty.cpp
    src/graph/RoadEdgeProperty.cpp
    src/graph/RoadGraph.cpp
    src/graph/RoadVertexProperty.cpp
    src/hola/Hola.cpp
    src/hola/TreeMap.cpp
    src/optimization/Deformation.cpp
    src/optimization/Floorplan.cpp
    src/optimization/Flow.cpp
    src/optimization/Layout.cpp
    src/optimization/MIP.cpp
    src/optimization/QMIP.cpp
    src/render/GraphicsArrowItem.cpp
    src/render/GraphicsBallItem.cpp
    src/render/GraphicsBaseItem.cpp
    src/render/GraphicsCurveItem.cpp
    src/render/GraphicsDomainItem.cpp
    src/render/GraphicsEdgeItem.cpp
    src/render/GraphicsFlowItem.cpp
    src/render/GraphicsMetaboliteItem.cpp
    src/render/GraphicsNameItem.cpp
    src/render/GraphicsReactionItem.cpp
    src/render/GraphicsRouterItem.cpp
    src/render/GraphicsTreemapItem.cpp
    src/render/GraphicsTreemapNameItem.cpp
    src/render/GraphicsVertexItem.cpp
    src/render/GraphicsView.cpp
    src/render/Widget.cpp
    src/render/Window.cpp
    src/xml/bionetsbgnml.cpp
    src/xml/bionetsbml.cpp
    src/Common.cpp
    src/ConfigSpace.cpp
    src/Coord2.cpp
    src/DiaLayout.cpp
    src/GraphVizAPI.cpp
    src/Line2.cpp
    src/Pathway.cpp
    src/PathwayData.cpp
    src/Rectangle2.cpp
    src/main.cpp
    )

set( DEFINEQT5 ON )
find_package(Qt5 COMPONENTS Core Gui Svg Widgets REQUIRED)

# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )
QT5_WRAP_CPP( MOC_SRCS ${UI_HEADERS})
QT5_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
QT5_WRAP_CPP( MOC_DIALAYOUT ${CMAKE_CURRENT_SOURCE_DIR}/include/DiaLayout.h )
QT5_WRAP_CPP( MOC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/include/render/Window.h )
QT5_WRAP_CPP( MOC_WIDGET ${CMAKE_CURRENT_SOURCE_DIR}/include/render/Widget.h )
QT5_WRAP_CPP( MOC_GRAPHICSVIEW ${CMAKE_CURRENT_SOURCE_DIR}/include/render/GraphicsView.h )

set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/moc_ui_DiaLayout.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/moc_DiaLayout.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_Window.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_Widget.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_GraphicsView.cpp PROPERTY SKIP_AUTOGEN ON )
set_property( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/include/render/moc_GraphicsVertexItem.cpp PROPERTY SKIP_AUTOGEN ON )

add_executable (metabopolis ${SOURCES} ${MOC_SRCS} ${MOC_DIALAYOUT} ${MOC_WINDOW} ${MOC_WIDGET} ${MOC_GRAPHICSVIEW})
target_link_libraries (metabopolis ${CXX_FLAGS})

# qt stuff
qt5_use_modules(metabopolis Core Gui Svg Widgets)
