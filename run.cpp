#include "DxLib.h"

enum Scene
{
    TITLE,
    RUN,
    JUMP,
    RESULT
};

struct SandParticle
{
    float x, y;
    float vx, vy;
    bool active;
};

int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    SetGraphMode(1280, 720, 32);

    if (DxLib_Init() == -1)
        return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    // 画像読み込み
    //int bgImg = LoadGraphWithCheck("undoukai_tokyousou_white.png");
    //int fieldImg = LoadGraphWithCheck("rikujou_sandantobi.png");
    int runImg = LoadGraphWithCheck("走り幅跳び　走る.png");
    int jumpImg = LoadGraphWithCheck("走り幅跳び　飛ぶ.png");
    int schoolImg = LoadGraphWithCheck("学校背景.jpg");

    Scene scene = TITLE;

    const int MAX_SAND = 200;
    SandParticle sand[MAX_SAND];

    for (int i = 0; i < MAX_SAND; i++)
    {
        sand[i].active = false;
    }

    // プレイヤー
    float playerX = 100;
    float playerY = 500;
    float speed = 3.0f;

    // パワー
    int power = 0;

    // サウンドの読み込みと音量設定
    int se = LoadSoundMem("AS_40475_【チャイム＿学校02-02】.mp3");
    int se2 = LoadSoundMem("AS_53089_警官のホイッスルを2回吹く.mp3");

    // ジャンプ
    float jumpX = 0;
    float jumpY = 0;
    float vx = 0;
    float vy = 0;
    const float gravity = 0.6f;

    // ルール
    float lineX = 900.0f;
    bool foul = false;
    float record = 0.0f;

    // キー入力（前フレーム）
    char prevUp = 0;
    char prevSpace = 0;
    char prevEnter = 0;

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();
        for (int i = 0; i < MAX_SAND; i++)
        {
            if (!sand[i].active)
                continue;

            sand[i].x += sand[i].vx;
            sand[i].y += sand[i].vy;

            sand[i].vy += 0.2f;

            if (sand[i].y > 550)
                sand[i].active = false;
        }

        //DrawGraph(0, 0, bgImg, TRUE);
        //DrawGraph(10, 10, fieldImg, TRUE);
        DrawExtendGraph(
            0, 0,
            1280, 720,
            schoolImg,
            TRUE
        );

        // 現在キー
        char nowSpace = CheckHitKey(KEY_INPUT_SPACE);
        char nowUp = CheckHitKey(KEY_INPUT_UP);
        char nowEnter = CheckHitKey(KEY_INPUT_RETURN);

        switch (scene)
        {
        case TITLE:

            DrawString(500, 200, "HASIRIHABATOBI", GetColor(255, 255, 255));
            DrawString(488, 300, "PRESS UP TO START", GetColor(255, 255, 0));

            if (nowUp && !prevUp)
            {
                scene = RUN;
                playerX = 100;
                power = 0;
                foul = false;
            }

            break;

        case RUN:

            // 自動走行
            playerX += speed;

            // Shift連打（押した瞬間だけ）
            if (nowUp && !prevUp)
            {
                power++;
            }

            // 地面・踏切
            DrawLine(0, 550, 1280, 550, GetColor(255, 255, 255));
            DrawLine((int)lineX, 450, (int)lineX, 550, GetColor(0, 0, 0));

            // プレイヤー
            DrawExtendGraph(
                (int)playerX,
                (int)playerY,
                (int)playerX + 50,
                (int)playerY + 60,
                runImg,
                TRUE);

            DrawFormatString(30, 30, GetColor(255, 255, 255),
                "Power: %d", power);

            DrawString(30, 60, "PRESS SPACE TO JUMP", GetColor(255, 255, 0));
            DrawString(30, 10, "PRESS UP to PowerCharge", GetColor(255, 255, 0));;
            DrawString(893, 425, "↓Don't over here!", GetColor(255, 0, 0));

            // ジャンプ（押した瞬間）
            if (nowSpace && !prevSpace)
            {
                scene = JUMP;

                jumpX = playerX;
                jumpY = playerY;

                vx = power * 0.25f;
                vy = -12.0f;

                foul = (playerX + 25 > lineX);
            }

            break;

        case JUMP:

            // 物理
            jumpX += vx;
            jumpY += vy;
            vy += gravity;

            // 地面着地
            if (jumpY >= 500)
            {
                jumpY = 500;
                int sandAmount = 20 + (int)(record * 15);

                if (sandAmount > MAX_SAND)
                    sandAmount = MAX_SAND;

                for (int i = 0; i < sandAmount; i++)
                {
                    sand[i].active = true;

                    sand[i].x = jumpX;
                    sand[i].y = 550;

                    sand[i].vx = (GetRand(100) - 50) / 10.0f;
                    sand[i].vy = -(GetRand(60) / 10.0f);
                }

                if (foul)
                {
                    record = 0;
                }
                else
                {
                    record = (jumpX - lineX) / 50.0f;
                    if (record < 0) record = 0;
                }

                scene = RESULT;
            }

            // 描画
            DrawLine(0, 550, 1280, 550, GetColor(255, 255, 255));
            DrawLine((int)lineX, 450, (int)lineX, 550, GetColor(255, 0, 0));

            DrawExtendGraph(
                (int)jumpX,
                (int)jumpY,
                (int)jumpX + 50,
                (int)jumpY + 60,
                jumpImg,
                TRUE);

            break;

        case RESULT:

            DrawString(520, 200, "RESULT", GetColor(255, 255, 0));

            if (foul)
            {
                DrawString(520, 300, "FOUL!", GetColor(255, 0, 0));
                PlaySoundMem(se2, DX_PLAYTYPE_BACK);
            }
            else
            {
                DrawFormatString(480, 300,
                    GetColor(0, 0, 255),
                    "Record: %.2f m", record);
                PlaySoundMem(se, DX_PLAYTYPE_BACK);
            }

            DrawString(435, 450,
                "PRESS ENTER TO RETURN TITLE",
                GetColor(255, 255, 255));

            if (nowEnter && !prevEnter)
            {
                scene = TITLE;
                playerX = 100;
                power = 0;
                record = 0;
                foul = false;
            }

            break;
        }

        for (int i = 0; i < MAX_SAND; i++)
        {
            if (!sand[i].active)
                continue;

            DrawCircle(
                (int)sand[i].x,
                (int)sand[i].y,
                2,
                GetColor(220, 200, 120),
                TRUE);
        }

        // 前フレーム更新
        prevUp = nowUp;
        prevSpace = nowSpace;
        prevEnter = nowEnter;

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}

int LoadGraphWithCheck(const char* file)
{
    int res = LoadGraph(file);
    if (res == -1)
    {
        MessageBox(GetMainWindowHandle(), file, "画像読み込みに失敗", MB_OK | MB_ICONSTOP);
    }
    return res;
}

// 音声の読み込み、読み込み失敗時は通知
int LoadSoundMemWithCheck(const char* file)
{
    int res = LoadSoundMem(file);
    if (res == -1)
    {
        MessageBox(GetMainWindowHandle(), file, "音声読み込みに失敗", MB_OK | MB_ICONSTOP);
    }
    return res;
}