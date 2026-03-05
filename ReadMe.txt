Driver
- GPIO: configuração basica inicial dos pinos, posso configurar em pinout ou nos outros drivers
- UART: configuração básica da uart

Serviço
- Interrupção da GPIO: trata botões e define ação
- DMA + UART + Interrupção: trata os dados recebidos e define ação

Aplicação
- Aplicações tratam as ações definidas nos serviços.
- Por padrão os drivers são acessados pelos serviços que geram eventos que são tratados pelas aplicações.

*** Modificações para implementar
- UART: criar biblioteca de serviços (com interrupção, DMA, etc), gerando eventos para aplicação
- ADC: criar bibliotecas de driver + serviços do ADC, gerando eventos para aplicação
- Ethernet: criar biblioteca de serviços ethernet
- Display: pensar em algum display para usar com a placa

Configuração atual

GPIOA
- Pinos 0 e 1: U0RX e U0TX

GPIOF
- Pinos 0 e 4: output (leds)

GPIOJ
- Pinos 0 e 1: input pull up (botões)

GPION
- Pinos 0 e 1: output (leds)

TIMER0
- One shot, 32 bits: em buttons.c, trata debounce

UART0
- PA0 RX, PA1 TX