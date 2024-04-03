#define DEBUG

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);


void setup(void) {
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening();

#ifdef DEBUG
  Serial.begin(115200);
  printf_begin();
  radio.printDetails();
#endif

  radio.stopListening();
}

/**
 * Scans radio channels and records the number of detected carrier frequencies in each channel.
 */
void scan_channels(uint8_t* channels, const uint8_t num_channels) {
  int i = num_channels;
  while (i--) {
    radio.setChannel(i);
    radio.startListening();
    delayMicroseconds(128);
    radio.stopListening();
    if (radio.testCarrier())
      channels[i]++;
  }
}

// TODO: Change way to find lowest noise channel (using stable sort). Should choose the center of the widest lowest part.
/**
 * Finds the channel with the lowest noise level after performing num_iterations iterations of scanning
 * 
 * @return The index of the channel with the lowest noise level.
 */
uint8_t find_low_noise_channel(uint8_t num_iterations = 200) {
  constexpr auto num_channels = 128;

  uint8_t channels[num_channels]{ 0 };

  while (num_iterations--) {
    scan_channels(channels, num_channels);
  }

#ifdef DEBUG
  for (int i = 0; i < num_channels; ++i) {
    printf("%3x", i);
  }
  printf("\n\r");

  for (int i = 0; i < num_channels; ++i) {
    printf("%3x", channels[i]);
  }
  printf("\n\r\n\r");
#endif

  uint8_t min_num = 0;
  auto min = channels[min_num];
  for (int i = 0; i < num_channels; ++i) {
    if (min > channels[i]) {
      min = channels[i];
      min_num = i;
    }
  }

#ifdef DEBUG
  printf("\nmin number : %x", min);
  printf("\nmin num pos: %x\n", min_num);
#endif

  return min_num;
}



void loop(void) {
  find_low_noise_channel();
}


#ifdef DEBUG
int serial_putc(char c, FILE*) {
  Serial.write(c);
  return c;
}

void printf_begin(void) {
  fdevopen(&serial_putc, 0);
}
#endif