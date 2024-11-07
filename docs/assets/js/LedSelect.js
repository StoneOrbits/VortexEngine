let currentLed = 0; // Start from the "all LEDs selected" state
const leds = document.querySelectorAll('.led');
let intervalId = null; // For controlling the cycling interval
const statusText = document.getElementById('status-text'); // Get the status text element

function updateLedSelection() {
    console.log("Updating LED selection for currentLed =", currentLed); // Diagnostic log
    if (currentLed === 0) {
      clearCycling();
      leds.forEach(led => {
        led.style.opacity = '1'; // Full opacity
      });
      statusText.innerHTML = "<strong>Selection:</strong> All LEDs";
    } else if (currentLed === 1) {
      startCycling();
      statusText.innerHTML = "<strong>Selection:</strong> MultiLed";
    } else if (currentLed === 2) {
      clearCycling();
      leds.forEach((led, index) => {
        led.style.opacity = index % 2 === 0 ? '1' : '0.3';
      });
      statusText.innerHTML = "<strong>Selection:</strong> Group Odds";
    } else if (currentLed === 3) {
      clearCycling();
      leds.forEach((led, index) => {
        led.style.opacity = index % 2 !== 0 ? '1' : '0.3';
      });
      statusText.innerHTML = "<strong>Selection:</strong> Group Evens";
    } else {
      clearCycling();
      leds.forEach((led, index) => {
        led.style.opacity = index === currentLed - 4 ? '1' : '0.3';
      });
      statusText.innerHTML = `<strong>Selection:</strong> LED ${currentLed - 3}`; // Adjusting text to start from 1
    }
  }
  

function startCycling() {
  if (intervalId !== null) return; // Avoid multiple intervals
  let cycleIndex = 0;
  intervalId = setInterval(() => {
    leds.forEach((led, index) => {
      led.style.backgroundColor = index === cycleIndex ? '#9600ff' : '#555555';
      led.style.opacity = index === cycleIndex ? '1' : '0.3';
    });
    cycleIndex = (cycleIndex + 1) % leds.length;
  }, 100); // Cycle every 100ms
}

function clearCycling() {
  if (intervalId !== null) {
    clearInterval(intervalId);
    intervalId = null;
  }
}

document.getElementById('next-led-button').addEventListener('click', () => {
  currentLed = (currentLed + 1) % (leds.length + 4); // Include all LEDs, cycling mode, even, odd, and individual LEDs
  updateLedSelection();
});

updateLedSelection(); // Initial update to set all LEDs
