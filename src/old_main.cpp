#include <flecs.h>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include <vital_module.h>

#include <map>
#include <functional>

double get_process_memory_in_gb() {
    std::ifstream status_file("/proc/self/status");
    if (!status_file.is_open()) {
        std::cerr << "Failed to open /proc/self/status to read memory usage." << std::endl;
        return 0.0;
    }

    std::string line;
    while (std::getline(status_file, line)) {
        if (line.find("VmRSS:") == 0) { // Look for the "VmRSS" line
            std::istringstream iss(line);
            std::string label;
            size_t memory_kb;
            iss >> label >> memory_kb; // Parse the memory in KB
            return static_cast<double>(memory_kb) / 1048576.0; // Convert KB to GB
        }
    }

    return 0.0; // Return 0.0 GB if "VmRSS" not found
}

// Component to store the title of an article
struct Title {
    std::string value;
};

// Relationships
struct BelongsToCategory {};    // Relationship: Article -> Category
struct BelongsToSubcategory {}; // Relationship: Article -> Subcategory
struct PartOfCategory {};       // Relationship: Subcategory -> Category

// For as far as the flecs core/performance is concerned all components are runtime components, so the only thing 
// you'd get from code generation is a nicer API
// Sander
// void create_runtime_components(flecs::world& world, const std::string& file_path) {
//     // Open JSON file
//     std::ifstream file(file_path);
//     if (!file.is_open()) {
//         std::cerr << "Failed to open file: " << file_path << std::endl;
//         return;
//     }

//     // Parse JSON
//     nlohmann::json data;
//     file >> data;

//     // Maps to store dynamically created components for categories and subcategories
//     std::unordered_map<std::string, flecs::entity> category_components;
//     std::unordered_map<std::string, flecs::entity> subcategory_components;

//     size_t count = 0;

//     for (const auto& entry : data) {
//         if (!entry.contains("title") || !entry.contains("category")) {
//             std::cerr << "Skipping entry: missing required fields" << std::endl;
//             continue;
//         }

//         std::string title = entry["title"];
//         std::string category_name = entry["category"];
//         std::string subcategory_name = entry.contains("subcategory") && !entry["subcategory"].is_null()
//     ? entry["subcategory"].get<std::string>()
//     : "";

//         // Dynamically create or retrieve the component for the category
//         flecs::entity category_component;
//         if (category_components.find(category_name) == category_components.end()) {
//             category_component = world.component(category_name.c_str());
//             category_components[category_name] = category_component;
//             std::cout << "Created runtime component for category: " << category_name << std::endl;
//         } else {
//             category_component = category_components[category_name];
//         }

//         // Dynamically create or retrieve the component for the subcategory
//         flecs::entity subcategory_component;
//         if (!subcategory_name.empty() && subcategory_components.find(subcategory_name) == subcategory_components.end()) {
//             subcategory_component = world.component(subcategory_name.c_str());
//             subcategory_components[subcategory_name] = subcategory_component;
//             std::cout << "Created runtime component for subcategory: " << subcategory_name << std::endl;
//         } else if (!subcategory_name.empty()) {
//             subcategory_component = subcategory_components[subcategory_name];
//         }

//         auto article_entity = world.entity(title.c_str());
//         article_entity.add(category_component);
//         if (!subcategory_name.empty())
//         {
//             article_entity.add(subcategory_component);
//         }

//         count++;

//         // Debug output every 1000 entities
//         if (count % 1000 == 0) {
//             std::cout << "Loaded " << count << " articles..." << std::endl;
//         }
//     }

//     std::cout << "Finished loading " << count << " Wikipedia articles." << std::endl;
// }

void create_codegen_components(flecs::world& world, const std::string& file_path) {
    // Open JSON file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    // Parse JSON
    nlohmann::json data;
    file >> data;

    size_t count = 0;

    for (const auto& entry : data) {
        // Validate required fields
        if (!entry.contains("title") || !entry.contains("category")) {
            std::cerr << "Skipping entry: missing required fields" << std::endl;
            continue;
        }

        std::string title = entry["title"];
        std::string category_name = entry["category"];
        std::string subcategory_name = entry.contains("subcategory") && !entry["subcategory"].is_null()
            ? entry["subcategory"].get<std::string>()
            : "";

        // Create the entity for the article
        // TODO: Escaped string lookup...
        auto article_entity = world.lookup(title.c_str());
        if (article_entity.is_valid())
        {
            char *str = ecs_type_str(world, ecs_get_type(world, article_entity));
            printf("[%s]\n", str);
            ecs_os_free(str);
        }

        // Add the component entity

        // Apply the category component using the mappings
        // auto category_it = component_mappings.find(category_name);
        // if (category_it != component_mappings.end()) {
        //     category_it->second(article_entity); // Call the mapped function
        // } else {
        //     std::cerr << "Unknown category: " << category_name << std::endl;
        // }

        // // Apply the subcategory component using the mappings, if available
        // if (!subcategory_name.empty()) {
        //     auto subcategory_it = component_mappings.find(subcategory_name);
        //     if (subcategory_it != component_mappings.end()) {
        //         subcategory_it->second(article_entity); // Call the mapped function
        //     } else {
        //         std::cerr << "Unknown subcategory: " << subcategory_name << std::endl;
        //     }
        // }

        // // Apply hierarchy components
        // if (entry.contains("hierarchy") && entry["hierarchy"].is_array()) {
        //     for (const auto& hierarchy_name : entry["hierarchy"]) {
        //         if (hierarchy_name.is_string()) {
        //             // Escape double quotes for valid search
        //             std::string escaped_hierarchy_name = hierarchy_name.get<std::string>();
        //             std::replace(escaped_hierarchy_name.begin(), escaped_hierarchy_name.end(), '"', '\"');

        //             // Look up the hierarchy component in the mappings
        //             auto hierarchy_it = component_mappings.find(escaped_hierarchy_name);
        //             if (hierarchy_it != component_mappings.end()) {
        //                 hierarchy_it->second(article_entity); // Call the mapped function
        //             } else {
        //                 std::cerr << "Unknown hierarchy component: " << escaped_hierarchy_name << std::endl;
        //             }
        //         }
        //     }
        // }

        // count++;

        // // Debug output every 1000 entities
        // if (count % 1000 == 0) {
        //     std::cout << "Processed " << count << " articles..." << std::endl;
        // }
    }

    std::cout << "Finished processing " << count << " Wikipedia articles using codegen mappings." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_json_file>" << std::endl;
        return 1;
    }

    const std::string file_path = argv[1];

    // Initialize Flecs world
    flecs::world world;

    ECS_IMPORT(world, VitalModule);

    world.each([](flecs::entity e, vital.module.COM_Topology) {
        std::cout << "Component: " << e.name() << std::endl;
    });

    // Process articles using code-generated mappings
    //create_codegen_components(world, file_path);

    // // Example query: print all entities with COM_Video_games
    // auto q = world.query_builder<COM_Video_games>().cached().build();
    // std::cout << "Entities with COM_Video_games:" << std::endl;
    // q.each([](flecs::entity e, const COM_Video_games& subcategory) {
    //     std::cout << e.name() << std::endl;
    // });

    return 0;
}