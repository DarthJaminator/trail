#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string readFile(std::string name) {
    std::string buffer;
    std::ifstream read(name);

    while(std::getline(read, buffer)) {
        std::cout << buffer;
    }
    return buffer;
}

namespace fs = std::filesystem;
using namespace ftxui;

void load_list(std::vector<std::string> &list) {
    list.clear();
    for (const auto &entry : fs::directory_iterator(fs::current_path().string())) {
        list.push_back(entry.path().filename().string());
    }
}
int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    int selected = 0;
    std::vector<std::string> list;
    load_list(list);
    auto menu = Menu(&list, &selected);
    auto preview = Renderer([&]{
            return vbox(text("preview")|bold,
                   text(list[selected]) | bold,
                   text(readFile(list[selected]))
            );
    });
    auto container = Container::Horizontal({menu, preview});
    auto app = Renderer(container, [&] {
            return hbox({
                    menu->Render() | color(Color::Orange3)| border | size(HEIGHT, EQUAL, 10) | size(WIDTH, EQUAL, 16),
                    preview->Render() | border | flex,
            });
    });
    screen.Loop(app);
}
