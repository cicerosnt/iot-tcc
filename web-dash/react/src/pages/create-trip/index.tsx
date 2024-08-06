import { FormEvent, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { InviteGustsModal } from './invite-guests-modal';
import { ConfirmTripModal } from './confirm-trip-modal';
import { toast } from 'react-toastify';
import { DestinationAndDateSteps } from './steps/destination-and-dare-step';
import { InviteGustsStep } from './steps/invite-guests-step';
import { DateRange } from 'react-day-picker';
import { api } from '../../lib/axios';

export function CreateTripPage() {
  const navigate = useNavigate();
  const [isGuestsInputOpen, setIsGuestInputOpen] = useState(false);
  const [isGuestsModalOpen, setIsGuestModalOpen] = useState(false);
  const [isConfirmTripeModalOpen, setIsConfirmTripeModalOpen] = useState(false);
 
  const [destination, setDestination] = useState('')
  const [ownerName, setOwnerName] = useState('')
  const [ownerEmail, setOwnerEmail] = useState('')
  const [eventStartAndDates, setEventStartAndDates] = useState<
    DateRange | undefined
  >()
  
  const [emailsToInvite, setEmailsToInvite] = useState([
    'cicerosnt@mail.com',
    'mail@teste2.com',
  ]);

  function openGuestsInput() {
    setIsGuestInputOpen(true);
  }

  function closeGuestsInput() {
    setIsGuestInputOpen(false);
  }

  function openModalGuests() {
    if (!isGuestsModalOpen) {
      setIsGuestModalOpen(true);
      return;
    }
    setIsGuestModalOpen(false);
  }

  function addEmailToInvite(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    const data = new FormData(event.currentTarget);
    const email = data.get('email')?.toString();

    if (!email || emailsToInvite.includes(email)) {
      toast.warn('E-mail inválido ou já cadastrado!')
      return;
    }

    setEmailsToInvite([...emailsToInvite, email]);

    event.currentTarget.reset();
    
    toast.success('E-mail cadastrado com sucesso!')
  }

  function removeEmailToInvite(emailToRemove: string) {
    const newEmailsList = emailsToInvite.filter(
      (email) => email !== emailToRemove
    );
    setEmailsToInvite(newEmailsList);
  }

  function openConfirmTripModal() {
    if (!isConfirmTripeModalOpen) {
      setIsConfirmTripeModalOpen(true);
      return;
    }
    setIsConfirmTripeModalOpen(false);
  }
  
  async function createTrip(event: FormEvent<HTMLFormElement>) {
    event.preventDefault();
    console.log(destination)
    console.log(ownerName)
    console.log(ownerEmail)
    console.log(eventStartAndDates)
    console.log(emailsToInvite)
    
    if(!destination && !ownerEmail && !ownerName && !eventStartAndDates?.from && !eventStartAndDates?.to && !emailsToInvite){
      toast.warn('Verifique os dados infromados, são obrigatórios!')
      return
    }
    
    const response = await api.post('/trips', {
      destination,
      starts_at: eventStartAndDates?.from,
      ends_at: eventStartAndDates?.to,
      emails_to_invite: emailsToInvite,
      owner_name: ownerName,
      owner_email:ownerEmail,
    })
    
    const {tripId} = response.data
    
    if(tripId != undefined || tripId != null || tripId != '') {
      toast.warn('Viagem criada com sucesso!')
      navigate(`/trips/${tripId}`);
    }
  }
  
  return (
    <div className="h-screen flex items-center justify-center bg-pattern bg-no-repeat bg-center">
      <div className="max-w-3xl w-full px-6 text-center space-y-10">
        <div className="flex flex-col items-center gap-3">
          <img src="/logo.svg" />

          <p className="text-zinc-300">
            Convide seus amigos e planeje sua próxima viagem!
          </p>
        </div>

        <div className="space-y-0">
          
          <DestinationAndDateSteps
            isGuestsInputOpen={isGuestsInputOpen}
            closeGuestsInput={closeGuestsInput}
            openGuestsInput={openGuestsInput}
            setDestination={setDestination}
            setEventStartAndDates={setEventStartAndDates}
            eventStartAndDates={eventStartAndDates}
          />

          {isGuestsInputOpen && (
            <InviteGustsStep
              openConfirmTripModal={openConfirmTripModal}
              emailsToInvite={emailsToInvite}
              openModalGuests={openModalGuests}
            />
          )}
        </div>

        <p className="text-sm text-zinc-500">
          Ao planejar sua viagem pela plann.er você automaticamente concorda{' '}
          <br /> com nossos{' '}
          <a href="#" className="text-zinc-300 underline">
            termos de uso
          </a>{' '}
          e{' '}
          <a href="#" className="text-zinc-300 underline">
            políticas de privacidade
          </a>
          .
        </p>
      </div>

      {isGuestsModalOpen && (
        <InviteGustsModal
          addEmailToInvite={(addEmailToInvite)}
          emailsToInvite={emailsToInvite}
          openModalGuests={(openModalGuests)}
          removeEmailToInvite={removeEmailToInvite}
        />
      )}

      {isConfirmTripeModalOpen && (
        <ConfirmTripModal
        openConfirmTripModal={(openConfirmTripModal)}
        createTrip={createTrip}   
        setOwnerName={setOwnerName}
        setOwnerEmail={setOwnerEmail}
        />
      )}
    </div>
  );
}
