#include <stdio.h>
#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    int health;
    float updateTime;
    float runningTime;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame /*,bool flip*/)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update anim frame
        data.rec.x = data.frame * data.rec.width;
        // printf("%.4f\n", data.rec.x);
        /*if (flip)
        {
            data.rec.width *= -1;
        }*/
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

bool isOnLedge(Rectangle rec, Rectangle rec2)
{
    return rec.y >= rec2.y - rec.height &&
           rec.y < rec2.y;
}

bool isUnderLedge(Rectangle rec, Rectangle rec2)
{
    return rec.y < rec2.y + rec2.height &&
           rec.y > rec2.y;
}

bool stageEnd(Rectangle rec, float end, Rectangle block)
{
    return rec.x > end - rec.width &&
           rec.x < end + rec.width &&
           rec.y < block.y;
}

int main()
{
    int screenWidth{928};
    int screenHeight{725};

    InitWindow(screenWidth, screenHeight, "Adventure Game");

    // acceleration due to gravity
    const int gravity{1'000};

    // velocity
    int velocity{0};
    int jumpVel{-600}; // pixels per second

    int direction{5};
    int levitate{1};

    bool isInAir{};
    bool isRunning{};
    // bool isLeft{};
    bool isAlive{true};
    bool gotKey1{};
    bool gotKey2{};
    bool gotKey3{};
    bool gotKeys{};

    Texture2D heroIdle = LoadTexture("textures/character/adventurer_idle.png");
    AnimData heroData;
    // Rectangle
    heroData.rec.x = 0.f;
    heroData.rec.y = 0.f;
    heroData.rec.width = heroIdle.width / 4;
    heroData.rec.height = heroIdle.height;
    // Vector2
    heroData.pos.x = screenWidth / 2 - heroData.rec.width / 2;
    heroData.pos.y = screenHeight - heroData.rec.height;
    // frame, update and running Time
    heroData.frame = 0;
    heroData.health = 3;
    heroData.updateTime = 1.f / 6.f;
    heroData.runningTime = 0.f;

    // hero jump
    Texture2D heroJump = LoadTexture("textures/character/adventurer_jump.png");
    AnimData jumpData;
    jumpData.rec.x = 0.f;
    jumpData.rec.y = 0.f;
    jumpData.rec.width = heroJump.width / 3;
    jumpData.rec.height = heroJump.height;
    jumpData.frame = 0;
    jumpData.updateTime = 1.f / 6.f;
    jumpData.frame = 0.f;

    // hero run
    Texture2D heroRun = LoadTexture("textures/character/adventurer_run.png");
    AnimData runData;
    runData.rec.x = 0.f;
    runData.rec.y = 0.f;
    runData.rec.width = heroRun.width / 6;
    runData.rec.height = heroRun.height;
    runData.frame = 0;
    runData.updateTime = 1.f / 10.f;
    runData.frame = 0.f;

    Rectangle block1{0.f, 550.f, 416.f, 64.f};

    const int numOfBlocks{10};
    Rectangle blocks[numOfBlocks]{};

    for (int i = 0; i < numOfBlocks; i++)
    {
        blocks[i].x = block1.x + (i * 575.f);
        blocks[i].y = block1.y;
        blocks[i].width = block1.width;
        blocks[i].height = block1.height;
    }
    blocks[1].y = block1.y - 150.f;
    blocks[3].y = block1.y - 250.f;

    // enemy Rec
    Texture2D enemy = LoadTexture("textures/character/enemy.png");
    AnimData enemyData;
    // Rectangle
    enemyData.rec.x = 0.f;
    enemyData.rec.y = 0.f;
    enemyData.rec.width = enemy.width / 8;
    enemyData.rec.height = enemy.height;
    // Vector2
    enemyData.pos.x = blocks[7].x + 20.f;
    enemyData.pos.y = blocks[7].y - 200.f;
    // frame, update and running Time
    enemyData.frame = 0;
    enemyData.updateTime = 1.f / 12.f;
    enemyData.runningTime = 0.f;

    Rectangle door{blocks[9].x + 300.f, blocks[9].y - 100.f, 70.f, 100.f};

    const int numOfKeys{3};
    Rectangle key{block1.x, block1.y - 120.f, 20.f, 20.f};
    Rectangle keys[numOfKeys]{};

    keys[0].x = key.x + 300.f;
    keys[0].y = key.y - 200.f;
    keys[0].width = key.width;
    keys[0].height = key.height;
    keys[1].x = blocks[3].x + 20.f;
    keys[1].y = blocks[3].y + 100.f;
    keys[1].width = key.width;
    keys[1].height = key.height;
    keys[2].x = blocks[8].x;
    keys[2].y = blocks[0].y - 200.f;
    keys[2].width = key.width;
    keys[2].height = key.height;

    float finishLine{blocks[numOfBlocks - 1].x + 330.f};

    // Rectangle block2{576.f, 320.f, 385.f, 64.f};
    // Rectangle block3{1088.f, 416.f, 192.f, 64.f};

    /*Texture2D background = LoadTexture("textures/background/background.png");

    const int numOfBg{10};
    AnimData backgrounds[numOfBg]{};

    for (int i = 0; i < numOfBg; i++)
    {
        backgrounds[i].rec.x = 0.f;
        backgrounds[i].rec.y = 0.f;
        backgrounds[i].rec.width = background.width;
        backgrounds[i].rec.height = background.height;
        backgrounds[i].pos.y = 0.f;
        backgrounds[i].pos.x = 0.f + (i * 928.f);
    }*/

    Camera2D camera = {0};
    camera.target = (Vector2){heroData.pos.x + 32.0f, heroData.pos.y + 32.0f};
    camera.offset = (Vector2){(screenWidth / 2.0f), (screenHeight / 2.0f)};
    camera.rotation = 0.0f;
    camera.zoom = 1.f;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        const float dT{GetFrameTime()};
        BeginDrawing();
        ClearBackground(GRAY);

        // ground check
        if (isOnGround(heroData, screenHeight))
        {
            heroData.pos.y = screenHeight - heroData.rec.height;
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // rect is in air, apply gravity
            // multiply velocity when applying gravity
            velocity += gravity * dT;
            isInAir = true;
        }
        // collisions
        float pad{20};
        Rectangle heroRec{
            heroData.pos.x + pad,
            heroData.pos.y,
            heroData.rec.width - pad * 2,
            heroData.rec.height,
        };

        if (CheckCollisionRecs(heroRec, keys[0]))
        {
            gotKey1 = true;
            gotKeys = false;
        }
        if (CheckCollisionRecs(heroRec, keys[1]))
        {
            gotKey2 = true;
            gotKeys = false;
        }
        if (CheckCollisionRecs(heroRec, keys[2]))
        {
            gotKey3 = true;
            gotKeys = false;
        }
        if (gotKey1 && gotKey2 && gotKey3)
        {
            gotKeys = true;
        }

        // collision with ledges
        for (int i = 0; i < numOfBlocks; i++)
        {
            if (CheckCollisionRecs(heroRec, blocks[i]))
            {
                // on ledge
                if (isOnLedge(heroRec, blocks[i]))
                {
                    // heroData.pos.y = blocks[i].y - heroData.rec.height;
                    velocity = 0;
                    isInAir = false;
                }

                // under ledge
                if (isUnderLedge(heroRec, blocks[i]))
                {
                    heroData.pos.y = blocks[i].y + blocks[i].height;
                    velocity = 0;
                    isInAir = true;
                }
            }
        }

        Rectangle enemyRec{
            enemyData.pos.x + pad * 5,
            enemyData.pos.y + pad * 5,
            enemyData.rec.width - pad * 11,
            enemyData.rec.height - pad * 10,
        };

        // enemy collision
        if (CheckCollisionRecs(heroRec, enemyRec))
        {
            heroData.health--;
            heroData.pos.x -= 150;
            heroData.pos.y -= 100;
            if (heroData.health < 0)
            {
                isAlive = false;
            }
        }

        //  hero update animframe
        if (isInAir)
        {
            jumpData = updateAnimData(jumpData, dT, 2);
        }
        else
        {
            if (isRunning)
            {
                runData = updateAnimData(runData, dT, 5);
            }
            else
            {
                heroData = updateAnimData(heroData, dT, 3);
            }
        }

        enemyData = updateAnimData(enemyData, dT, 7);

        // movement
        // player movement
        if (IsKeyDown(KEY_RIGHT))
        {
            heroData.pos.x += direction;
            isRunning = true;
            // isLeft = false;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            heroData.pos.x -= direction;
            isRunning = true;
            // isLeft = true;
        }
        else
        {
            isRunning = false;
        }
        if (IsKeyDown(KEY_UP) && !isInAir)
        {
            velocity += jumpVel;
        }

        // key movement

        keys[0].y += levitate;
        if (keys[0].y + keys[0].height > 250.f || keys[0].y < 150)
        {
            levitate = -levitate;
        }
        keys[1].x += levitate;
        if (keys[1].x + keys[1].width > blocks[3].width || keys[1].x < blocks[3].x + 50.f)
        {
            levitate = -levitate;
        }
        keys[2].x += levitate;
        if (keys[2].x + keys[2].width > blocks[7].width || keys[2].x < blocks[7].x)
        {
            levitate = -levitate;
        }

        // update camera target
        camera.target = (Vector2){heroData.pos.x + 32.0f, heroData.pos.y + 32.0f};

        // update Y pos of character
        heroData.pos.y += velocity * dT;

        BeginMode2D(camera);

        for (int i = 0; i < numOfBlocks; i++)
        {
            DrawRectangle(blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].height, BROWN);
        }
        DrawRectangle(door.x, door.y, door.width, door.height, BLACK);

        // draw character
        Texture2D *sprite = NULL;

        if (isInAir)
            sprite = &heroJump;
        else
        {
            if (isRunning)
                sprite = &heroRun;
            else
                sprite = &heroIdle;
        }

        AnimData *spriteData = NULL;
        if (isInAir)
            spriteData = &jumpData;
        else
        {
            if (isRunning)
                spriteData = &runData;
            else
                spriteData = &heroData;
        }
        /*if (isLeft)
        {
            spriteData->rec.width *= -1;
        }*/
        if (isAlive)
        {
            DrawTextureRec(*sprite, spriteData->rec, heroData.pos, WHITE);
        }

        // draw keys
        if (!gotKey1)
        {
            DrawRectangle(keys[0].x, keys[0].y, keys[0].width, keys[0].height, RED);
        }
        if (!gotKey2)
        {
            DrawRectangle(keys[1].x, keys[1].y, keys[1].width, keys[1].height, BLUE);
        }
        if (!gotKey3)
        {
            DrawRectangle(keys[2].x, keys[2].y, keys[2].width, keys[2].height, YELLOW);
        }

        // draw enemy
        DrawTextureRec(enemy, enemyData.rec, enemyData.pos, WHITE);

        EndMode2D();

        // end of stage
        if (stageEnd(heroRec, finishLine, blocks[9]))
        {
            if (gotKeys)
            {
                DrawText("LEVEL COMPLETE", screenWidth / 2 - 100, screenHeight / 2, 24, RED);
                direction = 0;
                jumpVel = 0;
                isRunning = false;
            }
            else
            {
                DrawText("KEY MISSING", screenWidth / 2 - 100, screenHeight / 2, 24, RED);
            }
        }
        else if (!isAlive)
        {
            DrawText("GAME OVER", screenWidth / 2 - 100, screenHeight / 2, 24, RED);
            direction = 0;
            jumpVel = 0;
            isRunning = false;
        }

        EndDrawing();
    }
    // UnloadTexture(background);
    UnloadTexture(heroIdle);
    UnloadTexture(heroJump);
    UnloadTexture(heroRun);
    UnloadTexture(enemy);
}