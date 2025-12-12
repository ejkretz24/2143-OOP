#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "CellularAutomaton.hpp"

using json = nlohmann::json;

using namespace std;

struct termSize 
{
    int row;
    int col;
};

class sdlscreen
{
    private:
        ifstream f; 
        json data;
        int cellSize = 20;
        int windowHeight;
        int windowWidth;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        CellularAutomaton& grid;
        int frameRate;
        bool paused;


    public:

    sdlscreen(termSize a, CellularAutomaton& b) : f("patterns.json"), grid(b) 
    { 
            if (!f.is_open()) 
            {
                std::cerr << "Failed to open patterns.json, skipping JSON load.\n";
            }
            else 
            {
                data = json::parse(f);
                f.close();
            }
        
        //Converting integers to pixel sizes
        windowHeight = a.row * cellSize;
        windowWidth = a.col * cellSize;
        frameRate = 16;
        paused = false;
    };


    //verifies the sdl window is open
    //parameters: none
    //returns: int
    int windowVerify()
    {
        if (!window) 
        {
            std::cerr << "Window Error: " << SDL_GetError() << "\n";
            SDL_Quit();  // Clean up SDL before exiting
            return 1;
        }
        return 0;
    }

    //function for running the entirety of the Sdl window
    //parameters: none
    //returns: int
     int runSDL()
    {
          // ------------------------------------------------------------
    // INITIALIZE SDL
    // ------------------------------------------------------------
    // SDL_Init starts the requested SDL subsystems.
    // SDL_INIT_VIDEO allows us to create a window and draw graphics.

        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
            return 1;  // Return non-zero to indicate an error
        }
    

    // ------------------------------------------------------------
    // CREATE THE WINDOW
    // ------------------------------------------------------------
    // SDL_CreateWindow opens a visible OS-level window.
    // Parameters:
    //   - Title text
    //   - x, y screen position (SDL_WINDOWPOS_CENTERED lets SDL decide)
    //   - Width and height in pixels
    //   - Flags (SDL_WINDOW_SHOWN = visible on creation)

    window = SDL_CreateWindow(
        "SDL2 Grid Example",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );

    // Verify the window was successfully created
    windowVerify();

