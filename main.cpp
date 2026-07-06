#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

bool isTextFile(const fs::path &path) {
    char buffer[1024];
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cout << "file does not exist" << std::endl;
    }

    file.read(buffer, sizeof(buffer));
    for (std::streamsize i = 0; i < file.gcount(); i++) {
        if (buffer[i] == '\0') {
            return false;
        } 
    }
    return true;
}

std::string readFile(std::string name) {
    if (!isTextFile(name)) {
        return "not readable";
    } else {
        std::ifstream file(name);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}

namespace fs = std::filesystem;
using namespace ftxui;

void load_list(std::vector<std::string> &list, bool show_hidden) {
    list.clear();
    for (const auto &entry : fs::directory_iterator(fs::current_path().string())) {
        if (!show_hidden) {
            if (!entry.path().filename().string().starts_with(".")) {
                list.push_back(entry.path().filename().string());
            }
        } else {
            list.push_back(entry.path().filename().string());
        }
    }
}

int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    int selected = 0;
    std::vector<std::string> list;
    load_list(list, false);

    MenuOption option;
    option.entries_option.transform = [] (EntryState state) {
        Element e = text(state.label);
        if (state.active) {
            e = text("❯ " + state.label) | color(Color::DodgerBlue1);
        } else {
            e = text("  " + state.label) | dim;
        }
        return e;
    };

    auto menu = Menu(&list, &selected, option);
    auto preview = Renderer([&]{
            return vbox(hbox(text("preview: ")|bold | color(Color::White),
                   text(list[selected])) | color(Color::DodgerBlue1),
                   separator() | color(Color::DodgerBlue3),
                   text(readFile(list[selected])) | size(WIDTH, EQUAL, 84) | size(HEIGHT, EQUAL, 8)
            );
    });

    auto container = Container::Horizontal({menu, preview});
    auto app = Renderer(container, [&] {
            return hbox({
                    menu->Render()
                        | color(Color::DodgerBlue1) | border | size(HEIGHT, EQUAL, 10) | size(WIDTH, EQUAL, 16),
                    preview->Render()
                        | border | flex,
            });
    });
    screen.Loop(app);
}
