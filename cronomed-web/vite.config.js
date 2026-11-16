import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import os from "node:os";

// Descobre o IP da máquina na rede local automaticamente (Wi-Fi ou Ethernet)
function getNetworkIp() {
  const interfaces = os.networkInterfaces();
  for (const name of Object.keys(interfaces)) {
    for (const iface of interfaces[name]) {
      if (iface.family === "IPv4" && !iface.internal) {
        return iface.address;
      }
    }
  }
  return "127.0.0.1";
}

export default defineConfig({
  plugins: [react()],
  define: {
    // Cria uma variável global no React com o IP verdadeiro do computador
    __IP_AUTOMATICO__: JSON.stringify(getNetworkIp()),
  },
  server: {
    proxy: {
      // Redireciona todas as chamadas /api para o Back-end Spring Boot
      "/api": {
        target: "http://localhost:8080",
        changeOrigin: true,
        secure: false,
      },
    },
  },
});
