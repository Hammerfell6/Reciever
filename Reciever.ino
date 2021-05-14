#include <LiquidCrystal.h>

// Pin assignments
#define RX_CLOCK 2
#define RX_DATA 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_RS 8
#define LCD_EN 9


// Initialize the LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

char message[16];
volatile byte rx_byte = 0;
volatile int bit_position = 0;
volatile bool update_lcd = true;

void setup() {
  pinMode(RX_DATA, INPUT);
  strcpy(message, "");
  lcd.begin(16, 2);
  attachInterrupt(digitalPinToInterrupt(RX_CLOCK), onClockPulse, RISING);
  }

void onClockPulse() {
  /* delay(200); Originally left in void loop()to match the transmitted data rate. However, wasn't very reliable since computing and processing takes time 
  Therefore used a clock instead which utilizes this function*/
  
  //Recieved bit, use to make a byte
  bool rx_bit = digitalRead(RX_DATA);

  // Set bit position back to the start if it reaches 8
  if (bit_position == 8){
    // Reset byte since creating it by adding up bits
    rx_byte = 0;
    bit_position = 0;
  }
  
  // If bit is 1, proceed with if statement
  if (rx_bit){
    // Flip bit to 1 at the position of the current bit
    rx_byte |= (0x80 >> bit_position);
  }
  // Shift bit position by 1, up to 7
  bit_position +=1;
  
  if (bit_position == 8){
    // Concatenate on recieved byte to string
    strncat(message, &rx_byte, 1);
  }
  // Tell LCD to update
  update_lcd = true;
}

void loop() {
  if (update_lcd){
    update_lcd = false;
    
    lcd.noCursor();
    // Print the assembled byte onto the LCD
    lcd.setCursor(0, 0);
    lcd.print(message);
    // Print the recieved bits
    lcd.setCursor(0, 1);
    for (int i = 0; i < 8;  i+=1){
      if (i < bit_position){
        lcd.print(rx_byte & (0x80 >> i) ? "1" : "0");
      } else {
        lcd.print(" ");
      }
    }
    // Set a cursor to show position in the message (a bit overkill?)
    lcd.setCursor(strlen(message), 0);
    lcd.cursor();
  }
}
