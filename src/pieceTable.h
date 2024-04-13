#pragma once
#include <vector>
#include "utils.hpp"

enum PieceKind {
    Square = 0,
    Straight,
    L,
    L_Mirr,
    N,
    N_Mirr,
    T
};

typedef std::vector<std::vector<char>> PieceShape; // using references (&) intead of pointers (*), so it still behaves like a object (aka it's a pointer that can be used like a normal object) (https://youtu.be/IzoFn3dfsPA)

static PieceShape Pieces[7] =
    {
        // Square
        {
            {{1, 1},
             {1, 1}}},

        // I
        {
            {{2}, // IDK why but its laying on its side
             {2},
             {2},
             {2}}},

        // L
        {
            {{0, 0, 3},
             {3, 3, 3}}},

        // L mirrored
        {
            {{4, 0, 0},
             {4, 4, 4}}},

        // N
        {
            {{5, 5, 0},
             {0, 5, 5}}},

        // N mirrored
        {
            {{0, 6, 6},
             {6, 6, 0}}},

        // T
        {
            {{0, 7, 0},
             {7, 7, 7}}}};

constexpr static Vec2 PiecesCenters[7] = { // for rotating
    {0, 0},//  Square
    {2, 0},//  I
    {1, 1},//  L
    {1, 1},//  L mirr
    {1, 1},//  N
    {1, 1},//  N mirr
    {1, 1} //  T
};