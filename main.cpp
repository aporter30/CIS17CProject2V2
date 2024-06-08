/* 
 * File:   main.cpp
 * Author: ajporter
 *
 *      Created: May 21, 2024, 11:08 AM
 *      Purpose: Project 2 - Building on Project 1 Blackjack game with trees and
 *                           graphs concepts from past month
 */

//System Libraries
#include <iostream>
#include <iomanip>  //setprecision
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <vector>
#include <unordered_map>

#include "deck.h"
#include "player.h"
#include "gameStats.h"

using namespace std;

//Function prototypes
void prntTitle();
int factorial(int n);
int fibonacci(int n);
void quicksort(vector<Card>& cards, int low, int high);
int partition(vector<Card>& cards, int low, int high);
void inorderTraversal(Node* root);

int main() {
    Deck deck;
    Player player("John", 1000);
    GameStats stats;
    
    prntTitle();
    deck.displayRules();
    
    char playAgain;
    do {
        player.placeBet();
        
        list<Card> playerHand;
        list<Card> dealerHand;
        
        deck.dealInitialCards(playerHand, dealerHand);
        deck.displayInitialHands(playerHand, dealerHand);
        
        deck.playerTurn(playerHand);
        deck.dealerTurn(dealerHand, &Deck::calculateHandValue);
        
        deck.determineWinner(playerHand, dealerHand);
        deck.updateScore(player.balance, playerHand, dealerHand);
        
        if (deck.calculateHandValue(playerHand) > 21) {
            stats.recordLoss();
        } else if (deck.calculateHandValue(dealerHand) > 21 || deck.calculateHandValue(playerHand) > deck.calculateHandValue(dealerHand)) {
            stats.recordWin();
            player.winBet();
        } else {
            stats.recordLoss();
            player.loseBet();
        }
        
        cout << "Your current balance: $" << player.balance << endl;
        if(player.balance <= 0){
            cout<<"Pit Boss: Buddy, you lost all of your money. Get outta here!\n";
            cout<<"\nBLACKJACK STATS\n";
            break;
        }
        cout << "Do you want to play again? (y/n): ";
        cin >> playAgain;
    } while (playAgain == 'y' && player.balance > 0);
    
    stats.displayStats();
    deck.displayGoodbyeMessage();
    return 0;
}

