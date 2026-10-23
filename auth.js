import { auth } from "./firebase.js";
import {
    signInWithEmailAndPassword,
    createUserWithEmailAndPassword,
    sendPasswordResetEmail,
    GoogleAuthProvider,
    signInWithPopup,
    onAuthStateChanged,
    signOut
} from "firebase/auth";

let isSignupMode = false;



const loginButton = document.getElementById("login-button");
const loginModal = document.getElementById("login-modal");
const closeLogin = document.getElementById("close-login");
const galleryLogin = document.getElementById("gallery-login");
const loginForm = document.getElementById("login-form");
const loginError = document.getElementById("login-error");
const galleryLoginMessage = document.querySelector(
    ".gallery-login-message"
);
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

const googleLogin = document.getElementById("google-login");
const googleProvider = new GoogleAuthProvider();

loginButton.addEventListener("click", function () {
    loginModal.style.display = "flex";
});

closeLogin.addEventListener("click", function () {
    loginModal.style.display = "none";
});

if (galleryLogin) {
    galleryLogin.addEventListener("click", function () {
        if (!auth.currentUser) {
            loginModal.style.display = "flex";
        }
    });
}


eyeOpen.style.display = "block";
eyeClosed.style.display = "none";

loginForm.addEventListener("submit", async function (event) {
    event.preventDefault();

    const email = document.getElementById("login-email").value.trim();
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

        loginModal.style.display = "none";
        loginError.textContent = "";

    } catch (error) {
    console.error("Firebase Auth Error:", error.code, error.message);

    switch (error.code) {
        case "auth/invalid-credential":
        case "auth/wrong-password":
            loginError.textContent = "Incorrect email or password.";
            break;

        case "auth/user-not-found":
            loginError.textContent = "No account found with this email.";
            break;

        case "auth/invalid-email":
            loginError.textContent = "Please enter a valid email address.";
            break;

        case "auth/too-many-requests":
            loginError.textContent =
                "Too many failed attempts. Please try again later.";
            break;

        case "auth/network-request-failed":
            loginError.textContent =
                "Network error. Please check your internet connection.";
            break;

        case "auth/email-already-in-use":
            loginError.textContent =
                "An account with this email already exists. Please log in.";
            break;

        case "auth/weak-password":
            loginError.textContent =
                "Your password is too weak. Please choose a stronger password.";
            break;

        default:
            loginError.textContent =
                "Something went wrong. Please try again.";
    }
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

let googleLoginInProgress = false;

googleLogin.addEventListener("click", async function () {
    if (googleLoginInProgress) return;

    googleLoginInProgress = true;
    googleLogin.disabled = true;
    loginError.textContent = "";

    try {
        await signInWithPopup(auth, googleProvider);

        loginModal.style.display = "none";
        loginError.textContent = "";
    } catch (error) {
        console.error(
            "Google authentication failed:",
            error.code,
            error.message
        );

        if (error.code === "auth/popup-closed-by-user") {
            loginError.textContent = "Sign-in cancelled. Try again.";
        } else if (error.code === "auth/cancelled-popup-request") {
            loginError.textContent =
                "A sign-in request was cancelled. Please try again once.";
        } else {
            loginError.textContent =
                "Google sign-in failed. Please try again.";
        }
    } finally {
        googleLoginInProgress = false;
        googleLogin.disabled = false;
    }
});

const forgotPasswordLinks = document.querySelectorAll(".forgot-password");

forgotPasswordLinks.forEach(link => {
    link.addEventListener("click", async function () {
        const email = document.getElementById("login-email").value.trim();

        if (!email) {
            loginError.textContent = "Enter your email address first.";
            return;
        }

        try {
            await sendPasswordResetEmail(auth, email);

            loginError.textContent =
                "If an account exists for this email, a reset link will be sent. Check your inbox.";

        } catch (error) {
            console.error("Password reset error:", error);

            if (error.code === "auth/invalid-email") {
                loginError.textContent = "Please enter a valid email address.";
            } else if (error.code === "auth/network-request-failed") {
                loginError.textContent =
                    "Network error. Check your connection and try again.";
            } else {
                loginError.textContent =
                    "Unable to send the reset email. Please try again.";
            }
        }
    });
});     

const accountButton = document.getElementById("account-button");
const accountMenu = document.getElementById("account-menu");
const accountEmail = document.getElementById("account-email");
const logoutButton = document.getElementById("logout-button");

accountButton.addEventListener("click", function () {
    const isOpen = accountMenu.style.display === "block";
    accountMenu.style.display = isOpen ? "none" : "block";
});

onAuthStateChanged(auth, (user) => {
    if (user) {
        loginButton.style.display = "none";
        accountButton.style.display = "block";
        accountEmail.textContent = user.email || "Signed in with Google";
    } else {
        loginButton.style.display = "flex";
        accountButton.style.display = "none";
        accountMenu.style.display = "none";
    }
    document.querySelector(".account-container")
    ?.classList.add("auth-ready");
    if (galleryLoginMessage) {
    galleryLoginMessage.style.display = user ? "none" : "block";
}
});

logoutButton.addEventListener("click", async function () {
    try {
        await signOut(auth);
        accountMenu.style.display = "none";
    } catch (error) {
        console.error("Logout failed:", error);
    }
});

