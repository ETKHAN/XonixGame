#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <sstream>

using namespace sf;

const int M = 25;
const int N = 40;
int grid[M][N] = {0};
int ts = 18;

enum GameState { MENU, PLAYING, GAMEOVER };
GameState gameState = MENU;

int score = 0;

struct Enemy {
    int x, y, dx, dy;
    void reset() {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
    }
    void move() {
        x += dx;
        if (grid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }
        y += dy;
        if (grid[y / ts][x / ts] == 1) { dy = -dy; y += dy; }
    }
};

void drop(int y, int x) {
    if (y < 0 || y >= M || x < 0 || x >= N) return; // ✅ prevent going outside grid
    if (grid[y][x] != 0) return; // ✅ only process empty area

    grid[y][x] = -1; // mark as visited
    drop(y - 1, x);
    drop(y + 1, x);
    drop(y, x - 1);
    drop(y, x + 1);
}

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(N * ts, M * ts), "Xonix");
    window.setFramerateLimit(60);

    Texture t1, t2, t3, t4;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");
    t4.loadFromFile("images/background.png");

    Font font;
    font.loadFromFile("images/arial.ttf");

    Text menuText, gameOverText, scoreText;
    menuText.setFont(font);
    menuText.setCharacterSize(28);
    menuText.setFillColor(Color::Yellow);
    menuText.setPosition(60, 60);
    menuText.setString("Select Difficulty:\n1 - Easy (2 enemies)\n2 - Medium (4 enemies)\n3 - Hard (6 enemies)\n\nESC - Exit");

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(28);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setPosition(50, 80);

    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, 10);

    Sprite sTile(t1), sEnemy(t3), sBackground(t4);
    sEnemy.setOrigin(20, 20);

    int x = 10, y = 0, dx = 0, dy = 0;
    float timer = 0, delay = 0.07;
    Clock clock;
    bool Game = false;

    Enemy enemies[10];
    int enemyCount = 0;

    float levelUpTimer = 0;

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;
        levelUpTimer +=time;


        if (20 < levelUpTimer) {
            for(int i = 0; i < enemyCount; i++){
                enemies[i].dx > 0 ? enemies[i].dx++ : enemies[i].dx--; 
                enemies[i].dy > 0 ? enemies[i].dy++ : enemies[i].dy--; 
            }

            levelUpTimer = 0;
        }



        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    if (gameState == MENU)
                        window.close();
                    else {
                        gameState = MENU;
                        Game = false;
                    }
                }
                if (gameState == MENU) {
                    if (e.key.code == Keyboard::Num1 || e.key.code == Keyboard::Num1) { enemyCount = 2; gameState = PLAYING; Game = true; levelUpTimer = clock.restart().asSeconds();}
                    if (e.key.code == Keyboard::Num2 || e.key.code == Keyboard::Num2) { enemyCount = 4; gameState = PLAYING; Game = true; levelUpTimer = clock.restart().asSeconds();}
                    if (e.key.code == Keyboard::Num3 || e.key.code == Keyboard::Num3) { enemyCount = 6; gameState = PLAYING; Game = true; levelUpTimer = clock.restart().asSeconds();}


                    if (Game) {
                        
                        for (int i = 0; i < M; i++)
                            for (int j = 0; j < N; j++)
                                grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
                        for (int i = 0; i < enemyCount; i++) enemies[i].reset();
                        x = 10; y = 0; dx = dy = 0;
                        score = 0;
                        
                    }
                }
            }
        }

        if (gameState == PLAYING && Game) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { dx = -1; dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Right)) { dx = 1; dy = 0; }
            if (Keyboard::isKeyPressed(Keyboard::Up)) { dx = 0; dy = -1; }
            if (Keyboard::isKeyPressed(Keyboard::Down)) { dx = 0; dy = 1; }

            if (timer > delay) {
                x += dx; y += dy;
                if (x < 0) x = 0;
                if (x > N - 1) x = N - 1;
                if (y < 0) y = 0;
                if (y > M - 1) y = M - 1;

                if (grid[y][x] == 2) Game = false;
                if (grid[y][x] == 0) { grid[y][x] = 2; score++; }
                
                timer = 0;
            }

            for (int i = 0; i < enemyCount; i++) enemies[i].move();

            if (grid[y][x] == 1) {
                dx = dy = 0;
                for (int i = 0; i < enemyCount; i++)
                    drop(enemies[i].y / ts, enemies[i].x / ts);

                for (int i = 0; i < M; i++)
                    for (int j = 0; j < N; j++) {
                        if (grid[i][j] == -1) grid[i][j] = 0;
                        if (grid[i][j] == 2) grid[i][j] = 1;
                    }
            }

            for (int i = 0; i < enemyCount; i++)
                if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2)
                    Game = false;

            if (!Game) gameState = GAMEOVER;
        }

        // DRAWING
        window.clear();
        window.draw(sBackground);

        if (gameState == MENU) {
            window.draw(menuText);
        } else {
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0) continue;
                    if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                    if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setPosition(j * ts, i * ts);
                    window.draw(sTile);
                }

            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(x * ts, y * ts);
            window.draw(sTile);

            for (int i = 0; i < enemyCount; i++) {
                sEnemy.setPosition(enemies[i].x, enemies[i].y);
                window.draw(sEnemy);
            }

            std::ostringstream ss;
            ss << "Score: " << score;
            scoreText.setString(ss.str());
            window.draw(scoreText);

            if (gameState == GAMEOVER) {
                std::ostringstream os;
                os << "Game Over!\nFinal Score: " << score << "\nPress ESC to return to menu.";
                gameOverText.setString(os.str());
                window.draw(gameOverText);
            }
        }

        window.display();
    }

    return 0;
}
