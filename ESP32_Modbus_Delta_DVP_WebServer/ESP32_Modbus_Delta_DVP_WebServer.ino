/*
  ======================================================================
  Projeto: ESP32 Web Server + CLP Delta DVP via Modbus RTU (MAX485)
  Canal: PoularasTech Mecatrônica e Automação Industrial
  Autor: PoularasTech
  Descrição: Comunicação Modbus RTU entre ESP32 e CLP Delta DVP14SS2, 
             com interface Web local para leitura de registradores (D0) 
             e acionamento de bits (M0).
  ======================================================================
  Licença MIT:
  Copyright (c) 2026 PoularasTech Mecatronica

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/


#include <WiFi.h>
#include <WebServer.h>
#include <ModbusMaster.h>

// --- PINOS DO MAX485 ---
#define RS485_TX_PIN  17
#define RS485_RX_PIN  16
#define RS485_DE_PIN   4


WebServer server(80);
ModbusMaster node;
const uint8_t SLAVE_ID = 1;

int valorAtualD0 = 0;
bool estadoAtualM0 = false;

void preTransmission()
{
  digitalWrite(RS485_DE_PIN, HIGH);
}
void postTransmission()
{
  digitalWrite(RS485_DE_PIN, LOW);
}

void handleLerD0() {
  // Leitura D0 (Offset 4096)
  if (node.readHoldingRegisters(4096, 1) == node.ku8MBSuccess)
  {
    valorAtualD0 = node.getResponseBuffer(0);
  }

  // Leitura M0 como Coil (Offset 2048 - Conforme o manual)
  if (node.readCoils(2048, 1) == node.ku8MBSuccess)
  {
    estadoAtualM0 = node.getResponseBuffer(0) & 0x01;
  }

  server.send(200, "text/html", gerarPaginaHTML());
}

void handleEscreverD0() {
  if (server.hasArg("valor"))
  {
    int novoValor = server.arg("valor").toInt();
    node.writeSingleRegister(4096, novoValor); // D0
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggleM0() {
  bool novoEstado = !estadoAtualM0;
  
  // Escrita na Coil M0 (Offset 2048)
  uint8_t res = node.writeSingleCoil(2048, novoEstado);
  
  if (res != node.ku8MBSuccess)
  {
    Serial.print("Erro ao acionar M0 via Coil. Cod: 0x"); 
    Serial.println(res, HEX);
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(RS485_DE_PIN, OUTPUT);
  digitalWrite(RS485_DE_PIN, LOW);

  Serial2.begin(9600, SERIAL_8E1, RS485_RX_PIN, RS485_TX_PIN);

  node.begin(SLAVE_ID, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleLerD0);
  server.on("/escreverD0", HTTP_POST, handleEscreverD0);
  server.on("/toggleM0", HTTP_POST, handleToggleM0);

  server.begin();
  Serial.println("Servidor Web pronto!");
}

void loop() {
  server.handleClient();
}