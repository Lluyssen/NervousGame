#include <iostream>
#include <vector>
#include <ctime>
#include <random>

#include "raylib.h"

class MapDataGen
{
public:
    int width = 0, height = 0;

    std::vector<std::vector<int>> grid;

    MapDataGen(int w, int h) : width(w), height(h)
    {
        grid.assign(height, std::vector<int>(width, 0));
    }

    void addRandomPath()
    {
        int x = 0;
        int y = rand() % (height - 2) + 1;

        while (x < width)
        {
            grid[y][x] = 1;

            // Marche aléatoire vers la droite
            int dir = rand() % 3;

            if (dir == 0 && y > 1)
            {
                --y; // Monte
            }
            else if (dir == 1 && y < height - 2)
            {
                ++y; // Descend
            }
            else
            {
                ++x; // Avance
            }
        }
    }

    // Calcule l'ID de texture (0-15 pour les 4 directions de base)
    int getTileID(int x, int y)
    {
        if (grid[y][x] == 0)
        {
            return 0;
        }

        int mask = 0;

        if (y > 0 && grid[y - 1][x] == 1)
            mask |= 1; // Haut
        if (x < width - 1 && grid[y][x + 1] == 1)
            mask |= 2; // Droite
        if (y < height - 1 && grid[y + 1][x] == 1)
            mask |= 4; // Bas
        if (x > 0 && grid[y][x - 1] == 1)
            mask |= 8; // Gauche

        return mask;
    }

    void generate(int paths)
    {
        for (int i = 0; i < paths; ++i)
        {
            addRandomPath();
        }
    }
};

int main()
{
    srand(time(0));

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    //  Faudra trouver la taille adaptée
    MapDataGen dungeon(screenWidth / 32, screenHeight / 32);
    dungeon.generate(3);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make the window resizable
    InitWindow(screenWidth, screenHeight, "raylib [textures] example - tiled drawing");

    // Coordinates for all patterns inside the texture
    const Rectangle recPattern[] =
        {
            {1 * 64, 3 * 64, 64, 64},
            {3 * 64, 3 * 64, 64, 64},
            {3 * 64, 0 * 64, 64, 64},
            {0 * 64, 4 * 64, 64, 64},
            {3 * 64, 2 * 64, 64, 64},
            {0 * 64, 2 * 64, 64, 64},
            {0 * 64, 6 * 64, 64, 64},
            {4 * 64, 0 * 64, 64, 64},
            {3 * 64, 1 * 64, 64, 64},
            {2 * 64, 4 * 64, 64, 64},
            {1 * 64, 1 * 64, 64, 64},
            {4 * 64, 3 * 64, 64, 64},
            {2 * 64, 1 * 64, 64, 64},
            {4 * 64, 1 * 64, 64, 64},
            {4 * 64, 2 * 64, 64, 64},
            {2 * 64, 0 * 64, 64, 64},
        };

    Texture test = LoadTexture("../assets/blue_castle.png");

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GREEN);

        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();

        float width = screenWidth / dungeon.width;
        float height = screenHeight / dungeon.height;

        for (int y = 0; y < dungeon.height; ++y)
        {
            for (int x = 0; x < dungeon.width; ++x)
            {
                Rectangle destRec =
                    {
                        x * width,
                        y * height,
                        width,
                        height,
                    };

                DrawTexturePro(test, recPattern[dungeon.getTileID(x, y)], destRec, {0.0f, 0.0f}, 0.0f, WHITE);
            }
        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    return 0;
}