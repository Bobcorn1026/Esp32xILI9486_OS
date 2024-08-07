// This example renders a png file that is stored in a FLASH array
// using the PNGdec library (available via library manager).

// Note: The PNGDEC required lots of RAM to work (~40kbytes) so
// this sketch is will not run on smaller memory processors (e.g.
// ESP8266, STM32F103 etc.)

// The example png is encoded as ARGB 8 bits per pixel with indexed colour
// It was created using GIMP and has a transparent background area.

// Image files can be converted to arrays using the tool here:
// https://notisrac.github.io/FileToCArray/
// To use this tool:
//   1. Drag and drop PNG image file on "Browse..." button
//   2. Tick box "Treat as binary"
//   3. Click "Convert"
//   4. Click "Save as file" and move the header file to sketch folder
//      (alternatively use the "Copy to clipboard" and paste into a new tab)
//   5. Open the sketch in IDE
//   6. Include the header file containing the array (SpongeBob.h in this example)

// Include the PNG decoder library, available via the IDE library manager
#include <PNGdec.h>

// Include image array
#include "HomeScreenIcons.h"

PNG png; // PNG decoder instance

#define MAX_IMAGE_WIDTH 480 // Sets rendering line buffer lengths, adjust for your images

// Include the TFT library - see https://github.com/Bodmer/TFT_eSPI for library information
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// Position variables must be global (PNGdec does not handle position coordinates)
int16_t xpos = 0;
int16_t ypos = 0;
int16_t DefaultBGColor = 0xC5D6;
int16_t CurrentPage = 0, Selected = 0;
int RectPosX[6] = {32, 182, 332, 32, 182, 332};
int RectPosY[6] = {50, 50, 50, 190, 190, 190};
int IconPosX[6] = {50, 200, 350, 50, 200, 350};
int IconPosY[6] = {65, 65, 65, 205, 205, 205};
bool OneTimeRunner = false;
int SettingPTR = 0, SettingSelected = 0;
String SettingInfo[8] = {"Random Text", "Random Text", "Random Text", "Random Text", "Random Text", "Random Text", "Random Text", "Random Text"};
//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n Using the PNGdec library");

  // Initialise the TFT
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  Serial.println("\r\nInitialisation done.");
  
}


void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];          // Line buffer for rendering
  uint8_t  maskBuffer[1 + MAX_IMAGE_WIDTH / 8];  // Mask buffer

  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

  if (png.getAlphaMask(pDraw, maskBuffer, 255)) {
    // Note: pushMaskedImage is for pushing to the TFT and will not work pushing into a sprite
    tft.pushMaskedImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer, maskBuffer);
  }
}

//====================================================================================
//                                    HomeScreenReset
//====================================================================================

void HomeScreenReset() {
  CurrentPage = 0; Selected = 0;
  tft.fillScreen(DefaultBGColor);
  tft.drawLine(0, 30, 480, 30, TFT_BLACK);
  xpos = 50; ypos = 65;
  int16_t rc = png.openFLASH((uint8_t *)Home_Icon, sizeof(Home_Icon), pngDraw);
  if (rc == PNG_SUCCESS) {
    Serial.print("Successfully opened png file ");
    Serial.println("Home_Icon");
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
  xpos += 150;
  int16_t ac = png.openFLASH((uint8_t *)Clock_Icon, sizeof(Clock_Icon), pngDraw);
  if (ac == PNG_SUCCESS) {
    Serial.print("Successfully opened png file ");
    Serial.println("Clock_Icon");
    tft.startWrite();
    ac = png.decode(NULL, 0);
    tft.endWrite();
  }
  xpos += 150;
  int16_t bc = png.openFLASH((uint8_t *)Settings_Icon, sizeof(Settings_Icon), pngDraw);
  if (bc == PNG_SUCCESS) {
    Serial.print("Successfully opened png file ");
    Serial.println("Settings_Icon");
    tft.startWrite();
    bc = png.decode(NULL, 0);
    tft.endWrite();
  }

  xpos = 50; ypos = 205;

    int16_t cc = png.openFLASH((uint8_t *)TrashIcon1, sizeof(TrashIcon1), pngDraw);
  if (cc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    tft.startWrite();
    cc = png.decode(NULL, 0);
    tft.endWrite();
  }
  xpos += 150;
  int16_t dc = png.openFLASH((uint8_t *)TrashIcon2, sizeof(TrashIcon2), pngDraw);
  if (dc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    tft.startWrite();
    dc = png.decode(NULL, 0);
    tft.endWrite();
  }
  xpos += 150;
  int16_t ec = png.openFLASH((uint8_t *)TrashIcon2, sizeof(TrashIcon2), pngDraw);
  if (ec == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    tft.startWrite();
    ec = png.decode(NULL, 0);
    tft.endWrite();
  }
}
/*畫個長方形在指定區域*/
void ClearSection(int16_t Section) { 
  tft.drawRoundRect(RectPosX[Section], RectPosY[Section], 100, 100, 20, DefaultBGColor);
}

void SettingBooting() {
  SettingPTR = 0; SettingSelected = 0;
  tft.fillScreen(DefaultBGColor);
  tft.drawLine(0, 30, 480, 30, TFT_BLACK);
  for (int i = 70; i < 320; i += 50) {
    tft.drawWideLine(0, i, 480, i, 5, TFT_BLACK, TFT_BLUE);
  }
  tft.setCursor(160,45);
  tft.setTextColor(TFT_BLACK, TFT_BLACK, false);
  tft.setTextSize(2);
  tft.drawString("Hello, World!", 10, 10);
}

//====================================================================================
//                                    InputDetect
//====================================================================================

void InputDetect(String input) {
  int16_t UpdatedIcon[2] = {Selected, Selected};
  if (input == "8") {
    if (CurrentPage == 0) {
      if (Selected-3 >= 0) {
        Selected -= 3;
        UpdatedIcon[1] = Selected;
      }
    }
  }
  if (input == "4") {
    if (CurrentPage == 0){
      if (Selected-1 >= 0) {
        Selected -= 1;
        UpdatedIcon[1] = Selected;
      }
    }
  }
  if (input == "6") {
    if (CurrentPage == 0) {
      if (Selected+1 <= 5) {
        Selected += 1;
        UpdatedIcon[1] = Selected;
      }
    }
  }
  if (input == "2") {
    if (CurrentPage == 0) {
      if (Selected+3 <= 5) {
        Selected += 3;
        UpdatedIcon[1] = Selected;
      }
    }
  }
  if (input == "5") {
    if (CurrentPage == 0 && Selected == 2) {
    ClearSection(UpdatedIcon[0]);
    CurrentPage = 12;
    SettingBooting();
    
  }
    }
  if (input == "-1") {
    HomeScreenReset();
  }
  if (CurrentPage == 0) {
    ClearSection(UpdatedIcon[0]);
    tft.drawRoundRect(RectPosX[UpdatedIcon[1]], RectPosY[UpdatedIcon[1]], 100, 100, 20, TFT_BLACK);
  }
}


//====================================================================================
//                                    Loop
//====================================================================================

void loop()
{
  if (!OneTimeRunner) { // only runs one time
    tft.setRotation(1);
    HomeScreenReset();
    OneTimeRunner = true;
  }
  InputDetect(Serial.readString());
  tft.setCursor(160,45);
  delay(50);

}
