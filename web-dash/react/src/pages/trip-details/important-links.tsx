import { Link2, Plus } from 'lucide-react';
import { Button } from '../../components/button';
import { useParams } from 'react-router-dom';
import { useEffect, useState } from 'react';
import { api } from '../../lib/axios';

interface Links {
  id: string;
  title: string;
  url: string;
}

interface CreateLinkModalProps{
  openLinkModal: () => void
}

export function ImportantLinks({openLinkModal}: CreateLinkModalProps) {
  const { tripId } = useParams();
  const [links, setLinks] = useState<Links[]>([]);

  useEffect(() => {
    api
      .get(`/trips/${tripId}/links`)
      .then((response) => setLinks(response.data.links));
  }, [tripId]);

  return (
    <div className="space-y-6">
      <h2 className="font-serif text-3xl">Links Importantes</h2>
      <div className="space-y-5">
        {links.length > 0 ? (
          <div>
            {links.map((link) => {
            return (
              <div
                key={link.id}
                className="flex items-center justify-between gap-4 bg-slate-500 space-y-3 py-3"
              >
                <div className="space-1.5 flex-1">
                  <span className="block font-medium text-zinc-100">
                    {link.title}
                  </span>
                  <a
                    href=""
                    className="block text-zinc-400 truncate hover:text-lime-300"
                  >
                    {link.url}
                  </a>
                </div>
                <Link2 className="text-zinc-400 size-5 shrink-0 " />
                {links.length < 0 &&(
                  <p>nenhum link encontrado</p>
                )}
              </div>
            );
          })}
          </div>
        ):(
          <div>
            <p>nenhum link cadastrado!</p>
          </div>
        )}
      </div>

      <Button onClick={openLinkModal} variant="primary" size="full">
        <Plus />
        Novo
      </Button>
    </div>
  );
}
