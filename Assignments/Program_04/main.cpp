#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>    // STDOUT_FILENO
#include <sys/ioctl.h> // ioctl, winsize
#endif

#include "AutomatonUtils.hpp"
#include "ConwayLife.hpp"
#include "Screen.hpp"
#include "argsToJson.hpp"
#include "json.hpp"
#include "sdlscreen.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>


using namespace std;
using nlohmann::json;


// Gets the terminal width
// Input: No parameters
// Returns: int
termSize getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) 
    {
        termSize x;
		x.row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		x.col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return x;
    }
    return { 24, 80}; // fallback for typical termainal size
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return { w.ws_row, w.ws_col };
    }
	return{ 24, 80 }; // fallback for typical termainal size
#endif
}

// Prints the terminal width
// Input: No parameters
// Returns: void
void printTerminalWidth() {
#ifdef _WIN32
    // Windows code
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        std::cout << "Terminal rows:    "
            << csbi.srWindow.Bottom - csbi.srWindow.Top + 1
            << std::endl;
        std::cout << "Terminal columns: "
            << csbi.srWindow.Right - csbi.srWindow.Left + 1
            << std::endl;
    }
#else
    // Linux/macOS code
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        std::cout << "Terminal rows:    " << w.ws_row << std::endl;
        std::cout << "Terminal columns: " << w.ws_col << std::endl;
    }
    else {
        std::cerr << "Error getting terminal size." << std::endl;
    }
#endif
}

// --------------------------------------------------------------
// Default simulation parameters.
// These values are applied *only if* the user does not provide
// command-line overrides of the form key=value.
// --------------------------------------------------------------
json defaults = {{"width", 800}, {"height", 600}, {"generations", 1000}, {"cellSize", 10}, {"frameDelayMs", 500}};

int main(int argc, char* argv[]) {


    // ----------------------------------------------------------
    // Parse command-line arguments into a JSON object.
    // ArgsToJson expects arguments like:
    //     width=500 height=300 frameDelayMs=20
    //
    // This keeps your parameter-handling clean and extensible.
    // ----------------------------------------------------------
    json params = ArgsToJson(argc, argv);

    // ----------------------------------------------------------
    // Merge defaults: if a parameter wasn't specified by the user,
    // use the value from `defaults`.
    //
    // Result:
    //   - user-provided params overwrite defaults
    //   - missing keys fall back to defaults
    // ----------------------------------------------------------
    for (auto& [key, value] : defaults.items()) {
        if (params.find(key) == params.end()) {
            params[key] = value;
        }
    }

    cout << "Simulation Parameters:\n"
         << params.dump(4)  // pretty-printed JSON
         << endl;

    // ----------------------------------------------------------
    // Determine the terminal window size.
    // Using ioctl() with TIOCGWINSZ retrieves:
    //   - number of character rows (w.ws_row)
    //   - number of character columns (w.ws_col)
    //
    // This allows the simulation to scale to the user's terminal.
    // ----------------------------------------------------------

    struct termSize w = getTerminalWidth();

    // ----------------------------------------------------------
    // TextScreen implements the Screen interface using simple
    // Unicode/ASCII-based rendering in the terminal.
    // ----------------------------------------------------------
    //TextScreen txtscr;

    // ----------------------------------------------------------
    // Construct a ConwayLife automaton based on available space.
    //
    // Terminal columns are divided by 2 because each rendered cell
    // uses two characters ("o" or two spaces). Without dividing,
    // the grid would overflow horizontally.
    //
    // Subtracting 1 row prevents scrolling due to newline behavior.
    // ----------------------------------------------------------
    ConwayLife gol(w.row - 1, w.col / 2);

    sdlscreen txtscr(w, gol);
	txtscr.runSDL();
    


    return 0;
}