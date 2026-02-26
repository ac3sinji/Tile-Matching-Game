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

struct StageData {
    std::vector<GeneratedMap> maps;
};

int getMapCountPerStage(bool isMultiplayerMode) {
    return isMultiplayerMode ? 2 : 1;
}

std::vector<GeneratedMap> createMapsForStage(int mapWidth, int mapHeight, int mapCountPerStage) {
    std::vector<GeneratedMap> maps;
    maps.reserve(mapCountPerStage);

    for (int mapIndex = 0; mapIndex < mapCountPerStage; ++mapIndex) {
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

std::vector<StageData> createStages(
    int stageCount,
    int mapWidth,
    int mapHeight,
    bool isMultiplayerMode
) {
    const int mapCountPerStage = getMapCountPerStage(isMultiplayerMode);
    std::vector<StageData> stages;
    stages.reserve(stageCount);

    for (int stageIndex = 0; stageIndex < stageCount; ++stageIndex) {
        StageData stage;
        stage.maps = createMapsForStage(mapWidth, mapHeight, mapCountPerStage);
        stages.push_back(std::move(stage));
    }

    return stages;
}

void syncStageMapCountToMode(
    std::vector<StageData>& stages,
    int mapWidth,
    int mapHeight,
    bool isMultiplayerMode
) {
    const int mapCountPerStage = getMapCountPerStage(isMultiplayerMode);

    for (StageData& stage : stages) {
        if (static_cast<int>(stage.maps.size()) > mapCountPerStage) {
            stage.maps.resize(mapCountPerStage);
            continue;
        }

        if (static_cast<int>(stage.maps.size()) < mapCountPerStage) {
            const int missingMapCount = mapCountPerStage - static_cast<int>(stage.maps.size());
            std::vector<GeneratedMap> additionalMaps = createMapsForStage(mapWidth, mapHeight, missingMapCount);
            stage.maps.insert(stage.maps.end(), additionalMaps.begin(), additionalMaps.end());
        }
    }
}
} // namespace

int AppUI::run() {
    int stageCount = 1;
    int mapWidth = 3;
    int mapHeight = 2;
    bool isMultiplayerMode = false;
    std::vector<StageData> generatedStages;
    int currentStageIndex = 0;
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
        ImGui::TextUnformatted("Stage Count");
        ImGui::Separator();
        ImGui::InputInt("Stages to Generate", &stageCount);
        if (stageCount < 1) {
            stageCount = 1;
        }

        const int mapCountPerStage = getMapCountPerStage(isMultiplayerMode);
        ImGui::Text(
            "Each stage contains %d map(s) in %s mode.",
            mapCountPerStage,
            isMultiplayerMode ? "Multi" : "Single"
        );

        if (ImGui::Button("Start Making Stages")) {
            generationLogs.clear();
            generationLogs.push_back(
                "[INFO] Generating " + std::to_string(stageCount) + " stage(s)..."
            );

            generatedStages = createStages(stageCount, mapWidth, mapHeight, isMultiplayerMode);
            currentStageIndex = 0;

            generationLogs.push_back("[INFO] Done.");
            generationLogs.push_back(
                "[INFO] Created " + std::to_string(stageCount) +
                " stage(s), each with " + std::to_string(mapCountPerStage) + " map(s)."
            );
        }

        ImGui::Text(
            "Ready to generate %d stage(s). Current mode: %s (%d map(s) per stage).",
            stageCount,
            isMultiplayerMode ? "Multi" : "Single",
            mapCountPerStage
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
            syncStageMapCountToMode(generatedStages, mapWidth, mapHeight, isMultiplayerMode);

            if (!generatedStages.empty()) {
                currentStageIndex = std::clamp(currentStageIndex, 0, static_cast<int>(generatedStages.size()) - 1);
            } else {
                currentStageIndex = 0;
            }

            generationLogs.push_back(
                "[INFO] Mode switched to " +
                std::string(isMultiplayerMode ? "Multi" : "Single") +
                ". Each stage now has " + std::to_string(getMapCountPerStage(isMultiplayerMode)) +
                " map(s)."
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
        if (generatedStages.empty()) {
            ImGui::TextUnformatted("Press 'Start Making Stages' to create stages.");
        } else {
            constexpr float kTileSize = 28.0f;
            constexpr float kTileGap = 4.0f;
            constexpr float kMapPanelGap = 32.0f;

            currentStageIndex = std::clamp(currentStageIndex, 0, static_cast<int>(generatedStages.size()) - 1);
            StageData& currentStage = generatedStages[currentStageIndex];

            if (ImGui::Button("Prev Stage")) {
                currentStageIndex = std::max(0, currentStageIndex - 1);
            }
            ImGui::SameLine();
            if (ImGui::Button("Next Stage")) {
                currentStageIndex = std::min(static_cast<int>(generatedStages.size()) - 1, currentStageIndex + 1);
            }
            ImGui::SameLine();
            ImGui::Text("Stage %d / %zu", currentStageIndex + 1, generatedStages.size());

            ImGui::Separator();

            auto drawMapPanel = [&](const GeneratedMap& map, int mapNumberInStage) {
                ImGui::Text("Stage %d - Map %d", currentStageIndex + 1, mapNumberInStage);
                ImGui::Text("Tile Map (%d x %d)", map.width, map.height);
                ImGui::Separator();

                for (int row = 0; row < map.height; ++row) {
                    for (int col = 0; col < map.width; ++col) {
                        const int tileIndex = row * map.width + col;
                        ImGui::PushID((currentStageIndex * 1000000) + (mapNumberInStage * 100000) + tileIndex);
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
                if (!currentStage.maps.empty()) {
                    drawMapPanel(currentStage.maps[0], 1);
                }
                ImGui::EndChild();

                ImGui::SameLine(0.0f, kMapPanelGap);

                ImGui::BeginChild("MapPanelRight", ImVec2(panelWidth, 0), true);
                if (currentStage.maps.size() >= 2) {
                    drawMapPanel(currentStage.maps[1], 2);
                } else {
                    ImGui::TextUnformatted("No Map");
                }
                ImGui::EndChild();
            } else {
                if (!currentStage.maps.empty()) {
                    drawMapPanel(currentStage.maps[0], 1);
                } else {
                    ImGui::TextUnformatted("No Map");
                }
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
