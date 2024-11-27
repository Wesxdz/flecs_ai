#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <flecs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 2560;
const int SCREEN_HEIGHT = 1440;

// Flecs Components
struct Position {
    int x, y;
};

struct Size {
    int width, height;
};

struct Texture {
    SDL_Texture* texture;
};

struct Camera {
    int x_offset;
};

struct MouseState {
    int cursor_x, cursor_y;
    int prev_cursor_x, prev_cursor_y;
    bool is_down;
};

struct InputState {
    bool left_pressed = false;
    bool right_pressed = false;
    float movement_speed = 800.0f; // Pixels per second
    float offset = 0.0f;           // Offset position
    float hold_accelerate_rate = 1000.0f;
    float accelerated_movement = 0.0f;
};

struct FileInfo {
    std::string name;
    std::string creation_time;
};

struct Tick {
    int x;            // Horizontal position of the tick
    int height;       // Height of the tick
    SDL_Color color;  // Color of the tick (RGBA)
};


void create_ticks(flecs::world& world, int screen_width) {
    // Gather all FileInfo entities and their creation times
    std::vector<std::pair<std::time_t, flecs::entity>> file_entities;

    world.each<FileInfo>([&](flecs::entity e, const FileInfo& file_info) {
        struct tm tm;
        strptime(file_info.creation_time.c_str(), "%a %b %d %H:%M:%S %Y", &tm); // Parse the time string
        std::time_t creation_time = std::mktime(&tm);
        file_entities.push_back({creation_time, e});
    });

    if (file_entities.empty()) {
        std::cerr << "No FileInfo entities found to create ticks!" << std::endl;
        return;
    }

    // Sort entities by creation time
    std::sort(file_entities.begin(), file_entities.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    // Get the time domain
    std::time_t first_time = file_entities.front().first;
    std::time_t last_time = file_entities.back().first;
    double time_range = std::max(1.0, static_cast<double>(last_time - first_time)); // Avoid division by zero

    int last_x_position = -1; // Track the last used x position

    for (const auto& [creation_time, entity] : file_entities) {
        // Map creation time to screen width
        double normalized_time = static_cast<double>(creation_time - first_time) / time_range;
        int x_position = static_cast<int>(normalized_time * screen_width);

        // Handle overlapping x positions
        if (x_position <= last_x_position) {
            x_position = last_x_position + 1;
        } else if (x_position >= screen_width) {
            x_position = screen_width - 1;
            while (x_position <= last_x_position) {
                x_position--;
            }
        }

        last_x_position = x_position;

        // Create regular tick
        world.entity().set<Tick>({
            x_position,      // x position
            32,              // Height for regular ticks
            {255, 255, 255, 128} // White color for regular ticks
        });

        // Add a shorter day tick at the same position
        std::time_t day_start_time = first_time;
        while (day_start_time <= last_time) {
            double normalized_time = static_cast<double>(day_start_time - first_time) / time_range;
            int x_position = static_cast<int>(normalized_time * screen_width);

            world.entity().set<Tick>({
                x_position,      // x position
                24,              // Height for day ticks
                {200, 0, 0, 255} // Red color for day ticks
            });

            // Debug output
            std::cout << "Day Tick: Time: " << std::ctime(&day_start_time)
                    << ", X Position: " << x_position << std::endl;

            // Move to the next day
            day_start_time += 24 * 60 * 60;
        }

        // Debug output
        const FileInfo* file_info = entity.get<FileInfo>();
        if (file_info) {
            std::cout << "Tick for file: " << file_info->name
                      << ", Creation Time: " << file_info->creation_time
                      << ", X Position: " << x_position << std::endl;
        }
    }
}



// Function to load PNG files as entities
void load_png_files_as_entities(flecs::world& world, SDL_Renderer* renderer, const std::string& directory) {
    struct dirent* entry;
    struct stat file_stat;
    std::string filepath;
    std::vector<std::pair<std::time_t, flecs::entity>> entities;

    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        std::string file_name = entry->d_name;
        if (file_name.length() < 4 || file_name.substr(file_name.length() - 4) != ".png") {
            continue;
        }

        filepath = directory + "/" + file_name;

        if (stat(filepath.c_str(), &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            std::time_t creation_time = file_stat.st_ctime;

            SDL_Surface* surface = IMG_Load(filepath.c_str());
            if (!surface) {
                std::cerr << "Failed to load image: " << file_name
                          << " SDL_image Error: " << IMG_GetError() << std::endl;
                continue;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            int imageWidth = surface->w;
            int imageHeight = surface->h;
            SDL_FreeSurface(surface);

            int renderHeight = std::min(500, imageHeight);
            int renderWidth = (renderHeight * imageWidth) / imageHeight;

            auto entity = world.entity()
                .set<FileInfo>({file_name, std::ctime(&creation_time)})
                .set<Position>({0, 0})
                .set<Size>({renderWidth, renderHeight})
                .set<Texture>({texture});

            entities.push_back({creation_time, entity});
        }
    }

    closedir(dir);

    if (entities.empty()) {
        std::cerr << "No PNG files found in directory: " << directory << std::endl;
        return;
    }

    std::sort(entities.begin(), entities.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    const std::time_t base_time = entities.front().first;

    const int padding = 16; // Fixed space between images
    const int center_y = SCREEN_HEIGHT/2;

    // Start current_x with an initial position at a fixed point (no centering adjustment for simplicity)
    auto& [creation_time, entity] = entities[0];
    int current_x = 0;

    for (size_t i = 0; i < entities.size(); ++i) {
        auto& [creation_time, entity] = entities[i];

        current_x += entity.get<Size>()->width/2;
        // Position the current entity
        entity.set<Position>({current_x, center_y});

        // Debug output for verification
        const FileInfo* file_info = entity.get<FileInfo>();
        const Size* size = entity.get<Size>();
        if (file_info && size) {
            std::cout << "Entity " << i << ": "
                    << "File: " << file_info->name
                    << ", Width: " << size->width
                    << ", Position: (" << current_x << ", " << center_y << ")"
                    << std::endl;

            // Update current_x for the next image
            current_x += size->width/2 + padding; // Add image width and fixed padding
        } else {
            std::cerr << "Entity " << i << " missing Size or FileInfo components" << std::endl;
        }
    }


}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Entropy Executioner",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);


    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    flecs::world world;

    world.component<FileInfo>();
    world.component<Position>();
    world.component<Size>();
    world.component<Texture>();
    world.component<Camera>();
    world.component<InputState>();
    world.component<Tick>();

    load_png_files_as_entities(world, renderer, "/home/wesxdz/paphos/maids/modus/chassis/dev-log/");
    create_ticks(world, SCREEN_WIDTH);
    
    auto camera = world.entity()
        .set<Camera>({0})
        .add<InputState>();

    world.system<InputState>()
        .kind(flecs::OnUpdate)
        .each([&](InputState& input_state) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    world.quit();
                }
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT: input_state.left_pressed = true; break;
                        case SDLK_RIGHT: input_state.right_pressed = true; break;
                    }
                }
                if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT: input_state.left_pressed = false; break;
                        case SDLK_RIGHT: input_state.right_pressed = false; break;
                    }
                }
            }

            const float delta_time = world.delta_time();
            if (input_state.left_pressed) {
                input_state.offset -= (input_state.movement_speed + input_state.accelerated_movement) * delta_time;
                input_state.accelerated_movement += (input_state.hold_accelerate_rate + input_state.accelerated_movement) * delta_time;
            }
            else if (input_state.right_pressed) {
                input_state.offset += (input_state.movement_speed + input_state.accelerated_movement) * delta_time;
                input_state.accelerated_movement += (input_state.hold_accelerate_rate + input_state.accelerated_movement) * delta_time;
            }
            else
            {
                input_state.accelerated_movement = 0.0f;
            }
        });

    world.system<Camera, InputState>()
        .kind(flecs::OnUpdate)
        .each([](Camera& cam, InputState& input_state) {
            cam.x_offset = input_state.offset;
        });

    // world.system<Tick>()
    //     .kind(flecs::OnUpdate)
        

    while (world.progress()) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        const Camera* cam = camera.get<Camera>();
        int camera_offset = cam->x_offset;

        world.each([&](const Position& pos, const Size& size, const Texture& tex) {
            SDL_Rect renderQuad = {
                pos.x - size.width / 2 - camera_offset,
                pos.y - size.height / 2,
                size.width,
                size.height
            };
            SDL_RenderCopy(renderer, tex.texture, NULL, &renderQuad);
        });

        world.each([&](const Tick& tick) {
            // Set tick color
            SDL_SetRenderDrawColor(renderer, tick.color.r, tick.color.g, tick.color.b, tick.color.a);

            // Draw the tick
            SDL_RenderDrawLine(renderer, tick.x, SCREEN_HEIGHT, tick.x, SCREEN_HEIGHT - tick.height);
        });

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
