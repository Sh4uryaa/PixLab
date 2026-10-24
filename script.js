
document.querySelectorAll(".comparison-container").forEach((container) => {
    const images = container.querySelectorAll("img");
    const slider = container.querySelector('input[type="range"]');
    const line = container.querySelector(".slider-line");

    if (images.length < 2 || !slider || !line) return;

    const originalImage = images[0];
    const resultImage = images[1];

    function updateSlider() {
        const position = Number(slider.value);

        resultImage.style.clipPath =
            `inset(0 0 0 ${position}%)`;

        line.style.left = `${position}%`;
    }

    slider.addEventListener("input", updateSlider);

    updateSlider();
});



const filterOptions = document.getElementById("filterOptions");
const originalExample = document.getElementById("filterExampleImg");
const filteredExample = document.getElementById("filterExampleResultImg");

const filterExamples = {
    grayscale: "images/exampleImageGrayScale.png",
    sepia: "images/exampleImageSepia.png",
    blur: "images/exampleImageBlur.png",
    reflection: "images/exampleImageReflection.png"
};

filterOptions.addEventListener("change", function () {
    const selectedFilter = filterOptions.value;
    const newImage = filterExamples[selectedFilter];

    if (newImage) {
        filteredExample.src = newImage;
    }
});
