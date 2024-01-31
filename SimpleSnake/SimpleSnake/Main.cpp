#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <random>

const int winSizeX = 800;
const int winSizeY = 800;

const int cellSizeX = 25;
const int cellSizeY = 25;

const int gridSizeX = winSizeX / cellSizeX;
const int gridSizeY = winSizeY / cellSizeY;

const int cellID_empty = 0;
const int cellID_snakeHead = 1;
const int cellID_snakePart = 2;
const int cellID_fruit = 3;

const float move_CD = .25f;
const float move_timerMultiplicatorPerFruits = 0.05;
float move_Timer;
bool canChangeNextPos;

static sf::RectangleShape cell;

bool isLeftMousePressed;
bool isRightMousePressed;

struct CellGraph
{
    sf::Color color;
    sf::Color outlineColor;
    float outlineThickness;
};

const sf::Color snakeHeadColor = { 53, 252, 3, 255 };
const sf::Color snakePartColor = { 59, 181, 29, 255 };

const std::map<int, CellGraph> cellsGraphs
{
    {cellID_empty, CellGraph { sf::Color::Transparent, sf::Color::Black, .5f}},
    {cellID_snakeHead, CellGraph { snakeHeadColor, sf::Color::Black, .5f}},
    {cellID_snakePart, CellGraph { snakePartColor, sf::Color::Black, .5f}},
    {cellID_fruit, CellGraph { sf::Color::Red, sf::Color::Black, .5f}},
};

struct CellData
{
    int ID;
};

struct ItemData
{
    int cellID;
    sf::Vector2i pos;
};

sf::Vector2i nextPos;
sf::Vector2i queuedNextPos;


std::list<ItemData> snake;
std::map<std::tuple<float, float>, ItemData> fruits;

CellData grid[gridSizeX][gridSizeY];
std::list<sf::Vector2i> emptyCells;
sf::RenderWindow window (sf::VideoMode(winSizeX, winSizeY), "Simple Snake");

sf::VertexArray gridVA(sf::Lines, (gridSizeX + gridSizeY + 2) * 2);

float simulationSpeed = 1;

void DrawItem(ItemData data)
{
    CellGraph cg = cellsGraphs.at(grid[data.pos.x][data.pos.y].ID);
    cell.setFillColor(cg.color);
    cell.setOutlineColor(cg.outlineColor);
    cell.setOutlineThickness(cg.outlineThickness);
    cell.setPosition(cellSizeX * data.pos.x, cellSizeY * data.pos.y);
    window.draw(cell);
}

void DrawGrid()
{
    for (ItemData spd : snake) DrawItem(spd);
    for (auto& fruit : fruits) DrawItem(fruit.second);

    window.draw(gridVA);
}

void SpawnFruit()
{
    if (emptyCells.size() == 0)
    {
        simulationSpeed = 0;
        return;
    }

    sf::Vector2i fruitpos;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, emptyCells.size() - 1);

    std::list<sf::Vector2i>::iterator it = emptyCells.begin();
    std::advance(it, dis(gen));

    sf::Vector2i randomPos = *it;

    ItemData newFruit =
    {
        cellID_fruit,
        randomPos
    };

    fruits.insert({ {randomPos.x, randomPos.y}, newFruit });
    emptyCells.remove(randomPos);
    grid[randomPos.x][randomPos.y] = { cellID_fruit };
}

void ResetGame()
{
    snake.clear();
    fruits.clear();
    nextPos = { 0,0 };
    queuedNextPos = { 0,0 };
    move_Timer = 0;

    for (size_t x = 0; x < gridSizeX; x++)
    {
        for (size_t y = 0; y < gridSizeY; y++)
        {
            grid[x][y].ID = 0;
        }
    }

    sf::Vector2i startPos = { gridSizeX / 2, gridSizeY / 2 };
    grid[startPos.x][startPos.y] = { cellID_snakeHead };

    ItemData head =
    {
        cellID_snakeHead,
        startPos
    };
    snake.push_front(head);
    SpawnFruit();
}

sf::Vector2i GetGridMousePos()
{
    return sf::Vector2i(sf::Mouse::getPosition(window).x / cellSizeX, sf::Mouse::getPosition(window).y / cellSizeY);
}

void TrySetNextPos(int x, int y)
{
    sf::Vector2i pos = { x,y };

    if (canChangeNextPos)
    {
        if (pos == nextPos * -1) return;
        nextPos = pos;
        canChangeNextPos = false;
        return;
    }

    if (pos == nextPos * -1) return;
    queuedNextPos = pos;
}

void ProcessInputs()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                    case sf::Keyboard::R:
                        ResetGame();
                        break;

                    case sf::Keyboard::Up:
                        TrySetNextPos(0, 1);
                        break;

                    case sf::Keyboard::Down:
                        TrySetNextPos(0, -1);
                        break;

                    case sf::Keyboard::Left:
                        TrySetNextPos(-1, 0);
                        break;

                    case sf::Keyboard::Right:
                        TrySetNextPos(1, 0);
                        break;

                    case sf::Keyboard::Num1:
                        simulationSpeed++;
                        break;

                    case sf::Keyboard::Num2:
                        simulationSpeed--;
                        if (simulationSpeed < 0)
                            simulationSpeed = 0;
                        break;

                    case sf::Keyboard::P:
                        simulationSpeed = simulationSpeed == 0 ? 1 : 0;
                        break;
                }
                break;

            case sf::Event::MouseButtonPressed:
                switch (event.mouseButton.button)
                {
                    case sf::Mouse::Left:
                        isLeftMousePressed = true;
                        break;
                    case sf::Mouse::Right:
                        isRightMousePressed = true;
                        break;
                    case sf::Mouse::Middle:
                        sf::Vector2i mousePos = GetGridMousePos();
                        std::cout << "x:" + std::to_string(mousePos.x) + " y:" + std::to_string(mousePos.y) + " : " + std::to_string(grid[mousePos.x][mousePos.y].ID) << std::endl;
                        break;
                }
                break;

            case sf::Event::MouseButtonReleased:
                switch (event.mouseButton.button)
                {
                    case sf::Mouse::Left:
                        isLeftMousePressed = false;
                        break;
                    case sf::Mouse::Right:
                        isRightMousePressed = false;
                        break;
                }
                break;
        }
    }
}

