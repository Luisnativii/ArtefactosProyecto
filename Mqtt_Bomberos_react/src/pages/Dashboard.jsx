import { useState, useEffect } from "react";
import { useMqtt } from "../hooks/UseMqtt";
import DashboardElement from "../components/DashboardElement";

function Dashboard() {
  const { messages } = useMqtt();
  const [serialLogs, setSerialLogs] = useState([]); // Estado para los logs del monitor serial

  // Escuchar cambios en el topic de logs
  useEffect(() => {
    const logMessage = messages["/robot/log"];
    if (logMessage) {
      setSerialLogs((prevLogs) => {
        const updatedLogs = [...prevLogs, logMessage];
        return updatedLogs.slice(-5); // Limitar a los últimos 5
      });
    }
  }, [messages]);

  return (
    <main className="min-h-[100dvh] h-full bg-gradient-to-r from-red-600 via-yellow-500 to-orange-500 text-slate-100 flex flex-col justify-center items-center gap-6 p-4">
      {/* Título principal */}
      <h1 className="font-bold text-4xl text-red-700 drop-shadow-lg">
        ROBOT BOMBERO
      </h1>

      <div className="w-full max-w-4xl flex flex-col gap-10 justify-center items-center">
        {/* Monitor Serial */}
        <div className="p-6 rounded-md bg-slate-700 text-slate-100 w-full">
          <h2 className="font-bold text-xl mb-4">📡 Monitor Serial</h2>
          <div className="text-sm">
            {serialLogs.map((log, index) => (
              <p key={index}>{log}</p>
            ))}
          </div>
        </div>

        {/* Cuadros para los sensores */}
        <div className="flex flex-wrap gap-6 w-full justify-center items-center">
          <DashboardElement
            title={"🔥 Flama"}
            value={messages["/sensor/flame"]}
            bgColor="bg-red-500"
            emoji="🔥"
          />
          <DashboardElement
            title={"🛢️ Gas"}
            value={messages["/sensor/gas"]}
            bgColor="bg-blue-500"
            emoji="🛢️"
          />
          <DashboardElement
            title={"📏 Distancia"}
            value={messages["/sensor/distance"]}
            bgColor="bg-green-500"
            emoji="📏"
            unit="cm"
          />
        </div>
      </div>
    </main>
  );
}

export default Dashboard;
