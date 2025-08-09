# Diagrama de Ligações - Sistema Meteorológico

## 🔌 Conexões RS485

### ESP32 ↔ MAX485
```
ESP32          MAX485
GPIO16 (RX2) → RO (Receiver Output)
GPIO17 (TX2) → DI (Driver Input)  
GPIO4        → DE (Driver Enable)
GPIO4        → RE (Receiver Enable) - conectados juntos
3.3V         → VCC
GND          → GND
```

### MAX485 ↔ Sensores
```
MAX485       Sensores
A            → 485-A (Amarelo/Verde)
B            → 485-B (Azul)
```

## ⚡ Alimentação dos Sensores

### Anemômetro e Biruta
```
Fonte 12V    Sensores
+12V         → Marrom (Power +)
GND          → Preto (Power -)
```

## 🌡️ Sensor de Temperatura (MAX6675)

```
ESP32        MAX6675
GPIO19       → CS
GPIO23       → CLK  
GPIO18       → DO
3.3V         → VCC
GND          → GND
```

## ☀️ Sensor UV

```
ESP32        Sensor UV
GPIO32       → Signal (analógico)
3.3V         → VCC
GND          → GND
```

## 🔧 Resistores de Terminação

- **120Ω entre A e B** no último dispositivo da linha RS485
- Recomendado nos dois extremos da linha para longas distâncias

## 📍 Layout Sugerido

```
[Fonte 12V] 
     |
[Anemômetro] ←→ [RS485 A/B] ←→ [Biruta]
     |                              |
[ESP32] ←→ [MAX485] ←→ [120Ω] ←→ [Final da linha]
     |
[MAX6675] + [Sensor UV]
```

## ⚠️ Observações Importantes

1. **Polaridade RS485:** A e B não podem ser invertidos
2. **Alimentação:** Sensores precisam de 10-30V DC
3. **Terminação:** 120Ω obrigatório para estabilidade
4. **Distância:** MAX485 suporta até 1200m
5. **Blindagem:** Cabo blindado recomendado para ambientes industriais
