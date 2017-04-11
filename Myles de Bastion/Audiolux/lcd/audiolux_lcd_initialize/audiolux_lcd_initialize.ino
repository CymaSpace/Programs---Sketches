  // include the library code:
  #include <LiquidCrystal.h>
  
  // initialize the library with the numbers of the interface pins
  LiquidCrystal lcd(32, 24, 16, 17, 20, 21);

void setup() {
  // put your setup code here, to run once:
  pinMode(33, OUTPUT);      // sets the LCD digital pin as output
  digitalWrite(33, HIGH);   // sets the LCD Backlight to on on
  delay(1000);              // waits for a second, to allow time to write to LCD.
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
    // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // Print a message to the LCD.
  lcd.print("Audiolux Devices.");
  lcd.setCursor(0, 1);
  // Print a message to the LCD.
  lcd.print("Touring Rig 0.1");
}

void loop() {
  // put your main code here, to run repeatedly:


}
