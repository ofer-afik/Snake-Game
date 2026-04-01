#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>
#include "button.hpp"

#include "eat.h"
#include "hit.h"
#include "head.h"

double lastUpdateTime = 0;
Vector2 lastUpdateDir = Vector2{1, 0};

// Color constants
Color bgColor = {0, 255, 0, 255};
Color borderColor = {0, 100, 0, 255};
Color foodColor = {255, 255, 0, 255};
Color snakeColor = {255, 0, 0, 255};

// Resources
Wave eat_wave = LoadWaveFromMemory(".mp3", eat_mp3, eat_mp3_len);
Sound eat_sfx;
Wave hit_wave = LoadWaveFromMemory(".mp3", hit_mp3, hit_mp3_len);
Sound hit_sfx;
Image head_image = LoadImageFromMemory(".png", head_png, head_png_len);

float cellSize = 30.0;
float cellNum = 25.0;
float offset = 90.0;

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque)
{
    for (int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(element, deque[i]))
        {
            return true;
        }
    }
    return false;
}

bool eventTriggered(float interval)
{
    double currrentTime = GetTime(); 
    if (currrentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currrentTime;
        return true;
    }
    else
    {
        return false;
    }
} 

// Classes for the game
class Food
{
    public:
        Vector2 pos = {0, 0};

        Food(std::deque<Vector2> snakeBody)
        {
            pos = GenerateRandomPos(snakeBody);
        }
    
        void Draw()
        {
            DrawRectangle(offset + pos.x * cellSize, offset + pos.y * cellSize, cellSize, cellSize, foodColor);
        }

        Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody)
        {
            Vector2 position;
            do
            {
                float x = GetRandomValue(0, cellNum - 1);
                float y = GetRandomValue(0, cellNum - 1);
                position = Vector2{x, y};
            }
            while (ElementInDeque(position, snakeBody));
            return position;
        }
};

class Snake
{
    public:
        std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Vector2 dir = Vector2{1, 0};
        bool addSegment = false;
        Texture2D head;
        

        Snake()
        {
            head = LoadTextureFromImage(head_image);
        }

        ~Snake()
        {
            UnloadTexture(head);
        }

        void Draw()
        {
            DrawTexture(head, offset + body.front().x * cellSize, offset + body.front().y * cellSize, Color{255, 255, 255, 255});
            for (int i = 1; i < body.size(); i++)
            {
                Rectangle segment = Rectangle{offset + body[i].x * cellSize, offset + body[i].y * cellSize, cellSize, cellSize};
                DrawRectangleRounded(segment, 0.5, 6, snakeColor);
            }
        }

        void Update()
        {
            lastUpdateDir = dir;
            body.push_front(Vector2Add(body.front(), dir));
            if (addSegment)
            {
                addSegment = false;
            }
            else
            {
                body.pop_back();
            }
        }

        void Reset()
        {
            body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
            dir = Vector2{1, 0};
        }
};

class Game
{
    public:
        bool running = true;

        Snake snake = Snake();
        Food food = Food(snake.body);
        int score = 0;

        std::deque<KeyboardKey> input;

        Game()
        {
            InitAudioDevice();
            eat_sfx = LoadSoundFromWave(eat_wave);
            hit_sfx = LoadSoundFromWave(hit_wave);
        }

        ~Game()
        {
            UnloadSound(eat_sfx);
            UnloadSound(hit_sfx);
            CloseAudioDevice();
        }

        void Draw()
        {
            food.Draw();
            snake.Draw();
        }

        void Update()
        {
            if (running)
            {
                if (!input.empty())
                {
                    KeyboardKey cur_input = input.front();
                    input.pop_front();

                    if (cur_input == KEY_UP)
                    {
                        snake.dir = Vector2{0, -1};
                    }

                    if (cur_input == KEY_DOWN)
                    {
                        snake.dir = Vector2{0, 1};
                    }

                    if (cur_input == KEY_LEFT)
                    {
                        snake.dir = Vector2{-1, 0};
                    }

                    if (cur_input == KEY_RIGHT)
                    {
                        snake.dir = Vector2{1, 0};
                    }
                }

                snake.Update();
                CheckCollisionWithFood();
                CheckCollisionWithEdges();
                CheckCollisionWithTail();
            }
        }

        void CheckCollisionWithFood()
        {
            if (Vector2Equals(snake.body[0], food.pos))
            {
                food.pos = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score++;
                PlaySound(eat_sfx);
            }
        }

        void CheckCollisionWithEdges()
        {
            Vector2 head = snake.body.front();
            if (head.x == cellNum || head.x == -1 || head.y == cellNum || head.y == -1)
            {
                GameOver();
            }
        }

        void CheckCollisionWithTail()
        {
            Vector2 head = snake.body.front();
            for (int i = 1; i < snake.body.size(); i++)
            {
                if (head == snake.body[i])
                {
                    GameOver();
                    return;
                }
            }
        }

        void GameOver()
        {
            snake.Reset();
            food.pos = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
            PlaySound(hit_sfx);
        }
};


// Game function
int main()
{
    InitWindow(2 * offset + cellNum * cellSize, 2 * offset + cellNum * cellSize, "Retro Snake");
    SetTargetFPS(60);

    Button playBut = Button(350, 500, 200, 130, "Play", snakeColor, foodColor);

    while (true)
    {
        if (WindowShouldClose())
        {
            CloseWindow();
            return 0;
        }
        BeginDrawing();
            ClearBackground(bgColor);
            DrawText("Retro Snake", 150, 100, 100, borderColor);
            playBut.Draw();
            if (playBut.isPressed())
            {
                break;
            }
        EndDrawing();
    }

    Game game = Game();

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(bgColor);
            DrawText("Retro Snake", offset - 5, 30, 50, borderColor);
            DrawText(TextFormat("%i", game.score), cellNum * cellSize + offset + 5, 30, 50, borderColor);
            DrawRectangleLinesEx(Rectangle{offset - 5, offset - 5, cellNum * cellSize + 10, cellNum * cellSize + 10}, 5, borderColor);

            if (GetKeyPressed())
            {
                game.running = true;
            }
            if (IsKeyPressed(KEY_UP) && lastUpdateDir.y != 1)
            {
                game.input.push_back(KEY_UP);
            }

            if (IsKeyPressed(KEY_DOWN) && lastUpdateDir.y != -1)
            {
                game.input.push_back(KEY_DOWN);
            }

            if (IsKeyPressed(KEY_LEFT) && lastUpdateDir.x != 1)
            {
                game.input.push_back(KEY_LEFT);
            }

            if (IsKeyPressed(KEY_RIGHT) && lastUpdateDir.x != -1)
            {
                game.input.push_back(KEY_RIGHT);
            }

            if (eventTriggered(0.1))
            {
                game.Update();
            }

            game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}