void prntTitle() {
    cout << "WELCOME TO BLACKJACK\n";
    cout << " ___   ___\n";
    cout << "|A  | |10 |\n";
    cout << "| ♣ | | ♦ |\n";
    cout << "|__A| |_10|\n";
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void quicksort(vector<Card>& cards, int low, int high) {
    if (low < high) {
        int pi = partition(cards, low, high);
        quicksort(cards, low, pi - 1);
        quicksort(cards, pi + 1, high);
    }
}

int partition(vector<Card>& cards, int low, int high) {
    Card pivot = cards[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (cards[j].value < pivot.value) {
            i++;
            swap(cards[i], cards[j]);
        }
    }
    swap(cards[i + 1], cards[high]);
    return (i + 1);
}

void inorderTraversal(Node* root) {
    if (root == nullptr) return;
    inorderTraversal(root->left);
    cout << root->card.rank << " of " << root->card.suit << endl;
    inorderTraversal(root->right);
}
//PLAYER FUNCTIONS
// Function to place a bet
void Player::placeBet() {
    cout << name << ", you have $" << balance << ". Enter your bet amount: ";
    cin >> currentBet;
    while (currentBet > balance || currentBet <= 0) {
        cout << "Invalid bet amount. Enter a valid bet amount: ";
        cin >> currentBet;
    }
    balance -= currentBet;
}

// Function to win a bet
void Player::winBet() {
    balance += 2 * currentBet;
    currentBet = 0;
}

// Function to lose a bet
void Player::loseBet() {
    currentBet = 0;
}
//GAMESTATS FUNCTIONS
// Record a win
void GameStats::recordWin() {
    gamesPlayed++;
    gamesWon++;
}

// Record a loss
void GameStats::recordLoss() {
    gamesPlayed++;
    gamesLost++;
}

// Display game statistics
void GameStats::displayStats() const {
    cout << "Games Played: " << gamesPlayed << endl;
    cout << "Games Won: " << gamesWon << endl;
    cout << "Games Lost: " << gamesLost << endl;
    cout << "Win/Loss Percentage: "<<setprecision(4)<<gamesWon/gamesPlayed<<endl;
}
//DECK FUNCTIONS
Deck::Deck() {
    string suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };
    string vals[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    for (const auto& suit : suits) {
        for (const auto& value : vals) {
            Card card;
            card.suit = suit;
            card.rank = value;
            deck.push_back(card);
            remainingCards[card.rank + card.suit]++;
        }
    }
}
void Deck::shuffleDeck() {
    random_device rd;
    mt19937 rng(rd());
    list<Card> shuffledDeck;
    while (!deck.empty()) {
        auto it = next(deck.begin(), rng() % deck.size());
        shuffledDeck.splice(shuffledDeck.end(), deck, it);
    }
    deck.swap(shuffledDeck);
}
void Deck::dealInitialCards(list<Card>& playerHand, list<Card>& dealerHand) {
    shuffleDeck();

    playerHand.push_back(deck.front());
    usedCards.insert(deck.front().rank + deck.front().suit);
    deck.pop_front();
    updateRemainingCards(playerHand.back());
    playerDeck.push(playerHand.back());

    dealerHand.push_back(deck.front());
    usedCards.insert(deck.front().rank + deck.front().suit);
    deck.pop_front();
    updateRemainingCards(dealerHand.back());
    dealerDeck.push(dealerHand.back());

    playerHand.push_back(deck.front());
    usedCards.insert(deck.front().rank + deck.front().suit);
    deck.pop_front();
    updateRemainingCards(playerHand.back());
    playerDeck.push(playerHand.back());

    while (true) {
        const Card& dealerCard = deck.front();
        if (usedCards.find(dealerCard.rank + dealerCard.suit) == usedCards.end()) {
            dealerHand.push_back(dealerCard);
            usedCards.insert(dealerCard.rank + dealerCard.suit);
            deck.pop_front();
            updateRemainingCards(dealerHand.back());
            dealerDeck.push(dealerHand.back());
            break;
        }
        shuffleDeck();
    }
}
void Deck::updateRemainingCards(const Card& card) {
    remainingCards[card.rank + card.suit]--;
}
void Deck::displayInitialHands(const list<Card>& playerHand, const list<Card>& dealerHand) const {
    cout << "PLAYER'S HAND:" << endl;
    for (const auto& card : playerHand) {
        cout << card.rank << " of " << card.suit << endl;
    }
    cout << endl;

    cout << "DEALER'S HAND:" << endl;
    const Card& firstCard = dealerHand.front();
    cout << firstCard.rank << " of " << firstCard.suit << endl;
    cout << "[Hidden Card]" << endl;
}
void Deck::playerTurn(list<Card>& playerHand) {
    char choice;
    do {
        cout << "\nDo you want to hit (h) or stand (s)?" << endl;
        cin >> choice;
        cout<<"RESULT\n";
        cout << "=================================" << endl;
        if (choice == 'h') {
            playerHand.push_back(deck.front());
            deck.pop_front();
            updateRemainingCards(playerHand.back());

            cout << "PLAYER'S HAND:" << endl;
            for (const auto& card : playerHand) {
                cout << card.rank << " of " << card.suit << endl;
            }
            cout << endl;

            int handValue = calculateHandValue(playerHand);
            if (handValue > 21) {
                cout << "Player busts! Your hand value is over 21." << endl;
                break;
            }
        }
    } while (choice == 'h');
}

void Deck::dealerTurn(list<Card>& dealerHand, int (Deck::*calculateHandValue)(const list<Card>&) const) {
    cout << "DEALER'S HAND:" << endl;
    for (const auto& card : dealerHand) {
        cout << card.rank << " of " << card.suit << endl;
    }
    cout << endl;

    while ((this->*calculateHandValue)(dealerHand) < 17) {
        if (dealerDeck.empty()) {
            cout << "Dealer's deck is empty!" << endl;
            break;
        }

        dealerHand.push_back(dealerDeck.top());
        dealerDeck.pop();
        updateRemainingCards(dealerHand.back());

        cout << "Dealer hits and receives a card." << endl;
        cout << "Dealer's Hand:" << endl;
        for (const auto& card : dealerHand) {
            cout << card.rank << " of " << card.suit << endl;
        }
        cout << endl;

        break;
    }
}

bool Deck::isNumeric(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}

int Deck::calculateHandValue(const list<Card>& hand) const {
    int sum = 0;
    int numAces = 0;

    for (const auto& card : hand) {
        if (card.rank == "A") {
            numAces++;
        } else if (card.rank == "J" || card.rank == "Q" || card.rank == "K") {
            sum += 10;
        } else if (isNumeric(card.rank)) {
            sum += stoi(card.rank);
        }
    }

    for (int i = 0; i < numAces; ++i) {
        if (sum + 11 <= 21) {
            sum += 11;
        } else {
            sum += 1;
        }
    }

    return sum;
}

void Deck::determineWinner(const list<Card>& playerHand, const list<Card>& dealerHand) const {
    int playerValue = calculateHandValue(playerHand);
    int dealerValue = calculateHandValue(dealerHand);

    if (playerValue > 21) {
        cout << "Player busts! Dealer wins." << endl;
    } else if (dealerValue > 21) {
        cout << "Dealer busts! Player wins." << endl;
    } else {
        if (playerValue > dealerValue) {
            cout << "Player wins!" << endl;
        } else if (playerValue < dealerValue) {
            cout << "Dealer wins!" << endl;
        } else {
            cout << "It's a push! No one wins." << endl;
        }
    }
}

void Deck::updateScore(int& playerScore, const list<Card>& playerHand, const list<Card>& dealerHand) const {
    int playerValue = calculateHandValue(playerHand);
    int dealerValue = calculateHandValue(dealerHand);

    if (playerValue > 21 || (playerValue < dealerValue && dealerValue <= 21)) {
        cout << "Player loses this round." << endl;
        playerScore--;
    } else if ((dealerValue > 21 || playerValue > dealerValue) && playerValue <= 21) {
        cout << "Player wins this round!" << endl;
        playerScore++;
    } else {
        cout << "It's a tie! No change in score." << endl;
    }

    cout << "Player's score: " << playerScore << endl;
}

bool Deck::compareCards(const Card& card1, const Card& card2) {
    int value1 = card1.rank == "A" ? 1 : (isNumeric(card1.rank) ? stoi(card1.rank) : 10);
    int value2 = card2.rank == "A" ? 1 : (isNumeric(card2.rank) ? stoi(card2.rank) : 10);
    return value1 < value2;
}

void Deck::printDeck() const {
    for (const auto& card : deck) {
        cout << card.rank << " of " << card.suit << endl;
    }
}
// Welcome message to greet the player
void Deck::displayRules() {
    cout << "=================================" << endl;
    cout << "Rules:" << endl;
    cout << "1. Try to get as close to 21 without going over." << endl;
    cout << "2. Face cards are worth 10. Aces are worth 1 or 11." << endl;
    cout << "3. You start with two cards. The dealer has one card hidden." << endl;
    cout << "4. Type 'h' to hit (get another card)." << endl;
    cout << "5. Type 's' to stand (end your turn)." << endl;
    cout << "Good luck!" << endl << endl;
}

// Goodbye message to thank the player
void Deck::displayGoodbyeMessage() {
    cout << "Thank you for playing Blackjack!" << endl;
    cout << "Hope to see you again soon!" << endl;
}
