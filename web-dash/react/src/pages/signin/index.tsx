import { ArrowRight, Lock, Mail } from "lucide-react";
import { Button } from "../../components/button";
import { useEffect, useState } from "react";
import { toast } from "react-toastify";

import { auth } from "../../services/firebase";
import { signInWithEmailAndPassword, UserInfo } from 'firebase/auth';
import { useNavigate } from "react-router-dom";



export function Login() {

  const [email, setEmail] = useState<string>('');
  const [password, setPassword] = useState<string>('');
  //const [userFirebaseGoogle, setUserFirebaseGoogle] = useState<User>({} as User);
  const navigate = useNavigate()
  // const [userFirebase, setUserFirebase] = useState<UserInfo>();
  
  useEffect(() => {
    auth.onAuthStateChanged((user) => {
      if (!user) {
        toast.warning('Sem permissão para acesso a pagina');
        navigate('/');
        auth.signOut();
        return;
      }
      navigate('/dash')
    });
  }, []);
  
  async function loginWithMailPassword(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();    
    
    if(email == '' || password == ''){
      toast.warn('Dados obrigatorio não informado ou inválidos!')
      return
    }
    
    try {
      await signInWithEmailAndPassword(auth, email!, password!);
      toast.success("Login efetuado com sucesso!")
      navigate('/dash')
    } catch (error) {
      toast.warn("Error: " + error)
      console.error(error)
    }
    

    
    
    // return
    
    // const response = await api.post('/trips', {
    //   destination,
    //   starts_at: eventStartAndDates?.from,
    //   ends_at: eventStartAndDates?.to,
    //   emails_to_invite: emailsToInvite,
    //   owner_name: ownerName,
    //   owner_email:ownerEmail,
    // })
    
    // const {tripId} = response.data
    
    // if(tripId != undefined || tripId != null || tripId != '') {
    //   toast.warn('Viagem criada com sucesso!')
    //   navigate(`/trips/${tripId}`);
    // }
  }
  
  // async function loginWighGoogle() {
  //   const provider = new GoogleAuthProvider()
  //   signInWithPopup(auth, provider)
  //   .then((result) => {
  //     setUserFirebaseGoogle(result.user)
  //     console.log(result);
  //   }).catch((err) => {
  //     console.log(err);
  //   })
  // }
  
  return (
    <main className="flex justify-center items-center h-[100vh]">
      <div className="max-w-sm flex flex-col gap-4">
      <div className="space-y-1 text-center">
        <h1 className="text-2xl">Login</h1>
        <p>Entre com seu e-mail e senha para acessar sua conta.</p>
      </div>
      
      
      <form onSubmit={loginWithMailPassword} className="flex flex-col gap-4">
          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Mail className="text-zinc-400 size-5" />
            <input
              value={email} 
              onChange={(e) => setEmail(e.target.value)} 
              type="email"
              placeholder="E-mail"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <div className="py-2.5 px-4 bg-zinc-950 border border-zinc-800 rounded-lg flex gap-2 items-center">
            <Lock className="text-zinc-400 size-5" />
            <input
              value={password} 
              onChange={(e) => setPassword(e.target.value)} 
              type="password"
              placeholder="Senha"
              className="h-10 bg-transparent text-lg placeholder-zinc-400 outline-none flex-1"
            />
          </div>

          <Button variant="primary" size="full" type="submit">
            Logar
            <ArrowRight />
          </Button>
        </form>
        
        {/* <Button onClick={loginWighGoogle} variant="secondary" size="full" type="submit">
            Entrar com Google
          </Button> */}

      </div>
    </main>
  )
}