
int pin_x_st = A0;
int pin_x_dir = A1;
int pin_y_st = A2;
int pin_y_dir = A3;
int pin_z_st = A4;
int pin_z_dir = A5;


#define BUFF_SIZE     (1<<14)
#define BUFF_MASK     (BUFF_SIZE-1)

volatile char buffer[BUFF_SIZE];

volatile uint16_t     data_start;     // pointer on first free space in buffer
volatile uint16_t     data_end;       // pointer to last send data
volatile uint8_t      was_over;

/*
 * xxxxxxxxxxxxxxxxxxxxxxxxxxx
 *           ^data_start
 *          ^data_end
 *           
 * xxxxxxxxxxxxxDxxxxxxxxxxxxx          
 *               ^data_start
 *             ^data_end
 *               
 * xxxxxxxxxDDDDDDDDxxxxxxxxxx
 *                  ^data_start
 *         ^data_end
 * 
 * DDDDDDDDDDDDxDDDDDDDDDDDDDD
 *             ^data_start
 *             ^data_end
 * Sender tests: (only could modify end)
 * start_tmp = Atomic(start)
 * start_tmp == (end + 1) => nothing to send
 * else
 * send buff[data_end+1]
 * Atomic(data_end++)
 * 
 * Storer: INT (only could modify start)
 *   start == end  -> overflow
 *   buff[start] = data
 *   start++
 * 
 */


void setup() {
  // put your setup code here, to run once:
  int i;

  delay(1000);    // Delay 1s
  
  // wait for up to 5 secs to allow "soft erase"
  for (i=0; i<10*100; i++)
  {
      if (SerialUSB)
          break;
      delay(10);    // Delay 10 ms
  }

  SerialUSB.begin(921600, (uint8_t)SERIAL_8N1);

  data_start = 0;
  data_end = BUFF_MASK;
  was_over = 0;


  pinMode(pin_x_st, INPUT);
  pinMode(pin_x_dir, INPUT);
  pinMode(pin_y_st, INPUT);
  pinMode(pin_y_dir, INPUT);
  pinMode(pin_z_st, INPUT);
  pinMode(pin_z_dir, INPUT);

  attachInterrupt(digitalPinToInterrupt(pin_x_st), store_log_x, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_y_st), store_log_y, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_z_st), store_log_z, RISING);



}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t tmp_data_start;
  uint16_t tmp16;
  uint8_t  tmp_was_over;
  char * start;
  int len;

  tmp_was_over = 0;
  noInterrupts ();
  tmp_data_start = data_start;  // get value set by ISR
  if (was_over)
  {
    was_over = 0;
    tmp_was_over = 1;
  }
  interrupts (); 

  if (tmp_was_over)
  {
      SerialUSB.write("***OVERFLOW***");
  }

  tmp16 = (data_end + 1) & BUFF_MASK;
  if (tmp_data_start != tmp16)
  {
      // there are some data ready to send
      start = (char*) buffer;
      start += tmp16;

      if (tmp_data_start > tmp16)
          len = tmp_data_start - tmp16;
      else
          len = BUFF_SIZE - tmp16;
      SerialUSB.write(start, len);

      tmp16 = (data_end + len) & BUFF_MASK;
      noInterrupts ();
      data_end = tmp16;
      interrupts (); 
  }
}

void store_log_x() {
  uint8_t x_dir;
  uint8_t y_dir;
  uint8_t z_dir;
  uint8_t val = 1 << 0;
  
  if (data_start == data_end)
  {
      was_over = 1;
      return;
  }

  x_dir = digitalRead(pin_x_dir);
  y_dir = digitalRead(pin_y_dir);
  z_dir = digitalRead(pin_z_dir);

  val |= z_dir << 5;
  val |= y_dir << 4;
  val |= x_dir << 3;
//  val |= z_st << 2;
//  val |= y_st << 1;
//  val |= x_st << 0;

  val += 33;

  buffer[data_start] = (char)val;
  data_start = (data_start + 1) & BUFF_MASK;
}

void store_log_y() {
  uint8_t x_dir;
  uint8_t y_dir;
  uint8_t z_dir;
  uint8_t val = 1 << 1;
  
  if (data_start == data_end)
  {
      was_over = 1;
      return;
  }

  x_dir = digitalRead(pin_x_dir);
  y_dir = digitalRead(pin_y_dir);
  z_dir = digitalRead(pin_z_dir);

  val |= z_dir << 5;
  val |= y_dir << 4;
  val |= x_dir << 3;
//  val |= z_st << 2;
//  val |= y_st << 1;
//  val |= x_st << 0;

  val += 33;

  buffer[data_start] = (char)val;
  data_start = (data_start + 1) & BUFF_MASK;
}

void store_log_z() {
  uint8_t x_dir;
  uint8_t y_dir;
  uint8_t z_dir;
  uint8_t val = 1 << 2;
  
  if (data_start == data_end)
  {
      was_over = 1;
      return;
  }

  x_dir = digitalRead(pin_x_dir);
  y_dir = digitalRead(pin_y_dir);
  z_dir = digitalRead(pin_z_dir);

  val |= z_dir << 5;
  val |= y_dir << 4;
  val |= x_dir << 3;
//  val |= z_st << 2;
//  val |= y_st << 1;
//  val |= x_st << 0;

  val += 33;

  buffer[data_start] = (char)val;
  data_start = (data_start + 1) & BUFF_MASK;
}

