#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

class Map
{
public:
    int width;
    int height;

    std::vector<std::vector<int>> grid;
    std::mt19937 rng;

    Map(int w, int h)
        : width(w), height(h),
          grid(h, std::vector<int>(w, 0)),
          rng(std::random_device{}())
    {
    }

    void carveHorizontal(int x1, int x2, int y)
    {
        if (x1 > x2)
            std::swap(x1, x2);
        for (int x = x1; x <= x2; x++)
            grid[y][x] = 1; // chemin
    }

    void carveVertical(int y1, int y2, int x)
    {
        if (y1 > y2)
            std::swap(y1, y2);
        for (int y = y1; y <= y2; y++)
            grid[y][x] = 1; // chemin
    }

    void generatePath(int turns)
    {
        std::uniform_int_distribution<int> distY(2, height - 3);

        int x = 0;
        int y = distY(rng);

        int segment = width / (turns + 1);

        for (int i = 0; i < turns; i++)
        {
            int nx = (i + 1) * segment;
            int ny = distY(rng);

            carveHorizontal(x, nx, y);
            carveVertical(y, ny, nx);

            x = nx;
            y = ny;
        }

        carveHorizontal(x, width - 1, y);
    }

    void fillDecor()
    {
        std::uniform_int_distribution<int> decorDist(16, 31);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (grid[y][x] == 0)
                {
                    if (rng() % 3 == 0) // 1/3 de chance d’avoir du décor
                        grid[y][x] = decorDist(rng);
                }
            }
        }
    }

    int getTileMask(int x, int y)
    {
        // calcul uniquement pour le chemin 0–15
        if (grid[y][x] < 1 || grid[y][x] > 15)
            return 0;

        int mask = 0;
        if (y > 0 && grid[y - 1][x] >= 1 && grid[y - 1][x] <= 15)
            mask |= 1;
        if (x < width - 1 && grid[y][x + 1] >= 1 && grid[y][x + 1] <= 15)
            mask |= 2;
        if (y < height - 1 && grid[y + 1][x] >= 1 && grid[y + 1][x] <= 15)
            mask |= 4;
        if (x > 0 && grid[y][x - 1] >= 1 && grid[y][x - 1] <= 15)
            mask |= 8;

        return mask;
    }

    void print()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (grid[y][x] >= 1 && grid[y][x] <= 15)
                    std::cout << "#"; // chemin
                else if (grid[y][x] >= 16)
                    std::cout << "*"; // décor
                else
                    std::cout << "."; // vide
            }
            std::cout << "\n";
        }
    }

    void printTiles()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int mask = getTileMask(x, y);
                if (mask > 0)
                    std::cout << mask << " ";
                else
                    std::cout << grid[y][x] << " ";
            }
            std::cout << "\n";
        }
    }
};

#include <map>
#include "raylib.h"

/*int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    //  Faudra trouver la taille adaptée
    Map map(50, 30);
    map.generatePath(10);
    map.fillDecor();

    map.print();
    map.printTiles();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make the window resizable
    InitWindow(screenWidth, screenHeight, "raylib [textures] example - tiled drawing");

    // Coordinates for all patterns inside the texture
    const Rectangle dirtPattern[] =
        {
            {0 * 16, 0 * 16, 16, 16},
            {3 * 16, 2 * 16, 16, 16}, //  Up
            {0 * 16, 3 * 16, 16, 16}, //  Down
            {4 * 16, 3 * 16, 16, 16}, //  Corner Down-Right
            {2 * 16, 4 * 16, 16, 16}, //  Right
            {3 * 16, 1 * 16, 16, 16}, //  Vertical
            {4 * 16, 0 * 16, 16, 16}, //  Down-Right
            {4 * 16, 4 * 16, 16, 16}, //  Down-Right
            {2 * 16, 3 * 16, 16, 16}, //  Down
            {7 * 16, 3 * 16, 16, 16}, //  Corner Down-Left
            {1 * 16, 3 * 16, 16, 16}, //  Horizontal (Left-Right)
            {8 * 16, 3 * 16, 16, 16}, //  Corner Up-Right
            {7 * 16, 0 * 16, 16, 16}, //  Corner Up-Left
            {7 * 16, 4 * 16, 16, 16}, //  Corner Down-Right
            {8 * 16, 0 * 16, 16, 16}, //  Corner Down-Left
            {8 * 16, 4 * 16, 16, 16}, //  Intersection (cross)
        };

    Texture Dirt = LoadTexture("../assets/Tilled_Dirt.png");
    Texture Grass = LoadTexture("../assets/Grass.png");

    // Coordinates for all patterns inside the texture
    const std::map<int, Rectangle> grassPattern =
        {
            {0, {1 * 16, 1 * 16, 16, 16}},
            {16, {1 * 16, 1 * 16, 16, 16}},
            {17, {1 * 16, 5 * 16, 16, 16}},
            {18, {2 * 16, 5 * 16, 16, 16}},
            {19, {3 * 16, 5 * 16, 16, 16}},
            {20, {4 * 16, 5 * 16, 16, 16}},
            {21, {5 * 16, 5 * 16, 16, 16}},
            {22, {0 * 16, 6 * 16, 16, 16}},
            {23, {1 * 16, 6 * 16, 16, 16}},
            {24, {2 * 16, 6 * 16, 16, 16}},
            {22, {3 * 16, 6 * 16, 16, 16}},
            {23, {4 * 16, 6 * 16, 16, 16}},
            {24, {5 * 16, 6 * 16, 16, 16}},
            {25, {5 * 16, 6 * 16, 16, 16}},
            {26, {5 * 16, 6 * 16, 16, 16}},
            {27, {5 * 16, 6 * 16, 16, 16}},
            {28, {5 * 16, 6 * 16, 16, 16}},
            {29, {5 * 16, 6 * 16, 16, 16}},
            {30, {5 * 16, 6 * 16, 16, 16}},
            {31, {5 * 16, 6 * 16, 16, 16}},
        };

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLANK);

        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();

        float width = screenWidth / map.width;
        float height = screenHeight / map.height;

        for (int y = 0; y < map.height; y++)
        {
            for (int x = 0; x < map.width; x++)
            {
                Rectangle destRec =
                    {
                        x * width,
                        y * height,
                        width,
                        height,
                    };

                //  Modern cpp ta vu ou koi
                if (int mask = map.getTileMask(x, y); mask > 1)
                {
                    DrawTexturePro(Grass, grassPattern.at(0), destRec, {0.0f, 0.0f}, 0.0f, BEIGE);
                    DrawTexturePro(Dirt, dirtPattern[mask], destRec, {0.0f, 0.0f}, 0.0f, WHITE);
                }
                else
                {
                    DrawTexturePro(Grass, grassPattern.at(map.grid[y][x]), destRec, {0.0f, 0.0f}, 0.0f, BEIGE);
                }
            }
        }

        DrawFPS(10, 10);

        EndDrawing();
    }
}*/
