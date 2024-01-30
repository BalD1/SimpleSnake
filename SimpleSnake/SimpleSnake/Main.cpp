#include <SFML/Graphics.hpp>
#include <iostream>

const int winSizeX = 800;
const int winSizeY = 800;

const int cellSizeX = 10;
const int cellSizeY = 10;

const int gridSizeX = winSizeX / cellSizeX;
const int gridSizeY = winSizeY / cellSizeY;

const int cellID_empty = 0;

static sf::RectangleShape cell;

bool isLeftMousePressed;
bool isRightMousePressed;

struct CellGraph
{
    sf::Color color;
    sf::Color outlineColor;
    float outlineThickness;
};

const std::map<int, CellGraph> cellsGraphs
{
    {cellID_empty, CellGraph { sf::Color::Transparent, sf::Color::Black, .5f}},
};

struct CellData
{
    int ID;
};

CellData grid[gridSizeX][gridSizeY];
sf::RenderWindow window (sf::VideoMode(winSizeX, winSizeY), "Simple Snake");

void DrawGrid()
{
    for (size_t x = 0; x < gridSizeX; x++)
    {
        for (size_t y = 0; y < gridSizeY; y++)
        {
            CellGraph cg = cellsGraphs.at(grid[x][y].ID);
            cell.setFillColor(cg.color);
            cell.setOutlineColor(cg.outlineColor);
            cell.setOutlineThickness(cg.outlineThickness);
            cell.setPosition(cellSizeX * x, cellSizeY * y);
            window.draw(cell);
        }
    }
}

void ResetGrid()
{
    for (size_t x = 0; x < gridSizeX; x++)
    {
        for (size_t y = 0; y < gridSizeY; y++)
        {
            grid[x][y].ID = 0;
        }
    }
}

sf::Vector2i GetGridMousePos()
{
    return sf::Vector2i(sf::Mouse::getPosition(window).x / cellSizeX, sf::Mouse::getPosition(window).y / cellSizeY);
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
                        ResetGrid();
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

int main()
{
    window.setFramerateLimit(60);

    cell.setSize(sf::Vector2f(cellSizeX, cellSizeY));

    for (size_t x = 0; x < gridSizeX; x++)
    {
        for (size_t y = 0; y < gridSizeY; y++)
        {
            grid[x][y] = { cellID_empty };
        }
    }

    while (window.isOpen())
    {
        ProcessInputs();
        
        window.clear(sf::Color(18, 33, 43));
        DrawGrid();
        window.display();
    }

    return 0;
}