import { useState } from 'react';
import { CreateActivityModal } from './create-activity-modal';
import { ImportantLinks } from './important-links';
import { Activities } from './activites';
import { DestinationAndDateHeader } from './destination-and-date-header';
import { Plus } from 'lucide-react';
import { Guests } from './gusts';
import { CreateLinksModal } from './create-link-modal';
import { Button } from '../../components/button';

export function TripDetailsPage() {
  const [isCreateActivityModalOpen, setIsCreateActivityModalOpen] =
    useState(false);
  const [isCreateLinkModalOpen, setIsCreateLinkModalOpen] = useState(false);

  function openActiveModal() {
    if (isCreateActivityModalOpen) {
      setIsCreateActivityModalOpen(false);
    } else {
      setIsCreateActivityModalOpen(true);
    }
  }

  function openLinkModal() {
    if (isCreateLinkModalOpen) {
      setIsCreateLinkModalOpen(false);
    } else {
      setIsCreateLinkModalOpen(true);
    }
  }

  return (
    <div className="max-w-6xl px-6 py-10 mx-auto space-y-8">
      <DestinationAndDateHeader />

      <main className="flex gap-16 px-4">
        <div className="flex-1 space-y-6">
          <div className="flex items-center justify-between">
            <h2 className="text-3xl font-semibold">Atividades</h2>
            <Button variant="primary" size="default" onClick={openActiveModal}>
              <Plus />
              Atividade
            </Button>
          </div>

          <Activities />
        </div>
        <div className="w-80 space-y-6">
          <ImportantLinks openLinkModal={openLinkModal} />

          <div className="w-full h-px bg-zinc-800"></div>

          <Guests />
        </div>
      </main>

      {isCreateActivityModalOpen && (
        <CreateActivityModal openActiveModal={openActiveModal} />
      )}

      {isCreateLinkModalOpen && (
        <CreateLinksModal openLinkModal={openLinkModal} />
      )}
    </div>
  );
}
