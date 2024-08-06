import { MoveUp, MoveUpRight, MoveRight, MoveDownRight, MoveDown } from 'lucide-react'; // Substitua por sua biblioteca de ícones

const iconComponents = [
  <MoveUp size={40} />,
  <MoveUpRight size={40} />,
  <MoveRight size={40} />,
  <MoveDownRight size={40} />,
  <MoveDown size={40} />
];

const values = [1, 2, 3, 4, 4]; // Array de valores

export const IconTrend = () => {
  return (
    <div>
      {values.map((value, index) => (
        <div key={index}>
          {iconComponents[value - 1]}
        </div>
      ))}
    </div>
  );
};
