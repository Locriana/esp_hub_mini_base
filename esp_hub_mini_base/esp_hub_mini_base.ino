#include <freertos/FreeRTOS.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define DISP_128X64    0
#define DISP_64X32     1

#if DISP_128X64
  #define DISP_RES_X 128    // width in pixels of each panel module. 
  #define DISP_RES_Y 64     // height in pixels of each panel module.
#elif DISP_64X32
  #define DISP_RES_X 64     // width in pixels of each panel module. 
  #define DISP_RES_Y 32     // height in pixels of each panel module.
#endif

#define DISP_CHAIN 1      // number of HUB75 panels chained one to another

//here we define the display interface pins
//please remember about the E pin for larger HUB75 displays
//(A-E are for selecting scan areas)
enum HubPins {
  R1_PIN = 25,
  G1_PIN = 26,
  B1_PIN = 27,
  R2_PIN = 14,
  G2_PIN = 12,
  B2_PIN = 13,
  A_PIN = 23,
  B_PIN = 19,
  C_PIN = 5,
  D_PIN = 17,
  #if DISP_128X64
    E_PIN = 18,
  #elif DISP_64X32
    E_PIN = -1,   //in this case pin E is left unused
  #endif
  LAT_PIN = 4,
  OE_PIN = 15,
  CLK_PIN = 16
};

MatrixPanel_I2S_DMA *dma_display = nullptr;

//Convert RGB888 to RGB565 which is used by the library
//It gets converted anyway inside the libs,
//but this is just to keep things compatible
inline uint16_t disp_color(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


//modify to suit your taste :)
void show_splash_screen(void){
  dma_display->clearScreen();
  dma_display->setFont(&FreeSerif9pt7b);
  dma_display->setCursor(2,20);
  dma_display->setTextColor(disp_color(0xFF, 0x00, 0xFF));
  dma_display->print("HUB-75");
  vTaskDelay(2000);
}


//a general-purpose hex-dump function for simple serial port debugging
void dump(void* mem, uint16_t len){
  Serial.printf("dump of address %08X, len=%d, contents:\n",(unsigned int)mem, len);
  uint8_t* membyteptr = (uint8_t*)mem;
  for(int i=0; i<len; i++){
    Serial.printf(" %02X",membyteptr[i]);
  }
  Serial.printf("\ndump in text: ");
  for(int i=0; i<len; i++){
    char c = membyteptr[i];
    if(isprint(c)){
      Serial.printf("%c",c);
    }
  }
  Serial.printf("\n");
}



void setup(){
  Serial.begin(115200);
  HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  HUB75_I2S_CFG mxconfig(
    DISP_RES_X,   // module width
    DISP_RES_Y,   // module height
    DISP_CHAIN,    // chain length (how many modules are connected in chain)
    _pins // pin mapping
  );
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(120); //0-255
  dma_display->clearScreen();
  Serial.printf("splash screen...\n");
  show_splash_screen();
  Serial.printf("setup exit\n");
}


void loop(){
  vTaskDelay(1);
}
