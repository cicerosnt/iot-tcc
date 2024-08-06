import { ArrowRight, UserRoundPlus } from 'lucide-react';
import { Button } from '../../../components/button';

interface InviteGustsStepProps {
  openModalGuests: () => void;
  emailsToInvite: string[];
  openConfirmTripModal: () => void;
}

export function InviteGustsStep({
  openModalGuests,
  emailsToInvite,
  openConfirmTripModal,
}: InviteGustsStepProps) {
  return (
    <div className="bg-zinc-900 p-4 rounded-xl flex items-center justify-between shadow-shape gap-3">
      <Button variant="secondary" size="default" onClick={openModalGuests}>
        <UserRoundPlus className="size-5 text-zinc-400" />
        <span className="text-zinc-400 text-lg flex-1 text-left">
          {emailsToInvite.length > 0
            ? `${emailsToInvite.length} pessoas convidadas`
            : `Quem estará na viagem`}
        </span>
      </Button>
      
      <div className='w-px h-6 bg-zinc-800' />

      <Button variant="primary" size="default" onClick={openConfirmTripModal}>
        Confirmar viagem
        <ArrowRight />
      </Button>
    </div>
  );
}
