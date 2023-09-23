//
// Created by Christopher Gruber on 9/17/23.
//

#include <iostream>
#include "Game.h"

namespace battletank {
    void Game::initWindow() {
        this->window = new sf::RenderWindow(sf::VideoMode(800, 600), "Battle Tank", sf::Style::Close | sf::Style::Titlebar);
        this->window->setFramerateLimit(144);
        this->window->setVerticalSyncEnabled(false);
    }

    void Game::initPlayerTank() {
        this->playerTank = new PlayerTank();
    }

    void Game::initTextures() {
        this->textures["TANK"] = new sf::Texture();
        this->textures["TANK"]->loadFromFile("../Source/Resources/Textures/Tank.png");

        this->textures["TANK_SHELL"] = new sf::Texture();
        this->textures["TANK_SHELL"]->loadFromFile("../Source/Resources/Textures/TankShell.png");
    }

    Game::Game() {
        this->initWindow();
        this->initPlayerTank();
        this->initTextures();

        this->enemyTanks.push_back(new EnemyTank(400.f, 400.f));
    }

    Game::~Game() {
        delete this->window;
        delete this->playerTank;

        // Clean up textures
        for (auto &i : this->textures) {
            delete i.second;
        }

        // Clean up tank shells
        for (auto &i : this->tankShells) {
            delete i;
        }

        // Clean up enemy tanks
        for (auto &i : this->enemyTanks) {
            delete i;
        }
    }

    void Game::run() {
        while (this->window->isOpen()) {
            this->update();
            this->render();
        }
    }

    void Game::update() {
        this->updatePollEvents();
        this->updateInput();
        this->playerTank->update();
        this->updateTankShells();
        this->updateEnemyTanks();
    }

    void Game::render() {
        this->window->clear();

        this->playerTank->render(*this->window);

        for (auto *shell : this->tankShells) {
            shell->render(this->window);
        }

        for (auto *enemyTank : this->enemyTanks) {
            enemyTank->render(this->window);
        }

        this->window->display();
    }

    void Game::updatePollEvents() {
        sf::Event event{};

        while (this->window->pollEvent(event)) {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                this->window->close();
            }
        }
    }

    void Game::updateInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            this->playerTank->rotate(-1.f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            this->playerTank->rotate(1.f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            this->playerTank->move(0.f, -1.f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            this->playerTank->move(0.f, 1.f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && this->playerTank->canAttack()) {
            this->tankShells.push_back(new TankShell(this->textures["TANK_SHELL"], this->playerTank->getPosition().x, this->playerTank->getPosition().y, 0.f, -1.f, 3.f));
        }
    }

    void Game::updateTankShells() {
        unsigned counter = 0;

        for (auto *shell : this->tankShells) {
            shell->update();

            if (shell->isOutOfView(this->window)) {
                // Delete tank shell
                delete this->tankShells.at(counter);

                // Cull tank shell from vector as it is out of bounds
                this->tankShells.erase(this->tankShells.begin() + counter);

                --counter;
            }

            std::cout << this->tankShells.size() << " tank shells remaining." << std::endl;

            ++counter;
        }
    }

    void Game::updateEnemyTanks() {
        unsigned counter = 0;

        for (auto *enemyTank : this->enemyTanks) {
            enemyTank->update();

            if (enemyTank->canAttack()) {
                this->tankShells.push_back(new TankShell(this->textures["TANK_SHELL"], enemyTank->getPosition().x, enemyTank->getPosition().y, 0.f, 1.f, 3.f));
            }

            ++counter;
        }
    }
} // battletank