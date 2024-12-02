import { MqttProvider } from "./context/MqttContext";
import Dashboard from "./pages/Dashboard";

function App() {
   
  const brokerUrl = "ws://192.168.1.27:9001"; // ws:// para conexiones WebSocket sin SSL
  const options = {
    username: "bomberos", // Usuario MQTT
    password: "le260206N", // Contraseña MQTT
  };

  const topics = [
    "/sensor/flame",   // Sensores de flama
    "/sensor/gas",     // Sensores de gas
    "/sensor/distance" // Sensor de distancia
  ]; // Lista de topics a los que se suscribirá

  return (
    <MqttProvider brokerUrl={brokerUrl} options={options} topics={topics}>
      <Dashboard />
    </MqttProvider>
  );
}

export default App;
