# Music-Sync-LED
This project features an architectural model of the UTE campus . The project uses two-unit Arduino controllers . One Arduino microcontroller is dedicated to managing the LED strips and rings placed around the model to create dynamic lighting effects.Simultaneously the second Arduino  controls a LED Matrix display, which renders song lyrics in real-time. Both Arduinos are connected to a  physical button.The  button acts as a precise timing trigger.Once pressed at the exact moment, all LED light transitions and lyric displays will be perfectly in sync with the song's rhythm."
* **State Machine Architecture:** Employs a  state machine to manage system modes and lighting phases, enabling non-blocking multitasking across input handling and visual rendering.
* **Hardware-Level Controller Synchronization:** Connects both independent microcontrollers to a single physical button, allowing simultaneous trigger activation without requiring complex inter-chip communication.
* **Lighting Effects:** Generates all dynamic lighting effects, some  transitions, and color-wave movements are created through real-time mathematical calculations and trigonometric functions.
<div style="display: flex; gap: 10px; width: 100%;">
  
  <div style="flex: 1; aspect-ratio: 4 / 3; overflow: hidden; position: relative;">
    <img src="https://github.com/user-attachments/assets/e6dd40d3-e604-42d6-9709-94c6842faa39" alt="mohinhled" style="position: absolute; width: 100%; height: 100%; object-fit: cover;" />
  </div>

  <div style="flex: 1; aspect-ratio: 4 / 3; overflow: hidden; position: relative;">
    <img src="https://github.com/user-attachments/assets/303a6347-e3f9-4b52-a31c-ac76ceb4c5dc" alt="mohinhled2" style="position: absolute; width: 100%; height: 100%; object-fit: cover;" />
  </div>

  <div style="flex: 1; aspect-ratio: 4 / 3; overflow: hidden; position: relative;">
    <img src="https://github.com/user-attachments/assets/ef9a0441-6524-466f-8a13-a14f6bd9aa6b" alt="mohinhled3" style="position: absolute; width: 100%; height: 100%; object-fit: cover;" />
  </div>

</div>
