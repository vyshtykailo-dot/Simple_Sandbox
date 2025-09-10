#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

const int WIDTH = 300;
const int HEIGHT = 200;
const int PIXEL_SIZE = 3;

enum class ParticleType {
    EMPTY,
    SAND,
    WATER,
    FIRE,
    GUNPOWDER,
    SEED,
    CACTUS
};

struct Particle {
    ParticleType type = ParticleType::EMPTY;
    sf::Color color = sf::Color::Black;
    int life = 0; // lifetime for particles like fire or seed
};

std::vector<std::vector<Particle>> grid(WIDTH, std::vector<Particle>(HEIGHT));

Particle createParticle(ParticleType type) {
    Particle p;
    p.type = type;
    switch (type) {
        case ParticleType::SAND: p.color = sf::Color(194, 178, 128); break;
        case ParticleType::WATER: p.color = sf::Color::Blue; break;
        case ParticleType::FIRE: p.color = sf::Color::Red; p.life = 65; break;
        case ParticleType::GUNPOWDER: p.color = sf::Color(80, 80, 80); break;
        case ParticleType::SEED: p.color = sf::Color::Green; break;
        case ParticleType::CACTUS: p.color = sf::Color(0, 150, 0); break;
        default: p.color = sf::Color::Black; break;
    }
    return p;
}

bool inBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void updateParticle(int x, int y) {
    Particle& p = grid[x][y];
    if (p.type == ParticleType::SAND) {
        if (inBounds(x, y + 1) && grid[x][y + 1].type == ParticleType::EMPTY) {
            std::swap(p, grid[x][y + 1]);
        }
    } else if (p.type == ParticleType::WATER) {
        int dx[] = {0, -1, 1};
        int r = rand() % 3;
        int nx = x + dx[r];
        int ny = y + 1;
        if (inBounds(nx, ny) && grid[nx][ny].type == ParticleType::EMPTY) {
            std::swap(p, grid[nx][ny]);
        }
    } else if (p.type == ParticleType::FIRE) {
        p.life--;
        if (p.life <= 0) {
            p = createParticle(ParticleType::EMPTY);
        } else {
            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};
            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                if (inBounds(nx, ny)) {
                    if (grid[nx][ny].type == ParticleType::GUNPOWDER) {
                        grid[nx][ny] = createParticle(ParticleType::FIRE);
                    }
                }
            }
        }
    } else if (p.type == ParticleType::SEED) {
        if (inBounds(x, y + 1) && grid[x][y + 1].type == ParticleType::EMPTY) {
            std::swap(p, grid[x][y + 1]);
        }
        else if (inBounds(x, y + 1) && grid[x][y + 1].type == ParticleType::SAND) {
            // Grow cactus up from the seed position
            int height = 4 + (rand() % 11); // random height between 4 and 14
            for (int i = 0; i < height; ++i) {
                int ny = y - i;
                if (inBounds(x, ny) && (grid[x][ny].type == ParticleType::EMPTY || grid[x][ny].type == ParticleType::SEED)) {
                    grid[x][ny] = createParticle(ParticleType::CACTUS);
                } else {
                    break;
                }
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE), "SANDBOX");
    ParticleType currentType = ParticleType::SAND;

    // Load font for UI text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font arial.ttf" << std::endl;
        // Optionally, you can exit or continue without UI text
    }

    sf::Text uiText;
    uiText.setFont(font);
    uiText.setCharacterSize(16);
    uiText.setFillColor(sf::Color::White);
    uiText.setPosition(5, 5);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) currentType = ParticleType::SAND;
                if (event.key.code == sf::Keyboard::Num2) currentType = ParticleType::WATER;
                if (event.key.code == sf::Keyboard::Num3) currentType = ParticleType::FIRE;
                if (event.key.code == sf::Keyboard::Num4) currentType = ParticleType::GUNPOWDER;
                if (event.key.code == sf::Keyboard::Num5) currentType = ParticleType::SEED;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i pos = sf::Mouse::getPosition(window);
            int gx = pos.x / PIXEL_SIZE;
            int gy = pos.y / PIXEL_SIZE;
            if (inBounds(gx, gy)) {
                grid[gx][gy] = createParticle(currentType);
            }
        }

        // Update particles from bottom to top
        for (int y = HEIGHT - 1; y >= 0; --y) {
            for (int x = 0; x < WIDTH; ++x) {
                updateParticle(x, y);
            }
        }

        // Update UI text with current particle type
        std::string typeName;
        switch (currentType) {
            case ParticleType::SAND: typeName = "Sand"; break;
            case ParticleType::WATER: typeName = "Water"; break;
            case ParticleType::FIRE: typeName = "Fire"; break;
            case ParticleType::GUNPOWDER: typeName = "Gunpowder"; break;
            case ParticleType::SEED: typeName = "Seed"; break;
            case ParticleType::CACTUS: typeName = "Cactus"; break;
            default: typeName = "Empty"; break;
        }
        uiText.setString("Current: " + typeName);

        window.clear();

        // Draw grid particles
        sf::RectangleShape pixel(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
        for (int x = 0; x < WIDTH; ++x) {
            for (int y = 0; y < HEIGHT; ++y) {
                pixel.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
                pixel.setFillColor(grid[x][y].color);
                window.draw(pixel);
            }
        }

        // Draw UI text
        window.draw(uiText);

        window.display();
    }

    return 0;
}
