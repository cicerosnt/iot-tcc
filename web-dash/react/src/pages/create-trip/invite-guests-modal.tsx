import { AtSign, MailPlus, X } from 'lucide-react';
import { FormEvent } from 'react';
import { Button } from '../../components/button';

interface InviteGustsModalProps {
  openModalGuests: () => void;
  emailsToInvite: string[];
  addEmailToInvite: (event: FormEvent<HTMLFormElement>) => void;
  removeEmailToInvite: (email: string) => void;
}

export function InviteGustsModal({
  openModalGuests,
  emailsToInvite,
  addEmailToInvite,
  removeEmailToInvite,
}: InviteGustsModalProps) {
  return (
    <div className="fixed inset-0 bg-black/60 flex items-center justify-center">
      <div className="w-[640px] rounded-xl py-5 px-6 shadow-shape bg-zinc-900 space-y-5">
        <div className="space-y-2">
          <div className="flex justify-between items-center">
            <h1 className="text-lg font-semibold">Selecionar convidados</h1>
            <button type="button" onClick={openModalGuests}>
              <X className="size-6 bg-lime-500 text-zinc-950 rounded" />
            </button>
          </div>
          <p className="text-sm text-zinc-400">
            Os convidados irão receber e-mails para confirmar a participação na
            viagem.
          </p>
        </div>

        <div className="flex flex-wrap gap-2">
          {emailsToInvite.map((email) => {
            return (
              <div
                key={email}
                className="py-1.5 px-1.5 rounded-md bg-zinc-800 flex items-center gap-2"
              >
                <span className="text-zinc-300 flex justify-between items-center gap-2">
                  {email}
                  <X
                    onClick={() => removeEmailToInvite(email.toString())}
                    className="cursor-pointer size-5 text-lime-400"
                  />
                </span>
              </div>
            );
          })}
        </div>

        <div className="bg-zinc-800 h-px w-full"></div>

        <form
          onSubmit={addEmailToInvite}
          className="p-2.5 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center"
        >
          <AtSign className="text-zinc-400 size-5" />
          <input
            type="email"
            name="email"
            id="email"
            placeholder="Digite o e-mail do convidado"
            className="bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
          />

          <Button variant="primary" size="default" type="submit">
            <MailPlus />
            Convidar
          </Button>
        </form>
      </div>
    </div>
  );
}
