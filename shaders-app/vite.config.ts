import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig(({ command }) => {
  const isDev = command === 'serve';

  return {
    plugins: [
      react({
        babel: {
          plugins: [['babel-plugin-react-compiler']],
        },
      }),
    ],
    // Lógica condicional:
    // - En Dev: usa '/' (localhost:5173 normal)
    // - En Build: usa '/wasm-image-processing-engine/' (para GitHub Pages)
    base: isDev ? '/' : '/wasm-image-processing-engine/',
    server: {
      host: true
    }
  }
})