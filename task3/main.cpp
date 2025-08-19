#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <optional>

// Game constants
const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE     = 20;
const int GRID_WIDTH    = WINDOW_WIDTH  / GRID_SIZE;
const int GRID_HEIGHT   = WINDOW_HEIGHT / GRID_SIZE;

// Direction enum
enum class Direction { Up, Down, Left, Right };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x = 0, int y = 0): x(x), y(y) {}
};

struct Food {
    int x, y;
    Food(int x = 0, int y = 0): x(x), y(y) {}
};

class SnakeGame {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    sf::Font font;
    bool fontLoaded;

    std::vector<SnakeSegment> snake;
    Food food;
    Direction currentDirection;
    Direction nextDirection;
    bool gameOver;
    bool paused;
    int score;
    float moveTimer;
    const float moveInterval = 0.2f;

    std::mt19937 gen;
    std::uniform_int_distribution<> xDist, yDist;

public:
    SnakeGame()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "SFML 3.0 Snake")
    , gen(std::random_device{}())
    , xDist(0, GRID_WIDTH - 1)
    , yDist(0, GRID_HEIGHT - 1)
    , currentDirection(Direction::Right)
    , nextDirection(Direction::Right)
    , gameOver(false)
    , paused(false)
    , score(0)
    , moveTimer(0.f)
    {
        window.setFramerateLimit(60);
        fontLoaded = font.openFromFile("arial.ttf");
        if (!fontLoaded)
            std::cout << "Font not found; running without text\n";
        initializeGame();
    }

    void initializeGame() {
        snake = {
            {GRID_WIDTH/2, GRID_HEIGHT/2},
            {GRID_WIDTH/2 - 1, GRID_HEIGHT/2},
            {GRID_WIDTH/2 - 2, GRID_HEIGHT/2}
        };
        generateFood();
        currentDirection = nextDirection = Direction::Right;
        gameOver = paused = false;
        score = 0;
        moveTimer = 0.f;
        clock.restart();
    }

    void generateFood() {
        bool ok = false;
        while (!ok) {
            food.x = xDist(gen);
            food.y = yDist(gen);
            ok = true;
            for (auto& s : snake) {
                if (s.x == food.x && s.y == food.y) {
                    ok = false;
                    break;
                }
            }
        }
    }

    void handleInput() {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>())
                window.close();
            if (auto kp = ev->getIf<sf::Event::KeyPressed>())
                handleKey(kp->scancode);
        }
    }

    void handleKey(sf::Keyboard::Scancode sc) {
        switch (sc) {
            case sf::Keyboard::Scancode::Escape:
                window.close(); break;
            case sf::Keyboard::Scancode::Space:
                if (gameOver) initializeGame();
                else paused = !paused;
                break;
            case sf::Keyboard::Scancode::Up:
            case sf::Keyboard::Scancode::W:
                if (currentDirection != Direction::Down)
                    nextDirection = Direction::Up;
                break;
            case sf::Keyboard::Scancode::Down:
            case sf::Keyboard::Scancode::S:
                if (currentDirection != Direction::Up)
                    nextDirection = Direction::Down;
                break;
            case sf::Keyboard::Scancode::Left:
            case sf::Keyboard::Scancode::A:
                if (currentDirection != Direction::Right)
                    nextDirection = Direction::Left;
                break;
            case sf::Keyboard::Scancode::Right:
            case sf::Keyboard::Scancode::D:
                if (currentDirection != Direction::Left)
                    nextDirection = Direction::Right;
                break;
            case sf::Keyboard::Scancode::R:
                if (gameOver) initializeGame();
                break;
            default:
                break;
        }
    }

    void update() {
        if (gameOver || paused) return;
        float dt = clock.restart().asSeconds();
        moveTimer += dt;
        if (moveTimer >= moveInterval) {
            moveTimer -= moveInterval;
            moveSnake();
        }
    }

    void moveSnake() {
        currentDirection = nextDirection;
        SnakeSegment head = snake.front();
        switch (currentDirection) {
            case Direction::Up:    head.y--; break;
            case Direction::Down:  head.y++; break;
            case Direction::Left:  head.x--; break;
            case Direction::Right: head.x++; break;
        }
        if (head.x < 0 || head.x >= GRID_WIDTH ||
            head.y < 0 || head.y >= GRID_HEIGHT) {
            gameOver = true; return;
        }
        for (auto& s : snake) {
            if (s.x == head.x && s.y == head.y) {
                gameOver = true; return;
            }
        }
        snake.insert(snake.begin(), head);
        if (head.x == food.x && head.y == food.y) {
            score += 10;
            generateFood();
        } else {
            snake.pop_back();
        }
    }

    void render() {
        window.clear(sf::Color::Black);
        drawSnake();
        drawFood();
        drawUI();
        window.display();
    }

    void drawSnake() {
        for (size_t i = 0; i < snake.size(); ++i) {
            sf::RectangleShape seg({float(GRID_SIZE-1), float(GRID_SIZE-1)});
            seg.setPosition({
                float(snake[i].x * GRID_SIZE),
                float(snake[i].y * GRID_SIZE)
            });
            seg.setFillColor(i == 0 ? sf::Color::Green : sf::Color(0,150,0));
            window.draw(seg);
        }
    }

    void drawFood() {
        sf::RectangleShape f({float(GRID_SIZE-1), float(GRID_SIZE-1)});
        f.setPosition({
            float(food.x * GRID_SIZE),
            float(food.y * GRID_SIZE)
        });
        f.setFillColor(sf::Color::Red);
        window.draw(f);
    }

    void drawUI() {
        if (!fontLoaded)
            return;

        // Score text
        sf::Text scoreText(font); // pass font to constructor
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24); // pixels
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition({10.f, 10.f});
        window.draw(scoreText);

        // Paused state
        if (paused && !gameOver) {
            sf::Text pausedText(font);
            pausedText.setString("PAUSED");
            pausedText.setCharacterSize(48);
            pausedText.setFillColor(sf::Color::Yellow);
            centerText(pausedText, WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
            window.draw(pausedText);

            sf::Text contText(font);
            contText.setString("Press SPACE to continue");
            contText.setCharacterSize(20);
            contText.setFillColor(sf::Color::White);
            centerText(contText, WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 60.f);
            window.draw(contText);
        }

        // Game over state
        if (gameOver) {
            sf::Text gameOverText(font);
            gameOverText.setString("GAME OVER!");
            gameOverText.setCharacterSize(48);
            gameOverText.setFillColor(sf::Color::Red);
            centerText(gameOverText, WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 60.f);
            window.draw(gameOverText);

            sf::Text finalScoreText(font);
            finalScoreText.setString("Final Score: " + std::to_string(score));
            finalScoreText.setCharacterSize(28);
            finalScoreText.setFillColor(sf::Color::White);
            centerText(finalScoreText, WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 10.f);
            window.draw(finalScoreText);

            sf::Text restartText(font);
            restartText.setString("Press SPACE or R to restart");
            restartText.setCharacterSize(20);
            restartText.setFillColor(sf::Color::White);
            centerText(restartText, WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f + 40.f);
            window.draw(restartText);
        }
    }



    void centerText(sf::Text& t, float x, float y) {
        auto bounds = t.getLocalBounds();
        float halfW = bounds.size.x * 0.5f;
        float halfH = bounds.size.y * 0.5f;
        // Use brace-init to pass a Vector2f
        t.setOrigin({ bounds.position.x + halfW,
                    bounds.position.y + halfH });
        t.setPosition({ x, y });
    }


    void run() {
        while (window.isOpen()) {
            handleInput();
            update();
            render();
        }
    }
};

int main() {
    try {
        SnakeGame game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
    return 0;
}
