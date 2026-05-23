#include <3ds.h>
#include <citro2d.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#define SCREEN_WIDTH_TOP  400
#define SCREEN_HEIGHT_TOP 240
#define SCREEN_WIDTH_BOT  320
#define SCREEN_HEIGHT_BOT 240

// Colors
#define CLR_TASKBAR      C2D_Color32(10, 30, 60, 200)
#define CLR_WIN7_BLUE    C2D_Color32(0, 120, 215, 255)
#define CLR_WHITE        C2D_Color32(255, 255, 255, 255)
#define CLR_BLACK        C2D_Color32(0, 0, 0, 255)
#define CLR_WIN_FRAME    C2D_Color32(200, 220, 240, 230)
#define CLR_WIN_TITLE    C2D_Color32(30, 80, 150, 255)
#define CLR_START_MENU   C2D_Color32(240, 245, 255, 240)

// App Types
enum AppType { NONE, NOTEPAD, CALC, PAINT, MINESWEEPER, BROWSER, MEDIA_PLAYER };

struct Window {
    AppType type;
    float x, y, w, h;
    bool active;
    const char* title;
};

// Global State
bool startMenuOpen = false;
AppType activeApp = NONE;
std::vector<Window> windows;
char timeStr[10];

void updateTime() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeStr, sizeof(timeStr), "%H:%M", timeinfo);
}

void drawWindow(Window& win) {
    if (!win.active) return;
    // Window Frame
    C2D_DrawRectSolid(win.x, win.y, 0, win.w, win.h, CLR_WIN_FRAME);
    // Title Bar
    C2D_DrawRectSolid(win.x, win.y, 0, win.w, 20, CLR_WIN_TITLE);
    // Close Button
    C2D_DrawRectSolid(win.x + win.w - 20, win.y, 0, 20, 20, C2D_Color32(230, 50, 50, 255));
}

int main(int argc, char* argv[]) {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateMainTarget(GFX_TOP, GFX_LEFT, C2D_TARGET_CLEAR);
    C3D_RenderTarget* bottom = C2D_CreateMainTarget(GFX_BOTTOM, GFX_LEFT, C2D_TARGET_CLEAR);

    touchPosition touch;
    float cursorX = SCREEN_WIDTH_BOT / 2;
    float cursorY = SCREEN_HEIGHT_BOT / 2;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (hidKeysHeld() & KEY_TOUCH) {
            hidTouchRead(&touch);
            cursorX = touch.px;
            cursorY = touch.py;
            
            // Check Start Button click
            if (kDown & KEY_TOUCH) {
                if (touch.px < 40 && touch.py > SCREEN_HEIGHT_BOT - 40) {
                    startMenuOpen = !startMenuOpen;
                }
                // Simple Window Close Check
                for (auto &win : windows) {
                    if (win.active && touch.px > win.x + win.w - 20 && touch.px < win.x + win.w && touch.py > win.y && touch.py < win.y + 20) {
                        win.active = false;
                    }
                }
                
                // Start Menu Items
                if (startMenuOpen && touch.px > 10 && touch.px < 150) {
                    if (touch.py > 100 && touch.py < 120) { // Notepad
                        windows.push_back({NOTEPAD, 50, 50, 200, 120, true, "Notepad"});
                        startMenuOpen = false;
                    }
                    // Add other apps similarly...
                }
            }
        }

        updateTime();

        C3D_FrameBegin(C3D_FRAME_SYNCHRPARAMS);

        // --- Top Screen ---
        C2D_TargetClear(top, CLR_WIN7_BLUE);
        C2D_SceneBegin(top);
        // Draw Desktop Icons (Placeholder)
        for(int i=0; i<3; i++) C2D_DrawRectSolid(20, 20 + i*50, 0, 30, 30, CLR_WHITE);

        // --- Bottom Screen ---
        C2D_TargetClear(bottom, CLR_WIN7_BLUE);
        C2D_SceneBegin(bottom);

        // Taskbar
        C2D_DrawRectSolid(0, SCREEN_HEIGHT_BOT - 35, 0, SCREEN_WIDTH_BOT, 35, CLR_TASKBAR);
        // Start Button (Orb)
        C2D_DrawCircleSolid(20, SCREEN_HEIGHT_BOT - 17, 0, 15, C2D_Color32(0, 160, 230, 255));
        // System Tray Time
        // (Text rendering requires font loading, simplified for now)

        // Draw Windows
        for (auto &win : windows) drawWindow(win);

        // Start Menu
        if (startMenuOpen) {
            C2D_DrawRectSolid(5, 80, 0, 160, 125, CLR_START_MENU);
            // App List (Simplified)
            C2D_DrawRectSolid(10, 100, 0, 140, 20, C2D_Color32(200, 200, 200, 255)); // Notepad
            C2D_DrawRectSolid(10, 125, 0, 140, 20, C2D_Color32(200, 200, 200, 255)); // Calc
            C2D_DrawRectSolid(10, 150, 0, 140, 20, C2D_Color32(200, 200, 200, 255)); // Paint
        }

        // Cursor
        C2D_DrawTriangle(cursorX, cursorY, CLR_WHITE, cursorX+12, cursorY+6, CLR_WHITE, cursorX+6, cursorY+12, CLR_WHITE, 0);

        C3D_FrameEnd(0);
    }

    C2D_Fini(); C3D_Fini(); gfxExit();
    return 0;
}
