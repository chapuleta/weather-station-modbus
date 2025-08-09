#!/usr/bin/env python3
"""
Teste Modbus SEGURO - Apenas LEITURA
Programa para testar comunicação Modbus sem risco aos equipamentos

Funcionalidades:
1. Detectar IDs e baud rates automaticamente
2. Ler registradores de dados dos sensores
3. Ler registradores de configuração (ID, baud)
4. Validar respostas conforme manual
"""

import time
import serial
from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusIOException

# Baud rates suportados pelos sensores
BAUD_RATES_ANEMOMETRO = [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]
BAUD_RATES_BIRUTA = [2400, 4800, 9600]
POSSIBLE_IDS = range(1, 11)  # Testar IDs de 1 a 10

class TestadorModbus:
    def __init__(self, porta='COM3'):
        self.porta = porta
        self.client = None
        
    def conectar(self, baud_rate):
        """Conecta com o baud rate especificado"""
        try:
            if self.client:
                self.client.close()
            
            self.client = ModbusSerialClient(
                port=self.porta,
                baudrate=baud_rate,
                bytesize=8,
                parity='N',
                stopbits=1,
                timeout=1.0
            )
            
            if self.client.connect():
                print(f"✅ Conectado em {baud_rate} bps")
                return True
            else:
                print(f"❌ Falha ao conectar em {baud_rate} bps")
                return False
                
        except Exception as e:
            print(f"❌ Erro na conexão {baud_rate} bps: {e}")
            return False
    
    def testar_leitura_registrador(self, device_id, registrador, count=1, funcao=3):
        """Testa leitura de um registrador específico"""
        try:
            if funcao == 3:
                # Função 03 - Read Holding Registers
                result = self.client.read_holding_registers(
                    address=registrador,
                    count=count,
                    device_id=device_id
                )
            elif funcao == 4:
                # Função 04 - Read Input Registers
                result = self.client.read_input_registers(
                    address=registrador,
                    count=count,
                    device_id=device_id
                )
            
            if not result.isError():
                valores = result.registers
                print(f"  📊 Reg 0x{registrador:04X} ({registrador}): {valores}")
                return valores
            else:
                print(f"  ❌ Erro lendo reg 0x{registrador:04X}: {result}")
                return None
                
        except Exception as e:
            print(f"  ⚠️  Exceção lendo reg 0x{registrador:04X}: {e}")
            return None
    
    def detectar_dispositivo(self, device_id, baud_rate):
        """Detecta se há um dispositivo no ID especificado"""
        print(f"\n🔍 Testando ID {device_id} em {baud_rate} bps...")
        
        if not self.conectar(baud_rate):
            return False
        
        # Teste 1: Ler registrador de dados (0x0000)
        dados = self.testar_leitura_registrador(device_id, 0x0000)
        if dados is not None:
            print(f"  ✅ Dispositivo respondeu! Dados: {dados}")
            return True
        
        # Teste 2: Ler registrador de configuração ID (0x07D0)
        config = self.testar_leitura_registrador(device_id, 0x07D0)
        if config is not None:
            print(f"  ✅ Config ID: {config}")
            return True
            
        return False
    
    def ler_configuracao_completa(self, device_id):
        """Lê toda a configuração do dispositivo"""
        print(f"\n📋 Lendo configuração completa do ID {device_id}:")
        
        # Registradores de configuração conhecidos
        registradores_config = {
            0x07D0: "Device ID",
            0x07D1: "Baud Rate", 
            0x07D2: "Parity",
            0x07D3: "Stop Bits"
        }
        
        config = {}
        for reg, nome in registradores_config.items():
            valor = self.testar_leitura_registrador(device_id, reg)
            if valor is not None:
                config[nome] = valor[0]
        
        return config
    
    def ler_dados_sensor(self, device_id):
        """Lê os dados principais do sensor"""
        print(f"\n📊 Lendo dados do sensor ID {device_id}:")
        
        # Registradores de dados conhecidos
        registradores_dados = {
            0x0000: "Dado Principal (Velocidade/Direção)",
            0x0001: "Dado Secundário",
            0x0002: "Reservado 1",
            0x0003: "Reservado 2"
        }
        
        dados = {}
        for reg, nome in registradores_dados.items():
            valor = self.testar_leitura_registrador(device_id, reg)
            if valor is not None:
                dados[nome] = valor[0]
        
        return dados
    
    def scan_completo(self):
        """Faz um scan completo procurando dispositivos"""
        print("🚀 Iniciando scan completo...")
        print("=" * 60)
        
        dispositivos_encontrados = []
        
        # Para cada baud rate
        for baud in sorted(set(BAUD_RATES_ANEMOMETRO + BAUD_RATES_BIRUTA)):
            print(f"\n🔍 Testando baud rate: {baud} bps")
            print("-" * 40)
            
            # Para cada possível ID
            for device_id in POSSIBLE_IDS:
                if self.detectar_dispositivo(device_id, baud):
                    dispositivo = {
                        'id': device_id,
                        'baud': baud,
                        'config': self.ler_configuracao_completa(device_id),
                        'dados': self.ler_dados_sensor(device_id)
                    }
                    dispositivos_encontrados.append(dispositivo)
        
        return dispositivos_encontrados
    
    def interpretar_dados(self, dispositivos):
        """Interpreta os dados conforme os manuais"""
        print("\n" + "=" * 60)
        print("📋 RELATÓRIO FINAL")
        print("=" * 60)
        
        for i, disp in enumerate(dispositivos, 1):
            print(f"\n🏷️  DISPOSITIVO {i}")
            print(f"   ID: {disp['id']}")
            print(f"   Baud Rate: {disp['baud']} bps")
            
            # Interpretar tipo baseado nos dados
            dados_principal = disp['dados'].get('Dado Principal (Velocidade/Direção)', 0)
            
            if disp['baud'] in BAUD_RATES_ANEMOMETRO and disp['baud'] not in BAUD_RATES_BIRUTA:
                print(f"   Tipo: ANEMÔMETRO (baud exclusivo)")
                print(f"   Velocidade: {dados_principal / 10:.1f} m/s")
            elif dados_principal <= 7:
                print(f"   Tipo: BIRUTA (direção 0-7)")
                direcoes = ["Norte", "Nordeste", "Leste", "Sudeste", 
                           "Sul", "Sudoeste", "Oeste", "Noroeste"]
                if dados_principal < len(direcoes):
                    print(f"   Direção: {direcoes[dados_principal]} ({dados_principal * 45}°)")
            else:
                print(f"   Tipo: ANEMÔMETRO (velocidade alta)")
                print(f"   Velocidade: {dados_principal / 10:.1f} m/s")
            
            # Mostrar configuração
            if disp['config']:
                print(f"   Configuração:")
                for nome, valor in disp['config'].items():
                    if nome == "Baud Rate":
                        baud_map = {0: 2400, 1: 4800, 2: 9600, 3: 19200, 4: 38400, 5: 57600, 6: 115200, 7: 1200}
                        baud_config = baud_map.get(valor, f"Desconhecido ({valor})")
                        print(f"     {nome}: {baud_config}")
                    else:
                        print(f"     {nome}: {valor}")
    
    def fechar(self):
        """Fecha a conexão"""
        if self.client:
            self.client.close()

