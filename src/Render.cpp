#include "Render.h"
#include "SongDatabase.h"
#include <raylib.h>
#include <algorithm>
#include <cmath> // 新增，支援 std::fmod

static Texture2D backgroundTexture;
static Texture2D scoreBackgroundTexture; // 如果成績畫面有專用背景

// 🔵 加載與卸載背景資源
void LoadGameAssets()
{
    backgroundTexture = LoadTexture("resources/Gamebg.jpg");
    scoreBackgroundTexture = LoadTexture("resources/ScoreBg.png");
}

void UnloadGameAssets()
{
    UnloadTexture(backgroundTexture);
    UnloadTexture(scoreBackgroundTexture);
}

float Clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

void DrawCountdown(double countdownStartTime)
{
    double elapsed = GetTime() - countdownStartTime;
    int count = 3 - static_cast<int>(elapsed);
    if (elapsed >= 3.0 && elapsed < 4.0)
    {
        DrawText("GO!", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 40, 80, RED);
    }
    else if (count >= 0)
    {
        DrawText(TextFormat("%d", count + 1), SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT / 2 - 40, 80, BLACK);
    }
}

void DrawGame(const GameState &gameState, const InputState &inputState) // 🟢 傳入整個 GameState
{
    ClearBackground(WHITE);
    DrawTexture(backgroundTexture, 0, 0, WHITE);

    if (gameState.isCountdown)
    {
        DrawCountdown(gameState.countdownStartTime);
        return;
    }

    // 繪製軌道和判定線 (這部分邏輯不變，但可以從 gameState 取值)
    for (int i = 0; i < LANE_COUNT; i++)
    {
        Color laneColor = {200, 200, 200, 180};
        DrawRectangle(i * LANE_WIDTH, 0, LANE_WIDTH, SCREEN_HEIGHT, laneColor);
    }

    double currentTime = GetTime() - gameState.musicStartTime;
    float beatProgress = std::fmod(currentTime * gameState.currentBpm / 60.0f, 1.0f);
    int brightness = 255 - static_cast<int>(beatProgress * 100);
    Color judgeColor = {255, static_cast<unsigned char>(brightness), static_cast<unsigned char>(brightness), 255};
    DrawLine(0, JUDGE_LINE_Y, SCREEN_WIDTH, JUDGE_LINE_Y, judgeColor);

    DrawBlocks(gameState.blocks);
    DrawText(TextFormat("Score: %d", gameState.score), 10, 10, 30, YELLOW);

    // 🆕 繪製判定結果提示
    if (gameState.lastHitRating != HitRating::NONE && gameState.hitRatingDisplayTime > 0)
    {
        const char *ratingText = "";
        Color ratingColor = WHITE;
        int fontSize = 50; // 基礎字體大小

        switch (gameState.lastHitRating)
        {
        case HitRating::PERFECT:
            ratingText = "PERFECT";
            ratingColor = Color{255, 215, 0, 255}; // 金色 (Gold)
            fontSize = 60;                         // Perfect 可以稍大
            break;
        case HitRating::GOOD:
            ratingText = "GOOD";
            ratingColor = Color{50, 205, 50, 255}; // 青檸檬綠 (LimeGreen)
            break;
        case HitRating::MISS:
            ratingText = "MISS";
            ratingColor = Color{220, 20, 60, 255}; // 猩紅 (Crimson)
            fontSize = 45;                         // Miss 可以稍小
            break;
        case HitRating::BAD:
            ratingText = "BAD";
            ratingColor = Color{169, 169, 169, 255}; // 暗灰色 (DarkGray)
            fontSize = 45;                           // Bad 可以稍小
            break;
        case HitRating::NONE: // 理論上不會進入這裡，因為有 gameState.lastHitRating != HitRating::NONE 判斷
        default:
            break;
        }

        if (TextLength(ratingText) > 0)
        {
            // 動態效果：簡單的淡出
            float alphaFactor = gameState.hitRatingDisplayTime / GameState::HIT_RATING_DURATION;
            // 可以讓它在快結束時快速淡出
            if (alphaFactor < 0.3f)
            {                                     // 最後30%的時間
                alphaFactor = alphaFactor / 0.3f; // 重新映射到 0-1
            }
            else
            {
                alphaFactor = 1.0f;
            }
            alphaFactor = Clamp(alphaFactor, 0.0f, 1.0f); // 確保在 0-1 之間

            // 簡單的縮放效果：開始時大一點，然後縮回正常，消失時再縮小
            float scaleFactor = 1.0f;
            float timeProgress = 1.0f - (gameState.hitRatingDisplayTime / GameState::HIT_RATING_DURATION); // 0 (剛出現) to 1 (快消失)

            if (timeProgress < 0.2f)
            {                                                      // 初始放大 (前20%時間)
                scaleFactor = 1.0f + (0.2f - timeProgress) * 1.5f; // 從1.3倍縮到1.0倍
            }
            else if (timeProgress > 0.8f)
            {                                                      // 快消失時縮小 (後20%時間)
                scaleFactor = 1.0f - (timeProgress - 0.8f) * 2.5f; // 從1.0倍縮到0.5倍
            }
            scaleFactor = Clamp(scaleFactor, 0.1f, 1.3f);

            int currentFontSize = (int)(fontSize * scaleFactor);
            if (currentFontSize < 10)
                currentFontSize = 10; // 最小字體

            int textWidth = MeasureText(ratingText, currentFontSize);
            DrawText(ratingText,
                     SCREEN_WIDTH / 2 - textWidth / 2,
                     JUDGE_LINE_Y - 100 - currentFontSize / 2, // 在判定線上方一些
                     currentFontSize,
                     Fade(ratingColor, alphaFactor));
        }
    }

    // 🆕 處理遊戲結束時的漸暗效果
    if (gameState.gameEnding)
    {
        double timeSinceEnd = GetTime() - gameState.gameEndTime;
        float fadeDuration = 2.0f; // 與 Game.cpp 中的一致
        float alpha = Clamp((float)timeSinceEnd / fadeDuration, 0.0f, 1.0f);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, alpha));
        TraceLog(LOG_DEBUG, "Drawing fade out: alpha = %.2f", alpha);
    }
}

