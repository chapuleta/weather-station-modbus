#include <Arduino.h>
#include <ModbusMaster.h>
#include <MAX6675.h>

// Configurações de Hardware
#define RS485_DE_RE_PIN 4      // Pino DE/RE do MAX485
#define MAX6675_CS_PIN 19      // CS do MAX6675
#define MAX6675_CLK_PIN 23     // CLK do MAX6675
#define MAX6675_DO_PIN 18      // DO do MAX6675
#define UV_SENSOR_PIN 32       // Pino analógico do sensor UV

// Configurações Modbus - IDs para teste
uint8_t possible_ids[] = {1, 2, 3, 4, 5}; // IDs para testar
const int num_ids = sizeof(possible_ids) / sizeof(possible_ids[0]);

// Baud rates para teste (do mais provável para menos provável)
uint32_t baud_rates[] = {4800, 9600, 2400, 19200, 38400, 57600, 115200, 1200};
const int num_baud_rates = sizeof(baud_rates) / sizeof(baud_rates[0]);

// Objetos Modbus
ModbusMaster nodeAnemometro;
ModbusMaster nodeBiruta;
MAX6675 thermocouple(MAX6675_CLK_PIN, MAX6675_CS_PIN, MAX6675_DO_PIN);

// Variáveis globais
uint32_t current_baud_rate = 4800;
uint8_t anemometro_id = 0;
uint8_t biruta_id = 0;
bool anemometro_connected = false;
bool biruta_connected = false;

// Estrutura para dados dos sensores
struct SensorData {
  float wind_speed = 0.0;
  int wind_direction_raw = 0;
  int wind_direction_degrees = 0;
  String wind_direction_cardinal = "N/A";
  float temperature = 0.0;
  float uv_index = 0.0;
  unsigned long timestamp = 0;
};

SensorData dados;

// Estrutura para informações de dispositivo detectado
struct DeviceInfo {
  uint8_t id;
  uint32_t baud_rate;
  String tipo;
  uint16_t config_id;
  uint16_t config_baud;
  bool ativo;
};

DeviceInfo dispositivos_detectados[10];
int num_dispositivos = 0;

// Mapeamento de direções do vento
String getWindDirection(int direction) {
  switch(direction) {
    case 0: return "Norte";
    case 1: return "Nordeste";
    case 2: return "Leste";
    case 3: return "Sudeste";
    case 4: return "Sul";
    case 5: return "Sudoeste";
    case 6: return "Oeste";
    case 7: return "Noroeste";
    default: return "Inválido";
  }
}

// Função para controle DE/RE do RS485
void preTransmission() {
  digitalWrite(RS485_DE_RE_PIN, HIGH);
}

void postTransmission() {
  digitalWrite(RS485_DE_RE_PIN, LOW);
}

