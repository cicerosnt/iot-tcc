import { ArrowRight, InfoIcon, Link2, X } from "lucide-react";
import { Button } from "../../components/button";
import { FormEvent } from "react";
import { api } from "../../lib/axios";
import { useParams } from "react-router-dom";
import { toast } from "react-toastify";

interface CreateLinkModalProps{
  openLinkModal: () => void
}

export function CreateLinksModal({openLinkModal}: CreateLinkModalProps) {

  const {tripId} = useParams()
  
  function createLink(event: FormEvent<HTMLFormElement>){
    event?.preventDefault()
    
    const data = new FormData(event.currentTarget)
    const title = data.get('title')?.toString()
    const url = data.get('url')?.toString()
    
    if(!title || !url){
      toast.warn("Precisa informar os dados para o formulário!")
      return
    }
    
    const link = api.post(`/trips/${tripId}/links`, {
      title,
      url
    })
    
    if(link != null && link != undefined){
      toast.success("Link cadastrado com sucesso!")
      openLinkModal()
      window.document.location.reload()
    }
  }

  return (
    <div className="fixed inset-0 bg-black/60 flex items-center justify-center">
      <div className="w-[640px] rounded-xl py-5 px-6 shadow-shape bg-zinc-900 space-y-5">
        <div className="space-y-2">
          <div className="flex justify-between items-center">
            <h1 className="text-lg font-semibold">Cadastro de Lins</h1>
            <button type="button" onClick={openLinkModal}>
              <X className="size-6 bg-lime-500 text-zinc-950 rounded" />
            </button>
          </div>
          <p className="text-sm text-zinc-400">
            Todos os links fica disponiveis para os convidados.
          </p>
        </div>

        <form onSubmit={createLink} className="flex flex-col gap-4">
          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <InfoIcon className="text-zinc-400 size-5" />
            <input
              type="text"
              name="title"
              id="title"
              placeholder="Sobre o link"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Link2 className="text-zinc-400 size-5" />
            <input
              type="url"
              name="url"
              id="url"
              placeholder="Url"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>
          
          <Button variant='primary' size="full">
            Salvar link
            <ArrowRight />
          </Button>
        </form>
      </div>
    </div>
  );
}
