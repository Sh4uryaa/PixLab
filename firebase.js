import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getAuth } from "firebase/auth";

const firebaseConfig = {
    apiKey: "AIzaSyAHoYqeAHQKh30LSR-PYv8XBKd5rp-toko",
    authDomain: "pixlab-a564f.firebaseapp.com",
    projectId: "pixlab-a564f",
    storageBucket: "pixlab-a564f.firebasestorage.app",
    messagingSenderId: "663621421416",
    appId: "1:663621421416:web:9e32ede62a22363736eedd",
    measurementId: "G-H9BTB2VEHN"
};

const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
export const auth = getAuth(app);