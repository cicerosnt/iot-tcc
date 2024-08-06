import { ComponentProps, ReactNode } from 'react';
import { tv, VariantProps } from 'tailwind-variants';

interface ButtonProps extends ComponentProps<'button'>, VariantProps<typeof buttonVariants>{
  children: ReactNode;
}

const buttonVariants = tv({
  base: 'h-14 rounded-lg px-5 py-2 font-medium flex items-center justify-center gap-2',

  variants: {
    variant: {
      primary: ' bg-lime-300 text-lime-950 hover:bg-lime-400',
      secondary: 'bg-zinc-800 text-zinc-200 hover:bg-zinc-700',
    },
    
    size: {
      default: 'max-w-sm',
      full: 'w-full'
    }
  },
  defaultVariants: {
    variant: 'secondary',
    size: 'default'
  },
});

export function Button({ children, variant, size, ...props }: ButtonProps) {
  return (
    <button {...props} className={buttonVariants({variant, size})}>
      {children}
    </button>
  );
}
