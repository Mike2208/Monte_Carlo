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
    district_graph.cpp \
    tree_class_permanent.cpp \
    tree_node_permanent.cpp \
    algorithm_voronoi_fields_template.cpp \
    pos_2d.cpp \
    standard_definitions.cpp \
    algorithm_d_star_templates.cpp \
    d_star_map_templates.cpp \
    graph_class_templates.cpp \
    graph_vertice_templates.cpp \
    map_2d_templates.cpp \
    tree_class_permanent_templates.cpp \
    tree_node_permanent_templates.cpp \
    tree_node_templates.cpp \
    tree_class_templates.cpp \
    monte_carlo_option2.cpp \
    robot_data.cpp \
    mc_path_storage.cpp \
    algorithm_a_star.cpp \
    algorithm_a_star_templates.cpp \
    district_map_storage_templates.cpp \
    file_tree_node.cpp \
    file_tree.cpp \
    file_tree_node_templates.cpp \
    file_tree_templates.cpp \
    file_tree_save_file.cpp

HEADERS += \
    map_2d.h \
    pos_2d.h \
    occupancy_grid_map.h \
    standard_definitions.h \
    algorithm_d_star.h \
    robot_navigation.h \
    monte_carlo_option1.h \
    tree_node.h \
    tree_class.h \
    monte_carlo_branch_data.h \
    monte_carlo_definitions.h \
    monte_carlo_node_data.h \
    d_star_map.h \
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
    district_map_definitions.h \
    tree_class_permanent.h \
    tree_node_permanent.h \
    map_2d_bool.h \
    monte_carlo_option2.h \
    robot_data.h \
    mc_path_storage.h \
    algorithm_a_star.h \
    file_tree_node.h \
    file_tree.h \
    file_tree_save_file.h

unix:!macx: LIBS += -lpng
