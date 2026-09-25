#include <raylib.h>
#include <iostream>
#include <cmath>

struct GameConfig
{
    float movement_speed = 4.0f; // THIS IS PER-SECOND
    int DEFAULT_FPS = 60;
    float FPS = 60;
};
GameConfig GAME_CONFIG;
struct ScreenConfig
{
    std::string window_name = "Aadarsh's Game";
    int width = 800;
    int height = 450;
};

ScreenConfig GLOBAL_SCREEN_CONFIG;
struct Circle
{
    int x;
    int y;
    int radius; // Units unknown
};
// bool UP_STATE, bool DOWN_STATE, bool LEFT_STATE, bool RIGHT_STATE
void movement_v1(Circle &CircleData)
{
    if (IsKeyDown(KEY_UP))
    {
        std::cout << "Moving UP" << std::endl;
        CircleData.y = CircleData.y - 2;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        std::cout << "Moving DOWN" << std::endl;
        CircleData.y = CircleData.y + 2;
    }
    if (IsKeyDown(KEY_LEFT))
    {
        std::cout << "Moving LEFT" << std::endl;
        CircleData.x = CircleData.x - 2;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        std::cout << "Moving RIGHT" << std::endl;
        CircleData.x = CircleData.x + 2;
    }
}

/*
Since the s = distance is fixed = 2
using Pythagoras theorem, the height we need to move in both axes = s * root(0.5)
*/
void movement_v2(Vector2 &CirclePos)
{
    // First we find the direction
    int x = 0, y = 0;
    bool UP = IsKeyDown(KEY_UP);
    bool DOWN = IsKeyDown(KEY_DOWN);
    bool RIGHT = IsKeyDown(KEY_RIGHT);
    bool LEFT = IsKeyDown(KEY_LEFT);

    if (UP && DOWN)
    {
        y = 0;
    }
    else if (UP)
    {
        y = -1; // Y decreses going UP
    }
    else if (DOWN)
    {
        y = 1; // y increases going down
    }

    if (LEFT && RIGHT)
    {
        x = 0;
    }
    else if (LEFT)
    {
        x = -1; // Y decreses going UP
    }
    else if (RIGHT)
    {
        x = 1; // y increases going down
    }
    // TO MAKE sure movement stays consistent across FPS
    if (x * y == 0)
    {
        CirclePos.x += (x * GAME_CONFIG.movement_speed);
        CirclePos.y += (y * GAME_CONFIG.movement_speed);
    }
    else
    {
        CirclePos.x += x * (GAME_CONFIG.movement_speed) * sqrt(0.5);
        CirclePos.y += y * (GAME_CONFIG.movement_speed) * sqrt(0.5);
    }
}

int main()
{
    // Since Origin is top left (0,0), the starting point has to be half of the window size - radius;
    Color CircleColor = Color{255, 255, 255, 255};
    Circle CircleData = Circle{GLOBAL_SCREEN_CONFIG.width / 2, GLOBAL_SCREEN_CONFIG.height / 2, 10};
    Vector2 CirclePos = Vector2{GLOBAL_SCREEN_CONFIG.width / 2.0f, GLOBAL_SCREEN_CONFIG.height / 2.0f};
    std::vector<Vector2> PointsVector;
    InitWindow(GLOBAL_SCREEN_CONFIG.width, GLOBAL_SCREEN_CONFIG.height, GLOBAL_SCREEN_CONFIG.window_name.c_str());
    SetTargetFPS(GAME_CONFIG.FPS);
    while (!WindowShouldClose())
    {
        // Older version - has the diagonal BUG will use Vector2 instead of creating a new type
        // movement_v1(CircleData);
        movement_v2(CirclePos);
        BeginDrawing();
        ClearBackground(DARKGRAY);
        // DrawCircle(CircleData.x, CircleData.y, CircleData.radius, CircleColor);
        PointsVector.push_back(CirclePos);
        DrawCircleV(CirclePos, CircleData.radius, CircleColor);
        DrawLineStrip(PointsVector.data(), PointsVector.size(), BLACK);
        EndDrawing();
    }
    return 0;
}