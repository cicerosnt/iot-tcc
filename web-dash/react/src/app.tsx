import { createBrowserRouter, RouterProvider } from 'react-router-dom';

import { ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
import { Login } from './pages/signin';
import { Dash } from './pages/dash';
import { Contact } from './pages/contact';

const router = createBrowserRouter([
  {
    path: '/',
    element: <Login />,
    // element: <CreateTripPage />,
  },
  {
    path: '/dash',
    element: <Dash />,
  },
  {
    path: '/contato',
    element: <Contact />,
  },
]);

export function App() {
  return (
    <div>
      <RouterProvider router={router} />
      <ToastContainer
        autoClose={3000}
        position="top-right"
        hideProgressBar={false}
        newestOnTop={false}
        closeOnClick
        rtl={false}
        pauseOnFocusLoss
        draggable
        pauseOnHover
        theme="dark"
      />
    </div>
  );
}