void DrawIntroScene(GameState &state)
{
    static Texture2D logo1 = LoadTexture("resources/logo1.jpg");
    static Texture2D logo2 = LoadTexture("resources/logo2.png");
    double time = GetTime() - state.introStartTime;

    float scale = 0.25f; // 縮小比例

    ClearBackground(WHITE);

    if (time < 2.0)
    {
        float alpha = time / 2.0f;
        float scaledW = logo1.width * scale;
        float scaledH = logo1.height * scale;
        Vector2 position = {
            (SCREEN_WIDTH - scaledW) / 2,
            (SCREEN_HEIGHT - scaledH) / 2};
        DrawTextureEx(logo1, position, 0.0f, scale, Fade(WHITE, alpha));
    }
    else if (time < 4.0)
    {
        float alpha = 1.0f - (time - 2.0f) / 2.0f;
        float scaledW = logo1.width * scale;
        float scaledH = logo1.height * scale;
        Vector2 position = {
            (SCREEN_WIDTH - scaledW) / 2,
            (SCREEN_HEIGHT - scaledH) / 2};
        DrawTextureEx(logo1, position, 0.0f, scale, Fade(WHITE, alpha));
    }
    else if (time < 6.0)
    {
        float alpha = (time - 4.0f) / 2.0f;
        float scaledW = logo2.width * scale;
        float scaledH = logo2.height * scale;
        Vector2 position = {
            (SCREEN_WIDTH - scaledW) / 2,
            (SCREEN_HEIGHT - scaledH) / 2};
        DrawTextureEx(logo2, position, 0.0f, scale, Fade(WHITE, alpha));
    }
    else if (time < 8.0)
    {
        float alpha = 1.0f - (time - 6.0f) / 2.0f;
        float scaledW = logo2.width * scale;
        float scaledH = logo2.height * scale;
        Vector2 position = {
            (SCREEN_WIDTH - scaledW) / 2,
            (SCREEN_HEIGHT - scaledH) / 2};
        DrawTextureEx(logo2, position, 0.0f, scale, Fade(WHITE, alpha));
    }
    else if (time < 9.0)
    {
        // 保持 logo2 的最後一幀，避免畫面清空
        float scaledW = logo2.width * scale;
        float scaledH = logo2.height * scale;
        Vector2 position = {
            (SCREEN_WIDTH - scaledW) / 2,
            (SCREEN_HEIGHT - scaledH) / 2};

        // 淡出到黑色（改用黑色遮罩）
        float fadeOutAlpha = (time - 8.0f) / 1.0f;
        DrawRectangleRec((Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                         Fade(BLACK, fadeOutAlpha)); // 改用黑色淡出，與主選單淡入一致
    }
    else
    {
        // 增加一個短暫的完全黑色狀態，確保平滑銜接
        DrawRectangleRec((Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, BLACK);
        UnloadTexture(logo1);
        UnloadTexture(logo2);
        state.currentScene = Scene::MainMenu;
    }
}

void DrawMainMenu(GameState &state)
{
    static Texture2D bg = LoadTexture("resources/RCadeBg.png");
    static Texture2D bg2 = LoadTexture("resources/RCadeBg2.png");

    static bool showSecondBg = false;
    static float secondBgTimer = 0.0f;

    static float transitionAlpha = 1.0f;
    static bool firstEnter = true;

    // Zoom 動畫參數
    static float bgZoom = 1.0f;
    const float bgZoomSpeed = 0.04f;
    const float maxZoom = 1.4f;

    // 過渡動畫速度
    const float transitionSpeed = 0.7f;

    // 第二張圖淡入與停留時間
    const float crossfadeDuration = 0.6f; // 淡入時間
    const float displayDuration = 1.0f;   // 停留時間

    if (firstEnter)
    {
        transitionAlpha = 1.0f;
        firstEnter = false;
        showSecondBg = false;
        secondBgTimer = 0.0f;
        bgZoom = 1.0f;
    }

    // 點擊觸發第二張背景
    if (!state.transitioning && !showSecondBg && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        showSecondBg = true;
        secondBgTimer = 0.0f;
    }

    // 第二張圖計時與觸發場景轉換
    if (showSecondBg)
    {
        secondBgTimer += GetFrameTime();
        if (secondBgTimer >= crossfadeDuration + displayDuration && !state.transitioning)
        {
            state.transitioning = true;
        }
    }

    // 純淡入淡出控制
    if (!state.transitioning && transitionAlpha > 0.0f)
    {
        transitionAlpha -= GetFrameTime() * transitionSpeed;
        if (transitionAlpha < 0.0f)
            transitionAlpha = 0.0f;
    }
    if (state.transitioning)
    {
        transitionAlpha += GetFrameTime() * transitionSpeed;
        if (transitionAlpha >= 1.0f)
        {
            transitionAlpha = 1.0f;
            state.currentScene = Scene::SongSelect;
            state.transitioning = false;
            firstEnter = true;
            showSecondBg = false;
            return;
        }
    }

    // ==== 背景 (第一張) 縮放與平移 ====
    if (!showSecondBg && bgZoom < maxZoom)
    {
        bgZoom += GetFrameTime() * bgZoomSpeed;
        if (bgZoom > maxZoom)
            bgZoom = maxZoom;
    }

    float scale = std::max((float)SCREEN_WIDTH / bg.width, (float)SCREEN_HEIGHT / bg.height) * bgZoom;
    float resizedW = bg.width * scale;
    float resizedH = bg.height * scale;
    float offX = (resizedW - SCREEN_WIDTH) / 2;
    float offY = (resizedH - SCREEN_HEIGHT) / 2;
    Rectangle src = {offX / scale, offY / scale, (float)SCREEN_WIDTH / scale, (float)SCREEN_HEIGHT / scale};
    Rectangle dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    DrawTexturePro(bg, src, dst, (Vector2){0, 0}, 0.0f, WHITE);

    // ==== 第二張背景淡入覆蓋 ====
    if (showSecondBg)
    {
        float t = secondBgTimer;
        float alpha = (t < crossfadeDuration) ? (t / crossfadeDuration) : 1.0f;
        if (alpha > 1.0f)
            alpha = 1.0f;

        float s2 = std::max((float)SCREEN_WIDTH / bg2.width, (float)SCREEN_HEIGHT / bg2.height);
        float rW2 = bg2.width * s2;
        float rH2 = bg2.height * s2;
        float oX2 = (rW2 - SCREEN_WIDTH) / 2;
        float oY2 = (rH2 - SCREEN_HEIGHT) / 2;
        Rectangle src2 = {oX2 / s2, oY2 / s2, (float)SCREEN_WIDTH / s2, (float)SCREEN_HEIGHT / s2};

        DrawTexturePro(bg2, src2, dst, (Vector2){0, 0}, 0.0f, (Color){255, 255, 255, (unsigned char)(alpha * 255)});
    }

    // ==== R-CADE Logo 心跳效果 ====
    static float ht = 0.0f;
    ht += GetFrameTime();
    float pulse = 1.0f + 0.02f * sinf(ht * 3.0f);
    int fs = (int)(100 * pulse);
    const char *logo = "R-CADE";
    int lw = MeasureText(logo, fs);
    int lx = (SCREEN_WIDTH - lw) / 2;
    int ly = SCREEN_HEIGHT / 3;
    float lp = 0.05f * sinf(GetTime() * 2);
    float laf = 0.15f + lp;
    unsigned char la = (unsigned char)(laf * 80);
    unsigned char g1 = (unsigned char)(laf * 40);
    unsigned char g2 = (unsigned char)(laf * 20);
    DrawText(logo, lx + 3, ly + 3, fs, (Color){255, 255, 255, g2});
    DrawText(logo, lx + 1, ly + 1, fs, (Color){255, 255, 255, g1});
    DrawText(logo, lx, ly, fs, (Color){255, 255, 255, la});

    // ==== 閃爍文字 ====
    static float bt = 0.0f;
    bt += GetFrameTime();
    float ba = (sinf(bt * 2.0f) + 1.0f) / 2.0f;
    const char *msg = "-Click Anywhere to Start-";
    int fsz = 24;
    int tw = MeasureText(msg, fsz);
    DrawText(msg, (SCREEN_WIDTH - tw) / 2, SCREEN_HEIGHT - 120, fsz, (Color){255, 255, 255, (unsigned char)(ba * 255)});
    const char *tip = "-use headphones for better experience-";
    int tf = 18;
    int tw2 = MeasureText(tip, tf);
    DrawText(tip, (SCREEN_WIDTH - tw2) / 2, SCREEN_HEIGHT - 40, tf, WHITE);

    // ==== 黑色轉場遮罩 ====
    if (transitionAlpha > 0.0f)
    {
        DrawRectangleRec((Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, (Color){0, 0, 0, (unsigned char)(transitionAlpha * 255)});
    }
}

void DrawSongSelect(GameState &state)
{
    static Texture2D bgChoose = LoadTexture("resources/songChoosebg.jpg");
    static bool firstEnter = true;
    static float transAlpha = 1.0f;
    static bool fadingOut = false;
    static int selectedIndex = -1;
    const float transSpeed = 0.8f;

    const auto &songs = state.songDb.songs;

    if (firstEnter)
    {
        transAlpha = 1.0f;
        fadingOut = false;
        selectedIndex = -1;
        firstEnter = false;
    }

    if (!fadingOut && transAlpha > 0.0f)
    {
        transAlpha = Clamp(transAlpha - GetFrameTime() * transSpeed, 0.0f, 1.0f);
    }

    if (fadingOut)
    {
        transAlpha = Clamp(transAlpha + GetFrameTime() * transSpeed, 0.0f, 1.0f);
        if (transAlpha >= 1.0f)
        {
            InitializeGame(state, songs[selectedIndex].name);
            state.currentScene = Scene::Playing;
            // 重置狀態
            firstEnter = true; // 確保下次進入時重新初始化
            fadingOut = false;
            selectedIndex = -1;
            return;
        }
    }

    // 繪製背景
    float scaleBg = std::max((float)SCREEN_WIDTH / bgChoose.width, (float)SCREEN_HEIGHT / bgChoose.height);
    Rectangle srcBg = {
        (bgChoose.width - SCREEN_WIDTH / scaleBg) * 0.5f,
        (bgChoose.height - SCREEN_HEIGHT / scaleBg) * 0.5f,
        SCREEN_WIDTH / scaleBg,
        SCREEN_HEIGHT / scaleBg};
    Rectangle dstBg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    DrawTexturePro(bgChoose, srcBg, dstBg, {0, 0}, 0.0f, Fade(WHITE, 1.0f - transAlpha));

    // 繪製標題
    const char *title = "- Song List -";
    int titleSize = 36;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (SCREEN_WIDTH - titleW) / 2, 40, titleSize, Fade(WHITE, 1.0f - transAlpha));

    // 繪製歌曲列表
    Vector2 mouse = GetMousePosition();

    const int btnW = 400, btnH = 50, spacing = 20;
    const float totalHeight = songs.size() * (btnH + spacing) - spacing;
    const float halfScreenH = SCREEN_HEIGHT * 0.5f;
    float rawOff = -(mouse.y - halfScreenH);
    float minOff = std::min(0.0f, SCREEN_HEIGHT - totalHeight);
    float offsetY = Clamp(rawOff, minOff, 0.0f);

    for (int i = 0; i < (int)songs.size(); ++i)
    {
        float y = halfScreenH - totalHeight * 0.5f + i * (btnH + spacing) + offsetY;
        float x = (SCREEN_WIDTH - btnW) * 0.5f;
        Rectangle r = {x, y, (float)btnW, (float)btnH};
        bool hovered = CheckCollisionPointRec(mouse, r);

        float sc = hovered ? 1.1f : 1.0f;
        Vector2 pos = {r.x + (btnW - btnW * sc) * 0.5f, r.y + (btnH - btnH * sc) * 0.5f};
        Rectangle rs = {pos.x, pos.y, btnW * sc, btnH * sc};

        DrawRectangleRounded(rs, 0.2f, 6, Fade(GRAY, 1.0f - transAlpha));
        DrawText(songs[i].name.c_str(),
                 pos.x + 12,
                 pos.y + (rs.height - 24) * 0.5f,
                 24,
                 Fade(WHITE, 1.0f - transAlpha));

        if (!fadingOut && hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selectedIndex = i;
            fadingOut = true;
        }
    }
}

// 🆕 實現繪製成績畫面的函數
void DrawScoreScreen(const GameState &state)
{
    ClearBackground(DARKGRAY); // 或者使用背景圖
    // DrawTexture(scoreBackgroundTexture, 0, 0, WHITE);

    // 顯示最終分數
    const char *scoreText = TextFormat("FINAL SCORE: %d", state.score); // 🟢 使用 gameState.score
    int scoreTextWidth = MeasureText(scoreText, 60);
    DrawText(scoreText, (SCREEN_WIDTH - scoreTextWidth) / 2, SCREEN_HEIGHT / 2 - 50, 60, YELLOW);

    // 其他信息，例如：評價 (Perfect, Good, Miss...)，最大連擊等 (未來可以擴展)

    // 返回主選單或選歌介面的提示
    const char *continueText = "Press ENTER to return to Main Menu";
    int continueTextWidth = MeasureText(continueText, 20);
    DrawText(continueText, (SCREEN_WIDTH - continueTextWidth) / 2, SCREEN_HEIGHT - 60, 20, LIGHTGRAY);
}