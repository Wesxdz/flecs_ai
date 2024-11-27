#include <flecs.h>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <generated_mappings.h>

#include <map>
#include <functional>

struct CATEGORY_People {};
struct SUBCATEGORY_Writers_and_journalists {};
struct SUBCATEGORY_Artists__musicians__and_composers {};
struct SUBCATEGORY_Entertainers__directors__producers__and_screenwriters {};
struct SUBCATEGORY_Philosophers__historians__and_social_scientists {};
struct SUBCATEGORY_Religious_figures {};
struct SUBCATEGORY_Politicians_and_leaders {};
struct SUBCATEGORY_Military_personnel__revolutionaries__and_activists {};
struct SUBCATEGORY_Scientists__inventors__and_mathematicians {};
struct SUBCATEGORY_Sports_figures {};
struct SUBCATEGORY_Miscellaneous {};
struct CATEGORY_History {};
struct CATEGORY_Geography {};
struct SUBCATEGORY_Physical_geography {};
struct SUBCATEGORY_Countries_and_subdivisions {};
struct SUBCATEGORY_Cities {};
struct CATEGORY_Arts {};
struct CATEGORY_Philosophy_and_religion {};
struct CATEGORY_Everyday_life {};
struct SUBCATEGORY_Everyday_life {};
struct SUBCATEGORY_Sports__games_and_recreation {};
struct CATEGORY_Society_and_social_sciences {};
struct SUBCATEGORY_Social_studies {};
struct SUBCATEGORY_Politics_and_economics {};
struct SUBCATEGORY_Culture {};
struct CATEGORY_Biology_and_health_sciences {};
struct SUBCATEGORY_Biology__biochemistry__anatomy__and_physiology {};
struct SUBCATEGORY_Animals {};
struct SUBCATEGORY_Plants__fungi__and_other_organisms {};
struct SUBCATEGORY_Health__medicine__and_disease {};
struct CATEGORY_Physical_sciences {};
struct SUBCATEGORY_Basics_and_measurement {};
struct SUBCATEGORY_Astronomy {};
struct SUBCATEGORY_Chemistry {};
struct SUBCATEGORY_Earth_science {};
struct SUBCATEGORY_Physics {};
struct CATEGORY_Technology {};
struct CATEGORY_Mathematics {};

// Ordered map for category mappings
static const std::map<std::string, std::function<void(flecs::entity&)>> category_mappings = {
    {"People", [](flecs::entity& e) { e.add<CATEGORY_People>(); } },
    {"History", [](flecs::entity& e) { e.add<CATEGORY_History>(); } },
    {"Geography", [](flecs::entity& e) { e.add<CATEGORY_Geography>(); } },
    {"Arts", [](flecs::entity& e) { e.add<CATEGORY_Arts>(); } },
    {"Philosophy_and_religion", [](flecs::entity& e) { e.add<CATEGORY_Philosophy_and_religion>(); } },
    {"Everyday_life", [](flecs::entity& e) { e.add<CATEGORY_Everyday_life>(); } },
    {"Society_and_social_sciences", [](flecs::entity& e) { e.add<CATEGORY_Society_and_social_sciences>(); } },
    {"Biology_and_health_sciences", [](flecs::entity& e) { e.add<CATEGORY_Biology_and_health_sciences>(); } },
    {"Physical_sciences", [](flecs::entity& e) { e.add<CATEGORY_Physical_sciences>(); } },
    {"Technology", [](flecs::entity& e) { e.add<CATEGORY_Technology>(); } },
    {"Mathematics", [](flecs::entity& e) { e.add<CATEGORY_Mathematics>(); } },
};

