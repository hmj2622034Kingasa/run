#include "DxLib.h"

enum Scene
{
    TITLE,
    RUN,
    JUMP,
    RESULT
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    SetGraphMode(1280, 720, 32);

    if (DxLib_Init() == -1)
        return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    Scene scene = TITLE;

    // プレイヤー
    float playerX = 100;
    float playerY = 500;
    float speed = 3.0f;

    // パワー
    int power = 0;

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
    char prevSpace = 0;
    char prevZ = 0;
    char prevEnter = 0;

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        ClearDrawScreen();

        // 現在キー
        char nowSpace = CheckHitKey(KEY_INPUT_SPACE);
        char nowZ = CheckHitKey(KEY_INPUT_Z);
        char nowEnter = CheckHitKey(KEY_INPUT_RETURN);

        switch (scene)
        {
        case TITLE:

            DrawString(500, 200, "HASIRITAKATOBI", GetColor(255, 255, 255));
            DrawString(475, 300, "PRESS SPACE TO START", GetColor(255, 255, 0));

            if (nowSpace && !prevSpace)
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

            // Z連打（押した瞬間だけ）
            if (nowZ && !prevZ)
            {
                power++;
            }

            // 地面・踏切
            DrawLine(0, 550, 1280, 550, GetColor(255, 255, 255));
            DrawLine((int)lineX, 450, (int)lineX, 550, GetColor(255, 0, 0));

            // プレイヤー
            DrawBox(
                (int)playerX, (int)playerY,
                (int)playerX + 40, (int)playerY + 50,
                GetColor(0, 255, 255),
                TRUE);

            DrawFormatString(30, 30, GetColor(255, 255, 255),
                "Power: %d", power);

            DrawString(30, 60, "PRESS SPACE TO JUMP", GetColor(255, 255, 0));

            // ジャンプ（押した瞬間）
            if (nowSpace && !prevSpace)
            {
                scene = JUMP;

                jumpX = playerX;
                jumpY = playerY;

                vx = power * 0.25f;
                vy = -12.0f;

                foul = (playerX > lineX);
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

            DrawBox(
                (int)jumpX, (int)jumpY,
                (int)jumpX + 40, (int)jumpY + 50,
                GetColor(0, 255, 255),
                TRUE);

            break;

        case RESULT:

            DrawString(520, 200, "RESULT", GetColor(255, 255, 0));

            if (foul)
            {
                DrawString(520, 300, "FOUL!", GetColor(255, 0, 0));
            }
            else
            {
                DrawFormatString(480, 300,
                    GetColor(255, 255, 255),
                    "Record: %.2f m", record);
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

        // 前フレーム更新
        prevSpace = nowSpace;
        prevZ = nowZ;
        prevEnter = nowEnter;

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}