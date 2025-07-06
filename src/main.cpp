#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

// Definindo o mapa:
#define COMPRIMENTO 400
#define LARGURA 420
#define PIXEL 16

const int mapSize = 25;
char map[mapSize][mapSize + 1] = {
    "111111111111T111111111111",
    "1000000000000000000000001",
    "1011110111110110110111101",
    "1010000100000000110100001",
    "1010110111110111110101101",
    "1000100001000001000000101",
    "1111111011111111111111111",
    "1000001000000000001000001",
    "1011111011110111111011101",
    "1000000010000000000000001",
    "1011111110111111011111101",
    "1000000000100001000000001",
    "1011111110110111011111101",
    "T00000000000000000000000T",
    "1111111110111111011111111",
    "1000000010000100000000001",
    "1011111011110111111011101",
    "1000001000000000001000001",
    "1111111011111110111111101",
    "1000110001000000000000101",
    "1010110111110111110101101",
    "1010000100000100000100001",
    "1011110111110111110111101",
    "1000000000000000000000001",
    "111111111111T111111111111"};

// Definindo contadores:
int coins = 0, totalCoins = 0;
int points = 0;
int lifes = 1;
int mushrooms = 0;

double interval = 0.2f, mushroomInterval = 0, starInterval = 0;

bool isMushroomAvailable = false;
bool drawRect = false;

// Definindo direções:
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Definindo entidade:
struct Entity
{
    int x, y;
    bool estrela;
    int prevX, prevY;
    sf::Texture texture, rightTexture, leftTexture, upTexture, downTexture, deathTexture;
    sf::Sprite sprite;
    Direction currentDirection, intentionDirection;
};

// Definindo funções:
void restartGame(Entity &player, Entity turtles[6], Entity estrelas[6], sf::Text &coinsText, int &coins, int &lifes, sf::Music &themeMusic, sf::Text &pointsText)
{
    // Reseta o Mario:
    player.x = 12;
    player.y = 11;
    player.estrela = false;
    player.prevX = player.x;
    player.prevY = player.y;
    player.sprite.setTexture(player.downTexture);
    player.currentDirection = player.intentionDirection = UP;

    // Restaura a vida:
    lifes = 1;

    // Reinicia os contadores:
    coins = points = 0;
    mushroomInterval = 0;

    for (int i = 0; i < mapSize; i++)
        for (int j = 0; j < mapSize; j++)
            if (map[i][j] == ' ')
                map[i][j] = '0';

    // Reinicia o cogumelo:
    isMushroomAvailable = false;

    // Inicia a posição das tartarugas:
    turtles[0].x = turtles[0].y = turtles[1].x = turtles[2].y = turtles[4].x = 1;
    turtles[1].y = turtles[2].x = turtles[3].x = turtles[3].y = turtles[5].x = 23;
    turtles[4].y = turtles[5].y = 12;

    turtles[0].currentDirection = DOWN;
    turtles[1].currentDirection = turtles[4].currentDirection = RIGHT;
    turtles[2].currentDirection = UP;
    turtles[3].currentDirection = turtles[5].currentDirection = LEFT;

    for (int i = 0; i < 6; i++)
    {
        turtles[i].prevX = turtles[i].x;
        turtles[i].prevY = turtles[i].y;
    }

    // Inicia a posição das estrelas:
    estrelas[0].x = 13;
    estrelas[0].y = 1;

    estrelas[1].x = 1;
    estrelas[1].y = 13;

    estrelas[2].x = 25;
    estrelas[2].y = 13;

    estrelas[3].x = 13;
    estrelas[3].y = 11;

    estrelas[4].x = 13;
    estrelas[4].y = 23;

    estrelas[5].x = 1;
    estrelas[5].y = 21;

    for (int i = 0; i < 6; i++)
        estrelas[i].estrela = true;

    // Reseta os contadores:
    coinsText.setString("0 de " + std::to_string(totalCoins));
    pointsText.setString("00000");

    // Inicia a música tema:
    themeMusic.play();
    themeMusic.setLoop(true);
}

std::string getFormattedPoints(int p)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(5) << p;
    return oss.str();
}

Direction getOppositeDirection(Direction d)
{
    if (d == RIGHT)
        return LEFT;
    else if (d == LEFT)
        return RIGHT;
    else if (d == UP)
        return DOWN;
    else
        return UP;
}