// Ordered map for subcategory mappings
static const std::map<std::string, std::function<void(flecs::entity&)>> subcategory_mappings = {
    {"Writers_and_journalists", [](flecs::entity& e) { e.add<SUBCATEGORY_Writers_and_journalists>(); } },
    {"Artists,_musicians,_and_composers", [](flecs::entity& e) { e.add<SUBCATEGORY_Artists__musicians__and_composers>(); } },
    {"Entertainers,_directors,_producers,_and_screenwriters", [](flecs::entity& e) { e.add<SUBCATEGORY_Entertainers__directors__producers__and_screenwriters>(); } },
    {"Philosophers,_historians,_and_social_scientists", [](flecs::entity& e) { e.add<SUBCATEGORY_Philosophers__historians__and_social_scientists>(); } },
    {"Religious_figures", [](flecs::entity& e) { e.add<SUBCATEGORY_Religious_figures>(); } },
    {"Politicians_and_leaders", [](flecs::entity& e) { e.add<SUBCATEGORY_Politicians_and_leaders>(); } },
    {"Military_personnel,_revolutionaries,_and_activists", [](flecs::entity& e) { e.add<SUBCATEGORY_Military_personnel__revolutionaries__and_activists>(); } },
    {"Scientists,_inventors,_and_mathematicians", [](flecs::entity& e) { e.add<SUBCATEGORY_Scientists__inventors__and_mathematicians>(); } },
    {"Sports_figures", [](flecs::entity& e) { e.add<SUBCATEGORY_Sports_figures>(); } },
    {"Miscellaneous", [](flecs::entity& e) { e.add<SUBCATEGORY_Miscellaneous>(); } },
    {"Physical_geography", [](flecs::entity& e) { e.add<SUBCATEGORY_Physical_geography>(); } },
    {"Countries_and_subdivisions", [](flecs::entity& e) { e.add<SUBCATEGORY_Countries_and_subdivisions>(); } },
    {"Cities", [](flecs::entity& e) { e.add<SUBCATEGORY_Cities>(); } },
    {"Everyday_life", [](flecs::entity& e) { e.add<SUBCATEGORY_Everyday_life>(); } },
    {"Sports,_games_and_recreation", [](flecs::entity& e) { e.add<SUBCATEGORY_Sports__games_and_recreation>(); } },
    {"Social_studies", [](flecs::entity& e) { e.add<SUBCATEGORY_Social_studies>(); } },
    {"Politics_and_economics", [](flecs::entity& e) { e.add<SUBCATEGORY_Politics_and_economics>(); } },
    {"Culture", [](flecs::entity& e) { e.add<SUBCATEGORY_Culture>(); } },
    {"Biology,_biochemistry,_anatomy,_and_physiology", [](flecs::entity& e) { e.add<SUBCATEGORY_Biology__biochemistry__anatomy__and_physiology>(); } },
    {"Animals", [](flecs::entity& e) { e.add<SUBCATEGORY_Animals>(); } },
    {"Plants,_fungi,_and_other_organisms", [](flecs::entity& e) { e.add<SUBCATEGORY_Plants__fungi__and_other_organisms>(); } },
    {"Health,_medicine,_and_disease", [](flecs::entity& e) { e.add<SUBCATEGORY_Health__medicine__and_disease>(); } },
    {"Basics_and_measurement", [](flecs::entity& e) { e.add<SUBCATEGORY_Basics_and_measurement>(); } },
    {"Astronomy", [](flecs::entity& e) { e.add<SUBCATEGORY_Astronomy>(); } },
    {"Chemistry", [](flecs::entity& e) { e.add<SUBCATEGORY_Chemistry>(); } },
    {"Earth_science", [](flecs::entity& e) { e.add<SUBCATEGORY_Earth_science>(); } },
    {"Physics", [](flecs::entity& e) { e.add<SUBCATEGORY_Physics>(); } },
};

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
void create_runtime_components(flecs::world& world, const std::string& file_path) {
    // Open JSON file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    // Parse JSON
    nlohmann::json data;
    file >> data;

    // Maps to store dynamically created components for categories and subcategories
    std::unordered_map<std::string, flecs::entity> category_components;
    std::unordered_map<std::string, flecs::entity> subcategory_components;

    size_t count = 0;

    for (const auto& entry : data) {
        if (!entry.contains("title") || !entry.contains("category")) {
            std::cerr << "Skipping entry: missing required fields" << std::endl;
            continue;
        }

        std::string title = entry["title"];
        std::string category_name = entry["category"];
        std::string subcategory_name = entry.contains("subcategory") && !entry["subcategory"].is_null()
    ? entry["subcategory"].get<std::string>()
    : "";

        // Dynamically create or retrieve the component for the category
        flecs::entity category_component;
        if (category_components.find(category_name) == category_components.end()) {
            category_component = world.component(category_name.c_str());
            category_components[category_name] = category_component;
            std::cout << "Created runtime component for category: " << category_name << std::endl;
        } else {
            category_component = category_components[category_name];
        }

        // Dynamically create or retrieve the component for the subcategory
        flecs::entity subcategory_component;
        if (!subcategory_name.empty() && subcategory_components.find(subcategory_name) == subcategory_components.end()) {
            subcategory_component = world.component(subcategory_name.c_str());
            subcategory_components[subcategory_name] = subcategory_component;
            std::cout << "Created runtime component for subcategory: " << subcategory_name << std::endl;
        } else if (!subcategory_name.empty()) {
            subcategory_component = subcategory_components[subcategory_name];
        }

        auto article_entity = world.entity(title.c_str());
        article_entity.add(category_component);
        if (!subcategory_name.empty())
        {
            article_entity.add(subcategory_component);
        }

        count++;

        // Debug output every 1000 entities
        if (count % 1000 == 0) {
            std::cout << "Loaded " << count << " articles..." << std::endl;
        }
    }

    std::cout << "Finished loading " << count << " Wikipedia articles." << std::endl;
}

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
        auto article_entity = world.entity(title.c_str());

        // Apply the category component using the mappings
        auto category_it = category_mappings.find(category_name);
        if (category_it != category_mappings.end()) {
            category_it->second(article_entity); // Call the mapped function
        } else {
            std::cerr << "Unknown category: " << category_name << std::endl;
        }

        // Apply the subcategory component using the mappings, if available
        if (!subcategory_name.empty()) {
            auto subcategory_it = subcategory_mappings.find(subcategory_name);
            if (subcategory_it != subcategory_mappings.end()) {
                subcategory_it->second(article_entity); // Call the mapped function
            } else {
                std::cerr << "Unknown subcategory: " << subcategory_name << std::endl;
            }
        }

        count++;

        // Debug output every 1000 entities
        if (count % 1000 == 0) {
            std::cout << "Processed " << count << " articles..." << std::endl;
        }
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

    // Process articles using code-generated mappings
    create_codegen_components(world, file_path);

    // Example query: print all entities with SUBCATEGORY_Miscellaneous
    auto q = world.query_builder<CATEGORY_Everyday_life>().cached().build();
    std::cout << "Entities with CATEGORY_Everyday_life:" << std::endl;
    q.each([](flecs::entity e, const CATEGORY_Everyday_life& subcategory) {
        std::cout << e.name() << std::endl;
    });

    return 0;
}

