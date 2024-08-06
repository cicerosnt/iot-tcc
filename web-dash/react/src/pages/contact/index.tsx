import { LogOut } from "lucide-react";
import { Button } from "../../components/button";
import { Link, useNavigate } from 'react-router-dom';
import { auth } from "../../services/firebase";
import { useEffect, useState } from "react";
import { toast } from "react-toastify";
import { UserInfo } from "firebase/auth";

export function Contact(){
  const navigate = useNavigate();
  const [userFirebase, setUserFirebase] = useState<UserInfo>();
  
  useEffect(() => {
    auth.onAuthStateChanged((user) => {
      if (!user) {
        toast.warning('Sem permissão para acesso a pagina');
        navigate('/');
        auth.signOut();
        return;
      }
      setUserFirebase(user);
    });
  }, []);

  function handleLogout() {
    if (userFirebase) {
      auth.signOut();
    }
  }
  
  function handleNavigateContact(){
    navigate('/contato')
  }
  return (
  
    <>
    <header className="w-full py-4 h-24">
      <div className="max-w-4xl mx-auto flex justify-between items-center">
        <Link className="font-bold text-lg" to={'/'}>Glico-Agora!</Link>
        <div className="flex justify-between items-center gap-5">
        <Button onClick={handleNavigateContact} size="default" variant="secondary">
            Contato
          </Button>
          <Button onClick={handleLogout} size="default" variant="primary">
            <LogOut />
            Sair
          </Button>
        </div>
      </div>
    </header>

    <main className="max-w-7xl mx-auto flex items-center" style={{ height: 'calc(100vh - 96px)' }}>
      <div className="w-full flex flex-col gap-6">
        <div className="space-y-4 text-center">
          <h1 className="text-6xl font-extrabold">Relatório</h1>
          <p className="text-3xl font-bold">Acompanhamento em tempo real.</p>
        </div>
        
    <div className="w-full max-w-md">
      <div className="space-y-1 text-center">
        <h1 className="text-2xl">Fale conosco</h1>
        <p>Preencha o formulário abaixo para entrar em contato.</p>
      </div>
      <div className="space-y-4">
        <div className="grid grid-cols-2 gap-4">
          <div className="space-y-2">
            <label htmlFor="name">Nome</label>
            <input id="name" placeholder="Seu nome" required />
          </div>
          <div className="space-y-2">
            <label htmlFor="email">Email</label>
            <input id="email" type="email" placeholder="seu@email.com" required />
          </div>
        </div>
        <div className="space-y-2">
          <label htmlFor="subject">Assunto</label>
          <select id="subject" defaultValue="contato">
              <option value="Selecione o assunto" />
              <option value="contato">Contato</option>
              <option value="problema">Reportar problema</option>
          </select>
        </div>
        <div className="space-y-2">
          <label htmlFor="message">Mensagem</label>
          <textarea id="message" placeholder="Digite sua mensagem" required />
        </div>
      </div>
        <Button variant="primary" size="default">Enviar</Button>
    </div>
    
    </div>
    </main>
 
  
    
    </>
  )
}