    // ------------------------------------------------------------
    // CREATE A RENDERER
    // ------------------------------------------------------------
    // The renderer handles drawing operations on the window.
    // SDL_RENDERER_ACCELERATED tells SDL to use GPU acceleration.
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) 
        {
            std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    

    // ------------------------------------------------------------
    // MAIN LOOP
    // ------------------------------------------------------------
    // "running" flag controls the lifetime of the program.
    bool running = true;
    SDL_Event event;  // Struct that holds event information (keyboard, mouse, quit, etc.)


    while (running) 
    {
        // --------------------------------------------------------
        // EVENT HANDLING
        // --------------------------------------------------------
        // SDL_PollEvent() pulls events from the event queue.
        // This loop checks for any pending events, e.g. user clicking "X" to close.
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT)// Window close event
            {  
                running = false;         // Exit the main loop
            }

            //if else to check for all keydown events
            if (event.type == SDL_KEYDOWN) 
            {
                //saving key that was pressed
                SDL_Keycode key = event.key.keysym.sym;  
                        
                //keymod state for the shift + = (to retrieve +)
                SDL_Keymod mod = SDL_GetModState();      
                        
                //if escape was pressed put running to false to exit
                if (key == SDLK_ESCAPE)
                {
                    running = false;
                }
                        
                    //if space was pressed, change bool paused to opposite and cout correspoinding
                    if (key == SDLK_SPACE) 
                    {  // space bar toggles pause
                        paused = !paused;
                        std::cout << (paused ? "Paused\n" : "Resumed\n");
                    }

                    //if r was pressed call randomize function 
                    if (key == SDLK_r) 
                    {
                        grid.randomize(.20);
                    }

                    //if c is pressed call clear gris function
                    if (key == SDLK_c) 
                    {
                        grid.clearGrid();
                    }

                    //if the plus sign is pressed increase the frame rate
                    if (key == SDLK_EQUALS && (mod & KMOD_SHIFT)) 
                    {
                        frameRate = frameRate - 2;
                        if (frameRate <= 0)
                        {
                            frameRate = 0;
                        }
                    }         

                    //if minus is pressed decrease the framerate
                    if (key == SDLK_MINUS) 
                    {
                        frameRate = frameRate + 2;
                    }

                    //if g is pressed load a glider shape from json file to center of the grid
                    if (key == SDLK_g) 
                    {
                        std::string patternName = "glider";
                        if (data.contains(patternName)) 
                        {
                            auto& pattern = data[patternName];
                            auto& cells = pattern["cells"];
                            int w = pattern["size"]["w"];
                            int h = pattern["size"]["h"];

                            //get the start position
                            int startRow = (grid.getRows() - h) / 2;
                            int startCol = (grid.getCols() - w) / 2;

                            //clears out a space in the middle for the glider
                            for (int r = 0; r < h; ++r)
                            {
                                for (int c = 0; c < w; ++c)
                                {
                                    grid.setCellShape(startRow + r, startCol + c, 0);
                                }
                            }

                            //load the gloder into the grid
                            for (auto& cell : cells)
                            {
                                int r = startRow + cell["y"].get<int>();
                                int c = startCol + cell["x"].get<int>();
                                grid.setCellShape(r, c, 1);
                            }
                        }

                    }
    
                }

            //event type to check for mousle clicks
            if (event.type == SDL_MOUSEBUTTONDOWN) 
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                //Convert pixel coords to cell coords
                int cellX = mouseX / cellSize;
                int cellY = mouseY / cellSize;
                
                //calls function to toggle specific clicked cell
                grid.toggleCell(cellY, cellX);

                std::cout << "Clicked cell: " << cellX << ", " << cellY << std::endl;
            }
        };

        //step only if not paused
        if (!paused)
        {
            grid.step();
        }
        // --------------------------------------------------------
        // CLEAR SCREEN
        // --------------------------------------------------------
        // Set the background color first (dark blue-gray here).
        // The format is RGBA, each component 0–255.
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
        SDL_RenderClear(renderer);

        // --------------------------------------------------------
        // DRAW GRID LINES
        // --------------------------------------------------------
        // Set the color for the grid lines (lighter gray).
        SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);

        // Draw vertical lines.
        // Start at x = 0 and go to windowWidth, stepping by cellSize each time.
        for (int x = 0; x <= windowWidth; x += cellSize) {
            SDL_RenderDrawLine(renderer, x, 0, x, windowHeight);
        };

        // Draw horizontal lines.
        // Start at y = 0 and go to windowHeight, stepping by cellSize each time.
        for (int y = 0; y <= windowHeight; y += cellSize) {
            SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
        };

        for (size_t r = 0; r < grid.getGrid().size(); r++) {
            for (size_t c = 0; c < grid.getGrid()[r].size(); c++) {
                if (grid.getGrid()[r][c]) {
                    SDL_Rect rect;
                    rect.x = c * cellSize;
                    rect.y = r * cellSize;
                    rect.w = cellSize;
                    rect.h = cellSize;

                    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255); // red
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // --------------------------------------------------------
        // SHOW THE RESULT
        // --------------------------------------------------------
        // Swap the off-screen buffer with the on-screen buffer.
        // Everything drawn since the last call to SDL_RenderPresent()
        // now becomes visible.
        SDL_RenderPresent(renderer);

        // --------------------------------------------------------
        // FRAME RATE LIMIT
        // --------------------------------------------------------
        // Delay ~16 ms to target roughly 60 frames per second.
        // (1000 ms / 60 ≈ 16.6 ms)
        SDL_Delay(frameRate);
    };

    // ------------------------------------------------------------
    // CLEANUP
    // ------------------------------------------------------------
    // Free SDL resources before exiting to avoid memory leaks.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

     };
};


