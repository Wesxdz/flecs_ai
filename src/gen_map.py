import json
import re

# File paths
input_file = "vital_articles_hierarchy.json"  # Input JSON file
output_file = "generated_mappings.h"         # Output header file

# Define categories and subcategories
categories = {
    "People": [
        "Writers_and_journalists", "Artists,_musicians,_and_composers",
        "Entertainers,_directors,_producers,_and_screenwriters",
        "Philosophers,_historians,_and_social_scientists", "Religious_figures",
        "Politicians_and_leaders", "Military_personnel,_revolutionaries,_and_activists",
        "Scientists,_inventors,_and_mathematicians", "Sports_figures", "Miscellaneous"
    ],
    "History": [],
    "Geography": ["Physical_geography", "Countries_and_subdivisions", "Cities"],
    "Arts": [],
    "Philosophy_and_religion": [],
    "Everyday_life": ["Everyday_life", "Sports,_games_and_recreation"],
    "Society_and_social_sciences": ["Social_studies", "Politics_and_economics", "Culture"],
    "Biology_and_health_sciences": [
        "Biology,_biochemistry,_anatomy,_and_physiology", "Animals",
        "Plants,_fungi,_and_other_organisms", "Health,_medicine,_and_disease"
    ],
    "Physical_sciences": ["Basics_and_measurement", "Astronomy", "Chemistry", "Earth_science", "Physics"],
    "Technology": [],
    "Mathematics": []
}

# Read and process the JSON file
with open(input_file, "r", encoding="utf-8") as file:
    vital_articles = json.load(file)

# Extract unique hierarchy strings
unique_hierarchy_strings = set()
for article in vital_articles:
    unique_hierarchy_strings.update(article.get("hierarchy", []))

# Function to sanitize names for valid C++ identifiers
def sanitize_name(name):
    sanitized = re.sub(r"[^a-zA-Z0-9_]", "_", name)  # Replace invalid characters with underscores
    sanitized = re.sub(r"__+", "_", sanitized)       # Replace multiple underscores with a single one
    return sanitized.strip("_")                      # Remove leading or trailing underscores

# Combine all unique component names
all_components = set(categories.keys())  # Start with category keys
for subcategories in categories.values():
    all_components.update(subcategories)  # Add subcategories
all_components.update(unique_hierarchy_strings)  # Add hierarchy strings

# Sanitize all component names
sanitized_components = {sanitize_name(name): name for name in all_components}

# Generate C++ code
with open(output_file, "w") as file:
    # Write the header guard
    file.write("#ifndef GENERATED_MAPPINGS_H\n")
    file.write("#define GENERATED_MAPPINGS_H\n\n")

    # Include required headers
    file.write("#include <flecs.h>\n")
    file.write("#include <unordered_map>\n")
    file.write("#include <functional>\n\n")

    # Generate structs for components
    file.write("// Component definitions\n")
    for sanitized_name in sanitized_components.keys():
        file.write(f"struct COM_{sanitized_name} {{}};\n")

    # Generate a single unordered_map for all components
    file.write("\n// Component mappings\n")
    file.write("static const std::unordered_map<std::string, std::function<void(flecs::entity&)>> component_mappings = {\n")
    for sanitized_name, original_name in sanitized_components.items():
        escaped_name = original_name.replace('"', '\\"')  # Escape quotes
        file.write(f'    {{"{escaped_name}", [](flecs::entity& e) {{ e.add<COM_{sanitized_name}>(); }} }},\n')
    file.write("};\n\n")

    # Write the footer guard
    file.write("#endif // GENERATED_MAPPINGS_H\n")

print(f"Generated mappings written to {output_file}")