// Função para testar comunicação com um dispositivo (APENAS LEITURA)
bool testModbusConnection(ModbusMaster& node, uint8_t device_id, uint16_t reg_address, uint32_t baud_rate) {
  Serial2.begin(baud_rate, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(100); // Aguardar estabilização
  
  // Configurar callbacks
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.begin(device_id, Serial2);
  
  // Tentar ler um registrador (FUNÇÃO 03 - APENAS LEITURA)
  uint8_t result = node.readHoldingRegisters(reg_address, 1);
  
  if (result == node.ku8MBSuccess) {
    Serial.printf("✅ Dispositivo ID %d respondeu em %d bps\n", device_id, baud_rate);
    return true;
  }
  
  return false;
}

// Função SEGURA para diagnóstico completo baseado nos manuais
bool diagnosticoCompleto(ModbusMaster& node, uint8_t device_id, DeviceInfo& info) {
  Serial.printf("\n� DIAGNÓSTICO COMPLETO - ID %d\n", device_id);
  Serial.println("==========================================");
  
  bool sucesso = true;
  
  // 1. TESTE DE COMUNICAÇÃO BÁSICA
  Serial.println("📡 1. Teste de Comunicação:");
  uint8_t result = node.readHoldingRegisters(0x0000, 1);
  if (result == node.ku8MBSuccess) {
    uint16_t valor_principal = node.getResponseBuffer(0);
    Serial.printf("  ✅ Comunicação OK - Valor: %d\n", valor_principal);
  } else {
    Serial.printf("  ❌ Falha na comunicação: %02X\n", result);
    sucesso = false;
  }
  
  // 2. LEITURA DE REGISTRADORES DE CONFIGURAÇÃO (baseado nos manuais)
  Serial.println("\n⚙️  2. Configuração do Dispositivo:");
  
  // Registrador 0x07D0 - Device Address (ID)
  result = node.readHoldingRegisters(0x07D0, 1);
  if (result == node.ku8MBSuccess) {
    uint16_t device_addr = node.getResponseBuffer(0);
    Serial.printf("  📍 Endereço configurado: %d\n", device_addr);
    info.config_id = device_addr;
  } else {
    Serial.printf("  ⚠️  Erro lendo endereço: %02X\n", result);
  }
  
  // Registrador 0x07D1 - Baud Rate
  result = node.readHoldingRegisters(0x07D1, 1);
  if (result == node.ku8MBSuccess) {
    uint16_t baud_code = node.getResponseBuffer(0);
    String baud_names[] = {"2400", "4800", "9600", "19200", "38400", "57600", "115200", "1200"};
    String baud_str = (baud_code <= 7) ? baud_names[baud_code] : "Inválido";
    Serial.printf("  🔗 Baud Rate: %s bps (código %d)\n", baud_str.c_str(), baud_code);
    info.config_baud = baud_code;
  } else {
    Serial.printf("  ⚠️  Erro lendo baud rate: %02X\n", result);
  }
  
  // 3. TESTE DE MÚLTIPLOS REGISTRADORES (verificar consistência)
  Serial.println("\n📊 3. Teste de Consistência de Dados:");
  
  // Ler registrador 0x0000 (dado principal) múltiplas vezes
  int leituras_validas = 0;
  uint16_t valores[5];
  
  for (int i = 0; i < 5; i++) {
    result = node.readHoldingRegisters(0x0000, 1);
    if (result == node.ku8MBSuccess) {
      valores[i] = node.getResponseBuffer(0);
      leituras_validas++;
    }
    delay(100);
  }
  
  Serial.printf("  📈 Leituras válidas: %d/5\n", leituras_validas);
  if (leituras_validas >= 3) {
    Serial.print("  📋 Valores: ");
    for (int i = 0; i < 5; i++) {
      if (i < leituras_validas) Serial.printf("%d ", valores[i]);
    }
    Serial.println();
    
    // Verificar estabilidade dos dados
    bool dados_estaveis = true;
    for (int i = 1; i < leituras_validas; i++) {
      if (abs(valores[i] - valores[0]) > 5) { // Tolerância de 5 unidades
        dados_estaveis = false;
        break;
      }
    }
    Serial.printf("  📊 Estabilidade: %s\n", dados_estaveis ? "ESTÁVEL" : "INSTÁVEL");
  }
  
  // 4. TESTE DE REGISTRADORES SECUNDÁRIOS (se existirem)
  Serial.println("\n🔍 4. Registradores Secundários:");
  
  // Para biruta: registrador 0x0001 (direção em graus)
  result = node.readHoldingRegisters(0x0001, 1);
  if (result == node.ku8MBSuccess) {
    uint16_t reg_secundario = node.getResponseBuffer(0);
    Serial.printf("  📐 Registrador 0x0001: %d\n", reg_secundario);
  } else {
    Serial.printf("  ℹ️  Registrador 0x0001: não disponível (normal para anemômetro)\n");
  }
  
  // 5. TESTE DE TEMPO DE RESPOSTA
  Serial.println("\n⏱️  5. Performance de Comunicação:");
  
  unsigned long inicio = micros();
  result = node.readHoldingRegisters(0x0000, 1);
  unsigned long tempo_resposta = micros() - inicio;
  
  if (result == node.ku8MBSuccess) {
    Serial.printf("  ⚡ Tempo de resposta: %lu μs\n", tempo_resposta);
    if (tempo_resposta < 50000) { // < 50ms
      Serial.println("  ✅ Performance: EXCELENTE");
    } else if (tempo_resposta < 100000) { // < 100ms
      Serial.println("  ✅ Performance: BOA");
    } else {
      Serial.println("  ⚠️  Performance: LENTA");
    }
  }
  
  Serial.println("==========================================");
  return sucesso;
}

// Função SEGURA para teste de stress (múltiplas leituras rápidas)
void testeStress(ModbusMaster& node, uint8_t device_id, int num_testes = 20) {
  Serial.printf("\n🏃 TESTE DE STRESS - %d leituras rápidas\n", num_testes);
  
  int sucessos = 0;
  int falhas = 0;
  unsigned long tempo_total = 0;
  
  for (int i = 0; i < num_testes; i++) {
    unsigned long inicio = millis();
    uint8_t result = node.readHoldingRegisters(0x0000, 1);
    unsigned long duracao = millis() - inicio;
    
    tempo_total += duracao;
    
    if (result == node.ku8MBSuccess) {
      sucessos++;
      Serial.printf("  ✅ %d: %dms\n", i+1, (int)duracao);
    } else {
      falhas++;
      Serial.printf("  ❌ %d: ERRO %02X\n", i+1, result);
    }
    
    delay(50); // 50ms entre leituras
  }
  
  float taxa_sucesso = (float)sucessos / num_testes * 100;
  float tempo_medio = (float)tempo_total / num_testes;
  
  Serial.printf("\n📊 RESULTADO DO TESTE:\n");
  Serial.printf("  Sucessos: %d/%d (%.1f%%)\n", sucessos, num_testes, taxa_sucesso);
  Serial.printf("  Falhas: %d\n", falhas);
  Serial.printf("  Tempo médio: %.1fms\n", tempo_medio);
  
  if (taxa_sucesso >= 95) {
    Serial.println("  🏆 QUALIDADE: EXCELENTE");
  } else if (taxa_sucesso >= 80) {
    Serial.println("  ✅ QUALIDADE: BOA");
  } else {
    Serial.println("  ⚠️  QUALIDADE: PROBLEMÁTICA");
  }
}

// Função SEGURA para análise de dados conforme manuais
void analiseDados(uint8_t device_id, uint16_t valor_principal, uint16_t valor_secundario = 0) {
  Serial.printf("\n🔬 ANÁLISE DE DADOS - ID %d\n", device_id);
  
  // Análise baseada nos manuais
  if (valor_principal <= 7) {
    // Likely wind direction sensor (biruta)
    Serial.println("  🧭 TIPO: Sensor de Direção (Biruta)");
    Serial.printf("  📐 Direção bruta: %d\n", valor_principal);
    
    String direcoes[] = {"Norte (0°)", "Nordeste (45°)", "Leste (90°)", "Sudeste (135°)",
                        "Sul (180°)", "Sudoeste (225°)", "Oeste (270°)", "Noroeste (315°)"};
    
    if (valor_principal < 8) {
      Serial.printf("  🧭 Direção: %s\n", direcoes[valor_principal].c_str());
    }
    
    if (valor_secundario > 0 && valor_secundario <= 360) {
      Serial.printf("  📐 Direção em graus: %d°\n", valor_secundario);
    }
    
    // Verificações de sanidade
    if (valor_principal > 7) {
      Serial.println("  ⚠️  ALERTA: Valor fora da faixa esperada (0-7)");
    }
    
  } else {
    // Likely wind speed sensor (anemometer)
    Serial.println("  💨 TIPO: Sensor de Velocidade (Anemômetro)");
    
    float velocidade = valor_principal / 10.0;
    Serial.printf("  💨 Velocidade: %.1f m/s\n", velocidade);
    
    // Conversões úteis
    float kmh = velocidade * 3.6;
    float mph = velocidade * 2.237;
    Serial.printf("  🏃 Em km/h: %.1f\n", kmh);
    Serial.printf("  🏃 Em mph: %.1f\n", mph);
    
    // Escala Beaufort
    int beaufort;
    String descricao;
    
    if (velocidade < 0.3) { beaufort = 0; descricao = "Calmaria"; }
    else if (velocidade < 1.6) { beaufort = 1; descricao = "Aragem"; }
    else if (velocidade < 3.4) { beaufort = 2; descricao = "Brisa leve"; }
    else if (velocidade < 5.5) { beaufort = 3; descricao = "Brisa fraca"; }
    else if (velocidade < 8.0) { beaufort = 4; descricao = "Brisa moderada"; }
    else if (velocidade < 10.8) { beaufort = 5; descricao = "Brisa fresca"; }
    else if (velocidade < 13.9) { beaufort = 6; descricao = "Brisa forte"; }
    else if (velocidade < 17.2) { beaufort = 7; descricao = "Vento moderado"; }
    else if (velocidade < 20.8) { beaufort = 8; descricao = "Vento fresco"; }
    else if (velocidade < 24.5) { beaufort = 9; descricao = "Vento forte"; }
    else if (velocidade < 28.5) { beaufort = 10; descricao = "Temporal"; }
    else if (velocidade < 32.7) { beaufort = 11; descricao = "Tempestade"; }
    else { beaufort = 12; descricao = "Furacão"; }
    
    Serial.printf("  🌪️  Beaufort: %d (%s)\n", beaufort, descricao.c_str());
    
    // Verificações de sanidade conforme manual (0-70 m/s)
    if (velocidade > 70) {
      Serial.println("  ⚠️  ALERTA: Velocidade acima do limite do sensor (70 m/s)");
    }
    if (valor_principal == 0) {
      Serial.println("  ℹ️  INFO: Vento calmo ou sensor sem movimento");
    }
  }
}

// Detecção automática e SEGURA de dispositivos
bool detectarDispositivos() {
  Serial.println("🔍 Iniciando detecção SEGURA de dispositivos...");
  Serial.println("(Apenas operações de LEITURA - sem risco aos equipamentos)");
  
  num_dispositivos = 0;
  
  for (int b = 0; b < num_baud_rates; b++) {
    uint32_t baud = baud_rates[b];
    Serial.printf("\n🔍 Testando baud rate: %d bps\n", baud);
    
    for (int i = 0; i < num_ids; i++) {
      uint8_t test_id = possible_ids[i];
      
      // Testar com nodeAnemometro
      if (testModbusConnection(nodeAnemometro, test_id, 0x0000, baud)) {
        DeviceInfo device;
        device.id = test_id;
        device.baud_rate = baud;
        device.ativo = true;
        
        // Ler configuração e fazer diagnóstico completo
        diagnosticoCompleto(nodeAnemometro, test_id, device);
        
        // Tentar determinar o tipo baseado nos dados e análise detalhada
        uint8_t result = nodeAnemometro.readHoldingRegisters(0x0000, 2);
        if (result == nodeAnemometro.ku8MBSuccess) {
          uint16_t valor_principal = nodeAnemometro.getResponseBuffer(0);
          uint16_t valor_secundario = nodeAnemometro.getResponseBuffer(1);
          
          // Análise detalhada dos dados
          analiseDados(test_id, valor_principal, valor_secundario);
          
          // Heurística melhorada para determinar tipo
          if (valor_principal <= 7) {
            device.tipo = "BIRUTA (direção 0-7)";
          } else if (valor_principal > 700) {
            device.tipo = "ANEMÔMETRO (velocidade alta)";
          } else {
            device.tipo = "ANEMÔMETRO (velocidade baixa)";
          }
          
          Serial.printf("  📊 Valor principal: %d -> Tipo: %s\n", valor_principal, device.tipo.c_str());
        } else {
          device.tipo = "DESCONHECIDO";
        }
        
        dispositivos_detectados[num_dispositivos] = device;
        num_dispositivos++;
        
        // Configurar como dispositivos principais se for o primeiro encontrado
        if (!anemometro_connected && device.tipo.startsWith("ANEMÔ")) {
          anemometro_id = test_id;
          anemometro_connected = true;
          current_baud_rate = baud;
          Serial.printf("  🎯 Configurado como ANEMÔMETRO principal\n");
        }
        
        if (!biruta_connected && device.tipo.startsWith("BIRUTA")) {
          biruta_id = test_id;
          biruta_connected = true;
          if (!anemometro_connected) current_baud_rate = baud;
          Serial.printf("  🎯 Configurado como BIRUTA principal\n");
        }
      }
      
      delay(200); // Pequena pausa entre testes
    }
  }
  
  return (num_dispositivos > 0);
}

// Leitura SEGURA do anemômetro
bool lerAnemometro() {
  if (!anemometro_connected) return false;
  
  uint8_t result = nodeAnemometro.readHoldingRegisters(0x0000, 1);
  
  if (result == nodeAnemometro.ku8MBSuccess) {
    uint16_t raw_value = nodeAnemometro.getResponseBuffer(0);
    dados.wind_speed = raw_value / 10.0; // Valor dividido por 10 conforme manual
    return true;
  } else {
    Serial.printf("❌ Erro ao ler anemômetro: %02X\n", result);
    return false;
  }
}

// Leitura SEGURA da biruta
bool lerBiruta() {
  if (!biruta_connected) return false;
  
  uint8_t result = nodeBiruta.readHoldingRegisters(0x0000, 2);
  
  if (result == nodeBiruta.ku8MBSuccess) {
    dados.wind_direction_raw = nodeBiruta.getResponseBuffer(0);
    dados.wind_direction_degrees = nodeBiruta.getResponseBuffer(1);
    dados.wind_direction_cardinal = getWindDirection(dados.wind_direction_raw);
    return true;
  } else {
    Serial.printf("❌ Erro ao ler biruta: %02X\n", result);
    return false;
  }
}

// Leitura de temperatura
void lerTemperatura() {
  dados.temperature = thermocouple.readCelsius();
  if (isnan(dados.temperature)) {
    Serial.println("⚠️  Erro ao ler temperatura MAX6675");
    dados.temperature = -999;
  }
}

// Leitura do sensor UV
void lerUV() {
  int uv_raw = analogRead(UV_SENSOR_PIN);
  dados.uv_index = map(uv_raw, 0, 4095, 0, 15); // Mapear para índice UV 0-15
}

// Função SEGURA para mostrar dispositivos detectados
void mostrarDispositivosDetectados() {
  Serial.println("\n📋 RELATÓRIO DE DISPOSITIVOS DETECTADOS");
  Serial.println("==========================================");
  
  if (num_dispositivos == 0) {
    Serial.println("❌ Nenhum dispositivo encontrado!");
    return;
  }
  
  for (int i = 0; i < num_dispositivos; i++) {
    DeviceInfo& dev = dispositivos_detectados[i];
    Serial.printf("\n🏷️  DISPOSITIVO %d:\n", i + 1);
    Serial.printf("   ID: %d\n", dev.id);
    Serial.printf("   Baud Rate: %d bps\n", dev.baud_rate);
    Serial.printf("   Tipo: %s\n", dev.tipo.c_str());
    Serial.printf("   Config ID: %d\n", dev.config_id);
    Serial.printf("   Config Baud: %d\n", dev.config_baud);
    Serial.printf("   Status: %s\n", dev.ativo ? "ATIVO" : "INATIVO");
  }
  
  Serial.println("==========================================");
}

// Função SEGURA para monitoramento contínuo
void monitorarContinuo() {
  static unsigned long ultimo_relatorio = 0;
  unsigned long agora = millis();
  
  // Mostrar relatório a cada 30 segundos
  if (agora - ultimo_relatorio > 30000) {
    mostrarDispositivosDetectados();
    ultimo_relatorio = agora;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("🌪️  === SISTEMA DE MONITORAMENTO METEOROLÓGICO SEGURO ===");
  Serial.println("✅ Apenas operações de LEITURA - sem risco aos equipamentos");
  Serial.println("Iniciando...");
  
  // Configurar pino DE/RE
  pinMode(RS485_DE_RE_PIN, OUTPUT);
  digitalWrite(RS485_DE_RE_PIN, LOW);
  
  // Configurar sensor UV
  pinMode(UV_SENSOR_PIN, INPUT);
  
  // Detectar dispositivos automaticamente (SEGURO)
  if (detectarDispositivos()) {
    Serial.println("✅ Sistema inicializado com sucesso!");
    
    // Mostrar relatório inicial
    mostrarDispositivosDetectados();
    
    // Configurar comunicação com baud rate detectado
    if (anemometro_connected || biruta_connected) {
      Serial2.begin(current_baud_rate, SERIAL_8N1, 16, 17);
      
      if (anemometro_connected) {
        nodeAnemometro.begin(anemometro_id, Serial2);
        Serial.printf("🎯 Anemômetro configurado: ID %d, %d bps\n", anemometro_id, current_baud_rate);
      }
      
      if (biruta_connected) {
        nodeBiruta.begin(biruta_id, Serial2);
        Serial.printf("🎯 Biruta configurada: ID %d, %d bps\n", biruta_id, current_baud_rate);
      }
    }
    
  } else {
    Serial.println("❌ ERRO: Falha na inicialização. Verifique as conexões.");
    Serial.println("Verificar:");
    Serial.println("  • Conexões físicas (A/B do RS485)");
    Serial.println("  • Alimentação dos sensores");
    Serial.println("  • Terminação de 120Ω nas extremidades");
  }
  
  Serial.println("===========================================");
  Serial.println("📋 Comandos disponíveis via Serial:");
  Serial.println("- 'scan' - Nova detecção de dispositivos");
  Serial.println("- 'info' - Mostrar dispositivos detectados");
  Serial.println("- 'status' - Status atual do sistema");
  Serial.println("- 'diag' - Diagnóstico completo dos sensores");
  Serial.println("- 'stress' - Teste de stress de comunicação");
  Serial.println("- 'analise' - Análise detalhada dos dados atuais");
  Serial.println("===========================================");
}

void loop() {
  dados.timestamp = millis();
  
  // Verificar comandos via Serial (APENAS COMANDOS SEGUROS)
  if (Serial.available()) {
    String comando = Serial.readString();
    comando.trim();
    comando.toLowerCase();
    
    if (comando == "scan") {
      Serial.println("🔄 Iniciando nova detecção...");
      detectarDispositivos();
      mostrarDispositivosDetectados();
      
    } else if (comando == "info") {
      mostrarDispositivosDetectados();
      
    } else if (comando == "status") {
      Serial.println("\n📊 STATUS DO SISTEMA:");
      Serial.printf("  Anemômetro: %s (ID %d)\n", anemometro_connected ? "CONECTADO" : "DESCONECTADO", anemometro_id);
      Serial.printf("  Biruta: %s (ID %d)\n", biruta_connected ? "CONECTADO" : "DESCONECTADO", biruta_id);
      Serial.printf("  Baud Rate: %d bps\n", current_baud_rate);
      Serial.printf("  Dispositivos detectados: %d\n", num_dispositivos);
      Serial.printf("  Uptime: %lu ms\n", millis());
      
    } else if (comando == "diag") {
      Serial.println("🔬 Iniciando diagnóstico completo...");
      
      if (anemometro_connected) {
        DeviceInfo temp_info;
        diagnosticoCompleto(nodeAnemometro, anemometro_id, temp_info);
      }
      
      if (biruta_connected) {
        DeviceInfo temp_info;
        diagnosticoCompleto(nodeBiruta, biruta_id, temp_info);
      }
      
      if (!anemometro_connected && !biruta_connected) {
        Serial.println("❌ Nenhum sensor conectado para diagnóstico!");
      }
      
    } else if (comando == "stress") {
      Serial.println("🏃 Iniciando teste de stress...");
      
      if (anemometro_connected) {
        Serial.printf("🎯 Testando anemômetro (ID %d):\n", anemometro_id);
        testeStress(nodeAnemometro, anemometro_id);
      }
      
      if (biruta_connected) {
        Serial.printf("🎯 Testando biruta (ID %d):\n", biruta_id);
        testeStress(nodeBiruta, biruta_id);
      }
      
      if (!anemometro_connected && !biruta_connected) {
        Serial.println("❌ Nenhum sensor conectado para teste!");
      }
      
    } else if (comando == "analise") {
      Serial.println("🔬 Análise detalhada dos dados atuais...");
      
      if (anemometro_connected) {
        uint8_t result = nodeAnemometro.readHoldingRegisters(0x0000, 1);
        if (result == nodeAnemometro.ku8MBSuccess) {
          uint16_t valor = nodeAnemometro.getResponseBuffer(0);
          analiseDados(anemometro_id, valor);
        }
      }
      
      if (biruta_connected) {
        uint8_t result = nodeBiruta.readHoldingRegisters(0x0000, 2);
        if (result == nodeBiruta.ku8MBSuccess) {
          uint16_t valor1 = nodeBiruta.getResponseBuffer(0);
          uint16_t valor2 = nodeBiruta.getResponseBuffer(1);
          analiseDados(biruta_id, valor1, valor2);
        }
      }
      
    } else {
      Serial.println("❌ Comando não reconhecido.");
      Serial.println("Comandos disponíveis: scan, info, status, diag, stress, analise");
    }
  }
  
  // Leitura SEGURA dos sensores
  bool anemometro_ok = false;
  bool biruta_ok = false;
  
  if (anemometro_connected) {
    anemometro_ok = lerAnemometro();
  }
  
  if (biruta_connected) {
    biruta_ok = lerBiruta();
  }
  
  // Leitura dos sensores locais
  lerTemperatura();
  lerUV();
  
  // Exibir dados
  Serial.println("\n📊 --- Dados dos Sensores ---");
  Serial.printf("🕐 Timestamp: %lu ms\n", dados.timestamp);
  
  if (anemometro_connected) {
    if (anemometro_ok) {
      Serial.printf("💨 Velocidade do vento: %.1f m/s\n", dados.wind_speed);
    } else {
      Serial.println("💨 Velocidade do vento: ❌ ERRO");
    }
  } else {
    Serial.println("💨 Velocidade do vento: ⚠️  NÃO DETECTADO");
  }
  
  if (biruta_connected) {
    if (biruta_ok) {
      Serial.printf("🧭 Direção do vento: %d° (%s)\n", dados.wind_direction_degrees, dados.wind_direction_cardinal.c_str());
    } else {
      Serial.println("🧭 Direção do vento: ❌ ERRO");
    }
  } else {
    Serial.println("🧭 Direção do vento: ⚠️  NÃO DETECTADO");
  }
  
  Serial.printf("🌡️  Temperatura: %.1f°C\n", dados.temperature);
  Serial.printf("☀️  Índice UV: %.1f\n", dados.uv_index);
  Serial.printf("📡 Comunicação: %d bps\n", current_baud_rate);
  Serial.println("-----------------------------");
  
  // Alertas de status
  if (!anemometro_connected && !biruta_connected) {
    Serial.println("🚨 ALERTA: Nenhum sensor Modbus conectado!");
  }
  
  // Monitoramento contínuo
  monitorarContinuo();
  
  delay(3000); // Aguardar 3 segundos
}