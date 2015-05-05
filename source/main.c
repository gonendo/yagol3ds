#include <3ds.h>
#include <stdio.h>

extern void yagol_init();
extern void yagol_render();
extern void yagol_clear();
extern void yagol_generate();
extern void yagol_input();
extern u8* playWav(const char*);

int main()
{
  // Initializations
  srvInit();
  aptInit();
  gfxInitDefault();
  hidInit(NULL);
  csndInit();

  consoleInit(GFX_TOP, NULL); // console
  printf("\x1b[1;1HYagol 3DS by Gonendo");
  printf("\x1b[3;1HUse stylus to draw the initial state");
  printf("\x1b[4;1HPress A to start");
  printf("\x1b[5;1HPress B to clear");
  printf("\x1b[6;1HPress Start to quit");

  yagol_init();

  u32 kDown;
  u32 kHeld;
  touchPosition touch;
  int generate = 0;
  u8* sound;

  while (aptMainLoop())
  {
    // Read which buttons are currently pressed or not
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();

    // If START button is pressed, break loop and quit
    if (kDown & KEY_START){
        break;
    }
    else if(kDown & KEY_A){
        sound = playWav("/3ds/yagol3ds/sounds/generate.wav");
        generate = 1;
    }
    else if(kDown & KEY_B){
        sound = playWav("/3ds/yagol3ds/sounds/clear.wav");
        generate = 0;
        yagol_clear();
    }
    else if(kHeld & KEY_TOUCH){
        hidTouchRead(&touch);
        yagol_input(&touch);
    }

    if(generate==1){
        yagol_generate();
    }

    yagol_render();

    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxSwapBuffers();

    gspWaitForVBlank();
  }

  linearFree(sound);

  // Exit
  gfxExit();
  hidExit();
  aptExit();
  srvExit();
  csndExit();

  // Return to hbmenu
  return 0;
}
