import React, { useEffect, useState } from 'react';
import { auth } from '../../services/firebase';
import { toast } from 'react-toastify';
import { Link, useNavigate } from 'react-router-dom';
import { Button } from '../../components/button';
import {
  Bug,
  CloudOff,
  Contact,
  LogOut,
  MoveDown,
  MoveDownRight,
  MoveRight,
  MoveUp,
  MoveUpRight,
} from 'lucide-react';

import { getDatabase, ref, onValue, DataSnapshot } from 'firebase/database';
import { getColorBasedOnDextro } from '../../utils/setStateColor';
import { UserInfo } from 'firebase/auth';

interface User {
  id: string;
  name: string;
  email: string;
}

interface GlucoseData {
  id: string | undefined;
  time: string | undefined;
  dextro: number;
  trend: number;
  history: {
    id: string | undefined;
    time: string | undefined;
    dextro: number;
    trend: number;
  };
}

export function Dash() {
  const navigate = useNavigate();
  const [userFirebase, setUserFirebase] = useState<UserInfo>();
  const [glucose, setGlucose] = useState<GlucoseData[]>([]);
  const [infoDextro, setInfoDextro] = useState('');

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

  useEffect(() => {
    // Função para ler dados em tempo real
    const readRealtimeData = () => {
      const db = getDatabase();
      const usersRef = ref(db, 'glucose');

      // Configurar um listener para mudanças em tempo real
      const unsubscribe = onValue(
        usersRef,
        (snapshot: DataSnapshot) => {
          if (snapshot.exists()) {
            const data = snapshot.val();
            const dataGlucose: GlucoseData[] = Object.keys(data).map((key) => ({
              id: key,
              ...data[key],
            }));
            setGlucose(dataGlucose);
            console.log(glucose);
          } else {
            console.log('Nenhum dado disponível');
            setGlucose([]);
          }
        },
        (error) => {
          console.error('Erro ao ler dados:', error);
        }
      );

      // Retornar uma função de limpeza para remover o listener quando o componente for desmontado
      return () => unsubscribe();
    };

    // Chamar a função para começar a ler os dados
    const unsubscribe = readRealtimeData();

    // Função de limpeza
    return () => {
      unsubscribe();
    };
  }, []); // Array de dependências vazio significa que este efeito só será executado uma vez, na montagem do componente

  function handleLogout() {
    if (userFirebase) {
      auth.signOut();
    }
  }
  
  function handleNavigateContact(){
    navigate('/contato')
  }

  const iconComponents = [
    '',
    <MoveUp size={40} className="font-bold" />,
    <MoveUpRight size={50} className="font-bold" />,
    <MoveRight size={40} className="font-bold" />,
    <MoveDownRight size={40} className="font-bold" />,
    <MoveDown size={40} className="font-bold" />,
  ];

  const iconHistory = [
    '',
    <MoveUp size={20} />,
    <MoveUpRight size={20} />,
    <MoveRight size={20} />,
    <MoveDownRight size={20} />,
    <MoveDown size={20} />,
  ];

  return (
    <>
      <header className="w-full py-4 h-24 max-[1115px]:px-4 ">
        <div className="max-w-4xl mx-auto flex justify-between items-center">
        <Link className="font-bold text-lg" to={'/'}>Glico-Agora!</Link>
          <div className="flex justify-between items-center gap-5">
          <Button onClick={handleNavigateContact} size="default" variant="secondary">
              <Bug />
              Reportar
            </Button>
            <Button onClick={handleLogout} size="default" variant="primary">
              <LogOut />
              Sair
            </Button>
          </div>
        </div>
      </header>

      <main className="w-screen mx-auto flex items-center max-[1115px]:px-4 " style={{ height: 'calc(100vh - 96px)' }}>
        <div className="w-full flex flex-col gap-6">
          {/* <div className="space-y-4 text-center">
            <h1 className="text-6xl font-medium max-[1115px]:text-4xl">Relatório</h1>
            <p className="text-3xl font-bold">Acompanhamento em tempo real.</p>
          </div> */}

          <div className="max-[590px]:w-full max-[1115px]:flex-col max-[1115px]:justify-center max-[1115px]:mx-auto flex justify-evenly flex-row gap-4">
            {glucose.length > 0 ? (
              <>
                {glucose.map((data) => (
                  <React.Fragment key={data.id}>
                    <div className="flex gap-8 flex-row max-[590px]:justify-around items-center p-5 border border-lime-100 rounded-lg">
                      <div>
                        {data.history.map((h) => (
                          <div className="flex flex-row justify-between items-center gap-4 border-b border-lime-100 pb-2 mb-2 last:border-none">
                            {/* <div className="flex items-center gap-2"> */}
                              <strong className="text-2xl max-[590px]:text-sm">{h.dextro}</strong>
                              {iconHistory[h.trend]}
                            {/* </div> */}
                            <span className="text-2xl max-[590px]:text-sm">{h.time}</span>
                          </div>
                        ))}
                      </div>

                      <div className="border-l-8 pl-6 border-lime-400 max-[590px]:pl-3">
                        <h1 className="font-bold text-6xl max-[590px]:text-3xl">
                          {data.id.charAt(0).toUpperCase() + data.id?.slice(1)}
                        </h1>
                        <p className="text-4xl max-[590px]:text-2xl">{data.time}</p>
                        <div className="flex gap-4">
                          <strong
                            className={`font-bold text-9xl max-[590px]:text-6xl ${getColorBasedOnDextro(
                              data.dextro, data.trend
                            )}`}
                          >
                            {data.dextro}
                          </strong>
                          {iconComponents[data.trend]}
                        </div>
                      </div>
                    </div>

                    {/* <div className="h-auto w-px bg-lime-100"></div> */}
                  </React.Fragment>
                ))}
              </>
            ) : (
              <div className="flex flex-col justify-center items-center gap-5">
                <CloudOff size={100} />
                <p>Nenhum registro de dextro encontrado.</p>
              </div>
            )}
          </div>
        </div>
      </main>
    </>
  );
}
