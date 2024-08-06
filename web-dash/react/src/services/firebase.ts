import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyAVpDSYVBYGwMHvcrNsOGXo7vN6HAhMybs",
  authDomain: "glucose-now.firebaseapp.com",
  projectId: "glucose-now",
  storageBucket: "glucose-now.appspot.com",
  messagingSenderId: "604400946634",
  appId: "1:604400946634:web:7ec0c32477895c90871333"
};

const app = initializeApp(firebaseConfig);
export const auth = getAuth(app);
export default app;
export const database = getDatabase(app);