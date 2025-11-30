import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    react({
      babel: {
        plugins: [['babel-plugin-react-compiler']],
      },
    }),
  ],
  resolve: {
    alias: {
      fs: '/src/mocks/fs.js',
      path: '/src/mocks/fs.js',
      crypto: '/src/mocks/fs.js',
    },
  },
  build: {
    rollupOptions: {
      external: ['fs', 'path', 'crypto'],
    },
  },
})
