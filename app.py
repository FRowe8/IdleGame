import os

project_structure = {
    "data/balance": ["generators.json", "upgrades.json", "timelines.json", "prestige.json"],
    "data/ui/styles": ["main.rcss"],
    "data/ui": ["main_screen.rml", "timeline_view.rml", "prestige_screen.rml"],
    "data/localization": ["en_US.json"],
    "src/core": ["CMakeLists.txt", "GameState.h", "GameState.cpp", "TickSystem.h", "TickSystem.cpp", "BigNumber.h", "BigNumber.cpp", "TimeBank.h", "TimeBank.cpp", "ParadoxEngine.h", "ParadoxEngine.cpp", "ResourceManager.h", "ResourceManager.cpp", "PrestigeSystem.h", "PrestigeSystem.cpp"],
    "src/core/serialization": ["ISerializable.h", "JsonAdapter.h", "JsonAdapter.cpp", "BigNumberConverter.h", "BigNumberConverter.cpp"],
    "src/core/events": ["Event.h", "GameEvents.h"],
    "src/bridge": ["CMakeLists.txt", "EventBus.h", "EventBus.cpp", "CommandRouter.h", "CommandRouter.cpp", "DataBindingRegistry.h", "DataBindingRegistry.cpp", "UpdateThrottle.h", "UpdateThrottle.cpp"],
    "src/view": ["CMakeLists.txt", "ScreenManager.h", "ScreenManager.cpp", "RmlUiContext.h", "RmlUiContext.cpp", "DataModelBinder.h", "DataModelBinder.cpp"],
    "src/view/custom_elements": ["TimelineCanvas.h", "TimelineCanvas.cpp", "ParadoxMeter.h", "ParadoxMeter.cpp"],
    "src/platform": ["CMakeLists.txt", "Window.h", "Window.cpp", "Input.h", "Input.cpp", "FileSystem.h", "FileSystem.cpp"],
    "src/platform/emscripten": ["EmscriptenLoop.h", "EmscriptenLoop.cpp"],
    "tests/core": ["test_BigNumber.cpp", "test_TimeBank.cpp", "test_ParadoxEngine.cpp", "test_TickSystem.cpp", "test_Serialization.cpp"],
    "tests/test_data": ["simple_economy.json"],
    "external": [".gitkeep"],
    "builds": []
}

def create_structure():
    base_path = os.getcwd()

    # Create Root Files
    open("CMakeLists.txt", 'a').close()
    open("README.md", 'a').close()
    open(".gitignore", 'a').close()
    open(os.path.join("src", "main.cpp"), 'a').close()

    for folder, files in project_structure.items():
        path = os.path.join(base_path, folder)
        os.makedirs(path, exist_ok=True)
        for file in files:
            open(os.path.join(path, file), 'a').close()

    print("✅ Project Structure Generated for 'Paradox Protocol'")

if __name__ == "__main__":
    create_structure()