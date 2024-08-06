import { ArrowRight, Calendar, Tag, X } from "lucide-react";
import { Button } from "../../components/button";
import { FormEvent } from "react";
import { api } from "../../lib/axios";
import { useParams } from "react-router-dom";
import { toast } from "react-toastify";

interface CreateActivityModalProps{
  openActiveModal: () => void
}

export function CreateActivityModal({openActiveModal}: CreateActivityModalProps) {

  const {tripId} = useParams()
  
  function createActivity(event: FormEvent<HTMLFormElement>){
    event?.preventDefault()
    
    const data = new FormData(event.currentTarget)
    const title = data.get('title')?.toString()
    const occurs_at = data.get('occurs_at')?.toString()
    
    if(!title || !occurs_at){
      toast.warn("Precisa informar os dados para o formulário!")
      return
    }
    
    const activity = api.post(`/trips/${tripId}/activities`, {
      title,
      occurs_at
    })
    
    if(activity != null && activity != undefined){
      toast.success("Sua atividade foi cadastrada com suesso!")
      openActiveModal()
      window.document.location.reload()
    }
  }

  return (
    <div className="fixed inset-0 bg-black/60 flex items-center justify-center">
      <div className="w-[640px] rounded-xl py-5 px-6 shadow-shape bg-zinc-900 space-y-5">
        <div className="space-y-2">
          <div className="flex justify-between items-center">
            <h1 className="text-lg font-semibold">Cadastro de atividades</h1>
            <button type="button" onClick={openActiveModal}>
              <X className="size-6 bg-lime-500 text-zinc-950 rounded" />
            </button>
          </div>
          <p className="text-sm text-zinc-400">
            Todos os convidados pode ver as atividades.
          </p>
        </div>

        <form onSubmit={createActivity} className="flex flex-col gap-4">
          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Tag className="text-zinc-400 size-5" />
            <input
              type="text"
              name="title"
              id="title"
              placeholder="Qual a atividade?"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Calendar className="text-zinc-400 size-5" />
            <input
              type="datetime-local"
              name="occurs_at"
              id="occurs_at"
              placeholder="Data e horário"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>
          
          <Button variant='primary' size="full">
            Salvar atividade
            <ArrowRight />
          </Button>
        </form>
      </div>
    </div>
  );
}
