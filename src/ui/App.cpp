#include "ui/App.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include <SDL.h>

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace {
static constexpr ImWchar kKoreanRanges[] = {
    0x0020, 0x00FF,
    0x3131, 0x3163,
    0xAC00, 0xD7A3,
    0,
};

bool loadKoreanFontWithFallback() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    const std::vector<std::string> candidates = {
#if defined(_WIN32)
        "C:/Windows/Fonts/malgun.ttf",
        "C:/Windows/Fonts/NanumGothic.ttf",
#elif defined(__APPLE__)
        "/System/Library/Fonts/AppleSDGothicNeo.ttc",
        "/Library/Fonts/NanumGothic.ttf",
#else
        "/usr/share/fonts/truetype/nanum/NanumGothic.ttf",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
#endif
    };

    for (const auto& path : candidates) {
        if (!std::filesystem::exists(path)) {
            continue;
        }

        if (io.Fonts->AddFontFromFileTTF(path.c_str(), 18.0f, nullptr, kKoreanRanges) != nullptr) {
            return true;
        }
    }

    io.Fonts->AddFontDefault();
    return false;
}

struct GeneratedMap {
    int width = 0;
    int height = 0;
    std::vector<int> tiles;
};

std::vector<GeneratedMap> createTileMaps(int mapWidth, int mapHeight, int mapCount) {
    std::vector<GeneratedMap> maps;
    maps.reserve(mapCount);

    for (int mapIndex = 0; mapIndex < mapCount; ++mapIndex) {
        GeneratedMap map;
        map.width = mapWidth;
        map.height = mapHeight;
        map.tiles.reserve(mapWidth * mapHeight);

        for (int row = 0; row < mapHeight; ++row) {
            for (int col = 0; col < mapWidth; ++col) {
                map.tiles.push_back(col + 1);
            }
        }

        maps.push_back(std::move(map));
    }

    return maps;
}

void ensureMinimumMapCountForMultiplayer(
    std::vector<GeneratedMap>& maps,
    int mapWidth,
    int mapHeight
) {
    if (maps.size() >= 2) {
        return;
    }

    const int missingMapCount = 2 - static_cast<int>(maps.size());
    std::vector<GeneratedMap> additionalMaps = createTileMaps(mapWidth, mapHeight, missingMapCount);
    maps.insert(maps.end(), additionalMaps.begin(), additionalMaps.end());
}
} // namespace

