#include <bits/stdc++.h>
#include "raylib.h"
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

using namespace std;

struct cell {
    int value;
    Color c1;
    Color c2;
};

int score = 0;
const int W = 800;
const int H = 1000;
const int bucW = 400;
const int bucH = 800;
const int cellSize = 40;
int startX = 360; 
int startY = 200;
int falinspeed = 40;
bool check = true;
int boardCol;
int boardRow;
float falltimer = 0.0f;

vector<vector<cell>> board(20, vector<cell>(10, {0, BLACK, BLACK}));
map<int, vector<vector<cell>>> m;
vector<vector<cell>> currentpiece;  
vector<int> rowsum(20, 0);

void embeddmatrix(vector<vector<cell>>&m1, const vector<vector<cell>>&m2, int startrow, int startcolumn, vector<int> &rowsum) {
    for (int r = 0; r < m2.size(); r++) {
        for (int c = 0; c < m2[r].size(); c++) {
            if(m2[r][c].value == 1) {
                m1[startrow+r][startcolumn+c] = m2[r][c];
                rowsum[startrow+r]++;
            }
        }
    }
}

void UpdateDrawFrame() {
    for (int i = 0; i < 20; i++) {
        if(rowsum[i] == 10) {
            score++;
            board.erase(board.begin() + i);
            board.insert(board.begin(), vector<cell>(10, {0,BLACK,BLACK}));
            rowsum.erase(rowsum.begin() + i);
            rowsum.insert(rowsum.begin(), 0);
            i--;
        }
    }
            
    boardCol = (startX - 200) / cellSize;
    boardRow = (startY - 200) / cellSize;
            
    falltimer += GetFrameTime();
    
    BeginDrawing();
    ClearBackground(BLACK); 
            
    DrawText("SCORE:", 280, 40, 20, RAYWHITE);
    DrawText(TextFormat("%d", score), 400, 40, 20, WHITE);

    DrawLine(200, 200, 200, 200+bucH, GRAY);
    DrawLine(200, 200+bucH, 200+bucW, 200+bucH, GRAY);
    DrawLine(600, 200, 200+bucW, 200+bucH, GRAY);

    bool lshift = true;
    bool rshift = true;

    if(boardCol <= 0) lshift = false;
    else if(board[boardRow][(boardCol - 1)].value == 1) lshift = false;

    if(boardCol + (int)currentpiece[0].size() >= 10) rshift = false;
    else if(board[boardRow][boardCol + currentpiece[0].size()].value == 1) rshift = false;

    if (IsKeyPressed(KEY_A) && lshift) { 
        startX -= cellSize; 
        falltimer = 0.0f; 
    }
    else if (IsKeyDown(KEY_A) && lshift) {
        falltimer += GetFrameTime();
        if (falltimer >= 0.5f) { startX -= cellSize; falltimer = 0.0f; }
    }

    if (IsKeyPressed(KEY_D) && rshift) { 
        startX += cellSize; 
        falltimer = 0.0f; 
    }
    else if (IsKeyDown(KEY_D) && rshift) {
        falltimer += GetFrameTime();
        if (falltimer >= 0.5f) { startX += cellSize; falltimer = 0.0f; }
    }

    if (IsKeyPressed(KEY_S)) { 
        startY += cellSize; 
        falltimer = 0.0f; 
    }
    else if (IsKeyDown(KEY_S)) {
        falltimer += GetFrameTime();
        if (falltimer >= 0.5f) { startY += cellSize; falltimer = 0.0f; }
    }
            
    if (IsKeyPressed(KEY_RIGHT)) {
        int rows = currentpiece.size();
        int cols = currentpiece[0].size();
        vector<vector<cell>> rotated(cols, vector<cell>(rows));
            
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                rotated[c][rows - 1 - r] = currentpiece[r][c];
            }
        }   
        currentpiece = rotated;

        int minX = 200;
        int maxX = 200 + (10 - (int)currentpiece[0].size()) * cellSize;
        if (startX < minX) startX = minX;
        if (startX > maxX) startX = maxX;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        int rows = currentpiece.size();
        int cols = currentpiece[0].size();
        vector<vector<cell>> rotated(cols, vector<cell>(rows));
            
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                rotated[cols - 1 - c][r] = currentpiece[r][c];
            }
        }
        currentpiece = rotated;

        int minX = 200;
        int maxX = 200 + (10 - (int)currentpiece[0].size()) * cellSize;
        if (startX < minX) startX = minX;
        if (startX > maxX) startX = maxX;
    }

    if(falltimer >= 0.5) {
        startY += falinspeed;
        falltimer = 0.0f;
    }
    
    for (int r = 0; r < currentpiece.size(); r++) {
        for (int c = 0; c < currentpiece[r].size(); c++) {
            if (currentpiece[r][c].value == 1) {
                DrawRectangle(startX + c*cellSize, startY + r*cellSize, cellSize, cellSize, currentpiece[r][c].c1);
                DrawRectangleLines(startX+ c*cellSize, startY+ r*cellSize, cellSize, cellSize, currentpiece[r][c].c2);
            }
        }
    }

    bool locked = false;
    if (boardRow + (int)currentpiece.size() >= 20) locked = true;
    if (!locked) {
        for (int r = 0; r < currentpiece.size() && !locked; r++) {
            for (int c = 0; c < currentpiece[r].size() && !locked; c++) {
                if (currentpiece[r][c].value == 1 && board[boardRow+r+1][boardCol+c].value == 1) {
                    locked = true;
                }
            }
        }
    }
    
    if (locked) {
        embeddmatrix(board, currentpiece, boardRow, boardCol, rowsum);
        int rn = (rand() % 7) + 1;
        currentpiece = m[rn];
        startX = 360;
        startY = 200;

        int spawnCol = (startX - 200) / cellSize;
        bool gameOver = false;
        for (int r = 0; r < currentpiece.size() && !gameOver; r++) {
            for (int c = 0; c < currentpiece[r].size() && !gameOver; c++) {
                if (currentpiece[r][c].value == 1 && board[r][spawnCol + c].value == 1)
                    gameOver = true;
            }
        }

        if (gameOver) {
            score = 0;
            board.assign(20, vector<cell>(10, {0, BLACK, BLACK}));
            rowsum.assign(20, 0);
            startX = 360;
            startY = 200;
            currentpiece = m[(rand() % 7) + 1];
        }
    }
            
    for (int r = 0; r < board.size(); r++) {
        for (int c = 0; c < board[r].size(); c++) {
            if(board[r][c].value == 1) {
                int xpos = 200+(c*cellSize);
                int ypos = 200+(r*cellSize);
                DrawRectangle(xpos, ypos, cellSize, cellSize, board[r][c].c1);
                DrawRectangleLines(xpos, ypos, cellSize, cellSize, board[r][c].c2);
            }
        }
    }
    EndDrawing();
}

