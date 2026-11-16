// Identidade visual oficial (Cap. 4 - RF13, RF16, RF17): cor + padrao grafico + numero (sem formas geometricas).
export const COMPARTMENTS = {
  1: { pattern: 'diagonal',   color: '#DD4245', ink: '#1A1A1A', name: 'Vermelho com listras diagonais' },
  2: { pattern: 'dots',       color: '#56AF62', ink: '#1A1A1A', name: 'Verde com pontos' },
  3: { pattern: 'horizontal', color: '#F6D331', ink: '#1A1A1A', name: 'Amarelo com listras horizontais' },
  4: { pattern: 'grid',       color: '#9E5CA8', ink: '#1A1A1A', name: 'Roxo com xadrez' },
  5: { pattern: 'vertical',   color: '#5B9BD5', ink: '#1A1A1A', name: 'Azul com listras verticais' },
  6: { pattern: 'zigzag',     color: '#EE8843', ink: '#1A1A1A', name: 'Laranja com zigue-zague' },
};

export function compartmentStyle(index) {
  return COMPARTMENTS[index] || COMPARTMENTS[1];
}