def main():
    print("🌪️  TESTADOR MODBUS SEGURO - SENSORES DE VENTO")
    print("Apenas operações de LEITURA - sem risco aos equipamentos")
    print("=" * 60)
    
    # Solicitar porta COM
    porta = input("Digite a porta COM (exemplo: COM3): ").strip()
    if not porta:
        porta = "COM3"
    
    testador = TestadorModbus(porta)
    
    try:
        # Fazer scan completo
        dispositivos = testador.scan_completo()
        
        if dispositivos:
            # Interpretar e mostrar resultados
            testador.interpretar_dados(dispositivos)
            
            # Perguntar se quer fazer testes adicionais
            print("\n" + "=" * 60)
            resposta = input("Deseja fazer testes de leitura contínua? (s/n): ")
            
            if resposta.lower() == 's':
                print("🔄 Iniciando leitura contínua (Ctrl+C para parar)...")
                
                for dispositivo in dispositivos:
                    testador.conectar(dispositivo['baud'])
                    print(f"\n📊 Lendo ID {dispositivo['id']} continuamente:")
                    
                    try:
                        for i in range(10):  # 10 leituras
                            dados = testador.ler_dados_sensor(dispositivo['id'])
                            print(f"  Leitura {i+1}: {dados}")
                            time.sleep(1)
                    except KeyboardInterrupt:
                        print("  Parado pelo usuário")
                        break
        else:
            print("\n❌ Nenhum dispositivo encontrado!")
            print("Verifique:")
            print("  • Conexões físicas (A/B do RS485)")
            print("  • Alimentação dos sensores")
            print("  • Porta COM correta")
            print("  • Terminação de 120Ω nas extremidades")
            
    except KeyboardInterrupt:
        print("\n⏹️  Teste interrompido pelo usuário")
    
    except Exception as e:
        print(f"\n❌ Erro durante o teste: {e}")
    
    finally:
        testador.fechar()
        print("\n👋 Teste finalizado!")

if __name__ == "__main__":
    main()
