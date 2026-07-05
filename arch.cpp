#include "ftxui/component/app.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include <filesystem>
#include <fstream>
#include <ftxui/ftxui.hpp>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

// show list of files/directories
std::vector<std::string> showList() {
  std::vector<std::string> entries;
  for (const fs::directory_entry &entry :
       fs::directory_iterator(fs::current_path())) {
    entries.push_back(entry.path().filename().string());
  }
  return entries;
}

int main() {
  auto screen = ftxui::App::TerminalOutput();
  std::vector<std::string> entries = showList();
  int selected = 0;
  ftxui::MenuOption option;
  option.on_enter = screen.ExitLoopClosure();
  auto menu = ftxui::Menu(&entries, &selected, option);
  screen.Loop(menu);
  std::cout << "selected element: " << selected << std::endl;

  // create directory
  bool dircreatable = false;
  if (dircreatable)
    fs::create_directory("Hello");

  // create file
  std::ofstream file;
  bool creatable = false;
  if (creatable) {
    file.open("hellofile");
    file.close();
  }

  // remove file/directory
  bool removable = false;
  if (removable)
    std::remove("Hello");

  // change current working directory
  chdir("testdir");
  // show file/dir list

  return 0;
}
