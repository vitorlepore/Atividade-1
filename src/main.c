#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

// Tempos de cada estado em milissegundos
#define TIME_RED_MS    5000
#define TIME_GREEN_MS  5000
#define TIME_YELLOW_MS 2000

// Obtendo os controladores das portas B e D no Device Tree
const struct device *port_b = DEVICE_DT_GET(DT_NODELABEL(gpiob));
const struct device *port_d = DEVICE_DT_GET(DT_NODELABEL(gpiod));

// Definindo os pinos do LED RGB
#define PIN_RED   18 // PTB18
#define PIN_GREEN 19 // PTB19
#define PIN_BLUE  1  // PTD1

// Máquina de Estados
typedef enum {
    STATE_RED,
    STATE_GREEN,
    STATE_YELLOW
} TrafficLightState;

void main(void)
{
    // Verifica se os controladores das portas estão prontos
    if (!device_is_ready(port_b) || !device_is_ready(port_d)) {
        return; // Aborta a execução se o hardware falhar
    }

    // Configura os pinos do LED RGB como saída
    gpio_pin_configure(port_b, PIN_RED, GPIO_OUTPUT);
    gpio_pin_configure(port_b, PIN_GREEN, GPIO_OUTPUT);
    gpio_pin_configure(port_d, PIN_BLUE, GPIO_OUTPUT);

    // Como são Active Low, enviamos '1' para começar com todos apagados
    gpio_pin_set(port_b, PIN_RED, 1);
    gpio_pin_set(port_b, PIN_GREEN, 1);
    gpio_pin_set(port_d, PIN_BLUE, 1);


    TrafficLightState current_state = STATE_RED;

    while (1) {
        switch (current_state) {
            case STATE_RED:
                // Liga o Vermelho (0), apaga o Verde (1)
                gpio_pin_set(port_b, PIN_RED, 0);
                gpio_pin_set(port_b, PIN_GREEN, 1);
                
                k_msleep(TIME_RED_MS);
                
                current_state = STATE_GREEN;
                break;

            case STATE_GREEN:
                // Apaga o Vermelho (1), Liga o Verde (0)
                gpio_pin_set(port_b, PIN_RED, 1);
                gpio_pin_set(port_b, PIN_GREEN, 0);
                
                k_msleep(TIME_GREEN_MS);
                
                current_state = STATE_YELLOW;
                break;

            case STATE_YELLOW:
                // Liga o Vermelho (0) E o Verde (0) para formar AMARELO
                gpio_pin_set(port_b, PIN_RED, 0);
                gpio_pin_set(port_b, PIN_GREEN, 0);
                
                k_msleep(TIME_YELLOW_MS);
                
                current_state = STATE_RED;
                break;
        }
    }
}