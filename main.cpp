#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

// TODO: maybe wrap text && popup window for processing operations && [create, delete, copy, move, selection, rename,...] && more compact workflow && some title and status(especially statusbar bottom)

namespace fs = std::filesystem;
using namespace ftxui;

std::string getHome() {
    std::string home = std::getenv("HOME");
    return home;
}

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
  std::vector<std::string> list;
  std::string dirs;
  if (!isTextFile(name)) {
    return "not readable";
  } else if (fs::is_directory(name)) {
    for (const auto &entry : fs::directory_iterator(name)) {
      list.push_back(entry.path().filename().string());
    }
    for (const auto &i : list) {
      dirs += i + '\n';
    }
    return dirs;
  } else {
    try {
        std::ifstream file(name);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } catch(std::exception &e) {
        return e.what();
    }
  }
}

void load_list(std::vector<std::string> &list, bool show_hidden) {
  list.clear();
  for (const auto &entry :
       fs::directory_iterator(fs::current_path().string())) {
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
  auto screen = ScreenInteractive::Fullscreen();
  int selected = 0;
  std::vector<std::string> list;
  load_list(list, false);

  MenuOption option;
  option.entries_option.transform = [](EntryState state) {
    Element e = text(state.label);
    if (state.active) {
      if (fs::is_directory(state.label)) {
        e = text("\uea83 " + state.label) | color(Color::Yellow);
      } else {
        e = text("❯ " + state.label) | color(Color::Red);
      }
    } else {
      e = text("  " + state.label) | dim;
    }
    return e;
  };

  auto menu = Menu(&list, &selected, option);
  auto preview = Renderer([&] {
    return vbox(hbox(text("preview: ") | bold | color(Color::White),
                     text(list[selected])) |
                     color(Color::Red),
                     text(fs::current_path().relative_path().string()),
                separator() | color(Color::Yellow),
                text(readFile(list[selected])));
           });

  auto container = Container::Horizontal({menu, preview});
  auto app = CatchEvent(
      Renderer(container,
               [&] {
                 return hbox({
                     menu->Render() | color(Color::DodgerBlue1) | border |
                     size(WIDTH, GREATER_THAN, 20),
                     preview->Render() | border | flex
                 });
               }),
      [&](Event event) {
        if (event == Event::Character('q')) {
          screen.Exit();
          return true;
        }
        if (event == Event::Character('l')) {
            if (fs::is_directory(list[selected])) {
                fs::current_path(list[selected]);
                load_list(list, false);
                selected = 0;
            }
            return true;
        }
        if (event == Event::Character('h')) {
            fs::current_path(fs::path(fs::current_path()).parent_path());
            load_list(list, false);
            selected = 0;
            return true;
        }
        if (event == Event::Character('j')) {
            selected++;
            if (selected >= list.size()) {
                selected = 0;
            }
            return true;
        }
        if (event == Event::Character('k')) {
            selected--;
            if (selected == -1) {
                selected = list.size();
            }
            return true;
        }
        return false;
      });
  screen.Loop(app);
}
