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
    png_convert_image_to_ogm.cpp \
    ogm_divider.cpp \
    graph_class.cpp \
    graph_vertice.cpp \
    graph_edge.cpp \
    graph_node.cpp \
    test_functions.cpp \
    test_map_2d.cpp \
    policy_tree.cpp \
    robot_action.cpp \
    policy_monte_carlo_converter.cpp \
    test_policy.cpp \
	algorithm_voronoi_fields.cpp \
    district_map.cpp \
    district_map_storage.cpp \
    district_tree.cpp \
    district_graph.cpp

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
    png_convert_image_to_ogm.h \
    ogm_divider.h \
    graph_class.h \
    graph_vertice.h \
    graph_definitions.h \
    graph_edge.h \
    graph_node.h \
    test_functions.h \
    test_map_2d.h \
    policy_tree.h \
    robot_action.h \
    policy_monte_carlo_converter.h \
    test_policy.h \
    algorithm_voronoi_fields.h \
    district_map.h \
    district_map_storage.h \
    district_tree.h \
    district_graph.h \
    district_map_definitions.h

unix:!macx: LIBS += -lpng
