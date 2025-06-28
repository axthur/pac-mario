#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>

// Definindo o mapa:
#define COMPRIMENTO 400
#define LARGURA 420
#define PIXEL 16

const int mapSize = 25;
char map[mapSize][mapSize + 1] = {
    "1111111111111111111111111",
    "1000000000000000000000001",
    "1011110111110111110111101",
    "1010000100000100000100001",
    "1010110111110111110101101",
    "1000100001000001000000101",
    "1111111011111110111111111",
    "1000001000000000001000001",
    "1011111011110111111011101",
    "1000000010000100000000001",
    "1011111110111111011111101",
    "1000000000100001000000001",
    "1011111111110111111111101",
    "1000000000000000000000001",
    "1111111110111111011111111",
    "1000000010000100000000001",
    "1011111011110111111011101",
    "1000001000000000001000001",
    "1111111011111110111111111",
    "1000100001000001000000101",
    "1010110111110111110101101",
    "1010000100000100000100001",
    "1011110111110111110111101",
    "1000000000000000000000001",
    "1111111111111111111111111"};

// Definindo contadores:
int coins = 0;
int lifes = 1;

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
    sf::Texture texture, rightTexture, leftTexture, upTexture, downTexture;
    sf::Sprite sprite;
    Direction currentDirection, intentionDirection;
};

// Definindo funções:
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
    std::vector<Direction> routes;
    Direction oppositeDirection = getOppositeDirection(e.currentDirection);

    // Verifica as rotas disponíveis:
    if (map[e.x + 1][e.y] != '1' && RIGHT != oppositeDirection)
        routes.push_back(RIGHT);

    if (map[e.x - 1][e.y] != '1' && LEFT != oppositeDirection)
        routes.push_back(LEFT);

    if (map[e.x][e.y + 1] != '1' && DOWN != oppositeDirection)
        routes.push_back(DOWN);

    if (map[e.x][e.y - 1] != '1' && UP != oppositeDirection)
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

    // Carregando o fundo:
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("./assets/map.png");
    sf::Sprite background(backgroundTexture);

    // Carregando as paredes:
    sf::Texture blockTexture;
    blockTexture.loadFromFile("./assets/block.png");
    sf::Sprite blockSprite(blockTexture);

    // Carregando as moedas:
    sf::Texture coinTexture;
    coinTexture.loadFromFile("./assets/coin.png");
    sf::Sprite coinSprite(coinTexture);

    // Carregando o contador de moedas:
    sf::Font font;
    if (!font.loadFromFile("./assets/super_mario_world.ttf"))
    {
        std::cerr << "Erro ao carregar a fonte!" << std::endl;
        return 1;
    }

    sf::Text coinText;
    coinText.setFont(font);
    coinText.setCharacterSize(8);
    coinText.setFillColor(sf::Color::White);
    coinText.setPosition(22, 404);

    // Contando o total de moedas:
    int totalCoins = 0;
    for (int i = 0; i < mapSize; i++)
        for (int j = 0; j < mapSize; j++)
            if (map[i][j] == '0')
                totalCoins++;

    sf::Texture counterTexture;
    counterTexture.loadFromFile("./assets/counter.png");
    sf::Sprite counterSprite(counterTexture);
    counterSprite.setPosition(5, 405);

    coinText.setString("0 de " + std::to_string(totalCoins));

    // Carregando o Mario:
    Entity player;
    player.x = 12;
    player.y = 11;

    player.rightTexture.loadFromFile("./assets/mario_right.png");
    player.leftTexture.loadFromFile("./assets/mario_left.png");
    player.upTexture.loadFromFile("./assets/mario_up.png");
    player.downTexture.loadFromFile("./assets/mario_down.png");

    player.sprite.setTexture(player.downTexture);

    // Carregando as tartarugas:
    Entity turtles[6];

    turtles[0].x = turtles[0].y = turtles[1].x = turtles[2].y = turtles[4].x = 1;
    turtles[1].y = turtles[2].x = turtles[3].x = turtles[3].y = turtles[5].x = 23;
    turtles[4].y = turtles[5].y = 12;

    turtles[0].currentDirection = DOWN;
    turtles[1].currentDirection = RIGHT;
    turtles[2].currentDirection = UP;
    turtles[3].currentDirection = LEFT;
    turtles[4].currentDirection = RIGHT;
    turtles[5].currentDirection = LEFT;

    for (int i = 0; i < 6; i++)
    {
        turtles[i].texture.loadFromFile("./assets/shell.gif");
        turtles[i].sprite.setTexture(turtles[i].texture);
    }

    // Definindo o clock:
    sf::Clock clock;

    // Enquanto a janela estiver aberta:
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Se Mario estiver vivo:
            if (lifes > 0)
            {
                // Se pressiona uma tecla:
                if (event.type == sf::Event::KeyPressed)
                {
                    // Define a intenção de direção:
                    switch (event.key.code)
                    {
                    case sf::Keyboard::Left:
                        player.intentionDirection = LEFT;
                        break;
                    case sf::Keyboard::Right:
                        player.intentionDirection = RIGHT;
                        break;
                    case sf::Keyboard::Up:
                        player.intentionDirection = UP;
                        break;
                    case sf::Keyboard::Down:
                        player.intentionDirection = DOWN;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        // Atualiza a cada 0.2 segundos:
        char coord;
        if (clock.getElapsedTime() > sf::seconds(0.2f))
        {
            clock.restart();
            // Se Mario estiver vivo:
            if (lifes > 0)
            {
                int newX, newY;

                // Carrega a nova posição das tartarugas:
                for (int i = 0; i < 6; i++)
                {
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
                        std::vector<Direction> routes = getAvailableRoutes(turtles[i]);
                        int routesSize = routes.size();
                        std::cout << routesSize << " ";

                        if (routesSize > 1)
                            turtles[i].currentDirection = getRandomRouteAvailable(turtles[i]);
                        else if (routesSize == 1)
                            turtles[i].currentDirection = routes.at(0);
                        else
                            std::cout << "Erro: tartaruga sem caminho disponível." << std::endl;

                        turtles[i].x = newX;
                        turtles[i].y = newY;
                    }
                }
                std::cout << std::endl;

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

                    // Se coletar uma moeda:
                    if (coord == '0')
                    {
                        // Limpa a moeda do mapa e soma no contador:
                        map[newY][newX] = ' ';
                        coins++;
                        coinText.setString(std::to_string(coins) + " de " + std::to_string(totalCoins));
                    }
                }

                // Verifica encontro de tartarugas e Mario:
                for (int i = 0; i < 6; i++)
                {
                    if (player.x == turtles[i].x && player.y == turtles[i].y)
                    {
                        // Retira vida do Mario:
                        lifes--;
                        std::cout << "Mario foi atacado! " + std::to_string(lifes) + " vidas restantes.\n";
                        break;
                    }
                }
            }
        }

        // Desenha o fundo:
        window.clear(sf::Color::Black);
        window.draw(background);

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

        // Desenha o Mario:
        player.sprite.setPosition(player.x * PIXEL, player.y * PIXEL);
        window.draw(player.sprite);

        // Desenha o contador de moedas:
        window.draw(counterSprite);
        window.draw(coinText);

        // Exibe a janela:
        window.display();
    }

    return 0;
}
