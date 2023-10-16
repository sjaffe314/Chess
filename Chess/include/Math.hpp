#pragma once
#include <iostream>

#define CLAMP(i, min, max) ((i < min) ? min : ((i > max) ? max : i))

struct Vector2i
{
    Vector2i(int p_x = 0, int p_y = 0)
        : x(p_x), y(p_y)
    {
    }

    void print()
    {
        std::cout << x << ",  " << y << std::endl;
    }

    Vector2i operator-(int p_sub) { return {this->x - p_sub, this->y - p_sub}; }

    Vector2i operator-(Vector2i p_sub) { return {this->x - p_sub.x, this->y - p_sub.y}; }

    Vector2i operator+(int p_add) { return {this->x + p_add, this->y + p_add}; }

    Vector2i operator+(Vector2i p_add) { return {this->x + p_add.x, this->y + p_add.y}; }

    Vector2i operator/(int p_divisor) { return { this->x / p_divisor, this->y / p_divisor }; }

    Vector2i clamped(Vector2i min, Vector2i max) { return {CLAMP(this->x, min.x, max.x), CLAMP(this->y, min.y, max.y), }; }

    int x, y;
};

struct Vector2f
{
    Vector2f(float p_x = 0.0f, float p_y = 0.0f)
        : x(p_x), y(p_y)
    {
    }

    void print()
    {
        std::cout << x << ",  " << y << std::endl;
    }

    float x, y;
};