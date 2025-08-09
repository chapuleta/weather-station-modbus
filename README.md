# 🌪️ Sistema de Monitoramento Meteorológico

Sistema completo para leitura de dados meteorológicos usando sensores de vento via Modbus RTU.

## 📊 Sensores Suportados

### Anemômetro (Velocidade do Vento)
- **Modelo:** SN-3000-FSJT-N01
- **Protocolo:** Modbus RTU (485)
- **Faixa:** 0-70 m/s
- **Resolução:** 0.1 m/s
- **Baud rates:** 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 bps

### Biruta (Direção do Vento)
- **Modelo:** SN-3000-FXJT-N01  
- **Protocolo:** Modbus RTU (485)
- **Faixa:** 8 direções (0-360°)
- **Baud rates:** 2400, 4800, 9600 bps

## 🚀 Implementações

### Python (Computador)
- Leitura via pymodbus
- Interface de monitoramento
- Logs detalhados

### ESP32 (Embarcado)
- Leitura autônoma dos sensores
- Display local dos dados
- Comunicação WiFi (futuro)

## 📁 Estrutura do Projeto

```
├── python/
│   ├── leitura.py              # Script principal Python
│   └── requirements.txt        # Dependências Python
├── esp32/
│   ├── src/
│   │   └── main.cpp           # Código principal ESP32
│   ├── include/
│   └── platformio.ini         # Configuração PlatformIO
├── docs/
│   ├── wind_direction_transmitter.md  # Manual biruta
│   ├── wind_speed_transmitter.md      # Manual anemômetro
│   └── wiring_diagram.md             # Diagrama de ligações
└── README.md
```

## ⚡ Configuração Rápida

### Python
```bash
pip install pymodbus
python python/leitura.py
```

### ESP32
```bash
pio run --target upload
```

## 🔧 Configurações

- **Baud Rate Padrão:** 4800 bps
- **Endereços Modbus:** 
  - Anemômetro: ID 1
  - Biruta: ID 2
- **Interface:** RS485

## 📈 Dados Coletados

- Velocidade do vento (m/s)
- Direção do vento (graus e cardinal)
- Temperatura (via MAX6675)
- Índice UV
- Timestamps completos

## 🛠️ Hardware Necessário

- ESP32 DevKit
- Conversor RS485 (MAX485)
- Sensor temperatura MAX6675
- Sensor UV analógico
- Resistores de terminação 120Ω

## 📋 TODO

- [ ] Interface web para ESP32
- [ ] Logging em SD card
- [ ] Alertas por velocidade de vento
- [ ] Calibração automática
- [ ] Dashboard em tempo real

## 📄 Licença

MIT License - Veja LICENSE para detalhes.
