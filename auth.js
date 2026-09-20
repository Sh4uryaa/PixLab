import { auth } from "./firebase.js";
import {
    signInWithEmailAndPassword,
    createUserWithEmailAndPassword
} from "firebase/auth";

let isSignupMode = false;

const loginButton = document.getElementById("login-button");
const loginModal = document.getElementById("login-modal");
const closeLogin = document.getElementById("close-login");
const galleryLogin = document.getElementById("gallery-login");

loginButton.addEventListener("click", function () {
    loginModal.style.display = "flex";
});

closeLogin.addEventListener("click", function () {
    loginModal.style.display = "none";
});

if (galleryLogin) {
    galleryLogin.addEventListener("click", function () {
        loginModal.style.display = "flex";
    });
}

const loginForm = document.getElementById("login-form");
const loginError = document.getElementById("login-error");

const signupQuestion = document.getElementById("signup-question");
const loginQuestion = document.getElementById("login-question");
const loginLink = document.getElementById("login-link");
const signupLink = document.getElementById("signup-link");

const loginTitle = document.getElementById("login-title");
const loginSubmit = document.getElementById("login-submit");

const passwordInput = document.getElementById("login-password");
const togglePassword = document.getElementById("toggle-password");
const eyeClosed = document.getElementById("eye-closed");
const eyeOpen = document.getElementById("eye-open");

loginForm.addEventListener("submit", async function (event) {
    event.preventDefault();

    const email = document.getElementById("login-email").value;
    const password = document.getElementById("login-password").value;

    try {
        let userCredential;

        if (isSignupMode) {
            userCredential = await createUserWithEmailAndPassword(
                auth,
                email,
                password
            );
        } else {
            userCredential = await signInWithEmailAndPassword(
                auth,
                email,
                password
            );
        }

        console.log("Success:", userCredential.user);

    } catch (error) {
        loginError.textContent = "Invalid email or password.";
        console.error("Authentication failed:", error);
    }
});

signupLink.addEventListener("click", function () {
    isSignupMode = true;

    loginTitle.textContent = "Create your PixLab account";
    loginSubmit.textContent = "Sign Up";

    signupQuestion.style.display = "none";
    signupLink.style.display = "none";

    loginQuestion.style.display = "inline";
    loginLink.style.display = "inline";
});

loginLink.addEventListener("click", function () {
    isSignupMode = false;

    loginTitle.textContent = "Sign in to PixLab";
    loginSubmit.textContent = "Login";

    loginQuestion.style.display = "none";
    loginLink.style.display = "none";

    signupQuestion.style.display = "inline";
    signupLink.style.display = "inline";
});

togglePassword.addEventListener("click", function () {

    if (passwordInput.type === "password") {
        passwordInput.type = "text";

        eyeOpen.style.display = "none";
        eyeClosed.style.display = "block";

        togglePassword.setAttribute("aria-label", "Hide password");

    } else {
        passwordInput.type = "password";

        eyeOpen.style.display = "block";
        eyeClosed.style.display = "none";

        togglePassword.setAttribute("aria-label", "Show password");
    }
});     