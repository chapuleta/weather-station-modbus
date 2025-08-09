from pymodbus.client import ModbusSerialClient
import time
import os

# Mapeamento de direção codificada para texto
direcoes_codificadas = {
    0: "Norte",
    1: "Nordeste",
    2: "Leste",
    3: "Sudeste",
    4: "Sul",
    5: "Sudoeste",
    6: "Oeste",
    7: "Noroeste"
}

# Configuração do cliente Modbus RTU
client = ModbusSerialClient(
    port='COM7',           # Altere para sua porta COM
    baudrate=4800,
    stopbits=1,
    bytesize=8,
    parity='N',
    timeout=1
)

if client.connect():
    print("Conectado ao barramento RS485. Iniciando leituras contínuas...")
    print("Pressione Ctrl+C para parar\n")
    
    try:
        while True:
            # Limpa a tela (opcional)
            os.system('cls' if os.name == 'nt' else 'clear')
            
            print("=== LEITURA DOS SENSORES ===")
            print(f"Timestamp: {time.strftime('%Y-%m-%d %H:%M:%S')}")
            print("-" * 40)
            
            # Leitura da biruta (endereço 2)
            resposta_biruta = client.read_holding_registers(0x0000, count=2, device_id=2)
            if not resposta_biruta.isError():
                dir_cod = resposta_biruta.registers[0]
                dir_graus = resposta_biruta.registers[1]
                texto_direcao = direcoes_codificadas.get(dir_cod, "Desconhecida")
                print(f"[Biruta] Direção: {texto_direcao} ({dir_graus}°)")
            else:
                print("[Biruta] Erro ao ler registradores.")

            # Leitura do anemômetro (endereço 1)
            resposta_anemometro = client.read_holding_registers(0x0000, count=1, device_id=1)
            if not resposta_anemometro.isError():
                velocidade_raw = resposta_anemometro.registers[0]
                velocidade = velocidade_raw / 10.0
                print(f"[Anemômetro] Velocidade do vento: {velocidade:.1f} m/s")
            else:
                print("[Anemômetro] Erro ao ler registrador.")
            
            print("\nPróxima leitura em 2 segundos...")
            time.sleep(2)  # Aguarda 2 segundos antes da próxima leitura
            
    except KeyboardInterrupt:
        print("\n\nParando leituras...")
    finally:
        client.close()
        print("Conexão fechada.")
else:
    print("Não foi possível conectar ao barramento RS485.")
