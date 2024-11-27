#include <unordered_map>

// Define category components
struct People {};
struct History {};
struct Geography {};
struct Arts {};
struct Philosophy_and_religion {};
struct Everyday_life {};
struct Society_and_social_sciences {};
struct Biology_and_health_sciences {};
struct Physical_sciences {};
struct Technology {};
struct Mathematics {};

// Define subcategory components
struct Writers_and_journalists {};
struct Artists_musicians_and_composers {};
struct Entertainers_directors_producers_and_screenwriters {};
struct Philosophers_historians_and_social_scientists {};
struct Religious_figures {};
struct Politicians_and_leaders {};
struct Military_personnel_revolutionaries_and_activists {};
struct Scientists_inventors_and_mathematicians {};
struct Sports_figures {};
struct Miscellaneous {};

struct Physical_geography {};
struct Countries_and_subdivisions {};
struct Cities {};

struct Everyday_life_subcategory {};
struct Sports_games_and_recreation {};

struct Social_studies {};
struct Politics_and_economics {};
struct Culture {};

struct Biology_biochemistry_anatomy_and_physiology {};
struct Animals {};
struct Plants_fungi_and_other_organisms {};
struct Health_medicine_and_disease {};

struct Basics_and_measurement {};
struct Astronomy {};
struct Chemistry {};
struct Earth_science {};
struct Physics {};

// Template for category mapping
template <typename T>
struct CategoryMapping;

#define MAP_CATEGORY(name, type) \
    template <>                  \
    struct CategoryMapping<type> { static constexpr const char* value = name; };

// Map category names to types
MAP_CATEGORY("People", People)
MAP_CATEGORY("History", History)
MAP_CATEGORY("Geography", Geography)
MAP_CATEGORY("Arts", Arts)
MAP_CATEGORY("Philosophy_and_religion", Philosophy_and_religion)
MAP_CATEGORY("Everyday_life", Everyday_life)
MAP_CATEGORY("Society_and_social_sciences", Society_and_social_sciences)
MAP_CATEGORY("Biology_and_health_sciences", Biology_and_health_sciences)
MAP_CATEGORY("Physical_sciences", Physical_sciences)
MAP_CATEGORY("Technology", Technology)
MAP_CATEGORY("Mathematics", Mathematics)

// Subcategory template
template <typename T>
struct SubcategoryMapping;

#define MAP_SUBCATEGORY(name, type) \
    template <>                     \
    struct SubcategoryMapping<type> { static constexpr const char* value = name; };

// Map subcategory names to types
MAP_SUBCATEGORY("Writers_and_journalists", Writers_and_journalists)
MAP_SUBCATEGORY("Artists,_musicians,_and_composers", Artists_musicians_and_composers)
MAP_SUBCATEGORY("Entertainers,_directors,_producers,_and_screenwriters", Entertainers_directors_producers_and_screenwriters)
MAP_SUBCATEGORY("Religious_figures", Religious_figures)
// Add remaining mappings here...

// Register all components using templates
template <typename... Components>
void register_components(flecs::world& world) {
    (world.component<Components>(), ...);
}

// Assign components dynamically
template <typename T>
void assign_component(flecs::entity& entity, const std::string& name) {
    if (name == CategoryMapping<T>::value || name == SubcategoryMapping<T>::value) {
        entity.add<T>();
    }
}
