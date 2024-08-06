// utils/setStateColor.ts

// Define the type for the return value
type Color = 'text-red-600' | 'text-blue-600' | 'text-yellow-600';

// Function to determine the color based on the dextro value
export function getColorBasedOnDextro(dextroValue: number, trendValue: number): Color {
  if (dextroValue < 71 || dextroValue > 180) {
    return 'text-red-600';
  } else if (dextroValue >= 71 && dextroValue <= 180 && trendValue >= 3) {
    return 'text-blue-600';
  } else if (dextroValue > 170 && dextroValue && trendValue < 3) {
    return 'text-yellow-600';
  }
}
