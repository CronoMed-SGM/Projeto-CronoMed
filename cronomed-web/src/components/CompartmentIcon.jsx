import { compartmentStyle } from '../lib/compartments.js';

function patternOverlay(pattern, id) {
  const stroke = 'rgba(255,255,255,0.55)';
  switch (pattern) {
    case 'diagonal':
      return (
        <pattern id={id} width="8" height="8" patternUnits="userSpaceOnUse" patternTransform="rotate(45)">
          <line x1="0" y1="0" x2="0" y2="8" stroke={stroke} strokeWidth="3" />
        </pattern>
      );
    case 'dots':
      return (
        <pattern id={id} width="10" height="10" patternUnits="userSpaceOnUse">
          <circle cx="5" cy="5" r="2" fill={stroke} />
        </pattern>
      );
    case 'horizontal':
      return (
        <pattern id={id} width="8" height="8" patternUnits="userSpaceOnUse">
          <line x1="0" y1="4" x2="8" y2="4" stroke="rgba(26,26,26,0.28)" strokeWidth="2.5" />
        </pattern>
      );
    case 'grid':
      return (
        <pattern id={id} width="10" height="10" patternUnits="userSpaceOnUse">
          <path d="M 10 0 L 0 0 0 10" fill="none" stroke={stroke} strokeWidth="2" />
        </pattern>
      );
    case 'vertical':
      return (
        <pattern id={id} width="8" height="8" patternUnits="userSpaceOnUse">
          <line x1="4" y1="0" x2="4" y2="8" stroke={stroke} strokeWidth="2.5" />
        </pattern>
      );
    case 'zigzag':
      return (
        <pattern id={id} width="12" height="8" patternUnits="userSpaceOnUse">
          <path d="M0 6 L6 2 L12 6" fill="none" stroke={stroke} strokeWidth="2.2" />
        </pattern>
      );
    default:
      return null;
  }
}

export default function CompartmentIcon({ index, size = 40, label = true }) {
  const s = compartmentStyle(index);
  const patId = `cmp-pat-${index}`;
  return (
    <svg
      className="cmp"
      width={size}
      height={size}
      viewBox="0 0 48 48"
      role="img"
      aria-label={label ? `Compartimento ${index}: ${s.name}` : undefined}
      aria-hidden={label ? undefined : 'true'}
    >
      <defs>{patternOverlay(s.pattern, patId)}</defs>
      <rect x="3" y="3" width="42" height="42" rx="10" fill={s.color} stroke="#1A1A1A" strokeWidth="2.4" />
      <rect x="3" y="3" width="42" height="42" rx="10" fill={`url(#${patId})`} />
      <circle cx="24" cy="24" r="12" fill="#FFFFFF" stroke="#1A1A1A" strokeWidth="1.8" />
      <text x="24" y="29" textAnchor="middle" fontSize="16" fontWeight="800" fill="#1A1A1A" fontFamily="Roboto, Arial, sans-serif">
        {index}
      </text>
    </svg>
  );
}