void AddSnakePart()
{
    ItemData tail = snake.back();
    sf::Vector2i newPartPos = { tail.pos.x, tail.pos.y };
    if (tail.cellID == cellID_snakeHead)
        newPartPos -= nextPos;
    else
    {
        std::list<ItemData>::iterator it = snake.end();
        std::advance(it, -2);
        newPartPos = tail.pos - (it->pos - tail.pos);
    }

    ItemData part =
    {
        cellID_snakePart,
        newPartPos
    };
    grid[newPartPos.x][newPartPos.y].ID = part.cellID;
    emptyCells.remove(newPartPos);
    snake.push_back(part);
}

void MoveSnakePart(sf::Vector2i dir, ItemData* snakePart)
{
    grid[snakePart->pos.x][snakePart->pos.y] = { cellID_empty };
    emptyCells.push_back(snakePart->pos);

    sf::Vector2i nextPartPos = { snakePart->pos.x + dir.x , snakePart->pos.y - dir.y };

    if (nextPartPos.x < 0 || nextPartPos.x >= gridSizeX)
    {
        ResetGame();
        return;
    }
    if (nextPartPos.y < 0 || nextPartPos.y >= gridSizeY)
    {
        ResetGame();
        return;
    }
    if (grid[nextPartPos.x][nextPartPos.y].ID == cellID_snakePart)
    {
        ResetGame();
        return;
    }


    if (grid[nextPartPos.x][nextPartPos.y].ID == cellID_fruit)
    {
        fruits.erase({nextPartPos.x, nextPartPos.y});
        AddSnakePart();
        SpawnFruit();
    }

    grid[nextPartPos.x][nextPartPos.y] = { snakePart->cellID };
    emptyCells.remove(nextPartPos);
    snakePart->pos = nextPartPos;
}

void ProcessSnake(float dt)
{
    move_Timer -= dt;
    if (move_Timer > 0) return;
    move_Timer += (move_CD - (move_CD * move_timerMultiplicatorPerFruits * (snake.size() - 1)));
    if (snake.size() == 0) return;

    ItemData* head = &snake.front();
    sf::Vector2i lastPartPos = { head->pos.x, head->pos.y };
    MoveSnakePart(nextPos, head);
    canChangeNextPos = true;
    if (queuedNextPos != sf::Vector2i{ 0,0 })
    {
        nextPos = queuedNextPos;
        queuedNextPos = sf::Vector2i{ 0,0 };
    }
    if (snake.size() == 1) return;

    std::list<ItemData>::iterator current = snake.begin();

    sf::Vector2i tmp = lastPartPos;
    for (size_t i = 1; i < snake.size(); i++)
    {
        std::advance(current, 1);
        ItemData& spd = *current;

        tmp = spd.pos;
        sf::Vector2i nextPartPos = { lastPartPos.x - current->pos.x, current->pos.y - lastPartPos.y };
        MoveSnakePart(nextPartPos, &spd);
        lastPartPos = tmp;
    }
}

void Init()
{
    window.setFramerateLimit(30);
    cell.setSize(sf::Vector2f(cellSizeX, cellSizeY));

    for (size_t x = 0; x < gridSizeX; x++)
    {
        for (size_t y = 0; y < gridSizeY; y++)
        {
            grid[x][y] = { cellID_empty };
            emptyCells.push_back({ static_cast<int>(x),static_cast<int>(y) });
        }
    }

    sf::Vector2i startPos = { gridSizeX / 2, gridSizeY / 2 };
    grid[startPos.x][startPos.y] = { cellID_snakeHead };
    emptyCells.remove(startPos);

    nextPos = { 0,0 };

    ItemData head =
    {
        cellID_snakeHead,
        startPos
    };
    snake.push_front(head);

    for (size_t x = 0; x <= gridSizeX; x++)
    {
        gridVA[x * 2].position = sf::Vector2f(x * cellSizeX, 0.f);
        gridVA[x * 2].color = sf::Color::Black;
        gridVA[x * 2 + 1].position = sf::Vector2f(x * cellSizeX, gridSizeY * cellSizeY);
        gridVA[x * 2 + 1].color = sf::Color::Black;
    }
    for (size_t y = 0; y <= gridSizeY; y++)
    {
        gridVA[(gridSizeX + 1) * 2 + y * 2].position = sf::Vector2f(0.f, y * cellSizeY);
        gridVA[(gridSizeX + 1) * 2 + y * 2].color = sf::Color::Black;
        gridVA[(gridSizeX + 1) * 2 + y * 2 + 1].position = sf::Vector2f(gridSizeX * cellSizeX, y * cellSizeY);
        gridVA[(gridSizeX + 1) * 2 + y * 2 + 1].color = sf::Color::Black;
    }
    SpawnFruit();
}

int main()
{
    sf::Time dt;
    sf::Clock clock;

    Init();
    while (window.isOpen())
    {
        ProcessInputs();
        ProcessSnake(dt.asSeconds() * simulationSpeed);

        window.clear(sf::Color(18, 33, 43));
        DrawGrid();
        window.display();
        dt = clock.restart();
    }

    return 0;
}