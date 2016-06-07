TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    map_2d.cpp \
    occupancy_grid_map.cpp \
    algorithm_d_star.cpp \
    robot_navigation.cpp \
    monte_carlo_option1.cpp \
    tree_node.cpp \
    tree_class.cpp \
    monte_carlo_branch_data.cpp \
    monte_carlo_node_data.cpp \
    d_star_map.cpp \
    png_file.cpp \
    png_convert_image_to_ogm.cpp

HEADERS += \
    map_2d.h \
    pos_2d.h \
    map_2d_templates.h \
    occupancy_grid_map.h \
    standard_definitions.h \
    algorithm_d_star.h \
    algorithm_d_star_templates.h \
    robot_navigation.h \
    monte_carlo_option1.h \
    tree_node.h \
    tree_class.h \
    monte_carlo_branch_data.h \
    monte_carlo_definitions.h \
    monte_carlo_node_data.h \
    d_star_map.h \
    tree_node_templates.h \
    d_star_map_templates.h \
    tree_class_templates.h \
    png_file.h \
    png_convert_image_to_ogm.h

unix:!macx: LIBS += -lpng
