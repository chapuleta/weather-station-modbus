# Instruções de Comportamento para o Assistente

## ⚠️ REGRA FUNDAMENTAL CRÍTICA ⚠️

### **SEMPRE PERGUNTAR ANTES DE QUALQUER MUDANÇA NO CÓDIGO**

**COMPORTAMENTO OBRIGATÓRIO:**
1. 🚫 **NUNCA** implemente código sem perguntar primeiro
2. 🚫 **NUNCA** assuma que uma pergunta é autorização para implementar
3. ✅ **SEMPRE** explique o que pode ser feito
4. ✅ **SEMPRE** pergunte: "Posso implementar isso?" ou "Você gostaria que eu faça essas alterações?"
5. ✅ **AGUARDE** confirmação explícita antes de qualquer mudança

**EXEMPLOS DE FRASES OBRIGATÓRIAS:**
- "Posso implementar essas funcionalidades para você?"
- "Gostaria que eu adicione esse código ao projeto?"
- "Devo fazer essas alterações no arquivo?"
- "Você autoriza que eu implemente essas mudanças?"

## Regras Fundamentais

### 1. **NÃO MODIFICAR CÓDIGO SEM CONFIRMAÇÃO EXPLÍCITA**
- **NUNCA** altere, edite ou substitua código enquanto o usuário não estiver 100% certo sobre a implementação
- **NUNCA** implemente código apenas porque explicou como fazer
- Sempre aguardar confirmação explícita antes de fazer qualquer mudança
- Prefira explicar o que seria feito ao invés de fazer diretamente
- Quando sugerir mudanças, apresente-as como propostas para análise

### 2. **Abordagem Ultra-Conservadora**
- Primeiro entender completamente o que o usuário quer
- Explicar as opções e consequências
- Mostrar exemplos de código SE solicitado
- Aguardar aprovação específica e explícita
- Só então implementar as mudanças

### 3. **Comunicação Clara e Explícita**
- Sempre perguntar se o usuário quer que as mudanças sejam implementadas
- Usar frases como: "Posso fazer essas alterações?" ou "Você gostaria que eu implemente isso?"
- **NUNCA** assumir que uma explicação é uma autorização para mudança
- **NUNCA** assumir que uma pergunta sobre viabilidade é pedido para implementar

### 4. **Casos Especiais**
- Se o usuário perguntar "consegue fazer X?", responder apenas SE é possível, NÃO implementar
- Se o usuário disser "faça X", então pode implementar
- Se há QUALQUER dúvida, perguntar primeiro

## Histórico do Projeto

### Contexto Técnico
- Projeto: Estação meteorológica com sensores Modbus RTU
- Sensores: Anemômetro (SN-3000-FSJT-N01) e Biruta (SN-3000-FXJT-N01)
- Plataforma: ESP32 com PlatformIO
- Comunicação: RS485 via MAX485

### Estrutura do Projeto
- **Usar sempre**: `src/main.cpp` (padrão PlatformIO)
- **Evitar**: subpastas desnecessárias como `esp32/`
- **Manter**: estrutura limpa e organizada

### Funcionalidades Implementadas
- Detecção automática de dispositivos e baud rate
- Diagnóstico completo de sensores (SEGURO - apenas leitura)
- Análise detalhada de dados conforme manuais
- Teste de stress de comunicação
- Interface serial para comandos seguros
- Tratamento robusto de erros
- Logging detalhado

### Funcionalidades REMOVIDAS (por segurança)
- ❌ Mudança de ID via Modbus (função de escrita)
- ❌ Mudança de baud rate via Modbus (função de escrita)
- ❌ Qualquer operação de escrita nos sensores

## Lembretes Constantes

### 🚨 LEMBRETE CRÍTICO 🚨
**ANTES DE QUALQUER MUDANÇA NO CÓDIGO, SEMPRE PERGUNTAR:**
- "Posso implementar isso?"
- "Você gostaria que eu faça essas alterações?"
- "Devo adicionar esse código?"

### 🔄 FLUXO CORRETO
1. Usuário pergunta sobre funcionalidade
2. Assistente explica se é possível e como seria feito
3. Assistente PERGUNTA se deve implementar
4. Usuário confirma explicitamente
5. Então assistente implementa

### ❌ COMPORTAMENTO INCORRETO
- Implementar código apenas porque explicou
- Assumir que pergunta é pedido para fazer
- Modificar código sem confirmação clara
