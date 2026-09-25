#include <cmath>
#include <iostream>
#include "raylib.h"

constexpr double two_pie_three = (2 * 3.14159265358979323846264338327950288) / 3;
const int screenWidth = 800;
const int screenHeight = 450;
unsigned char CalculateSin(double time, double offset)
{
    double sinValue = sin(time + offset);
    return static_cast<unsigned char>(127.5 - (sinValue * 127.5));
}

Color HSVColor(double time)
{
    // double colorVal = abs(sin(time)) * 360;
    // double saturationVal = abs(sin(time));
    double colorVal = fmod(time * 60, 360);
    return ColorFromHSV(colorVal, 1.0, 1.0);
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Aadarsh's game");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        double time = GetTime();
        Color c = Color{CalculateSin(time, -two_pie_three), CalculateSin(time, 0), CalculateSin(time, two_pie_three), 90};
        Color HSV = HSVColor(time);
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(0, 0, screenWidth / 2.0, screenHeight, c);
        DrawRectangle(screenWidth / 2.0, 0, screenWidth / 2.0, screenHeight, HSV);
        EndDrawing();
    }
    CloseWindow(); // Cleans up the window
    return 0;
};