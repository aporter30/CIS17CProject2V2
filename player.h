/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   player.h
 * Author: ajporter
 *
 * Created on May 27, 2024, 6:33 PM
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>

using namespace std;

class Player {
public:
    string name;
    int balance;
    int currentBet;
    
    Player(string n, int b) : name(n), balance(b), currentBet(0) {}
    void placeBet();
    void winBet();
    void loseBet();
};


#endif /* PLAYER_H */
