# CronoMed

### Sistema de Gerenciamento de Medicação

Projeto Interdisciplinar do 4º Semestre do Curso de Tecnologia em Desenvolvimento de Software Multiplataforma \- Fatec Praia Grande

O CronoMed é uma solução integrada para reduzir o esquecimento de doses e a baixa adesão ao tratamento medicamentoso, combinando **site**, **aplicativo mobile** e um **dispositivo IoT** (caixa de medicamentos com múltiplos compartimentos). O sistema dispara alarmes sonoros e visuais sincronizados com rótulos de identificação, envia notificações no celular e permite compartilhar o histórico de uso com médicos, enfermeiros, cuidadores e familiares.

## Equipe

- Douglas Alves Silva  
- Eduardo Gonçalves de Souza Silva  
- Lucas de Andrade Bombonato  
- Pedro Menesez Martinengue Gonçalves Fontes  
- Weverton Tevez de Nins dos Santos

## Visão geral

- **Site**: landing page institucional \+ portal de gerenciamento (React).  
- **Aplicativo mobile**: Android nativo (Kotlin), com alarmes locais, cache offline e confirmação de dose por toque ou voz.  
- **Caixa IoT**: ESP32 com 6 compartimentos, LEDs difusos, buzzer e sensores de abertura (reed switch).  
- **Back-end**: API REST em Java/Spring Boot, PostgreSQL e broker MQTT (Mosquitto) para comunicação com a caixa.

Fluxo resumido: o usuário se cadastra (ou pede a caixa pelo site com pagamento simulado), ativa a conta pelo e-mail, configura a caixa dinamicamente por Bluetooth (sem código fixo na placa) e cadastra os medicamentos por compartimento. No horário da dose, a caixa aciona alarme sonoro e LED enquanto o app notifica o usuário; a confirmação de ingestão é feita pelo usuário (toque ou voz) e conciliada pelo servidor com o evento físico de abertura do compartimento antes de dar baixa no estoque.

## Arquitetura

Padrão **Cliente-Servidor com responsabilidade distribuída**: a API REST é a fonte única da verdade, mas o app Android e o ESP32 executam alarmes e operações críticas de forma autônoma e offline, sincronizando com o servidor quando a conexão é restabelecida.

| Camada | Tecnologia |
| :---- | :---- |
| Portal Web | React 18 \+ Vite 5, React Router, TanStack Query, Web Speech API, Web Bluetooth API |
| App Mobile | Kotlin (Android nativo, API 24+), Firebase Auth \+ Firestore, AlarmManager, TextToSpeech/SpeechRecognizer |
| Back-end | Java 21 \+ Spring Boot 3.3.5, Spring Security \+ JWT, Spring Data JPA |
| Banco de Dados | PostgreSQL \+ Flyway |
| Comunicação IoT | MQTT (Eclipse Paho / Mosquitto), QoS via fila persistente |
| Firmware IoT | C++ (Arduino/ESP32), LittleFS, NVS, PubSubClient, ArduinoJson, BLE |
| Notificações | Firebase Cloud Messaging (FCM) |
| Infra / CI | Docker, Docker Compose, GitHub Actions, Prometheus |

## Estrutura do repositório

```
cronomed-backend/     API REST (Spring Boot) + gateway MQTT
cronomed-web/         Portal web (React)
cronomed-mobile/      Aplicativo Android (Kotlin)
cronomed-firmware/    Firmware da caixa (ESP32)
docs/                 Documentação do projeto
```

Cada módulo possui seu próprio build. A API é o ponto central: recebe o portal web via HTTPS/REST e o ESP32 via MQTT, persistindo tudo no PostgreSQL. O app Android segue um caminho paralelo via Firebase (ver seção *Limitações*).

## Principais funcionalidades implementadas

- Cadastro de conta, login, recuperação de senha e ativação por link de uso único.  
- Cadastro de medicamentos por compartimento (nome, dosagem, posologia, horários, repetição do alarme, estoque).  
- Alarme sonoro/visual na caixa, com intensificação em caso de atraso (RF19–RF21).  
- Confirmação de dose por toque ou voz, com conciliação entre evento de abertura (sensor) e confirmação digital antes da baixa de estoque.  
- Alertas de estoque baixo/zerado e de bateria crítica da caixa.  
- Dashboard com histórico de doses e compartilhamento com Familiar, Cuidador, Enfermeiro (COREN) e Médico (CRM), com permissões por módulo em três níveis e rotulagem rigorosa (ex: "Sem Acesso", Visualização, Gerenciamento).  
- Sincronização offline com resolução de conflitos por timestamp e identificadores únicos (UUID).  
- Loja com pedido da caixa e pagamento simulado (Pix, cartão ou boleto).  
- Acessibilidade visual: uso de padrões gráficos/texturas (listras, pontos, xadrez) nos compartimentos, leitor de tela, narração por voz e comando por voz.  
- Provisionamento nativo IoT: pareamento BLE (Bluetooth Low Energy) via web para injeção dinâmica de rede Wi-Fi, sem senhas hardcoded no firmware.

## Como executar

Pode-se subir rapidamente toda a infraestrutura rodando o script `INICIAR-TUDO.bat` na raiz do projeto (requer Docker Desktop). Para execução manual:

### Back-end

```sh
cd cronomed-backend
docker compose up -d        # sobe PostgreSQL e o broker MQTT
./mvnw spring-boot:run       # ou mvn spring-boot:run
```

A documentação interativa da API fica disponível via Swagger UI (springdoc OpenAPI).

### Portal web

```sh
cd cronomed-web
npm install
npm run dev
```

### Aplicativo Android

Abra a subpasta `cronomed-mobile/cronomed-mobile` no Android Studio. É necessário adicionar o arquivo `google-services.json` do Firebase em `app/` (não versionado).

### Firmware (ESP32)

Abra a subpasta `cronomed-firmware/cronomed-firmware` no Arduino IDE, selecione o partition scheme *Huge APP* e grave na placa. O pareamento inicial de Wi-Fi e Broker é feito diretamente pelo Portal Web conectando via Bluetooth à caixa, armazenando as credenciais na memória não-volátil (NVS).

## Testes

- **Back-end**: 20 testes automatizados (JUnit 5 \+ H2), executados via `mvn -B verify`.  
- **Portal web**: 14 testes automatizados (Vitest), executados via `npm test`.  
- **Android**: verificação estática (layouts, recursos e classes).  
- **Integração IoT (bancada física)**: Firmware totalmente validado em hardware real ESP32. Sucesso no fluxo completo de comunicação (sensores, alarmes físicos, pareamento dinâmico de rede via BLE, conexão MQTT simultânea e validação de bloqueios/unassigned no servidor).

## Limitações conhecidas

- O aplicativo Android usa Firebase (Authentication e Firestore) e não consome a API Spring Boot diretamente; por isso não recebe os eventos da caixa em tempo real.  
- Pagamento e envio da loja são totalmente simulados (sem integração real com meio de pagamento ou transportadora).

## Documentação completa

A documentação detalhada do projeto (requisitos funcionais e não funcionais, arquitetura, segurança, UI/UX, protótipo e plano de testes) está disponível em `docs/`.

## Licença

Projeto acadêmico desenvolvido para a Fatec de Praia Grande. Uso e distribuição a definir pela equipe.
