import { ArrowRight, Mail, User, X } from 'lucide-react';
import { FormEvent } from 'react';
import { Button } from '../../components/button';

interface ConfirmTripModalProps {
  openConfirmTripModal: () => void;
  createTrip: (event: FormEvent<HTMLFormElement>) => void;
  setOwnerName: (ownerName: string) => void
  setOwnerEmail: (ownerEmail: string) => void
}

export function ConfirmTripModal({
  openConfirmTripModal,
  createTrip,
  setOwnerName,
  setOwnerEmail
}: ConfirmTripModalProps) {
  return (
    <div className="fixed inset-0 bg-black/60 flex items-center justify-center">
      <div className="w-[640px] rounded-xl py-5 px-6 shadow-shape bg-zinc-900 space-y-5">
        <div className="space-y-2">
          <div className="flex justify-between items-center">
            <h1 className="text-lg font-semibold">
              Confirmar criação de viagem
            </h1>
            <button type="button" onClick={openConfirmTripModal}>
              <X className="size-6 bg-lime-500 text-zinc-950 rounded" />
            </button>
          </div>
          <p className="text-sm text-zinc-400">
            Para concluir a criação da viagem para{' '}
            <span className="font-bold">Noronha</span>, Brasil nas datas
            de <span className="font-bold">16 a 27 de Agosto de 2024</span>{' '}
            preencha seus dados abaixo:
          </p>
        </div>

        <form onSubmit={createTrip} className="flex flex-col gap-4">
          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <User className="text-zinc-400 size-5" />
            <input
              onChange={event => setOwnerName(event.target.value)}
              type="text"
              name="ownerName"
              id="ownerName"
              placeholder="Seu nome"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Mail className="text-zinc-400 size-5" />
            <input
              onChange={event => setOwnerEmail(event.target.value)}
              type="email"
              name="ownerEmail"
              id="ownerEmail"
              placeholder="Seu melhor e-mail"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <Button variant="primary" size="full" type="submit">
            Confirmar viagem
            <ArrowRight />
          </Button>
        </form>
      </div>
    </div>
  );
}
