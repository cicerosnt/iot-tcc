import { CheckCircle2, CircleDashed, UserCog } from 'lucide-react';
import { Button } from '../../components/button';
import { useParams } from 'react-router-dom';
import { useEffect, useState } from 'react';
import { api } from '../../lib/axios';

interface Participants {
  id: string;
  name: string | null;
  email: string;
  is_confirmed: boolean;
}

export function Guests() {
  const { tripId } = useParams();
  const [participants, setParticipants] = useState<Participants[]>([]);

  useEffect(() => {
    api
      .get(`/trips/${tripId}/participants`)
      .then((response) => setParticipants(response.data.participants));
  }, [tripId]);

  return (
    <div className="space-y-6">
      <h2 className="font-serif text-xl">Convidados</h2>
      <div className="space-y-5">
        {participants.map((participant, index) => {
          return (
            <div key={participant.id} className="flex items-center justify-between gap-4">
            <div className="space-1.5 flex-1">
              <span className="block font-medium text-zinc-100">
                {participant.name ?? `Convidado #${index}` }
              </span>
              <span className="block text-zinc-400 truncate hover:text-lime-300">
              {participant.email}
              </span>
            </div>
            {participant.is_confirmed ? (
              <CircleDashed className="text-zinc-400 size-5 shrink-0 " />
            ) : (
              <CheckCircle2 className="text-lime-400 size-5 shrink-0 " />
            )}
          </div>
          )
        })}
      </div>

      <Button variant="secondary" size="full">
        <UserCog />
        Gerenciar convidados
      </Button>
    </div>
  );
}