int AppUI::run() {
    int mapCount = 1;
    int mapWidth = 3;
    int mapHeight = 2;
    bool isMultiplayerMode = false;
    std::vector<GeneratedMap> generatedMaps;
    int currentMapIndex = 0;
    std::vector<std::string> generationLogs = {
        "[INFO] Ready.",
        "[INFO] Waiting for generation tasks..."
    };

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Tile Matching UI",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1500,
        950,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    const bool koreanFontLoaded = loadKoreanFontWithFallback();

    if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer)) {
        SDL_Log("ImGui_ImplSDL2_InitForSDLRenderer failed");
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
        SDL_Log("ImGui_ImplSDLRenderer2_Init failed");
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Control Panel");
        ImGui::TextUnformatted("Stage Map Count");
        ImGui::Separator();
        ImGui::InputInt("Maps to Generate for Stage", &mapCount);
        if (mapCount < 1) {
            mapCount = 1;
        }
        const int mapsVisibleInViewer = isMultiplayerMode ? 2 : 1;
        if (ImGui::Button("Start Making Maps")) {
            generationLogs.clear();
            generationLogs.push_back("[INFO] Generating " + std::to_string(mapCount) + " map(s)...");
            generatedMaps.clear();
            generatedMaps = createTileMaps(mapWidth, mapHeight, mapCount);
            if (isMultiplayerMode) {
                ensureMinimumMapCountForMultiplayer(generatedMaps, mapWidth, mapHeight);
            }
            currentMapIndex = 0;
            generationLogs.push_back("[INFO] Done.");
            generationLogs.push_back(
                "[INFO] Viewer shows " + std::to_string(mapsVisibleInViewer) +
                " map(s) in " + (isMultiplayerMode ? std::string("Multi") : std::string("Single")) + " mode."
            );
        }
        ImGui::Text(
            "Ready to generate %d map(s). Viewer shows %d map(s) in %s mode.",
            mapCount,
            mapsVisibleInViewer,
            isMultiplayerMode ? "Multi" : "Single"
        );
        ImGui::Separator();
        ImGui::TextUnformatted("Map Size");
        ImGui::InputInt("Width", &mapWidth);
        ImGui::InputInt("Height", &mapHeight);
        if (mapWidth < 1) {
            mapWidth = 1;
        }
        if (mapHeight < 1) {
            mapHeight = 1;
        }
        ImGui::Separator();
        ImGui::TextUnformatted("Mode");
        const bool previousMultiplayerMode = isMultiplayerMode;
        ImGui::Checkbox("Multiplayer", &isMultiplayerMode);
        if (previousMultiplayerMode != isMultiplayerMode) {
            if (isMultiplayerMode) {
                ensureMinimumMapCountForMultiplayer(generatedMaps, mapWidth, mapHeight);
            }

            if (!generatedMaps.empty()) {
                currentMapIndex = std::clamp(currentMapIndex, 0, static_cast<int>(generatedMaps.size()) - 1);
            } else {
                currentMapIndex = 0;
            }

            generationLogs.push_back(
                "[INFO] Viewer shows " + std::to_string(isMultiplayerMode ? 2 : 1) +
                " map(s) in " + (isMultiplayerMode ? std::string("Multi") : std::string("Single")) + " mode."
            );
        }
        ImGui::TextUnformatted(isMultiplayerMode ? "Current: Multi Mode" : "Current: Single Mode");
        ImGui::Separator();
        ImGui::Text("Korean font loaded: %s", koreanFontLoaded ? "Yes" : "No (fallback)");
        ImGui::End();

        ImGui::Begin("Generation Logs");
        for (const std::string& log : generationLogs) {
            ImGui::TextUnformatted(log.c_str());
        }
        ImGui::End();

        ImGui::Begin("Viewer");
        if (generatedMaps.empty()) {
            ImGui::TextUnformatted("Press 'Start Making Maps' to create a tile map.");
        } else {
            constexpr float kTileSize = 28.0f;
            constexpr float kTileGap = 4.0f;
            constexpr float kMapPanelGap = 32.0f;

            currentMapIndex = std::clamp(currentMapIndex, 0, static_cast<int>(generatedMaps.size()) - 1);

            if (ImGui::Button("Prev")) {
                currentMapIndex = std::max(0, currentMapIndex - 1);
            }
            ImGui::SameLine();
            if (ImGui::Button("Next")) {
                currentMapIndex = std::min(static_cast<int>(generatedMaps.size()) - 1, currentMapIndex + 1);
            }
            ImGui::SameLine();
            if (isMultiplayerMode) {
                const int rightIndex = currentMapIndex + 1;
                if (rightIndex < static_cast<int>(generatedMaps.size())) {
                    ImGui::Text("Stages %d & %d / %zu", currentMapIndex + 1, rightIndex + 1, generatedMaps.size());
                } else {
                    ImGui::Text("Stage %d & No Stage / %zu", currentMapIndex + 1, generatedMaps.size());
                }
            } else {
                ImGui::Text("Stage %d / %zu", currentMapIndex + 1, generatedMaps.size());
            }

            ImGui::Separator();

            auto drawMapPanel = [&](int mapIndex, bool allowPlaceholder) {
                if (mapIndex < 0 || mapIndex >= static_cast<int>(generatedMaps.size())) {
                    if (allowPlaceholder) {
                        ImGui::TextUnformatted("No Map");
                        ImGui::Separator();
                        ImGui::BeginDisabled();
                        ImGui::Button("No Map", ImVec2(120.0f, 48.0f));
                        ImGui::EndDisabled();
                    }
                    return;
                }

                const GeneratedMap& map = generatedMaps[mapIndex];
                ImGui::Text("Stage %d", mapIndex + 1);
                ImGui::Text("Tile Map (%d x %d)", map.width, map.height);
                ImGui::Separator();

                for (int row = 0; row < map.height; ++row) {
                    for (int col = 0; col < map.width; ++col) {
                        const int tileIndex = row * map.width + col;
                        ImGui::PushID(static_cast<int>((mapIndex * 100000) + tileIndex));
                        const std::string tileLabel = std::to_string(map.tiles[tileIndex]);
                        ImGui::Button(tileLabel.c_str(), ImVec2(kTileSize, kTileSize));
                        ImGui::PopID();

                        if (col + 1 < map.width) {
                            ImGui::SameLine(0.0f, kTileGap);
                        }
                    }
                }
            };

            if (isMultiplayerMode) {
                const float availableWidth = ImGui::GetContentRegionAvail().x;
                const float panelWidth = std::max(120.0f, (availableWidth - kMapPanelGap) * 0.5f);

                ImGui::BeginChild("MapPanelLeft", ImVec2(panelWidth, 0), true);
                drawMapPanel(currentMapIndex, false);
                ImGui::EndChild();

                ImGui::SameLine(0.0f, kMapPanelGap);

                ImGui::BeginChild("MapPanelRight", ImVec2(panelWidth, 0), true);
                drawMapPanel(currentMapIndex + 1, true);
                ImGui::EndChild();
            } else {
                drawMapPanel(currentMapIndex, false);
            }
        }
        ImGui::End();

        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
