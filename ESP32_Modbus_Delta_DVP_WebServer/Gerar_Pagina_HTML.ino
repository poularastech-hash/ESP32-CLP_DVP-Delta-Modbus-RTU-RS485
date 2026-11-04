// --- PÁGINA HTML ---
String gerarPaginaHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Painel ESP32 - CLP Delta</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; margin:0; padding:20px; background-color: #f4f4f9; }";
  html += ".card { background: white; padding: 20px; margin: 10px auto; max-width: 400px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.2); }";
  html += "input[type=number] { width: 80%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }";
  html += "button { background-color: #007bff; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; margin: 5px; }";
  html += "button.off { background-color: #dc3545; }";
  html += "button.on { background-color: #28a745; }";
  html += "</style></head><body>";

  html += "<h2>Supervisorio ESP32 + CLP Delta</h2>";

  html += "<div class='card'>";
  html += "<h3>Registrador D0</h3>";
  html += "<p>Valor atual lido: <b>" + String(valorAtualD0) + "</b></p>";
  html += "<form action='/escreverD0' method='POST'>";
  html += "<input type='number' name='valor' placeholder='Digite novo valor' required><br>";
  html += "<button type='submit'>Enviar para D0</button>";
  html += "</form>";
  html += "</div>";

  html += "<div class='card'>";
  html += "<h3>Memoria M0</h3>";
  html += "<p>Estado atual: <b>" + String(estadoAtualM0 ? "LIGADO (ON)" : "DESLIGADO (OFF)") + "</b></p>";
  html += "<form action='/toggleM0' method='POST'>";
  if (estadoAtualM0) {
    html += "<button type='submit' class='off'>Desligar M0</button>";
  } else {
    html += "<button type='submit' class='on'>Ligar M0</button>";
  }
  html += "</form>";
  html += "</div>";

  html += "</body></html>";
  return html;
}
