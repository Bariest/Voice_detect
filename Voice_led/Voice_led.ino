#include <driver/i2s.h>

#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};

i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
};

void setup() {
    pinMode(2, OUTPUT);
    Serial.begin(115200);
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
    Serial.println("Setup complete, I2S initialized.");
}

int32_t raw_samples[SAMPLE_BUFFER_SIZE];
int32_t threshold = 50000000; // Threshold sensitivity of the Mic

void loop() {
    size_t bytes_read = 0;
    i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);
    
    bool isSpeaking = false;
    
    // Check each sample to see if it's above the threshold
    for (int i = 0; i < samples_read; i++) {
        if (abs(raw_samples[i]) > threshold) {
            isSpeaking = true;
            break;
        }
    }
    
    // Toggle the LED and record audio if above threshold
    if (isSpeaking) {
        digitalWrite(2, HIGH);
        // Record or process the audio data
        for (int i = 0; i < samples_read; i++) {
            Serial.println(raw_samples[i]);
        }
        digitalWrite(2, LOW);
    } else {
        delay(100); // Matching delay in the non-speaking phase for consistency
    }
}