std::vector<Direction> getAvailableRoutes(Entity e)
{
    // Pega todas as rotas disponíveis:
    std::vector<Direction> routes;
    Direction oppositeDirection = getOppositeDirection(e.currentDirection);

    if (map[e.y][e.x + 1] != '1' && RIGHT != oppositeDirection)
        routes.push_back(RIGHT);

    if (map[e.y][e.x - 1] != '1' && LEFT != oppositeDirection)
        routes.push_back(LEFT);

    if (map[e.y + 1][e.x] != '1' && DOWN != oppositeDirection)
        routes.push_back(DOWN);

    if (map[e.y - 1][e.x] != '1' && UP != oppositeDirection)
        routes.push_back(UP);

    if (routes.empty())
        routes.push_back(oppositeDirection);

    return routes;
}

Direction getRandomRouteAvailable(Entity e)
{
    // Pega uma rota disponível aleatória:
    std::vector<Direction> routes = getAvailableRoutes(e);
    int randomRoadIndex = rand() % routes.size();
    return routes.at(randomRoadIndex);
}

// Método principal:
int main()
{
    // Renderizando a janela:
    sf::RenderWindow window(sf::VideoMode(COMPRIMENTO, LARGURA), "Pac-Mario");
    sf::Image icon;
    icon.loadFromFile("./assets/icon.png");
    window.setIcon(16, 16, icon.getPixelsPtr());

    // Carregando o retângulo final:
    sf::RectangleShape rectangle(sf::Vector2f(LARGURA, COMPRIMENTO));
    rectangle.setFillColor(sf::Color::Black);
    rectangle.setPosition(0, 0);

    // Carregando as paredes:
    sf::Texture blockTexture;
    blockTexture.loadFromFile("./assets/block.png");
    sf::Sprite blockSprite(blockTexture);

    // Carregando os canos:
    sf::Texture tunnelDownTexture;
    tunnelDownTexture.loadFromFile("./assets/tunnel_down.png");
    sf::Sprite tunnelDownSprite(tunnelDownTexture);

    sf::Texture tunnelLeftTexture;
    tunnelLeftTexture.loadFromFile("./assets/tunnel_left.png");
    sf::Sprite tunnelLeftSprite(tunnelLeftTexture);

    sf::Texture tunnelRightTexture;
    tunnelRightTexture.loadFromFile("./assets/tunnel_right.png");
    sf::Sprite tunnelRightSprite(tunnelRightTexture);

    sf::Texture tunnelUpTexture;
    tunnelUpTexture.loadFromFile("./assets/tunnel_up.png");
    sf::Sprite tunnelUpSprite(tunnelUpTexture);

    // Carregando as moedas:
    sf::Texture coinTexture;
    coinTexture.loadFromFile("./assets/coin.png");
    sf::Sprite coinSprite(coinTexture);

    // Carregando o contador de moedas:
    sf::Font font;
    font.loadFromFile("./assets/super_mario_world.ttf");

    sf::Text coinsText;
    coinsText.setFont(font);
    coinsText.setCharacterSize(8);
    coinsText.setFillColor(sf::Color::White);
    coinsText.setPosition(22, 404);

    sf::Texture counterTexture;
    counterTexture.loadFromFile("./assets/counter.png");
    sf::Sprite counterSprite(counterTexture);
    counterSprite.setPosition(5, 404);

    for (int i = 0; i < mapSize; i++)
        for (int j = 0; j < mapSize; j++)
            if (map[i][j] == '0')
                totalCoins++;

    coinsText.setString("0 de " + std::to_string(totalCoins));

    // Carregando o contador de pontos:
    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setCharacterSize(12);
    pointsText.setFillColor(sf::Color::White);
    pointsText.setPosition(173, 400);
    pointsText.setString("00000");

    // Carregando texto de vitória:
    sf::Text finalText;
    finalText.setFont(font);
    finalText.setCharacterSize(20);
    finalText.setFillColor(sf::Color::White);
    finalText.setString(" ");

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(10);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(75, 200);
    restartText.setString("Aperte ENTER para jogar de novo");

    // Carregando o Mario:
    Entity player;
    player.rightTexture.loadFromFile("./assets/mario_right.png");
    player.leftTexture.loadFromFile("./assets/mario_left.png");
    player.upTexture.loadFromFile("./assets/mario_up.png");
    player.downTexture.loadFromFile("./assets/mario_down.png");
    player.deathTexture.loadFromFile("./assets/mario_death.png");

    // Carregando as tartarugas:
    Entity turtles[6];
    turtles[0].texture.loadFromFile("./assets/yellow_shell.png");
    turtles[1].texture.loadFromFile("./assets/purple_shell.png");
    turtles[2].texture.loadFromFile("./assets/red_shell.png");
    turtles[3].texture.loadFromFile("./assets/green_shell.png");
    turtles[4].texture.loadFromFile("./assets/cyan_shell.png");
    turtles[5].texture.loadFromFile("./assets/gray_shell.png");

    for (int i = 0; i < 6; i++)
        turtles[i].sprite.setTexture(turtles[i].texture);

    // Carregando o cogumelo:
    Entity mushroom;
    mushroom.x = 12;
    mushroom.y = 13;
    mushroom.texture.loadFromFile("./assets/mushroom.png");
    mushroom.sprite.setTexture(mushroom.texture);

    // Carregando as estrelas:
    Entity estrelas[6];
    for (int i = 0; i < 6; i++)
    {
        estrelas[i].texture.loadFromFile("./assets/star.png");
        estrelas[i].sprite.setTexture(estrelas[i].texture);
    }

    // Carregando efeitos sonoros:
    sf::Music coinSfx, mushroomSfx, kickSfx;
    coinSfx.openFromFile("./assets/coin.wav");
    mushroomSfx.openFromFile("./assets/1_up.wav");
    kickSfx.openFromFile("./assets/kick.wav");

    sf::Music themeMusic, victoryMusic, marioDiesMusic, gameOverMusic, starMusic;
    themeMusic.openFromFile("./assets/theme.wav");
    victoryMusic.openFromFile("./assets/victory.wav");
    marioDiesMusic.openFromFile("./assets/mario_dies.wav");
    gameOverMusic.openFromFile("./assets/game_over.wav");
    starMusic.openFromFile("./assets/star_theme.wav");

    // Iniciando o jogo pela primeira vez:
    restartGame(player, turtles, estrelas, coinsText, coins, lifes, themeMusic, pointsText);

    // Definindo o clock:
    sf::Clock clock;

    // Enquanto a janela estiver aberta:
    while (window.isOpen())
    {
        bool victory = coins >= totalCoins, death = lifes <= 0;
        bool isTheGameOver = victory || death;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Se pressiona uma tecla:
            if (event.type == sf::Event::KeyPressed)
            {
                // Define a intenção de direção:
                switch (event.key.code)
                {
                case sf::Keyboard::Left:
                    if (!isTheGameOver)
                        player.intentionDirection = LEFT;
                    break;
                case sf::Keyboard::Right:
                    if (!isTheGameOver)
                        player.intentionDirection = RIGHT;
                    break;
                case sf::Keyboard::Up:
                    if (!isTheGameOver)
                        player.intentionDirection = UP;
                    break;
                case sf::Keyboard::Down:
                    if (!isTheGameOver)
                        player.intentionDirection = DOWN;
                    break;
                case sf::Keyboard::Return:
                    if (isTheGameOver)
                    {
                        finalText.setString(" ");

                        restartGame(player, turtles, estrelas, coinsText, coins, lifes, themeMusic, pointsText);
                        marioDiesMusic.stop();
                        gameOverMusic.stop();
                        victoryMusic.stop();

                        drawRect = false;
                    }
                    break;
                case sf::Keyboard::Backspace:
                    coins = totalCoins;
                    coinsText.setString(std::to_string(coins) + " de " + std::to_string(totalCoins));
                    themeMusic.stop();
                    starMusic.stop();
                    victoryMusic.play();
                    victory = true;
                    break;
                default:
                    break;
                }
            }
        }

        // Atualiza a cada 0.2 segundos:
        char coord;
        if (clock.getElapsedTime() > sf::seconds(interval))
        {
            clock.restart();
            mushroomInterval += interval;
            // Se tiver passado 30 segundos, o cogumelo aparecerá de novo:
            if (mushroomInterval >= 30 && mushrooms < 3)
                isMushroomAvailable = true;

            // Se o mario estiver com a estrela:
            if (player.estrela)
                starInterval += interval;

            // Se Mario estiver vivo:
            if (!isTheGameOver)
            {
                int newX, newY;

                // Salva a posição anterior do Mario:
                player.prevX = player.x;
                player.prevY = player.y;

                // Carrega a nova posição das tartarugas:
                for (int i = 0; i < 6; i++)
                {
                    turtles[i].prevX = turtles[i].x;
                    turtles[i].prevY = turtles[i].y;

                    newX = turtles[i].x, newY = turtles[i].y;

                    if (turtles[i].currentDirection == LEFT)
                        newX--;
                    else if (turtles[i].currentDirection == RIGHT)
                        newX++;
                    else if (turtles[i].currentDirection == UP)
                        newY--;
                    else if (turtles[i].currentDirection == DOWN)
                        newY++;

                    coord = map[newY][newX];

                    // Se for uma parede:
                    if (coord == '1')
                    {
                        turtles[i].currentDirection = getRandomRouteAvailable(turtles[i]);
                    }
                    // Se não for uma parede:
                    else
                    {
                        turtles[i].x = newX;
                        turtles[i].y = newY;

                        std::vector<Direction> routes = getAvailableRoutes(turtles[i]);
                        int routesSize = routes.size();

                        if (routesSize > 1)
                            turtles[i].currentDirection = getRandomRouteAvailable(turtles[i]);
                        else if (routesSize == 1)
                            turtles[i].currentDirection = routes.at(0);
                    }

                    // Detecta os tuneis:
                    if (turtles[i].x < 0)
                        turtles[i].x = 24;

                    if (turtles[i].x > 24)
                        turtles[i].x = 0;

                    if (turtles[i].y < 0)
                        turtles[i].y = 24;

                    if (turtles[i].y > 24)
                        turtles[i].y = 0;
                }

                // Carrega a nova posição do Mario:
                int tryX = player.x, tryY = player.y;

                // Tenta seguir a intenção de direção:
                if (player.intentionDirection == LEFT)
                    tryX--;
                else if (player.intentionDirection == RIGHT)
                    tryX++;
                else if (player.intentionDirection == UP)
                    tryY--;
                else if (player.intentionDirection == DOWN)
                    tryY++;

                coord = map[tryY][tryX];

                // Se não for uma parede:
                if (coord != '1')
                {
                    // Movimenta para a direção desejada:
                    player.currentDirection = player.intentionDirection;
                }

                // Tenta seguir a direção atual:
                newX = player.x, newY = player.y;

                if (player.currentDirection == LEFT)
                    newX--;
                else if (player.currentDirection == RIGHT)
                    newX++;
                else if (player.currentDirection == UP)
                    newY--;
                else if (player.currentDirection == DOWN)
                    newY++;

                coord = map[newY][newX];

                // Se não for uma parede:
                if (coord != '1')
                {
                    if (player.currentDirection == LEFT)
                        player.sprite.setTexture(player.leftTexture);
                    else if (player.currentDirection == RIGHT)
                        player.sprite.setTexture(player.rightTexture);
                    else if (player.currentDirection == UP)
                        player.sprite.setTexture(player.upTexture);
                    else if (player.currentDirection == DOWN)
                        player.sprite.setTexture(player.downTexture);

                    player.x = newX;
                    player.y = newY;

                    // Detecta os tuneis:
                    if (player.x < 0)
                        player.x = 24;

                    if (player.x > 24)
                        player.x = 0;

                    if (player.y < 0)
                        player.y = 24;

                    if (player.y > 24)
                        player.y = 0;

                    // Se coletar uma moeda:
                    if (coord == '0')
                    {
                        // Limpa a moeda do mapa e soma no contador:
                        map[newY][newX] = ' ';
                        coins++;
                        points += 10;
                        coinSfx.play();
                        coinsText.setString(std::to_string(coins) + " de " + std::to_string(totalCoins));
                        pointsText.setString(getFormattedPoints(points));

                        // Se tiver acabado as moedas:
                        if (coins == totalCoins)
                        {
                            themeMusic.stop();
                            starMusic.stop();
                            victoryMusic.play();

                            victory = true;
                        }
                    }
                    // Se coletar um cogumelo:
                    else if (newX == mushroom.x && newY == mushroom.y && isMushroomAvailable)
                    {
                        mushroomSfx.play();
                        points += 100;
                        mushroomInterval = 0;
                        isMushroomAvailable = false;
                        mushrooms++;
                    }

                    // Se coletar uma estrela:
                    for (int i = 0; i < 6; i++)
                    {
                        if (newX == estrelas[i].x && newY == estrelas[i].y && estrelas[i].estrela)
                        {
                            starMusic.play();
                            themeMusic.stop();
                            points += 50;
                            estrelas[i].estrela = false;
                            starInterval = 0;
                            player.estrela = true;

                            for (int j = 0; j < 6; j++)
                                turtles[j].currentDirection = getOppositeDirection(turtles[j].currentDirection);
                        }
                    }
                }

                // Verifica encontro de tartarugas e Mario:
                for (int i = 0; i < 6; i++)
                {
                    // Se estão no mesmo pixel:
                    bool samePixel = (player.x == turtles[i].x && player.y == turtles[i].y);

                    // Se eles se atravessaram:
                    bool crossed = (player.x == turtles[i].prevX && player.y == turtles[i].prevY &&
                                    player.prevX == turtles[i].x && player.prevY == turtles[i].y);

                    // Se eles tem colisão:
                    if (samePixel || crossed)
                    {
                        // Retira vida do Mario se ele estiver normal ou mata a tartaruga se ele estiver com a estrela:
                        if (player.estrela)
                        {
                            turtles[i].x = 12;
                            turtles[i].y = 11;
                            points += 200;
                            std::cout << "Matou uma tartaruga!\n";
                            kickSfx.play();
                        }
                        else
                        {
                            lifes--;
                            std::cout << "Mario foi atacado! " + std::to_string(lifes) + " vidas restantes.\n";
                        }

                        // Se o Mario morreu:
                        if (lifes <= 0)
                        {
                            themeMusic.stop();
                            marioDiesMusic.play();

                            player.sprite.setTexture(player.deathTexture);
                            player.y--;

                            death = true;
                        }
                        break;
                    }
                }

                // Verifica se o efeito da estrela ja acabou:
                if (starInterval >= 10)
                {
                    player.estrela = false;
                    starInterval = 0;
                    starMusic.stop();
                    themeMusic.play();
                }
            }
            // Se o Mario estiver morto:
            else if (death)
            {
                if (player.y < (LARGURA / PIXEL) + 1)
                    player.y++;
                else if (marioDiesMusic.getStatus() == 0)
                    drawRect = true;

                finalText.setString("GAME OVER");
                finalText.setPosition(130, 180);
            }
            // Se o Mario coletar todas moedas:
            else if (victory)
            {
                finalText.setString("VICTORY");
                finalText.setPosition(140, 180);

                drawRect = true;
            }
        }

        // Desenha o fundo:
        window.clear(sf::Color::Black);

        // Desenha o mapa:
        for (int i = 0; i < mapSize; i++)
        {
            for (int j = 0; j < mapSize; j++)
            {
                coord = map[i][j];
                if (coord == '1')
                {
                    blockSprite.setPosition(j * PIXEL, i * PIXEL);
                    window.draw(blockSprite);
                }
                else if (coord == '0')
                {
                    coinSprite.setPosition(j * PIXEL, i * PIXEL);
                    window.draw(coinSprite);
                }
            }
        }

        // Desenha as tartarugas:
        for (int i = 0; i < 6; i++)
        {
            turtles[i].sprite.setPosition(turtles[i].x * PIXEL, turtles[i].y * PIXEL);
            window.draw(turtles[i].sprite);
        }

        // Desenha as estrelas:
        for (int i = 0; i < 6; i++)
        {
            if (estrelas[i].estrela)
            {
                estrelas[i].sprite.setPosition(estrelas[i].x * PIXEL, estrelas[i].y * PIXEL);
                window.draw(estrelas[i].sprite);
            }
        }

        // Desenha o Mario:
        player.sprite.setPosition(player.x * PIXEL, player.y * PIXEL);
        window.draw(player.sprite);

        // Desenha o cogumelo:
        if (isMushroomAvailable)
        {
            mushroom.sprite.setPosition(mushroom.x * PIXEL, mushroom.y * PIXEL);
            window.draw(mushroom.sprite);
        }

        // Desenha os tuneis:
        tunnelDownSprite.setPosition(12 * PIXEL, 0);
        window.draw(tunnelDownSprite);
        tunnelLeftSprite.setPosition(24 * PIXEL, 13 * PIXEL);
        window.draw(tunnelLeftSprite);
        tunnelRightSprite.setPosition(0, 13 * PIXEL);
        window.draw(tunnelRightSprite);
        tunnelUpSprite.setPosition(12 * PIXEL, 24 * PIXEL);
        window.draw(tunnelUpSprite);

        // Desenha os contadores:
        window.draw(counterSprite);
        window.draw(coinsText);

        window.draw(pointsText);

        // Se venceu, aparece texto de vitória:
        if (drawRect)
            window.draw(rectangle);

        if (isTheGameOver && drawRect)
        {
            window.draw(finalText);
            window.draw(restartText);

            if (death)
                gameOverMusic.play();
        }

        // Exibe a janela:
        window.display();
    }

    return 0;
}