int main() {
    srand(time(0));
    
    InitWindow(W, H, "TETRIS");

    Color hotPink      = {255,  45, 120, 255};
    Color darkMagenta  = {122,   0,  64, 255};
    Color electricCyan = {  0, 229, 255, 255};
    Color deepBlue     = {  0,  80, 160, 255};
    Color limeBlast    = {170, 255,   0, 255};
    Color forestGreen  = { 26, 102,   0, 255};
    Color goldenYellow = {255, 214,   0, 255};
    Color burntOrange  = {184,  74,   0, 255};
    Color softViolet   = {199, 125, 255, 255};
    Color deepIndigo   = { 58,   0, 122, 255};
    Color coralRed     = {255, 107, 107, 255};
    Color darkTeal     = {  0,  96,  96, 255};
    Color mintGreen    = {  0, 255, 178, 255};
    Color darkNavy     = {  0,  48,  96, 255};

    vector<vector<cell>> I = {
        {{1,hotPink,darkMagenta}},
        {{1,hotPink,darkMagenta}},
        {{1,hotPink,darkMagenta}},
        {{1,hotPink,darkMagenta}}
    };
    vector<vector<cell>> L = {
        {{1,electricCyan,deepBlue},{0,electricCyan,deepBlue}},
        {{1,electricCyan,deepBlue},{0,electricCyan,deepBlue}},
        {{1,electricCyan,deepBlue},{1,electricCyan,deepBlue}}
    };
    vector<vector<cell>> J = {
        {{0,limeBlast,forestGreen},{1,limeBlast,forestGreen}},
        {{0,limeBlast,forestGreen},{1,limeBlast,forestGreen}},
        {{1,limeBlast,forestGreen},{1,limeBlast,forestGreen}},
    };
    vector<vector<cell>> O = {
        {{1,goldenYellow,burntOrange},{1,goldenYellow,burntOrange}},
        {{1,goldenYellow,burntOrange},{1,goldenYellow,burntOrange}}
    };
    vector<vector<cell>> Z = {
        {{1,softViolet,deepIndigo},{1,softViolet,deepIndigo},{0,softViolet,deepIndigo}},
        {{0,softViolet,deepIndigo},{1,softViolet,deepIndigo},{1,softViolet,deepIndigo}}
    };
    vector<vector<cell>> T = {
        {{0,coralRed,darkTeal},{1,coralRed,darkTeal},{0,coralRed,darkTeal}},
        {{1,coralRed,darkTeal},{1,coralRed,darkTeal},{1,coralRed,darkTeal}}
    };
    vector<vector<cell>> S = {
        {{0,mintGreen,darkNavy},{1,mintGreen,darkNavy},{1,mintGreen,darkNavy}},
        {{1,mintGreen,darkNavy},{1,mintGreen,darkNavy},{0,mintGreen,darkNavy}}
    };

    m[1] = I;
    m[2] = L;
    m[3] = J;
    m[4] = O;
    m[5] = Z;
    m[6] = T;
    m[7] = S;

    currentpiece = m[(rand() % 7) + 1]; 

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
    #else
        SetTargetFPS(120);
        while (!WindowShouldClose()) {
            UpdateDrawFrame();
        }
    #endif

    CloseWindow(); 
    return 0;
}
