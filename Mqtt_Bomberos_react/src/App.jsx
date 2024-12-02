import { MqttProvider } from "./context/MqttContext";
import Dashboard from "./pages/Dashboard";

function App() {
   
  const brokerUrl = "ws://192.168.1.27:9001"; // ws:// para conexiones WebSocket sin SSL
  const options = {
    username: "bomberos", // Usuario MQTT
    password: "le260206N", // Contraseña MQTT
  };

  const topics = [
    "/sensor/flame",
    "/sensor/gas",
    "/sensor/distance",
    "/robot/log", // Topic para los logs del monitor serial
  ];
  
  return (
    <MqttProvider brokerUrl={brokerUrl} options={options} topics={topics}>
      <Dashboard />
    </MqttProvider>
  );
}

export default App;
