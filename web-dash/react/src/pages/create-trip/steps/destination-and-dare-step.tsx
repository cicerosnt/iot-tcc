import { ArrowRight, Calendar, MapPin, Settings2, X } from 'lucide-react';
import { Button } from '../../../components/button';
import { useState } from 'react';
import { DateRange, DayPicker } from 'react-day-picker';
import { format } from 'date-fns';

import 'react-day-picker/dist/style.css';

interface DestinationAndDateStepsProps {
  isGuestsInputOpen: boolean;
  closeGuestsInput: () => void;
  eventStartAndDates: DateRange | undefined;
  openGuestsInput: () => void;
  setDestination: (destination: string) => void;
  setEventStartAndDates: (dates: DateRange | undefined) => void;
}

export function DestinationAndDateSteps({
  isGuestsInputOpen,
  closeGuestsInput,
  openGuestsInput,
  setDestination,
  setEventStartAndDates,
  eventStartAndDates,
}: DestinationAndDateStepsProps) {
  const [isDatePickerOpen, setIsDatePickerOpen] = useState(false);

  function openAndCloseDatePicker() {
    if (isDatePickerOpen) {
      setIsDatePickerOpen(false);
    } else {
      setIsDatePickerOpen(true);
    }
  }

  const displayedDate = eventStartAndDates && eventStartAndDates.from && eventStartAndDates.to 
  ? format(eventStartAndDates.from, "d' de 'LLL").concat(' até ').concat(format(eventStartAndDates.to, "d' de 'LLL"))
  : null

  return (
    <div className="h-16 bg-zinc-900 px-4 rounded-xl flex items-center shadow-shape gap-3">
      <div className="flex items-center gap-2 flex-1">
        <MapPin className="size-5 text-zinc-400" />
        <input
          onChange={(event) => setDestination(event.target.value)}
          disabled={isGuestsInputOpen}
          type="text"
          placeholder="Pra onde tu vai?"
          className="bg-transparent text-lg placeholder-zinc-400 outline-none flex-1 "
        />
      </div>

      <button
        disabled={isGuestsInputOpen}
        onClick={openAndCloseDatePicker}
        className="flex items-center gap-2 outline-none bg-transparent text-left flex-1"
      >
        <Calendar className="size-5 text-zinc-400" />
        <span className=" text-lg text-zinc-400 w-40 flex-1">
          {displayedDate || `Quando?`}
        </span>
      </button>

      {isDatePickerOpen && (
        <div className="fixed inset-0 bg-black/60 flex items-center justify-center">
          <div className="rounded-xl py-5 px-6 shadow-shape bg-zinc-900 space-y-5">
            <div className="space-y-2">
              <div className="flex justify-between items-center">
                <h1 className="text-lg font-semibold">Selecione a data</h1>
                <button type="button" onClick={openAndCloseDatePicker}>
                  <X className="size-6 bg-lime-500 text-zinc-950 rounded" />
                </button>
              </div>
            </div>

            <DayPicker
              mode="range"
              selected={eventStartAndDates}
              onSelect={setEventStartAndDates}
            />
          </div>
        </div>
      )}

      <div className="w-px h-6 bg-zinc-800"></div>

      {isGuestsInputOpen ? (
        <Button variant="primary" size="default" onClick={closeGuestsInput}>
          Alterar local/data
          <Settings2 />
        </Button>
      ) : (
        <Button variant="primary" size="default" onClick={openGuestsInput}>
          Continuar
          <ArrowRight />
        </Button>
      )}
    </div>
  );
}
